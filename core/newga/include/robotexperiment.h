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

#ifndef ROBOTEXPERIMENT_H
#define ROBOTEXPERIMENT_H

#include "resourcesuser.h"
#include "evonet.h"
#include "neuroninterfaces.h"
#include "robots.h"
#include "world.h"
#include "physphere.h"
#include "phybox.h"
#include "phycylinder.h"
#include "configurationparameters.h"
#include "parametersettable.h"
#include "arena.h"
#include "logger.h"
#include "simpletimer.h"
#include "randomgenerator.h"
#include "experimentinput.h"
#include "gaevaluator.h"
#include "flowcontrol.h"

#include <QCoreApplication>
#include <QVector>
#include <QMap>
#include <QString>

namespace salsa {

class Evoga;

/*! \brief The base common class that evaluate the fitness of a robot
 *
 *  This class don't actually implement any particular scenario or fitness function.
 *  The user must subclass from this, and provide the implementation for the necessary
 *  methods for setting up his scenario and fitness function.
 *
 *  The main assumption that cannot be changed in the subclasses are:
 *  - the robot and the environment are a simulated physic world (Worldsim::World).
 *  - the controller is a neural network
 *  - the fitness is computed evaluating the robot over a set of trials
 *
 *  The methods that can be re-implemented in order to setup a scenario and a fitness function are:
 *  - initGeneration
 *  - initIndividual
 *  - initTrial
 *  - initStep
 *  - afterSensorsUpdate
 *  - beforeMotorsUpdate
 *  - endStep (this is pure virtual, so it MUST be re-implemented)
 *  - endTrial
 *  - endIndividual
 *  - endGeneration
 *  In the most common cases, you need to re-implment just few of them but they are provided for fullfill almost
 *  any situations. The schema on which they are called during the evolution of individual is the following:
 *  <pre>
 *  foreach generation do
 *      initGeneration
 *      foreach inidividual do
 *          initIndividual
 *          foreach trial do
 *              initTrial
 *              foreach step do
 *                  initStep
 *                  afterSensorsUpdate
 *                  beforeMotorsUpdate
 *                  endStep
 *              endTrial
 *          endIndividual
 *      endGeneration
 *  </pre>
 *
 *  In the most common case
 *
 *  The resources declared by this experiment are:
 *  - "evonet": the neural network
 *  - "neuronsIterator": the Neurons Iterator for iterate over neurons
 *  - "world": the physic world where the robot are simulated
 *  - "robot": the robot
 *  - "arena": if present, the arena with objects for wheeled robots simulations
 */
class SALSA_NEWGA_API RobotExperiment : public OneNeuralNetworkExperimentInput<Evonet>, public SingleFitnessGAEvaluator, public FlowControlled, public ParameterSettableWithConfigureFunction, public ConcurrentResourcesUser
{
public:
	//! Constructor
	RobotExperiment();
	//! Destructor
	virtual ~RobotExperiment();

