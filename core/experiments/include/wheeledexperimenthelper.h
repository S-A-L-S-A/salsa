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

#ifndef WHEELEDEXPERIMENTHELPER_H
#define WHEELEDEXPERIMENTHELPER_H

#include "experimentsconfig.h"
#include "world.h"
#include "wvector.h"
#include "phyobject.h"
#include "phybox.h"
#include "phycylinder.h"
#include "physphere.h"
#include <QVector>

namespace salsa {

class Arena;
class RobotOnPlane;
class WheeledRobot2DWrapper;

/**
 * \brief A class wrapping a PhyObject to add methods suitable for wheeled
 *        robots simulations
 *
 * This class wraps a PhyObject and adds methods which should be used when doing
 * wheeled robots simulations to ensure everything works well. Objects cannot be
 * instantiated directly, they are returned by functions of the Arena class
 */
class SALSA_EXPERIMENTS_API PhyObject2DWrapper
{
public:
	/**
	 * \brief The possible type of wrapped objects
	 */
	enum Type {
		Plane = 0,
		Wall,
		Box,
		RectangularTargetArea,
		Cylinder,
		SmallCylinder,
		BigCylinder,
		CircularTargetArea,
		LightBulb,
		WheeledRobot,
		NumberOfDifferentTypes
	};

	/**
	 * \brief The structure with angular range and color used by the
	 *        computeLinearViewFieldOccupiedRange() function
	 */
	class AngularRangeAndColor
	{
	public:
		/**
		 * \brief Constructor
		 */
		AngularRangeAndColor() :
			minAngle(0.0),
			maxAngle(0.0),
			color()
		{
		}

		/**
		 * \brief Constructor
		 *
		 * \param mn the min angle of the range
		 * \param mx the max angle of the range
		 * \param c the color of the range
		 */
		AngularRangeAndColor(double mn, double mx, QColor c) :
			minAngle(mn),
			maxAngle(mx),
			color(c)
		{
		}

		/**
		 * \brief The min angle of the range
		 */
		double minAngle;

		/**
		 * \brief The max angle of the range
		 */
		double maxAngle;

		/**
		 * \brief The color of the range
		 */
		QColor color;
	};

public:
	/**
	 * \brief Returns a pointer to the wrapped object
	 *
	 * You should avoid changing the wrapped directly, use the methods of
	 * this class, instead. The default implementation of this function
	 * simply calls phyObject()
	 * \return a pointer to the wrapped object
	 */
	virtual WObject* wObject();

	/**
	 * \brief Returns a pointer to the wrapped object (const version)
	 *
	 * The default implementation of this function simply calls phyObject()
	 * \return a const pointer to the wrapped object
	 */
	virtual const WObject* wObject() const;

	/**
	 * \brief Returns a pointer to the wrapped PhyObject
	 *
	 * You should avoid changing the PhyObject directly, use the methods of
	 * this class, instead. The returned pointer can be nullptr if the wrapped
	 * object is not a phyObject
	 * \return a pointer to the wrapped PhyObject
	 */
	virtual PhyObject* phyObject() = 0;

	/**
	 * \brief Returns a pointer to the wrapped PhyObject (const version)
	 *
	 * The returned pointer can be nullptr if the wrapped object is not a
	 * phyObject
	 * \return a const pointer to the wrapped PhyObject
	 */
	virtual const PhyObject* phyObject() const = 0;

	/**
	 * \brief Returns the type of this wrapper object
	 *
	 * The type is used by e.g. sensors
	 * \return the type of this wrapper object
	 */
	virtual Type type() const = 0;

	/**
	 * \brief Sets whether the object is static or not
	 *
	 * Not all type of objects can change between static and dynamic. This
	 * function does nothing if the wrapped object is not a PhyObject (i.e.
	 * phyObject() returns nullptr)
	 * \param s if true the object will be static
	 */
	virtual void setStatic(bool s);

