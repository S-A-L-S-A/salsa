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

#include "phykhepera.h"
#include "phybox.h"
#include "phycylinder.h"
#include "phyballandsocket.h"
#include "physphere.h"
#include "physuspension.h"
#include "mathutils.h"
#include "graphicalmarkers.h"
#include <cmath>

namespace salsa {

// Lengths are in meters, weights in kilograms.
const real PhyKhepera::bodydistancefromground = 0.0015f;
const real PhyKhepera::bodyr = 0.035f;
const real PhyKhepera::bodyh = 0.030f;
const real PhyKhepera::bodym = 0.066f;
const real PhyKhepera::wheelr = 0.015f;
const real PhyKhepera::wheelh = 0.003f;
const real PhyKhepera::wheelm = 0.005f;
const real PhyKhepera::axletrack = 0.055f;
const real PhyKhepera::passivewheelr = 0.003f;
const real PhyKhepera::passivewheelm = 0.002f;

PhyKhepera::PhyKhepera(World* world, SharedDataWrapper<Shared> shared, QString name, const wMatrix& tm)
	: QObject(nullptr)
	, WObject(world, shared, name, tm)
	, m_shared(shared)
	, m_body(nullptr)
	, m_bodyTransformation()
	, m_bodyInvTransformation()
	, m_wheels()
	, m_wheelsTransformation()
	, m_wheelJoints()
	, m_wheelsCtrl(nullptr)
	, m_proximityIR(nullptr)
	, m_frontMarker(nullptr)
{
	// --- reference frame
	//  X
	//  ^
	//  |       ------
	//  |       |    |
	//  |  -------------------
	//  |  |   battery       |
	//  |  |                 |
	//  |  -------------------
	//  |       |    |
	//  |       ------
	//  |
	//  +---------------------> Y
	// The front of the robot is towards -Y (positive speeds of the wheel cause the robot to move towards -Y)

	// Creating a material for the khepera
	world->materials().createMaterial("kheperaMaterial");
	world->materials().setProperties("kheperaMaterial", "default", 0.0f, 0.0f, 0.01f, 0.01f, true);
	world->materials().createMaterial("kheperaTire");
	world->materials().setProperties("kheperaTire", "default", 1.2f, 0.9f, 0.01f, 0.01f, true);

	// Now creating the body of the khepera. It is made up of a single cylinder. We put the
	// frame of reference of the body (which is equal to that of the whole robot) on the ground to have
	// more stability and to simplify moving the robot (no need to apply displacements to put it on a
	// plane)
	{
		// Creating the body
		m_body = world->createEntity(TypeToCreate<PhyCylinder>(), bodyr, bodyh, "body");
		m_bodyTransformation = wMatrix::yaw(toRad(90.0f));
		m_bodyTransformation.w_pos.z = bodydistancefromground + (bodyh / 2.0f);
		m_body->setMatrix(m_bodyTransformation * matrix());
		m_body->setMass(bodym);
		m_body->setMaterial("kheperaMaterial");
		m_body->setOwner(this, false);
		m_bodyInvTransformation = m_bodyTransformation.inverse();
	}

	// Creating all the wheels. We first create the two motorized wheels and then the two passive ones.
	// The position of the wheels and their indexes are as follows (remember that the front of the
	// robot is towards -Y):
	//
	//  X
	//  ^
	//  |       ------
	//  |       | 0  |
	//  |  -------------------
	//  |  |     battery     |
	//  |  | 3             2 |
	//  |  -------------------
	//  |       | 1  |
	//  |       ------
	//  |
	//  +---------------------> Y

	// Creating the first motorized wheel and its joint
	{
		// The matrix is relative to the robot frame of reference. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(axletrack / 2.0f, 0.0f, wheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhyCylinder>(), wheelr, wheelh, "motorWheel");
		wheel->setMass(wheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("kheperaTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhySuspension>(), m_body, wheel, wVector::Z(), m_body->matrix().untransformVector(wheel->matrix().w_pos), wVector::X());
		joint->dofs()[0]->disableLimits();
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the second motorized wheel and its joint
	{
		// The matrix is relative to the robot frame of reference. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(-axletrack / 2.0f, 0.0f, wheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhyCylinder>(), wheelr, wheelh, "motorWheel");
		wheel->setMass(wheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("kheperaTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhySuspension>(), m_body, wheel, wVector::Z(), m_body->matrix().untransformVector(wheel->matrix().w_pos), wVector::X());
		joint->dofs()[0]->disableLimits();
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the first passive wheel and its joint
	{
		// The matrix is relative to the robot frame of reference. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(0.0f, bodyr - passivewheelr, passivewheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhySphere>(), passivewheelr, "passiveWheel");
		wheel->setMass(passivewheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("kheperaTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhyBallAndSocket>(), m_body, wheel, m_body->matrix().untransformVector(wheel->matrix().w_pos));
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the second passive wheel and its joint
	{
		// The matrix is relative to the robot frame of reference. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(0.0f, -(bodyr - passivewheelr), passivewheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhySphere>(), passivewheelr, "passiveWheel");
		wheel->setMass(passivewheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("kheperaTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhyBallAndSocket>(), m_body, wheel, m_body->matrix().untransformVector(wheel->matrix().w_pos));
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Now we can create the motor controller
	QVector<PhyDOF*> motors;
	motors << m_wheelJoints[0]->dofs()[0] << m_wheelJoints[1]->dofs()[0];
	m_wheelsCtrl = world->createEntity(TypeToCreate<WheelMotorController>(), motors);
	m_wheelsCtrl->setOwner(this, false);
	// The min and max speed in rad/s have been derived for the e-puck wheel considering as maximum linear
	// displacement 14 cm/s (that for the give wheel radius is approximately 1.1 ratations per second)
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI DECIDERE LA VELOCITÀ MASSIMA DEL KHEPERA, PER IL MOMENTO È LA STESSA DELL E-PUCK
#endif
	const real maxAngularSpeed = 1.1f * 2.0f * PI_GRECO;
	m_wheelsCtrl->setSpeedLimits(-maxAngularSpeed, -maxAngularSpeed, maxAngularSpeed, maxAngularSpeed);

	// Connecting wheels speed signals to be able to move the robot when in kinematic
	connect(m_wheelJoints[0]->dofs()[0], SIGNAL(changedDesiredVelocity(real)), this, SLOT(setRightWheelDesideredVelocity(real)));
	connect(m_wheelJoints[1]->dofs()[0], SIGNAL(changedDesiredVelocity(real)), this, SLOT(setLeftWheelDesideredVelocity(real)));

	// Creating the proximity IR sensors
	QVector<SingleIR::InitParams> sensors;

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI DECIDERE PER I RANGE/APERTURE E LE POSIZIONI
#endif
	// Adding the sensors. The khepera has 8 proximity infrared sensors
	const double sensorsAngles[] = {-60.0, -36.0, -12.0, 12.0, 36.0, 60.0, 160.0, 200.0};
	for (unsigned int i = 0; i < 8; i++) {
		const double curAngle = sensorsAngles[i] - 90.0; //double(i) * (360.0 / 8.0) + ((360.0 / 8.0) / 2.0);
		const double radius = bodyr;

		wMatrix mtr = wMatrix::yaw(toRad(curAngle)) * wMatrix::roll(PI_GRECO / 2.0);
		mtr.w_pos.x = -bodydistancefromground /*- (bodyh / 2.0f) + 0.005*/;
		mtr.w_pos.y = radius * sin(toRad(curAngle));
		mtr.w_pos.z = radius * cos(toRad(curAngle));

		sensors.append(SingleIR::InitParams(m_body, mtr, 0.00, 0.05, 10.0, 5));
	}
	m_proximityIR = world->createEntity(TypeToCreate<SimulatedIRProximitySensorController>(), sensors);
	m_proximityIR->setOwner(this, false);
}

PhyKhepera::~PhyKhepera()
{
	foreach (PhyJoint* j, m_wheelJoints) {
		world()->deleteEntity(j);
	}
	foreach (PhyObject* w, m_wheels) {
		world()->deleteEntity(w);
	}
	world()->deleteEntity(m_wheelsCtrl);
	world()->deleteEntity(m_body);
	world()->deleteEntity(m_proximityIR);
}

void PhyKhepera::preUpdate()
{
	// Updating motors
	if (m_wheelsCtrl->isEnabled()) {
		m_wheelsCtrl->update();
	}

	if (m_shared->kinematicSimulation) {
		// In kinematic mode, we have to manually move the robot depending on the wheel velocities
		wMatrix mtr = matrix();
		wheeledRobotsComputeKinematicMovement(mtr, m_shared->leftWheelVelocity, m_shared->rightWheelVelocity, wheelr, axletrack, world()->timeStep());

		// This will also update the position of all pieces
		setMatrix(mtr);
	}
}

void PhyKhepera::postUpdate()
{
	// Updating sensors
	if (m_proximityIR->isEnabled()) {
		m_proximityIR->update();
	}

	// Updating the transformation matrix of the robot. It is coincident with the matrix of the body
	m_shared.getModifiableShared()->tm = m_bodyInvTransformation * m_body->matrix();
}

void PhyKhepera::setProximityIRSensorsGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_proximityIR->setGraphicalProperties(drawSensor, drawRay, drawRealRay);
}

void PhyKhepera::setDrawFrontMarker(bool drawMarker)
{
	if (drawingFrontMarker() == drawMarker) {
		return;
	}

	if (drawMarker) {
		m_frontMarker = world()->createEntity(TypeToCreate<PlanarArrowGraphicalMarker>(), bodyr, bodyr / 6.0f, bodyr / 4.0f, 0.7f);
		m_frontMarker->setUseColorTextureOfOwner(false);
		m_frontMarker->setColor(Qt::green);
		m_frontMarker->setTexture("");

		wMatrix displacement = wMatrix::roll(-PI_GRECO / 2.0f);
		displacement.w_pos = wVector(0.0, 0.0, bodyh + bodydistancefromground + 0.0001f);
		m_frontMarker->setDisplacement(displacement);
		m_frontMarker->setOwner(this, true);
	} else {
		world()->deleteEntity(m_frontMarker);
		m_frontMarker = nullptr;
	}
}

bool PhyKhepera::drawingFrontMarker() const
{
	return (m_frontMarker != nullptr);
}

void PhyKhepera::doKinematicSimulation(bool k)
{
	if (m_shared->kinematicSimulation == k) {
		return;
	}

	m_shared.getModifiableShared()->kinematicSimulation = k;
	if (m_shared->kinematicSimulation) {
		// First disabling all joints...
		for (int i = 0; i < m_wheelJoints.size(); i++) {
			m_wheelJoints[i]->enable(false);
		}

		// ... then setting all objects to kinematic behaviour
		m_body->setKinematic(true, true);
		for (int i = 0; i < m_wheels.size(); i++) {
			m_wheels[i]->setKinematic(true, true);
		}
	} else {
		// First setting all objects to dynamic behaviour...
		m_body->setKinematic(false);
		for (int i = 0; i < m_wheels.size(); i++) {
			m_wheels[i]->setKinematic(false);
		}

		// ... then enabling all joints (if the corresponding part is enabled)
		for (int i = 0; i < m_wheelJoints.size(); i++) {
			m_wheelJoints[i]->enable(true);
			m_wheelJoints[i]->updateJointInfo();
		}
	}
}

void PhyKhepera::setLeftWheelDesideredVelocity(real velocity)
{
	m_shared.getModifiableShared()->leftWheelVelocity = velocity;
}

void PhyKhepera::setRightWheelDesideredVelocity(real velocity)
{
	m_shared.getModifiableShared()->rightWheelVelocity = velocity;
}

void PhyKhepera::changedMatrix()
{
	wMatrix tm = matrix();
	m_body->setMatrix(m_bodyTransformation * tm);
	for (int i = 0; i < m_wheels.size(); i++) {
		m_wheels[i]->setMatrix(m_wheelsTransformation[i] * tm);
	}
	foreach (PhyJoint* j, m_wheelJoints) {
		j->updateJointInfo();
	}
}

} // end namespace salsa
