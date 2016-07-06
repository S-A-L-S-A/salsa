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

#ifndef PHYJOINT_H
#define PHYJOINT_H

#include "worldsimconfig.h"
#include "world.h"
#include "phyobject.h"
#include "wvector.h"
#include "wmatrix.h"
#include "wquaternion.h"
#include <QObject>

#include <vector>

namespace farsa {

class PhyJoint;
class PhyJointPrivate;
class PhyObjectPrivate;
class WorldPrivate;
class Motor;
class RenderPhyJoint;

/**
 * \brief The shared data for a PhyDOF
 *
 * This is used inside PhyJointShared to maintain the status of DOFs and a
 * reference is also stored inside each DOF (it is not a subclass of
 * WEntityShared because it is used internally by joints and DOFs)
 */
class FARSA_WSIM_TEMPLATE PhyDOFShared
{
public:
	/**
	 * \brief The type that encodes the current modality of motion (Off
	 *        means motor switched off)
	 */
	enum MotionMode {
		Force,
		Velocity,
		Position,
		Off
	};

public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyDOFShared()
		: axis(0.0, 0.0, 0.0)
		, xAxis(0.0, 0.0, 0.0)
		, yAxis(0.0, 0.0, 0.0)
		, centre(0.0, 0.0, 0.0)
		, isTranslate(false)
		, stiffness(0.99f)
		, forcea(0.0)
		, desiredPos(0.0)
		, desiredVel(0.0)
		, motionMode(Position)
		, position(0.0)
		, velocity(0.0)
		, limitsOn(false)
		, loLimit(-3.14f)
		, hiLimit(3.14f)
		, maxVelocity(1.5708f)
		, maxForce(-1.0)
	{
	}

	/**
	 * \brief The axis of rotation/translation of the DOF
	 */
	wVector axis;

	/**
	 * \brief The X axis of the DOF
	 */
	wVector xAxis;

	/**
	 * \brief The Y axis of the DOF
	 */
	wVector yAxis;

	/**
	 * \brief The center of the translation/rotation frame
	 */
	wVector centre;

	/**
	 * \brief If true this is a translational DOF
	 */
	bool isTranslate;

	/**
	 * \brief The stiffness of the DOF
	 */
	real stiffness;

	/**
	 * \brief The force/torque applied using applyForce
	 */
	real forcea;

	/**
	 * \brief The desired position of this DOF
	 */
	real desiredPos;

	/**
	 * \brief The desired Velocity of this DOF
	 */
	real desiredVel;

	/**
	 * \brief The mode of motion of this DOF
	 */
	MotionMode motionMode;

	/**
	 * \brief The position of the joint
	 *
	 * This is set by PhyJoint because PhyDOF doesn't know the coordinate
	 * reference frame
	 */
	real position;

	/**
	 * \brief The velocity of the joint
	 */
	real velocity;

	/**
	 * \brief If true, the rotation/translation of the DOF is limited
	 */
	bool limitsOn;

	/**
	 * \brief Lower limit
	 */
	real loLimit;

	/**
	 * \brief Upper limit
	 */
	real hiLimit;

	/**
	 * \brief The maximum speed of the joint (rad/sec or m/sec)
	 */
	real maxVelocity;