	/**
	 * \brief Returns true if the object is static
	 *
	 * This function returns always true if the wrapped object is not a
	 * PhyObject (i.e. phyObject() returns nullptr)
	 * \return true if the object is static (or if the wrapped object is not
	 *         a phyObject)
	 */
	bool getStatic() const;

	/**
	 * \brief Sets whether the object is kinematic or not
	 *
	 * A kinematic object is not simulated by the physics simulator, but it
	 * can be taken into account in collisions by the arena. Not all type of
	 * objects can change between kinematic and dynamic
	 * \param b if true the object is set to kinematic behaviour
	 * \param c if true the object collides with other objects even in
	 *          kinematic mode, influencing them without being influenced
	 *          (technically, we set its mass to 0 in Newton Game Dynamics
	 *          engine)
	 */
	virtual void setKinematic(bool b, bool c = false);

	/**
	 * \brief Returns true if the object has kinematic behaviour
	 *
	 * This function returns always true if the wrapped object is not a
	 * PhyObject (i.e. phyObject() returns nullptr)
	 * \return true if the object has kinematic behaviour
	 */
	bool getKinematic() const;

	/**
	 * \brief Sets the position of the object in the plane
	 *
	 * The Z coordinate is discarded. Not all type of objects can be moved
	 * \param pos the new position (the z coordinate is discarded)
	 */
	void setPosition(wVector pos);

	/**
	 * \brief Sets the position of the object in the plane
	 *
	 * Not all type of objects can be moved
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(real x, real y) = 0;

	/**
	 * \brief Returns the position of the object
	 *
	 * \return the position of the object
	 */
	wVector position() const;

