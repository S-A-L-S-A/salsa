/********************************************************************************
 *  SALSA                                                                       *
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

#include "worldsimconfig.h"
#include "motorcontrollers.h"
#include "phyjoint.h"
#include "world.h"
#include "phymarxbot.h"
#include "phyfixed.h"
#include "mathutils.h"
#include <cmath>
#include <QSet>

namespace salsa {

wPID::wPID() {
	p_gain = 0;
	i_gain = 0;
	d_gain = 0;
	acc_ff = 0;
	fri_ff = 0;
	vel_ff = 0;
	bias = 0;
	accel_r = 0;
	setpt = 0;
	min = 0;
	max = 0;
	slew = 0;
	this_target = 0;
	next_target = 0;
	integral = 0;
	last_error = 0;
	last_output = 0;
}

double wPID::pidloop( double PV ) {
	double     this_error;
	double     this_output;
	double     accel;
	double     deriv;
	double     friction;
	/* the desired PV for this iteration is the value     */
	/* calculated as next_target during the last loop     */
	this_target = next_target;
	/* test for acceleration, compute new target PV for   */
	/* the next pass through the loop                     */
	if ( accel_r > 0 && this_target != setpt) {
		if ( this_target < setpt ) {
			next_target += accel_r;
			if ( next_target > setpt ) {
				next_target = setpt;
			}
		} else { /* this_target > setpoint */
			next_target -= accel_r;
			if ( next_target < setpt ) {
				next_target = setpt;
			}
		}
	} else {
		next_target = setpt;
	}
	/* acceleration is the difference between the PV      */
	/* target on this pass and the next pass through the  */
	/* loop                                               */
	accel = next_target - this_target;
	/* the error for the current pass is the difference   */
	/* between the current target and the current PV      */
	this_error = this_target - PV;
	/* the derivative is the difference between the error */
	/* for the current pass and the previous pass         */
	deriv = this_error - last_error;
	/* a very simple determination of whether there is    */
	/* special friction to be overcome on the next pass,  */
	/* if the current PV is 0 and the target for the next */
	/* pass is not 0, stiction could be a problem         */
	friction = ( PV == 0.0 && next_target != 0.0 );
	/* the new error is added to the integral             */
	integral += this_target - PV;
	/* now that all of the variable terms have been       */
	/* computed they can be multiplied by the appropriate */
	/* coefficients and the resulting products summed     */
	this_output = p_gain * this_error
				+ i_gain * integral
				+ d_gain * deriv
				+ acc_ff * accel
				+ vel_ff * next_target
				+ fri_ff * friction
				+ bias;
	last_error   = this_error;
	/* check for slew rate limiting on the output change  */
	if ( 0 != slew ) {
		if ( this_output - last_output > slew ) {
			this_output = last_output + slew;
		} else if ( last_output - this_output > slew ) {
			this_output = last_output - slew;
		}
	}
	/* now check the output value for absolute limits     */
	if ( this_output < min ) {
		this_output = min;
	} else if ( this_output > max ) {
		this_output = max;
	}
	/* store the new output value to be used as the old   */
	/* output value on the next loop pass                 */
	return last_output = this_output;
}

void wPID::reset() {
	// Resetting status variables
	this_target = 0;
	next_target = 0;
	integral = 0;
	last_error = 0;
	last_output = 0;
}

MotorController::MotorController(World* world, SharedDataWrapper<Shared> shared, QString name)
	: WEntity(world, shared, name)
	, m_enabled(true)
{
}

MotorController::~MotorController()
{
}

