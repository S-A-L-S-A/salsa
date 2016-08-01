/********************************************************************************
 *  WorldSim -- library for robot simulations                                   *
 *  Copyright (C) 2012-2013                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Fabrizio Papi <erkito87@gmail.com>                                          *
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

#include "phyepuck.h"
#include "phybox.h"
#include "phycylinder.h"
#include "phyfixed.h"
#include "phyballandsocket.h"
#include "physphere.h"
#include "physuspension.h"
#include "phycompoundobject.h"
#include "phycone.h"
#include "mathutils.h"
#include "graphicalmarkers.h"
#include <cmath>

namespace salsa {

// All measures have been taken on the LARAL e-puck or the official specifications. Lengths are in
// meters, weights in kilograms.
const real PhyEpuck::batteryplacex = 0.043f;
const real PhyEpuck::batteryplacey = 0.055f;
const real PhyEpuck::batteryplacez = 0.033f;
const real PhyEpuck::batterym = 0.042f;
const real PhyEpuck::batteryplacedistancefromground = 0.0015f;
const real PhyEpuck::bodyr = 0.035f;
const real PhyEpuck::bodyh = 0.021f;
const real PhyEpuck::wholebodym = 0.078f;
const real PhyEpuck::wheelr = 0.0205f;
const real PhyEpuck::wheelh = 0.003f;
const real PhyEpuck::wheelm = 0.010f;
const real PhyEpuck::axletrack = 0.053f;
const real PhyEpuck::passivewheelr = 0.005f;
const real PhyEpuck::passivewheelm = 0.005f;

PhyEpuck::PhyEpuck(World* world, SharedDataWrapper<Shared> shared, QString name, const wMatrix& tm)
	: QObject(NULL)
	, WObject(world, shared, name, tm)
	, m_shared(shared)
	, m_body(NULL)
	, m_turret(NULL)
	, m_wheels()
	, m_wheelsTransformation()
	, m_wheelJoints()
	, m_wheelsCtrl(NULL)
	, m_proximityIR(NULL)
	, m_groundIR(NULL)
	, m_frontMarker(NULL)
	, m_uniformColor(QList<PhyCylinderSegmentColor>() << PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), color()))
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
	// The front of the robot is towards -Y (positive speeds of the wheel make the robot move towards -Y)
	// and the camera is in -Y

	// Creating a material for the e-puck
	world->materials().createMaterial("epuckMaterial");
	world->materials().setProperties("epuckMaterial", "default", 0.0f, 0.0f, 0.01f, 0.01f, true);
	world->materials().createMaterial("epuckTire");
	world->materials().setProperties("epuckTire", "default", 1.2f, 0.9f, 0.01f, 0.01f, true);

	// Now creating the body of the e-puck. It is made up of two pieces: a box modelling the battery
	// pack and a short cylinder modelling the upper part (where the electronic board is). We put the
	// frame of reference of the body (which is equal to that of the whole robot) on the ground to have
	// move stability and to simplify moving the robot (no need to apply displacements to put it on a
	// plane)
	{
		PhyCompoundComponentsList components(world);
		wMatrix mtr = wMatrix::identity();

		// First we create the battery pack
		mtr.w_pos.z = (batteryplacez / 2.0f) + batteryplacedistancefromground;
		components.createComponent(TypeToCreate<PhyBox>(), mtr, batteryplacex, batteryplacey, batteryplacez);

		// Then we create the upper part
		mtr = wMatrix::yaw(toRad(90.0f));
		mtr.w_pos.z = batteryplacez + batteryplacedistancefromground + (bodyh / 2.0f);
		m_turret = components.createComponent(TypeToCreate<PhyCylinder>(), mtr, bodyr, bodyh);

		// Finally we create the compound object actually modelling the body
		m_body = world->createEntity(TypeToCreate<PhyCompoundObject>(), components, "base");
		m_body->setMass(batterym + wholebodym);
		m_body->setMaterial("epuckMaterial");
		m_body->setOwner(this, false);
		m_body->setMatrix(tm);
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
		// The matrix is relative to the base. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(axletrack / 2.0f, 0.0f, wheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhyCylinder>(), wheelr, wheelh, "motorWheel");
		wheel->setMass(wheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("epuckTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * m_body->matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhySuspension>(), m_body, wheel, m_body->matrix().x_ax, wheel->matrix().w_pos, m_body->matrix().z_ax);
		joint->dofs()[0]->disableLimits();
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the second motorized wheel and its joint
	{
		// The matrix is relative to the base. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(-axletrack / 2.0f, 0.0f, wheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhyCylinder>(), wheelr, wheelh, "motorWheel");
		wheel->setMass(wheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("epuckTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * m_body->matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhySuspension>(), m_body, wheel, m_body->matrix().x_ax, wheel->matrix().w_pos, m_body->matrix().z_ax);
		joint->dofs()[0]->disableLimits();
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the first passive wheel and its joint
	{
		// The matrix is relative to the base. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(0.0f, (batteryplacey / 2.0f) - passivewheelr, passivewheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhySphere>(), passivewheelr, "passiveWheel");
		wheel->setMass(passivewheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("epuckTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * m_body->matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhyBallAndSocket>(), m_body, wheel, wheel->matrix().w_pos);
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the second passive wheel and its joint
	{
		// The matrix is relative to the base. First creating the wheel
		wMatrix tm = wMatrix::identity();
		tm.w_pos = wVector(0.0f, -((batteryplacey / 2.0f) - passivewheelr), passivewheelr);
		PhyObject* wheel = world->createEntity(TypeToCreate<PhySphere>(), passivewheelr, "passiveWheel");
		wheel->setMass(passivewheelm);
		wheel->setColor(Qt::blue);
		wheel->setMaterial("epuckTire");
		wheel->setOwner(this, false);
		wheel->setMatrix(tm * m_body->matrix());
		m_wheels.append(wheel);
		m_wheelsTransformation.append(tm);

		// Now creating the joint
		PhyJoint* joint = world->createEntity(TypeToCreate<PhyBallAndSocket>(), m_body, wheel, wheel->matrix().w_pos);
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
	// Adding the sensors. The epuck has 8 proximity infrared sensors
	const double sensorsAngles[] = {-90.0, -54.0, -18.0, 18.0, 54.0, 90.0, 150.0, 210.0};
	for (unsigned int i = 0; i < 8; i++) {
		const double curAngle = sensorsAngles[i] - 90.0; //double(i) * (360.0 / 8.0) + ((360.0 / 8.0) / 2.0);
		const double radius = bodyr;

		wMatrix mtr = wMatrix::yaw(toRad(curAngle + 90.0)) * wMatrix::pitch(PI_GRECO / 2.0);
		mtr.w_pos.x = radius * cos(toRad(curAngle));
		mtr.w_pos.y = radius * sin(toRad(curAngle));
		mtr.w_pos.z = batteryplacez + batteryplacedistancefromground;

		sensors.append(SingleIR::InitParams(m_body, mtr, 0.0, 0.05, 10.0, 5));
	}
	m_proximityIR = world->createEntity(TypeToCreate<SimulatedIRProximitySensorController>(), sensors);

	// Now creating the three ground IR sensors
	sensors.clear();
	wMatrix mtr = wMatrix::yaw(PI_GRECO);

	// Adding the sensors. The e-puck has 3 ground infrared sensors in the front part
	mtr.w_pos = wVector(-batteryplacex / 2.0f, -batteryplacey / 2.0f, batteryplacedistancefromground);
	sensors.append(SingleIR::InitParams(m_body, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	mtr.w_pos = wVector(0.0f, -batteryplacey / 2.0f, batteryplacedistancefromground);
	sensors.append(SingleIR::InitParams(m_body, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	mtr.w_pos = wVector(batteryplacex / 2.0f, -batteryplacey / 2.0f, batteryplacedistancefromground);
	sensors.append(SingleIR::InitParams(m_body, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	m_groundIR = world->createEntity(TypeToCreate<SimulatedIRGroundSensorController>(), sensors);
}

PhyEpuck::~PhyEpuck()
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
	world()->deleteEntity(m_groundIR);
}

void PhyEpuck::preUpdate()
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

void PhyEpuck::postUpdate()
{
	// Updating sensors
	if (m_proximityIR->isEnabled()) {
		m_proximityIR->update();
	}
	if (m_groundIR->isEnabled()) {
		m_groundIR->update();
	}

	// Updating the transformation matrix of the robot. It is coincident with the matrix of the body
	m_shared.getModifiableShared()->tm = m_body->matrix();
}

void PhyEpuck::setProximityIRSensorsGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_proximityIR->setGraphicalProperties(drawSensor, drawRay, drawRealRay);
}

void PhyEpuck::setGroundIRSensorsGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_groundIR->setGraphicalProperties(drawSensor, drawRay, drawRealRay);
}

void PhyEpuck::setDrawFrontMarker(bool drawMarker)
{
	if (getDrawFrontMarker() == drawMarker) {
		return;
	}

	if (drawMarker) {
		m_frontMarker = world()->createEntity(TypeToCreate<PlanarArrowGraphicalMarker>(), bodyr, bodyr / 6.0f, bodyr / 4.0f, 0.7f);
		m_frontMarker->setUseColorTextureOfOwner(false);
		m_frontMarker->setColor(Qt::green);
		m_frontMarker->setTexture("");

		wMatrix displacement = wMatrix::roll(-PI_GRECO / 2.0f);
		displacement.w_pos = wVector(0.0, 0.0, batteryplacedistancefromground + batteryplacez + bodyh + 0.0001f);
		m_frontMarker->setDisplacement(displacement);
		m_frontMarker->setOwner(this, true);
	} else {
		world()->deleteEntity(m_frontMarker);
		m_frontMarker = NULL;
	}
}

bool PhyEpuck::getDrawFrontMarker() const
{
	return (m_frontMarker != NULL);
}

void PhyEpuck::doKinematicSimulation(bool k)
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

void PhyEpuck::setLedColors(QList<QColor> c, QColor upperBaseColor)
{
	if (c.size() != 8) {
		return;
	}

	m_turret->setUseColorTextureOfOwner(false);

	m_shared.getModifiableShared()->ledColors = c;
	QList<PhyCylinderSegmentColor> s;
	const real startAngle = (PI_GRECO / 2.0f) - (PI_GRECO / 8.0f);
	for (int i = 0; i < 8; i++) {
		const real rangeMin = (real(i) * PI_GRECO / 4.0) + startAngle;
		const real rangeMax = (real(i + 1) * PI_GRECO / 4.0) + startAngle;
		s.append(PhyCylinderSegmentColor(SimpleInterval(normalizeRad(rangeMin), normalizeRad(rangeMax)), m_shared->ledColors[i]));
	}
	m_turret->setSegmentsColor(Qt::white, s);
	m_turret->setUpperBaseColor(color());
	m_turret->setLowerBaseColor(upperBaseColor.isValid() ? upperBaseColor : color());
}

QList<QColor> PhyEpuck::ledColors() const
{
	if (m_shared->ledColors.isEmpty()) {
		QList<QColor> c;
		for (int i = 0; i < 8; i++) {
			c.append(color());
		}
		return c;
	} else {
		return m_shared->ledColors;
	}
}

const QList<PhyCylinderSegmentColor>& PhyEpuck::segmentsColor() const
{
	if (m_shared->ledColors.isEmpty()) {
		m_uniformColor[0].color = color();
		return m_uniformColor;
	} else {
		return m_turret->segmentsColor();
	}
}

void PhyEpuck::setLeftWheelDesideredVelocity(real velocity)
{
	m_shared.getModifiableShared()->leftWheelVelocity = velocity;
}

void PhyEpuck::setRightWheelDesideredVelocity(real velocity)
{
	m_shared.getModifiableShared()->rightWheelVelocity = velocity;
}

void PhyEpuck::changedMatrix()
{
	wMatrix tm = matrix();
	m_body->setMatrix(tm);
	for (int i = 0; i < m_wheels.size(); i++) {
		m_wheels[i]->setMatrix(m_wheelsTransformation[i] * tm);
	}
	foreach (PhyJoint* j, m_wheelJoints) {
		j->updateJointInfo();
	}
}

} // end namespace salsa
