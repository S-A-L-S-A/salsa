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

#include "phymarxbot.h"
#include "phybox.h"
#include "phycylinder.h"
#include "phyfixed.h"
#include "phyballandsocket.h"
#include "physphere.h"
#include "physuspension.h"
#include "phycompoundobject.h"
#include "phycone.h"
#include "phyhinge.h"
#include "mathutils.h"
#include "graphicalmarkers.h"
#include <cmath>

namespace farsa {

const real PhyMarXbot::basex = 0.034f;
const real PhyMarXbot::basey = 0.143f;
const real PhyMarXbot::basez = 0.048f;
const real PhyMarXbot::basem = 0.4f;
const real PhyMarXbot::bodyr = 0.085f;
const real PhyMarXbot::bodyh = 0.0055f;
const real PhyMarXbot::bodym = 0.02f;
const real PhyMarXbot::axledistance = 0.104f;
const real PhyMarXbot::trackradius = 0.022f;
const real PhyMarXbot::trackheight = 0.0295f;
const real PhyMarXbot::trackm = 0.05f;
const real PhyMarXbot::treaddepth = 0.004f;
const real PhyMarXbot::wheelr = 0.027f;
const real PhyMarXbot::wheelh = 0.0215f;
const real PhyMarXbot::wheelm = 0.02f;
const real PhyMarXbot::turreth = 0.0385f;
const real PhyMarXbot::turretm = 0.08f;
const real PhyMarXbot::attachdevr = bodyr;
const real PhyMarXbot::attachdevx = turreth * 0.8f;
const real PhyMarXbot::attachdevy = turreth * 0.4f;
const real PhyMarXbot::attachdevz = turreth * 0.2f;
const real PhyMarXbot::attachdevm = 0.03f;

PhyMarXbot::PhyMarXbot(World* world, SharedDataWrapper<Shared> shared, QString name, const wMatrix& tm)
	: QObject(NULL)
	, WObject(world, shared, name, tm)
	, m_shared(shared)
	, m_base(NULL)
	, m_wheels()
	, m_wheelstm()
	, m_wheelJoints()
	, m_wheelsCtrl(NULL)
	, m_turret(NULL)
	, m_turrettm()
	, m_forceSensor(NULL)
	, m_attachdev(NULL)
	, m_attachdevtm()
	, m_attachdevjoint(NULL)
	, m_attachdevCtrl(NULL)
	, m_proximityIR(NULL)
	, m_groundBottomIR(NULL)
	, m_groundAroundIR(NULL)
	, m_tractionSensor(NULL)
	, m_frontMarker(NULL)
	, m_uniformColor()
{
	// --- reference frame
	//  X
	//  ^
	//  |       ------
	//  | ______|____|_________
	//  | |_|_| track |_|_|_|_|
	//  |  -------------------
	//  |  |   battery       |
	//  |  -------------------
	//  | |_|_| track |_|_|_|_|
	//  |       |    |
	//  |       ------
	//  |
	//   ---------------------> Y
	// The front of the robot is towards -Y (positive speeds of the wheel make the robot move towards -Y)

	// --- create material for the marXbot
	//     introducing a semplification for the collision: two robot collide only with the attachring
	world->materials().createMaterial("marXbotLowerBody");
	world->materials().setElasticity("marXbotLowerBody", "default", 0.01f);
	world->materials().setSoftness("marXbotLowerBody", "default", 0.01f);
	world->materials().createMaterial("marXbotUpperBody");
	world->materials().enableCollision("marXbotLowerBody", "marXbotLowerBody", false);
	world->materials().enableCollision("marXbotLowerBody", "marXbotUpperBody", false);

	PhyCompoundComponentsList components(world);
	wMatrix mtr = wMatrix::identity();
	mtr.w_pos[2] = (basez + trackradius * 2.0f) / 2.0f + treaddepth;
	// --- create the body of the base of the marxbot
	components.createComponent(TypeToCreate<PhyBox>(), mtr, basex + 2.0f * trackheight, axledistance, basez + trackradius * 2.0f);
	// merge all togheter. Using a PhyCompoundObject even if there is only one object allows
	// to displace the transformation matrix
	m_base = world->createEntity(TypeToCreate<PhyCompoundObject>(), components, "base");
	m_base->setMass(basem + bodym);
	m_base->setMatrix(tm);
	m_base->setMaterial("marXbotLowerBody");
	m_base->setUseColorTextureOfOwner(false);
	m_base->setOwner(this, false);

	// --- create the two external wheels
	//     they are motorized and they move the robot
	//     and create four spheres for supporting the tracks
	// NOTE: the tracks are not simulated, they are just rigid bodies and
	//       the four sphere move freely in order to approximate the real movement
	// --- position of wheels and their IDs in the vector
	//          +-----+
	//       |3||     ||2|
	//     |1|  |     |  |0|
	//       |5||     ||4|
	//          +-----+
	//  --------------------> X
	// arrays of X and Y offsets for calculate the positions
	double xoffs[6] = {1, -1,  1, -1,  1, -1};
	double yoffs[6] = {0,  0,  1,  1, -1, -1};
	PhyObject* awheel;
	PhyJoint* joint;
	for(int i = 0; i < 6; i++) {
		// --- relative to base
		wMatrix wtm = wMatrix::identity();
		wtm.w_pos = wVector(
			xoffs[i] * (basex + trackheight) / 2.0f + (yoffs[i] == 0 ? xoffs[i] * (trackheight / 2.0 + wheelh / 2.0f + 0.006f) : 0.0f),
			yoffs[i] * (axledistance / 2.0f),
			0.0f);
		if (i < 2) {
			// the two motorized wheels
			wtm.w_pos[2] = wheelr;
			awheel = world->createEntity(TypeToCreate<PhyCylinder>(), wheelr, wheelh, "motorWheel");
		} else {
			// the four sphere supporting the track
			wtm.w_pos[2] = treaddepth - 0.003f;
			awheel = world->createEntity(TypeToCreate<PhySphere>(), treaddepth, "passiveWheel");
		}
		m_wheelstm.append(wtm);
		awheel->setMass(wheelm);
		awheel->setMaterial("marXbotLowerBody");
		awheel->setUseColorTextureOfOwner(false);
		awheel->setOwner(this, false);
		awheel->setMatrix(wtm * tm);
		m_wheels.append(awheel);

		//--- create the joints
		if (i < 2) {
			// the two motorized wheels
			joint = world->createEntity(
				TypeToCreate<PhySuspension>(),
				m_base, m_wheels[i],
				m_base->matrix().x_ax, // rotation axis
				m_wheels[i]->matrix().w_pos,
				m_base->matrix().z_ax); // suspension 'vertical' axis
			joint->dofs()[0]->disableLimits();
		} else {
			// the four sphere supporting the track
			joint = world->createEntity(
				TypeToCreate<PhyBallAndSocket>(),
				m_base, m_wheels[i],
				m_wheels[i]->matrix().w_pos);
		}
		joint->setOwner(this, false);
		joint->updateJointInfo();
		m_wheelJoints.append(joint);
	}

	// Creating the turret
	m_turret = world->createEntity(TypeToCreate<PhyCylinder>(), bodyr, turreth);
	m_turret->setMass(turretm);
	m_turrettm = wMatrix::yaw(toRad(-90.0f));
	m_turrettm.w_pos.z = 0.005f + basez + bodyh + turreth + 0.0015f;
	m_turret->setMatrix(m_turrettm * tm);
	m_turret->setMaterial("marXbotUpperBody");
	m_turret->setOwner(this, false);
	m_turret->setUseColorTextureOfOwner(true);

	// --- create the joint for sensing the force
	m_forceSensor = world->createEntity(TypeToCreate<PhyFixed>(), m_base, m_turret);
	m_forceSensor->setOwner(this, false);

	// We always create the attachment device controller, it will however not work unless the attachment
	// device is enabled
	m_attachdevCtrl = world->createEntity(TypeToCreate<MarXbotAttachmentDeviceMotorController>(), this);
	m_attachdevCtrl->setOwner(this, false);

	// create the motor controller
	QVector<PhyDOF*> motors;
	motors << m_wheelJoints[0]->dofs()[0] << m_wheelJoints[1]->dofs()[0];
	m_wheelsCtrl = world->createEntity(TypeToCreate<WheelMotorController>(), motors);
	m_wheelsCtrl->setOwner(this, false);
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI DECIDERE PER LE VELOCITÀ MASSIME E MINIME DELLE RUOTE
#endif
	m_wheelsCtrl->setSpeedLimits( -10.0, -10.0, 10.0, 10.0 );

	// Connecting wheels speed signals to be able to move the robot when in kinematic
	connect(m_wheelJoints[0]->dofs()[0], SIGNAL(changedDesiredVelocity(real)), this, SLOT(setRightWheelDesideredVelocity(real)));
	connect(m_wheelJoints[1]->dofs()[0], SIGNAL(changedDesiredVelocity(real)), this, SLOT(setLeftWheelDesideredVelocity(real)));

	// Creating the proximity IR sensors
	QVector<SingleIR::InitParams> sensors;

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI DECIDERE PER I RANGE/APERTURE E LE POSIZIONI (SPECIE PER I GROUND)
#endif
	// Adding the sensors. The marxbot has 24 proximity infrared sensors
	for (unsigned int i = 0; i < 24; i++) {
		const double curAngle = double(i) * (360.0 / 24.0) + ((360.0 / 24.0) / 2.0);
		const double radius = bodyr;

		wMatrix mtr = wMatrix::yaw(toRad(curAngle + 90.0)) * wMatrix::pitch(PI_GRECO / 2.0);
		mtr.w_pos.x = radius * cos(toRad(curAngle));
		mtr.w_pos.y = radius * sin(toRad(curAngle));
		mtr.w_pos.z = 0.005f + basez + bodyh + turreth / 2.0f + 0.0015f;

		sensors.append(SingleIR::InitParams(m_base, mtr, 0.0, 0.1, 10.0, 5));
	}
	m_proximityIR = world->createEntity(TypeToCreate<SimulatedIRProximitySensorController>(), sensors);

	// Now creating the ground IR sensors below the battery pack
	sensors.clear();
	mtr = wMatrix::yaw(PI_GRECO);
	const double distFromBorder = 0.003;

	// Adding the sensors. The marxbot has 4 ground infrared sensors below the battery pack
	mtr.w_pos = wVector(basex / 2.0f + trackheight - distFromBorder, axledistance / 2.0f - distFromBorder, treaddepth);
	sensors.append(SingleIR::InitParams(m_base, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	mtr.w_pos = wVector(basex / 2.0f + trackheight - distFromBorder, -axledistance / 2.0f + distFromBorder, treaddepth);
	sensors.append(SingleIR::InitParams(m_base, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	mtr.w_pos = wVector(-basex / 2.0f - trackheight + distFromBorder, axledistance / 2.0f - distFromBorder, treaddepth);
	sensors.append(SingleIR::InitParams(m_base, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	mtr.w_pos = wVector(-basex / 2.0f - trackheight + distFromBorder, -axledistance / 2.0f + distFromBorder, treaddepth);
	sensors.append(SingleIR::InitParams(m_base, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	m_groundBottomIR = world->createEntity(TypeToCreate<SimulatedIRGroundSensorController>(), sensors);

	// Creating the ground IR sensors below the base
	sensors.clear();

	// Adding the sensors. The marxbot has 8 ground infrared sensors below the base
	for (unsigned int i = 0; i < 8; i++) {
		const double curAngle = double(i) * (360.0 / 8.0);
		const double radius = bodyr - 0.003;

		wMatrix mtr = wMatrix::yaw(PI_GRECO);
		mtr.w_pos.x = radius * cos(toRad(curAngle));
		mtr.w_pos.y = radius * sin(toRad(curAngle));
		mtr.w_pos.z = 0.005f + basez + bodyh + turreth / 2.0f + 0.0015f;

		sensors.append(SingleIR::InitParams(m_base, mtr, 0.0, mtr.w_pos.z + 0.005, 0.0, 1));
	}
	m_groundAroundIR = world->createEntity(TypeToCreate<SimulatedIRGroundSensorController>(), sensors);

	// Creating the traction sensor
	m_tractionSensor = world->createEntity(TypeToCreate<TractionSensorController>(), m_forceSensor);

	m_uniformColor.append(PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), color()));
}

PhyMarXbot::~PhyMarXbot() {
	// First of all signalling we are about to delete the attachment device (and all the rest)
	m_attachdevCtrl->attachmentDeviceAboutToBeDestroyed();

	world()->deleteEntity(m_wheelsCtrl);
	world()->deleteEntity(m_attachdevCtrl);
	world()->deleteEntity(m_proximityIR);
	world()->deleteEntity(m_groundBottomIR);
	world()->deleteEntity(m_groundAroundIR);
	foreach(PhyJoint* susp, m_wheelJoints) {
		world()->deleteEntity(susp);
	}
	foreach(PhyObject* w, m_wheels) {
		world()->deleteEntity(w);
	}
	world()->deleteEntity(m_forceSensor);
	world()->deleteEntity(m_attachdevjoint);
	world()->deleteEntity(m_turret);
	world()->deleteEntity(m_base);
	world()->deleteEntity(m_attachdev);
}

void PhyMarXbot::preUpdate()
{
	// Updating motors
	if (m_wheelsCtrl->isEnabled()) {
		m_wheelsCtrl->update();
	}
	if (m_attachdevCtrl->isEnabled()) {
		m_attachdevCtrl->update();
	}

	if (m_shared->kinematicSimulation) {
		// In kinematic mode, we have to manually move the robot depending on the wheel velocities
		wMatrix mtr = matrix();
		wheeledRobotsComputeKinematicMovement(mtr, m_shared->leftWheelVelocity, m_shared->rightWheelVelocity, wheelr, m_wheelstm[0].w_pos.x - m_wheelstm[1].w_pos.x, world()->timeStep());

		// This will also update the position of all pieces
		setMatrix(mtr);
	}
}

void PhyMarXbot::postUpdate()
{
	// Updating sensors
	if (m_proximityIR->isEnabled()) {
		m_proximityIR->update();
	}
	if (m_groundBottomIR->isEnabled()) {
		m_groundBottomIR->update();
	}
	if (m_groundAroundIR->isEnabled()) {
		m_groundAroundIR->update();
	}

	// Updating the transformation matrix of the robot. It is coincident with the matrix of the base
	m_shared.getModifiableShared()->tm = m_base->matrix();
}

void PhyMarXbot::setProximityIRSensorsGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_proximityIR->setGraphicalProperties(drawSensor, drawRay, drawRealRay);
}

void PhyMarXbot::setGroundBottomIRSensorsGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_groundBottomIR->setGraphicalProperties(drawSensor, drawRay, drawRealRay);
}

void PhyMarXbot::setGroundAroundIRSensorsGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_groundAroundIR->setGraphicalProperties(drawSensor, drawRay, drawRealRay);
}

void PhyMarXbot::setDrawFrontMarker(bool drawMarker)
{
	if (getDrawFrontMarker() == drawMarker) {
		return;
	}

	if (drawMarker) {
		m_frontMarker =  world()->createEntity(TypeToCreate<PlanarArrowGraphicalMarker>(), PhyMarXbot::bodyr, PhyMarXbot::bodyr / 6.0f, PhyMarXbot::bodyr / 4.0f, 0.7f);
		m_frontMarker->setUseColorTextureOfOwner(false);
		m_frontMarker->setColor(Qt::green);
		m_frontMarker->setTexture("");

		wMatrix displacement = wMatrix::roll(-PI_GRECO / 2.0f);
		displacement.w_pos = wVector(0.0, 0.0, basez + trackradius * 2.0f + treaddepth + turreth - 0.010f + 0.0001f);
		m_frontMarker->setDisplacement(displacement);
		m_frontMarker->setOwner(this, true);
	} else {
		world()->deleteEntity(m_frontMarker);
		m_frontMarker = NULL;
	}
}

bool PhyMarXbot::getDrawFrontMarker() const
{
	return (m_frontMarker != NULL);
}

void PhyMarXbot::enableAttachmentDevice(bool enable)
{
	if (attachmentDeviceEnabled() == enable) {
		return;
	}

	if (enable) {
		// This little cube shows the position of the attachment fingers
		m_attachdev = world()->createEntity(TypeToCreate<PhyBox>(), attachdevx, attachdevy, attachdevz);
		m_attachdev->setMass(attachdevm);
		m_attachdevtm = m_turret->matrix();
		m_attachdevtm.w_pos.y -= attachdevr;
		m_attachdev->setMatrix(m_attachdevtm);
		m_attachdev->setOwner(this, false);

		// Creating the joint for the attachment device
		m_attachdevjoint = world()->createEntity(TypeToCreate<PhyHinge>(), m_turret, m_attachdev, wVector(1.0f, 0.0, 0.0), wVector(0.0, 0.0, 0.0), 0.0);
		m_attachdevjoint->dofs()[0]->disableLimits();
		m_attachdevjoint->dofs()[0]->setDesiredPosition(0.0); // This way the attachdev doesn't rotate
		m_attachdevjoint->dofs()[0]->setStiffness(0.1f); // This way the attachdev motor is not very strong
		m_attachdevjoint->setOwner(this, false);
	} else {
		// Notifying the attachment device controller
		m_attachdevCtrl->attachmentDeviceAboutToBeDestroyed();
		// Destroying the joint and the attachment device
		world()->deleteEntity(m_attachdevjoint);
		m_attachdevjoint = NULL;
		world()->deleteEntity(m_attachdev);
		m_attachdev = NULL;
	}
}

void PhyMarXbot::resetAttachmentDevice()
{
	// If the attachment device is enabled, we simply disable and then re-enable it
	if (attachmentDeviceEnabled()) {
		enableAttachmentDevice(false);
		enableAttachmentDevice(true);
	}
}

void PhyMarXbot::doKinematicSimulation(bool k)
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
		m_forceSensor->enable(false);
		if (m_attachdevjoint != NULL) {
			m_attachdevjoint->enable(false);
		}

		// ... then setting all objects to kinematic behaviour
		m_base->setKinematic(true, true);
		for (int i = 0; i < m_wheels.size(); i++) {
			m_wheels[i]->setKinematic(true, true);
		}
		m_turret->setKinematic(true, true);
		if (m_attachdev != NULL) {
			m_attachdev->setKinematic(true, true);
		}
	} else {
		// First setting all objects to dynamic behaviour...
		m_base->setKinematic(false);
		for (int i = 0; i < m_wheels.size(); i++) {
			m_wheels[i]->setKinematic(false);
		}
		m_turret->setKinematic(false);
		if (m_attachdev != NULL) {
			m_attachdev->setKinematic(false);
		}

		// ... then enabling all joints (if the corresponding part is enabled)
		for (int i = 0; i < m_wheelJoints.size(); i++) {
			m_wheelJoints[i]->enable(true);
			m_wheelJoints[i]->updateJointInfo();
		}
		m_forceSensor->enable(true);
		m_forceSensor->updateJointInfo();
		if (m_attachdevjoint != NULL) {
			m_attachdevjoint->enable(true);
			m_attachdevjoint->updateJointInfo();
		}
	}
}

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning PROBLEMA RELATIVO AI LED DEL MARXBOT: NON C È L'ATTACHRING, SOLO LA TORRETTA CHE PERÒ È FISSA. NEL ROBOT REALE I LED SONO SULL' ATTACHRING, QUINDI RUOTANO (CONTROLLARE!!!), QUI SONO SULLA TORRETTA QUINDI SONO FISSI
#endif
void PhyMarXbot::setLedColors(QList<QColor> c)
{
	if (c.size() != 12) {
		return;
	}

	m_turret->setUseColorTextureOfOwner(false);

	m_shared.getModifiableShared()->ledColors = c;
	QList<PhyCylinderSegmentColor> s;
	const real startAngle = (PI_GRECO / 2.0f) - (PI_GRECO / 12.0f);
	for (int i = 0; i < 12; i++) {
		const real rangeMin = (real(i) * PI_GRECO / 6.0) + startAngle;
		const real rangeMax = (real(i + 1) * PI_GRECO / 6.0) + startAngle;
		s.append(PhyCylinderSegmentColor(SimpleInterval(normalizeRad(rangeMin), normalizeRad(rangeMax)), m_shared->ledColors[i]));
	}
	m_turret->setSegmentsColor(Qt::white, s);
	m_turret->setUpperBaseColor(color());
	m_turret->setLowerBaseColor(color());
}

QList<QColor> PhyMarXbot::ledColors() const
{
	if (m_shared->ledColors.isEmpty()) {
		QList<QColor> c;
		for (int i = 0; i < 12; i++) {
			c.append(color());
		}
		return c;
	} else {
		return m_shared->ledColors;
	}
}

const QList<PhyCylinderSegmentColor>& PhyMarXbot::segmentsColor() const
{
	if (m_shared->ledColors.isEmpty()) {
		m_uniformColor[0].color = color();
		return m_uniformColor;
	} else {
		return m_turret->segmentsColor();
	}
}

void PhyMarXbot::setLeftWheelDesideredVelocity(real velocity)
{
	m_shared.getModifiableShared()->leftWheelVelocity = velocity;
}

void PhyMarXbot::setRightWheelDesideredVelocity(real velocity)
{
	m_shared.getModifiableShared()->rightWheelVelocity = velocity;
}

void PhyMarXbot::changedMatrix() {
	wMatrix tm = matrix();
	m_base->setMatrix( tm );
	for(int i = 0; i < m_wheels.size(); i++) {
		m_wheels[i]->setMatrix(m_wheelstm[i] * tm);
	}
	foreach(PhyJoint* ajoint, m_wheelJoints) {
		ajoint->updateJointInfo();
	}
	m_turret->setMatrix(m_turrettm * tm);
	m_forceSensor->updateJointInfo();
	if ((m_attachdev != NULL) && (m_attachdevjoint != NULL)) {
		m_attachdev->setMatrix(m_attachdevtm * tm);
		m_attachdevjoint->updateJointInfo();
	}
}

} // end namespace farsa
