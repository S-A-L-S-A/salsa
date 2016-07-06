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

#include "arena.h"
#include "configurationhelper.h"
#include "phybox.h"
#include "logger.h"
#include "robots.h"
#include "utilitiesexceptions.h"
#include "randomgenerator.h"

namespace farsa {

// This anonymous namespace contains some constants used throughout the code
namespace {
	/**
	 * \brief The default height for objects
	 *
	 * \internal
	 */
	const real defaultHeight = 0.3f;

	/**
	 * \brief The thickness of the box modelling the plane
	 *
	 * \internal
	 */
	const real planeThickness = 0.1f;

	/**
	 * \brief The default radius of small cylinders
	 *
	 * \internal
	 */
	const real defaultSmallCylinderRadius = 0.0125f;

	/**
	 * \brief The default radius of big cylinders
	 *
	 * \internal
	 */
	const real defaultBigCylinderRadius = 0.027f;

	/**
	 * \brief The height of target areas
	 *
	 * \internal
	 */
	const real targetAreasHeight = 0.01f;

	/**
	 * \brief How much target areas protrude from the ground
	 *
	 * \internal
	 */
	const real targetAreasProtrusion = 0.005f;

	/**
	 * \brief The default radius of light bulbs
	 *
	 * \internal
	 */
	const real defaultLightBulbRadius = 0.01f;
}

Arena::KinematicCollisionHandlers Arena::stringToCollisionHandler(QString str)
{
	str = str.toLower();

	if (str == "simplecollisions") {
		return SimpleCollisions;
	} else if (str == "circlecollisions") {
		return CircleCollisions;
	} else {
		return UnknownCollisionHandler;
	}
}

QString Arena::collisionHandlerToString(Arena::KinematicCollisionHandlers h)
{
	switch (h) {
		case SimpleCollisions:
			return "SimpleCollisions";
		case CircleCollisions:
			return "CircleCollisions";
		case UnknownCollisionHandler:
		default:
			return "UnknownCollisionHandler";
	}
}

Arena::Arena(ConfigurationManager& params)
	: Component(params)
	, m_z(ConfigurationHelper::getReal(configurationManager(), confPath() + "z"))
	, m_smallCylinderRadius(ConfigurationHelper::getReal(configurationManager(), confPath() + "smallCylinderRadius"))
	, m_bigCylinderRadius(ConfigurationHelper::getReal(configurationManager(), confPath() + "bigCylinderRadius"))
	, m_lightBulbRadius(ConfigurationHelper::getReal(configurationManager(), confPath() + "lightBulbRadius"))
	, m_collisionHandler(stringToCollisionHandler(ConfigurationHelper::getEnum(configurationManager(), confPath() + "collisionHandler")))
	, m_objects2DList(QVector<PhyObject2DWrapper*>())
	, m_plane(createPlane(m_z))
	, m_robotResourceWrappers()
	, m_kinematicRobotCollisions()
	, m_world(NULL)
{
	addNotifiedResource("world");

	// Also getting world directly, because we could be used in the configuration phase and we need world
	m_world = getResource<World>("world");
}

Arena::~Arena()
{
	// Removing all wrappers
	for (int i = 0; i < m_objects2DList.size(); i++) {
		delete m_objects2DList[i];
	}
}

void Arena::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The class modelling an arena in which robots can live");

	d.describeReal("z").def(0.0).help("The z coordinate of the plane in the world frame of reference", "This is the z coordinate of the upper part of the plane in the world frame of reference");
	d.describeReal("smallCylinderRadius").def(defaultSmallCylinderRadius).limits(0.0001, +Infinity).help("The radius of small cylinders", "This is the value to use for the radius of small cylinders. The default value is the one used in evorobot. If you change this, be careful which sample files you use.");
	d.describeReal("bigCylinderRadius").def(defaultBigCylinderRadius).limits(0.0001, +Infinity).help("The radius of big cylinders", "This is the value to use for the radius of big cylinders. The default value is the one used in evorobot. If you change this, be careful which sample files you use.");
	d.describeReal("lightBulbRadius").def(defaultLightBulbRadius).limits(0.0001, +Infinity).help("The radius of light bulbs");
	d.describeEnum("collisionHandler").def("SimpleCollisions").values(QStringList() << "SimpleCollisions" << "CircleCollisions").help("The type of collision handler to use", "This can be either SimpleCollisions (which puts robots back to their position before collision) or CircleCollisions (which handles collisions between circular objects like robots and cylinders computing the results based on objects masses)");
	d.describeReal("planeWidth").def(2.5).limits(0.01, +Infinity).help("The width of the arena");
	d.describeReal("planeHeight").def(2.5).limits(0.01, +Infinity).help("The height of the arena");
}

