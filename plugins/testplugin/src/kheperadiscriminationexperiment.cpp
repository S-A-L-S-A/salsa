/********************************************************************************
 *  SALSA - Total99                                                             *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "kheperadiscriminationexperiment.h"
#include "utilitiesexceptions.h"
#include "randomgenerator.h"
#include "robots.h"
#include "world.h"
#include "phybox.h"
#include "wheeledexperimenthelper.h"
#include "logger.h"
#include "configurationhelper.h"
#include "mathutils.h"

KheperaDiscriminationExperiment::KheperaDiscriminationExperiment(salsa::ConfigurationManager& params)
	: salsa::EvoRobotExperiment(params)
	, m_wallThickness(0.03f)
	, m_objectHeights(0.05f)
	, m_maxPlacingAttempts(100)
	, m_playgroundWidth(0.5f)
	, m_playgroundHeight(0.5f)
	, m_distanceThreshold(0.1f)
	, m_minObjectDistanceFromWall(0.05f)
	, m_minInitialRobotDistanceFromObject(0.1f)
	, m_minInitialRobotDistanceFromWall(0.1f)
	, m_object(NULL)
	, m_rewardArea(NULL)
	, m_recreateWorld(false)
{
}

KheperaDiscriminationExperiment::~KheperaDiscriminationExperiment()
{
}

void KheperaDiscriminationExperiment::configure()
{
	// Calling parent function
	EvoRobotExperiment::configure();

	// Loading our parameters. Checks on m_playgroundWidth and m_playgroundHeight will be done in setupArena() because
	// we need a valid object
	m_playgroundWidth = salsa::ConfigurationHelper::getReal(configurationManager(), confPath() + "playgroundWidth");
	m_playgroundHeight = salsa::ConfigurationHelper::getReal(configurationManager(), confPath() + "playgroundHeight");
	m_distanceThreshold = salsa::ConfigurationHelper::getReal(configurationManager(), confPath() + "distanceThreshold");
	m_minObjectDistanceFromWall = salsa::ConfigurationHelper::getReal(configurationManager(), confPath() + "minObjectDistanceFromWall");
	m_minInitialRobotDistanceFromObject = salsa::ConfigurationHelper::getReal(configurationManager(), confPath() + "minInitialRobotDistanceFromObject");
	m_minInitialRobotDistanceFromWall = salsa::ConfigurationHelper::getReal(configurationManager(), confPath() + "minInitialRobotDistanceFromWall");
}

void KheperaDiscriminationExperiment::describe(salsa::RegisteredComponentDescriptor& d)
{
	// Calling parent function
	salsa::EvoRobotExperiment::describe(d);

	d.help("The experiment in which a khepera robot has to discriminate between an object in the arena and the arena walls");

	d.describeReal("playgroundWidth").def(0.5).limits(0.0, +salsa::Infinity).help("The width of the part of the arena surrounded by walls", "This is the width of the playground. The playground is the area where the robot can move");
	d.describeReal("playgroundHeight").def(0.5).limits(0.0, +salsa::Infinity).help("The height of the part of the arena surrounded by walls", "This is the height of the playground. The playground is the area where the robot can move");
	d.describeReal("distanceThreshold").def(0.1).limits(0.0, +salsa::Infinity).help("The distance from the object below which the robot is rewarded", "If the distance of the robot from the object is less than this value, the robot is rewarded for the current timestep. This is the distance of the nearest points of the robot and the object (i.e. the robot and object radii are subtracted from the distance between the centers)");
	d.describeReal("minObjectDistanceFromWall").def(0.05).limits(0.0, +salsa::Infinity).help("The minimum distance of the object from walls", "This is the minimum distance from the walls at which the object is placed. The default is 0.05");
	d.describeReal("minInitialRobotDistanceFromObject").def(0.1).limits(0.0, +salsa::Infinity).help("The minimum initial distance of the robot from the object", "This is the minimum distance from the object at which the robot is initialized. The default is 0.1");
	d.describeReal("minInitialRobotDistanceFromWall").def(0.1).limits(0.0, +salsa::Infinity).help("The minimum initial distance of the robot from walls", "This is the minimum distance from the walls at which the robot is initialized. The default is 0.1");
}

void KheperaDiscriminationExperiment::postConfigureInitialization()
{
	// Calling parent function
	EvoRobotExperiment::postConfigureInitialization();

	// Here we create the arena and the object
	setupArena();
}

void KheperaDiscriminationExperiment::initGeneration(int /*generation*/)
{
}