	/**
	 * \brief Computes the portion of a linear the view field occupied by
	 *        this object
	 *
	 * This function must be implemented in subclasses to return the angular
	 * range occupied by the object on a linear camera described by the
	 * given matrix. Each of the range in the list has its own color. The Z
	 * axis of the matrix is the upvector, the camera lies on the XY plane
	 * and points towars the X axis (i.e. angles start from the X axis). If
	 * the object is not visible  by the camera, this function returns a
	 * negative distance. The min angle can be greater than the max angle in
	 * a range if the object is behind the camera.
	 * \param mtr the matrix describing the camera (see function
	 *            description)
	 * \param rangesAndColors the list of occupied ranges and their color
	 * \param distance this is set to the distance of the object from the
	 *                 camera. It is negative if the object is not visible
	 *                 by the camera
	 * \param maxDistance this is the maximum distance the camera can see.
	 *                    If the object is farther than this distance the
	 *                    function can safely return a negative distance
	 *                    (i.e. the object is not seen) and stop any other
	 *                    computation
	 */
	virtual void computeLinearViewFieldOccupiedRange(const wMatrix& mtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const = 0;

	/**
	 * \brief Computes the distance and orientation of this object respect
	 *        to the given robot
	 *
	 * This function computes the distance of this object from the given
	 * robot. The distance is computed between the nearest points of the two
	 * objects. The robot is always modelled as a vertical cylindrical
	 * object. The function returns false if it makes no sense to compute
	 * the distance
	 * \param robot the robot from which the distance should be computed
	 * \param distance the computed distance
	 * \param angle the computed orientation
	 * \return false if the distance couldn't be computed
	 */
	virtual bool computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const = 0;

	/**
	 * \brief Set the texture to use when rendering this
	 *
	 * \param textureName the name of the texture
	 */
	void setTexture(QString textureName);

	/**
	 * \brief Returns the name of the texture
	 *
	 * \return the name of the texture
	 */
	QString texture() const;

	/**
	 * \brief Sets the color to use when rendering this
	 *
	 * \param color the color to use when rendering this
	 */
	void setColor(QColor color);

	/**
	 * \brief Returns the color to use when rendering this
	 *
	 * \return the color to use when rendering this
	 */
	QColor color() const;

	/**
	 * \brief Sets whether this will be rendered with the color and texture
	 *        of our owner (if we have one)
	 *
	 * \param b if true we will use the color and texture of our owner (if
	 *          we have one
	 */
	void setUseColorTextureOfOwner(bool b);

	/**
	 * \brief Returns whether this will be rendered with the color and
	 *        texture of the owner (if we have one)
	 *
	 * \return true if this will be rendered using the color and texture of
	 *         our owner
	 */
	bool useColorTextureOfOwner() const;

	/**
	 * \brief Stores the current matrix in the previousMatrix
	 *
	 * Call this to update the value of m_previousMatrix
	 */
	void storePreviousMatrix()
	{
		m_previousMatrix = wObject()->matrix();
	}

	/**
	 * \brief Returns the transformation matrix of the previous time-step
	 *
	 * \return the transformation matrix of the previous time-step
	 */
	const wMatrix& previousMatrix() const
	{
		return m_previousMatrix;
	}

protected:
	/**
	 * \brief Constructor
	 *
	 * \param arena the arena in which this object lives
	 */
	PhyObject2DWrapper(Arena* arena);

	/**
	 * \brief Destructor
	 */
	virtual ~PhyObject2DWrapper();

	/**
	 * \brief The pointer to the arena in which this object lives
	 */
	Arena *const m_arena;

private:
	/**
	 * \brief The matrix at the previous timestep
	 *
	 * This is not updated by this class, it is set externally (dirty, I
	 * know...)
	 */
	wMatrix m_previousMatrix;

	/**
	 * \brief Copy constructor
	 *
	 * Here to prevent usage
	 */
	PhyObject2DWrapper(const PhyObject2DWrapper&);

	/**
	 * \brief Copy operator
	 *
	 * Here to prevent usage
	 */
	PhyObject2DWrapper& operator=(const PhyObject2DWrapper&);

	/**
	 * \brief The Arena class is friend to be able to instantiate and delete
	 *        this class
	 */
	friend class Arena;
};

/**
 * \brief The subclass of PhyObject2DWrapper wrapping a box
 */
class SALSA_EXPERIMENTS_API Box2DWrapper : public PhyObject2DWrapper
{
public:
	/**
	 * \brief Returns a pointer to the wrapped PhyObject
	 *
	 * You should avoid changing the PhyObject directly, use the methods of
	 * this class, instead
	 * \return a pointer to the wrapped PhyObject
	 */
	virtual PhyBox* phyObject();

	/**
	 * \brief Returns a pointer to the wrapped PhyObject (const version)
	 *
	 * \return a const pointer to the wrapped PhyObject
	 */
	virtual const PhyBox* phyObject() const;

	/**
	 * \brief Returns the type of this wrapper object
	 *
	 * \return the type of this wrapper object
	 */
	virtual Type type() const;

	/**
	 * \brief Sets whether the object is static or not
	 *
	 * Whether this can be made static or not depends on the type of box
	 * \param s if true the object will be static
	 */
	virtual void setStatic(bool s);

	/**
	 * \brief Sets whether the object is kinematic or not
	 *
	 * Whether this can be made kinematic or not depends on the type of box
	 * \param b if true the object is set to kinematic behaviour
	 * \param c if true the object collides with other objects even in
	 *          kinematic mode, influencing them without being influenced
	 *          (technically, we set its mass to 0 in Newton Game Dynamics
	 *          engine)
	 */
	virtual void setKinematic(bool b, bool c = false);

