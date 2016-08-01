/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#include "evorobotexperiment.h"
#include "sensors.h"
#include "motors.h"
#include "evoga.h"
#include "configurationhelper.h"
#include "logger.h"
#include "simpletimer.h"
#include "guirendererscontainer.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QTime>
#include <QThread>
#include <iostream>
#include <cstdlib>

// All the suff below is to avoid warnings on Windows about the use of unsafe
// functions. This should be only a temporary workaround, the solution is stop
// using C string and file functions...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

#warning QUANDO SI RISCRIVE QUESTA ROBA, CONTROLLARE ANCHE BaseExperiment E FlowControl

EvoRobotExperiment::EvoRobotExperiment(ConfigurationManager& params)
	: QObject()
	, Component(params)
	, ntrials(1)
	, nsteps(1)
	, nstep(0)
	, ntrial(0)
	, ga(NULL)
	, world()
	, renderersContainer(NULL)
	, timestep(0.05f)
	, eagents()
	, gaPhase(NONE)
	, stopCurrentTrial(false)
	, skipCurrentTrial(false)
	, restartCurrentTrial(false)
	, endCurrentIndividualLife(false)
	, batchRunning(false)
	, arena(NULL)
	, stepDelay(timestep*1000)
	, sameRandomSequence(false)
	, randomGeneratorInUse(salsa::globalRNG)
	, localRNG(1)
{
}

EvoRobotExperiment::~EvoRobotExperiment()
{
	// Removing resources
	try {
		deleteResource("arena");
		deleteResource("world");
		deleteResource("experiment");
	} catch (...) {
		// Doing nothing, this is here just to prevent throwing an exception from the destructor
		qDebug() << "INTERNAL ERROR: Exception when deleting resources in EvoRobotExperiment::~EvoRobotExperiment()";
	}

	foreach(EmbodiedAgent* agent, eagents) {
		delete agent;
	}

	delete arena;

	// auto_ptr stuffs will be deleted automatically
}

void EvoRobotExperiment::configure()
{
	ntrials = 1;
	nsteps = 1;

	batchRunning = ConfigurationHelper::getBool(configurationManager(), "__INTERNAL__/BatchRunning", batchRunning); // If we are running in batch or not
	ntrials = ConfigurationHelper::getInt(configurationManager(), confPath() + "ntrials"); // number of trials to do
	nsteps = ConfigurationHelper::getInt(configurationManager(), confPath() + "nsteps"); // number of step for each trial

	// Reading world parameters. We need to do this before calling createWorld because that function uses the parameters
	timestep = ConfigurationHelper::getReal(configurationManager(), confPath() + "World/timestep");
	// initializing the stepDelay at the same amount of timestep
	// will slow down the simulation at real-time pace when the GUI is on
	stepDelay = timestep * 1000;

	// Getting the parameter that lets the user choose whether all individuals should use the same
	// random sequence or not. We also set randomGeneratorInUse to point to the correct generator
	sameRandomSequence = ConfigurationHelper::getBool(configurationManager(), confPath() + "sameRandomSequence");
	if (sameRandomSequence) {
		randomGeneratorInUse = &localRNG;
	} else {
		randomGeneratorInUse = salsa::globalRNG;
	}

	// create a World by default in order to exit from here with all configured properly
	// if they are already created it will not destroy and recreate
	createWorld();

	// Creates the Embodied Agents
	// number of agents to create
	int nagents = ConfigurationHelper::getInt(configurationManager(), confPath() + "nagents");
	// Creating a subgroup for each agent (cloning the AGENT group), the removing the AGENT group
	// (to avoid problems) and finally creating all the agents
	const QString agentGroup = confPath() + "AGENT";
	for (int i = 0; i < nagents; ++i) {
		const QString copiedAgentGroup = agentGroup + ":" + QString::number(i);
		configurationManager().copyGroup(agentGroup, copiedAgentGroup);
	}
	configurationManager().deleteGroup(agentGroup);
	for (int i = 0; i < nagents; ++i) {
		const QString copiedAgentGroup = agentGroup + ":" + QString::number(i);
		eagents.append(configurationManager().getComponentFromGroup<EmbodiedAgent>(copiedAgentGroup));

#warning THIS WILL BE REMOVED WHEN WE HAVE REMOVED/HEAVILY REFACTORED THE Evoga/Evonet/EvorobotExperiment MESS
		if (dynamic_cast<Evonet*>(eagents.last()->controller()) == NULL) {
			ConfigurationHelper::throwUserConfigError(copiedAgentGroup, "", "The only supported controller for the moment is Evonet*");
		}
	}

	// Declaring the evonet resource
#warning THIS WILL BE REMOVED WHEN WE HAVE REMOVED/HEAVILY REFACTORED THE Evoga/Evonet/EvorobotExperiment MESS
	declareResource("evonet", dynamic_cast<Evonet*>(eagents[0]->controller()));

	// Creates the arena (if the group Arena is present)
	createArena();

	// Declaring ourself as a resource (the static_cast is needed because we are both a QObject and a Component)
	declareResource("experiment", static_cast<Component*>(this));

	Logger::info(configurationManager().getValue(confPath() + "type") + " Configured");
}

