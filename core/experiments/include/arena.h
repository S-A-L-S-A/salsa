/********************************************************************************
 *  SALSA Experimentes Library                                                  *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
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

#ifndef ARENA_H
#define ARENA_H

#include "experimentsconfig.h"
#include "component.h"
#include "world.h"
#include "wheeledexperimenthelper.h"
#include "baseexception.h"
#include "salsamiscutilities.h"
#include <QVector>
#include <QMap>
#include <QString>
#include <QSet>

namespace salsa {

/**
 * \brief The class modelling an arena
 *
 * This class contains functions to create and manipulate an arena, which is
 * used mostly in simulations with wheeled robots. The arena always has a plane
 * at the given height, you can add more planes at different heights and you can
 * connected different planes with ramps. Planes are perpendicular to the Z
 * axis. The dimension of the main plane and its Z position can only be set via
 * configuration parameters and are fixed.
 */
class SALSA_EXPERIMENTS_API Arena : public Component
{
public:
	/**
	 * \brief A resource name for a robot and its owner
	 */
	typedef QPair<QString, Component*> RobotResource;

	/**
	 * \brief A list of robots (used when adding robots to the arena
	 */
	typedef QList<RobotResource> RobotsList;

private:
	/**
	 * \brief The possible types of kinematic collision handlers
	 */
	enum KinematicCollisionHandlers {
		SimpleCollisions, /**< The simple collision handler which puts
		                       robots back to their position before
		                       collision */
		CircleCollisions, /**< The collision handler which handles
		                       collisions between circular objects
		                       (robots and cylinders) computing the
		                       results based on objects mass */
		UnknownCollisionHandler /**< An invalid value */
	};

	/**
	 * \brief Converts a string to a value of the KinematicCollisionHandlers
	 *        enum
	 *
	 * \param str the string to convert
	 * \return the KinematicCollisionHandlers value corresponding to the
	 *         string. UnknownCollisionHandler is returned if the string is
	 *         not recognized
	 */
	static KinematicCollisionHandlers stringToCollisionHandler(QString str);

