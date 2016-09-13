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
#include "gas/stefanosteadystatega.h"
#include "core/reproduction.h"
#include "core/genome.h"
#include "core/evaluation.h"
#include "logger.h"
#include "configurationhelper.h"
#include <QThreadPool>
#include <QtConcurrentMap>
using namespace QtConcurrent;

namespace salsa {

StefanoSteadyStateGA::StefanoSteadyStateGA() :
	GeneticAlgo(),
	fitfunc(nullptr),
	muta(nullptr),
	curPhase(initParent),
	curGenotype(0),
	isInitialized(false),
	isFinalized(true),
	numEvaluations(),
	cumulatedFitness(),
	offspring(nullptr)
{
}

StefanoSteadyStateGA::~StefanoSteadyStateGA() {
	delete fitfunc;
	delete muta;
}

void StefanoSteadyStateGA::configure( ConfigurationParameters& params, QString prefix ) {
	setGenome( params.getObjectFromGroup<Genome>( prefix + QString( "GENOME" ) ) );
	setEvaluation( params.getObjectFromGroup<Evaluation>( prefix + QString( "EVALUATION" ) ) );
	fitfunc->setGenome( genome() );
	setMutation( params.getObjectFromGroup<Mutation>( prefix + QString( "MUTATION" ) ) );
	setNumGenerations( ConfigurationHelper::getInt( params, prefix + QString( "ngenerations" ), 1000 ) );
}

void StefanoSteadyStateGA::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "StefanoSteadyStateGA" );
	params.createParameter( prefix, QString("ngenerations"), QString("%1").arg( numGenerations() ) );
	//--- EVALUATION
	fitfunc->save( params, params.createSubGroup( prefix, "EVALUATION" ) );
	//--- MUTATION
	mutation()->save( params, params.createSubGroup( prefix, "MUTATION" ) );
	//--- GENOME
	genome()->save( params, params.createSubGroup( prefix, "GENOME" ) );
}

void StefanoSteadyStateGA::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A variant of Steady State Genetic Algorithm developed by Stefano Nolfi" );
	d.describeInt( "numThreads" ).limits( 1, 32 ).help( "Number of threads to parallelize the evaluation of individuals" );
	d.describeInt( "ngenerations" ).limits( 1, MaxInteger ).def( 1000 ).help( "Number of the generations of the evolutionary process" );
	d.describeSubgroup( "EVALUATION" ).type( "Evaluation" ).props( IsMandatory ).help( "Object that calculate the fitness", "Create a subclass of Evalution and code your custom fitness function" );
	d.describeSubgroup( "MUTATION").type( "Mutation" ).props( IsMandatory ).help( "Object that mutate the genotype of an individual" );
	d.describeSubgroup( "GENOME" ).type( "Genome" ).props( IsMandatory ).help( "Object containing the individuals under evolution" );
}

void StefanoSteadyStateGA::setEvaluation( Evaluation* fitfunc ) {
	this->fitfunc = fitfunc;
	this->fitfunc->setGA( this );
}

Evaluation* StefanoSteadyStateGA::evaluationPrototype()
{
	return fitfunc;
}

QVector<Evaluation*> StefanoSteadyStateGA::evaluationPool() {
	// For the moment we only have one evaluator (no threads)
	QVector<Evaluation*> ev;
	ev.append(fitfunc);
	return ev;
}

void StefanoSteadyStateGA::setMutation( Mutation* mutation ) {
	this->muta = mutation;
	this->muta->setGA( this );
}

Mutation* StefanoSteadyStateGA::mutation() {
	return muta;
}

