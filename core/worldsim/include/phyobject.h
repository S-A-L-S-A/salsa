/********************************************************************************
 *  FARSA                                                                       *
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

#ifndef PHYOBJECT_H
#define PHYOBJECT_H

#include "worldsimconfig.h"
#include "wvector.h"
#include "wmatrix.h"
#include "wquaternion.h"
#include "wobject.h"

namespace farsa {

class World;
class PhyJoint;
class PhyObjectPrivate;
class WorldPrivate;
class PhyCompoundObject;
class RenderPhyObject;

/**
 * \brief The shared data for the PhyObject
 */
class FARSA_WSIM_TEMPLATE PhyObjectShared : public WObjectShared
{
public:
	/**
	 * \brief Additional flags for the PhyObject
	 *
	 * This are in addition to flags from WObject
	 */
	enum PhyObjectFlags {
		DrawForces = 0x08 /// If set forces and torques are drawn
	};

public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyObjectShared()
		: WObjectShared()
		, material("default")
		, forceAcc(0.0f, 0.0f, 0.0f)
		, torqueAcc(0.0f, 0.0f, 0.0f)
		, isKinematic(false)
		, isKinematicCollidable(false)
		, isStatic(false)
		, objInertiaVec(0.0f, 1.0f, 1.0f, 1.0f)
		, objInvInertiaVec(0.0f, 1.0f, 1.0f, 1.0f)
		, collisionShapeOffset(wMatrix::identity())
	{
	}

	/**
	 * \brief The material of the object
	 */
	QString material;

	/**
	 * \brief The force accumulated on the object
	 */
	wVector forceAcc;

	/**
	 * \brief The torque accumulated on the object
	 */
	wVector torqueAcc;

	/**
	 * \brief whether the object has a kinematic behavour or not (see
	 *        PhyObject::setKinematic)
	 */
	bool isKinematic;

	/**
	 * \brief Whether the object is collidable even if it is kinematic (see
	 *        setKinematic)
	 */
	bool isKinematicCollidable;

	/**
	 * \brief Whether the object is static or not
	 */
	bool isStatic;

	/**
	 * \brief The inertia of the object
	 *
	 * The content of the vector is: (mass, Ixx, Iyy, Izz)
	 */
	wVector objInertiaVec;

	/**
	 * \brief The inverse inertia of the object
	 */
	wVector objInvInertiaVec;

	/**
	 * \brief The offset of the collision shape with respect to the object
	 *        center
	 */
	wMatrix collisionShapeOffset;
};

/**
 * \brief The base class for physical objects in the World
 */