	/**
	 * \brief Sets the position of the object in the plane
	 *
	 * Whether this can be moved or not depends on the type of box
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(real x, real y);

	/**
	 * \brief Computes the portion of a linear the view field occupied by
	 *        this object
	 *
	 * This function returns the angular range occupied by this object on a
	 * linear camera described by the given matrix. The Z axis of the matrix
	 * is the upvector, the camera lies on the XY plane and points towars
	 * the X axis (i.e. angles start from the X axis). If the object is not
	 * visible  by the camera, this function returns a negative distance.
	 * The min angle can be greater than the max angle if the object is
	 * behind the camera. Here we make the assumption that the camera is
	 * outside the box (otherwise the algorithm gives wrong results). The
	 * matrix representing the camera is translated so that it is on the
	 * same plane as the vertexes of the box before doing computations
	 * \param cameraMtr the matrix describing the camera (see function
	 *                  description)
	 * \param rangesAndColors the list of occupied ranges and their color
	 * \param distance this is set to the distance of the object from the
	 *                 camera. It is negative if the object is not visible
	 *                 by the camera
	 * \param maxDistance this is the maximum distance the camera can see.
	 *                    If the object is farther than this distance the
	 *                    function can safely return a negative distance
	 *                    (i.e. the object is not seen) and stop any other
	 *                    computation
	 */
	virtual void computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const;

	/**
	 * \brief Computes the distance and orientation of this object respect
	 *        to the given robot
	 *
	 * This function computes the distance of this object from the given
	 * robot. The distance is computed between the nearest points of the two
	 * objects. The robot is always modelled as a vertical cylindrical
	 * object. The function returns false if it makes no sense to compute
	 * the distance
	 * \param robot the robot from which the distance should be computed
	 * \param distance the computed distance
	 * \param angle the computed orientation
	 * \return false if the distance couldn't be computed
	 */
	virtual bool computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const;

	/**
	 * \brief Returns the coordinates of the i-th vertex of the box on the
	 *        plane in the global frame of reference
	 *
	 * \param i the index of the vertex, between 0 and 3
	 * \return the coordinates of the i-th vertex of the box on the plane
	 *         in the global frame of reference.
	 */
	const wVector& vertex(int i) const
	{
		return m_vertexes[i];
	}

	/**
	 * \brief Returns the coordinates of the center of the box on the plane
	 *        in the global frame of reference
	 *
	 * \return the coordinates of the center of the box on the plane in the
	 *         global frame of reference
	 */
	const wVector& centerOnPlane() const
	{
		return m_centerOnPlane;
	}

	/**
	 * \brief Returns the width of the box
	 *
	 * \return the width of the box
	 */
	real width() const
	{
		return m_box->sideX();
	}

	/**
	 * \brief Returns the depth of the box
	 *
	 * \return the depth of the box
	 */
	real depth() const
	{
		return m_box->sideY();
	}

protected:
	/**
	 * \brief Constructor
	 *
	 * \param arena the arena in which this object lives
	 * \param box the box to wrap
	 * \param type the type of box. This can only be one of Plane, Wall,
	 *             Box, RectangularTargetArea. This can only be made
	 *             non-static if the type is Box. This can only be moved if
	 *             the type is Box or RectangularTargetArea
	 */
	Box2DWrapper(Arena* arena, PhyBox* box, Type type);

	/**
	 * \brief Destructor
	 */
	~Box2DWrapper();

private:
	/**
	 * \brief The wrapped box
	 */
	PhyBox* const m_box;

	/**
	 * \brief The coordinates of the vertexes of the box on the plane
	 *
	 * The vector has 4 elements. Coordinates are in the global frame of
	 * reference.
	 */
	QVector<wVector> m_vertexes;

	/**
	 * \brief The coordinates of the center of the box on the plane
	 *
	 * Coordinates are in the global frame of reference.
	 */
	wVector m_centerOnPlane;

	/**
	 * \brief The type of box
	 *
	 * This can only be one of Plane, Wall, Box, RectangularTargetArea. This
	 * can only be made non-static if the type is Box. This can only be
	 * moved if the type is Box or RectangularTargetArea
	 */
	const Type m_type;

	/**
	 * \brief The Arena class is friend to be able to instantiate and delete
	 *        this class
	 */
	friend class Arena;
};

/**
 * \brief The subclass of PhyObject2DWrapper wrapping a cylinder
 */
class SALSA_EXPERIMENTS_API Cylinder2DWrapper : public PhyObject2DWrapper
{
public:
	/**
	 * \brief Returns a pointer to the wrapped PhyObject
	 *
	 * You should avoid changing the PhyObject directly, use the methods of
	 * this class, instead
	 * \return a pointer to the wrapped PhyObject
	 */
	virtual PhyCylinder* phyObject();

