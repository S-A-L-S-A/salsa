/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2008 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "gas/laralga.h"
#include "core/mutation.h"
#include "core/genome.h"
#include "evaluations/multitrials.h"
#include "factory.h"
#include "configurationhelper.h"
#include <QThreadPool>
#include <QtConcurrentMap>
using namespace QtConcurrent;

namespace farsa {

LaralGA::LaralGA()
	: GeneticAlgo() {
	fitfunc = 0;
	muta = 0;
	currGenotype = 0;
	numGens = 0;
	nreproducing = 0;
	noffspring = 0;
	nelitism = 0;
	elitismEnabled = 0;
	currPhase = initEvaluation;
	numThreadv = 1;
	isInitialized = false;
	isFinalized = true;
}

LaralGA::~LaralGA() {
	foreach( LaralGA::evaluationThread* e, evalThreads ) {
		delete e;
	}
	delete fitfunc;
}

void LaralGA::configure( ConfigurationParameters& params, QString prefix ) {
	setGenome( params.getObjectFromGroup<Genome>( prefix + QString( "GENOME" ) ) );
	setFitnessFunction( params.getObjectFromGroup<MultiTrials>( prefix + QString( "EVALUATION" ) ) );
	fitfunc->setGenome( genome() );
	setMutation( params.getObjectFromGroup<Mutation>( prefix + QString("REPRODUCTION/MUTATION") ) );
	setNumGenerations( ConfigurationHelper::getInt( params, prefix + QString( "ngenerations" ), 1000 ) );
	const QString reproductionGroup( prefix + QString( "REPRODUCTION" ) + ConfigurationParameters::GroupSeparator() );
	int nreproduce = params.getValue( reproductionGroup + QString( "nreproducing" ) ).toInt();
	bool useElitism = ! (params.getValue( reproductionGroup + QString( "elitism" ) ).compare( "true", Qt::CaseInsensitive));
	int nelitism = params.getValue( reproductionGroup + QString( "nelited" ) ).toInt();
	setReproduceParams( nreproduce, useElitism, nelitism );
	setNumThreads( ConfigurationHelper::getInt( params, prefix + QString( "numThreads" ), 1 ) );
}

void LaralGA::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "LaralGA" );
	params.createParameter( prefix, QString("numThreads"), QString("%1").arg(numThreads()) );
	params.createParameter( prefix, QString("ngenerations"), QString("%1").arg( numGenerations() ) );
	//--- EVALUATION
	fitnessFunction()->save( params, params.createSubGroup( prefix, "EVALUATION" ) );
	//--- REPRODUCTION
	QString reproductionGroup = params.createSubGroup( prefix, "REPRODUCTION" );
	params.createParameter( reproductionGroup, "nreproducing", QString("%1").arg( numReproducing() ) );
	params.createParameter( reproductionGroup, "elitism", isElitismEnabled() ? "true" : "false" );
	params.createParameter( reproductionGroup, "nelited", QString("%1").arg( numElitism() ) );
	mutation()->save( params, params.createSubGroup( reproductionGroup, "MUTATION" ) );
	//--- GENOME
	genome()->save( params, params.createSubGroup( prefix, "GENOME" ) );
}

void LaralGA::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A simple Genetic Algorithm used at LARAL laboratory", "This Genetic Algorithm runs more than one replica of the evolutionary process each of them use a reproduction schema without crossover and with a deterministic rank selection" );
	d.describeInt( "numThreads" ).limits( 1, 32 ).def(1).help( "Number of threads to parallelize the evaluation of individuals" ).runtime(&LaralGA::setNumThreads, &LaralGA::numThreads);;
	d.describeInt( "ngenerations" ).limits( 1, INT_MAX ).props( IsMandatory ).help( "Number of generations per evolutionary process" );
	d.describeSubgroup( "EVALUATION" ).type( "MultiTrials" ).props( IsMandatory ).help( "Object that calculate the fitness", "Create a subclass of MultiTrials and code your custom fitness function" );
	SubgroupDescriptor r = d.describeSubgroup( "REPRODUCTION" ).props( IsMandatory ).help( "Paramenters that affect the reproduction process" );
		r.describeInt( "nreproducing" ).limits( 1, INT_MAX ).def(10).props( IsMandatory ).help( "Number of the most fitted individual to reproduce" );
		r.describeBool( "elitism" ).def( false ).help( "Enable the elitism", "When true, the nelited most fitted individual will be copied exactly (without mutation) to the new generations" );
		r.describeInt( "nelited" ).limits( 1, INT_MAX ).def( 10 ).help( "Number of individual to copy exactly to the new generation" );
		r.describeSubgroup( "MUTATION" ).type( "Mutation" ).props( IsMandatory ).help( "The type of mutation to use" );
	d.describeSubgroup( "GENOME" ).type( "Genome" ).props( IsMandatory ).help( "The genome containing the individual under evolution" );
}

void LaralGA::setNumThreads( int numThreads ) {
	if ( numThreads < 1 ) {
		qWarning( "The number of Threads must be greater than one!!" );
	}
	Q_ASSERT_X( !isInitialized && isFinalized ,
			"LaralGA::setNumThreads",
			"This method can only called before initialize of LaralGA" );
	Q_ASSERT_X( fitfunc != 0 ,
			"LaralGA::setNumThreads",
			"This method must be called after a MultiTrials object has been setted by LaralGA::setFitnessFunction" );
	numThreadv = numThreads;
	return;
}

int LaralGA::numThreads() const {
	return numThreadv;
}