	/**
	 * \brief The maximum force/torque applied by the joint
	 *
	 * If negative there is no limit
	 */
	real maxForce;
};

/**
 * \brief PhyDOF class
 *
 * Model a single degree of freedom of a joint. This is a subclass of QObject,
 * it defines signals to which to connect to be notified of changes in the
 * status of a joint. Constructor and destructor are private because objects of
 * this kind are only created and destroyed by PhyJoint
 */
class FARSA_WSIM_API PhyDOF : public QObject
{
	Q_OBJECT

public:
	/**
	 * \brief The type that encodes the current modality of motion
	 *
	 * The enum is defined inside PhyDOFShared, the typedef here is just for
	 * convenience
	 */
	typedef PhyDOFShared::MotionMode MotionMode;

private:
	/**
	 * \brief Constructor
	 *
	 * This is private so that it can only be called by PhyJoint
	 * \param parent the joint to which this DOF belongs
	 * \param shared the structure with the status of the DOF
	 * \param axis is the main axis of translation/rotation
	 * \param centre is the centre of translation/rotation frame
	 * \param translate true mean that this is a linear DOF, otherwise this
	 *                  is a rotational DOF
	 */
	PhyDOF(PhyJoint* parent, PhyDOFShared* shared, wVector axis, wVector centre, bool translate)
		: m_parent(parent)
		, m_shared(shared)
	{
		m_shared->axis = axis;
		m_shared->centre = centre;
		m_shared->isTranslate = translate;
	};

	/**
	 * \brief Destructor
	 *
	 * This is private so that it can only be called by PhyJoint
	 */
	virtual ~PhyDOF()
	{
	}

public:
	/**
	 * \brief Switches the motor off
	 */
	void switchOff()
	{
		m_shared->motionMode = PhyDOFShared::Off;
	}

	/**
	 * \brief Returns the PhyJoint to which this DOF belongs
	 *
	 * \return the PhyJoint to which this DOF belongs
	 */
	PhyJoint* joint()
	{
		return m_parent;
	}

	/**
	 * \brief Returns the PhyJoint to which this DOF belongs (const version)
	 *
	 * \return the PhyJoint to which this DOF belongs
	 */
	const PhyJoint* joint() const
	{
		return m_parent;
	}

	/**
	 * \brief Returns the Force/Torque applied using applyForce
	 *
	 * \return the Force/Torque applied using applyForce
	 */
	real appliedForce() const
	{
		return m_shared->forcea;
	}

	/**
	 * \brief Returns the desired position
	 *
	 * \return the desired position
	 */
	real desiredPosition() const
	{
		return m_shared->desiredPos;
	}

	/**
	 * \brief Returns the desired velocity
	 *
	 * \return the desired velocity
	 */
	real desiredVelocity() const
	{
		return m_shared->desiredVel;
	}

	/**
	 * \brief Returns the current motion mode
	 *
	 * \return the current motion mode
	 */
	PhyDOFShared::MotionMode motion() const {
		return m_shared->motionMode;
	}

	/**
	 * \brief Changes the modality of motion
	 *
	 * \param m the new modality of motion
	 */
	void setMotionMode(PhyDOFShared::MotionMode m)
	{
		m_shared->motionMode = m;
	}

	/**
	 * \brief Returns the centre of rotation/translation
	 *
	 * \return the centre of rotation/translation
	 */
	const wVector& centre() const
	{
		return m_shared->centre;
	}

	/**
	 * \brief Returns the current position of bodies
	 *
	 * For rotational DOFs this is the angle of rotation, for linear DOFs
	 * this is the offset from the centre
	 * \return the current position of bodies
	 */
	real position() const
	{
		return m_shared->position;
	}

	/**
	 * \brief Returns the current relative velocity of bodies along the axis
	 *        of rotation/translation
	 *
	 * \return the current relative velocity of bodies
	 */
	real velocity() const
	{
		return m_shared->velocity;
	}

	/**
	 * \brief Returns the limits
	 *
	 * \param lo the variable that will contain the lower limit
	 * \param hi the variable that will contain the upper limit
	 */
	void limits(real& lo, real& hi) const
	{
		lo = m_shared->loLimit;
		hi = m_shared->hiLimit;
	}

	/**
	 * \brief Returns the stiffness
	 *
	 * \return the stiffness
	 */
	real stiffness() const
	{
		return m_shared->stiffness;
	}

	/**
	 * \brief Sets the stiffness of this DOF
	 *
	 * \param newStiff the new value of the stiffness
	 */
	void setStiffness(real newStiff)
	{
		// Clamp newstiff between 0 and 0.99
		m_shared->stiffness = ramp(0.0f, 0.99f, newStiff);
		emit changedStiffness(m_shared->stiffness);
	}

