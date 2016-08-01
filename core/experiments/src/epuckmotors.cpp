// /********************************************************************************
//  *  SALSA Experiments Library                                                   *
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
// #include "epuckmotors.h"
// #include "configurationhelper.h"
// #include "randomgenerator.h"
// #include "logger.h"
//
// namespace salsa {
//
// EpuckMotor::EpuckMotor(ConfigurationParameters& params, QString prefix) :
// 	Motor(params, prefix),
// 	m_epuckResource("robot"),
// 	m_neuronsIteratorResource("neuronsIterator")
// {
// 	// Reading parameters
// 	m_epuckResource = actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "epuck", m_epuckResource));
// 	m_neuronsIteratorResource = actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "neuronsIterator", m_neuronsIteratorResource));
//
// 	// Declaring the resources that are needed here
// 	usableResources(QStringList() << m_epuckResource << m_neuronsIteratorResource);
// }
//
// EpuckMotor::~EpuckMotor()
// {
// 	// Nothing to do here
// }
//
// void EpuckMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	Motor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckMotor", this);
// 	params.createParameter(prefix, "epuck", m_epuckResource);
// 	params.createParameter(prefix, "neuronsIterator", m_neuronsIteratorResource);
// }
//
// void EpuckMotor::describe(QString type)
// {
// 	// Calling parent function
// 	Motor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The base class for e-puck motors");
// 	d.describeString("epuck").def("robot").help("The name of the resource associated with the e-puck robot to use (default is \"robot\")");
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("The name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// }
//
// void EpuckMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
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
// EpuckWheelVelocityMotor::EpuckWheelVelocityMotor(ConfigurationParameters& params, QString prefix) :
// 	EpuckMotor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL)
// {
// }
//
// EpuckWheelVelocityMotor::~EpuckWheelVelocityMotor()
// {
// 	// Nothing to do here
// }
//
// void EpuckWheelVelocityMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	EpuckMotor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckWheelVelocityMotor", this);
// }
//
// void EpuckWheelVelocityMotor::describe(QString type)
// {
// 	// Calling parent function
// 	EpuckMotor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The motor controlling the velocity of the wheels of the e-puck robot");
// }
//
// void EpuckWheelVelocityMotor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
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
// }
//
// int EpuckWheelVelocityMotor::size()
// {
// 	return 2;
// }
//
// void EpuckWheelVelocityMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	EpuckMotor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_epuckResource) {
// 		m_robot = getResource<PhyEpuck>();
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
// }
