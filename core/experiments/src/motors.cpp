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
// #include "motors.h"
// #include "configurationparameters.h"
// #include "configurationhelper.h"
// #include "randomgenerator.h"
// #include "motorcontrollers.h"
// #include "logger.h"
// #include <QStringList>
// #include <QMap>
//
// namespace salsa {
//
// FakeMotor::FakeMotor(ConfigurationParameters& params, QString prefix) :
// 	Motor(params, prefix),
// 	m_additionalOutputs(ConfigurationHelper::getUnsignedInt(params, prefix + "additionalOutputs", 1)),
// 	m_neuronsIteratorResource(actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "neuronsIterator", "neuronsIterator"))),
// 	m_additionalOutputsResource(actualResourceNameForMultirobot(ConfigurationHelper::getString(params, prefix + "additionalOutputsResource", "additionalOutputs"))),
// 	m_neuronsIterator(NULL)
// {
// 	usableResources(QStringList() << m_neuronsIteratorResource << m_additionalOutputsResource);
//
// 	for (unsigned int i = 0; i < m_additionalOutputs.size(); i++) {
// 		m_additionalOutputs[i] = 0.0;
// 	}
// }
//
// FakeMotor::~FakeMotor()
// {
// 	// Removing resources
// 	try {
// 		deleteResource(m_additionalOutputsResource);
// 	} catch (...) {
// 		// Doing nothing, this is here just to prevent throwing an exception from the destructor
// 	}
// }
//
// void FakeMotor::save(ConfigurationParameters& params, QString prefix)
// {
// 	Motor::save( params, prefix );
// 	params.startObjectParameters(prefix, "FakeMotor", this);
// 	params.createParameter(prefix, "additionalOutputs", QString::number(m_additionalOutputs.size()));
// 	params.createParameter(prefix, "neuronsIterator", m_neuronsIteratorResource);
// 	params.createParameter(prefix, "additionalOutputsResource", m_additionalOutputsResource);
// }
//
// void FakeMotor::describe(QString type)
// {
// 	Motor::describe(type);
//
// 	Descriptor d = addTypeDescription(type, "Adds output neurons that can be used for custom operations", "With this motor you can specify how many additional outputs are needed in the controller. This also declares a resource that can be used to access the additional outputs");
// 	d.describeInt("additionalOutputs").def(1).limits(1,100).props(IsMandatory).help("The number of additional outputs that will be added to the controller (default 1)");
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("The name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// 	d.describeString("additionalOutputsResource").def("additionalOutputs").help("The name of the resource associated with the vector of additional outputs (default is \"additionalOutputs\")");
// }
//
// void FakeMotor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	ResourcesLocker locker(this);
//
// 	// Copying the output inside the vector of additional outputs
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (unsigned int i = 0; i < m_additionalOutputs.size(); i++, m_neuronsIterator->nextNeuron()) {
// 		m_additionalOutputs[i] = m_neuronsIterator->getOutput();
// 	}
// }
//
// int FakeMotor::size()
// {
// 	return m_additionalOutputs.size();
// }
//
// void FakeMotor::shareResourcesWith(ResourcesUser* other)
// {
// 	// Calling parent function
// 	Motor::shareResourcesWith(other);
//
// 	// Now declaring our resource
// 	declareResource(m_additionalOutputsResource, &m_additionalOutputs);
// }
//
// void FakeMotor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	if (changeType == Deleted) {
// 		resetNeededResourcesCheck();
// 		return;
// 	}
//
// 	if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for(int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("Fk" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else if (resourceName != m_additionalOutputsResource) {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// ProportionalController::ProportionalController() :
// 	m_k(0.3),
// 	m_maxVelocity(20.0)
// {
// }
//
// ProportionalController::ProportionalController(double k, double maxVelocity) :
// 	m_k(k),
// 	m_maxVelocity(maxVelocity)
// {
// }
//
// ProportionalController::ProportionalController(const ProportionalController& other) :
// 	m_k(other.m_k),
// 	m_maxVelocity(other.m_maxVelocity)
// {
// }
//
// ProportionalController& ProportionalController::operator=(const ProportionalController& other)
// {
// 	if (this == &other) {
// 		return *this;
// 	}
//
// 	m_k = other.m_k;
// 	m_maxVelocity = other.m_maxVelocity;
//
// 	return *this;
// }
//
// ProportionalController::~ProportionalController()
// {
// 	// Nothing to do here
// }
//
// double ProportionalController::velocityForJoint(double desired, double current) const
// {
// 	double vel = m_k * (desired - current);
//
// 	if (vel > +m_maxVelocity) {
// 		vel = +m_maxVelocity;
// 	} else if (vel < -m_maxVelocity) {
// 		vel = -m_maxVelocity;
// 	}
//
// 	return vel;
// }
//
// } // end namespace salsa
//
