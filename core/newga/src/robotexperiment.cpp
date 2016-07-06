/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#include "robotexperiment.h"
#include "robotexperimentui.h"
#include "sensors.h"
#include "motors.h"
#include "configurationhelper.h"
#include "factory.h"
#include "logger.h"
#include "simpletimer.h"
#include "evonetui.h"

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

namespace farsa {

/*
 * Experiment constructor
 */
RobotExperiment::RobotExperiment()
	: OneNeuralNetworkExperimentInput<Evonet>()
	, SingleFitnessGAEvaluator()
	, FlowControlled()
	, ntrials(1)
	, nsteps(1)
	, nstep(0)
	, ntrial(0)
	, savedConfigurationParameters(NULL)
	, savedPrefix(NULL)
	, world(NULL)
	, timestep(0.05f)
	, eagents()
	, agentIdSelected(0)
	, gaPhase(NONE)
	, stopCurrentTrial(false)
	, skipCurrentTrial(false)
	, restartCurrentTrial(false)
	, endCurrentIndividualLife(false)
	, batchRunning(false)
	, arena(NULL)
	, evonet(NULL)
	, stepDelay(timestep*1000)
	, sameRandomSequence(false)
	, randomGeneratorInUse(farsa::globalRNG)
	, localRNG(1)
	, curInd(-1)
{
	// Stating which resources we use here. This is here in the constructor so that we are sure to
	// be the first to declare resources (if we did this later we should have used addUsableResources
	// because child classes could declare they will use resources before us)
	usableResources( QStringList() << "world" << "arena" << "experiment" << "robot" << "evonet" << "neuronsIterator" );
}

RobotExperiment::~RobotExperiment()
{
	// Removing resources
	try {
		deleteResource("experiment");
		deleteResource("arena");
		deleteResource("world");
		deleteResource("robot");
		deleteResource("evonet");
		deleteResource("neuronsIterator");
	} catch (...) {
		// Doing nothing, this is here just to prevent throwing an exception from the destructor
	}

	foreach( EmbodiedAgent* agent, eagents ) {
		delete agent;
	}

	delete savedConfigurationParameters;
	delete savedPrefix;
	delete arena;
	delete evonet;
	delete world;
}

void RobotExperiment::configure(ConfigurationParameters& params, QString prefix)
{
	// Saving configuration parameters and prefix for cloning
	delete savedConfigurationParameters;
	delete savedPrefix;
	savedConfigurationParameters = new ConfigurationParameters(params);
	savedConfigurationParameters->shareObserversWith(params);
	savedPrefix = new QString(prefix);
	// Setting ourself as resource manager in the configuration parameters object
	params.setResourcesUser(this);
	savedConfigurationParameters->setResourcesUser(this);

	ntrials = 1;
	nsteps = 1;

	batchRunning = ConfigurationHelper::getBool(params, "__INTERNAL__/BatchRunning", batchRunning); // If we are running in batch or not
	ntrials = ConfigurationHelper::getInt(params, prefix + "ntrials", ntrials); // number of trials to do
	notifyChangesToParam( "ntrials" );
	nsteps = ConfigurationHelper::getInt(params, prefix + "nsteps", nsteps); // number of step for each trial

	// Reading world parameters. We need to do this before calling recreateWorld because that function uses the parameters
	timestep = ConfigurationHelper::getDouble(params, prefix + "World/timestep", timestep);
	// initializing the stepDelay at the same amount of timestep
	// will slow down the simulation at real-time pace when the GUI is on
	stepDelay = timestep*1000;

	// Getting the parameter that lets the user choose whether all individuals should use the same
	// random sequence or not. We also set randomGeneratorInUse to point to the correct generator
	sameRandomSequence = ConfigurationHelper::getBool(params, prefix + "sameRandomSequence", sameRandomSequence);
	if (sameRandomSequence) {
		randomGeneratorInUse = &localRNG;
	} else {
		randomGeneratorInUse = farsa::globalRNG;
	}

	// create a World by default in order to exit from here with all configured properly
	// if they are already created it will not destroy and recreate
	recreateWorld();
	// Creates the arena (if the group Arena is present)
	recreateArena();
	// Creates the Embodied Agents
	// number of agents to create
	int nagents = ConfigurationHelper::getInt(params, prefix + "nagents", 1);
	if ( nagents > 1 ) {
		// refactor the configuration parameters and create a subgroup foreach agent
		for( int i=0; i<nagents; i++ ) {
			QString agentPrefix = prefix + "AGENT:" + QString::number(i) + "/";
			savedConfigurationParameters->createGroup( agentPrefix );
			savedConfigurationParameters->copyGroupTree( prefix+"ROBOT", agentPrefix+"ROBOT" );
			QStringList sensorsList = savedConfigurationParameters->getGroupsWithPrefixList(prefix, "Sensor:");
			foreach( QString sensorGroup, sensorsList ) {
				savedConfigurationParameters->copyGroupTree( prefix+sensorGroup, agentPrefix+sensorGroup );
			}
			QStringList motorsList = savedConfigurationParameters->getGroupsWithPrefixList(prefix, "Motor:");
			foreach( QString motorGroup, motorsList ) {
				savedConfigurationParameters->copyGroupTree( prefix+motorGroup, agentPrefix+motorGroup );
			}
			eagents.append( new EmbodiedAgent(i, agentPrefix, this) );
			eagents.last()->configure();
		}
	} else {
		eagents.append( new EmbodiedAgent(0, prefix, this) );
		eagents.last()->configure();
	}
	//selectAgent(0);

	// Adding robots to the arena (if the arena exists)
	if (arena != NULL) {
		QStringList robots;
		foreach(EmbodiedAgent* e, eagents) {
			robots.append(e->resourcePrefix+"robot");
		}
		arena->addRobots(robots);
	}

	// declaring other resources
	declareResource( "experiment", static_cast<ParameterSettableWithConfigureFunction*>(this) );

	Logger::info( params.getValue(prefix+"type") + " Configured" );
}

void RobotExperiment::save(ConfigurationParameters&, QString)
{
	Logger::error("NOT IMPLEMENTED (RobotExperiment::save)");
	abort();
}

void RobotExperiment::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "The experimental setup that defines the conditions and the fitness function of the evolutionary experiment" );
	d.describeInt( "ntrials" ).def(1).limits(1,MaxInteger).runtime( &RobotExperiment::setNTrials, &RobotExperiment::getNTrials ).help("The number of trials the individual will be tested to calculate its fitness");
	d.describeInt( "nsteps" ).def(1).limits(1,MaxInteger).help("The number of step a trials will last");
	d.describeInt( "nagents" ).def(1).limits(1,MaxInteger).help("The number of embodied agents to create", "This parameter allow to setup experiments with more than one robot; all agents are clones");
	d.describeBool("sameRandomSequence").def(false).help("Whether the generated random number sequence should be the same for all individuals in the same generation or not (default false)");
	d.describeSubgroup( "ROBOT" ).props( IsMandatory ).type( "Robot" ).help( "The robot");
	d.describeSubgroup( "Sensor" ).props( AllowMultiple ).type( "Sensor" ).help( "One of the Sensors from which the neural network will receive information about the environment" );
	d.describeSubgroup( "Motor" ).props( AllowMultiple ).type( "Motor" ).help( "One of the Motors with which the neural network acts on the robot and on the environment" );
	d.describeSubgroup( "Arena" ).type( "Arena" ).help( "The arena where robots live");

	SubgroupDescriptor world = d.describeSubgroup( "World" ).help( "Parameters affecting the simulated World" );
	world.describeReal( "timestep" ).def(0.05).runtime( &RobotExperiment::setWorldTimestep, &RobotExperiment::getWorldTimeStep ).help( "The time in seconds corresponding to one simulated step of the World" );
}

