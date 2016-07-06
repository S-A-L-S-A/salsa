// /********************************************************************************
//  *  FARSA Experimentes Library                                                  *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
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
// #ifndef EPUCKMOTORS_H
// #define EPUCKMOTORS_H
//
// #include "experimentsconfig.h"
// #include "neuroninterfaces.h"
// #include "robots.h"
// #include "motors.h"
//
// namespace farsa {
//
// /**
//  * \brief The base abstract class for e-puck motors
//  *
//  * This class simply has a couple of parameters that are common to all e-puck
//  * motors (see below).
//  *
//  * In addition to all parameters defined by the parent class (Motor), this
//  * class also defines the following parameters:
//  * - epuck: the name of the resource associated with the MarXbot robot to use
//  *          (default is "robot")
//  * - neuronsIterator: the name of the resource associated with the neural
//  *                    network iterator (default is "neuronsIterator")
//  *
//  * The resources required by this Motor are:
//  * - name defined by the epuck parameter: the simulated physical e-puck robot
//  * - name defined by the neuronsIterator parameter: the object to iterate over
//  *   neurons of the neural network
//  */
// class FARSA_EXPERIMENTS_API EpuckMotor : public Motor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * This also reads configuration parameters. Moreover this adds the
// 	 * e-puck and neuronsIterator resources to the list of resources used
// 	 * here
// 	 *
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 *               for the sensor
// 	 * \param prefix the path prefix to the paramters for this sensor
// 	 */
// 	EpuckMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckMotor();
//
// 	/**
// 	 * \brief Saves the parameters of the sensor into the
// 	 *        ConfigurationParameters object
// 	 *
// 	 * \param params the ConfigurationParameters where save the parameters
// 	 *               of the sensors
// 	 * \param prefix the path prefix for the parameters to save
// 	 */
// 	virtual void save(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Describes all the parameters for this sensor
// 	 *
// 	 * \param type the name with which this sensor is registered into the
// 	 *             factory
// 	 */
// 	static void describe(QString type);
//
// protected:
// 	/**
// 	 * \brief The function called when a resource used here is changed
// 	 *
// 	 * \param resourceName the name of the resource that has changed.
// 	 * \param chageType the type of change the resource has gone through
// 	 *                  (whether it was created, modified or deleted)
// 	 */
// 	virtual void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
// 	/**
// 	 * \brief The name of the resource associated with the e-puck robot
// 	 */
// 	QString m_epuckResource;
//
// 	/**
// 	 * \brief The name of th resource associated with the neural network
// 	 *        iterator
// 	 */
// 	QString m_neuronsIteratorResource;
// };
//
// /**
//  * \brief The motor controlling the velocity of the wheels of the e-puck
//  *
//  * The motor controlling the velocity of the wheels of the e-puck. This motor
//  * applies noise if requested.
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (EpuckMotor)
//  *
//  * The resources required by this Motor are the same as those of the parent
//  * class
//  */
// class FARSA_EXPERIMENTS_API EpuckWheelVelocityMotor : public EpuckMotor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the motor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	EpuckWheelVelocityMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckWheelVelocityMotor();
//
// 	/**
// 	 * \brief Saves current parameters into the given
// 	 *        ConfigurationParameters object
// 	 *
// 	 * \param params the ConfigurationParameters object in which parameters
// 	 *               should be saved
// 	 * \param prefix the name and path of the group where to save parametrs
// 	 */
// 	virtual void save(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Generates a description of this class and its parameters
// 	 *
// 	 * \param type the string representation of this class name
// 	 */
// 	static void describe(QString type);
//
// 	/**
// 	 * \brief Performs the motor update. This also modifies the activation
// 	 *        of input neurons
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of neurons required by this motor
// 	 *
// 	 * \return the number of neurons required by this motor
// 	 */
// 	virtual int size();
//
// private:
// 	/**
// 	 * \brief The function called when a resource used here is changed
// 	 *
// 	 * \param resourceName the name of the resource that has changed.
// 	 * \param chageType the type of change the resource has gone through
// 	 *                  (whether it was created, modified or deleted)
// 	 */
// 	virtual void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
// 	/**
// 	 * \brief The robot to use
// 	 */
// 	PhyEpuck* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };
//
// }
//
// #endif