	//! Return true if the simulation is running in batch modality, false if is running with the GUI
	bool inBatchRunning();
	/*! \brief return the number of agents present in the experiment
	 */
	int getNAgents();
	/*! \brief select the i-the agent and make available the corresponding resources
	 *  \param agentID the id of the agent
	 *
	 *  The resources robot, evonet and neuronsIterator references to the selected agent
	 *
	 *  By default the selected agent is the 0-th, hence in the single robot experiments the
	 *  resources robot, evonet and neuronsIterator points to the only robot present into the world
	 *  \return false if the agent is disabled, true otherwise. If the agent is disabled, no action
	 *          is performed
	 *
	 *  \warning This function internally uses declareResource() to change the robot, evonet and
	 *           neuronsIterator pointed by resources. This means that this can only be called when
	 *           the lock on resources is not held.
	 */
	bool selectAgent( int agentId );
	/**
	 * \brief Enables the i-th agent
	 *
	 * When an agent is enabled, sensors, motors and the neural network are
	 * updated and the robot exists in the world
	 * \param agentId the id of the agent to enable
	 *
	 * \warning This function internally uses declareResource() to declare
	 *          the robot resource, so it can only be called when the lock
	 *          on resources is not held.
	 */
	void enableAgent( int agentId );
	/**
	 * \brief Disables the i-th agent
	 *
	 * When an agent is disnabled, sensors, motors and the neural network
	 * are not updated and the robot is destroyed
	 * \param agentId the id of the agent to disable
	 *
	 * \warning This function internally uses declareResource() to remove
	 *          the robot resource, so it can only be called when the lock
	 *          on resources is not held.
	 */
	void disableAgent( int agentId );
	/**
	 * \brief Returns true if the agent is enabled, false otherwise
	 *
	 * See the description of enableAgent() and disableAgent() for more
	 * information
	 * \param agentId the ID of the agent
	 * \return true if the agent is enabled, false otherwise
	 */
	bool agentEnabled( int agentId );
	/*! Return the neural network used for the experiment. The pointer to the network is returned even if
	 *  the agent is disabled
	 *  \param agentID the id of the agent
	 */
	Evonet* getNeuralNetwork( int agentId=0 );
	/**
	 * \brief Sets the neural network used for the experiment.
	 *        It modifies the current network with the passed one.
	 *        It is worth noting that the experiment is responsible
	 *        for the deletion of the neural network (i.e., both the
	 *        old net and the passed one)
	 * \param net the neural network
	 */
	void setNeuralNetwork(const Evonet* net);
	//! Called at the beginning of a generation. This function is NEVER called concurrently on different objects
	virtual void initGeneration(int generation);
	//! Called at the beginning of an individual's life
	virtual void initIndividual(int individual);
	//! Called at the beginning of a trial. The default behaviour is to reset the neural network of all agents
	//! (even disabled ones)
	virtual void initTrial(int trial);
	//! Initialize whatever at step granularity when needed
	virtual void initStep( int step );
	//! perform the evaluation
	void evaluate();
	//! return the current value of the fitness
	real getFitness();
	//! return the current value of the error
	double getError();
	/**
	 * \brief Stops the current trial
	 *
	 * Call this function to stop the current trial and move to the next trial (if present). This function
	 * can only be called initStep() or endStep(). If you call this from initStep(), the functions
	 * afterSensorsUpdate(), beforeMotorsUpdate() and endStep() are also called before terminating the trial.
	 * After endStep(), execution continues with endTrial() as if the trial ended normally. This also means
	 * that trialFitnessValue is taken into account (unless you overrided the endTrial() function, in which
	 * case the behaviour depends on your implementation). You should never mix calls to this functions with
	 * calls to skipTrial(), restartTrial() or endIndividualLife() during the same trial.
	 */
	void stopTrial();
	/**
	 * \brief Skips the next trial
	 *
	 * Call this function to skip the execution of the next trial. This function can only be called from
	 * initTrial(). If you call this function, after initTrial(), execution continues with initTrial() (if
	 * there are more trials) or with endIndividual() (if there are no more trials to do). Of course no
	 * fitness is assigned for the skipped trials. You should never mix calls to this functions with calls
	 * to stopTrial(), restartTrial() or endIndividualLife() during the same trial.
	 */
	void skipTrial();
	/**
	 * \brief Stops the current trial and restarts it from scratch
	 *
	 * Call this function to stop the current trial and restart it from scratch. The current fitness value
	 * is discarded, too. You can call this function only from initStep() or endStep(). After the end of
	 * the function from which this is called, the trial is restarted and initTrial() is called. In particular
	 * no call to endTrial() is performed in any case and, if you call this function from initStep(), the
	 * functions afterSensorsUpdate(), beforeMotorsUpdate() and endStep() for the current step are not called.
	 * You should never mix calls to this functions with calls to stopTrial(), skipTrial() or endIndividualLife()
	 * during the same trial.
	 */
	void restartTrial();
	/**
	 * \brief Ends the invidual life
	 *
	 * Call this function to end the life of the individual. This can only be called from initTrial(),
	 * initStep(), endStep() and endTrial(). When you call this function the current trial is stopped as if
	 * stopTrial() was called, so everthing said for the stopTrial() function also applies to this function
	 * when called inside initStep() or endStep(). If you call this function from initTrial() the trial is
	 * terminated immediately without calling any step function. After returning from initTrial(), endTrial()
	 * is called. In all cases the fitness for the last trial is taken into account. After the call to endTrial()
	 * endIndividual() is called as if the individual life ended normally. You should never mix calls to this
	 * functions with calls to stopTrial(), skipTrial() or restartTrial() during the same trial.
	 */
	void endIndividualLife();
	/*! Called just after the updating of sensors and before the updating of the neural network
	 *  \note useful, for example, to overwrite the inputs of the neural network
	 *        (i.e.: to silence some neurons during the experiment withouth modifing sensors classes)
	 */
	virtual void afterSensorsUpdate();
	/*! Called just before the updating of motors and after the updating of the neural network
	 *  \note useful, for example, to overwrite the outputs of the neural network
	 */
	virtual void beforeMotorsUpdate();
	/*! Called just before the world advances, after the update of motors
	 *  \note useful, for example, to manually actuate motors overriding the robot controller commands
	 */
	virtual void beforeWorldAdvance();
	//! Called at the end of the step
	virtual void endStep( int step ) = 0;
	/*! Called at the end of a trial
	 * \note the default implementation will add the current trialFitnessValue to totalFitnessValue
	 */
	virtual void endTrial(int trial);
	/*! Called at the end of an individual's life
	 */
	virtual void endIndividual(int individual);
	//! Called at the end of a generation. This function is NEVER called concurrently on different objects
	virtual void endGeneration(int generation);
	//! Performs all trials for the given individual
	void doAllTrialsForIndividual(int individual);
	