	/**
	 * \brief Returns a pointer to the wrapped PhyObject (const version)
	 *
	 * \return a const pointer to the wrapped PhyObject
	 */
	virtual const PhyCylinder* phyObject() const;

	/**
	 * \brief Sets whether the object is static or not
	 *
	 * Whether this can be made kinematic or not depends on the type of
	 * cylinder
	 * \param s if true the object will be static
	 */
	virtual void setStatic(bool s);

	/**
	 * \brief Sets whether the object is kinematic or not
	 *
	 * Whether this can be made static or not depends on the type of
	 * cylinder
	 * \param b if true the object is set to kinematic behaviour
	 * \param c if true the object collides with other objects even in
	 *          kinematic mode, influencing them without being influenced
	 *          (technically, we set its mass to 0 in Newton Game Dynamics
	 *          engine)
	 */
	virtual void setKinematic(bool b, bool c = false);

	/**
	 * \brief Returns the type of this wrapper object
	 *
	 * \return the type of this wrapper object
	 */
	virtual Type type() const;

	/**
	 * \brief Sets the position of the object in the plane
	 *
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(real x, real y);

	/**
	 * \brief Computes the portion of a linear the view field occupied by
	 *        this object
	 *
	 * This function returns the angular range occupied by this object on a
	 * linear camera described by the given matrix. The Z axis of the matrix
	 * is the upvector, the camera lies on the XY plane and points towars
	 * the X axis (i.e. angles start from the X axis). If the object is not
	 * visible  by the camera, this function returns a negative distance.
	 * The min angle can be greater than the max angle if the object is
	 * behind the camera. Here we make the assumption that the camera is
	 * outside the object (otherwise the algorithm gives wrong results). The
	 * matrix representing the camera is translated so that it is on the
	 * same plane as the lower base of the object before doing computations
	 * \param cameraMtr the matrix describing the camera (see function
	 *                  description)
	 * \param rangesAndColors the list of occupied ranges and their color
	 * \param distance this is set to the distance of the object from the
	 *                 camera. It is negative if the object is not visible
	 *                 by the camera
	 * \param maxDistance this is the maximum distance the camera can see.
	 *                    If the object is farther than this distance the
	 *                    function can safely return a negative distance
	 *                    (i.e. the object is not seen) and stop any other
	 *                    computation
	 */
	virtual void computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const;

	/**
	 * \brief Computes the distance and orientation of this object respect
	 *        to the given robot
	 *
	 * This function computes the distance of this object from the given
	 * robot. The distance is computed between the nearest points of the two
	 * objects. The robot is always modelled as a vertical cylindrical
	 * object. The function returns false if it makes no sense to compute
	 * the distance
	 * \param robot the robot from which the distance should be computed
	 * \param distance the computed distance
	 * \param angle the computed orientation
	 * \return false if the distance couldn't be computed
	 */
	virtual bool computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const;

	/**
	 * \brief Returns the radius of this cylinder
	 *
	 * \return the radius of this cylinder
	 */
	real radius() const
	{
		return m_cylinder->radius();
	}

protected:
	/**
	 * \brief Constructor
	 *
	 * \param arena the arena in which this object lives
	 * \param cylinder the cylinder to wrap
	 * \param type the type of cylinder. This can only be one of Cylinder,
	 *             SmallCylinder, BigCylinder or CircularTargetArea. If the
	 *             type is CircularTargetArea, this cannot be made
	 *             non-static
	 */
	Cylinder2DWrapper(Arena* arena, PhyCylinder* cylinder, Type type);

	/**
	 * \brief Destructor
	 */
	~Cylinder2DWrapper();

private:
	/**
	 * \brief The wrapped cylinder
	 */
	PhyCylinder* const m_cylinder;