void wheeledRobotsComputeKinematicMovement(wMatrix &mtr, real leftWheelVelocity, real rightWheelVelocity, real wheelr, real axletrack, real timestep)
{
	const wMatrix origMtr = mtr;

	// To compute the actual movement of the robot I assume the movement is on the plane on which
	// the two wheels lie. This means that I assume that both wheels are in contact with a plane
	// at every time. Then I compute the instant centre of rotation and move the robot rotating
	// it around the axis passing through the instant center of rotation and perpendicular to the
	// plane on which the wheels lie (i.e. around the local XY plane)

	// First of all computing the linear speed of the two wheels
	const real leftSpeed = leftWheelVelocity * wheelr;
	const real rightSpeed = rightWheelVelocity * wheelr;

	// If the speed of the two wheels is very similar, simply moving the robot forward (doing the
	// computations would probably lead to invalid matrices)
	if (fabs(leftSpeed - rightSpeed) < 0.00001f) {
		// The front of the robot is towards -y
		mtr.w_pos += -mtr.y_ax.scale(rightSpeed * timestep);
	} else {
		// The first thing to do is to compute the instant centre of rotation. We do the
		// calculation in the robot local frame of reference. We proceed as follows (with
		// uppercase letters we indicate vectors and points):
		//
		// -------------------+------>
		//                    |    X axis
		//      ^             |
		//      |\            |
		//      | \A          |
		//      |  \          |
		//     L|   ^         |
		//      |   |\        |
		//      |   | \       |
		//      |  R|  \      |
		//      |   |   \     |
		//     O+--->----+C   |
		//        D           |
		//                    |
		//                    |
		//                    v Y axis (the robot moves forward towards -Y)
		//
		// In the picture L and R are the velocity vectors of the two wheels, D is the vector
		// going from the center of the left wheel to the center of the right wheel, A is the
		// vector going from the tip of L to the tip of R and C is the instant centre of
		// rotation. D is fixed an parallel to the X axis, while L and R are always parallel
		// to the Y axis. Also, for the moment, we take the frame of reference origin at O.
		// The construction shown in the picture allows to find the instant center of rotation
		// for any L and R except when they are equal. In this case C is "at infinite" and the
		// robot moves forward of backward without rotation. All the vectors lie on the local
		// XY plane. To find C we proceed in the following way. First of all we note that
		// A = R - L. If a and b are two real numbers, then we can say that C is at the
		// instersection of L + aA with bD, that is we need a and b such that:
		// 	L + aA = bD.
		// If we take the cross product on both sides we get:
		// 	(L + aA) x D = bD x D => (L + aA) x D = 0
		// This means that we need to find a such that L + aA and D are parallel. As D is parallel
		// to the X axis, its Y component is 0, so we can impose that L + aA has a 0 Y component,
		// too, that is:
		// 	(Ly) + a(Ay) = 0 => a = -(Ly) / (Ay)
		// Once we have a, we can easily compute C:
		// 	C = L + aA
		// In the following code we use the same names we have used in the description above. Also
		// note that the actual origin of the frame of reference is not in O: it is in the middle
		// between the left and right wheel (we need to take this into account when computing the
		// point around which the robot rotates)
		const wVector L(0.0, -leftSpeed, 0.0);
		const wVector A(axletrack, -(rightSpeed - leftSpeed), 0.0);
		const real a = -(L.y / A.y);
		const wVector C = L + A.scale(a);

		// Now we have to compute the angular velocity of the robot. This is simply equal to v/r where
		// v is the linear velocity at a point that is distant r from C. We use the velocity of one of
		// the wheel: which weels depends on its distance from C. We take the wheel which is furthest from
		// C to avoid having r = 0 (which would lead to an invalid value of the angular velocity). Distances
		// are signed (they are taken along the X axis)
		const real distLeftWheel = -C.x;
		const real distRightWheel = -(C.x - A.x); // A.x is equal to D.x
		const real angularVelocity = (fabs(distLeftWheel) > fabs(distRightWheel)) ? (-leftSpeed / distLeftWheel) : (-rightSpeed / distRightWheel);

		// The angular displacement is simply the angular velocity multiplied by the timeStep. Here we
		// also compute the center of rotation in world coordinates (we also need to compute it in the frame
		// of reference centered between the wheels, not in O) and the axis of rotation (that is simply the
		// z axis of the robot in world coordinates)
		const real angularDisplacement = angularVelocity * timestep;
		const wVector centerOfRotation = origMtr.transformVector(C - wVector(axletrack / 2.0, 0.0, 0.0));
		const wVector axisOfRotation = origMtr.z_ax;

		// Rotating the robot transformation matrix to obtain the new position
		mtr = mtr.rotateAround(axisOfRotation, centerOfRotation, angularDisplacement);
	}
}

