/*******************************************************************************
 * SALSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#include "newgaconfig.h"
#include "evoalgocomponent.h"
#include "evoalgo.h"
#include "xnesalgo.h"
#include "steadystatealgo.h"
#include "hillclimberalgo.h"
#include "experimentinput.h"
#include "experimentoutput.h"
#include "gaevaluator.h"
#include "genotype.h"
#include "genotypetester.h"
#include "robotexperiment.h"
#include "evoalgotest.h"
#include "evoalgotestcurrent.h"
#include "evoalgotestindividual.h"
#include "evoalgotestrandom.h"

// Headers from the configuration and utilities library
#include "configurationparameters.h"
#include "parametersettable.h"
#include "factory.h"
#include "logger.h"

namespace salsa {

bool initNewGALib() {
	static bool dummy = false;
	if ( dummy ) return true;

	//--- Init the Factory
	Factory::getInstance().registerClass<EvoAlgoComponent>( "EvoAlgoComponent", "BaseExperiment" );
	Factory::getInstance().registerClass<EvoAlgo>( "EvoAlgo", "ParameterSettableInConstructor" );
	Factory::getInstance().registerClass<XnesAlgo>( "XnesAlgo", "EvoAlgo" );
	Factory::getInstance().registerClass<SteadyStateAlgo>( "SteadyStateAlgo", "EvoAlgo" );
	Factory::getInstance().registerClass<HillClimberAlgo>( "HillClimberAlgo", "EvoAlgo" );
	Factory::getInstance().registerClass<Genotype>( "Genotype", "ParameterSettableInConstructor" );
	Factory::getInstance().registerClass<GenotypeFloat>( "GenotypeFloat", "Genotype" );
	Factory::getInstance().registerClass<GenotypeInt>( "GenotypeInt", "Genotype" );
	Factory::getInstance().registerClass<GenotypeTester>( "GenotypeTester", "ParameterSettableInConstructor" );
	Factory::getInstance().registerClass<SingleGenotypeTester>( "SingleGenotypeTester", "GenotypeTester" );
	Factory::getInstance().registerClass<MultipleGenotypesTester>( "MultipleGenotypesTester", "GenotypeTester" );
	Factory::getInstance().registerClass<SingleGenotypeFloatToEvonet>( "SingleGenotypeFloatToEvonet", "SingleGenotypeTester" );
	Factory::getInstance().registerClass<SingleGenotypeIntToEvonet>( "SingleGenotypeIntToEvonet", "SingleGenotypeTester" );
	Factory::getInstance().registerClass<RobotExperiment>( "RobotExperiment", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<NewAbstractTest>( "NewAbstractTest", "ParameterSettableWithConfigureFunction" );
	Factory::getInstance().registerClass<EvoAlgoTest>( "EvoAlgoTest", "NewAbstractTest" );
	Factory::getInstance().registerClass<EvoAlgoTestCurrent>( "EvoAlgoTestCurrent", "EvoAlgoTest" );
	Factory::getInstance().registerClass<EvoAlgoTestIndividual>( "EvoAlgoTestIndividual", "EvoAlgoTest" );
	Factory::getInstance().registerClass<EvoAlgoTestRandom>( "EvoAlgoTestRandom", "EvoAlgoTest" );

	dummy = true;
	Logger::info( "Library New GA initialized" );
	return true;
}

} // end namespace salsa