	/**
	 * \brief The type of cylinder
	 *
	 * This can only be one of Cylinder, SmallCylinder, BigCylinder or
	 * CircularTargetArea. If the type is CircularTargetArea, this cannot be
	 * made non-static
	 */
	const Type m_type;

	/**
	 * \brief The Arena class is friend to be able to instantiate and delete
	 *        this class
	 */
	friend class Arena;
};

/**
 * \brief The subclass of PhyObject2DWrapper wrapping a sphere
 *
 * For the moment the only available spherical object is the LightBulb, which is
 * always kinematic (it doesn't collide with anything). Moreover LightBulbs are
 * not seen by the linear camera
 */
class SALSA_EXPERIMENTS_API Sphere2DWrapper : public PhyObject2DWrapper
{
public:
	/**
	 * \brief Returns a pointer to the wrapped PhyObject
	 *
	 * You should avoid changing the PhyObject directly, use the methods of
	 * this class, instead
	 * \return a pointer to the wrapped PhyObject
	 */
	virtual PhySphere* phyObject();

	/**
	 * \brief Returns a pointer to the wrapped PhyObject (const version)
	 *
	 * \return a const pointer to the wrapped PhyObject
	 */
	virtual const PhySphere* phyObject() const;

	/**
	 * \brief Sets whether the object is static or not
	 *
	 * This object cannot be made non-static
	 * \param s if true the object will be static
	 */
	virtual void setStatic(bool s);

	/**
	 * \brief Sets whether the object is kinematic or not
	 *
	 * This object cannot be made non-kinematic
	 * \param b if true the object is set to kinematic behaviour
	 * \param c if true the object collides with other objects even in
	 *          kinematic mode, influencing them without being influenced
	 *          (technically, we set its mass to 0 in Newton Game Dynamics
	 *          engine)
	 */
	virtual void setKinematic(bool b, bool c = false);

	/**
	 * \brief Returns the type of this wrapper object
	 *
	 * \return the type of this wrapper object
	 */
	virtual Type type() const;

	/**
	 * \brief Sets the position of the object in the plane
	 *
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(real x, real y);

	/**
	 * \brief Computes the portion of a linear the view field occupied by
	 *        this object
	 *
	 * This function returns the angular range occupied by this object on a
	 * linear camera described by the given matrix. The Z axis of the matrix
	 * is the upvector, the camera lies on the XY plane and points towars
	 * the X axis (i.e. angles start from the X axis). If the object is not
	 * visible  by the camera, this function returns a negative distance.
	 * The min angle can be greater than the max angle if the object is
	 * behind the camera. Here we make the assumption that the camera is
	 * outside the object (otherwise the algorithm gives wrong results). The
	 * matrix representing the camera is translated so that it is on the
	 * same plane as the lower base of the object before doing computations
	 * \param cameraMtr the matrix describing the camera (see function
	 *                  description)
	 * \param rangesAndColors the list of occupied ranges and their color
	 * \param distance this is set to the distance of the object from the
	 *                 camera. It is negative if the object is not visible
	 *                 by the camera
	 * \param maxDistance this is the maximum distance the camera can see.
	 *                    If the object is farther than this distance the
	 *                    function can safely return a negative distance
	 *                    (i.e. the object is not seen) and stop any other
	 *                    computation
	 */
	virtual void computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const;

	/**
	 * \brief Computes the distance and orientation of this object respect
	 *        to the given robot
	 *
	 * This function computes the distance of this object from the given
	 * robot. The distance is computed between the nearest points of the two
	 * objects. The robot is always modelled as a vertical cylindrical
	 * object. The function returns false if it makes no sense to compute
	 * the distance
	 * \param robot the robot from which the distance should be computed
	 * \param distance the computed distance
	 * \param angle the computed orientation
	 * \return false if the distance couldn't be computed
	 */
	virtual bool computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const;