bool wheeledRobotsComputeWheelsSpeed(const wMatrix& start, const wMatrix& end, real wheelr, real axletrack, real timestep, real& leftWheelVelocity, real& rightWheelVelocity)
{
	bool ret = true;

	// What we need do to compute the speeds of the wheels is to compute the position of
	// the instant rotation center. The hypothesis is that the wheels move at constant
	// speed when bringing the robot from start to end. In this case the instant rotation
	// center is constant and we can compute the distance travelled by each wheel (along an
	// arc of a circumference). Once we have the distance it is easy to compute the speed.
	// Under the hypothesis of constant wheel speed, the instant rotation center can be
	// computed as the intersection of the local X axis at start and end (the wheels axis
	// is along X). Moreover the distance from the intersection to the center of the robot
	// must be the same at start and end. To compute the intersection we need to solve the
	// following equation: Ps + k1*Xs = Pe + k2*Xe where Ps and Pe are the starting and ending
	// positions of the robot, Xs and Xe are the local X axes at start and end and k1 and k2
	// are two unknowns. If k1 == k2, the hypothesis of constant wheel speed holds, otherwise
	// it doesn't. However, we must consider the side case of Xs and Xe parallel (see below)

	// Useful constants
	const real epsilon = 0.0001f;
	const wVector& Xs = start.x_ax; SALSA_DEBUG_TEST_INVALID(Xs.x) SALSA_DEBUG_TEST_INVALID(Xs.y) SALSA_DEBUG_TEST_INVALID(Xs.z)
	const wVector& Xe = end.x_ax; SALSA_DEBUG_TEST_INVALID(Xe.x) SALSA_DEBUG_TEST_INVALID(Xe.y) SALSA_DEBUG_TEST_INVALID(Xe.z)
	const real halfAxletrack = axletrack / 2.0f;
	const wVector displacementVector = end.w_pos - start.w_pos; SALSA_DEBUG_TEST_INVALID(displacementVector.x) SALSA_DEBUG_TEST_INVALID(displacementVector.y) SALSA_DEBUG_TEST_INVALID(displacementVector.z)
	const real displacement = displacementVector.norm(); SALSA_DEBUG_TEST_INVALID(displacement)
	// If displacement is 0, direction will be invalid (the check for displacement != 0 is done below)
	const wVector direction = displacementVector.scale(1.0f / displacement);

	// The space travelled by the left and right wheel (signed)
	real leftWheelDistance = 0.0f;
	real rightWheelDistance = 0.0f;

	// First of all checking if Xs and Xe are parallel. If they also have the same direction
	// (Pe - Ps) should be along -Y (otherwise the hypothesis of constant wheel speed doesn't
	// hold); if they have opposite directions (Pe - Ps) should be parallel to Xs and Xe,
	// meaning that the robot has rotated of 180° (otherwise the hypothesis of constant wheel
	// speed doesn't hold). In the latter case, however which wheel has positive velocity is not
	// defined, here we always return the left wheel as having positive velocity
	const salsa::real dotX = min(1.0f, max(-1.0f, Xs % Xe)); SALSA_DEBUG_TEST_INVALID(dotX)
	if (fabs(1.0f - dotX) < epsilon) {
		// If the displacement is 0, the robot hasn't moved at all
		if (fabs(displacement) < epsilon) {
			leftWheelDistance = 0.0f;
			rightWheelDistance = 0.0f;
		} else {
			// Xs and Xe are parallel and have the same direction. Checking if Pe-Ps is along
			// -Y or not (start.y_ax and end.y_ax are parallel because we just checked that
			// Xs and Xe are parallel and the Z axes are parallel by hypothesis)
			const salsa::real dotDirY = direction % (-start.y_ax); SALSA_DEBUG_TEST_INVALID(dotDirY)

			if (fabs(1.0f - dotDirY) < epsilon) {
				// Positive speed
				leftWheelDistance = displacement;
				rightWheelDistance = displacement;
			} else if (fabs(1.0f + dotDirY) < epsilon) {
				// Negative speed
				leftWheelDistance = -displacement;
				rightWheelDistance = -displacement;
			} else {
				// Impossible movement, multiplying by dotDirY (i.e. the cos of the angle)
				// to get the displacement along -Y
				leftWheelDistance = dotDirY * displacement; SALSA_DEBUG_TEST_INVALID(leftWheelDistance)
				rightWheelDistance = dotDirY * displacement; SALSA_DEBUG_TEST_INVALID(rightWheelDistance)
				ret = false;
			}
		}
	} else if (fabs(1.0f + dotX) < epsilon) {
		// If the displacement is 0, the robot has rotated on the spot by 180°
		if (fabs(displacement) < epsilon) {
			leftWheelDistance = PI_GRECO * axletrack;
			rightWheelDistance = -leftWheelDistance;
		} else {
			// Xs and Xe are parallel and have opposite directions. Checking if Pe-Ps is
			// along X or not.
			const salsa::real dotDirX = direction % Xs; SALSA_DEBUG_TEST_INVALID(dotDirX)

			const real distCenterOfRotationToCenterOfRobot = displacement / 2.0f; SALSA_DEBUG_TEST_INVALID(distCenterOfRotationToCenterOfRobot)
			const real slowWheel = (distCenterOfRotationToCenterOfRobot - halfAxletrack) * PI_GRECO; SALSA_DEBUG_TEST_INVALID(slowWheel)
			const real fastWheel = (distCenterOfRotationToCenterOfRobot + halfAxletrack) * PI_GRECO; SALSA_DEBUG_TEST_INVALID(fastWheel)

			if (fabs(1.0f - dotDirX) < epsilon) {
				leftWheelDistance = slowWheel;
				rightWheelDistance = fastWheel;
			} else if (fabs(1.0f + dotDirX) < epsilon) {
				leftWheelDistance = fastWheel;
				rightWheelDistance = slowWheel;
			} else {
				// Impossible movement, multiplying by dotDirX (i.e. the cos of the angle)
				// to get the displacement along X
				if (dotDirX > 0.0f) {
					leftWheelDistance = dotDirX * slowWheel; SALSA_DEBUG_TEST_INVALID(leftWheelDistance)
					rightWheelDistance = dotDirX * fastWheel; SALSA_DEBUG_TEST_INVALID(rightWheelDistance)
				} else {
					leftWheelDistance = dotDirX * fastWheel; SALSA_DEBUG_TEST_INVALID(leftWheelDistance)
					rightWheelDistance = dotDirX * slowWheel; SALSA_DEBUG_TEST_INVALID(rightWheelDistance)
				}
				ret = false;
			}
		}
	} else {
		// Xs and Xe are not parallel, computing k1 and k2
		const real delta = (Xs.y * Xe.x) - (Xs.x * Xe.y);
		const real invDelta = 1.0 / delta;
		const real k1 = invDelta * (Xe.x * displacementVector.y - Xe.y * displacementVector.x);
		const real k2 = invDelta * (Xs.x * displacementVector.y - Xs.y * displacementVector.x);

		// If k1 == k2, we have k == k1 == k2 and we can compute the correct velocities. If k1 != k2
		// we use the average of k1 and k2 to compute velocities but return false. If k is positive the
		// robot moved forward, if it is negative, it moved backward
		const real k = (k1 + k2) / 2.0f;
		if ((delta < epsilon) && (delta > -epsilon)) {
			// We get here if the two vectors are parallel. Setting ret to false and wheels distance
			// to displacement
			ret = false;
			leftWheelDistance = displacement;
			rightWheelDistance = displacement;
		} else {
			if (fabs(k1 - k2) >= epsilon) {
				ret = false;
			}

			// First of all we need the angle between the two X axes (i.e. how much the robot has rotated).
			// We need to know the sign of the rotation
			const real rotationSign = (((Xs * Xe) % start.z_ax) < 0.0f) ? -1.0f : 1.0f; SALSA_DEBUG_TEST_INVALID(rotationSign)
			const real angle = acos(dotX) * rotationSign; SALSA_DEBUG_TEST_INVALID(angle)

			// Now computing the distance of each wheel from the instant center of rotation. The distance
			// is signed (positive along local X)
			const real distLeftWheel = k + halfAxletrack; SALSA_DEBUG_TEST_INVALID(distLeftWheel)
			const real distRightWheel = k - halfAxletrack; SALSA_DEBUG_TEST_INVALID(distRightWheel)

			// Now computing the arc travelled by each wheel (signed, positive for forward motion, negative
			// for backward motion)
			leftWheelDistance = distLeftWheel * angle; SALSA_DEBUG_TEST_INVALID(leftWheelDistance)
			rightWheelDistance = distRightWheel * angle; SALSA_DEBUG_TEST_INVALID(rightWheelDistance)
		}
	}

	// Computing the wheel velocities
	leftWheelVelocity = (leftWheelDistance / wheelr) / timestep; SALSA_DEBUG_TEST_INVALID(leftWheelVelocity)
	rightWheelVelocity = (rightWheelDistance / wheelr) / timestep; SALSA_DEBUG_TEST_INVALID(rightWheelVelocity)

	return ret;
}