	/**
	 * \brief Converts a value of the KinematicCollisionHandlers enum to
	 *        string
	 *
	 * \param h the value to convert
	 * \return the string representation of h
	 */
	static QString collisionHandlerToString(KinematicCollisionHandlers h);

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object with parameters to
	 *               use
	 */
	Arena(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~Arena();

	/**
	 * \brief Adds to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns the list of 2D objects
	 *
	 * \return the list of 2D objects
	 */
	const QVector<PhyObject2DWrapper*>& getObjects() const
	{
		return m_objects2DList;
	}

	/**
	 * \brief Returns the z of the plane
	 *
	 * \return the z of the plane
	 */
	real getZ() const
	{
		return m_z;
	}

	/**
	 * \brief Returns the width of the main plane
	 *
	 * \return the width of the main plane
	 */
	real getWidth() const
	{
		return m_plane->phyObject()->sideX();
	}

	/**
	 * \brief Returns the height of the main plane
	 *
	 * \return the height of the main plane
	 */
	real getHeight() const
	{
		return m_plane->phyObject()->sideY();
	}

	/**
	 * \brief Adds robots to the list of 2D objects
	 *
	 * This function adds the given robots to the list of 2D objects so that
	 * they can be perceived by the sensors of other robots (like, e.g. the
	 * linear camera). Each element of the list is made up of the name of
	 * the resource associated to the robot and the component to which the
	 * resource belongs.
	 * \param robots the name of the resources with robots and their owners
	 */
	void addRobots(RobotsList robots);

	/**
	 * \brief Returns a pointer to the wrapper of a robot given the robot
	 *        resource name
	 *
	 * \param robotResource the name of the resource for the robot and its owner
	 * \return the wrapper for the robot with the given resource or nullptr if
	 *         no wrapper exists for the given robot
	 */
	const WheeledRobot2DWrapper* getRobotWrapper(RobotResource robotResource) const;

	/**
	 * \brief Returns the plane of the arena
	 *
	 * \return the plane of the arena
	 */
	Box2DWrapper* getPlane();

	/**
	 * \brief Returns the plane of the arena (const version)
	 *
	 * \return the plane of the arena
	 */
	const Box2DWrapper* getPlane() const;

	/**
	 * \brief Creates a wall
	 *
	 * The wall is created using a starting point and an ending point. The z
	 * component of the vectors for the starting point and the ending point
	 * are discarded. If you imagine the wall as a rectangle, the starting
	 * point and the ending point are in the middle of two opposite sides.
	 * The wall also have a thickness and an height
	 * \param color the color of the wall
	 * \param start the starting point of the wall. The Z coordinate of the
	 *              vector is discarded
	 * \param end the ending point of the wall. The Z component is discarded
	 * \param thickness the thickness of the wall
	 * \param height the height of the wall. If negative the default height
	 *               for objects is used
	 * \return the new object
	 */
	Box2DWrapper* createWall(QColor color, wVector start, wVector end, real thickness, real height = -1.0);

	/**
	 * \brief Creates a small cylinder
	 *
	 * The cylinder has a fixed radius. You can specify its color and
	 * height
	 * \param color the color of the cylinder
	 * \param height the height of the wall. If negative the default height
	 *               for objects is used
	 * \return the new object
	 */
	Cylinder2DWrapper* createSmallCylinder(QColor color, real height = -1.0);

	/**
	 * \brief Creates a big cylinder
	 *
	 * The cylinder has a fixed radius. You can specify its color and
	 * height
	 * \param color the color of the cylinder
	 * \param height the height of the wall. If negative the default height
	 *               for objects is used
	 * \return the new object
	 */
	Cylinder2DWrapper* createBigCylinder(QColor color, real height = -1.0);

	/**
	 * \brief Creates a cylinder
	 *
	 * This creates a cylinder with the specified radius
	 * \param radius the radius of the cylinder
	 * \param color the color of the cylinder
	 * \param height the height of the wall. If negative the default height
	 *               for objects is used
	 * \return the new object
	 * \note If you plan to use sampled sensors, you should use either
	 *       createSmallCylinder or createBigCylinder as cylinders created
	 *       by this function are not taken into account
	 */
	Cylinder2DWrapper* createCylinder(real radius, QColor color, real height = -1.0);

	/**
	 * \brief Creates a circular target area
	 *
	 * The target area is placed on the ground and is detectable by ground
	 * sensors
	 * \param radius the radius of the area
	 * \param color the color of the area
	 * \return the new object
	 */
	Cylinder2DWrapper* createCircularTargetArea(real radius, QColor color);

	/**
	 * \brief Creates a rectangular target area
	 *
	 * The target area is placed on the ground and is detectable by ground
	 * sensors
	 * \param width the width of the area
	 * \param depth the depth of the area
	 * \param color the color of the area
	 * \return the new object
	 */
	Box2DWrapper* createRectangularTargetArea(real width, real depth, QColor color);

	/**
	 * \brief Creates a LightBulb
	 *
	 * The light bulb is a spherical object which acts as a source of light
	 * and is seen by certain sensors. It is not a physical object, i.e. it
	 * doesn't interact with other objects or robots
	 * \param color the color of the light bulb
	 * \return the new object
	 */
	Sphere2DWrapper* createLightBulb(QColor color);

	/**
	 * \brief Deletes an object
	 *
	 * This deletes the wrapper and the underlying physical object. This
	 * also takes care of removing the object from the list of 2D objects of
	 * the arena. If the object is not in the arena, this does nothing and
	 * returns false
	 * \warning You should NEVER pass a robot to this function. If you do,
	 *          the function does nothing and returns false
	 * \param obj the object to delete
	 * \return true if the object is deleted, false if the object is not in
	 *         the arena
	 */
	bool delete2DObject(PhyObject2DWrapper* obj);

	/**
	 * \brief Call this before advancing the world to prepare stuffs needed
	 *        by a call to handleKinematicRobotCollisions()
	 *
	 * This function has to be called before world advance if you plan to
	 * use the handleKinematicRobotCollisions() afterward. What it actually
	 * does is storing the previous robot position.
	 */
	void prepareToHandleKinematicRobotCollisions();

	/**
	 * \brief Checks collisions of kinematic robots
	 *
	 * This function checks collisions of kinematic robots with walls, other
	 * robots and objects. If a robot collides with something else, it is
	 * moved back to the position and orientation it had in the previous
	 * timestep. This is not very accurate but it is fast.
	 */
	void handleKinematicRobotCollisions();

	/**
	 * \brief Returns the set of objects that collided with the given
	 *        kinematic robot at the previous timestep
	 *
	 * \param robot the robot whose collisions are requested
	 * \return the set of objects that collided with the given robot at the
	 *         previous timestep. The set is empty if the robot had no
	 *         collisions or the robot is not in the list
	 * \note This only works for robots that are kinematic!!!
	 */
	QSet<PhyObject2DWrapper*> getKinematicRobotCollisionsSet(WheeledRobot2DWrapper* robot) const;

	/**
	 * \brief Returns the set of objects that collided with the given
	 *        kinematic robot at the previous timestep
	 *
	 * \param robotResource the name of the resource of the robot whose
	 *                      collisions are requested and its owner
	 * \return the set of objects that collided with the given robot at the
	 *         previous timestep. The set is empty if the robot had no
	 *         collisions or the robot is not in the list
	 * \note This only works for robots that are kinematic!!!
	 */
	QSet<PhyObject2DWrapper*> getKinematicRobotCollisionsSet(RobotResource robotResource) const;

private:
	/**
	 * \brief Creates a cylinder with the given radius
	 *
	 * \param color the color of the cylinder
	 * \param radius the radius of the cylinder
	 * \param height the height of the cylinder. If negative the default
	 *               height for objects is used
	 * \param type the type of wrapper to create
	 * \return the new object
	 */
	Cylinder2DWrapper* createCylinder(QColor color, real radius, real height, Cylinder2DWrapper::Type type);

	/**
	 * \brief Creates a box with the given dimensions
	 *
	 * \param color the color of the box
	 * \param width the height of the box
	 * \param depth the depth of the box
	 * \param height the height of the box. If negative the default height
	 *               for objects is used
	 * \param type the type of wrapper to create
	 * \return the new object
	 */
	Box2DWrapper* createBox(QColor color, real width, real depth, real height, Box2DWrapper::Type type);

	/**
	 * \brief Creates a sphere with the given radius
	 *
	 * \param color the color of the sphere
	 * \param radius the radius of the sphere
	 * \param type the type of wrapper to create
	 * \return the new object
	 */
	Sphere2DWrapper* createSphere(QColor color, real radius, Sphere2DWrapper::Type type);

	/**
	 * \brief The function called when a resource used here is changed
	 *
	 * \param name the name of the resource that has changed
	 * \param owner the owner of the resource that has changed
	 * \param chageType the type of change the resource has gone through
	 *                  (whether it was created, modified or deleted)
	 */
	virtual void resourceChanged(QString name, Component* owner, ResourceChangeType changeType);

	/**
	 * \brief The implementation of the SimpleCollisions collision handler
	 *        called by prepareToHandleKinematicRobotCollisions()
	 */
	void simpleCollisionsPreAdvance();

	/**
	 * \brief The implementation of the SimpleCollisions collision handler
	 *        called by handleKinematicRobotCollisions()
	 */
	void simpleCollisionsHandle();

	/**
	 * \brief The implementation of the CircleCollisions collision handler
	 *        called by prepareToHandleKinematicRobotCollisions()
	 */
	void circleCollisionsPreAdvance();

	/**
	 * \brief The implementation of the CircleCollisions collision handler
	 *        called by handleKinematicRobotCollisions()
	 */
	void circleCollisionsHandle();

	/**
	 * \brief Creates the plane of the arena
	 *
	 * \param z the z of the upper surface of the plane
	 * \return the plane
	 */
	Box2DWrapper* createPlane(real z);

	/**
	 * \brief The Z position of the plane in the world
	 */
	const real m_z;

	/**
	 * \brief The radius of small cylinders
	 *
	 * The default value is the one used in evorobot. If you change this, be
	 * careful which sample files you use.
	 */
	const real m_smallCylinderRadius;

	/**
	 * \brief The radius of big cylinders
	 *
	 * The default value is the one used in evorobot. If you change this, be
	 * careful which sample files you use.
	 */
	const real m_bigCylinderRadius;

	/**
	 * \brief The radius of light bulbs
	 *
	 * The default value is hard coded in the source file.
	 */
	const real m_lightBulbRadius;

	/**
	 * \brief The collision handler to use
	 */
	const KinematicCollisionHandlers m_collisionHandler;

	/**
	 * \brief The list of 2D object wrappers
	 */
	QVector<PhyObject2DWrapper*> m_objects2DList;

	/**
	 * \brief The main plane
	 */
	Box2DWrapper* const m_plane;

	/**
	 * \brief The map between robot resources and the wrappers
	 *
	 * We need this to delete wrappers if robots are destroyed
	 */
	QMap<RobotResource, WheeledRobot2DWrapper*> m_robotResourceWrappers;

	/**
	 * \brief The map with collisions of kinematic robots with objects
	 *
	 * For each robot stores a list of objects with which the robot collided
	 * in the previous timestep
	 */
	QMap<WheeledRobot2DWrapper*, QSet<PhyObject2DWrapper*> > m_kinematicRobotCollisions;

	/**
	 * \brief The simulated world
	 */
	World* m_world;
};

// All the suff below is to avoid warnings on Windows about the use of the
// unsafe function sprintf and strcpy...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

/**
 * \brief The exception thrown at runtime by the arena
 */
class SALSA_EXPERIMENTS_TEMPLATE ArenaException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param reasong the description of why the exception was thrown. The
	 *                buffer for this is at most 256 characters (including
	 *                the '\0' terminator)
	 */
	ArenaException(const char* reason) throw() :
		BaseException()
	{
		strncpy(m_reason, reason, 256);
		m_reason[255] = '\0';
		sprintf(m_errorMessage, "Runtime error in the Arena, reason: %s", m_reason);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ArenaException(const ArenaException& other) throw() :
		BaseException(other)
	{
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ArenaException& operator=(const ArenaException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ArenaException() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_errorMessage;
	}

	/**
	 * \brief Returns the description of why the exception was thrown
	 *
	 * \return the description of why the exception was thrown
	 */
	const char *reason() const throw()
	{
		return m_reason;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ArenaException)

private:
	/**
	 * \brief The description of why the exception was thrown
	 */
	char m_reason[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[512];
};

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

} // end namespace salsa

#endif
