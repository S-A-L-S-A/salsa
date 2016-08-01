// /********************************************************************************
//  *  SALSA Experimentes Library                                                  *
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
// #ifndef MARXBOTMOTORS_H
// #define MARXBOTMOTORS_H
//
// #include "experimentsconfig.h"
// #include "neuroninterfaces.h"
// #include "robots.h"
// #include "motors.h"
// #include <QColor>
//
// namespace salsa {
//
// /**
//  * \brief The base abstract class for MarXbot motors
//  *
//  * This class simply has a couple of parameters that are common to all MarXbot
//  * motors (see below).
//  *
//  * In addition to all parameters defined by the parent class (Motor), this
//  * class also defines the following parameters:
//  * - marxbot: the name of the resource associated with the MarXbot robot to use
//  *            (default is "robot")
//  * - neuronsIterator: the name of the resource associated with the neural
//  *                    network iterator (default is "neuronsIterator")
//  *
//  * The resources required by this Motor are:
//  * - name defined by the marxbot parameter: the simulated physical MarXbot robot
//  * - name defined by the neuronsIterator parameter: the object to iterate over
//  *   neurons of the neural network
//  */
// class SALSA_EXPERIMENTS_API MarXbotMotor : public Motor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * This also reads configuration parameters. Moreover this adds the
// 	 * MarXbot and neuronsIterator resources to the list of resources used
// 	 * here
// 	 *
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 *               for the sensor
// 	 * \param prefix the path prefix to the paramters for this sensor
// 	 */
// 	MarXbotMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotMotor();
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
// 	 * \brief The name of the resource associated with the MarXbot robot
// 	 */
// 	QString m_marxbotResource;
//
// 	/**
// 	 * \brief The name of th resource associated with the neural network
// 	 *        iterator
// 	 */
// 	QString m_neuronsIteratorResource;
// };
//
// /**
//  * \brief The motor controlling the velocity of the wheels of the MarXbot
//  *
//  * The motor controlling the velocity of the wheels of the MarXbot. This motor
//  * applies noise if requested.
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (MarXbotMotor)
//  *
//  * The resources required by this Motor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotWheelVelocityMotor : public MarXbotMotor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the motor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotWheelVelocityMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotWheelVelocityMotor();
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
// 	 * \brief Performs the motor update
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
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };
//
// /**
//  * \brief The motor controlling the velocity of the wheels of the MarXbot
//  *
//  * The motor controlling the velocity of the wheels of the MarXbot
//  * The state of the motor neurons is post-processed so that the first
//  * control the speed and the second the direction of movement. This motor
//  * applies noise if requested.
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (MarXbotMotor)
//  *
//  * The resources required by this Motor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotWheelVelOrient : public MarXbotMotor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the motor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotWheelVelOrient(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotWheelVelOrient();
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
// 	 * \brief Performs the motor update
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
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };
//
// /**
//  * \brief The motor controlling the color of the MarXbot
//  *
//  * This motor controls the color of a MarXbot robot. The robot can have one of
//  * two colors, an "on" color and an "off" color. When the output is above a
//  * certain threshold, the color is changed to the "on" color, when it is below a
//  * certain threshold, the color is changed to the "off" color. The two
//  * thresholds can have different values. This motor does not apply noise
//  *
//  * In addition to all parameters defined by the parent class (MarXbotMotor),
//  * this class also defines the following parameters:
//  * - onColor: the "on" color. This is a string (see QColor::setNamedColor() for
//  *            a description of possible formats). The default is "+00FF00"
//  * - offColor: the "off" color. This is a string (see QColor::setNamedColor()
//  *             for a description of possible formats). The default is "+0000FF"
//  * - onThreshold: the value above which the color is switched to the "on" color.
//  *                The default value is 0.8
//  * - offThreshold: the value below which the color is switched to the "off"
//  *                 color. The default value is 0.2
//  * We have to use + in place of # for colors because # is the comment in .ini
//  * files...
//  *
//  * The resources required by this Motor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotColorMotor : public MarXbotMotor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the motor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotColorMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotColorMotor();
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
// 	 * \brief Performs the motor update
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
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The "on" color
// 	 */
// 	const QColor m_onColor;
//
// 	/**
// 	 * \brief The "off" color
// 	 */
// 	const QColor m_offColor;
//
// 	/**
// 	 * \brief The value above which the color is switched to the "on" color
// 	 */
// 	const real m_onThreshold;
//
// 	/**
// 	 * \brief The value below which the color is switched to the "off" color
// 	 */
// 	const real m_offThreshold;
// };
//
// /**
//  * \brief The motor controlling the attachment device of the MarXbot
//  *
//  * The motor controlling the attachment device of the MarXbot. This motor uses
//  * two outputs: one controls the movement of the attachment device, the other
//  * its status. The movement is relative: values above a upper threshold move the
//  * attachment device counterclockwise, those below a lower threshold move the
//  * attachment device clockwise and those between the two thresholds do not move
//  * the attachment device. The second output controls the status of the
//  * attachment device. The possible status depend on the typeOfClosure parameter:
//  * if it is "onlyClose", status is either Open (for values below 0.5) or Closed
//  * (for values above 0.5); if it is "onlyHalfClose", status is either Open (for
//  * values below 0.5) or HalfClosed (for values above 0.5); if it is "both",
//  * status can be Open (for values below 0.33), HalfClosed (for values between
//  * 0.33 and 0.66) or Closed (for values above 0.66). This motor does not apply
//  * noise
//  *
//  * In addition to all parameters defined by the parent class (MarXbotMotor),
//  * this class also defines the following parameters:
//  * - typeOfClosure: this parameter controls how the attachement device is
//  *                  closed. Possible values are: "onlyClose", "onlyHalfClose",
//  *                  "both" (default is "onlyClose")
//  * - noMotionRange: this parameter defines how big is the range of activation
//  *                  which corresponds to no movement of the attachement device
//  *                  (default is 0.3)
//  *
//  * The resources required by this Motor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotAttachmentDeviceMotor : public MarXbotMotor
// {
// private:
// 	/**
// 	 * \brief The possible type of closures
// 	 *
// 	 * Values correspond to possible values of the typeOfClosure parameter
// 	 */
// 	enum TypeOfClosure {
// 		OnlyClose,
// 		OnlyHalfClose,
// 		Both,
// 		Invalid ///< For invalid values of the parameter
// 	};
//
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the motor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotAttachmentDeviceMotor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotAttachmentDeviceMotor();
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
// 	 * \brief Performs the motor update
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
// 	 * \brief Converts a string to the corresponding TypeOfClosure value
// 	 *
// 	 * Invalid is returned if the string is not recognized as anything else
// 	 * \return the TypeOfClosure represented by the string
// 	 */
// 	static TypeOfClosure typeOfClosureFromString(QString value);
//
// 	/**
// 	 * \brief Converts a TypeOfClosure value to string
// 	 *
// 	 * \return the string representing the TypeOfClosure value
// 	 */
// 	static QString typeOfClosureToString(TypeOfClosure value);
//
// 	/**
// 	 * \brief The robot to use
// 	 */
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The type of closure
// 	 */
// 	const TypeOfClosure m_typeOfClosure;
//
// 	/**
// 	 * \brief How big is the range of activation which corresponds to no
// 	 *        movement of the attachement device
// 	 */
// 	const real m_noMotionRange;
//
// 	/**
// 	 * \brief The lower bound of the range of no movement
// 	 *
// 	 * This is computed from the value of m_noMotionRange
// 	 */
// 	const real m_noMotionRangeLow;
//
// 	/**
// 	 * \brief The upper bound of the range of no movement
// 	 *
// 	 * This is computed from the value of m_noMotionRange
// 	 */
// 	const real m_noMotionRangeUp;
// };
//
// }
//
// #endif
