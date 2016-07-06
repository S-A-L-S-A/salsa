// /********************************************************************************
//  *  FARSA Experiments Library                                                   *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #include "marxbotmotors.h"
// #include "configurationhelper.h"
// #include "randomgenerator.h"
// #include "logger.h"
//
// namespace farsa {
//
// MarXbotMotor::MarXbotMotor(ConfigurationParameters& params, QString prefix) :
// 	Motor(params, prefix),
// 	m_marxbotResource("robot"),
// 	m_neuronsIteratorResource("neuronsIterator")
// {
// 	// Reading parameters
// 	m_marxbotResource = actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "marxbot", m_marxbotResource));
// 	m_neuronsIteratorResource = actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "neuronsIterator", m_neuronsIteratorResource));
//
// 	// Declaring the resources that are needed here
// 	usableResources(QStringList() << m_marxbotResource << m_neuronsIteratorResource);
// }
//
// MarXbotMotor::~MarXbotMotor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	Motor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotMotor", this);
// 	params.createParameter(prefix, "marxbot", m_marxbotResource);
// 	params.createParameter(prefix, "neuronsIterator", m_neuronsIteratorResource);
// }
//
// void MarXbotMotor::describe(QString type)
// {
// 	// Calling parent function
// 	Motor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The base class for MarXbot motors");
// 	d.describeString("marxbot").def("robot").help("The name of the resource associated with the MarXbot robot to use (default is \"robot\")");
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("The name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// }
//
// void MarXbotMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	Motor::resourceChanged(resourceName, changeType);
//
// 	// Here we only check whether the resource has been deleted and reset the check flag, the
// 	// actual work is done in subclasses
// 	if (changeType == Deleted) {
// 		resetNeededResourcesCheck();
// 		return;
// 	}
// }
//
// MarXbotWheelVelocityMotor::MarXbotWheelVelocityMotor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotMotor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL)
// {
// }
//
// MarXbotWheelVelocityMotor::~MarXbotWheelVelocityMotor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotWheelVelocityMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotMotor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotWheelVelocityMotor", this);
// }
//
// void MarXbotWheelVelocityMotor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotMotor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The motor controlling the velocity of the wheels of the MarXbot robot");
// }
//
// void MarXbotWheelVelocityMotor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting limit velocities for wheels
// 	double minSpeed1;
// 	double minSpeed2;
// 	double maxSpeed1;
// 	double maxSpeed2;
// 	m_robot->wheelsController()->getSpeedLimits(minSpeed1, minSpeed2, maxSpeed1, maxSpeed2);
//
// 	// Computing desired wheel velocities. When appying noise we don't check boundaries, so robots could go a bit faster than their maximum speed
// 	m_neuronsIterator->setCurrentBlock(name());
// 	const double v1 = (maxSpeed1 - minSpeed1) * applyNoise(m_neuronsIterator->getOutput(), 0.0, -1.0) + minSpeed1;
// 	m_neuronsIterator->nextNeuron();
// 	const double v2 = (maxSpeed2 - minSpeed2) * applyNoise(m_neuronsIterator->getOutput(), 0.0, -1.0) + minSpeed2;
//
// 	m_robot->wheelsController()->setSpeeds(v1, v2);
//
// }
//
// int MarXbotWheelVelocityMotor::size()
// {
// 	return 2;
// }
//
// void MarXbotWheelVelocityMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotMotor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("W" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotWheelVelOrient::MarXbotWheelVelOrient(ConfigurationParameters& params, QString prefix) :
// 	MarXbotMotor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL)
// {
// }
//
// MarXbotWheelVelOrient::~MarXbotWheelVelOrient()
// {
// 	// Nothing to do here
// }
//
// void MarXbotWheelVelOrient::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotMotor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotWheelVelOrient", this);
// }
//
// void MarXbotWheelVelOrient::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotMotor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The motor controlling the velocity of the wheels of the MarXbot robot");
// }
//
// void MarXbotWheelVelOrient::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting limit velocities for wheels
// 	double minSpeed1;
// 	double minSpeed2;
// 	double maxSpeed1;
// 	double maxSpeed2;
// 	m_robot->wheelsController()->getSpeedLimits(minSpeed1, minSpeed2, maxSpeed1, maxSpeed2);
//
// 	// Computing desired wheel velocities
// 	m_neuronsIterator->setCurrentBlock(name());
//
// 	const double vel = (maxSpeed1 - minSpeed1) * applyNoise(m_neuronsIterator->getOutput(), 0.0, -1.0) + minSpeed1;
// 	m_neuronsIterator->nextNeuron();
// 	const double dir = applyNoise(m_neuronsIterator->getOutput(), 0.0, 1.0);
//
// 	// the delta parameter determine how sharply the direction change (a delta=2 or 3 would lead to smooter orientation change than delta=1)
// 	// dir=0.5 means no orientation change.
// 	const double delta = 1.0;
// 	const double offset = (pow(-2.0, delta * 2 + 1) * pow(dir - 0.5, delta * 2)) + 1;
// 	double v1;
// 	double v2;
//
// 	if (dir > 0.5) {
// 		v1 = vel * offset;
// 		v2 = vel;
// 	} else {
// 		v1 = vel;
// 		v2 = vel * offset;
// 	}
//
// 	m_robot->wheelsController()->setSpeeds(v1, v2);
// }
//
// int MarXbotWheelVelOrient::size()
// {
// 	return 2;
// }
//
// void MarXbotWheelVelOrient::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotMotor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("W" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotColorMotor::MarXbotColorMotor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotMotor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_onColor(ConfigurationHelper::getString(params, prefix + "onColor", "+00FF00").replace("+", "#")), // We have to do this because # is for comments in .ini files
// 	m_offColor(ConfigurationHelper::getString(params, prefix + "offColor", "+0000FF").replace("+", "#")), // We have to do this because # is for comments in .ini files
// 	m_onThreshold(ConfigurationHelper::getDouble(params, prefix + "onThreshold", 0.8f)),
// 	m_offThreshold(ConfigurationHelper::getDouble(params, prefix + "offThreshold", 0.2f))
// {
// 	if (m_onThreshold < m_offThreshold) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "offThreshold", params.getValue(prefix + "offThreshold"), "The offThreshold must be less than the onThreshold");
// 	}
// 	if (!m_onColor.isValid()) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "onColor", params.getValue(prefix + "onColor"), "The onColor value is not a valid color");
// 	}
// 	if (!m_offColor.isValid()) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "offColor", params.getValue(prefix + "offColor"), "The offColor value is not a valid color");
// 	}
// }
//
// MarXbotColorMotor::~MarXbotColorMotor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotColorMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotMotor::save(params, prefix);
//
// 	// Saving our parameters
// 	params.startObjectParameters(prefix, "MarXbotColorMotor", this);
// 	params.createParameter(prefix, "onColor", m_onColor.name().replace("#", "+"));
// 	params.createParameter(prefix, "offColor", m_offColor.name().replace("#", "+"));
// 	params.createParameter(prefix, "onThreshold", QString::number(m_onThreshold));
// 	params.createParameter(prefix, "offThreshold", QString::number(m_offThreshold));
// }
//
// void MarXbotColorMotor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotMotor::describe(type);
//
// 	Descriptor d = addTypeDescription(type, "The MarXbot motor changing the robot color");
// 	d.describeString("onColor").def("+00FF00").help("The \"on\" color of the robot.", "This is a string. Its format can be: +RGB, +RRGGBB, +RRRGGGBBB, +RRRRGGGGBBBB (each of R, G, and B is a single hex digit) or a name from the list of colors defined in the list of SVG color keyword names provided by the World Wide Web Consortium (see http://www.w3.org/TR/SVG/types.html#ColorKeywords). The default value is \"+00FF00\"");
// 	d.describeString("offColor").def("+0000FF").help("The \"off\" color of the robot.", "This is a string. Its format can be: +RGB, +RRGGBB, +RRRGGGBBB, +RRRRGGGGBBBB (each of R, G, and B is a single hex digit) or a name from the list of colors defined in the list of SVG color keyword names provided by the World Wide Web Consortium (see http://www.w3.org/TR/SVG/types.html#ColorKeywords). The default value is \"+0000FF\"");
// 	d.describeReal("onThreshold").def(0.8f).limits(0.0f, 1.0f).help("The value above which the color is switched to the \"on\" color", "This is the threshold above which the color of the robot is set to the one defined by the onColor parameter. The default value is 0.8");
// 	d.describeReal("offThreshold").def(0.2f).limits(0.0f, 1.0f).help("The value below which the color is switched to the \"off\" color", "This is the threshold below which the color of the robot is set to the one defined by the offColor parameter. The default value is 0.2");
// }
//
// void MarXbotColorMotor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting the value of the output neuron
// 	m_neuronsIterator->setCurrentBlock(name());
// 	const real output = m_neuronsIterator->getOutput();
//
// 	// Setting the color of the robot
// 	if (output > m_onThreshold) {
// 		m_robot->setColor(m_onColor);
// 	} else if (output < m_offThreshold) {
// 		m_robot->setColor(m_offColor);
// 	}
// }
//
// int MarXbotColorMotor::size()
// {
// 	return 1;
// }
//
// void MarXbotColorMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotMotor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Setting color to the off color
// 		m_robot->setColor(m_offColor);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		m_neuronsIterator->setGraphicProperties("C", 0.0, 1.0, Qt::red);
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotAttachmentDeviceMotor::MarXbotAttachmentDeviceMotor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotMotor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_typeOfClosure(typeOfClosureFromString(ConfigurationHelper::getString(params, prefix + "typeOfClosure", "onlyClose"))),
// 	m_noMotionRange(ConfigurationHelper::getDouble(params, prefix + "noMotionRange", 0.3f)),
// 	m_noMotionRangeLow(0.5 - (m_noMotionRange / 2.0)),
// 	m_noMotionRangeUp(0.5 + (m_noMotionRange / 2.0))
// {
// 	if (m_typeOfClosure == Invalid) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "typeOfClosure", params.getValue(prefix + "typeOfClosure"), "The typeOfClosure value is not valid");
// 	}
// 	if ((m_noMotionRange < 0.0) || (m_noMotionRange > 1.0)) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "noMotionRange", params.getValue(prefix + "noMotionRange"), "The noMotionRange value must be between 0.0 and 1.0");
// 	}
// }
//
// MarXbotAttachmentDeviceMotor::~MarXbotAttachmentDeviceMotor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotAttachmentDeviceMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotMotor::save(params, prefix);
//
// 	// Saving our parameters
// 	params.startObjectParameters(prefix, "MarXbotAttachmentDeviceMotor", this);
// 	params.createParameter(prefix, "typeOfClosure", typeOfClosureToString(m_typeOfClosure));
// 	params.createParameter(prefix, "noMotionRange", QString::number(m_noMotionRange));
// }
//
// void MarXbotAttachmentDeviceMotor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotMotor::describe(type);
//
// 	Descriptor d = addTypeDescription(type, "The motor controlling the attachment device of the MarXbot");
// 	d.describeEnum("typeOfClosure").def("onlyClose").values(QStringList() << "onlyClose" << "onlyHalfClose" << "both").help("How the attachment device is closed", "this parameter controls how the attachement device is closed. Possible values are: \"onlyClose\", \"onlyHalfClose\" and \"both\" (default is \"onlyClose\")");
// 	d.describeReal("noMotionRange").def(0.3f).limits(0.0f, 1.0f).help("The dimension of the range corresponding to no movement", "This parameter defines how big is the range of activation which corresponds to no movement of the attachement device. If, for example, the value is 0.4, it means that activations from 0.3 to 0.7 cause no movement of the attachment device (default is 0.3)");
// }
//
// void MarXbotAttachmentDeviceMotor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting the value of the output neurons
// 	m_neuronsIterator->setCurrentBlock(name());
// 	const real outVel = m_neuronsIterator->getOutput();
// 	m_neuronsIterator->nextNeuron();
// 	const real outStatus = m_neuronsIterator->getOutput();
//
// 	// Getting the controller for the attachment device
// 	MarXbotAttachmentDeviceMotorController *const ctrl = m_robot->attachmentDeviceController();
//
// 	// First computing the actual velocity
// 	real velocity;
// 	if (outVel < m_noMotionRangeLow) {
// 		velocity = ((outVel - m_noMotionRangeLow) / m_noMotionRangeLow) * ctrl->getMaxVelocity();
// 	} else if (outVel > m_noMotionRangeUp) {
// 		velocity = ((outVel - m_noMotionRangeUp) / (1.0 - m_noMotionRangeUp)) * ctrl->getMaxVelocity();
// 	} else {
// 		velocity = 0;
// 	}
// 	ctrl->setDesiredVelocity(velocity);
//
// 	// Now computing the new desired status of the attachment device
// 	MarXbotAttachmentDeviceMotorController::Status status = MarXbotAttachmentDeviceMotorController::Open;
// 	switch (m_typeOfClosure) {
// 		case OnlyClose:
// 			if (outStatus > 0.5) {
// 				status = MarXbotAttachmentDeviceMotorController::Closed;
// 			}
// 			break;
// 		case OnlyHalfClose:
// 			if (outStatus > 0.5) {
// 				status = MarXbotAttachmentDeviceMotorController::HalfClosed;
// 			}
// 			break;
// 		case Both:
// 			if (outStatus > 0.66) {
// 				status = MarXbotAttachmentDeviceMotorController::Closed;
// 			} else if (outStatus > 0.33) {
// 				status = MarXbotAttachmentDeviceMotorController::HalfClosed;
// 			}
// 			break;
// 		default:
// 			// We should never get here
// 			abort();
// 			break;
// 	}
// 	ctrl->setDesiredStatus(status);
// }
//
// int MarXbotAttachmentDeviceMotor::size()
// {
// 	return 2;
// }
//
// void MarXbotAttachmentDeviceMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotMotor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Enabling the attachment device motor controller
// 		m_robot->enableAttachmentDevice(true);
// 		m_robot->attachmentDeviceController()->setEnabled(true);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		m_neuronsIterator->setGraphicProperties("AV", 0.0, 1.0, Qt::red);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setGraphicProperties("AC", 0.0, 1.0, Qt::red);
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotAttachmentDeviceMotor::TypeOfClosure MarXbotAttachmentDeviceMotor::typeOfClosureFromString(QString value)
// {
// 	value = value.toUpper();
//
// 	if (value == "ONLYCLOSE") {
// 		return OnlyClose;
// 	} else if (value == "ONLYHALFCLOSE") {
// 		return OnlyHalfClose;
// 	} else if (value == "BOTH") {
// 		return Both;
// 	} else {
// 		return Invalid;
// 	}
// }
//
// QString MarXbotAttachmentDeviceMotor::typeOfClosureToString(MarXbotAttachmentDeviceMotor::TypeOfClosure value)
// {
// 	switch(value) {
// 		case OnlyClose:
// 			return "OnlyClose";
// 		case OnlyHalfClose:
// 			return "OnlyHalfClose";
// 		case Both:
// 			return "Both";
// 		default:
// 			return "Invalid";
// 	}
// }
//
// }