void KheperaDiscriminationExperiment::initIndividual(int /*individual*/)
{
	// Resetting the world to avoid numerical problems
	m_recreateWorld = true;
}

void KheperaDiscriminationExperiment::initTrial(int /*trial*/)
{
	if (m_recreateWorld) {
		resetWorld();
		recreateAllRobots();
		recreateArena();

		setupArena();

		m_recreateWorld = false;
	}

	salsa::Arena* arena = getResource<salsa::Arena>("arena");

	// First of all moving the object to a random position in the arena
	const salsa::real arenaWidthHalfLimitForObject = m_playgroundWidth / 2.0 - m_object->phyObject()->radius() - m_minObjectDistanceFromWall;
	const salsa::real arenaHeightHalfLimitForObject = m_playgroundHeight / 2.0 - m_object->phyObject()->radius() - m_minObjectDistanceFromWall;
	const salsa::real ox = salsa::globalRNG->getDouble(-arenaWidthHalfLimitForObject, arenaWidthHalfLimitForObject);
	const salsa::real oy = salsa::globalRNG->getDouble(-arenaHeightHalfLimitForObject, arenaHeightHalfLimitForObject);
	m_object->setPosition(ox, oy);
	m_rewardArea->setPosition(ox, oy);

	// Now placing the robot
	salsa::RobotOnPlane* robot = getResource<salsa::RobotOnPlane>("robot");
	unsigned int numAttempts = 0;
	do {
		if (numAttempts > m_maxPlacingAttempts) {
			salsa::throwUserRuntimeError("Unable to place the robot (more than " + QString::number(m_maxPlacingAttempts) + " attempts made)");
		}
		numAttempts++;

		const salsa::real arenaWidthHalfLimitForRobot = m_playgroundWidth / 2.0 - robot->robotRadius() - m_minInitialRobotDistanceFromWall;
		const salsa::real arenaHeightHalfLimitForRobot = m_playgroundHeight / 2.0 - robot->robotRadius() - m_minInitialRobotDistanceFromWall;
		const salsa::real rx = salsa::globalRNG->getDouble(-arenaWidthHalfLimitForRobot, arenaWidthHalfLimitForRobot);
		const salsa::real ry = salsa::globalRNG->getDouble(-arenaHeightHalfLimitForRobot, arenaHeightHalfLimitForRobot);
		robot->setPosition(arena->getPlane(), rx, ry);
	} while (robotObjectDistance(robot) < m_minInitialRobotDistanceFromObject);

	// Changing also the robot orentation
	robot->setOrientation(arena->getPlane(), salsa::globalRNG->getDouble(-PI_GRECO, PI_GRECO));

	// Resetting fitness for the current trial
	trialFitnessValue = 0;
}

void KheperaDiscriminationExperiment::initStep(int /*step*/)
{
	// Checking we don't have any NaN. We just check a value in a matrix because NaN spreads rapidly.
	// If there is a NaN, we simply restart the trial from scratch.
	salsa::RobotOnPlane* robot = getResource<salsa::RobotOnPlane>("robot");
	const salsa::real valueToCheck = robot->position().x;
	if (isnan(valueToCheck) || isinf(valueToCheck)) {
		salsa::Logger::warning("Found a NaN value, recreating world and restarting trial from scratch");

		restartTrial();
		m_recreateWorld = true;

		return;
	}
}

void KheperaDiscriminationExperiment::afterSensorsUpdate()
{
}

void KheperaDiscriminationExperiment::beforeMotorsUpdate()
{
}

void KheperaDiscriminationExperiment::beforeWorldAdvance()
{
}