void Arena::addRobots(RobotsList robots)
{
	// First of all adding robots to the list of usable resources. This will call the resourceChanged callback
	// because robots already exist, however they are not in the m_robotResourceWrappers map so the callback will
	// do nothing (we take care of creating the wrappers here)
	foreach(RobotResource robot, robots) {
		addNotifiedResource(robot.first, robot.second);
	}

	foreach(RobotResource robot, robots) {
		const QString& robotResourceName = robot.first;
		Component* const robotOwner = robot.second;
		const bool robotExists = resourceExists<RobotOnPlane>(robotResourceName, robotOwner);

		// The first thing to do is to check if the robot existed: in that case we have to first delete
		// the old wrapper
		if (m_robotResourceWrappers.contains(robot)) {
			// This will probably crash if the robot is not in the list (this would be a bug anyway)
			m_objects2DList.remove(m_objects2DList.indexOf(m_robotResourceWrappers[robot]));

			// Deleting the wrapper
			delete m_robotResourceWrappers[robot];
		}

		// If the robot actually exists, adding a wrapper, otherwise we simply add the robot to the map to
		// remember the association
		if (robotExists) {
			RobotOnPlane* r = getResource<RobotOnPlane>(robotResourceName, robotOwner);

			// Creating the wrapper
			WheeledRobot2DWrapper* wrapper = new WheeledRobot2DWrapper(this, r, r->robotHeight(), r->robotRadius());

			// Adding the wrapper to the list
			m_objects2DList.push_back(wrapper);
			m_robotResourceWrappers.insert(robot, wrapper);
		} else {
			// Adding the robot to the map with a NULL wrapper
			m_robotResourceWrappers.insert(robot, NULL);
		}
	}
}

const WheeledRobot2DWrapper* Arena::getRobotWrapper(RobotResource robotResource) const
{
	if (m_robotResourceWrappers.contains(robotResource)) {
		return m_robotResourceWrappers[robotResource];
	} else {
		return NULL;
	}
}

Box2DWrapper* Arena::getPlane()
{
	return m_plane;
}

const Box2DWrapper* Arena::getPlane() const
{
	return m_plane;
}

Box2DWrapper* Arena::createWall(QColor color, wVector start, wVector end, real thickness, real height)
{
	// Changing parameters
	start.z = m_z;
	end.z = m_z;

	// Creating the box, then we have to change its position and make it static
	Box2DWrapper* b = createBox(color, (end - start).norm(), thickness, height, Box2DWrapper::Wall);

	// Moving the wall and setting the texture. We have to compute the rotation around the Z axis of the
	// wall and the position of its center to build its transformation matrix
	const real angle = atan2(end.y - start.y, end.x - start.x);
	const wVector centerPosition = start + (end - start).scale(0.5);
	wMatrix mtr = wMatrix::roll(angle);
	mtr.w_pos = centerPosition + wVector(0.0, 0.0, b->phyObject()->matrix().w_pos.z);
	b->phyObject()->setMatrix(mtr);
	b->setTexture("tile2");

	return b;
}

Cylinder2DWrapper* Arena::createSmallCylinder(QColor color, real height)
{
	return createCylinder(color, m_smallCylinderRadius, height, Cylinder2DWrapper::SmallCylinder);
}

Cylinder2DWrapper* Arena::createBigCylinder(QColor color, real height)
{
	return createCylinder(color, m_bigCylinderRadius, height, Cylinder2DWrapper::BigCylinder);
}

