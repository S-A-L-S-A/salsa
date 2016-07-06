/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
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
#include "gas/nsga2.h"
#include "core/reproduction.h"
#include "core/genome.h"
#include "core/evaluation.h"
#include "configurationhelper.h"
#include <cmath>
#include <cfloat>
#include <QThreadPool>
#include <QtConcurrentMap>
using namespace QtConcurrent;

namespace farsa {

NSGA2::NSGA2()
	: GeneticAlgo(), lastPareto() {
	fitfunc = 0;
	reprod = 0;
	numGens = 0;
	currPhase = initEvaluation;
	numThreadv = 1;
	isInitialized = false;
	isFinalized = true;
}

NSGA2::~NSGA2() {
	foreach( NSGA2::evaluationThread* e, evalThreads ) {
		delete e;
	}
	delete fitfunc;
}

void NSGA2::configure( ConfigurationParameters& params, QString prefix ) {
	setGenome( params.getObjectFromGroup<Genome>( prefix + QString( "GENOME" ) ) );
	setEvaluation( params.getObjectFromGroup<Evaluation>( prefix + QString( "EVALUATION" ) ) );
	fitfunc->setGenome( genome() );
	setReproduction( params.getObjectFromGroup<Reproduction>( prefix + QString( "REPRODUCTION" ) ) );
	setNumGenerations( ConfigurationHelper::getInt( params, prefix + QString( "ngenerations" ), 1000 ) );
	setNumThreads( ConfigurationHelper::getInt( params, prefix + QString( "numThreads" ), 1 ) );
}

void NSGA2::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "NSGA2" );
	params.createParameter( prefix, QString("numThreads"), QString("%1").arg( numThreads() ) );
	params.createParameter( prefix, QString("ngenerations"), QString("%1").arg( numGenerations() ) );
	//--- EVALUATION
	fitfunc->save( params, params.createSubGroup( prefix, "EVALUATION" ) );
	//--- REPRODUCTION
	reproduction()->save( params, params.createSubGroup( prefix, "REPRODUCTION" ) );
	//--- GENOME
	genome()->save( params, params.createSubGroup( prefix, "GENOME" ) );
}

void NSGA2::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Non-dominated Sorting Genetic Algorithm version 2" );
	d.describeInt( "numThreads" ).limits( 1, 32 ).def(1).help( "Number of threads to parallelize the evaluation of individuals" ).runtime(&NSGA2::setNumThreads, &NSGA2::numThreads);;
	d.describeInt( "ngenerations" ).limits( 1, INT_MAX ).def( 1000 ).help( "Number of the generations of the evolutionary process" );
	d.describeSubgroup( "EVALUATION" ).type( "Evaluation" ).props( IsMandatory ).help( "Object that calculate the fitness", "Create a subclass of Evalution and code your custom fitness function" );
	d.describeSubgroup( "REPRODUCTION" ).type( "Reproduction" ).props( IsMandatory ).help( "Object that generate the new generations" );
	d.describeSubgroup( "GENOME" ).type( "Genome" ).props( IsMandatory ).help( "Object containing the individuals under evolution" );
}

void NSGA2::setNumThreads( int numThreads ) {
	if ( numThreads < 1 ) {
		qWarning( "The number of Threads must be greater than one!!" );
	}
	Q_ASSERT_X( !isInitialized && isFinalized ,
			"NSGA2::setNumThreads",
			"This method can only called before initialize of NSGA2" );
	Q_ASSERT_X( fitfunc != 0 ,
			"NSGA2::setNumThreads",
			"This method must be called after an Evaluation object has been setted by NSGA2::setEvaluation" );
	numThreadv = numThreads;
	return;
}

int NSGA2::numThreads() const {
	return numThreadv;
}

void NSGA2::setEvaluation( Evaluation* fitfunc ) {
	this->fitfunc = fitfunc;
	this->fitfunc->setGA( this );
}

Evaluation* NSGA2::evaluationPrototype()
{
	return fitfunc;
}

QVector<Evaluation*> NSGA2::evaluationPool() {
	QVector<Evaluation*> ev;
	foreach( NSGA2::evaluationThread* e, evalThreads ) {
		ev.append( e->eval );
	}
	return ev;
}