void KheperaDiscriminationExperiment::endStep(int /*step*/)
{
	salsa::RobotOnPlane* robot = getResource<salsa::RobotOnPlane>("robot");
	const salsa::Arena* arena = getResource<salsa::Arena>("arena");

	// If robot collided with something, stopping the trial
	if (arena->getKinematicRobotCollisionsSet(salsa::Arena::RobotResource("robot", getAgent(0))).size() != 0) {
		stopTrial();

		return;
	}

	// Computing the distance of the robot with the object
	const salsa::real distance = robotObjectDistance(robot);

	if (distance < m_distanceThreshold) {
		trialFitnessValue += 1.0;
	}
}

void KheperaDiscriminationExperiment::endTrial(int /*trial*/)
{
	totalFitnessValue += trialFitnessValue / salsa::real(getNSteps());
}

void KheperaDiscriminationExperiment::endIndividual(int /*individual*/)
{
	totalFitnessValue = totalFitnessValue / salsa::real(getNTrials());
}

void KheperaDiscriminationExperiment::endGeneration(int /*generation*/)
{
}

void KheperaDiscriminationExperiment::setupArena()
{
	// Getting the arena
	salsa::Arena* arena = getResource<salsa::Arena>("arena");

	// Creating walls all around the arena
	const salsa::real halfHeight = m_playgroundHeight / 2.0;
	const salsa::real halfWidth = m_playgroundWidth / 2.0;
	const salsa::real topWallPos = halfHeight + m_wallThickness / 2.0;
	const salsa::real bottomWallPos = -(halfHeight + m_wallThickness / 2.0);
	const salsa::real rightWallPos = halfWidth + m_wallThickness / 2.0;
	const salsa::real leftWallPos = -(halfWidth + m_wallThickness / 2.0);
	arena->createWall(Qt::yellow, salsa::wVector(-halfWidth, topWallPos, 0.0), salsa::wVector(halfWidth, topWallPos, 0.0), m_wallThickness, m_objectHeights);
	arena->createWall(Qt::yellow, salsa::wVector(-halfWidth, bottomWallPos, 0.0), salsa::wVector(halfWidth, bottomWallPos, 0.0), m_wallThickness, m_objectHeights);
	arena->createWall(Qt::yellow, salsa::wVector(rightWallPos, -halfHeight, 0.0), salsa::wVector(rightWallPos, halfHeight, 0.0), m_wallThickness, m_objectHeights);
	arena->createWall(Qt::yellow, salsa::wVector(leftWallPos, -halfHeight, 0.0), salsa::wVector(leftWallPos, halfHeight, 0.0), m_wallThickness, m_objectHeights);

	// Now creating the object
	m_object = arena->createSmallCylinder(Qt::yellow, m_objectHeights);
	m_object->setStatic(true);

	// We also add a round target area to show the area in which the robot is rewarded
	m_rewardArea = arena->createCircularTargetArea(m_object->phyObject()->radius() + m_distanceThreshold, Qt::cyan);

	// Finally checking if the arena is big enough. We only check against the robot radius because it is bigger than the object radius
	salsa::RobotOnPlane* robot = getResource<salsa::RobotOnPlane>("robot");
	const salsa::real minDimension = robot->robotRadius() + m_minObjectDistanceFromWall + m_minInitialRobotDistanceFromObject + m_minInitialRobotDistanceFromWall;
	if (m_playgroundWidth < minDimension) {
		salsa::throwUserRuntimeError("Cannot run the experiment because the arena is too small (width too small)");
	} else if (m_playgroundHeight < minDimension) {
		salsa::throwUserRuntimeError("Cannot run the experiment because the arena is too small (height too small)");
	}
}

salsa::real KheperaDiscriminationExperiment::robotObjectDistance(salsa::RobotOnPlane* robot) const
{
	// Computing the distance of the robot with the object
	const salsa::wVector robotPosition(robot->position().x, robot->position().y, 0.0);
	const salsa::wVector objectPosition(m_object->position().x, m_object->position().y, 0.0);
	const salsa::real distance = (robotPosition - objectPosition).norm() - robot->robotRadius() - m_object->phyObject()->radius();

	return distance;
}