Cylinder2DWrapper* Arena::createCylinder(real radius, QColor color, real height)
{
	return createCylinder(color, radius, height, Cylinder2DWrapper::Cylinder);
}

Cylinder2DWrapper* Arena::createCircularTargetArea(real radius, QColor color)
{
	// Creating the cylinder, then we only have to change its z position and set the material to nonCollidable
	Cylinder2DWrapper* c = createCylinder(color, radius, targetAreasHeight, Cylinder2DWrapper::CircularTargetArea);

	wMatrix mtr = c->phyObject()->matrix();
	mtr.w_pos = wVector(0.0, 0.0, m_z - (targetAreasHeight / 2.0) + targetAreasProtrusion);
	c->phyObject()->setMatrix(mtr);
	c->phyObject()->setMaterial("nonCollidable");

	return c;
}

Box2DWrapper* Arena::createRectangularTargetArea(real width, real depth, QColor color)
{
	// Creating the box, then we only have to change its z position and set the material to nonCollidable
	Box2DWrapper* b = createBox(color, width, depth, targetAreasHeight, Box2DWrapper::RectangularTargetArea);

	wMatrix mtr = b->phyObject()->matrix();
	mtr.w_pos = wVector(0.0, 0.0, m_z - (targetAreasHeight / 2.0) + targetAreasProtrusion);
	b->phyObject()->setMatrix(mtr);
	b->phyObject()->setMaterial("nonCollidable");

	return b;
}

Sphere2DWrapper* Arena::createLightBulb(QColor color)
{
	Sphere2DWrapper* s = createSphere(color, m_lightBulbRadius, Sphere2DWrapper::LightBulb);

	// The light bulb is always kinematic, we don't need to change anything here
	return s;
}

bool Arena::delete2DObject(PhyObject2DWrapper* obj)
{
	int index = m_objects2DList.indexOf(obj);

	if ((index == -1) || (dynamic_cast<farsa::WheeledRobot2DWrapper*>(obj) != NULL)) {
		return false;
	}

	// Deleting the physical object
	m_world->deleteEntity(obj->phyObject());

	// Deleting the object
	delete m_objects2DList[index];
	m_objects2DList.remove(index);

	return true;
}

void Arena::prepareToHandleKinematicRobotCollisions()
{
	switch (m_collisionHandler) {
		case SimpleCollisions:
			simpleCollisionsPreAdvance();
			break;
		case CircleCollisions:
			circleCollisionsPreAdvance();
			break;
		default:
			// We should never get here
			throwUserRuntimeError("Invalid Arena Collision Handler!!!");
			break;
	}
}

void Arena::handleKinematicRobotCollisions()
{
	switch (m_collisionHandler) {
		case SimpleCollisions:
			simpleCollisionsHandle();
			break;
		case CircleCollisions:
			circleCollisionsHandle();
			break;
		default:
			// We should never get here
			throwUserRuntimeError("Invalid Arena Collision Handler!!!");
			break;
	}
}

QSet<PhyObject2DWrapper*> Arena::getKinematicRobotCollisionsSet(WheeledRobot2DWrapper* robot) const
{
	return m_kinematicRobotCollisions.value(robot);
}

QSet<PhyObject2DWrapper*> Arena::getKinematicRobotCollisionsSet(RobotResource robotResource) const
{
	if (m_robotResourceWrappers.contains(robotResource)) {
		return getKinematicRobotCollisionsSet(m_robotResourceWrappers.value(robotResource));
	} else {
		throw ArenaException(QString("The arena doesn't contain the robot named %1").arg(robotResource.first).toLatin1().data());
	}
}