void RobotExperiment::postConfigureInitialization()
{
	// Doing evolution by default
	gaPhase=RobotExperiment::INEVOLUTION;
}

void RobotExperiment::doTrial()
{
	restartCurrentTrial = false;
	stopCurrentTrial = false;
	trialFitnessValue = 0.0;
	trialErrorValue = 0.0;
	for(nstep = 0; nstep < nsteps; nstep++) {
		initStep( nstep );
		pauseFlow();
		if (stopFlow() || restartCurrentTrial)
		{
			break;
		}
		doStep();
		pauseFlow();
		if (stopFlow())
		{
			break;
		}
		endStep( nstep );
		pauseFlow();
		if (stopFlow() || stopCurrentTrial || restartCurrentTrial)
		{
			break;
		}
	}
}

void RobotExperiment::doAllTrialsForIndividual(int individual)
{
	// Checking if we have to reset the seed for the current individual
	if (sameRandomSequence) {
		localRNG.setSeed(0); // TO BE FIXED AS SOON AS POSSIBLE!!!!
	}

	endCurrentIndividualLife = false;
	totalFitnessValue = 0.0;

	initIndividual(individual);
	pauseFlow();
	if (stopFlow())
	{
		return;
	}

	for (ntrial = 0; ntrial < ntrials; ntrial++) {
		skipCurrentTrial = false;

		initTrial(ntrial);
		pauseFlow();
		if (stopFlow())
		{
			break;
		}
		if (skipCurrentTrial) { // && !ga->commitStep()) {
			continue;
		}
		if (!endCurrentIndividualLife) {
			doTrial();
		}
		pauseFlow();
		if (stopFlow())
		{
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

		pauseFlow();
		if (stopFlow() || endCurrentIndividualLife)
		{
			break;
		}

	}

	endIndividual(individual);
	pauseFlow();
	if (stopFlow())
	{
		return;
	}

#ifdef FARSA_MAC
	// If the GUI is active, adding a small delay to let the GUI catch up (this is only done on MacOSX
	// which seems the most troublesome system)
	if (!batchRunning) {
		// Using also the namespace to avoid name clashes
		farsa::msleep(50);
	}
#endif
}

void RobotExperiment::initGeneration(int)
{
}

void RobotExperiment::initIndividual(int)
{
}

void RobotExperiment::initTrial(int)
{
	// reset the neural controller
	ResourcesLocker locker(this);
	foreach( EmbodiedAgent* agent, eagents ) {
		agent->evonet->resetNet();
	}
}

void RobotExperiment::initStep(int)
{
}

void RobotExperiment::evaluate()
{
	// perform the evaluation
	doAllTrialsForIndividual(curInd);
	curInd++;
}

void RobotExperiment::setIndividualCounter()
{
	curInd = 0;
}

void RobotExperiment::resetIndividualCounter()
{
	curInd = -1;
}

real RobotExperiment::getFitness()
{
	return totalFitnessValue;
}

double RobotExperiment::getError()
{
	return totalErrorValue;
}

void RobotExperiment::afterSensorsUpdate()
{
}

void RobotExperiment::beforeMotorsUpdate()
{
}

void RobotExperiment::beforeWorldAdvance()
{
}

void RobotExperiment::endTrial(int)
{
	totalFitnessValue += trialFitnessValue;
	totalErrorValue += trialErrorValue;
}

void RobotExperiment::endIndividual(int)
{
}

void RobotExperiment::endGeneration(int)
{
}

void RobotExperiment::doStep()
{
	// There is no getResource below, but we are actually using resources so we must take the lock.
	// We don't acquire the lock here, but lock and unlock when needed in the body of the function
	ResourcesLocker locker(this, false);

	/*if (!batchRunning && stepDelay>0) {
		// To use platform-independent sleep functions we have to do this...
		class T : public QThread
		{
		public:
			using QThread::sleep;
			using QThread::msleep;
			using QThread::usleep;
		};
		T::msleep( stepDelay );
	}*/

	// update sensors
	foreach( EmbodiedAgent* agent, eagents ) {
		if (agent->enabled) {
			for (int s = 0; s < agent->sensors.size(); s++) {
				agent->sensors[s]->update();
			}
		}
	}
	afterSensorsUpdate();
	// update the neural controller
	locker.lock();
	foreach( EmbodiedAgent* agent, eagents ) {
		if (agent->enabled) {
			agent->evonet->updateNet();
		}
	}
	locker.unlock();
	beforeMotorsUpdate();
	// setting motors
	foreach( EmbodiedAgent* agent, eagents ) {
		if (agent->enabled) {
			for (int m = 0; m < agent->motors.size(); m++) {
				agent->motors[m]->update();
			}
		}
	}
	beforeWorldAdvance();
	// advance the world simulation
	locker.lock();
	if (arena != NULL) {
		arena->prepareToHandleKinematicRobotCollisions();
	}
	world->advance();
	if (arena != NULL) {
		arena->handleKinematicRobotCollisions();
	}
	locker.unlock();
}

void RobotExperiment::stopTrial()
{
	stopCurrentTrial = true;
}

void RobotExperiment::skipTrial()
{
	skipCurrentTrial = true;
}

void RobotExperiment::restartTrial()
{
	restartCurrentTrial = true;
}

void RobotExperiment::endIndividualLife()
{
	endCurrentIndividualLife = true;
	stopCurrentTrial = true;
}

int RobotExperiment::getRequestedSensors() const
{
	// calculate the number of sensors neurons
	int nSensors = 0;
	foreach( EmbodiedAgent* agent, eagents ) {
		if (agent->enabled) {
			foreach( Sensor* sensor, agent->sensors ) {
				nSensors += sensor->size();
			}
			break;
		}
	}
	return nSensors;
}

int RobotExperiment::getRequestedMotors() const
{
	// calculate the number of motors neurons
	int nMotors = 0;
	foreach( EmbodiedAgent* agent, eagents ) {
		if (agent->enabled) {
			foreach( Motor* motor, agent->motors ) {
				nMotors += motor->size();
			}
			break;
		}
	}
	return nMotors;
}

Sensor* RobotExperiment::getSensor( QString name, int id ) {
	if ( !eagents[id]->enabled ) {
		Logger::error( "getSensor returned NULL pointer because the agent "+QString::number(id)+" is disabled" );
		return NULL;
	} else if ( eagents[id]->sensorsMap.contains( name ) ) {
		return eagents[id]->sensorsMap[name];
	} else {
		Logger::error( "getSensor returned NULL pointer because there is no sensor named "+name+" in the agent "+QString::number(id) );
		return NULL;
	}
}

Motor* RobotExperiment::getMotor( QString name, int id ) {
	if ( !eagents[id]->enabled ) {
		Logger::error( "getMotor returned NULL pointer because the agent "+QString::number(id)+" is disabled" );
		return NULL;
	} else if ( eagents[id]->motorsMap.contains( name ) ) {
		return eagents[id]->motorsMap[name];
	} else {
		Logger::error( "getMotor returned NULL pointer because there is no motor named "+name+" in the agent "+QString::number(id) );
		return NULL;
	}
}

bool RobotExperiment::inBatchRunning()
{
	return batchRunning;
}

int RobotExperiment::getNAgents() {
	return eagents.size();
}

bool RobotExperiment::selectAgent( int id ) {
	if (!eagents[id]->enabled) {
		return false;
	}
	agentIdSelected = id;
	declareResource( "robot",
					 eagents[agentIdSelected]->robot,
					 eagents[agentIdSelected]->resourcePrefix+"robot" );
	declareResource( "evonet",
					 static_cast<farsa::ParameterSettable*>(eagents[agentIdSelected]->evonet),
					 eagents[agentIdSelected]->resourcePrefix+"evonet" );
	declareResource( "neuronsIterator",
					 eagents[agentIdSelected]->neuronsIterator,
					 eagents[agentIdSelected]->resourcePrefix+"neuronsIterator" );
	return true;
}

void RobotExperiment::enableAgent( int agentId ) {
	eagents[agentId]->enable();
}

void RobotExperiment::disableAgent( int agentId ) {
	eagents[agentId]->disable();
}

bool RobotExperiment::agentEnabled( int agentId ) {
	return eagents[agentId]->enabled;
}

Evonet* RobotExperiment::getNeuralNetwork( int id )
{
	return eagents[id]->evonet;
}

void RobotExperiment::setNeuralNetwork(const Evonet* net)
{
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning Problema: nel caso di multiagent, ogni agente deve avere la sua GUI della rete neurale. Bisogna quindi creare anche le GUI qui (prima funzionava perché si ricreava la rete da ConfigurationParameters tutte le volte)
	#warning ALTRO PROBLEMA: CAMBIARE LA NET DELLE UI NON È THREAD-SAFE TRANNE CHE PER IL NEURON VIEWER (CHE USA UPLOADER/DOWNLOADER). BISOGNA PASSARE TUTTO A UPLOADER/DOWNLOADER
#endif
	foreach( EmbodiedAgent* agent, eagents ) {
		if (agent->enabled) {
			Evonet* oldNet = agent->evonet;
			agent->evonet = net->cloneNet();
			agent->neuronsIterator->setEvonet( agent->evonet );
			// create the blocks associated to the network
			int startIndex = 0;
			foreach( Sensor* sensor, agent->sensors ) {
				agent->neuronsIterator->defineBlock( sensor->name(), EvonetIterator::InputLayer, startIndex, sensor->size() );
				startIndex += sensor->size();
			}
			startIndex = 0;
			foreach( Motor* motor, agent->motors ) {
				agent->neuronsIterator->defineBlock( motor->name(), EvonetIterator::OutputLayer, startIndex, motor->size() );
				startIndex += motor->size();
			}
			declareResource( agent->resourcePrefix+"evonet", static_cast<farsa::ParameterSettable*>(agent->evonet) );
			declareResource( agent->resourcePrefix+"neuronsIterator", agent->neuronsIterator, agent->resourcePrefix+"evonet" );
			delete oldNet;

			// Making this net the one responsible of updating guis
			EvonetUI* uimanager = net->getCurrentUIManager();
			if (uimanager != NULL) {
				uimanager->changeNet(agent->evonet);
			}
		}
	}
	selectAgent(0);
}

ParameterSettableUI* RobotExperiment::getUIManager() {
	return new RobotExperimentUI( this );
}

void RobotExperiment::setTestingAgentAndSeed(int,int)
{
	Logger::error("RobotExperiment::setTestingAgentAndSeed() not yet implemented");
}

void RobotExperiment::recreateWorld() {
	// Saving the old robot, the old arena and world to delete them after the new world has been
	// created (as world is a resource, we need the old instance to exists during notifications.
	// It can be safely deleted afterward)
	World* const old_world = world;

	// TODO: parametrize the name and the dontUseYarp and all other parameters
	world = new World( "World" );
	world->setTimeStep( timestep );
	world->setSize( wVector( -2.0f, -2.0f, -0.50f ), wVector( +2.0f, +2.0f, +2.0f ) );
	world->setFrictionModel( "exact" );
	world->setSolverModel( "exact" );
	world->setMultiThread( 1 );
	world->setIsRealTime( false );

	// Removing deleted resources (if they existed) and then re-declaring world
	Arena* old_arena = arena;
	if ( arena != NULL ) {
		deleteResource( "arena" );
		arena = NULL;
	}

	QList<Robot*> old_robots;
	if ( eagents.size() > 0 ) {
		deleteResource( "robot" );
		for( int i=0; i<eagents.size(); i++ ) {
			if (eagents[i]->enabled) {
				deleteResource( eagents[i]->resourcePrefix+"robot" );
				old_robots.push_back(eagents[i]->robot);
				eagents[i]->robot = NULL;
			}
		}
	}

	declareResource( "world", world );

	// Now we can actually free memory
	delete old_arena;
	for (int i = 0; i < old_robots.size(); ++i) {
		delete old_robots[i];
	}
	delete old_world;
}

bool RobotExperiment::recreateRobot( int id ) {
	if (!eagents[id]->enabled) {
		return false;
	}
	eagents[id]->recreateRobot();
	if ( id == agentIdSelected ) {
		// rebind the resource of the robot
		declareResource( "robot",
						eagents[agentIdSelected]->robot,
						eagents[agentIdSelected]->resourcePrefix+"robot" );
	}
	return true;
}

void RobotExperiment::recreateAllRobots() {
	for (int i = 0; i < eagents.size(); i++ ) {
		if (eagents[i]->enabled) {
			recreateRobot(i);
		}
	}
}

void RobotExperiment::recreateArena() {
	// First of all we need to check whether there is an Arena group or not
	if (!ConfigurationHelper::hasGroup( *savedConfigurationParameters, (*savedPrefix) + "Arena" ) ) {
		// This is just to be sure...
		arena = NULL;
		return;
	}

	// to be sure that a World exist
	if ( !world ) {
		recreateWorld();
	}

	// Taking lock because we need to use world
	ResourcesLocker locker(this);
	// Saving the old arena to delete it after the new arena has been created
	Arena* const old_arena = arena;

	// Now creating the arena. We first set ourself as the resouce manager
	savedConfigurationParameters->setResourcesUser(this);
	arena = savedConfigurationParameters->getObjectFromGroup<Arena>((*savedPrefix) + "Arena");
	arena->shareResourcesWith(this);
	QStringList robots;
	foreach(EmbodiedAgent* e, eagents) {
		robots.append(e->resourcePrefix+"robot");
	}
	arena->addRobots(robots);

	// Unlocking before redeclaring the arena resource
	locker.unlock();

	declareResource("arena", static_cast<Resource*>(arena), "world");
	delete old_arena;
}

/*void RobotExperiment::recreateNeuralNetwork( int id ) {
	eagents[id]->recreateNeuralNetwork();
	if ( id == agentIdSelected ) {
		// rebind the resource of the evonet
		declareResource( "evonet",
						static_cast<farsa::ParameterSettable*>(eagents[agentIdSelected]->evonet),
						eagents[agentIdSelected]->resourcePrefix+"evonet" );
	}
}

void RobotExperiment::recreateAllNeuralNetworks() {
	for (int i = 0; i < eagents.size(); i++ ) {
		recreateNeuralNetwork(i);
	}
}*/

RandomGenerator* RobotExperiment::getRNG()
{
	return randomGeneratorInUse;
}

void RobotExperiment::setWorldTimestep( float timestep ) {
	ResourcesLocker locker(this);
	this->timestep = timestep;
	if ( !world ) return;
	world->setTimeStep( timestep );
}

float RobotExperiment::getWorldTimeStep() const {
	return timestep;
}

void RobotExperiment::newGASeed(int seed)
{
	localRNG.setSeed(seed);
}

void RobotExperiment::setStepDelay( int delay ) {
	stepDelay = delay;
}

int RobotExperiment::getStepDelay() {
	return stepDelay;
}

RobotExperiment::EmbodiedAgent::EmbodiedAgent( int id, QString agentPath, RobotExperiment* exp ) {
	this->id = id;
	enabled = true;
	this->agentPath = agentPath;
	this->exp = exp;
	evonet = NULL;
	neuronsIterator = new EvonetIterator();
	robot = NULL;
	resourcePrefix = QString("agent[%1]:").arg(id);
}

void RobotExperiment::EmbodiedAgent::configure() {
	exp->savedConfigurationParameters->setResourcesUser(exp);
	// add to the experiment the resources will create here
	exp->addUsableResource( resourcePrefix+"evonet" );
	exp->addUsableResource( resourcePrefix+"robot" );
	exp->addUsableResource( resourcePrefix+"neuronsIterator" );
	recreateRobot();

	// Reading the sensors parameters. For each sensor there must be a subgroup Sensor:NN where NN is a progressive number
	// (needed to specify the sensors order). Here we also actually create sensors
	QStringList sensorsList = exp->savedConfigurationParameters->getGroupsWithPrefixList(agentPath, "Sensor:");
	sensorsList.sort();
	foreach( QString sensorGroup, sensorsList ) {
		// Setting the prefix for resources that depend on the robot.
		// !! WARNING !! this should only be a temporary implementation, a better implementation is needed
		exp->savedConfigurationParameters->createParameter(
				agentPath+sensorGroup, "__resourcePrefix__", resourcePrefix );
		// !! END OF TRICK !!
		Sensor* sensor = exp->savedConfigurationParameters->getObjectFromGroup<Sensor>(agentPath + sensorGroup);
		if ( sensor == NULL ) {
			Logger::error("Cannot create the Sensor from group " + *(exp->savedPrefix) + sensorGroup + ". Aborting");
			abort();
		}
		// in order to avoid name clash when using more than one Sensor,
		// the Sensors are renamed using the same name of the Group when they don't have a name assigned
		if ( sensor->name() == QString("unnamed") ) {
			sensor->setName( sensorGroup );
		}
		sensors.append( sensor );
		sensors.last()->shareResourcesWith( exp );
		Logger::info( "Created a Sensor named "+sensor->name() );
		// if the user manually set the name and create a name clash, it is only reported as error in Logger
		if ( sensorsMap.contains( sensor->name() ) ) {
			Logger::error( "More than one sensor has name "+sensor->name()+" !! The name has to be unique !!" );
		} else {
			// add to the map
			sensorsMap[sensor->name()] = sensor;
		}
	}

	// Now we do for motors what we did for sensors. Motor groups are in the form Motor:NN
	QStringList motorsList = exp->savedConfigurationParameters->getGroupsWithPrefixList(agentPath, "Motor:");
	motorsList.sort();
	foreach( QString motorGroup, motorsList ) {
		// Setting the prefix for resources that depend on the robot.
		// !! WARNING !! this should only be a temporary implementation, a better implementation is needed
		exp->savedConfigurationParameters->createParameter(
				agentPath+motorGroup, "__resourcePrefix__", resourcePrefix );
		// !! END OF TRICK !!
		Motor* motor = exp->savedConfigurationParameters->getObjectFromGroup<Motor>(agentPath + motorGroup);
		if (motor == NULL) {
			Logger::error("Cannot create the Motor from group " + *(exp->savedPrefix) + motorGroup + ". Aborting");
			abort();
		}
		// in order to avoid name clash when using more than one Motor,
		// the Motors are renamed using the same name of the Group when they don't have a name assigned
		if ( motor->name() == QString("unnamed") ) {
			motor->setName( motorGroup );
		}
		motors.append( motor );
		motors.last()->shareResourcesWith( exp );
		Logger::info( "Created a Motor named "+motor->name() );
		// if the user manually set the name and create a name clash, it is only reported as error in Logger
		if ( motorsMap.contains( motor->name() ) ) {
			Logger::error( "More than one motor has name "+motor->name()+" !! The name has to be unique !!" );
		} else {
			// add to the map
			motorsMap[motor->name()] = motor;
		}
	}

	//recreateNeuralNetwork();
	//exp->declareResource( resourcePrefix+"evonet", static_cast<farsa::ParameterSettable*>(NULL) );

	//exp->declareResource( resourcePrefix+"neuronsIterator", neuronsIterator, resourcePrefix+"evonet" );
}

RobotExperiment::EmbodiedAgent::~EmbodiedAgent() {
	delete robot;
	delete evonet;
	delete neuronsIterator;
	for (int i = 0; i < sensors.size(); i++) {
		delete sensors[i];
	}
	for (int i = 0; i < motors.size(); i++) {
		delete motors[i];
	}
}

void RobotExperiment::EmbodiedAgent::recreateRobot() {
	// to be sure that a World exist
	if ( !(exp->world) ) {
		exp->recreateWorld();
	}

	// Taking lock because we need to use world
	ResourcesLocker locker(exp);
	// Saving the old robot to delete it after the new robot has been created
	Robot* const old_robot = robot;

	// Now creating the robot
	exp->savedConfigurationParameters->setResourcesUser(exp);
	robot = exp->savedConfigurationParameters->getObjectFromGroup<Robot>(agentPath + "ROBOT");

	// Unlocking before redeclaring the robot resource
	locker.unlock();

	exp->declareResource( resourcePrefix+"robot", robot, "world" );
	delete old_robot;
}

/*void RobotExperiment::EmbodiedAgent::recreateNeuralNetwork() {
	// Saving the old evonet to delete it after the new evonet has been created
	Evonet* const old_evonet = evonet;

	// Check the subgroup [NET]
	if ( exp->savedConfigurationParameters->getValue( agentPath+"NET/netFile" ).isEmpty() ) {
		// calculate the number of sensors and motors neurons
		int nSensors = 0;
		foreach( Sensor* sensor, sensors ) {
			nSensors += sensor->size();
		}
		int nMotors = 0;
		foreach( Motor* motor, motors ) {
			nMotors += motor->size();
		}
		// it inject the calculated nSensor and nMotors
		exp->savedConfigurationParameters->createParameter( agentPath+"NET", "nSensors", QString::number(nSensors) );
		exp->savedConfigurationParameters->createParameter( agentPath+"NET", "nMotors", QString::number(nMotors) );
	}
	// Now creating the neural network. We first set ourself as the resouce manager, then we lock resources (because during configuration
	// evonet could use resources, but the resource user it will use is not thread safe (SimpleResourceUser))
	ResourcesLocker locker(exp);
	exp->savedConfigurationParameters->setResourcesUser(exp);
	evonet = exp->savedConfigurationParameters->getObjectFromGroup<Evonet>( agentPath+"NET" );
	evonet->setNetworkName(QString::number(id));
	locker.unlock();

	exp->declareResource( resourcePrefix+"evonet", static_cast<farsa::ParameterSettable*>(evonet) );

	// Here we have to take the lock again because we are going to change neuronsIterator
	locker.lock();
	delete old_evonet;
	neuronsIterator->setEvonet( evonet );
	// create the blocks associated to the network
	int startIndex = 0;
	foreach( Sensor* sensor, sensors ) {
		neuronsIterator->defineBlock( sensor->name(), EvonetIterator::InputLayer, startIndex, sensor->size() );
		startIndex += sensor->size();
	}
	startIndex = 0;
	foreach( Motor* motor, motors ) {
		neuronsIterator->defineBlock( motor->name(), EvonetIterator::OutputLayer, startIndex, motor->size() );
		startIndex += motor->size();
	}
}*/

void RobotExperiment::EmbodiedAgent::disable() {
	if (enabled) {
		enabled = false;

		// Deleting the resource for the robot and then robot
		exp->deleteResource( resourcePrefix+"robot" );
		delete robot;
		robot = NULL;
	}
}

void RobotExperiment::EmbodiedAgent::enable() {
	if (!enabled) {
		enabled = true;

		recreateRobot();
	}
}

} // end namespace farsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
