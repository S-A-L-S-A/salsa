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

#include "gaconfig.h"
#include "crossovers/onepoint.h"
#include "evaluations/multitrials.h"
#include "gas/laralga.h"
#include "gas/simplega.h"
#include "gas/parallelga.h"
#include "gas/nsga2.h"
#include "gas/stefanosteadystatega.h"
#include "core/genome.h"
#include "genotypes/realgenotype.h"
#include "genotypes/signedrangegenotype.h"
#include "mutations/flipbit.h"
#include "mutations/randombit.h"
#include "mutations/randomfloat.h"
#include "mutations/gaussianfloat.h"
#include "reproductions/fixedsize.h"
#include "selections/deterministicrank.h"
#include "selections/roulettewheelselection.h"

// Headers from the utilities library
#include "configurationparameters.h"
#include "parametersettable.h"
#include "factory.h"
#include "logger.h"

namespace farsa {

bool initGALib() {
	static bool dummy = false;
	if ( dummy ) return true;
	//--- Init the Factory
	Factory::getInstance().registerClass<Crossover>( "Crossover", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<OnePoint>( "OnePoint", "Crossover" );

	Factory::getInstance().registerClass<Evaluation>( "Evaluation", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<MultiTrials>( "MultiTrials", "Evaluation" );

	Factory::getInstance().registerClass<GeneticAlgo>( "GeneticAlgo", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<LaralGA>( "LaralGA", "GeneticAlgo" );
	Factory::getInstance().registerClass<SimpleGA>( "SimpleGA", "GeneticAlgo" );
	Factory::getInstance().registerClass<ParallelGA>( "ParallelGA", "GeneticAlgo" );
	Factory::getInstance().registerClass<NSGA2>( "NSGA2", "GeneticAlgo" );
	Factory::getInstance().registerClass<StefanoSteadyStateGA>( "StefanoSteadyStateGA", "GeneticAlgo" );

	Factory::getInstance().registerClass<Genotype>( "Genotype", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<DoubleGenotype>( "DoubleGenotype", "Genotype" );
	Factory::getInstance().registerClass<RealGenotype>( "RealGenotype", "DoubleGenotype" );
	Factory::getInstance().registerClass<SignedRangeGenotype>( "SignedRangeGenotype", "DoubleGenotype" );

	Factory::getInstance().registerClass<Genome>( "Genome", "ParameterSettableWithConfigureFunction" );

	Factory::getInstance().registerClass<Mutation>( "Mutation", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<FlipBit>( "FlipBit", "Mutation" );
	Factory::getInstance().registerClass<RandomBit>( "RandomBit", "Mutation" );
	Factory::getInstance().registerClass<RandomFloat>( "RandomFloat", "Mutation" );
	Factory::getInstance().registerClass<GaussianFloat>( "GaussianFloat", "Mutation" );

	Factory::getInstance().registerClass<Reproduction>( "Reproduction", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<FixedSize>( "FixedSize", "Reproduction" );

	Factory::getInstance().registerClass<Selection>( "Selection", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<DeterministicRank>( "DeterministicRank", "Selection" );
	Factory::getInstance().registerClass<RouletteWheelSelection>( "RouletteWheelSelection", "Selection" );

	dummy = true;
	Logger::info( "Library GA initialized" );
	return true;
}

} // end namespace farsa