	/**
	 * \brief Sets the maximum velocity for the DOF (rad/sec or m/sec)
	 *
	 * \param maxVel the new maximum velocity for the DOF
	 */
	void setMaxVelocity(real maxVel)
	{
		m_shared->maxVelocity = maxVel;
	}

	/**
	 * \brief Returns the maximum velocity (rad/sec or m/sec)
	 *
	 * \return the maximum velocity
	 */
	real maxVelocity() const
	{
		return m_shared->maxVelocity;
	}

	/**
	 * \brief Sets the maximum force/torque applied by the joint
	 *
	 * \param maxForce the maximum force/torque applied by the joint. A
	 *                 negative value means no limit
	 */
	void setMaxForce(real maxForce)
	{
		m_shared->maxForce = maxForce;
	}

	/**
	 * \brief Returns the maximum force/torque
	 *
	 * \return the maximum force/torque. A negative value means no limit
	 */
	real maxForce() const
	{
		return m_shared->maxForce;
	}

	/**
	 * \brief Returns the axis of rotation/translation
	 *
	 * \return the axis of rotation/translation
	 */
	const wVector& axis() const
	{
		return m_shared->axis;
	}

	/**
	 * \brief Returns the X axis of local frame of reference of the DOF
	 *
	 * This is the zero angle position
	 * \return the X axis of local frame of reference of the DOF
	 */
	const wVector& xAxis() const
	{
		return m_shared->xAxis;
	}

	/**
	 * \brief Returns the Y axis of local frame of reference of the DOF
	 *
	 * \return the Y axis of local frame of reference of the DOF
	 */
	const wVector& yAxis() const
	{
		return m_shared->yAxis;
	}

	/**
	 * \brief Returns true if this is a translational DOF
	 *
	 * \return true if this is a translational DOF
	 */
	bool translate() const
	{
		return m_shared->isTranslate;
	}

	/**
	 * \brief Returns true if this is a rotational DOF
	 *
	 * \return true if this is a rotational DOF
	 */
	bool rotate() const
	{
		return !m_shared->isTranslate;
	}

	/**
	 * \brief Returns true if rotation/translation is limited
	 *
	 * \return true if rotation/translation is limited
	 */
	bool isLimited() const
	{
		return m_shared->limitsOn;
	}

	/**
	 * \brief Enables limits
	 */
	void enableLimits()
	{
		m_shared->limitsOn = true;
	}

	/**
	 * \brief Disables limits
	 */
	void disableLimits()
	{
		m_shared->limitsOn = false;
	}

	/**
	 * \brief Sets the position
	 *
	 * This is called by joints to update information
	 * \param newPos the new position
	 * \internal
	 */
	void setPosition(real newPos)
	{
		m_shared->position = newPos;
		emit changedPosition(newPos);
	}

	/**
	 * \brief Sets the velocity
	 *
	 * This is called by joints to update information
	 * \param newVel the new velocity
	 * \internal
	 */
	void setVelocity(real newVel)
	{
		m_shared->velocity = newVel;
		emit changedVelocity(newVel);
	}

public slots:
	/**
	 * \brief Applies a force to the objects linked by this DOF
	 *
	 * For prismatic DOF this means that the force will be applied to both
	 * bodies in opposite direction along axis of DOF, while for rotational
	 * DOF the force will be intepreted as a torque applied around the axis
	 * of the DOF
	 * \param force the force to apply
	 */
	void applyForce(real force)
	{
		m_shared->forcea = force;
		m_shared->motionMode = PhyDOFShared::Force;
		emit appliedForce(force);
	}