Cylinder2DWrapper* Arena::createCylinder(QColor color, real radius, real height, Cylinder2DWrapper::Type type)
{
	// Changing parameters
	if (height < 0.0) {
		height = defaultHeight;
	}

	PhyCylinder* cylinder = m_world->createEntity(TypeToCreate<PhyCylinder>(), radius, height, "cylinder");
	wMatrix mtr = wMatrix::yaw(-PI_GRECO / 2.0);
	mtr.w_pos = wVector(0.0, 0.0, m_z + (height / 2.0));
	cylinder->setMatrix(mtr);
	cylinder->setUseColorTextureOfOwner(false);
	cylinder->setColor(color);
	cylinder->setTexture("");

	// Creating the wrapper
	Cylinder2DWrapper* wrapper = new Cylinder2DWrapper(this, cylinder, type);

	// Appending the wrapper to the list
	m_objects2DList.append(wrapper);

	return wrapper;
}

Box2DWrapper* Arena::createBox(QColor color, real width, real depth, real height, Box2DWrapper::Type type)
{
	// Changing parameters
	if (height < 0.0) {
		height = defaultHeight;
	}

	PhyBox* box = m_world->createEntity(TypeToCreate<PhyBox>(), width, depth, height, "box");

	// Moving the box so that it lies on the plane
	wMatrix mtr = wMatrix::identity();
	mtr.w_pos = wVector(0.0, 0.0, m_z + (height / 2.0));
	box->setMatrix(mtr);
	box->setUseColorTextureOfOwner(false);
	box->setColor(color);
	box->setTexture("");

	// Creating the wrapper
	Box2DWrapper* wrapper = new Box2DWrapper(this, box, type);

	// Appending the wrapper to the list
	m_objects2DList.append(wrapper);

	return wrapper;
}

Sphere2DWrapper* Arena::createSphere(QColor color, real radius, Sphere2DWrapper::Type type)
{
	PhySphere* sphere = m_world->createEntity(TypeToCreate<PhySphere>(), radius, "sphere");

	// Moving the sphere so that it lies on the plane
	wMatrix mtr = wMatrix::identity();
	mtr.w_pos = wVector(0.0, 0.0, m_z + radius);
	sphere->setMatrix(mtr);
	sphere->setUseColorTextureOfOwner(false);
	sphere->setColor(color);
	sphere->setTexture("");

	// Creating the wrapper
	Sphere2DWrapper* wrapper = new Sphere2DWrapper(this, sphere, type);

	// Appending the wrapper to the list
	m_objects2DList.append(wrapper);

	return wrapper;
}

void Arena::resourceChanged(QString name, Component* owner, ResourceChangeType changeType)
{
	RobotResource robotResource(name, owner);

	if (name == "world") {
		switch (changeType) {
			case ResourceCreated:
			case ResourceModified:
				m_world = getResource<World>();
				break;
			case ResourceDeclaredAsNull:
			case ResourceDeleted:
				m_world = NULL;
				break;
		}
	} else if (m_robotResourceWrappers.contains(robotResource)) {
		// The resource for a robot has changed status
		switch (changeType) {
			case ResourceCreated:
			case ResourceModified:
				// We have to delete the previous wrapper and add a new one
				{
					// Deleting the old wrapper if it exists
					if (m_robotResourceWrappers[robotResource] != NULL) {
						m_objects2DList.remove(m_objects2DList.indexOf(m_robotResourceWrappers[robotResource]));

						// Deleting the wrapper
						delete m_robotResourceWrappers[robotResource];
					}

					// Creating a new wrapper
					RobotOnPlane* r = getResource<RobotOnPlane>();
					WheeledRobot2DWrapper* wrapper = new WheeledRobot2DWrapper(this, r, r->robotHeight(), r->robotRadius());

					// Adding the wrapper to the list
					m_objects2DList.push_back(wrapper);
					m_robotResourceWrappers.insert(robotResource, wrapper);
				}
				break;
			case ResourceDeclaredAsNull:
			case ResourceDeleted:
				// Simply removing the old wrapper
				{
					m_objects2DList.remove(m_objects2DList.indexOf(m_robotResourceWrappers[robotResource]));

					// Deleting the wrapper
					delete m_robotResourceWrappers[robotResource];
					m_robotResourceWrappers[robotResource] = NULL;
				}
				break;
		}
	}
}