WheelMotorController::WheelMotorController(World* world, SharedDataWrapper<Shared> shared, QVector<PhyDOF*> wheels, QString name)
	: MotorController(world, shared, name)
	, m_motors(wheels)
	, m_desiredVel(m_motors.size(), 0.0)
	, m_minVel(m_motors.size(), -1.0)
	, m_maxVel(m_motors.size(), 1.0)
{
}

WheelMotorController::~WheelMotorController()
{
	/* nothing to do */
}

void WheelMotorController::update()
{
	for(int i = 0; i < m_motors.size(); i++) {
		m_motors[i]->setDesiredVelocity(m_desiredVel[i]);
	}
}

void WheelMotorController::setSpeeds(const QVector<double>& speeds)
{
	for( int i=0; i<qMin(speeds.size(), m_desiredVel.size()); i++ ) {
		if ( speeds[i] < m_minVel[i] ) {
			m_desiredVel[i] = m_minVel[i];
		} else if ( speeds[i] > m_maxVel[i] ) {
			m_desiredVel[i] = m_maxVel[i];
		} else {
			m_desiredVel[i] = speeds[i];
		}
	}
}

void WheelMotorController::setSpeeds( double sp1, double sp2 )
{
	if ( sp1 < m_minVel[0] ) {
		m_desiredVel[0] = m_minVel[0];
	} else if ( sp1 > m_maxVel[0] ) {
		m_desiredVel[0] = m_maxVel[0];
	} else {
		m_desiredVel[0] = sp1;
	}

	if ( sp2 < m_minVel[1] ) {
		m_desiredVel[1] = m_minVel[1];
	} else if ( sp2 > m_maxVel[1] ) {
		m_desiredVel[1] = m_maxVel[1];
	} else {
		m_desiredVel[1] = sp2;
	}
}