	/**
	 * \brief Returns the radius of this sphere
	 *
	 * \return the radius of this sphere
	 */
	real radius() const
	{
		return m_sphere->radius();
	}

protected:
	/**
	 * \brief Constructor
	 *
	 * \param arena the arena in which this object lives
	 * \param sphere the sphere to wrap
	 * \param type the type of sphere. For the moment this can only be
	 *             LightBulb (different values are discarded).
	 */
	Sphere2DWrapper(Arena* arena, PhySphere* sphere, Type type);

	/**
	 * \brief Destructor
	 */
	~Sphere2DWrapper();

private:
	/**
	 * \brief The wrapped sphere
	 */
	PhySphere* const m_sphere;

	/**
	 * \brief The type of sphere
	 *
	 * This can only be LightBulb
	 */
	const Type m_type;

	/**
	 * \brief The Arena class is friend to be able to instantiate and delete
	 *        this class
	 */
	friend class Arena;
};

/**
 * \brief The subclass of PhyObject2DWrapper wrapping a wheeled robot
 */
class SALSA_EXPERIMENTS_API WheeledRobot2DWrapper : public PhyObject2DWrapper
{
public:

	/**
	 * \brief Returns a pointer to the wrapped object as a RobotOnPlane
	 *
	 * You should avoid changing the wrapped directly, use the methods of
	 * this class, instead
	 * \return a pointer to the wrapped object
	 */
	RobotOnPlane* robotOnPlane();

	/**
	 * \brief Returns a pointer to the wrapped object as a RobotOnPlane
	 *        (const version)
	 *
	 * \return a const pointer to the wrapped object
	 */
	const RobotOnPlane* robotOnPlane() const;

	/**
	 * \brief Returns a pointer to the wrapped object
	 *
	 * You should avoid changing the wrapped directly, use the methods of
	 * this class, instead
	 * \return a pointer to the wrapped object
	 */
	virtual WObject* wObject();

	/**
	 * \brief Returns a pointer to the wrapped object (const version)
	 *
	 * \return a const pointer to the wrapped object
	 */
	virtual const WObject* wObject() const;

	/**
	 * \brief Returns a pointer to the wrapped PhyObject
	 *
	 * This always returns nullptr, as robots are not subclasses of PhyObject
	 * \return always nullptr
	 */
	virtual PhyObject* phyObject();

	/**
	 * \brief Returns a pointer to the wrapper PhyObject (const version)
	 *
	 * This always returns nullptr, as robots are not subclasses of PhyObject
	 * \return always nullptr
	 */
	virtual const PhyObject* phyObject() const;

	/**
	 * \brief Returns the type of this wrapped object
	 *
	 * \return the type of this wrapped object
	 */
	virtual Type type() const;

	/**
	 * \brief Sets the position of the object in the plane
	 *
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(real x, real y);

	/**
	 * \brief Computes the portion of a linear the view field occupied by
	 *        this object
	 *
	 * This function returns the angular range occupied by this object on a
	 * linear camera described by the given matrix. The Z axis of the matrix
	 * is the upvector, the camera lies on the XY plane and points towars
	 * the X axis (i.e. angles start from the X axis). If the object is not
	 * visible  by the camera, this function returns a negative distance.
	 * The min angle can be greater than the max angle if the object is
	 * behind the camera. Here we make the assumption that the camera is
	 * outside the object (otherwise the algorithm gives wrong results). The
	 * matrix representing the camera is translated so that it is on the
	 * same plane as the object base before doing computations
	 * \param cameraMtr the matrix describing the camera (see function
	 *                  description)
	 * \param rangesAndColors the list of occupied ranges and their color
	 * \param distance this is set to the distance of the object from the
	 *                 camera. It is negative if the object is not visible
	 *                 by the camera
	 * \param maxDistance this is the maximum distance the camera can see.
	 *                    If the object is farther than this distance the
	 *                    function can safely return a negative distance
	 *                    (i.e. the object is not seen) and stop any other
	 *                    computation
	 */
	virtual void computeLinearViewFieldOccupiedRange(const wMatrix& cameraMtr, QVector<AngularRangeAndColor>& rangesAndColors, double& distance, double maxDistance) const;

