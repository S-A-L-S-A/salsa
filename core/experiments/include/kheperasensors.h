/********************************************************************************
 *  SALSA Experimentes Library                                                  *
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

#ifndef KHEPERASENSORS_H
#define KHEPERASENSORS_H

#include "experimentsconfig.h"
#include "controllerinputoutput.h"
#include "world.h"
#include "robots.h"
#include "phybox.h"
#include "phycylinder.h"
#include "sensors.h"

namespace salsa {

/**
 * \brief The base abstract class for khepera sensors
 *
 * This class simply has a parameter that is common to all khepera sensors (see
 * below). Some of the sensors work directly with Worldsim objects, while others
 * need an instance of the Arena to work. Those that do not need an Arena
 * generally also work with objects created using the Arena object, but in most
 * of the cases are slower. See the description of individual sensors for more
 * information about what each sensor needs.
 *
 * This class defines the following parameters:
 * - khepera: the name of the resource associated with the khepera robot to use
 *            (default is "robot")
 *
 * The resources required by this Sensor are:
 * - name defined by the khepera parameter: the simulated physical khepera robot
 */
class SALSA_EXPERIMENTS_API KheperaSensor : public AbstractControllerInput
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	KheperaSensor(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~KheperaSensor();

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

// /**
//  * \brief The infrared proximity sensors of the khepera
//  *
//  * The infrared proximity sensors of the khepera. This version of the infrared
//  * proximity sensors works with all Worldsim objects, but it is a bit slow. This
//  * sensor applies noise if requested
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (KheperaSensor)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API KheperaProximityIRSensor : public KheperaSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	KheperaProximityIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~KheperaProximityIRSensor();
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
// 	 * \brief Performs the sensor update. This also modifies the activation
// 	 *        of input neurons
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of neurons required by this sensor
// 	 *
// 	 * \return the number of neurons required by this sensor
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
// 	PhyKhepera* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };

/**
 * \brief The sampled proximity infrared sensors of the Khepera
 *
 * The sampled proximity infrared sensors of the Khepera. This version of the
 * infrared proximity sensors is based on data sampled from the real robot. It
 * is fast but only works with objects created using the Arena. This sensor
 * applies noise if requested
 *
 * In addition to all parameters defined by the parent class (KheperaSensor),
 * this class also defines the following parameters:
 * - activeSensors: Which IR sensors of the robot are actually enabled. This is
 *                  a string of exactly 8 elements. Each element can be either
 *                  "0" or "1" to respectively disable/enable the corresponding
 *                  proximity IR sensor. The first sensor is the one on the left
 *                  side of the robot and the others follow clockwise
 *                  (i.e. left, back, right, front)
 * - roundSamples: the file with samples for big round objects
 * - smallSamples: the file with samples for small round objects
 * - wallSamples: the file with samples for walls
 *
 * The resources required by this Sensor are the same as those of the parent
 * class plus:
 * - arena: the instance of the Arena object where the robot lives
 */
class SALSA_EXPERIMENTS_API KheperaSampledProximityIRSensor : public KheperaSensor
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	KheperaSampledProximityIRSensor(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~KheperaSampledProximityIRSensor();

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
	 * \brief Returns the number of neurons required by this sensor
	 *
	 * \return the number of neurons required by this sensor
	 */
	virtual int size() const;

private:
	virtual void iteratorChanged(AbstractControllerInputIterator* oldIt);
	virtual void updateCalled();
	virtual void resourceChanged(QString name, Component* owner, ResourceChangeType changeType);

	Khepera* m_robot;
	Arena* m_arena;
	const QVector<bool> m_activeSensors;
	const int m_numActiveSensors;
	const SampledIRDataLoader m_roundSamples;
	const SampledIRDataLoader m_smallSamples;
	const SampledIRDataLoader m_wallSamples;
};

// /**
//  * \brief The sampled light sensors of the Khepera
//  *
//  * The sampled proximity light sensors of the Khepera. This version of the light
//  * sensors is based on data sampled from the real robot. It is fast but only
//  * works with objects created using the Arena. This sensor applies noise if
//  * requested
//  *
//  * In addition to all parameters defined by the parent class (KheperaSensor),
//  * this class also defines the following parameters:
//  * - activeSensors: Which light sensors of the robot are actually enabled. This
//  *                  is a string of exactly 8 elements. Each element can be
//  *                  either "0" or "1" to respectively disable/enable the
//  *                  corresponding light sensor. The first sensor is the one on
//  *                  the left side of the robot and the others follow clockwise
//  *                  (i.e. left, back, right, front)
//  * - lightSamples: the file with samples for a 2W light bulb
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API KheperaSampledLightSensor : public KheperaSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	KheperaSampledLightSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~KheperaSampledLightSensor();
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
// 	 * \brief Performs the sensor update. This also modifies the activation
// 	 *        of input neurons
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of neurons required by this sensor
// 	 *
// 	 * \return the number of neurons required by this sensor
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
// 	Khepera* m_robot;
//
// 	/**
// 	 * \brief The arena with objects to take into account
// 	 */
// 	Arena* m_arena;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The vector storing which sensors are active
// 	 */
// 	const QVector<bool> m_activeSensors;
//
// 	/**
// 	 * \brief The number of active sensors
// 	 *
// 	 * This is also the number of neurons required by this sensor
// 	 */
// 	const int m_numActiveSensors;
//
// 	/**
// 	 * \brief The object with samples for light sensors
// 	 */
// 	const SampledIRDataLoader m_lightSamples;
//
// };
//
// /**
//  * \brief The ground sensor of the Khepera
//  *
//  * This sensors might be physically realized by monting an additional infrared
//  * sensors on the bottom side of the robot. Such sensor can approximately
//  * measure the grey level color of the ground. This sensor only works with
//  * objects in the Arena
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API KheperaGroundSensor : public KheperaSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	KheperaGroundSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~KheperaGroundSensor();
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
// 	 * \brief Performs the sensor update. This also modifies the activation
// 	 *        of input neurons
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of neurons required by this sensor
// 	 *
// 	 * \return the number of neurons required by this sensor
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
// 	Khepera* m_robot;
//
// 	/**
// 	 * \brief The arena with objects to take into account
// 	 */
// 	Arena* m_arena;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// };
//
//
// /**
//  * \brief The sensor reporting the actual velocity of the wheels of the Khepera
//  *
//  * This sensor reads the actual velocity of the wheels of the Khepera. It can
//  * work in two modalities: absolute (the default), meaning that the senors
//  * returns the current velocity of the wheels (scaled between -1 and 1) and
//  * delta, meaning that the sensor returns the absolute value of the difference
//  * between the desired velocity and the current velocity. This sensor provides
//  * two values, one for each wheel. This sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (KheperaSensor),
//  * this class also defines the following parameters:
//  * - mode: one of "Absolute" or "Delta" (see the general description for more
//  *         information). The default is "Absolute"
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API KheperaWheelSpeedsSensor : public KheperaSensor
// {
// public:
// 	/**
// 	 * \brief The enum of different modalities for this sensor
// 	 */
// 	enum Mode {
// 		AbsoluteMode, /**< The sensor returns the current velocity of
// 		                   the wheels */
// 		DeltaMode, /**< The sensor returns the absolute value of the
// 		                difference between the desired and actual
// 		                velocity of the wheels */
// 		UnknownMode
// 	};
//
// 	/**
// 	 * \brief Returns the string representation of the given modality
// 	 *
// 	 * \param mode the modality to convert to string
// 	 * \return the string representation of the modality
// 	 */
// 	static QString modeToString(Mode mode);
//
// 	/**
// 	 * \brief Converts the given string to a modality
// 	 *
// 	 * \param s the string to convert
// 	 * \return the modality for the string. Returns UnknownMode if the
// 	 *         string is not recognized
// 	 */
// 	static Mode stringToMode(QString mode);
//
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	KheperaWheelSpeedsSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~KheperaWheelSpeedsSensor();
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
// 	 * \brief Performs the sensor update. This also modifies the activation
// 	 *        of input neurons
// 	 */
// 	virtual void update();
//
// 	/**
// 	 * \brief Returns the number of neurons required by this sensor
// 	 *
// 	 * \return the number of neurons required by this sensor
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
// 	PhyKhepera* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The working modality
// 	 */
// 	const Mode m_mode;
// };

}

#endif