class FARSA_WSIM_API PhyObject : public WObject
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyObjectShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyObject Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * Create a physics object with no collision-shape
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PhyObject(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyObject();

public:
	/**
	 * \brief Changes between kinematic/dynamic behaviour for the object
	 *
	 * A kinematic object doesn't interact at all with other objects; it can
	 * only be moved directly setting its transformation matrix. By default
	 * the object is dynamic
	 * \param b if true the object is set to kinematic behaviour
	 * \param c if true the object collides with other objects even in
	 *          kinematic mode, influencing them without being influenced
	 *          (technically, we set its mass to 0 in Newton Game Dynamics
	 *          engine)
	 */
	void setKinematic(bool b, bool c = false);

	/**
	 * \brief Returns true if the object has kinematic behaviour
	 *
	 * \return true if the object has kinematic behaviour
	 */
	bool getKinematic() const
	{
		return m_shared->isKinematic;
	}

	/**
	 * \brief Returns true if this object collides with other object.
	 *
	 * To collide with other objects, this must be non-kinematic or, if it
	 * is, setKinematic must have been called with the c parameter set to
	 * true
	 * \return true if this object collides with other object.
	 */
	bool isCollidable() const
	{
		return !m_shared->isKinematic || m_shared->isKinematicCollidable;
	}

	/**
	 * \brief Makes the object static or not
	 *
	 * A static object is unaffected by external forces
	 * \param b if true the object is static
	 */
	void setStatic(bool b);

	/**
	 * \brief Returns true if the object is static
	 *
	 * \return true if the object is static
	 */
	bool getStatic() const
	{
		return m_shared->isStatic;
	}

	/**
	 * \brief Resets the object status
	 *
	 * This function:
	 * 	- sets the velocity to zero;
	 * 	- sets the angural velocity to zero;
	 * 	- sets any residual forces and torques to zero;
	 * 	- removes any pending collision
	 */
	void reset();

	/**
	 * \brief Adds a force on the object
	 *
	 * \param force the force to apply to the object
	 */
	void addForce(const wVector& force);

	/**
	 * \brief Sets the force on the object
	 *
	 * \param force the total force on the object
	 */
	void setForce(const wVector& force);

	/**
	 * \brief The force on the object
	 *
	 * \return the force on the object
	 */
	wVector force();

	/**
	 * \brief Adds a torque on the object
	 *
	 * \param force the torque to apply to the object
	 */
	void addTorque(const wVector& torque);

	/**
	 * \brief Sets the torque on the object
	 *
	 * \param force the total torque on the object
	 */
	void setTorque(const wVector& torque);

	/**
	 * \brief The torque on the object
	 *
	 * \return the torque on the object
	 */
	wVector torque();

	/**
	 * \brief Sets the Mass and momentum of Inertia
	 *
	 * \param mi the vector with mass and inertia. The vector content is:
	 *           (mass, Ixx, Iyy, Izz)
	 */
	void setMassInertiaVec(const wVector& mi);

	/**
	 * \brief Returns the mass and momentum of inertia
	 *
	 * \return a vector with mass and momentum of inertia
	 */
	const wVector& massInertiaVec() const;

	/**
	 * \brief Returns the vector with the momentum of inertia
	 *
	 * \return the vector with the momentum of inertia
	 */
	wVector inertiaVec() const;

	/**
	 * \brief Returns the inverse mass and inertia vector
	 *
	 * \return the inverse mass and inertia vector
	 */
	const wVector& invMassInertiaVec() const;

	/**
	 * \brief Retursn the inverse inertia vector
	 *
	 * \return the inverse inertia vector
	 */
	wVector invInertiaVec() const;

	/**
	 * \brief Sets the mass
	 *
	 * \param newmass the new mass of the object
	 */
	void setMass(real newmass);

	/**
	 * \brief Returns the mass
	 *
	 * \return the mass
	 */
	real mass() const;

	/**
	 * \brief Sets the angular velocity
	 *
	 * \param omega the angular velocity
	 */
	void setOmega(const wVector& omega);

	/**
	 * \brief Returns the angular velocity
	 *
	 * \return the angular velocity
	 */
	wVector omega();

	/**
	 * \brief Sets the linear velocity
	 *
	 * \param velocity the linear velocity
	 */
	void setVelocity(const wVector& velocity);

	/**
	 * \brief Returns the linear velocity
	 *
	 * \return the linear velocity
	 */
	wVector velocity();

	/**
	 * \brief Adds an impulse
	 *
	 * \param pointDeltaVeloc the velocity variation at pointPosit
	 * \param pointPosit the point where the impulse is applied
	 */
	void addImpulse(const wVector& pointDeltaVeloc, const wVector& pointPosit);

	/**
	 * \brief Sets the material of the object
	 *
	 * \param material the material of the object
	 */
	void setMaterial(QString material);

	/**
	 * \brief Returns the material of the object
	 *
	 * \return the material of the object
	 */
	QString material() const;

	/**
	 * \brief Returns true if forces and torques should be drawn
	 *
	 * \return true if forces and torques should be drawn
	 */
	bool forcesDrawn() const;

	/**
	 * \brief Sets whether forces and torques should be drawn
	 *
	 * By default forces and torques are not drawn. Force vector is drawn in
	 * red, torque in green
	 * \param d if true forces and torques should be drawn
	 */
	void setForcesDrawn(bool d);

protected:
	/**
	 * \brief The virtual function called before the transformation matrix
	 *        is changed
	 *
	 * This prevents a change in the matrix if the private newton object has
	 * been deleted (this can happend if we are part of a compound object)
	 * \return true if the matrix can be changed
	 */
	virtual bool aboutToChangeMatrix();

	/**
	 * \brief Syncronizes the transformation matrix of this object with
	 *        the underlying physic object
	 *
	 * If you need to override this function in child classes remember to
	 * call the parent class function!
	 */
	virtual void changedMatrix();

	/**
	 * \brief Creates the objects needed by the underlying physics engine
	 *
	 * This is called by world after the object has been constructed (so
	 * that the correct virtual function is called)
	 * \param onlyCollisionShape if true only the collision shape is created
	 * \param collisionShapeOffset if not NULL, the offset of the collision
	 *                             shape is set to this matrix
	 */
	virtual void createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset);

	/**
	 * \brief This is called after createPrivateObject()
	 *
	 * Re-implement if you need to perform actions after the physics engine
	 * has been initialized. The default implementation does nothing
	 */
	virtual void postCreatePrivateObject();

	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

#ifdef WORLDSIM_USE_NEWTON
	/**
	 * \brief The object with physics engine related stuff
	 *
	 * This is initialized in createPrivateObject(). It is NULL if we are
	 * part of a compound object
	 */
	PhyObjectPrivate* m_priv;
#endif

	/**
	 * \brief A pointer to physics engine stuffs
	 */
	WorldPrivate* const m_worldPriv;

	/**
	 * \brief PhyObjectPrivate is friend to access m_priv
	 */
	friend class PhyObjectPrivate;

	/**
	 * \brief WorldPrivate is friend to access m_priv
	 */
	friend class WorldPrivate;

	/**
	 * \brief PhyJoint is friend to access m_priv
	 */
	friend class PhyJoint;

	/**
	 * \brief World is friend to access m_priv and call both
	 *        createPrivateObject() and postCreatePrivateObject()
	 */
	friend class World;

	/**
	 * \brief PhyCompoundObject is friend to access m_priv
	 */
	friend class PhyCompoundObject;
};

/**
 * \brief The class rendering the PhyObject
 *
 * The implementation of functions in this class is mostly empty. We only have
 * this class to add utility functions for drawing forces. The AABB and MBBSize
 * are empty
 */
class FARSA_WSIM_API RenderPhyObject : public RenderWObject
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity the entity we render. YOU MUST NOT KEEP A REFERENCE TO
	 *               IT!!! This is only passed in case you need to do custom
	 *               initializations, the render() function will be passed
	 *               updated data at each call externally
	 */
	RenderPhyObject(const PhyObject* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyObject();

	/**
	 * \brief Draws the forces and torques on the object
	 *
	 * This draws the forces and torques on the object. You don't have to
	 * re-implement this for subclasses unless you have particular needings
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void drawForces(const PhyObjectShared* sharedData, GLContextAndData* contextAndData);

protected:
	/**
	 * \brief Returns the transformation matrix of the collision shape in
	 *        the global frame of reference
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 */
	wMatrix collisionShapeMatrix(const PhyObjectShared* sharedData) const;
};

} // end namespace farsa

#endif
