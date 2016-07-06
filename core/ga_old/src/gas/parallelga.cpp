/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2009 Gianluca Massera <emmegian@yahoo.it>                *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

#include "factory.h"
#include "gas/parallelga.h"
#include "core/reproduction.h"
#include "core/genome.h"
#include "core/evaluation.h"
#include "configurationhelper.h"
#include <QThreadPool>
#include <QtConcurrentMap>
using namespace QtConcurrent;

namespace farsa {

ParallelGA::ParallelGA()
	: GeneticAlgo() {
	fitfunc = 0;
	reprod = 0;
	numGens = 0;
	currPhase = initEvaluation;
	numThreadv = 1;
	isInitialized = false;
	isFinalized = true;
	future = new QFuture<void>();
}

ParallelGA::~ParallelGA() {
	foreach( ParallelGA::evaluationThread* e, evalThreads ) {
		delete e;
	}
	delete fitfunc;
}

void ParallelGA::configure( ConfigurationParameters& params, QString prefix ) {
	setGenome( params.getObjectFromGroup<Genome>( prefix + QString( "GENOME" ) ) );
	setEvaluation( params.getObjectFromGroup<Evaluation>( prefix + QString( "EVALUATION" ) ) );
	fitfunc->setGenome( genome() );
	setReproduction( params.getObjectFromGroup<Reproduction>( prefix + QString( "REPRODUCTION" ) ) );
	setNumGenerations( ConfigurationHelper::getInt( params, prefix + QString( "ngenerations" ), 1000 ) );
	setNumThreads( ConfigurationHelper::getInt( params, prefix + QString( "numThreads" ), 1 ) );
}

void ParallelGA::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "ParallelGA" );
	params.createParameter( prefix, QString("numThreads"), QString("%1").arg( numThreads() ) );
	params.createParameter( prefix, QString("ngenerations"), QString("%1").arg( numGenerations() ) );
	//--- EVALUATION
	fitfunc->save( params, params.createSubGroup( prefix, "EVALUATION" ) );
	//--- REPRODUCTION
	reproduction()->save( params, params.createSubGroup( prefix, "REPRODUCTION" ) );
	//--- GENOME
	genome()->save( params, params.createSubGroup( prefix, "GENOME" ) );
}

void ParallelGA::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Parallel Genetic Algorithm", "Respect to SimpleGA and others type of Genetic Algorithm, the implementation of the parallelization is more efficient" );
	d.describeInt( "numThreads" ).limits( 1, 32 ).def(1).help( "Number of threads to parallelize the evaluation of individuals" ).runtime(&ParallelGA::setNumThreads, &ParallelGA::numThreads);
	d.describeInt( "ngenerations" ).limits( 1, INT_MAX ).def( 1000 ).help( "Number of the generations of the evolutionary process" );
	d.describeSubgroup( "EVALUATION" ).type( "Evaluation" ).props( IsMandatory ).help( "Object that calculate the fitness", "Create a subclass of Evalution and code your custom fitness function" );
	d.describeSubgroup( "REPRODUCTION").type( "Reproduction" ).props( IsMandatory ).help( "Object that generate the new generations" );
	d.describeSubgroup( "GENOME" ).type( "Genome" ).props( IsMandatory ).help( "Object containing the individuals under evolution" );
}

void ParallelGA::setNumThreads( int numThreads ) {
	if ( numThreads < 1 ) {
		qWarning( "The number of Threads must be greater than one!!" );
	}
	Q_ASSERT_X( !isInitialized && isFinalized ,
			"ParallelGA::setNumThreads",
			"This method can only called before initialize of ParallelGA" );
	Q_ASSERT_X( fitfunc != 0 ,
			"ParallelGA::setNumThreads",
			"This method must be called after an Evaluation object has been setted by ParallelGA::setEvaluation" );
	numThreadv = numThreads;
	return;
}

int ParallelGA::numThreads() const {
	return numThreadv;
}

void ParallelGA::setEvaluation( Evaluation* fitfunc ) {
	this->fitfunc = fitfunc;
	this->fitfunc->setGA( this );
}

Evaluation* ParallelGA::evaluationPrototype()
{
	return fitfunc;
}

QVector<Evaluation*> ParallelGA::evaluationPool() {
	QVector<Evaluation*> ev;
	foreach( ParallelGA::evaluationThread* e, evalThreads ) {
		ev.append( e->eval );
	}
	return ev;
}

void ParallelGA::setReproduction( Reproduction* reprod ) {
	this->reprod = reprod;
	this->reprod->setGA( this );
}

Reproduction* ParallelGA::reproduction() {
	return reprod;
}