	int getRequestedSensors() const;
	
	int getRequestedMotors() const;

	void setIndividualCounter();

	void resetIndividualCounter();

	/*! Return a new instance of the RobotExperimentUI
     */
    ParameterSettableUI* getUIManager();

	/*! Return a pointer to the Sensor with name specified
	 *  \param name is the name of the sensor your are looking for
	 *  \param agentID the id of the agent
	 *  \return a pointer to the sensor or NULL is the sensor doesn't exists
	 *          or the agent is disabled
	 */
	Sensor* getSensor( QString sensorName, int agentId=0 );
	/*! Return a pointer to the Motor with name specified
	 *  \param name is the name of the motor your are looking for
	 *  \param agentID the id of the agent
	 *  \return a pointer to the motor or NULL is the sensor doesn't exists
	 *          or the agent is disabled
	 */
	Motor* getMotor( QString motorName, int agentId=0 );

	//! Returns the arena. This can returns NULL if no arena object is present
	Arena* getArena() {
		return arena;
	}

	//! called by AbstractTest and subclasses to inform which individual is going to test
	virtual void setTestingAgentAndSeed( int idindividual, int nreplica );

	//! this type define the phase of which the RobotExperiment
	enum Phases {
		INTEST,
		INEVOLUTION,
		NONE
	};
	//! return the state of the RobotExperiment
	Phases getActivityPhase() {
		return gaPhase;
	};
	//! set the new activity phase
	void setActivityPhase( Phases newPhase ) {
		gaPhase = newPhase;
	};

	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 *
	 * This also creates and configures the neural network controlling the
	 * individual
	 * \param params the configuration parameters object with parameters to
	 *               use
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters. This is guaranteed to end with the
	 *               separator character when called by the factory, so you
	 *               don't need to add one
	 */
	virtual void configure(ConfigurationParameters& params, QString prefix);

	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters
	 *        object passed
	 *
	 * This is not implemented, a call to this function will cause an abort
	 * \param params the configuration parameters object on which save actual
	 *               parameters
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters.
	 */
	virtual void save(ConfigurationParameters& params, QString prefix);

	/*!
	 * \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups
	 * \param type is the name of the type regarding the description. The type is used when a subclass
	 * reuse the description of its parent calling the parent describe method passing the type of
	 * the subclass. In this way, the result of the method describe of the parent will be the addition
	 * of the description of the parameters of the parent class into the type of the subclass
	 */
	static void describe( QString type );

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 */
	virtual void postConfigureInitialization();

	/**
	 * \brief Returns the number of trials
	 *
	 * \return the number of trials
	 */
	int getNTrials() const
	{
		return ntrials;
	}
	/**
	 * \brief Sets the number of trials
	 *
	 * \param ntrials the new number of trials
	 */
	void setNTrials( int new_ntrials ) {
		ntrials = new_ntrials;
	}

