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

#ifndef EMBODIEDAGENT_H
#define EMBODIEDAGENT_H

#include "component.h"
#include "controller.h"
#include "robots.h"
#include <memory>

namespace salsa {

/**
 * \brief The class modelling an embodied agent
 *
 * The agent is made up of a robotic body, sensors, motors and a controller
 * (with the associated iterator). This class implements both the
 * ControllerInputsList and the ControllerOutputsList interfaces, so it can be
 * used in the inputsList and outputsList parameters of the Controller. To
 * perform a simulation step call in sequence the following functions:
 * 	- updateSensors();
 * 	- updateController();
 * 	- updateMotors().
 * There is not a unified update() function to be able to precisely control the
 * execution of the simulation step. An agent can be disabled. In this case the
 * update functions do nothing and neither do the recreateRobot function. When
 * disabled the robot is destroyed. This class does not configures in
 * constructor to avoid cycles with the controller (which requestes a
 * non-configured ControllerInputsList and ControllerOutputsList)
 *
 * This class has no configuration parameters but has the following subgroups:
 * 	- ROBOT, the robotic body (a subclass of Robot)
 * 	- CONTROLLER, the controller (a subclass of Controller)
 * 	- SENSOR:XX, one or more sensors (subclasses of AbstractControllerInput)
 * 	- MOTOR:XX, one or more motors (subclasses of AbstractControllerOutput)
 * This class declares the following resources:
 * 	- "robot", the robotic body
 * 	- "controller", the controller
 */
class SALSA_EXPERIMENTS_API EmbodiedAgent : public Component, public ControllerInputsList, public ControllerOutputsList
{
public:
	/**
	 * \brief The function returning whether instances of this class
	 *        configure themselves in the constructor or not
	 *
	 * \return true, this class configures in constructor
	 */
	static bool configuresInConstructor()
	{
		return false;
	}

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	EmbodiedAgent(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~EmbodiedAgent();

	/**
	 * \brief Configures the object using the ConfigurationManager
	 *
	 */
	virtual void configure();

	/**
	 * \brief Add to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Recreates the robot
	 */
	void recreateRobot();

	/**
	 * \brief Disables the agent
	 *
	 * This means that the sensors, motors and controller are no longer
	 * updated and that the robot is destroyed
	 */
	void disable();

	/**
	 * \brief Enables the agent
	 *
	 * This means that the sensors, motors and controller are updated and
	 * that the robot is created
	 */
	void enable();

	/**
	 * \brief Returns true if the agent is enabled
	 *
	 * \return true if the agent is enabled
	 */
	bool enabled() const;

	/**
	 * \brief Updates all sensors
	 */
	void updateSensors();

	/**
	 * \brief Performs a step of the controller
	 */
	void updateController();

	/**
	 * \brief Updates all motors
	 */
	void updateMotors();

	/**
	 * \brief Returns the robot
	 *
	 * This is NULL if the agent is disabled
	 * \return the robotic model
	 */
	Robot* robot();

	/**
	 * \brief Returns the robot (const version)
	 *
	 * This is NULL if the agent is disabled
	 * \return the robotic model
	 */
	const Robot* robot() const;

	/**
	 * \brief Returns the controller
	 *
	 * \return the controller
	 */
	Controller* controller();

	/**
	 * \brief Returns the controller (const version)
	 *
	 * \return the controller
	 */
	const Controller* controller() const;

	/**
	 * \brief Returns the number of controller inputs
	 *
	 * \return the number of controller inputs
	 */
	virtual int numControllerInputs() const;

	/**
	 * \brief Returns the i-th input
	 *
	 * \param i the input to return (between 0 and numInputBlocks() - 1)
	 * \return the input
	 */
	virtual AbstractControllerInput* getControllerInput(int i) const;

	/**
	 * \brief Returns the number of controller outputs
	 *
	 * \return the number of controller outputs
	 */
	virtual int numControllerOutputs() const;

	/**
	 * \brief Returns the i-th output
	 *
	 * \param i the output to return (between 0 and numOutputBlocks() - 1)
	 * \return the output
	 */
	virtual AbstractControllerOutput* getControllerOutput(int i) const;

private:
	void destroyRobot();
	void createRobot();
	// This is needed because if the world is destroyed, we must invalidate the robot
	virtual void resourceChanged(QString name, Component* owner, ResourceChangeType changeType);

	bool m_enabled;
	std::unique_ptr<Controller> m_controller;
	std::unique_ptr<Robot> m_robot;
	QVector<AbstractControllerInput*> m_inputs;
	QVector<AbstractControllerOutput*> m_outputs;
};

} // end namespace salsa

#endif
