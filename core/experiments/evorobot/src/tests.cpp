/********************************************************************************
 *  FARSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Piero Savastano <piero.savastano@istc.cnr.it>                               *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#include "tests.h"
#include "evorobotcomponent.h"
#include "evorobotexperiment.h"
#include "logger.h"

#include <iostream>

namespace farsa {

TestRandom::TestRandom() :
	AbstractTest()
{
	m_menuText = "Random Individual";
	m_tooltip = "Create a random genotype and it will run a test on it";
	m_iconFilename = QString();
}

TestRandom::~TestRandom()
{
}

int* TestRandom::buildRandomDNA()
{
	EvoRobotExperiment* exp = component()->getGA()->getEvoRobotExperiment();
	//get genome length and build new array
	int length = exp->getGenomeLength();
	int* randomDna = new int[length];

	//overwrite .phe parameters
	Evonet* net = dynamic_cast<Evonet*>(exp->getAgent(0)->controller());
	net->copyPheParameters(randomDna);

	//randomize genes
	for(int i=0; i<length; i++)
		if(randomDna[i] == Evonet::DEFAULT_VALUE)
			randomDna[i] = rand()%256;

	return randomDna;
}

void TestRandom::runTest()
{
	Logger::info( "Test Random Individual - Starting the test of an agent with a random DNA" );

	// Setting seed to 0, here with a random DNA the seed number is not important
	component()->getGA()->setSeed(0);
	EvoRobotExperiment* exp = component()->getGA()->getEvoRobotExperiment();
	exp->setActivityPhase( EvoRobotExperiment::INTEST );
	// get a random genotype and set it on the net
	int* dna = buildRandomDNA();
	exp->setNetParameters(dna);
	// initialize the experiment
	exp->initGeneration(0);
	exp->doAllTrialsForIndividual(0);
	exp->endGeneration(0);
	delete []dna;
}

TestIndividual::TestIndividual() :
	AbstractTest(),
	idIndividual(0),
	populationFile(),
	populationLoaded(true)
{
	m_menuText = "Selected Individual";
	m_tooltip = "Run a test on the individual selected with \"Individual to Test\" view";
	m_iconFilename = QString();
}

TestIndividual::~TestIndividual()
{
}

void TestIndividual::runTest()
{
	if ( !populationLoaded ) {
		component()->getGA()->loadGenotypes(populationFile);
		// extract the number of corresponding seed
		QRegExp reg("[BG][0-9]+S([0-9]+).gen");
		reg.indexIn( populationFile );
		component()->getGA()->setSeed( reg.cap(1).toInt() );
		Logger::info( QString("TestIndividual - Loaded Genome from %1").arg(populationFile) );
		populationLoaded = true;
	}
	if (idIndividual >= int(component()->getGA()->numLoadedGenotypes())) {
		Logger::error( QString("TestIndividual - Individual %1 doesn't exists (%2 individual have been loaded)").arg(idIndividual).arg(component()->getGA()->numLoadedGenotypes()) );
		return;
	}
	Logger::info( QString("TestIndividual - Start of the Test of Individual %1").arg(idIndividual) );
	EvoRobotExperiment* exp = component()->getGA()->getEvoRobotExperiment();
	exp->setActivityPhase( EvoRobotExperiment::INTEST );
	exp->setNetParameters( component()->getGA()->getGenesForIndividual(idIndividual) );
	exp->initGeneration(0);
	exp->doAllTrialsForIndividual(idIndividual);
	exp->endGeneration(0);
	Logger::info( QString("TestIndividual - End of the Test of Individual %1").arg(idIndividual) );
}

void TestIndividual::setIndividualToTest( int idIndividual ) {
	this->idIndividual = idIndividual;
}

void TestIndividual::setPopulationToTest( QString filename, bool deferLoading ) {
	if ( populationFile != filename ) {
		populationFile = filename;
		populationLoaded = false;
	}
	if ( !deferLoading ) {
		component()->getGA()->loadGenotypes(populationFile);
		// extract the number of corresponding seed
		QRegExp reg("[BG][0-9]+S([0-9]+).gen");
		reg.indexIn( populationFile );
		component()->getGA()->setSeed( reg.cap(1).toInt() );
		Logger::info( QString("TestIndividual - Loaded Genome from %1").arg(populationFile) );
		populationLoaded = true;
	}
}

TestCurrent::TestCurrent() :
	AbstractTest()
{
	m_menuText = "Current Individual";
	m_tooltip = "Run a test on the current individual";
	m_iconFilename = QString();
}

TestCurrent::~TestCurrent()
{
}

void TestCurrent::runTest()
{
	Logger::info( QString("TestCurrent - Start of the Test of Current Individual") );
	EvoRobotExperiment* exp = component()->getGA()->getEvoRobotExperiment();
	exp->setActivityPhase( EvoRobotExperiment::INTEST );
	exp->initGeneration(0);
	exp->doAllTrialsForIndividual(0);
	exp->endGeneration(0);
	Logger::info( QString("TestCurrent - End of the Test of Current Individual") );
}

} //end namespace farsa
