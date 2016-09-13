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

#include "sensorcontrollers.h"
#include "mathutils.h"
#include "phyobject.h"

namespace salsa {

SensorController::SensorController(World* world, SharedDataWrapper<Shared> shared, QString name)
	: WEntity(world, shared, name)
	, m_enabled(true)
{
}

SensorController::~SensorController()
{
	// Nothing to do here
}

IRSensorController::IRSensorController(World* world, SharedDataWrapper<Shared> shared, unsigned int numSensors, QString name)
	: SensorController(world, shared, name)
	, m_activations(numSensors)
	, m_activeSensor(numSensors, true)
{
}

IRSensorController::~IRSensorController()
{
	// Nothing to do here
}

void IRSensorController::setSensorActive(int i, bool active)
{
	m_activeSensor[i] = active;
}

SimulatedIRProximitySensorController::SimulatedIRProximitySensorController(World* world, SharedDataWrapper<Shared> shared, const QVector<SingleIR::InitParams>& sensors, QString name)
	: IRSensorController(world, shared, sensors.size(), name)
	, m_sensors(sensors.size())
	, m_drawSensor(false)
	, m_drawRay(false)
	, m_drawRealRay(false)
{
	// Creating sensors
	for (int i = 0; i < m_sensors.size(); ++i) {
		m_sensors[i] = world->createEntity(TypeToCreate<SingleIR>(), sensors[i]);
		m_sensors[i]->setOwner(this);
	}

	setGraphicalProperties(m_drawSensor, m_drawRay, m_drawRealRay);
}

SimulatedIRProximitySensorController::~SimulatedIRProximitySensorController()
{
	// Nothing to do here
}

void SimulatedIRProximitySensorController::update()
{
	for (int i = 0; i < m_sensors.size(); i++) {
		if (m_activeSensor[i]) {
			m_sensors[i]->update();
			// If there was no hit, distance is 1.0 and activation 0.0
			m_activations[i] = 1.0 - m_sensors[i]->getRayCastHit().distance;
		} else {
			m_activations[i] = 0.0;
		}
	}
}

void SimulatedIRProximitySensorController::setSensorActive(int i, bool active)
{
	// Calling parent function
	IRSensorController::setSensorActive(i, active);

	if (active) {
		m_sensors[i]->setGraphicalProperties(m_drawSensor, m_drawRay, m_drawRealRay);
	} else {
		m_sensors[i]->setGraphicalProperties(false, m_drawRay, m_drawRealRay);
	}
}

void SimulatedIRProximitySensorController::setGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_drawSensor = drawSensor;
	m_drawRay = drawRay;
	m_drawRealRay = drawRealRay;

	for (int i = 0; i < m_sensors.size(); i++) {
		m_sensors[i]->setGraphicalProperties(m_activeSensor[i] && drawSensor, drawRay, drawRealRay);
	}
}

SimulatedIRGroundSensorController::SimulatedIRGroundSensorController(World* world, SharedDataWrapper<Shared> shared, const QVector<SingleIR::InitParams>& sensors, QString name)
	: IRSensorController(world, shared, sensors.size(), name)
	, m_sensors(sensors.size())
	, m_drawSensor(false)
	, m_drawRay(false)
	, m_drawRealRay(false)
{
	// Creating sensors
	for (int i = 0; i < m_sensors.size(); ++i) {
		m_sensors[i] = world->createEntity(TypeToCreate<SingleIR>(), sensors[i]);
	}

	setGraphicalProperties(m_drawSensor, m_drawRay, m_drawRealRay);
}

SimulatedIRGroundSensorController::~SimulatedIRGroundSensorController()
{
	// Nothing to do here
}

void SimulatedIRGroundSensorController::update()
{
	for (int i = 0; i < m_sensors.size(); i++) {
		if (m_activeSensor[i]) {
			m_sensors[i]->update();

			// Now taking the color of the nearest solid, converting to HSL and taking
			// normalized lightness as activation
			if (m_sensors[i]->getRayCastHit().object != nullptr) {
				m_activations[i] = m_sensors[i]->getRayCastHit().object->color().lightnessF();
			} else {
				m_activations[i] = 0.0;
			}
		} else {
			m_activations[i] = 0.0;
		}
	}
}

void SimulatedIRGroundSensorController::setSensorActive(int i, bool active)
{
	// Calling parent function
	IRSensorController::setSensorActive(i, active);

	if (active) {
		m_sensors[i]->setGraphicalProperties(m_drawSensor, m_drawRay, m_drawRealRay);
	} else {
		m_sensors[i]->setGraphicalProperties(false, m_drawRay, m_drawRealRay);
	}
}

void SimulatedIRGroundSensorController::setGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	m_drawSensor = drawSensor;
	m_drawRay = drawRay;
	m_drawRealRay = drawRealRay;

	for (int i = 0; i < m_sensors.size(); i++) {
		m_sensors[i]->setGraphicalProperties(m_activeSensor[i] && drawSensor, drawRay, drawRealRay);
	}
}

TractionSensorController::TractionSensorController(World* world, SharedDataWrapper<Shared> shared, const PhyJoint* j, QString name)
	: SensorController(world, shared, name)
	, m_joint(j)
{
}

TractionSensorController::~TractionSensorController()
{
}

void TractionSensorController::update()
{
	// Nothing to do here, we read the traction force directly in the traction() function
}

wVector TractionSensorController::traction() const
{
	return -m_joint->getForceOnJoint();
}

} // end namespace salsa