	/**
	 * \brief Accelerates the DOF to the velocity passed and tries to stay
	 *        at that velocity
	 *
	 * \param wishVel the desired velocity
	 */
	void setDesiredVelocity(real wishVel)
	{
		m_shared->desiredVel = wishVel;
		m_shared->motionMode = PhyDOFShared::Velocity;
		emit changedDesiredVelocity(wishVel);
	}

	/**
	 * \brief Moves the DOF at the position passed and tries to stay there
	 *
	 * \param wishPos the desired position
	 */
	void setDesiredPosition(real wishPos)
	{
		// Change the current desired position to avoid to push to the limits
		real offset = (fabs(m_shared->hiLimit)-fabs(m_shared->loLimit))*0.005;
		m_shared->desiredPos = ramp(m_shared->loLimit + offset, m_shared->hiLimit - offset, wishPos);
		m_shared->motionMode = PhyDOFShared::Position;
		emit changedDesiredPosition(m_shared->desiredPos);
	}

	/**
	 * \brief Sets limits of this DOF
	 *
	 * The meaning of parameters changes depending if it's a rotational or
	 * translation DOF
	 * \param loLimit the lower limit
	 * \param hiLimit the upper limit
	 * \warning The lower limit must be greater that -pi and the upper limit
	 *          must be lesse that pi. Only when the library is compiled in
	 *          debug, this function will check the correctness of these
	 *          parameters
	 */
	void setLimits(real loLimit, real hiLimit)
	{
#ifdef FARSA_DEBUG
		if (!m_shared->isTranslate && (loLimit <= -PI_GRECO)) {
			qDebug() << "DOF Lower Limit must be greater than -pi";
		}
		if (!m_shared->isTranslate && (hiLimit >= PI_GRECO)) {
			qDebug() << "DOF Higher Limit must be lesser that pi";
		}
#endif
		m_shared->loLimit = loLimit;
		m_shared->hiLimit = hiLimit;

		// Changes the current desired position to avoid to push to the limits
		real offset = (fabs(hiLimit)-fabs(loLimit))*0.005;
		m_shared->desiredPos = ramp(loLimit + offset, hiLimit - offset, m_shared->desiredPos);
		emit changedLimits(loLimit, hiLimit);
	};

signals:
	/**
	 * \brief The signal emitted when a force/torque is applied
	 *
	 * \param force the force/torque applied
	 */
	void appliedForce(real force);

	/**
	 * \brief The signal emitted when the desired position changes
	 *
	 * \param wishPos the new desired position
	 */
	void changedDesiredPosition(real wishPos);

	/**
	 * \brief The signal emitted when the desired velocity changes
	 *
	 * \param wishVel the new desired velocity
	 */
	void changedDesiredVelocity(real wishVel);

	/**
	 * \brief The signal emitted when position changes
	 *
	 * \param newPos the new position
	 */
	void changedPosition(real newPos);

	/**
	 * \brief The signal emitted when velocity changes
	 *
	 * \param newVel the new velocity
	 */
	void changedVelocity(real newVel);

	/**
	 * \brief The signal emitted when stiffness changes
	 *
	 * \param newStiff the new stiffness
	 */
	void changedStiffness(real newStiff);

	/**
	 * \brief The signal emitted when limits changes
	 *
	 * \param loLimit the new lower limit
	 * \param hiLimit the new upper limit
	 */
	void changedLimits(real loLimit, real hiLimit);

private:
	/**
	 * \brief The joint to which this DOF belongs
	 */
	PhyJoint* const m_parent;

	/**
	 * \brief The structure with the status of the DOF
	 */
	PhyDOFShared* const m_shared;

	/**
	 * \brief PhyJoint is friend to be able to create and destroy objects of
	 *        this type
	 */
	friend class PhyJoint;
};

/**
 * \brief The shared data for a PhyJoint
 */
class FARSA_WSIM_TEMPLATE PhyJointShared : public WEntityShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default values
	 */
	PhyJointShared()
		: WEntityShared()
		, dofs()
		, enabled(true)
	{
	}

