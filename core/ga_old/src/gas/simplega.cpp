/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
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
#include "gas/simplega.h"
#include "core/reproduction.h"
#include "core/genome.h"
#include "core/evaluation.h"
#include "configurationhelper.h"
#include <QThreadPool>
#include <QtConcurrentMap>
using namespace QtConcurrent;

namespace salsa {

SimpleGA::SimpleGA()
	: GeneticAlgo() {
	fitfunc = 0;
	reprod = 0;
	numGens = 0;
	currPhase = initEvaluation;
	numThreadv = 1;
	isInitialized = false;
	isFinalized = true;
}

SimpleGA::~SimpleGA() {
	foreach( SimpleGA::evaluationThread* e, evalThreads ) {
		delete e;
	}
	delete fitfunc;
}

void SimpleGA::configure( ConfigurationParameters& params, QString prefix ) {
	setGenome( params.getObjectFromGroup<Genome>( prefix + QString( "GENOME" ) ) );
	setEvaluation( params.getObjectFromGroup<Evaluation>( prefix + QString( "EVALUATION" ) ) );
	fitfunc->setGenome( genome() );
	setReproduction( params.getObjectFromGroup<Reproduction>( prefix + QString( "REPRODUCTION" ) ) );
	setNumGenerations( ConfigurationHelper::getInt( params, prefix + QString( "ngenerations" ), 1000 ) );
	setNumThreads( ConfigurationHelper::getInt( params, prefix + QString( "numThreads" ), 1 ) );
}

void SimpleGA::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "SimpleGA" );
	params.createParameter( prefix, QString("numThreads"), QString("%1").arg( numThreads() ) );
	params.createParameter( prefix, QString("ngenerations"), QString("%1").arg( numGenerations() ) );
	//--- EVALUATION
	fitfunc->save( params, params.createSubGroup( prefix, "EVALUATION" ) );
	//--- REPRODUCTION
	reproduction()->save( params, params.createSubGroup( prefix, "REPRODUCTION" ) );
	//--- GENOME
	genome()->save( params, params.createSubGroup( prefix, "GENOME" ) );
}

void SimpleGA::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Simple Genetic Algorithm" );
	d.describeInt( "numThreads" ).limits( 1, 32 ).def(1).help( "Number of threads to parallelize the evaluation of individuals" ).runtime(&SimpleGA::setNumThreads, &SimpleGA::numThreads);
	d.describeInt( "ngenerations" ).limits( 1, INT_MAX ).def( 1000 ).help( "Number of the generations of the evolutionary process" );
	d.describeSubgroup( "EVALUATION" ).type( "Evaluation" ).props( IsMandatory ).help( "Object that calculate the fitness", "Create a subclass of Evalution and code your custom fitness function" );
	d.describeSubgroup( "REPRODUCTION").type( "Reproduction" ).props( IsMandatory ).help( "Object that generate the new generations" );
	d.describeSubgroup( "GENOME" ).type( "Genome" ).props( IsMandatory ).help( "Object containing the individuals under evolution" );
}

void SimpleGA::setNumThreads( int numThreads ) {
	if ( numThreads < 1 ) {
		qWarning( "The number of Threads must be greater than one!!" );
	}
	Q_ASSERT_X( !isInitialized && isFinalized ,
			"SimpleGA::setNumThreads",
			"This method can only called before initialize of SimpleGA" );
	Q_ASSERT_X( fitfunc != 0 ,
			"SimpleGA::setNumThreads",
			"This method must be called after an Evaluation object has been setted by SimpleGA::setEvaluation" );
	numThreadv = numThreads;
	return;
}

int SimpleGA::numThreads() const {
	return numThreadv;
}

void SimpleGA::setEvaluation( Evaluation* fitfunc ) {
	this->fitfunc = fitfunc;
	this->fitfunc->setGA( this );
}

Evaluation* SimpleGA::evaluationPrototype()
{
	return fitfunc;
}

QVector<Evaluation*> SimpleGA::evaluationPool() {
	QVector<Evaluation*> ev;
	foreach( SimpleGA::evaluationThread* e, evalThreads ) {
		ev.append( e->eval );
	}
	return ev;
}

void SimpleGA::setReproduction( Reproduction* reprod ) {
	this->reprod = reprod;
	this->reprod->setGA( this );
}

Reproduction* SimpleGA::reproduction() {
	return reprod;
}

void SimpleGA::initialize() {
	if ( isInitialized && !isFinalized ) return;
	Q_ASSERT_X( fitfunc != 0 ,
			"SimpleGA::initialize",
			"You have to setup the Evaluation object of SimpleGA (Fitness Function)" );
	Q_ASSERT_X( reprod !=0 ,
			"SimpleGA::initialize",
			"You have to setup the Reproduction operator of SimpleGA" );

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
		evalThreads.append( new evaluationThread( this, fitfunc ) );
		evalThreads.last()->eval->initGeneration( 0 );
	}
	//--- set the number of thread to create
	QThreadPool::globalInstance()->setMaxThreadCount( numThreadv );
}

void SimpleGA::gaStep() {
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
			evalThreads[i]->eval->initialize( genome()->at( evalThreads[i]->id ) );
			evalThreads[i]->blocked = false;
		}
		currPhase = evaluating;
		break;
	case evaluating: { /* Multi Thread Block (i.e. Parallel Evaluation of Genotypes */
		nextGeneration = true;
		if ( numThreadv == 1 ) {
			// Don't use Threads if is not necessary
			evalThreads[0]->runStep();
		} else {
			QFuture<void> future = map( evalThreads, SimpleGA::runStepWrapper );
			future.waitForFinished();
		}
		if ( nextGeneration ) {
			currPhase = nextGeneration_pass1;
		}
		} /* End of Multi Thread Block */
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
		qFatal( "Default switch in SimpleGA::gaStep" );
		break;
	}
}

void SimpleGA::skipEvaluation() {
	// Set evaluation done, and check which phase to go to
	evaluationDone = true;
	if ( generation() < numGens ) {
		currPhase = nextGeneration_pass2;
	} else {
		currPhase = endEvolution;
	}
}

void SimpleGA::finalize() {
	if ( isFinalized && !isInitialized ) return;

	isInitialized = false;
	isFinalized = true;
	evolutionEnd = true;
}

SimpleGA::evaluationThread::evaluationThread( SimpleGA* p, Evaluation* eProto )
	: parent(p), id(0), blocked(false) {
	eval = eProto->clone();
	eval->setGenome( p->genome() );
	eval->setGA( p );
}

SimpleGA::evaluationThread::~evaluationThread() {
	delete eval;
	sequence.clear();
}

void SimpleGA::evaluationThread::runStep() {
	if ( blocked ) {
		return;
	}
	
	parent->nextGeneration = false;
	eval->evaluateStep();
	if ( eval->isEvaluationDone() ) {
		int nextIdSeq = idSeq + 1;
		eval->finalize();
		eval->genotype()->setRank( eval->genotype()->fitness() );
		if ( nextIdSeq >= sequence.size() ) {
			blocked = true;
			return;
		}
		idSeq = nextIdSeq;
		int nextId = sequence[ idSeq ];
		id = nextId;
		eval->initialize( parent->genome()->at( id ) );
	}
}

void SimpleGA::runStepWrapper( SimpleGA::evaluationThread* e ) {
	e->runStep();
}

} // end namespace salsa