void Arena::simpleCollisionsPreAdvance()
{
	// Cycling through the list of robots and storing the matrix
	foreach (WheeledRobot2DWrapper* robot, m_robotResourceWrappers) {
		// We only act on kinematic robots
		if ((robot == NULL) || (!robot->robotOnPlane()->isKinematic())) {
			continue;
		}
		robot->storePreviousMatrix();
	}

	// Clearing the list of collisions for each robot
	m_kinematicRobotCollisions.clear();
}

void Arena::simpleCollisionsHandle()
{
	// Here we use a naive approach: we check collision of every object with every robot. This is not
	// the best possible algorithm (complexity is O(n^2)...), but it's the way things are done in evorobot

	// Cycling through the list of robots
	foreach (WheeledRobot2DWrapper* robot, m_robotResourceWrappers) {
		// We only act on kinematic robots
		if ((robot == NULL) || (!robot->robotOnPlane()->isKinematic())) {
			continue;
		}

		bool collides = false;
		// Checking if the robot collides
		foreach (PhyObject2DWrapper* obj, m_objects2DList) {
			// Not checking the collision of the robot with itself (obviously...) and
			// not checking collisions with non-collidable objects
			if ((obj == robot) || ((obj->phyObject() != NULL) && (obj->phyObject()->isCollidable() == false))) {
				continue;
			}

			double distance;
			double angle;

			// Getting the distance between the robot and the object. If computeDistanceAndOrientationFromRobot
			// returns false, we have to discard the object
			if (!obj->computeDistanceAndOrientationFromRobot(*robot, distance, angle)) {
				continue;
			}

			// If the distance is negative, we had a collision
			if (distance < 0.0) {
				// Checking if this collision was already handled
				if (m_kinematicRobotCollisions.contains(robot) && m_kinematicRobotCollisions[robot].contains(obj)) {
					continue;
				}

				collides = true;

				m_kinematicRobotCollisions[robot].insert(obj);

				// If obj is a robot, registering the collision for him, too
				WheeledRobot2DWrapper* otherRobot = dynamic_cast<WheeledRobot2DWrapper*>(obj);
				if (otherRobot != NULL) {
					m_kinematicRobotCollisions[otherRobot].insert(robot);
				}
			}
		}

		// If the robot collides with something else, moving it back to its previous position
		if (collides) {
			robot->wObject()->setMatrix(robot->previousMatrix());
		}
	}
}

void Arena::circleCollisionsPreAdvance()
{
	// Cycling through the list of robots and storing the matrix
	foreach (WheeledRobot2DWrapper* robot, m_robotResourceWrappers) {
		// We only act on kinematic robots
		if ((robot == NULL) || (!robot->robotOnPlane()->isKinematic())) {
			continue;
		}
		robot->storePreviousMatrix();
	}

	// Clearing the list of collisions for each robot
	m_kinematicRobotCollisions.clear();
}

