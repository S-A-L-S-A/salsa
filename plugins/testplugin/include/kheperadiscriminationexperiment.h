/********************************************************************************
 *  FARSA - Total99                                                             *
 *  Copyright (C) 2012-2013 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef KHEPERADISCRIMINATIONEXPERIMENT_H
#define KHEPERADISCRIMINATIONEXPERIMENT_H

#include "farsaplugin.h"
#include "evorobotexperiment.h"
#include "wvector.h"
#include <QVector>

#warning =====> This is a test plugin that is compiled directly in the FARSA tree. We will remove it as soon as the compilation and installation process for FARSA works again <=====

#warning FIX THE farsapluginhelper PREPROCESSOR TO AT LEAST EXTRACT CLASS NAMES OF INHERITED CLASSES REMOVING THE NAMESPACE
using farsa::EvoRobotExperiment;

/**
 * \brief An experiment in which a khepera robot has to discriminate between an
 *        object and the arena walls
 *
 * The resources used by this experiment are the same as the ones of the parent
 * class (EvoRobotExperiment)
 *
 * The parameters for this experiment are:
 * 	- distanceThreshold: the distance from the object below which the robot
 * 	                     is rewarded. This is the distance of the nearest
 * 	                     points of the robot and the object
 * 	- playgroundWidth: the width of the part of the arena surrounded by
 * 	                   walls (the playground is the area where the robot can
 * 	                   move)
 * 	- playgroundHeight: the height of the part of the arena surrounded by
 * 	                    walls (the playground is the area where the robot
 * 	                    can move)
 * 	- minObjectDistanceFromWall: the minimum distance from the walls at
 * 	                             which the object is placed. The default is
 * 	                             0.05
 * 	- minInitialRobotDistanceFromObject: the minimum distance from the
 * 	                                     object at which the robot is
 * 	                                     initialized. The default is 0.1
 * 	- minInitialRobotDistanceFromWall: the minimum distance from the walls
 * 	                                   at which the robot is initialized.
 * 	                                   The default is 0.1
 */
class FARSA_PLUGIN_API KheperaDiscriminationExperiment : public EvoRobotExperiment
{
	Q_OBJECT
	FARSA_REGISTER_CLASS

public:
	/**
	 * \brief Constructor
	 */
	KheperaDiscriminationExperiment(farsa::ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	~KheperaDiscriminationExperiment();

	/**
	 * \brief Configures the object
	 */
	virtual void configure();

	/**
	 * \brief Add to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * It's mandatory in all subclasses where configure and save methods
	 * have been re-implemented for dealing with new parameters and
	 * subgroups to also implement the describe method
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(farsa::RegisteredComponentDescriptor& d);

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 *
	 * See the description of the ConfigurationParameters class for more
	 * information. This method creates the arena and fills it with objects
	 */
	virtual void postConfigureInitialization();

	/**
	 * \brief Called at the begin of each generation
	 *
	 * \param generation the generation about to start
	 */
	virtual void initGeneration(int generation);

	/**
	 * \brief Called before the evaluation of a new individual
	 *
	 * \param individual the id of the individual about to be tested
	 */
	virtual void initIndividual(int individual);

	/**
	 * \brief Called at the beginning of each trial
	 *
	 * \param trial the trial about to start
	 */
	virtual void initTrial(int trial);

	/**
	 * \brief Called at the beginning of each step (before world advance)
	 *
	 * \param step the step about to start
	 */
	virtual void initStep(int step);

	/**
	 * \brief Celled after all sensors have been updated but before network
	 *        spreading
	 *
	 * This is useful, for example, to overwrite the inputs of the neural
	 * network (i.e.: to silence some neurons during the experiment without
	 * modifing sensors classes)
	 */
	virtual void afterSensorsUpdate();

	/**
	 * \brief Called just before updating motors and after updating the
	 *        neural network
	 *
	 * This is useful, for example, to overwrite the outputs of the neural
	 * network
	 */
	virtual void beforeMotorsUpdate();

	/**
	 * \brief Called just before the world advances, after the update of
	 *        motors
	 *
	 * This is useful, for example, to manually actuate motors overriding
	 * the robot controller commands
	 */
	virtual void beforeWorldAdvance();

	/**
	 * \brief Called at the end of each step
	 *
	 * \param step the step about to end
	 */
	virtual void endStep(int step);

	/**
	 * \brief Called at the end of each trial
	 *
	 * \param trial the trial about to end
	 */
	virtual void endTrial(int trial);

	/**
	 * \brief Called at the end of an individual life
	 *
	 * \param individual the individual that has been just tested
	 */
	virtual void endIndividual(int individual);

	/**
	 * \brief Called at the end of each generation
	 *
	 * \param generation the generation about to end
	 */
	virtual void endGeneration(int generation);

private:
	/**
	 * \brief Creates the arena
	 */
	void setupArena();

	/**
	 * \brief Returns the distance between the robot and the object
	 *
	 * \param robot the robot (specified as a parameter so that we don't
	 *              use resources inside this function)
	 * \return the distance between the robot and the object
	 */
	farsa::real robotObjectDistance(farsa::RobotOnPlane* robot) const;

	/**
	 * \brief The thickness of arena walls
	 */
	const farsa::real m_wallThickness;

	/**
	 * \brief The height of objects
	 *
	 * This only has graphical consequences
	 */
	const farsa::real m_objectHeights;

	/**
	 * \brief The maximum number of attemps of placing the robot
	 *
	 * After this number of attemps the simulation is halted with an
	 * exception
	 */
	const unsigned int m_maxPlacingAttempts;

	/**
	 * \brief The width of the part of the arena surrounded by walls
	 *
	 * The playground is the area where the robot can move
	 */
	farsa::real m_playgroundWidth;

	/**
	 * \brief The height of the part of the arena surrounded by walls
	 *
	 * The playground is the area where the robot can move
	 */
	farsa::real m_playgroundHeight;

	/**
	 * \brief The distance from the object below which the robot is rewarded
	 *
	 * This is the distance of the nearest points of the robot and the
	 * object
	 */
	farsa::real m_distanceThreshold;

	/**
	 * \brief The minimum allowed distance of the object from the wall
	 */
	farsa::real m_minObjectDistanceFromWall;

	/**
	 * \brief The minimum allowed initial distance of the robot from the
	 *        object
	 */
	farsa::real m_minInitialRobotDistanceFromObject;

	/**
	 * \brief The minimum allowed initial distance of the robot from the
	 *        walls
	 */
	farsa::real m_minInitialRobotDistanceFromWall;

	/**
	 * \brief The object placed inside the arena
	 */
	farsa::Cylinder2DWrapper* m_object;

	/**
	 * \brief A round circular target area to show where the robot is
	 *        rewarded
	 */
	farsa::Cylinder2DWrapper* m_rewardArea;

	/**
	 * \brief If true re-creates the world at the beginning of the next
	 *        trial
	 */
	bool m_recreateWorld;
};

#endif