	/**
	 * \brief The vector with DOF's data
	 */
#warning QVector HERE LEADS TO A CRASH, std::vector DOESN T. PERHAPS IT IS A PROBLEM RELATED TO IMPLICIT SHARING AND MULTITHREAD (QT BUG?)
	std::vector<PhyDOFShared> dofs;
	// QVector<PhyDOFShared> dofs;

	/**
	 * \brief True if the joint is enabled, false otherwise
	 */
	bool enabled;

	/**
	 * \brief The "length" of the joint
	 *
	 * This is a value that is only used to draw the joint
	 */
	farsa::real length;

	/**
	 * \brief The "radius" of the joint
	 *
	 * This is a value that is only used to draw the joint
	 */
	farsa::real radius;
};

/**
 * \brief The base class for joints
 *
 * This joint doesn't add constraints... so, it's a zero-DOF joint :-). This is
 * not instantiable as it is an abstract class. Note that if one of the linked
 * object is destroyed, the joint is destroyed, too
 */
class FARSA_WSIM_API PhyJoint : public WEntity
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyJointShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyJoint Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * Inside child classes you can use the createDOFs() utility function to
	 * create the needed number of PhyDOFs
	 * \param world the world where entity lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               entity has been deleted)
	 * \param parent the parent object. The local frame is translated
	 *               towards centre to create the complete local coordinate
	 *               frame of this joint
	 * \param child the child object
	 * \param name the name of the object
	 */
	PhyJoint(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	virtual ~PhyJoint();

public:
	/**
	 * \brief Returns the child object attached to this joint
	 *
	 * See Newton Documentation
	 * \return the child object attached to this joint
	 */
	PhyObject* child()
	{
		return m_child;
	}

	/**
	 * \brief Returns the child object attached to this joint (const
	 *        version)
	 *
	 * See Newton Documentation
	 * \return the child object attached to this joint
	 */
	const PhyObject* child() const
	{
		return m_child;
	}

	/**
	 * \brief Returns the parent object
	 *
	 * A NULL return value means that the joint is attached to a fixed point
	 * in the world; see Netwon Documentation
	 * \return the parent object attached to this joint
	 */
	PhyObject* parent()
	{
		return m_parent;
	}

	/**
	 * \brief Returns the parent object (const version)
	 *
	 * A NULL return value means that the joint is attached to a fixed point
	 * in the world; see Netwon Documentation
	 * \return the parent object attached to this joint
	 */
	const PhyObject* parent() const
	{
		return m_parent;
	}

	/**
	 * \brief Returns the number of DOF of this joint
	 *
	 * \return the number of DOF of this joint
	 */
	unsigned int numDofs() const
	{
		return m_dofs.size();
	}

	/**
	 * \brief Returns the DOFs
	 *
	 * \return the vector of DOFs
	 */
	QVector<PhyDOF*> dofs()
	{
		return m_dofs;
	}

	/**
	 * \brief Returns the DOFs (const version)
	 *
	 * \return the vector of DOFs
	 */
	const QVector<PhyDOF*>& dofs() const
	{
		return m_dofs;
	}

	/**
	 * \brief Enables or disables the joint
	 *
	 * \param b if true the joint is enabled, if false it is disabled
	 */
	void enable(bool b);

	/**
	 * \brief Returns true if the joint is enabled
	 *
	 * \return true if the joint is enabled
	 */
	bool isEnabled() const
	{
		return m_shared->enabled;
	}

	/**
	 * \brief Returns the centre of this joint in world coordinate
	 *
	 * \return the centre of this joint in world coordinate
	 */
	virtual wVector centre() const = 0;

	/**
	 * \brief Returns the force applied to this joint
	 *
	 * \return the force applied to this joint
	 */
	virtual wVector getForceOnJoint() const = 0;

	/**
	 * \brief Updates the joint information
	 *
	 * All PhyDOFs will be updated
	 */
	virtual void updateJointInfo() = 0;

protected:
	/**
	 * \brief The structure that contains the initialization parameters for
	 *        a DOF
	 *
	 * This is used to pass initialization parameters of DOFs to the
	 * createDOFs() function
	 */
	struct PhyDOFInitParams {
		/**
		 * \brief Constructor
		 *
		 * \param a the main axis of translation/rotation
		 * \param c the centre of translation/rotation frame
		 * \param t if true it mean that this is a linear DOF, otherwise
		 *          this is a rotational DOF
		 */
		PhyDOFInitParams(wVector a, wVector c, bool t)
			: axis(a)
			, centre(c)
			, translate(t)
		{
		}

		/**
		 * \brief The main axis of translation/rotation
		 */
		wVector axis;

		/**
		 * \brief The centre of translation/rotation frame
		 */
		wVector centre;

		/**
		 * \brief If true it mean that this is a linear DOF, otherwise
		 *        this is a rotational DOF
		 */
		bool translate;
	};

	/**
	 * \brief The type for the list of DOFs initialization parameters
	 */
	typedef QList<PhyDOFInitParams> PhyDOFInitParamsList;

	/**
	 * \brief Creates dofs
	 *
	 * This can be called in the constructor of child classes to set up the
	 * DOFs
	 * \param initParams the list with initialization parameters. The number
	 *                   of DOFs that will be created is equal to the number
	 *                   of elements in the list
	 */
	void createDOFs(const PhyDOFInitParamsList& initParams);

	/**
	 * \brief Creates the objects needed by the underlying physics engine
	 *
	 * This is called by world after the joint has been constructed (so
	 * that the correct virtual function is called)
	 */
	virtual void createPrivateJoint() = 0;

	/**
	 * \brief This is called after createPrivateJoint()
	 *
	 * Re-implement if you need to perform actions after the physics engine
	 * has been initialized. The default implementation does nothing
	 */
	virtual void postCreatePrivateJoint();

	/**
	 * \brief Updates the joint (called by Newton callback)
	 *
	 * \param timestep the size of the timestep
	 */
	virtual void updateJoint(real timestep) = 0;

	/**
	 * \brief The shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief The vector of DOFs
	 */
	QVector<PhyDOF*> m_dofs;

	/**
	 * \brief The parent object
	 */
	PhyObject* const m_parent;

	/**
	 * \brief The child object
	 */
	PhyObject* const m_child;

	/**
	 * \brief The private implementation
	 */
	PhyJointPrivate* const m_priv;

	/**
	 * \brief The private implementation of the parent object
	 */
	PhyObjectPrivate* const m_parentPriv;

	/**
	 * \brief The private implementation of the child object
	 */
	PhyObjectPrivate* const m_childPriv;

	/**
	 * \brief The private implementation of world
	 */
	WorldPrivate* const m_worldPriv;

	/**
	 * \brief PhyJointPrivate is friend to access private members
	 */
	friend class PhyJointPrivate;

	/**
	 * \brief World is friend to access m_priv and call both
	 *        createPrivateObject() and postCreatePrivateObject() and to
	 *        create instances
	 */
	friend class World;
};

/**
 * \brief The class rendering the PhyJoint
 *
 * This class doesn't do the rendering, it only provides few utility functions
 * that can be used by subclasses to do the actual drawing
 */
class FARSA_WSIM_API RenderPhyJoint : public RenderWEntity
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
	RenderPhyJoint(const PhyJoint* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyJoint();

protected:
	/**
	 * \brief The function rendering a joint
	 *
	 * You can simply call this in the render() function of the subclass to
	 * render a joint
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 * \param center the center of the joint
	 * \param end1 the point on the first object where the joint is attached
	 * \param end2 the point on the second object where the joint is
	 *             attached
	 */
	void drawJoint(const PhyJointShared* sharedData, GLContextAndData* contextAndData, const wVector& center, const wVector& end1, const wVector& end2);
};

} // end namespace farsa

#endif