void Arena::circleCollisionsHandle()
{
	// Here we use a naive approach: we check collision of every object with every robot like in the other implementation
	// (simpleCollisions). If the robot collides with a non-static object, we move the object forward and the robot a bit
	// backward. The percentage of the robot displacement which is transferred to the object is defined by the constant
	// below (not a parameter because this part should probably be re-written from scratch)
	const real robotDisplacementFractionToObject = 0.5;
	// If the robot collides with another robot we move both robots backward to resolve the collision, if it collides with
	// a static object, we move only the colliding robot bacward. In both cases we add some noise to the position and
	// orientation of the robot after the collision. The percentage of noise is defined by the constant below
	const real noiseOnPosition = 0.05f; // Absolute, added to k, see below
	const real noiseOnOrientation = PI_GRECO * 0.005f; // Absolute maximum angular displacement

	// Cycling through the list of robots
	foreach (WheeledRobot2DWrapper* robot, m_robotResourceWrappers) {
		// We only act on kinematic robots
		if ((robot == NULL) || (!robot->robotOnPlane()->isKinematic())) {
			continue;
		}

		// The vectors with distances and angles of colliding objects (these should all be negative values)
		QVector<double> distances;
		QVector<double> angles;
		QVector<PhyObject2DWrapper*> collidingObjs;

		// Checking if the robot collides
		bool collides = false;
		foreach (PhyObject2DWrapper* obj, m_objects2DList) {
			// Not checking the collision of the robot with itself (obviously...) and
			// not checking collisions with non-collidable objects
			if ((obj == robot) || ((obj->phyObject() != NULL) && (obj->phyObject()->isCollidable() == false))) {
				continue;
			}

			double distance;
			double angle;

			// Getting the distance between the robot and the object. If computeDistanceAndOrientationFromRobot
			// returns false, we have to discard the object
			if (!obj->computeDistanceAndOrientationFromRobot(*robot, distance, angle)) {
				continue;
			}

			// If the distance is negative, we had a collision
			if (distance < 0.0) {
				// Checking if this collision was already handled
				if (m_kinematicRobotCollisions.contains(robot) && m_kinematicRobotCollisions[robot].contains(obj)) {
					continue;
				}

				collides = true;

				m_kinematicRobotCollisions[robot].insert(obj);
				collidingObjs.append(obj);
				distances.append(distance);
				angles.append(angle);

				// If obj is a robot, registering the collision for him, too
				WheeledRobot2DWrapper* otherRobot = dynamic_cast<WheeledRobot2DWrapper*>(obj);
				if (otherRobot != NULL) {
					m_kinematicRobotCollisions[otherRobot].insert(robot);
				}
			}
		}

		// If the robot collides with something else we have to check if it only collided with movable objects (we then
		// move both the objects and the robot) or with static objects (we thus move only the robot)
		if (collides) {
			PhyObject2DWrapper* firstStaticObj = NULL;
			double distanceStaticObj = 0.0;
			double angleStaticObj = 0.0;
			for (int i = 0; (i < distances.size()) && (firstStaticObj == NULL); i++) {
				PhyObject2DWrapper* obj = collidingObjs[i];
				if (obj->getStatic()) {
					// We get here also if the other object is a robot (getStatic() returns true for robots...)
					firstStaticObj = obj;
					distanceStaticObj = distances[i];
					angleStaticObj = angles[i];
					break;
				}
			}

			if (firstStaticObj == NULL) {
				const wVector robotDisplacement = robot->position() - robot->previousMatrix().w_pos;
				const wVector objectDisplacement = robotDisplacement.scale(robotDisplacementFractionToObject);

				// Moving robot back (the hypothesis is that the robot can rotate freely but not move forward as
				// expected in case no obstacle was in place)
				wMatrix robotMtr = robot->wObject()->matrix();
				robotMtr.w_pos -= objectDisplacement;
				robot->wObject()->setMatrix(robotMtr);

				// Now moving objects
				foreach (PhyObject2DWrapper* obj, collidingObjs) {
					wMatrix objectMtr = obj->wObject()->matrix();
					objectMtr.w_pos += objectDisplacement;
					obj->wObject()->setMatrix(objectMtr);
				}
			} else {
				if (dynamic_cast<WheeledRobot2DWrapper*>(firstStaticObj) != NULL) {
					WheeledRobot2DWrapper* otherRobot = (WheeledRobot2DWrapper*) firstStaticObj;

					// Here we move both robots back (along the direction of their velocity) of a quantity which is sufficient
					// to remove the collision. We then add a small random noise to both position and orientation. To compute
					// how much to move robots, we can write:
					// 	r1 + r2 = || (c1 - k*v1) - (c2 - k*v2) ||
					// where r1 and r2 are the radii of the two robots, c1 and c2 are the positions on the plane of the centers
					// of the two robots at the moment of the collision, v1 and v2 are the velocities of the two robots in the
					// current timestep and k is the factor multiplying the two velocities that tells how much to move the robots
					// backward. This means that we should find k so that when the robots are moved back along the respective
					// velocity vectors by k*v1 and k*v2, the distance between their centers should be equal to r1 + r2. The
					// solution to the equation above gives two values of k. Only the positive one (which should be between 0 and
					// 1, if the robots were not colliding in the previous timestep) whould be taken (the negative one corrsponds
					// to the robots being moved forward to eliminate the collision).
					const real r1 = robot->getRadius();
					const real r2 = otherRobot->getRadius();
					const real r = r1 + r2;
					const wVector c1 = robot->wObject()->matrix().w_pos;
					const wVector c2 = otherRobot->wObject()->matrix().w_pos;
					const wVector v1 = c1 - robot->previousMatrix().w_pos;
					const wVector v2 = c2 - otherRobot->previousMatrix().w_pos;
					const real dcx = c1.x - c2.x;
					const real dcy = c1.y - c2.y;
					const real dvx = v1.x - v2.x;
					const real dvy = v1.y - v2.y;
					const real a = dvx * dvx + dvy * dvy;
					const real b = dvx * dcx + dvy * dcy;
					const real c = dcx * dcx + dcy * dcy - r * r;
					if (a < 0.000001) {
						// If we get here, both velocities were zero, so we don't do anything
						continue;
					}
					const real sqrtDelta = sqrt(b*b - a*c);
					if (isnan(sqrtDelta)) {
						// If we get here there was some error, doing nothing
						qDebug() << "Negative delta when computing collisions, skipping";

						continue;
					}
					const real k1 = (b + sqrtDelta) / a;
					const real k2 = (b - sqrtDelta) / a;
					// k is how much I have to move the robots backwards. k1 and k2 must always have different signs, because
					// the positive one is how much I have to move robots backwards to remove the collision, while the negative
					// one is how much I have to move them forward. I simply take the bigger of the two, which will be positive.
					// It could happend that the biggest is 0 (or nearly so): for example if robots are already very near and
					// they move at a very slow velocity when colliding. This is not a problem, robots will not be moved in this
					// step and they will be moved back if they move more in subsequent steps
					const real k = (k1 > k2) ? k1 : k2;

					// Sanity check. Here we have to allow small negative values (which will be ignored) that are possible in
					// the case described above (close robots moving at slow speed)
					if (k < -0.000001) {
						Logger::warning(QString("Wrong factor when resolving collisions, setting a value to NaN. r1 = %1, r2 = %2, r = %3, c1 = %4, c2 = %5, v1 = %6, v2 = %7, dcx = %8, dcy = %9, dvx = %10, dvy = %11, a = %12, b = %13, c = %14, sqrtDelta = %15, k1 = %16, k2 = %17, k = %18").arg(r1).arg(r2).arg(r).arg(c1).arg(c2).arg(v1).arg(v2).arg(dcx).arg(dcy).arg(dvx).arg(dvy).arg(a).arg(b).arg(c).arg(sqrtDelta).arg(k1).arg(k2).arg(k));

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning For the moment we don t kill the simulation, just for testing purpouse. We set the position of one robot to NaN
#endif
						//qFatal("Wrong factor when resolving collisions in %s at line %d", __FILE__, __LINE__);
						wMatrix robotMtr = robot->wObject()->matrix();
						robotMtr.w_pos.x = sqrt(-1.0);
						robot->wObject()->setMatrix(robotMtr);
						continue;
					} else if (k < 0.0) {
						continue;
					}

					// Moving robots back
					// First robot
					wMatrix robotMtr = robot->wObject()->matrix();
					wVector robotPos = robotMtr.w_pos - v1.scale(k + globalRNG->getDouble(0.0, noiseOnPosition));
					robotMtr = robotMtr.rotateAround(wVector::Z(), robotMtr.w_pos, globalRNG->getDouble(-noiseOnOrientation, noiseOnOrientation));
					robotMtr.w_pos = robotPos;
					robot->wObject()->setMatrix(robotMtr);
					// Second robot
					robotMtr = otherRobot->wObject()->matrix();
					robotPos = robotMtr.w_pos - v2.scale(k + globalRNG->getDouble(0.0, noiseOnPosition));
					robotMtr = robotMtr.rotateAround(wVector::Z(), robotMtr.w_pos, globalRNG->getDouble(-noiseOnOrientation, noiseOnOrientation));
					robotMtr.w_pos = robotPos;
					otherRobot->wObject()->setMatrix(robotMtr);
				} else if (dynamic_cast<Box2DWrapper*>(firstStaticObj) != NULL) {
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning COMPLETARE QUESTO (COLLISIONI CON BOX)
#endif
					Box2DWrapper* obj = (Box2DWrapper*) firstStaticObj;

// 					Qui per il muro bisogna usare l'angolo tra muro e robot per poter calcolare di quanto si deve spostare il robot indietro.
// 					muro

// 					distanceStaticObj
// 					angleStaticObj

					robot->wObject()->setMatrix(robot->previousMatrix());
				} else if (dynamic_cast<Cylinder2DWrapper*>(firstStaticObj) != NULL) {
					// Computing the robot displacement
					const wVector robotDisplacement = robot->position() - robot->previousMatrix().w_pos;

					// Now computing how much the robot should be moved backward and adding some noise as a portion of the robot displacement. The
					// minus sign when computing k is because the distance is negative in case of collisions. We check the robotDisplacement.norm()
					// is not too small to avoid numerical problems. If it is, simply moveing to the previous location (no noise on position since
					// it would be tiny)
					const real robotDisplacementNorm = robotDisplacement.norm();
					if (robotDisplacementNorm < 0.0001) {
						// Moving back the robot and adding noise on orientation only
						wMatrix robotMtr = robot->wObject()->matrix();
						robotMtr = robotMtr.rotateAround(wVector::Z(), robotMtr.w_pos, globalRNG->getDouble(-noiseOnOrientation, noiseOnOrientation));
						robotMtr.w_pos = robot->previousMatrix().w_pos;
						robot->wObject()->setMatrix(robotMtr);
					} else {
						const real k = -distanceStaticObj / robotDisplacementNorm;

						// Moving back the robot and adding noise
						wMatrix robotMtr = robot->wObject()->matrix();
						const wVector robotPos = robotMtr.w_pos - robotDisplacement.scale(k + globalRNG->getDouble(0.0, noiseOnPosition));
						robotMtr = robotMtr.rotateAround(wVector::Z(), robotMtr.w_pos, globalRNG->getDouble(-noiseOnOrientation, noiseOnOrientation));
						robotMtr.w_pos = robotPos;
						robot->wObject()->setMatrix(robotMtr);
					}
				} else {
					qDebug() << "Unknown object type when resolving collisions, skipping";
				}
			}
		}
	}
}