	/**
	 * \brief Returns the number of steps
	 *
	 * \return the number of steps
	 */
	int getNSteps() const
	{
		return nsteps;
	}
	/**
	 * \brief Sets the number of steps
	 *
	 * \param nsteps the new number of steps
	 */
	void setNSteps( int new_nsteps ) {
		nsteps = new_nsteps;
	}

	/**
	 * \brief Returns the current step
	 *
	 * \return the current step
	 */
	int getCurStep() const
	{
		return nstep;
	}

	/**
	 * \brief Returns the current trial
	 *
	 * \return the current trial
	 */
	int getCurTrial() const
	{
		return ntrial;
	}

	/**
	 * \brief Called by the ga when the seed changes (e.g. in a new
	 *        replication)
	 *
	 * \param seed the new seed
	 */
	void newGASeed(int seed);

	/**
	 * \brief Added by Jonata for the SpecializerSteadyState GA
	 *
	 * This filled with the weights in the tests.cpp file
	 */
	QVector<int*> m_weightIndividual;

	/*! \brief set the delay to apply at each step for slowing down the simulation
	 *  \param delay the delay expressed in msec
	 *  \note if the delay is equal to the timestep the simulation will run at real-time
	 */
	void setStepDelay( int delay );

	/*! \brief returns the current delay applied at each step */
	int getStepDelay();

	//! helper method for getting timestep of the world at runtime
	float getWorldTimeStep() const;

protected:
	int ntrials;			//! number of test
	int nsteps;			//! number of cycles
	int nstep;			//! current step
	int ntrial;			//! current trial

	/*! \brief Recreate the world
	 *
	 *  This method helps in the creation of the World where the robot and the objects are simulated.
	 *  It's strongly advised to use this method instead of creating a World by yourself.
	 *  You can configure the World after this method accessing to the resource "world".
	 *  \warning it will destroy the World with any robot and object inside and
	 *   create a new one without restoring any robot and objects present before.
	 */
	void recreateWorld();
	/*! \brief Recreate the robot
	 *  \param agentID the id of the agent having the robot
	 *
	 *  This method helps in the creation of the Robot.
	 *  It's strongly advised to use this method instead of creating a robot by yourself.
	 *  You can configure the robot after having called this method accessing the resource "robot".
	 *
	 *  In case of collective experiment, the agentId allow to specify which robot needs to be recreated
	 *  \return false if the agent is disabled, true otherwise. If the agent is disabled, the robot is not
	 *          created
	 *
	 *  \warning it will destroy the robot and create a new one.
	 */
	bool recreateRobot( int agentId = 0 );
	/*! \brief Recreates all robots
	 *
	 *  This method simply calls recreateRobot() for all enabled agents
	 *
	 *  \warning it will destroy all robots and create a new ones.
	 */
	void recreateAllRobots();
	/*! \brief Recreates the arena
	 *
	 *  This methods creates an arena if the group Arena is in the ConfigurationParameters
	 *  \warning this destroys the current arena and creates a new one
	 */
	void recreateArena();
	/*! \brief Recreate the neural network
	 *  \param agentID the id of the agent having the neural network
	 *
	 *  This method helps in the creation of the Neural Network (only Evonet for now).
	 *  It's strongly advised to use this method instead of creating a neural network by yourself.
	 *  You can configure the neural network after this method accessing to the resource "evonet".
	 *
	 *  In case of collective experiment, the agentId allow to specify which neural network needs to be recreated
	 *  \warning it will destroy the neural network and create a new one.
	 *  \note This works also on disabled agents
	 */
	//void recreateNeuralNetwork( int agentId = 0 );
	/*! \brief Recreates all neural networks
	 *
	 *  This method simply calls recreateNeuralNetwork() for all agents' neural networks (even disabled ones)
	 *
	 *  \warning it will destroy the neural network and create a new one.
	 */
	//void recreateAllNeuralNetworks();

