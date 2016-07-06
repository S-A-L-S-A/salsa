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

#ifndef MOTORCONTROLLERS_H
#define MOTORCONTROLLERS_H

#include "worldsimconfig.h"
#include "phyjoint.h"
#include "wentity.h"
#include <QVector>

namespace farsa {

class World;
class PhyDOF;

/*! wPID class
 *
 *  \par Motivation
 *  this class provide an implementation of PID algorithm.
 *  \note this is a C++ adaptation of pidloop example at http://jk-technology.com/C_Unleashed/code_list.html
 */
class FARSA_WSIM_API wPID {
public:
	/*! Constructor */
	wPID();
	/*! it compute a loop of PID algorithm
	 *  \param currentvalue is the current value of variable under the control of PID
	 */
	double pidloop( double currentvalue );
	/*! Resets the PID status
	 */
	void reset();
	/*! 'P' proportional gain          */
	double p_gain;
	/*! 'I' integral gain              */
	double i_gain;
	/*! 'D' derivative gain            */
	double d_gain;
	/*! 'A' acceleration feed forward  */
	double acc_ff;
	/*! 'F' friction feed forward      */
	double fri_ff;
	/*! 'V' velocity feed forward      */
	double vel_ff;
	/*! 'B' bias                       */
	double bias;
	/*! 'R' acceleration rate          */
	double accel_r;
	/*! 'S' set point                  */
	double setpt;
	/*! 'N' minimum output value       */
	double min;
	/*! 'M' maximum output value       */
	double max;
	/*! 'W' maximum slew rate          */
	double slew;
private:
	double this_target;
	double next_target;
	double integral;
	double last_error;
	double last_output;
};

/**
 * \brief MotorController Class
 *
 * A common interface among MotorController hierachy. This has no renderer nor
 * shared data
 */
class FARSA_WSIM_API MotorController : public WEntity
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the motorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               motorcontroller has been deleted)
	 * \param name the name of the object
	 */
	MotorController(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed");

	/**
	 * \brief Destructor
	 *
	 * This deletes all owned object marked to be automatically destroyed
	 */
	virtual ~MotorController();

public:
	/**
	 * \brief This is the pure virtual method implemented by actual motor
	 *        controllers that apply the logic of controller
	 */
	virtual void update() = 0;

	/**
	 * \brief Returns true if enabled (hence if it is on)
	 *
	 * \return true if enabled
	 */
	bool isEnabled()
	{
		return m_enabled;
	}

	/**
	 * \brief Enables or disables this motorcontroller
	 *
	 * \param b if true the motorcontroller is enabled, otherwise it is
	 *          disabled
	 */
	void setEnabled(bool b)
	{
		m_enabled = b;
	}

private:
	/**
	 * \brief If true the motorcontroller is enabled
	 */
	bool m_enabled;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

/**
 * \brief An helper function to compute the kinematic movement of wheeled robots
 *
 * This function takes some parameters of the robot and computes the new
 * transformation given the speed of the two wheels. This function assumes that
 * the robot moves forward towards -Y and that the wheel axis is along X
 * \param mtr the current transformation matrix of the robot. This is changed to
 *            reflect the robot movement when the function returns
 * \param leftWheelVelocity the angular velocity of the left wheel
 * \param rightWheelVelocity the angular velocity of the right wheel
 * \param wheelr the radius of the wheels
 * \param axletrack the distance between the two wheels
 * \param timestep the world timestep
 */
void FARSA_WSIM_API wheeledRobotsComputeKinematicMovement(wMatrix &mtr, real leftWheelVelocity, real rightWheelVelocity, real wheelr, real axletrack, real timestep);

/**
 * \brief An helper function to compute the speed of the wheels of wheeled
 *        robots corresponding to a given kinematic movement
 *
 * This function takes some parameters of the robot and computes the speed of
 * the wheels to move the robot from a point to another. Not all movements are
 * possible with constant wheel speed; in this case this function returns false.
 * Wheels velocities are computed in any case (they try to be as plausible as
 * possible). This function assumes that the robot moves forward towards -Y and
 * that the wheel axis is along X. Moreover it also assumes that the Z axis are
 * parallel (i.e. the robot moves on the plane) and that the robot cannot change
 * its orientation of more than 180° in one step.
 * \param start the transformation matrix of the robot at the initial position
 * \param end the transformation matrix of the robot at the final position
 * \param wheelr the radius of the wheels
 * \param axletrack the distance between the two wheels
 * \param timestep the world timestep
 * \param leftWheelVelocity the computed angular velocity of the left wheel
 * \param rightWheelVelocity the computed angular velocity of the right wheel
 * \return true if the computed velocities are correct
 */
bool FARSA_WSIM_API wheeledRobotsComputeWheelsSpeed(const wMatrix& start, const wMatrix& end, real wheelr, real axletrack, real timestep, real& leftWheelVelocity, real& rightWheelVelocity);

/**
 * \brief The controller of wheels for wheeled robots
 */
class FARSA_WSIM_API WheelMotorController : public MotorController
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the motorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               motorcontroller has been deleted)
	 * \param dofs vector of wheels actuated by this controller
	 * \param name the name of the object
	 */
	WheelMotorController(World* world, SharedDataWrapper<Shared> shared, QVector<PhyDOF*> wheels, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	virtual ~WheelMotorController();

public:
	/**
	 * \brief Returns the wheels controlled
	 *
	 * \return the wheels controlled
	 */
	QVector<PhyDOF*>& wheels()
	{
		return m_motors;
	}

	/**
	 * \brief Applies the velocities on the wheels
	 */
	virtual void update();

	/**
	 * \brief Sets the desired speeds for the wheels
	 *
	 * \param speeds the vector with the speeds of wheels
	 * \note speed is expressed in rad/sec
	 */
	void setSpeeds(const QVector<double>& speeds);

	/**
	 * \brief An utility method for setting speeds of the first two wheels
	 *
	 * \param sp1 the speed of the first wheel
	 * \param sp2 the speed of the second wheel
	 * \note speed is expressed in rad/sec
	 */
	void setSpeeds(double sp1, double sp2);

	/**
	 * \brief Gets the current speed of the wheels
	 *
	 * \param speeds the vector that will contain the speeds
	 * \note speed is expressed in rad/sec
	 */
	void getSpeeds(QVector<double>& speeds) const;

	/**
	 * \brief An utility method to get the current speed of the first two
	 *        wheels
	 *
	 * \param sp1 the speed of the first wheel
	 * \param sp2 the speed of the second wheel
	 * \note speed is expressed in rad/sec
	 */
	void getSpeeds(double& sp1, double& sp2) const;

	/**
	 * \brief Gets the desired speed of the wheels
	 *
	 * \param speeds the vector that will contain the desired speeds
	 * \note speed is expressed in rad/sec
	 */
	void getDesiredSpeeds(QVector<double>& speeds) const;

	/**
	 * \brief An utility method to get the desired speed to the first two
	 *        wheels
	 *
	 * \param sp1 the desired speed of the first wheel
	 * \param sp2 the desired speed of the second wheel
	 * \note speed is expressed in rad/sec
	 */
	void getDesiredSpeeds(double& sp1, double& sp2) const;

	/**
	 * \brief Sets the minimum and maximum velocities for each wheel
	 *
	 * \param minSpeeds the minimum velocities of wheels
	 * \param maxSpeeds the maximum velocities of wheels
	 * \note speed is expressed in rad/sec
	 */
	void setSpeedLimits(const QVector<double>& minSpeeds, const QVector<double>& maxSpeeds);

	/**
	 * \brief An utility method to set the minimum and maximum velocities of
	 *        the first two wheels
	 *
	 * \param minSpeed1 the minimum speed of the first wheel
	 * \param minSpeed2 the minimum speed of the second wheel
	 * \param maxSpeed1 the maximum speed of the first wheel
	 * \param maxSpeed2 the maximum speed of the second wheel
	 *  \note speed is expressed in rad/sec
	 */
	void setSpeedLimits(double minSpeed1, double minSpeed2, double maxSpeed1, double maxSpeed2);

	/**
	 * \brief Returns the minimum and maximum velocities for each wheel
	 *
	 * \param minSpeeds the minimum velocities of wheels
	 * \param maxSpeeds the maximum velocities of wheels
	 * \note speed is expressed in rad/sec
	 */
	void getSpeedLimits(QVector<double>& minSpeeds, QVector<double>& maxSpeeds) const;

	/**
	 * \brief An utility method to get the minimum and maximum velocities of
	 *        the first two wheels
	 *
	 * \param minSpeed1 the minimum speed of the first wheel
	 * \param minSpeed2 the minimum speed of the second wheel
	 * \param maxSpeed1 the maximum speed of the first wheel
	 * \param maxSpeed2 the maximum speed of the second wheel
	 * \note speed is expressed in rad/sec
	 */
	void getSpeedLimits(double& minSpeed1, double& minSpeed2, double& maxSpeed1, double& maxSpeed2) const;

	/**
	 * \brief Sets the maximum allowed torque of wheels
	 *
	 * \param maxTorque the maximum torque of wheels
	 */
	void setMaxTorque(double maxTorque);

	/**
	 * \brief Gets the maximum allowed torque of wheels
	 *
	 * \return the maximum allowed torque of wheels
	 * \note this only reads the max torque of the first dof as methods of
	 *       this class only allow to set all max torques at the same value
	 */
	double getMaxTorque() const;

private:
	/**
	 * \brief The list of controlled motors
	 */
	QVector<PhyDOF*> m_motors;

	/**
	 * \brief The desired velocities
	 */
	QVector<double> m_desiredVel;

	/**
	 * \brief The minimum allowed velocities
	 */
	QVector<double> m_minVel;

	/**
	 * \brief The maximum allowed velocities
	 */
	QVector<double> m_maxVel;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

class PhyMarXbot;
class PhyFixed;

/**
 * \brief The class modelling the motor for the attachment device of the
 *        MarXbot
 *
 * This class models the motors for the attachment device of the MarXbot. The
 * device has two degrees of freedom: the orientation (i.e. where it is relative
 * to the robot turret) and the status, that is whether the device is open,
 * half-closed or closed.
 * Regarding orientation, it is possible to control it by position or by
 * velocity. In both cases the movement is never faster than a maximum speed
 * (that can be changed).
 * When the attachment device is open, it can be rotated using this motor. If it
 * is half-closed or closed and the robot is actually attached to another robot,
 * the motor controlling the rotation is disabled and the attachment device
 * rotates freely. Moreover every command to the attachment device (e.g.
 * setDesiredPosition, setDesiredVelocity...) is ignored. If instead the robot
 * is not attached to another robot, the device behaves normally (as if the
 * attachment device was open).
 * Given two robots A and B, A attaches to B if the following requirements are
 * satisfied:
 * 	- the attachment device of A is in contact with the turret of B and the
 * 	  attachment device of B is far from the turret of A. In other words the
 * 	  two attachment devices must not be in contact;
 * 	- the attachment device of A was in the open status in the previous
 * 	  timestep and is switched to half-closed or closed status in this
 * 	  timestep.
 * The attachment is instantaneous (i.e. it happends in one timestep). The
 * status of the device after the attachment determines how the two robots can
 * move with respect to each other: if it is closed, B cannot rotate its
 * attachment device (a fixed joint is added between the attachment device of A
 * and the turret of B); if it is half-closed, B can rotate its attachment
 * device with the limitation that it can collide with the attachment device of
 * A and so a 360° rotation is not possible (a hinge is added between the
 * attachment device of A and the turret of B).
 * To detach, a robot simply has to change the status of its attachment device
 * to open.
 * This motor only works for dynamic robots. For the attachment process to be
 * successful, both robots must be dynamical and have the attachment device
 * enabled. If the robot attachment device is not enabled, changes to the device
 * status are not applied
 */
class FARSA_WSIM_API MarXbotAttachmentDeviceMotorController : public MotorController
{
public:
	/**
	 * \brief The possible status of the attachment device. See class
	 *        description for more information
	 */
	enum Status {
		Open,
		HalfClosed,
		Closed
	};

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the motorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               motorcontroller has been deleted)
	 * \param robot the robot whose attachment device we control (must not
	 *              be NULL)
	 * \param name the name of the object
	 */
	MarXbotAttachmentDeviceMotorController(World* world, SharedDataWrapper<Shared> shared, PhyMarXbot* robot, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	virtual ~MarXbotAttachmentDeviceMotorController();

public:
	/**
	 * \brief The method updating the attachment device at each timestep
	 */
	virtual void update();

	/**
	 * \brief Returns true if the attachment device is enabled
	 *
	 * To enable/disable the attachment device you must use the PhyMarXbot
	 * methods
	 * \return true if the attachment device is enabled, false otherwise
	 */
	bool attachmentDeviceEnabled() const;

	/**
	 * \brief Sets the maximum speed at which the attachment device can move
	 *
	 * \param speed the maximum speed at which the attachment device can
	 *              move (in rad/sec)
	 */
	void setMaxVelocity(double speed);

	/**
	 * \brief Returns the maximum speed at which the attachment device can
	 *        move
	 *
	 * \return the maximum speed at which the attachment device can move (in
	 *         rad/sec)
	 */
	double getMaxVelocity() const;

	/**
	 * \brief Sets the desired position of the attachment device
	 *
	 * \param pos the desired position of the attachment device (in
	 *            radiants)
	 */
	void setDesiredPosition(double pos);

	/**
	 * \brief Returns the desired position of the attachment device
	 *
	 * \return the desired position of the attachment device
	 */
	double getDesiredPosition() const;

	/**
	 * \brief Returns the current position of the attachment device
	 *
	 * \return the current position of the attachment device (in radiants)
	 */
	double getPosition() const;

	/**
	 * \brief Sets the desired velocity of the attachment device
	 *
	 * \param vel the desidered velocity of the attachment device (in
	 *            rad/sec)
	 */
	void setDesiredVelocity(double vel);

	/**
	 * \brief Returns the desired velocity of the attachment device
	 *
	 * \return the desired velocity of the attachment device
	 */
	double getDesiredVelocity() const;

	/**
	 * \brief Returns the current velocity of the attachment device
	 *
	 * \return the current velocity of the attachment device (in rad/sec)
	 */
	double getVelocity() const;

	/**
	 * \brief Changes the status of the attachment device
	 *
	 * If the attachment device is already at the desired status, nothing
	 * happends
	 * \param status the desired status of the attachment device
	 */
	void setDesiredStatus(Status status);

	/**
	 * \brief Returns the desired status of the attachment device
	 *
	 * \return the desired status of the attachment device
	 */
	Status getDesiredStatus() const
	{
		return m_desiredStatus;
	}

	/**
	 * \brief Returns the current status of the attachment device
	 *
	 * \return the current status of the attachment device
	 */
	Status getStatus() const
	{
		return m_status;
	}

	/**
	 * \brief Returns true if this robot is near enough to another robot to
	 *        attach to it
	 *
	 * This function only check whether this robot is near enough to another
	 * robot and the attachment device is in the right direction. It doesn't
	 * check if the status of the attachment device is the right one for
	 * attaching or not
	 */
	bool attachmentPossible() const
	{
		return (tryToAttach() != NULL);
	}

	/**
	 * \brief Returns true if we are attached to another robot
	 *
	 * \return true if we are attached to another robot
	 */
	bool attachedToRobot() const
	{
		return (m_attachedRobot != NULL);
	}

	/**
	 * \brief Returns the robot to which we are attached
	 *
	 * If we are not attached to any robot, this returns NULL
	 * \return the robot to which we are attached or NULL if we are not
	 *         attached to another robot
	 */
	PhyMarXbot* attachedRobot()
	{
		return m_attachedRobot;
	}

	/**
	 * \brief Returns the robot to which we are attached (const version)
	 *
	 * If we are not attached to any robot, this returns NULL
	 * \return the robot to which we are attached or NULL if we are not
	 *         attached to another robot
	 */
	const PhyMarXbot* attachedRobot() const
	{
		return m_attachedRobot;
	}

	/**
	 * \brief Returns true if there are robots that are attached to us
	 *
	 * This does not include the robot to which we are attached
	 * \return true if there are robots that are attached to us
	 */
	bool otherAttachedToUs() const
	{
		return !m_otherAttachedRobots.isEmpty();
	}

	/**
	 * \brief Returns the list of all robots that are attached to us
	 *
	 * This list doesn't contain the robot to which we are attached
	 * \return the list of all robots that are attached to us
	 */
	QVector<PhyMarXbot*> otherAttachedRobots()
	{
		return m_otherAttachedRobots;
	}

private:
	/**
	 * \brief Tries to attach to another robot
	 *
	 * This function performs all tests needed to see if there is another
	 * robot to which we can attach closing the gripper. If it finds one,
	 * it returns it, otherwise NULL is returned
	 * \return the robot to which we can attach or NULL if there is no robot
	 *         to which to attach
	 */
	PhyMarXbot* tryToAttach() const;

	/**
	 * \brief Called by the robot to tell us when the attachment device is
	 *        being destroyed
	 */
	void attachmentDeviceAboutToBeDestroyed();

	/**
	 * \brief The robot whose attachment device we control
	 */
	PhyMarXbot* const m_robot;

	/**
	 * \brief The current status of the attachment device
	 */
	Status m_status;

	/**
	 * \brief The desired status of the attachment device
	 */
	Status m_desiredStatus;

	/**
	 * \brief The joint between two robots that are attached using the
	 *        attachment device
	 *
	 * This is NULL if the robot is not attached to any other robot
	 */
	PhyJoint* m_joint;

	/**
	 * \brief The robot to which we are attached
	 *
	 * This is NULL if we are not attached to any other robot
	 */
	PhyMarXbot* m_attachedRobot;

	/**
	 * \brief The list of all robots that are attached to us
	 *
	 * This list doesn't contain the robot to which we are attached
	 */
	QVector<PhyMarXbot*> m_otherAttachedRobots;

	/**
	 * \brief The PhyMarXbot class is friend to call the
	 *        attachmentDeviceAboutToBeDestroyed() function
	 */
	friend class PhyMarXbot;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

} // end namespace farsa

#endif