void StefanoSteadyStateGA::initialize() {
	if ( isInitialized && !isFinalized ) return;
	Q_ASSERT_X( fitfunc != 0 ,
			"StefanoSteadyStateGA::initialize",
			"You have to setup the Evaluation object of StefanoSteadyStateGA (Fitness Function)" );
	Q_ASSERT_X( muta !=0 ,
			"StefanoSteadyStateGA::initialize",
			"You have to setup the Mutation operator of StefanoSteadyStateGA" );

	isInitialized = true;
	isFinalized = false;
	setGeneration(0);
	curPhase = initParent;
	curGenotype = 0;
	evolutionEnd = false;
	evaluationDone = false;
	delete offspring;
	offspring = nullptr;

	getIODelegate()->recoverData(this);

	// Initializing mutation and evaluator
	muta->setGenome(genome());
	fitfunc->setGenome(genome());
	fitfunc->initGeneration( generation() );
	numEvaluations.fill(0, genome()->size());
	cumulatedFitness.fill(0.0, genome()->size());
}

void StefanoSteadyStateGA::gaStep() {
	switch (curPhase) {
		case initParent:
			{
				fitfunc->initialize(genome()->at(curGenotype));
				curPhase = evalParent;
			}
			break;
		case evalParent:
			{
				fitfunc->evaluateStep();
				if (fitfunc->isEvaluationDone()) {
					fitfunc->finalize();
					cumulatedFitness[curGenotype] += fitfunc->genotype()->fitness();
					numEvaluations[curGenotype]++;
					fitfunc->genotype()->setRank(cumulatedFitness[curGenotype] / double(numEvaluations[curGenotype]));
					curPhase = initOffspring;
				}
			}
			break;
		case initOffspring:
			{
				// Generating one offspring of the current genome
				delete offspring;
				offspring = genome()->at(curGenotype)->clone();
				muta->mutate(offspring);
				fitfunc->initialize(offspring);
				curPhase = evalOffspring;
			}
			break;
		case evalOffspring:
			{
				fitfunc->evaluateStep();
				if (fitfunc->isEvaluationDone()) {
					fitfunc->finalize();
					offspring->setRank(offspring->fitness());
					curPhase = compareOffspring;
				}
			}
			break;
		case compareOffspring:
			{
				// Replacing the individual with the lowest fitness with the offspring. First searching
				// the worst genome...
				unsigned int worstGenome = 0;
				double worstGenomeFitness = genome()->at(0)->rank();
				for (unsigned int i = 1; i < genome()->size(); i++) {
					if (numEvaluations[i] == 0) {
						continue;
					}
					if (genome()->at(i)->rank() < worstGenomeFitness) {
						worstGenome = i;
						worstGenomeFitness = genome()->at(i)->rank();
					}
				}
				// ... now checking if the worst genome is worse than the offspring. If so, swapping
				if (worstGenomeFitness < offspring->rank()) {
					*(genome()->at(worstGenome)) = *offspring;
					cumulatedFitness[worstGenome] = offspring->rank();
					numEvaluations[worstGenome] = 1;
				}
				if (curGenotype < (genome()->size() - 1)) {
					curGenotype++;
					curPhase = initParent;
				} else {
					evaluationDone = true;
					curPhase = nextGeneration;
				}
			}
			break;
		case nextGeneration:
			{
				// We don't sort the genome, there is no need to
				updateStats();
				evaluationDone = false;
				fitfunc->endGeneration( generation() );
				notifyEndGeneration();
				getIODelegate()->saveData(this);
				if (generation() < numGens) {
					curPhase = initParent;
					setGeneration( generation()+1 );
					fitfunc->initGeneration( generation() );
					curGenotype = 0;
				} else {
					curPhase = endEvolution;
				}
			}
			break;
		case endEvolution:
			{
				finalize();
			}
			break;
		default:
			qFatal("Default switch in StefanoSteadyStateGA::gaStep");
			break;
	}
}

void StefanoSteadyStateGA::skipEvaluation() {
	// Set evaluation done, and set the new phase
	evaluationDone = true;
	curPhase = nextGeneration;
}

void StefanoSteadyStateGA::finalize() {
	if ( isFinalized && !isInitialized ) return;

	isInitialized = false;
	isFinalized = true;
	evolutionEnd = true;
	delete offspring;
	offspring = nullptr;

	// Rank is already normalized

	// Finally sorting genome
	qSort( genome()->begin(), genome()->end(), Genotype::rankGreaterThanComparator );
}

} // end namespace salsa
