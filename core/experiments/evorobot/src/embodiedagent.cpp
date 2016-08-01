/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#include "embodiedagent.h"

namespace salsa {

EmbodiedAgent::EmbodiedAgent(ConfigurationManager& params)
	: Component(params)
	, m_enabled(true)
	, m_controller()
	, m_robot()
	, m_inputs()
	, m_outputs()
{
	// We need notifications for the world resource, because if it is destroyed or declared NULL we
	// must invalidate the robot pointer
	addNotifiedResource("world");
}

EmbodiedAgent::~EmbodiedAgent()
{
	// Deleting sensors and motors
	foreach (AbstractControllerInput* i, m_inputs) {
		delete i;
	}
	foreach (AbstractControllerOutput* o, m_outputs) {
		delete o;
	}

	try {
		deleteResource("controller");
		deleteResource("robot");
	} catch (...) {
		// This is to avoid having exception escape from the destructor. The only possible
		// exception here is because one of the resources hasn't been declared because of
		// a problem in the configuration phase, so we can safely ignore it
	}
}

void EmbodiedAgent::configure()
{
	m_controller.reset(configurationManager().getComponentFromGroup<Controller>(confPath() + "CONTROLLER", false));
	declareResource("controller", m_controller.get());

	createRobot();

	QStringList sensorsList = configurationManager().getGroupsWithPrefixList(confPath(), "SENSOR");
	sensorsList.sort();
	foreach (QString s, sensorsList) {
		m_inputs.append(configurationManager().getComponentFromGroup<AbstractControllerInput>(confPath() + s));
	}

	QStringList motorsList = configurationManager().getGroupsWithPrefixList(confPath(), "MOTOR");
	motorsList.sort();
	foreach (QString m, motorsList) {
		m_outputs.append(configurationManager().getComponentFromGroup<AbstractControllerOutput>(confPath() + m));
	}
}

void EmbodiedAgent::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("A simulated agent", "This class models a simulated agent, with a robotic body, a controller and various sensors and motors");

	d.describeSubgroup("ROBOT").props(ParamIsMandatory).componentType("Robot").help("The robotic model");
	d.describeSubgroup("CONTROLLER").props(ParamIsMandatory).componentType("Controller").help("The controller of the agent");
	d.describeSubgroup("SENSOR").props(ParamAllowMultiple).componentType("AbstractControllerInput").help("One of the Sensors from which the controller will receive information about the environment");
	d.describeSubgroup("MOTOR").props(ParamAllowMultiple).componentType("AbstractControllerOutput").help("One of the Motors with which the controller acts on the robot and on the environment");
}

void EmbodiedAgent::recreateRobot()
{
	if (!m_enabled) {
		return;
	}

	// We cannot create a new robot until the old one is destroyed, so we need to perform the
	// following steps:
	// 	- declare the robot resource as NULL
	// 	- destroy the robot
	// 	- create a new robot
	// 	- declare the robot resource
	// The first two steps are perfomed by destroyRobot(), while the second two by createRobot()

	destroyRobot();
	createRobot();
}

void EmbodiedAgent::disable()
{
	if (m_enabled) {
		m_enabled = false;

		destroyRobot();
	}
}

void EmbodiedAgent::enable()
{
	if (!m_enabled) {
		m_enabled = true;

		createRobot();
	}
}

bool EmbodiedAgent::enabled() const
{
	return m_enabled;
}

void EmbodiedAgent::updateSensors()
{
	if (!m_enabled) {
		return;
	}

	foreach (AbstractControllerInput* sensor, m_inputs) {
		sensor->update();
	}
}

void EmbodiedAgent::updateController()
{
	if (!m_enabled) {
		return;
	}

	m_controller->update();
}

void EmbodiedAgent::updateMotors()
{
	if (!m_enabled) {
		return;
	}

	foreach (AbstractControllerOutput* motor, m_outputs) {
		motor->update();
	}
}

Robot* EmbodiedAgent::robot()
{
	return m_robot.get();
}

const Robot* EmbodiedAgent::robot() const
{
	return m_robot.get();
}

Controller* EmbodiedAgent::controller()
{
	return m_controller.get();
}

const Controller* EmbodiedAgent::controller() const
{
	return m_controller.get();
}

int EmbodiedAgent::numControllerInputs() const
{
	return m_inputs.size();
}

AbstractControllerInput* EmbodiedAgent::getControllerInput(int i) const
{
	return m_inputs[i];
}

int EmbodiedAgent::numControllerOutputs() const
{
	return m_outputs.size();
}

AbstractControllerOutput* EmbodiedAgent::getControllerOutput(int i) const
{
	return m_outputs[i];
}

void EmbodiedAgent::destroyRobot()
{
	declareResourceAsNull("robot");
	m_robot.reset();
}

void EmbodiedAgent::createRobot()
{
	m_robot.reset(configurationManager().getComponentFromGroup<Robot>(confPath() + "ROBOT"));
	declareResource("robot", m_robot.get());
}

#warning Perhaps we should have a class wrapping the robot so that it is that class (or perhaps a subclass of that one) that monitors world? We perhaps need this if we have robots that are not physical...
#warning FOR THE MOMENT WE ALSO CHECK ResourceModified BELOW, BUT WE SHOULD INSTEAD LISTEN FOR ENITY DESTRUCTION IN WORLD, NOT USE RESOURCES FOR THIS
void EmbodiedAgent::resourceChanged(QString name, Component* owner, ResourceChangeType changeType)
{
	// If world was destroyed, we must release the robot pointer
	if (name == "world") {
		if ((changeType == ResourceDeclaredAsNull) || (changeType == ResourceDeleted) || (changeType == ResourceModified)) {
			declareResourceAsNull("robot");
			m_robot.release();
		}
	}
}

} // end namespace salsa