Box2DWrapper* Arena::createPlane(real z)
{
	// First of all getting the dimensions of the plane
	const real planeWidth = ConfigurationHelper::getReal(configurationManager(), confPath() + "planeWidth");
	const real planeHeight = ConfigurationHelper::getReal(configurationManager(), confPath() + "planeHeight");

	// Also getting the world
	World* world = getResource<World>("world");

	PhyBox* plane = world->createEntity(TypeToCreate<PhyBox>(), planeWidth, planeHeight, planeThickness, "plane");
	plane->setPosition(wVector(0.0, 0.0, z - (planeThickness / 2.0)));
	plane->setStatic(true);
	plane->setColor(Qt::white);
	plane->setTexture("tile2");

	// Also resizing world to comfortably fit the arena if it is smaller than necessary
	wVector worldMinPoint;
	wVector worldMaxPoint;
	world->size(worldMinPoint, worldMaxPoint);
	worldMinPoint.x = min(-planeWidth, worldMinPoint.x);
	worldMinPoint.y = min(-planeHeight, worldMinPoint.y);
	worldMinPoint.z = min(-planeThickness, worldMinPoint.z);
	worldMaxPoint.x = max(planeWidth, worldMaxPoint.x);
	worldMaxPoint.y = max(planeHeight, worldMaxPoint.y);
	worldMaxPoint.z = max(+6.0, worldMaxPoint.z);
	world->setSize(worldMinPoint, worldMaxPoint);

	// Creating the wrapper and returning it
	Box2DWrapper* ret = new Box2DWrapper(this, plane, Box2DWrapper::Plane);
	m_objects2DList.append(ret);
	return ret;
}

} // end namespace farsa