void NSGA2::setReproduction( Reproduction* reprod ) {
	this->reprod = reprod;
	this->reprod->setGA( this );
}

Reproduction* NSGA2::reproduction() {
	return reprod;
}

void NSGA2::initialize() {
	if ( isInitialized && !isFinalized ) return;
	Q_ASSERT_X( fitfunc != 0 ,
			"NSGA2::initialize",
			"You have to setup the Evaluation object of NSGA2 (Fitness Function)" );
	Q_ASSERT_X( reprod !=0 ,
			"NSGA2::initialize",
			"You have to setup the Reproduction operator of NSGA2" );

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
		evalThreads.last()->eval->initGeneration( generation() );
	}
	//--- set the number of thread to create
	QThreadPool::globalInstance()->setMaxThreadCount( numThreadv );
}

void NSGA2::gaStep() {
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
			QFuture<void> future = map( evalThreads, NSGA2::runStepWrapper );
			future.waitForFinished();
		}
		if ( nextGeneration ) {
			currPhase = nextGeneration_pass1;
		}
		} /* End of Multi Thread Block */
		break;
	case nextGeneration_pass1: {
		// --- Core of the NSGA-II Algorithm
		//   -1) merge previous front population lastPareto with current genome()
		//--- this merge assure that the best (elite) pareto-fronts are mantained
		nsgaGenome allGenome;
		for( unsigned int i=0; i<lastPareto.size(); i++ ) {
			allGenome.append( new nsgaGenotype( lastPareto.at(i), 0, 0.0 ) );
		}
		for( unsigned int i=0; i<genome()->size(); i++ ) {
			allGenome.append( new nsgaGenotype( genome()->at(i), 0, 0.0 ) );
		}
		//   -2) fastNonDominatedSort( onMergedPopulation )
		//--- genotype grouped by the front rank
		QVector<nsgaGenome> frontsByRank = fastNonDominatedSort( allGenome );
		//   -3) create new population of size genome().size() using the fronts
		//       calculated and use the crowdingDistanceAssignment
		unsigned int currentGenotype = 0;
		int numOfFronts = frontsByRank.size();
		int numObjs = allGenome[0]->genotype->numOfObjectives();
		for( int front=0; front<numOfFronts; front++ ) {
			//--- sort it on crowding distance and add to the new Genome
			crowdingDistanceAssignment( frontsByRank[front] );
			qStableSort( frontsByRank[front].begin(), frontsByRank[front].end(), NSGA2::crowdingDistanceGreaterThan );
			foreach( nsgaGenotype* gen, frontsByRank[front] ) {
				//--- assign the rank
				gen->genotype->setRank( 2*(numOfFronts - front) + (gen->distance/numObjs) );
				genome()->set( currentGenotype, gen->genotype );
				currentGenotype++;
				if ( currentGenotype == genome()->size() ) break;
			}
			if ( currentGenotype == genome()->size() ) break;
		}
		//   -4) lastPareto <- genome()
		//--- this pass correspond to elite the pareto-fronts
		lastPareto = *(genome());
		//   -5) clean up memory
		for( int i=0; i<allGenome.size(); i++ ) {
			delete (allGenome[i]);
		}
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
		qFatal( "Default switch in NSGA2::gaStep" );
		break;
	}
}

void NSGA2::skipEvaluation() {
	// Set evaluation done, and check which phase to go to
	evaluationDone = true;
	if ( generation() < numGens ) {
		currPhase = nextGeneration_pass2;
	} else {
		currPhase = endEvolution;
	}
}

void NSGA2::finalize() {
	if ( isFinalized && !isInitialized ) return;

	isInitialized = false;
	isFinalized = true;
	evolutionEnd = true;
}