void LaralGA::setFitnessFunction( MultiTrials* fitfunc ) {
	this->fitfunc = fitfunc;
	this->fitfunc->setGA( this );
}

Evaluation* LaralGA::evaluationPrototype()
{
	return fitfunc;
}

QVector<Evaluation*> LaralGA::evaluationPool() {
	QVector<Evaluation*> ev;
	foreach( LaralGA::evaluationThread* e, evalThreads ) {
		ev.append( e->eval );
	}
	return ev;
}

MultiTrials* LaralGA::fitnessFunction() {
	return fitfunc;
}

void LaralGA::setMutation( Mutation* mutate ) {
	this->muta = mutate;
	this->muta->setGA( this );
}

Mutation* LaralGA::mutation() {
	return muta;
}

void LaralGA::setReproduceParams( int nreproducing, bool useElitism, int nelitism ) {
	this->nreproducing = nreproducing;
	elitismEnabled = useElitism;
	this->nelitism = nelitism;
	noffspring = genomev->size()/nreproducing;
	//--- check if parameters are setted correctly
	Q_ASSERT_X( noffspring * nreproducing == (int)(genomev->size()),
				"LaralGA::setReproduceParams",
				QString( "nreproducing must be divisible by genome dimension: %1 %2 %3" ).arg(noffspring).arg(nreproducing).arg(genomev->size()).toLatin1().data() );
	if ( useElitism ) {
		Q_ASSERT_X( useElitism && ( nelitism <= nreproducing ),
				"LaralGA::setReproduceParams",
				"The number of elited genotype must be less or equal to selected ones" );
	}
}

int LaralGA::numReproducing() {
	return nreproducing;
}

int LaralGA::numOffspring() {
	return noffspring;
}

int LaralGA::numElitism() {
	return nelitism;
}

bool LaralGA::isElitismEnabled() {
	return elitismEnabled;
}

void LaralGA::initialize() {
	if ( isInitialized && !isFinalized ) return;
	Q_ASSERT_X( fitfunc != 0 ,
			"LaralGA::initialize",
			"You have to setup the FitnessFunction of LaralGA" );
	Q_ASSERT_X( muta !=0 ,
			"LaralGA::initialize",
			"You have to setup the Mutate operator of LaralGA" );

	isInitialized = true;
	isFinalized = false;
	currGenotype = 0;
	setGeneration( 0 );
	currPhase = initEvaluation;
	evolutionEnd = false;
	evaluationDone = false;
	//--- try to recover data
	getIODelegate()->recoverData( this );
	//--- Setting up the evalThreads
	for( int i=0; i<evalThreads.size(); i++ ) {
		delete (evalThreads[i]);
	}
	evalThreads.clear();
	for( int i=0; i<numThreadv; i++ ) {
		evalThreads.append( new evaluationThread( this, fitfunc ) );
		evalThreads.last()->eval->initGeneration( generation() );
	}
	//--- set the number of thread to create
	QThreadPool::globalInstance()->setMaxThreadCount( numThreadv );
}

void LaralGA::gaStep() {
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
			QFuture<void> future = map( evalThreads, LaralGA::runStepWrapper );
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
		// save the data
		getIODelegate()->saveData( this );
		if ( generation() < numGens ) {
			currPhase = nextGeneration_pass2;
		} else {
			currPhase = endEvolution;
		}
		break;
	case nextGeneration_pass2:
		//--- this additional pass is for avoid to modify the
		//--- genotypes contained in the last generation of the evolution
		//--- and to allow to save the genotypes after each generation
		setGeneration( generation()+1 );
		evaluationDone = false;
		createNextGeneration();
		for( int i=0; i<numThreadv; i++ ) {
			evalThreads[i]->eval->initGeneration( generation() );
		}
		currPhase = initEvaluation;
		break;
	case endEvolution:
		finalize();
		break;
	default:
		qFatal( "Default switch in LaralGA::gaStep" );
		break;
	}
}

void LaralGA::skipEvaluation() {
	// Set evaluation done, and check which phase to go to
	evaluationDone = true;
	if ( generation() < numGens ) {
		currPhase = nextGeneration_pass2;
	} else {
		currPhase = endEvolution;
	}
}

void LaralGA::finalize() {
	if ( isFinalized && !isInitialized ) return;

	isInitialized = false;
	isFinalized = true;
	evolutionEnd = true;
}

void LaralGA::createNextGeneration() {
	Genome& genref = *(genome());
	muta->setGenome( genome() );
	for( int i=0; i<nreproducing; i++ ) {
		//--- skip for now the first nreproducing position
		for( int j=1; j<noffspring; j++ ) {
			//--- id of the current genome to replace
			int id = i + j*nreproducing;
			//--- clone and mutate the i-th selected individual
			genref[id]->assign( genref[i] );
			muta->mutate( genref[id] );
		}
	}
	//--- also mutate the selected, excepted for the elited ones
	int startId = 0;
	if ( elitismEnabled ) {
		startId = nelitism;
	}
	for( int i=startId; i<nreproducing; i++ ) {
		muta->mutate( genref[i] );
	}
}

LaralGA::evaluationThread::evaluationThread( LaralGA* p, MultiTrials* eProto )
	: parent(p), id(0), blocked(false) {
	eval = eProto->clone();
	eval->setGenome( p->genome() );
	eval->setGA( p );
}

LaralGA::evaluationThread::~evaluationThread() {
	delete eval;
	sequence.clear();
}

void LaralGA::evaluationThread::runStep() {
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

void LaralGA::runStepWrapper( LaralGA::evaluationThread* e ) {
	e->runStep();
}

} // end namespace farsa
