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

#ifndef EVOROBOTEXPERIMENT_H
#define EVOROBOTEXPERIMENT_H

#include "component.h"
#include "evonet.h"
#include "evonetiterator.h"
#include "embodiedagent.h"
#include "robots.h"
#include "world.h"
#include "physphere.h"
#include "phybox.h"
#include "phycylinder.h"
#include "configurationmanager.h"
#include "arena.h"
#include "logger.h"
#include "simpletimer.h"
#include "randomgenerator.h"
#include "experimentsconfig.h"
#include "guirendererscontainer.h"
#include "renderer2d.h"

#include <QCoreApplication>
#include <QVector>
#include <QMap>
#include <QString>
#include <memory>

namespace salsa {

class Evoga;

/**
 * \brief A class representing a list of objects
 *
 * We need this to be able to declare a resource of this type. You can get the
 * resource directly as a QVector\<WObject*\> (so to avoid including this file).
 * All the constructors here directly call the corresponding ones in QVector
 */
class SALSA_EXPERIMENTS_TEMPLATE WObjectsList : public QVector<WObject*>, public Resource
{
public:
	/**
	 * \brief Constructor
	 */
	WObjectsList() :
		QVector<WObject*>()
	{
	}

	/**
	 * \brief Constructor
	 *
	 * Constructs a vector with an initial size of size elements. The
	 * elements are initialized with a default-constructed value.
	 * \param size the initial size of the vector
	 */
	WObjectsList(int size) :
		QVector<WObject*>(size)
	{
	}

	/**
	 * \brief Constructor
	 *
	 * Constructs a vector with an initial size of size elements. Each
	 * element is initialized with value.
	 * \param size the initial size of the vector
	 * \param value the value of all elements in the vector
	 */
	WObjectsList(int size, WObject* const & value) :
		QVector<WObject*>(size, value)
	{
	}

	/**
	 * \brief Copy Constructor
	 *
	 * \param other the vector to copy
	 */
	WObjectsList(const QVector<WObject*>& other) :
		QVector<WObject*>(other)
	{
	}
};

/*! \brief The base common class that evaluate the fitness of a robot
 *
 *  This class don't actually implement any particular scenario or fitness function.
 *  The user must subclass from this, and provide the implementation for the necessary
 *  methods for setting up his scenario and fitness function.
 *
 *  The main assumption that cannot be changed in the subclasses are:
 *  - the robot and the environment are a simulated physic world (Worldsim::World).
 *  - the controller is an Evonet neural network
 *  - the fitness is computed evaluating the robot over a set of trials
 *
 *  This class has two main subcomponents: Agent and Arena (only for 2D simulations)
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
 *  The resources declared by this experiment are:
 *  - "world": the physic world where the robot are simulated
 *  - "arena": if present, the arena with objects for wheeled robots simulations
 *  - "evonet": the Evonet controller of the first agent. This will hopefully be
 *              removed when the Evo* mess is removed or heavily refactored
 */
#warning FOR THE MOMENT I AIM TO HAVE THINGS WORK, HOWEVER WE MUST GET RID OF THE Evo* MESS AS SOON AS POSSIBLE
class SALSA_EXPERIMENTS_API EvoRobotExperiment : public QObject, public Component, public Renderer2D
{
	Q_OBJECT

public:
	static bool configuresInConstructor()
	{
		return false;
	}

public:
	//! Constructor
	EvoRobotExperiment(ConfigurationManager& params);

	//! Destructor
	virtual ~EvoRobotExperiment();

	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 *
	 * This also creates and configures the neural network controlling the
	 * individual
	 */
	virtual void configure();

	/*!
	 * \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 */
	virtual void postConfigureInitialization();

	//! Set the Evoga on which this EvoRobotExperiment is used
	void setEvoga( Evoga* ga );

	//! Return the pointer to the Evoga setted
	Evoga* getEvoga();

	//! Return true if the simulation is running in batch modality, false if is running with the GUI
	bool inBatchRunning() const;

	/*! \brief return the number of agents present in the experiment
	 */
	int getNAgents() const;

	/**
	 * \brief Returns the i-th agent
	 *
	 * \return the i-th agent
	 */
	EmbodiedAgent* getAgent(int i);

	/**
	 * \brief Returns the i-th agent (const version)
	 *
	 * \return the i-th agent
	 */
	const EmbodiedAgent* getAgent(int i) const;

	//! Sets the free parameters of the neural network. This is done for all agents, even disabled ones
	virtual void setNetParameters(float *genes);
	//! Sets the free parameters of the neural network. This is done for all agents, even disabled ones
	virtual void setNetParameters(int *genes);
	//! Called at the beginning of a generation. This function is NEVER called concurrently on different objects
	virtual void initGeneration(int generation);
	//! Called at the beginning of an individual's life
	virtual void initIndividual(int individual);
	//! Called at the beginning of a trial. The default behaviour is to reset the neural network of all agents
	//! (even disabled ones)
	virtual void initTrial(int trial);
	//! Initialize whatever at step granularity when needed
	virtual void initStep( int step );
	//! return the current value of the fitness
	double getFitness();
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

	//! Returns the length of the genome
	int getGenomeLength();

	//! Returns the arena. This can returns nullptr if no arena object is present
	Arena* getArena() {
		return arena;
	}

	//! called by AbstractTest and subclasses to inform which individual is going to test
	virtual void setTestingAgentAndSeed( int idindividual, int nreplica );

	//! this type define the phase of which the EvoRobotExperiment
	enum Phases {
		INTEST,
		INEVOLUTION,
		NONE
	};
	//! return the state of the EvoRobotExperiment
	Phases getActivityPhase() {
		return gaPhase;
	};
	//! set the new activity phase
	void setActivityPhase( Phases newPhase ) {
		gaPhase = newPhase;
	};

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
	 * \brief Returns the renderers container
	 *
	 * \return the renderers container
	 */
	WorldDataUploadeDownloaderSimSide* getRenderersContainerDataUploaderDownloader()
	{
		return renderersContainer;
	}

public slots:
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

	/*! \brief Resets the world
	 *
	 *  This method calls world->reset, effectively destroying every world object. It also
	 *  destroys the arena (call recreateArena afterwards to re-create it). You can configure
	 *  the World after this method accessing to the resource "world". This will also re-declare
	 *  the "world" resource, just to send notifications (even though the world pointer will stay
	 *  the same)
	 *  \warning it will destroy the World with any robot and object inside and
	 *   create a new one without restoring any robot and objects present before.
	 */
	void resetWorld();
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
	double trialFitnessValue;
	//! the fitness value of the individual all over the trials
	double totalFitnessValue;
	//! the error of the individual during the execution of one trial
	double trialErrorValue;
	//! the error of the individual all over the trials
	double totalErrorValue;

private:
	//! Perform one trial
	void doTrial();
	//! carries one step
	void doStep();

	void createWorld();
	void destroyArena();
	void createArena();

	//! the evolutionary algorithm
	Evoga* ga;
	//! the world where robot and objects are simulated
	std::unique_ptr<World> world;
	// ! The renderers container
	GUIRenderersContainer* renderersContainer;
	//! the timestep
	float timestep;
	//! the embodied agents
	QList<EmbodiedAgent*> eagents;
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
	//! The arena with objects for wheeled robot simulations. This can be nullptr
	Arena* arena;
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
};

} // end namespace salsa

#endif