QVector<NSGA2::nsgaGenome> NSGA2::fastNonDominatedSort( nsgaGenome& pareto ) {
	QMap<nsgaGenotype*, nsgaGenome> dominatedBy;
	QVector<nsgaGenome> frontsByRank;
	frontsByRank.resize(1);
	//--- create the first front containing the top solutions
	for( int p=0; p<pareto.size(); p++ ) {
		//--- domination counter of genP reset to zero
		pareto[p]->dominationCounter = 0;
		for( int q=0; q<pareto.size(); q++ ) {
			if ( p==q ) continue;
			if ( pareto[q]->genotype->dominatedBy( pareto[p]->genotype ) ) {
				// OPTIMIZE: it seems that most of the time is spent on accessing the QMap
				dominatedBy[ pareto[p] ].append( pareto[q] );
			} else if ( pareto[p]->genotype->dominatedBy( pareto[q]->genotype ) ) {
				pareto[p]->dominationCounter++;
			}
		}
		//--- if nP == 0 means that genP is a solution belongs to the top pareto-front
		if ( pareto[p]->dominationCounter == 0 ) {
			pareto[p]->rank = 0;
			frontsByRank[0].append( pareto[p] );
		}
	}
	//--- create all the other fronts
	bool done = false;
	int currentFront = 0;
	while( !done ) {
		nsgaGenome newFront;
		for( int i=0; i<frontsByRank[currentFront].size(); i++ ) {
			nsgaGenotype* p = frontsByRank[currentFront][i];
			nsgaGenome pDominate = dominatedBy[p];
			for( int q=0; q<pDominate.size(); q++ ) {
				pDominate[q]->dominationCounter--;
				if ( pDominate[q]->dominationCounter == 0 ) {
					pDominate[q]->rank = currentFront+1;
					newFront.append( pDominate[q] );
				}
			}
		}
		currentFront++;
		if ( newFront.isEmpty() ) {
			done = true;
		} else {
			frontsByRank.append( newFront );
		}
	}
	int total = 0;
	for( int i=0; i<frontsByRank.size(); i++ ) {
		total += frontsByRank[i].size();
	}
	return frontsByRank;
}

void NSGA2::crowdingDistanceAssignment( nsgaGenome& genome ) {
	int dimGenome = genome.size();
	if ( dimGenome == 0 ) return;
	int numObjs = genome[0]->genotype->numOfObjectives();
	//--- vectors containing the max and min values of objectives in this genome
	QVector<double> fmax;
	QVector<double> fmin;
	fmax.resize( numObjs );
	fmin.resize( numObjs );
	for( int i=0; i<numObjs; i++ ) {
		fmax[i] = genome[0]->genotype->objective( i );
		fmin[i] = genome[0]->genotype->objective( i );
	}
	//--- initialize distance and calculate fmax and fmin values
	for( int i=0; i<dimGenome; i++ ) {
		genome[i]->distance = 0;
		for( int m=0; m<numObjs; m++ ) {
			fmax[m] = qMax( fmax[m], genome[i]->genotype->objective(m) );
			fmin[m] = qMin( fmin[m], genome[i]->genotype->objective(m) );
		}
	}
	//--- calculate the distance
	nObjectiveGreaterThan objCompare;
	for( int m=0; m<numObjs; m++ ) {
		// currentObjective is used by nObjectiveGreaterThan for sorting
		objCompare.currentObjective = m;
		qStableSort( genome.begin(), genome.end(), objCompare );
		// the maximum value is numObj, setting to numObj assure that this two
		// genotypes are always the top in the current front
		genome[0]->distance = numObjs; //DBL_MAX;
		genome.last()->distance = numObjs; //DBL_MAX;
		for( int i=1; i<dimGenome-1; i++ ) {
			double m1 = genome[i+1]->genotype->objective(m);
			double m2 = genome[i-1]->genotype->objective(m);
			genome[i]->distance += fabs(m1-m2)/(fmax[m]-fmin[m]);
			// if the value is nan, then it will setted to zero (worst distance)
			if ( genome[i]->distance != genome[i]->distance ) {
				genome[i]->distance = 0.0;
			}
		}
	}
}

NSGA2::evaluationThread::evaluationThread( NSGA2* p, Evaluation* eProto )
	: parent(p), id(0), blocked(false) {
	eval = eProto->clone();
	eval->setGenome( p->genome() );
	eval->setGA( p );
}

NSGA2::evaluationThread::~evaluationThread() {
	delete eval;
	sequence.clear();
}

void NSGA2::evaluationThread::runStep() {
	if ( blocked ) {
		return;
	}
	
	parent->nextGeneration = false;
	eval->evaluateStep();
	if ( eval->isEvaluationDone() ) {
		int nextIdSeq = idSeq + 1;
		eval->finalize();
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

void NSGA2::runStepWrapper( NSGA2::evaluationThread* e ) {
	e->runStep();
}

} // end namespace farsa