void EvoRobotExperiment::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The experimental setup that defines the conditions and the fitness function of the evolutionary experiment");

	d.describeInt("ntrials").def(1).limits(1, MaxInteger).help("The number of trials the individual will be tested to calculate its fitness");
	d.describeInt("nsteps").def(1).limits(1, MaxInteger).help("The number of step a trials will last");
	d.describeInt("nagents").def(1).limits(1, MaxInteger).help("The number of embodied agents to create", "This parameter allow to setup experiments with more than one robot; all agents are clones");
	d.describeBool("sameRandomSequence").def(false).help("Whether the generated random number sequence should be the same for all individuals in the same generation or not (default false)");
	d.describeSubgroup("AGENT").props(ParamIsMandatory).componentType("EmbodiedAgent").help("The agent to test");
	d.describeSubgroup("ARENA").componentType("Arena").help("The arena where robots live");

	SubgroupDescriptor& world = d.describeSubgroup("World").help("Parameters affecting the simulated World");
	world.describeReal("timestep").def(0.05).help("The time in seconds corresponding to one simulated step of the World");
}

void EvoRobotExperiment::postConfigureInitialization()
{
	if (!batchRunning) {
		// preventing gas from using multithread, which is not supported if the GUI is present
		ga->doNotUseMultipleThreads();
	}

	// Doing evolution by default
	gaPhase = EvoRobotExperiment::INEVOLUTION;
}

void EvoRobotExperiment::setEvoga(Evoga* ga)
{
	this->ga = ga;

	// Also resetting the seed of the local random number generator
	localRNG.setSeed(ga->getCurrentSeed());
}

Evoga* EvoRobotExperiment::getEvoga()
{
	return ga;
}

bool EvoRobotExperiment::inBatchRunning() const
{
	return batchRunning;
}

int EvoRobotExperiment::getNAgents() const
{
	return eagents.size();
}

EmbodiedAgent* EvoRobotExperiment::getAgent(int i)
{
	return eagents[i];
}

const EmbodiedAgent* EvoRobotExperiment::getAgent(int i) const
{
	return eagents[i];
}

void EvoRobotExperiment::setNetParameters(float *genes)
{
	foreach(EmbodiedAgent* agent, eagents) {
		dynamic_cast<Evonet*>(agent->controller())->setParameters(genes);
	}
}

void EvoRobotExperiment::setNetParameters(int *genes)
{
	foreach(EmbodiedAgent* agent, eagents) {
		dynamic_cast<Evonet*>(agent->controller())->setParameters(genes);
	}
}

void EvoRobotExperiment::initGeneration(int)
{
}

void EvoRobotExperiment::initIndividual(int)
{
}

void EvoRobotExperiment::initTrial(int)
{
	// Reset the controller
	foreach(EmbodiedAgent* agent, eagents) {
		agent->controller()->resetControllerStatus();
	}
}

void EvoRobotExperiment::initStep(int)
{
}

double EvoRobotExperiment::getFitness()
{
	return totalFitnessValue;
}

double EvoRobotExperiment::getError()
{
	return totalErrorValue;
}

void EvoRobotExperiment::stopTrial()
{
	stopCurrentTrial = true;
}

void EvoRobotExperiment::skipTrial()
{
	skipCurrentTrial = true;
}

void EvoRobotExperiment::restartTrial()
{
	restartCurrentTrial = true;
}

void EvoRobotExperiment::endIndividualLife()
{
	endCurrentIndividualLife = true;
	stopCurrentTrial = true;
}

void EvoRobotExperiment::afterSensorsUpdate()
{
}

void EvoRobotExperiment::beforeMotorsUpdate()
{
}

void EvoRobotExperiment::beforeWorldAdvance()
{
}

void EvoRobotExperiment::endTrial(int)
{
	totalFitnessValue += trialFitnessValue;
	totalErrorValue += trialErrorValue;
}

void EvoRobotExperiment::endIndividual(int)
{
}

void EvoRobotExperiment::endGeneration(int)
{
}

void EvoRobotExperiment::doAllTrialsForIndividual(int individual)
{
	// Checking if we have to reset the seed for the current individual
	if (sameRandomSequence) {
		localRNG.setSeed(ga->getCurrentSeed() + (ga->getCurrentGeneration() * ga->getNumReplications()));
	}

	endCurrentIndividualLife = false;
	totalFitnessValue = 0.0;

	initIndividual(individual);
	if (ga->commitStep()) {
		return;
	}

	for (ntrial = 0; ntrial < ntrials; ntrial++) {
		skipCurrentTrial = false;

		initTrial(ntrial);
		if (ga->commitStep()) {
			break;
		}
		if (skipCurrentTrial) {
			continue;
		}
		if (!endCurrentIndividualLife) {
			doTrial();
		}
		if (ga->isStopped()) {
			break;
		}
		if (restartCurrentTrial) {
			ntrial--;
			continue;
		}
		endTrial(ntrial);

		if (gaPhase == INTEST) {
			Logger::info("Fitness for trial " + QString::number(ntrial) + ": " + QString::number(trialFitnessValue));
		}

		if (ga->commitStep() || endCurrentIndividualLife) {
			break;
		}

	}

	endIndividual(individual);
	ga->commitStep();

#ifdef SALSA_MAC
	// If the GUI is active, adding a small delay to let the GUI catch up (this is only done on MacOSX
	// which seems the most troublesome system)
	if (!batchRunning) {
		// Using also the namespace to avoid name clashes
		salsa::msleep(50);
	}
#endif
}