void WheelMotorController::getSpeeds(QVector<double>& speeds) const
{
	speeds.clear();
	for( int i=0; i<m_motors.size(); i++ ) {
		speeds.append(m_motors[i]->velocity());
	}
}

void WheelMotorController::getSpeeds( double& sp1, double& sp2 ) const
{
	sp1 = m_motors[0]->velocity();
	sp2 = m_motors[1]->velocity();
}

void WheelMotorController::getDesiredSpeeds( QVector<double>& speeds ) const
{
	speeds = m_desiredVel;
}

void WheelMotorController::getDesiredSpeeds( double& sp1, double& sp2 ) const
{
	sp1 = m_desiredVel[0];
	sp2 = m_desiredVel[1];
}

void WheelMotorController::setSpeedLimits(const QVector<double>& minSpeeds, const QVector<double>& maxSpeeds)
{
	m_minVel = minSpeeds;
	m_maxVel = maxSpeeds;
}

void WheelMotorController::setSpeedLimits( double minSpeed1, double minSpeed2, double maxSpeed1, double maxSpeed2 ) {
	m_minVel[0] = minSpeed1;
	m_minVel[1] = minSpeed2;
	m_maxVel[0] = maxSpeed1;
	m_maxVel[1] = maxSpeed2;
}

void WheelMotorController::getSpeedLimits( QVector<double>& minSpeeds, QVector<double>& maxSpeeds ) const
{
	minSpeeds = m_minVel;
	maxSpeeds = m_maxVel;
}

void WheelMotorController::getSpeedLimits( double& minSpeed1, double& minSpeed2, double& maxSpeed1, double& maxSpeed2 ) const
{
	minSpeed1 = m_minVel[0];
	minSpeed2 = m_minVel[1];
	maxSpeed1 = m_maxVel[0];
	maxSpeed2 = m_maxVel[1];
}

void WheelMotorController::setMaxTorque( double maxTorque )
{
	for (int i = 0; i < m_motors.size(); ++i) {
		m_motors[i]->setMaxForce( maxTorque );
	}
}

double WheelMotorController::getMaxTorque() const
{
	return m_motors[0]->maxForce();
}

