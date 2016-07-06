/********************************************************************************
 *  FARSA Experimentes Library                                                  *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
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

#ifndef KHEPERAMOTORS_H
#define KHEPERAMOTORS_H

#include "experimentsconfig.h"
#include "controllerinputoutput.h"
#include "robots.h"
#include "motors.h"

namespace farsa {

/**
 * \brief The base abstract class for khepera motors
 *
 * This class simply has a parameter that is common to all khepera motors (see
 * below).
 *
 * This class defines the following parameters:
 * - khepera: the name of the resource associated with the khepera robot to use
 *            (default is "robot")
 *
 * The resources required by this Sensor are:
 * - name defined by the khepera parameter: the simulated physical khepera robot
 */
class FARSA_EXPERIMENTS_API KheperaMotor : public AbstractControllerOutput
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	KheperaMotor(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~KheperaMotor();

	/**
	 * \brief Describes all the parameters for this sensor
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

protected:
	/**
	 * \brief The name of the resource associated with the khepera robot
	 */
	QString m_kheperaResource;
};

/**
 * \brief The motor controlling the velocity of the wheels of the khepera
 *
 * The motor controlling the velocity of the wheels of the khepera. This motor
 * applies noise if requested.
 *
 * This class does not add any other parameter to those defined by the parent
 * class (KheperaMotor)
 *
 * The resources required by this Motor are the same as those of the parent
 * class
 */
class FARSA_EXPERIMENTS_API KheperaWheelVelocityMotor : public KheperaMotor
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	KheperaWheelVelocityMotor(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~KheperaWheelVelocityMotor();

	/**
	 * \brief Generates a description of this class and its parameters
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns the number of neurons required by this motor
	 *
	 * \return the number of neurons required by this motor
	 */
	virtual int size() const;

private:
	virtual void iteratorChanged(AbstractControllerOutputIterator* oldIt);
	virtual void updateCalled();
	virtual void resourceChanged(QString name, Component* owner, ResourceChangeType changeType);

	// The robot to use
	PhyKhepera* m_robot;
};

}

#endif