int EvoRobotExperiment::getGenomeLength()
{
	return dynamic_cast<Evonet*>(eagents[0]->controller())->freeParameters();
}

void EvoRobotExperiment::setTestingAgentAndSeed(int,int)
{
	Logger::error("EvoRobotExperiment::setTestingAgentAndSeed() not yet implemented");
}

void EvoRobotExperiment::newGASeed(int seed)
{
	localRNG.setSeed(seed);
}

void EvoRobotExperiment::setStepDelay(int delay)
{
	stepDelay = delay;
}

int EvoRobotExperiment::getStepDelay()
{
	return stepDelay;
}

float EvoRobotExperiment::getWorldTimeStep() const
{
	return timestep;
}

void EvoRobotExperiment::resetWorld()
{
	if (world.get() == NULL) {
		return;
	}

	destroyArena();

	world->reset();

#warning THIS SHOULD BE REMOVED, BUT AFTER THE CALL ABOVE ALL WORLD OBJECT ARE DESTROYED!!! THIS MEANS THAT WE SHOULD USE NOTIFICATION OF WENTITY DESTRUCTIONS INSTEAD OF RESOURCE IN E.G. EmbodiedAgent
	declareResource("world", world.get());
}

void EvoRobotExperiment::recreateAllRobots()
{
	foreach (EmbodiedAgent* agent, eagents) {
		agent->recreateRobot();
	}
}

void EvoRobotExperiment::recreateArena()
{
	destroyArena();
	createArena();
}

RandomGenerator* EvoRobotExperiment::getRNG()
{
	return randomGeneratorInUse;
}

void EvoRobotExperiment::doTrial()
{
	restartCurrentTrial = false;
	stopCurrentTrial = false;
	trialFitnessValue = 0.0;
	trialErrorValue = 0.0;
	for(nstep = 0; nstep < nsteps; nstep++) {
		initStep(nstep);
		if (ga->commitStep() || restartCurrentTrial) {
			break;
		}
		doStep();
		if (ga->commitStep()) break;
		endStep(nstep);
		if (ga->commitStep() || stopCurrentTrial || restartCurrentTrial) {
			break;
		}
	}
}

void EvoRobotExperiment::doStep()
{
	if (!batchRunning && stepDelay>0) {
		// To use platform-independent sleep functions we have to do this...
		class T : public QThread
		{
		public:
			using QThread::sleep;
			using QThread::msleep;
			using QThread::usleep;
		};
		T::msleep( stepDelay );
	}

	// update sensors
	foreach(EmbodiedAgent* agent, eagents) {
		agent->updateSensors();
	}
	afterSensorsUpdate();
	// update the neural controller
	foreach(EmbodiedAgent* agent, eagents) {
		agent->updateController();
	}
	beforeMotorsUpdate();
	// setting motors
	foreach(EmbodiedAgent* agent, eagents) {
		agent->updateMotors();
	}
	beforeWorldAdvance();
	// advance the world simulation
	if (arena != NULL) {
		arena->prepareToHandleKinematicRobotCollisions();
	}
	world->advance();
	if (arena != NULL) {
		arena->handleKinematicRobotCollisions();
	}
}

void EvoRobotExperiment::createWorld()
{
	if (world.get() != NULL) {
		return;
	}

	// TODO: parametrize the name and the dontUseYarp and all other parameters
	world.reset(new World("World"));
	world->setTimeStep(timestep);
	world->setSize(wVector(-2.0f, -2.0f, -0.50f), wVector(+2.0f, +2.0f, +2.0f));
	world->setFrictionModel("exact");
	world->setSolverModel("exact");
	world->setMultiThread(1);

	declareResource("world", world.get());

	// Also creating the renderers container if we are not running in batch
	if (!batchRunning) {
		renderersContainer = world->createRenderersContainer(TypeToCreate<GUIRenderersContainer>());
	}
}

void EvoRobotExperiment::destroyArena()
{
	if ((arena == NULL) || !configurationManager().groupExists(confPath() + "ARENA")) {
		return;
	}

	declareResourceAsNull("arena");

	delete arena;
	arena = NULL;
}

void EvoRobotExperiment::createArena()
{
	if ((arena != NULL) || !configurationManager().groupExists(confPath() + "ARENA")) {
		return;
	}

	// Make sure that a World exist
	createWorld();

	arena = configurationManager().getComponentFromGroup<Arena>(confPath() + "ARENA");

	Arena::RobotsList robots;
	foreach(EmbodiedAgent* agent, eagents) {
		robots.append(Arena::RobotResource("robot", agent));
	}
	arena->addRobots(robots);

	declareResource("arena", arena);
}

} // end namespace salsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