MarXbotAttachmentDeviceMotorController::MarXbotAttachmentDeviceMotorController(World* world, SharedDataWrapper<Shared> shared, PhyMarXbot* robot, QString name) :
	MotorController(world, shared, name),
	m_robot(robot),
	m_status(Open),
	m_desiredStatus(Open),
	m_joint(nullptr),
	m_attachedRobot(nullptr),
	m_otherAttachedRobots()
{
}

MarXbotAttachmentDeviceMotorController::~MarXbotAttachmentDeviceMotorController()
{
	// Destroying the joint
	world()->deleteEntity(m_joint);
}

void MarXbotAttachmentDeviceMotorController::update()
{
	// If the desired status is equal to the current status, doing nothing
	if (m_desiredStatus == m_status) {
		return;
	}

	// Updating the status
	const Status previousStatus = m_status;
	m_status = m_desiredStatus;
	switch (m_status) {
		case Open:
			{
				// Here we simply have to detach
				world()->deleteEntity(m_joint);
				m_joint = nullptr;

				// Telling the other robot we are no longer attached
				if (m_attachedRobot != nullptr) {
					int i = m_attachedRobot->attachmentDeviceController()->m_otherAttachedRobots.indexOf(m_robot);
					if (i != -1) {
						m_attachedRobot->attachmentDeviceController()->m_otherAttachedRobots.remove(i);
					}

					m_attachedRobot = nullptr;
				}
			}
			break;
		case HalfClosed:
			{
				if (previousStatus == Open) {
					// Trying to attach
					m_attachedRobot = tryToAttach();
				} else {
					// If I was attached, the status was Closed for sure, so we have to change the kind
					// of joint
					if (m_attachedRobot != nullptr) {
						world()->deleteEntity(m_joint);
					}
				}

				if (m_attachedRobot != nullptr) {
					// Found a robot to which we can attach, creating the joint (a hinge). The parent
					// here is the other robot because this way it is easier to specify the axis and
					// center
					m_joint = world()->createEntity(TypeToCreate<PhyHinge>(), m_attachedRobot->turret(), m_robot->attachmentDevice(), wVector(1.0, 0.0, 0.0), wVector(0.0, 0.0, 0.0), 0.0);
					m_joint->dofs()[0]->disableLimits();
					m_joint->dofs()[0]->switchOff(); // This way the joint rotates freely

					// Telling the other robot we are now attached (if we weren't already)
					if (previousStatus == Open) {
						m_attachedRobot->attachmentDeviceController()->m_otherAttachedRobots.append(m_robot);
					}
				}
			}
			break;
		case Closed:
			{
				if (previousStatus == Open) {
					// Trying to attach
					m_attachedRobot = tryToAttach();
				} else {
					// If I was attached, the status was HalfClosed for sure, so we have to change the kind
					// of joint
					if (m_attachedRobot != nullptr) {
						world()->deleteEntity(m_joint);
					}
				}

				if (m_attachedRobot != nullptr) {
					// Found a robot to which we can attach, creating the joint (a fixed joint)
					m_joint = world()->createEntity(TypeToCreate<PhyFixed>(), m_robot->attachmentDevice(), m_attachedRobot->turret());

					// Telling the other robot we are now attached (if we weren't already)
					if (previousStatus == Open) {
						m_attachedRobot->attachmentDeviceController()->m_otherAttachedRobots.append(m_robot);
					}
				}
			}
			break;
	}
}

bool MarXbotAttachmentDeviceMotorController::attachmentDeviceEnabled() const
{
	return m_robot->attachmentDeviceEnabled();
}

void MarXbotAttachmentDeviceMotorController::setMaxVelocity(double speed)
{
	if (attachmentDeviceEnabled()) {
		m_robot->attachmentDeviceJoint()->dofs()[0]->setMaxVelocity(speed);
	}
}

double MarXbotAttachmentDeviceMotorController::getMaxVelocity() const
{
	if (attachmentDeviceEnabled()) {
		return m_robot->attachmentDeviceJoint()->dofs()[0]->maxVelocity();
	} else {
		return 0.0;
	}
}

void MarXbotAttachmentDeviceMotorController::setDesiredPosition(double pos)
{
	if (attachmentDeviceEnabled() && (!attachedToRobot())) {
		m_robot->attachmentDeviceJoint()->dofs()[0]->setDesiredPosition(pos);
	}
}