void ParallelGA::initialize() {
	if ( isInitialized && !isFinalized ) return;
	Q_ASSERT_X( fitfunc != 0 ,
			"ParallelGA::initialize",
			"You have to setup the Evaluation object of ParallelGA (Fitness Function)" );
	Q_ASSERT_X( reprod !=0 ,
			"ParallelGA::initialize",
			"You have to setup the Reproduction operator of ParallelGA" );

	isInitialized = true;
	isFinalized = false;
	setGeneration( 0 );
	currPhase = initEvaluation;
	evolutionEnd = false;
	evaluationDone = false;
	getIODelegate()->recoverData(this);
	//--- Setting up the evalThreads
	for( int i=0; i<evalThreads.size(); i++ ) {
		delete (evalThreads[i]);
	}
	evalThreads.clear();
	for( int i=0; i<numThreadv; i++ ) {
		evalThreads.append( new evaluationThread( fitfunc ) );
		evalThreads.last()->eval->initGeneration( generation() );
	}
	//--- set the number of thread to create
	QThreadPool::globalInstance()->setMaxThreadCount( numThreadv );
}

void ParallelGA::gaStep() {
	switch( currPhase ) {
	case initEvaluation:
		for( int i=0; i<numThreadv; i++ ) {
			evalThreads[i]->sequence.clear();
		}
		for( int i=0; i<(int)genome()->size(); i++ ) {
			evalThreads[ i%numThreadv ]->sequence.append( i );
		}
		for( int i=0; i<numThreadv; i++ ) {
			evalThreads[i]->idSeq = 0;
			evalThreads[i]->id = evalThreads[i]->sequence[ evalThreads[i]->idSeq ];
			evalThreads[i]->eval->setGenome( genome() );
		}
		currPhase = evaluating;
		//--- here also starts to run all sub-threads for evaluation
		//--- it evaluate all genotypes of the population in separate threads
		(*future) = map( evalThreads, ParallelGA::runStepWrapper );
		break;
	case evaluating: /* Multi Thread Block (i.e. Parallel Evaluation of Genotypes */
		//--- check if the evaluation has been completed
		if ( future->isFinished() ) {
			currPhase = nextGeneration_pass1;
		}
		break;
	case nextGeneration_pass1:
		qSort( genome()->begin(), genome()->end(), Genotype::rankGreaterThanComparator );
		updateStats();
		evaluationDone = true;
		for( int i=0; i<numThreadv; i++ ) {
			evalThreads[i]->eval->endGeneration( generation() );
		}
		notifyEndGeneration();
		getIODelegate()->saveData(this);
		if ( generation() < numGens ) {
			currPhase = nextGeneration_pass2;
		} else {
			currPhase = endEvolution;
		}
		break;
	case nextGeneration_pass2: {
		//--- this additional pass is for avoid to modify the
		//--- genotypes contained in the last generation of the evolution
		//--- and to allow to save the genotypes after each generation
		Genome* old = genome();
		setGenome( reprod->reproduction( old ) );
		delete old;
		fitfunc->setGenome( genome() );
		evaluationDone = false;
		setGeneration( generation()+1 );
		for( int i=0; i<numThreadv; i++ ) {
			evalThreads[i]->eval->initGeneration( generation() );
		}
		currPhase = initEvaluation;
		}
		break;
	case endEvolution:
		finalize();
		break;
	default:
		qFatal( "Default switch in ParallelGA::gaStep" );
		break;
	}
}

void ParallelGA::skipEvaluation() {
	// Set evaluation done, and check which phase to go to
	evaluationDone = true;
	if ( generation() < numGens ) {
		currPhase = nextGeneration_pass2;
	} else {
		currPhase = endEvolution;
	}
}

void ParallelGA::finalize() {
	if ( isFinalized && !isInitialized ) return;

	isInitialized = false;
	isFinalized = true;
	evolutionEnd = true;
}

ParallelGA::evaluationThread::evaluationThread( Evaluation* eProto )
	: id(0) {
	eval = eProto->clone();
	eval->setGenome( eProto->GA()->genome() );
	eval->setGA( eProto->GA() );
}

ParallelGA::evaluationThread::~evaluationThread() {
	delete eval;
	sequence.clear();
}

void ParallelGA::evaluationThread::runStep() {
	//--- it evaluate all individual assigned to this thread
	while( true ) {
		eval->initialize( eval->GA()->genome()->at( id ) );
		eval->evaluate();
		int nextIdSeq = idSeq + 1;
		eval->finalize();
		eval->genotype()->setRank( eval->genotype()->fitness() );
		if ( nextIdSeq >= sequence.size() ) {
			return;
		}
		idSeq = nextIdSeq;
		int nextId = sequence[ idSeq ];
		id = nextId;
	}
}

void ParallelGA::runStepWrapper( ParallelGA::evaluationThread* e ) {
	e->runStep();
}

} // end namespace farsa
