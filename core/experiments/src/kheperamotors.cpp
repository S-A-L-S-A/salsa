/********************************************************************************
 *  SALSA Experiments Library                                                   *
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

#include "kheperamotors.h"
#include "configurationhelper.h"
#include "randomgenerator.h"
#include "logger.h"

namespace salsa {

KheperaMotor::KheperaMotor(ConfigurationManager& params)
	: AbstractControllerOutput(params)
	, m_kheperaResource(ConfigurationHelper::getString(configurationManager(), confPath() + "khepera"))
{
	// Declaring the resources that are needed here
	addNotifiedResource(m_kheperaResource);
}

KheperaMotor::~KheperaMotor()
{
}

void KheperaMotor::describe(RegisteredComponentDescriptor& d)
{
	AbstractControllerOutput::describe(d);

	d.help("The base class for Khepera motors");

	d.describeString("khepera").def("robot").help("The name of the resource associated with the khepera robot to use (default is \"robot\")");
}

KheperaWheelVelocityMotor::KheperaWheelVelocityMotor(ConfigurationManager& params)
	: KheperaMotor(params)
	, m_robot(NULL)
{
}

KheperaWheelVelocityMotor::~KheperaWheelVelocityMotor()
{
}

void KheperaWheelVelocityMotor::describe(RegisteredComponentDescriptor& d)
{
	// Calling parent function
	KheperaMotor::describe(d);

	// Describing our parameters
	d.help("The motor controlling the velocity of the wheels of the khepera robot");
}

int KheperaWheelVelocityMotor::size() const
{
	return 2;
}

void KheperaWheelVelocityMotor::iteratorChanged(AbstractControllerOutputIterator*)
{
	for (int i = 0; i < size(); i++, it()->next()) {
		it()->setProperties("W" + QString::number(i), 0.0, 1.0, Qt::red);
	}
}

void KheperaWheelVelocityMotor::updateCalled()
{
	// Getting limit velocities for wheels
	double minSpeed1;
	double minSpeed2;
	double maxSpeed1;
	double maxSpeed2;
	m_robot->wheelsController()->getSpeedLimits(minSpeed1, minSpeed2, maxSpeed1, maxSpeed2);

	// Computing desired wheel velocities
	const double v1 = (maxSpeed1 - minSpeed1) * it()->getOutput() + minSpeed1;
	it()->next();
	const double v2 = (maxSpeed2 - minSpeed2) * it()->getOutput() + minSpeed2;

	m_robot->wheelsController()->setSpeeds(v1, v2);
}

void KheperaWheelVelocityMotor::resourceChanged(QString name, Component*, ResourceChangeType changeType)
{
	if (name != m_kheperaResource) {
		Logger::info("Unknown resource " + name + " for KheperaWheelVelocityMotor in configuration path \"" + confPath() + "\"");

		return;
	}

	if ((changeType == ResourceDeleted) && (changeType == ResourceDeclaredAsNull)) {
		m_robot = NULL;
	} else {
		m_robot = getResource<PhyKhepera>();
	}
}

}