double MarXbotAttachmentDeviceMotorController::getDesiredPosition() const
{
	if (attachmentDeviceEnabled()) {
		return m_robot->attachmentDeviceJoint()->dofs()[0]->desiredPosition();
	} else {
		return 0.0;
	}
}

double MarXbotAttachmentDeviceMotorController::getPosition() const
{
	if (attachmentDeviceEnabled()) {
		return m_robot->attachmentDeviceJoint()->dofs()[0]->position();
	} else {
		return 0.0;
	}
}

void MarXbotAttachmentDeviceMotorController::setDesiredVelocity(double vel)
{
	if (attachmentDeviceEnabled() && (!attachedToRobot())) {
		m_robot->attachmentDeviceJoint()->dofs()[0]->setDesiredVelocity(vel);
	}
}

double MarXbotAttachmentDeviceMotorController::getDesiredVelocity() const
{
	if (attachmentDeviceEnabled()) {
		return m_robot->attachmentDeviceJoint()->dofs()[0]->desiredVelocity();
	} else {
		return 0.0;
	}
}

double MarXbotAttachmentDeviceMotorController::getVelocity() const
{
	if (attachmentDeviceEnabled()) {
		return m_robot->attachmentDeviceJoint()->dofs()[0]->velocity();
	} else {
		return 0.0;
	}
}

void MarXbotAttachmentDeviceMotorController::setDesiredStatus(Status status)
{
	if (attachmentDeviceEnabled()) {
		m_desiredStatus = status;
	}
}

PhyMarXbot* MarXbotAttachmentDeviceMotorController::tryToAttach() const
{
	// Checking the collisions of the attachment device
	const ContactVec contacts = world()->contacts()[m_robot->attachmentDevice()];

	// Now for each contact checking whether it is a turret of another PhyMarXbot and not
	// the attachment device. We create two sets: one contains robots whose attachment
	// device collides with our attachment device (we surely won't attach to these robots),
	// the other the robots whose turret collides with our attachment device (we could attach
	// to these robots)
	QSet<PhyMarXbot*> discardedRobots;
	QSet<PhyMarXbot*> candidateRobots;
	foreach (const Contact& c, contacts) {
		PhyMarXbot* otherRobot = dynamic_cast<PhyMarXbot*>(c.collide->owner());
		if ((otherRobot != nullptr) && (otherRobot->attachmentDeviceEnabled()) && (!discardedRobots.contains(otherRobot))) {
			// Checking that the contact is the turret and not the attachment device
			if (c.collide == otherRobot->turret()) {
				// We have a candidate robot for attachment!
				candidateRobots.insert(otherRobot);
			} else if (c.collide == otherRobot->attachmentDevice()) {
				// We have to discard this robot
				discardedRobots.insert(otherRobot);
				candidateRobots.remove(otherRobot);
			}
		}
	}

	// Now we have a set of candidates. In practice we should always have at most one candidate due to
	// physical constraints. In case we have more than one, we simply return the first. If we have none,
	// we return nullptr
	if (candidateRobots.isEmpty()) {
		return nullptr;
	} else {
		return *(candidateRobots.begin());
	}
}

void MarXbotAttachmentDeviceMotorController::attachmentDeviceAboutToBeDestroyed()
{
	// Deleting the joint
	world()->deleteEntity(m_joint);
	m_status = Open;
	m_desiredStatus = Open;
	m_joint = nullptr;

	// Telling the other robot we are no longer attached
	if (m_attachedRobot != nullptr) {
		int i = m_attachedRobot->attachmentDeviceController()->m_otherAttachedRobots.indexOf(m_robot);
		if (i != -1) {
			m_attachedRobot->attachmentDeviceController()->m_otherAttachedRobots.remove(i);
		}
		m_attachedRobot = nullptr;
	}

	// Also detachting robots attached to us
	foreach (PhyMarXbot* other, m_otherAttachedRobots) {
		// Destroying their joint and setting the robot to which they are attached to nullptr
		world()->deleteEntity(other->attachmentDeviceController()->m_joint);
		other->attachmentDeviceController()->m_joint = nullptr;
		other->attachmentDeviceController()->m_attachedRobot = nullptr;
	}
}

} // end namespace salsa