	/**
	 * \brief Computes the distance and orientation of this object respect
	 *        to the given robot
	 *
	 * This function computes the distance of this object from the given
	 * robot. The distance is computed between the nearest points of the two
	 * objects. The robot is always modelled as a vertical cylindrical
	 * object. The function returns false if it makes no sense to compute
	 * the distance (e.g. if robot is the same as this)
	 * \param robot the robot from which the distance should be computed
	 * \param distance the computed distance
	 * \param angle the computed orientation
	 * \return false if the distance couldn't be computed
	 */
	virtual bool computeDistanceAndOrientationFromRobot(const WheeledRobot2DWrapper& robot, double& distance, double& angle) const;

	/**
	 * \brief Returns the height of the robot
	 *
	 * \return the height of the robot
	 */
	double getHeight() const
	{
		return m_height;
	}

	/**
	 * \brief Returns the radius of the robot
	 *
	 * \return the radius of the robot
	 */
	double getRadius() const
	{
		return m_radius;
	}

protected:
	/**
	 * \brief Constructor
	 *
	 * \param arena the arena in which this object lives
	 * \param robot the robot to wrap
	 * \param height the height of the robot
	 * \param radius the radius of the robot
	 */
	WheeledRobot2DWrapper(Arena* arena, RobotOnPlane* robot, double height, double radius);

	/**
	 * \brief Destructor
	 */
	~WheeledRobot2DWrapper();

private:
	/**
	 * \brief The wrapped robot
	 */
	RobotOnPlane* const m_robot;

	/**
	 * \brief The height of the robot
	 */
	const double m_height;

	/**
	 * \brief The radius of the robot
	 */
	const double m_radius;

	/**
	 * \brief The Arena class is friend to be able to instantiate and delete
	 *        this class
	 */
	friend class Arena;
};


/**
 * \brief Returns the 3D position on the plane given a 2D position
 *
 * This is an helper function that can be used to help moving 3D objects on the
 * arena plane. The returned position lies on the plane, translate it as needed
 * \param plane the plane on which the robot is placed. You can use the plane
 *              returned by Arena::getPlane(), here.
 * \param x the x position on the plane
 * \param y the y position on the plane
 * \return the 3D point on the plane corresponding to the given 2D coordinates
 */
wVector SALSA_EXPERIMENTS_API positionOnPlane(const Box2DWrapper* plane, real x, real y);

/**
 * \brief Returns a transformation matrix with the given orientation on the
 *        plane
 *
 * The position in the mtr matrix is not changed, but axes are changed so that z
 * is coincident with the plane local z axis and the orientation is given by
 * angle. An angle of 0 means that the X axis of the plane and of the robot are
 * coincident (positive angles follow the right-hand rule). This function
 * expects the angle to be in radiants
 * \param plane the plane on which the robot is placed. You can use the plane
 *              returned by Arena::getPlane(), here.
 * \param angle the new orientation in radiants
 * \param mtr the initial transformation that is changed by the function so that
 *            it has the new orientation
 */
void SALSA_EXPERIMENTS_API orientationOnPlane(const Box2DWrapper* plane, real angle, wMatrix& mtr);

/**
 * \brief Returns the angle between the two x axis of the two given matrices
 *        projected on the XY plane of the first matrix
 *
 * This returns the angle in radiants between the projection of the two x axis
 * of the two matrices on the xy plane of the first matrix. The result is only
 * correct for sure if the two z axes are almost parallel
 * \param mtr1 the first matrix
 * \param mtr2 the second matrix
 * \return the angle between the two x axes
 */
real SALSA_EXPERIMENTS_API angleBetweenXAxes(const wMatrix& mtr1, const wMatrix& mtr2);

} // end namespace salsa

#endif