	/*! \brief Returns the random generator
	 *
	 *  You should use this function in experiments instead of using salsa::globalRNG because this
	 *  returns the correct generator depending on whether the same random sequence should be generated
	 *  for all individuals of the same generation or not. The actual seed for each individual in a given
	 *  generation depends on the seed of the genetic algorithm, the current generation and the total number
	 *  of replications (keep this in mind if you want to exactly replicate an experiment)
	 *  \return the random generator
	 */
	RandomGenerator* getRNG();

	//! the fitness value of the individual during the execution of one trial
	real trialFitnessValue;
	//! the fitness value of the individual all over the trials
	real totalFitnessValue;
	//! the error of the individual during the execution of one trial
	double trialErrorValue;
	//! the error of the individual all over the trials
	double totalErrorValue;

	//--- it's not anymore const, because createNeuralNetwork may modify some [NET]
	//    parameters; in particual nSensors, nHiddens and nMotors
	ConfigurationParameters* savedConfigurationParameters;
	const QString* savedPrefix;

private:
	//! Perform one trial
	void doTrial();
	//! carries one step
	void doStep();

	//! helper method for setting timestep of the world at runtime
	void setWorldTimestep( float timestep );

	//! this class encapsulate all data concerning an embodied agent
	class EmbodiedAgent {
	public:
		//! constructor
		EmbodiedAgent( int id, QString agentPath, RobotExperiment* exp );
		//! destructor
		~EmbodiedAgent();
		//! create the agent from the configuration file
		void configure();
		//! recreate the robot
		void recreateRobot();
		//! recreate the neural network
		//void recreateNeuralNetwork();
		//! disables the agent. This means that the sensors, motors and
		//! neural network are no longer updated and that the robot is
		//! destroyed. The resource for the agent robot is also deleted
		void disable();
		//! enables the agent. This means that the sensors, motors and
		//! neural network are updated and that the robot is created.
		//! The resource for the agent robot is also created
		void enable();
		//! the id of the agent (unique id)
		int id;
		//! whether the robot is enabled or not (it is created enabled)
		bool enabled;
		//! the group on which the configuration is located
		QString agentPath;
		//! prefix string for resource naming
		QString resourcePrefix;
		//! neural controller class
		Evonet* evonet;
		//! the Evonet neuron iterator for Sensors and Motors
		EvonetIterator* neuronsIterator;
		//! icub robot in simulation
		Robot* robot;
		//! list of sensor classes to be created and used
		QVector<Sensor*> sensors;
		//! list of motor classes to be created and used
		QVector<Motor*> motors;
		//! Map name -> sensor for fast looking based on name
		QMap<QString, Sensor*> sensorsMap;
		//! Map name -> motor for fast looking based on name
		QMap<QString, Motor*> motorsMap;
		//! pointer to the RobotExperiment owning this object
		RobotExperiment* exp;
	};

	//! the world where robot and objects are simulated
	World* world;
	//! the timestep
	float timestep;
	//! the embodied agents
	QList<EmbodiedAgent*> eagents;
	//! current selected agent
	int agentIdSelected;
	//! current activity (evolution, test, batch)
	Phases gaPhase;
	//! If true stops the current trial
	bool stopCurrentTrial;
	//! If true the current trial is skipped
	bool skipCurrentTrial;
	//! If true the current trial is discarded and restarted from scratch
	bool restartCurrentTrial;
	//! If true ends the life of the current individual
	bool endCurrentIndividualLife;
	//! whether we are running in batch or not
	bool batchRunning;
	//! The arena with objects for wheeled robot simulations. This can be NULL
	Arena* arena;
	//! The neural network
	Evonet* evonet;
	/*! a delay expressed in msec to apply at each step for slowing down the simulation
	 *  It's only used when the GUI is active and not in batching mode
	 */
	int stepDelay;
	/*! Whether the generated random number sequence should be the same for
	 *  all individuals in the same generation or not (default not)
	 */
	bool sameRandomSequence;
	/*! The pointer to the random that getRNG returns
	 *
	 *  This points either to salsa::globalRNG or to localRNG
	 */
	RandomGenerator* randomGeneratorInUse;
	/*! A local random generator used if sameRandomSequence is true */
	RandomGenerator localRNG;

	int curInd;
};

} // end namespace salsa

#endif
