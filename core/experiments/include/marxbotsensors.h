// /********************************************************************************
//  *  SALSA Experimentes Library                                                  *
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
// #ifndef MARXBOTSENSORS_H
// #define MARXBOTSENSORS_H
//
// #include "experimentsconfig.h"
// #include "neuroninterfaces.h"
// #include "world.h"
// #include "robots.h"
// #include "phybox.h"
// #include "phycylinder.h"
// #include "sensors.h"
// #include "sensorcontrollers.h"
//
// namespace salsa {
//
// /**
//  * \brief The base abstract class for MarXbot sensors
//  *
//  * This class simply has a couple of parameters that are common to all MarXbot
//  * sensors (see below). Some of the sensors work directly with Worldsim objects,
//  * while others need an instance of the Arena to work. Those that do not need an
//  * Arena generally also work with objects created using the Arena object, but in
//  * most of the cases are slower. See the description of individual sensors for
//  * more information about what each sensor needs.
//  *
//  * In addition to all parameters defined by the parent class (Sensor), this
//  * class also defines the following parameters:
//  * - marxbot: the name of the resource associated with the MarXbot robot to use
//  *            (default is "robot")
//  * - neuronsIterator: the name of the resource associated with the neural
//  *                    network iterator (default is "neuronsIterator")
//  *
//  * The resources required by this Sensor are:
//  * - name defined by the marxbot parameter: the simulated physical MarXbot robot
//  * - name defined by the neuronsIterator parameter: the object to iterate over
//  *   neurons of the neural network
//  */
// class SALSA_EXPERIMENTS_API MarXbotSensor : public Sensor
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
// 	MarXbotSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotSensor();
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
//  * \brief The infrared proximity sensors of the MarXbot
//  *
//  * The infrared proximity sensors of the MarXbot. This version of the infrared
//  * proximity sensors works with all Worldsim objects, but it is a bit slow. This
//  * sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbot), this
//  * class also defines the following parameters:
//  * - activeSensors: Which IR sensors of the robot are actually enabled. This is
//  *                  a string of exactly 24 elements. Each element can be either
//  *                  "0" or "1" to respectively disable/enable the corresponding
//  *                  proximity IR sensor. The first sensor is the one on the left
//  *                  side of the robot and the others follow counterclockwise
//  *                  (i.e. left, back, right, front)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotProximityIRSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotProximityIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotProximityIRSensor();
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
// 	PhyMarXbot* m_robot;
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
// };
//
// /**
//  * \brief The infrared ground bottom sensors of the MarXbot
//  *
//  * The infrared ground sensors below the battery pack of the MarXbot. This only
//  * works with objects in the arena. This sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbot), this
//  * class also defines the following parameters:
//  * 	- invertActivation: if true the sensor is activated with 0.0 above white
//  * 	                    and with 1.0 above black, if false the opposite
//  * 	                    holds. The default value is false
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API MarXbotGroundBottomIRSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotGroundBottomIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotGroundBottomIRSensor();
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
// 	bool invertActivation() const
// 	{
// 		return m_invertActivation;
// 	}
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
// 	 * \brief The arena with objects to take into account
// 	 */
// 	Arena* m_arena;
//
// 	/**
// 	 * \brief If true inverts sensor activation
// 	 */
// 	const bool m_invertActivation;
// };
//
// /**
//  * \brief The infrared ground around sensors of the MarXbot
//  *
//  * The infrared ground sensors below the base of the robot (just above the
//  * wheels). This only works with objects in the arena. This sensor applies noise
//  * if requested
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (MarXbotSensor)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API MarXbotGroundAroundIRSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotGroundAroundIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotGroundAroundIRSensor();
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
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The arena with objects to take into account
// 	 */
// 	Arena* m_arena;
// };
//
// /**
//  * \brief The linear camera sensor of the MarXbot
//  *
//  * This is a linear camera with configurable aperture (using the aperture
//  * parameter). The number of photoreceptors can be configured using the
//  * numReceptors parameter. Each receptor returns three values, one for each of
//  * the three colors (red, green, blue). You can decide whether to use all three
//  * colors or only part of them. This means that the size returned by this sensor
//  * is numReceptors * \<num colors used\>. The inputs for the controller are
//  * returned in the following order: first the values of all the red receptors,
//  * then the values for all the green receptors, finally the values for all the
//  * blue receptors.
//  * This sensor only works with objects created using the Arena instance. This
//  * sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - numReceptors: the number of receptors of the sensor. Each receptor returns
//  *                 three values, one for each of the three colors (red, green,
//  *                 blue), but you can decide to ignore some colors (see the
//  *                 "useRedChannel", "useGreenChannel" and "useBlueChannel"
//  *                 parameters). This means that the size returned by this sensor
//  *                 is numReceptors * \<num colors used\> (default is 8)
//  * - useRedChannel: whether the red component of the perceived objects should be
//  *                  used or not (default true)
//  * - useGreenChannel: whether the green component of the perceived objects
//  *                    should be used or not (default true)
//  * - useBlueChannel: whether the blue component of the perceived objects should
//  *                  be used or not (default true)
//  * - aperture: the aperture of the camera in degrees. The real MarXbot has an
//  *             omnidirectional camera, so you can use here any value up to 360°
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API MarXbotLinearCameraSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotLinearCameraSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotLinearCameraSensor();
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
// 	/**
// 	 * \brief Returns a const pointer to the inner Linear camera object
// 	 *
// 	 * \return a const pointer to the inner Linear camera object
// 	 */
// 	const LinearCamera* getLinearCamera() const
// 	{
// 		return m_camera;
// 	}
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
// 	 * \brief The number of receptors of the sensor
// 	 *
// 	 * Each receptor returns at most three values, on for each of the three
// 	 * colors (red, green, blue). This means that the size returned by this
// 	 * sensor is m_numReceptors * \<num colors used\>
// 	 */
// 	const int m_numReceptors;
//
// 	/**
// 	 * \brief Whether the red component of the perceived objects should be
// 	 *        used or not
// 	 */
// 	const bool m_useRedChannel;
//
// 	/**
// 	 * \brief Whether the green component of the perceived objects should be
// 	 *        used or not
// 	 */
// 	const bool m_useGreenChannel;
//
// 	/**
// 	 * \brief Whether the blue component of the perceived objects should be
// 	 *        used or not
// 	 */
// 	const bool m_useBlueChannel;
//
// 	/**
// 	 * \brief The number of used color channels
// 	 */
// 	const int m_usedColorChannels;
//
// 	/**
// 	 * \brief The aperture of the camera in degrees
// 	 *
// 	 * The real MarXbot has an omnidirectional camera, so you can use here
// 	 * any value up to 360°
// 	 */
// 	const real m_aperture;
//
// 	/**
// 	 * \brief The distance above which objects are not seen by the camera in
// 	 *        meters
// 	 */
// 	const real m_maxDistance;
//
// 	/**
// 	 * \brief The linear camera
// 	 */
// 	LinearCamera* m_camera;
//
// 	/**
// 	 * \brief True if we have to draw the linear camera
// 	 */
// 	const bool m_drawCamera;
// };
//
// /**
//  * \brief The linear camera sensor of the MarXbot (new implementation)
//  *
//  * This is a linear camera with configurable aperture (using the aperture
//  * parameter). The number of photoreceptors can be configured using the
//  * numReceptors parameter. An alternative way of specifying the number of
//  * receptors and their aperture is using the receptorsRanges parameter (see
//  * description). Note that if you set receptorsRanges you should not set the
//  * aperture or the numReceptors parameters and vice versa (an exception is
//  * thrown). Each receptor returns three values, one for each of the three colors
//  * (red, green, blue). You can decide whether to use all three colors or only
//  * part of them. This means that the size returned by this sensor is
//  * numReceptors * \<num colors used\>. The inputs for the controller are
//  * returned in the following order: first the values of all the red receptors,
//  * then the values for all the green receptors, finally the values for all the
//  * blue receptors.
//  * This sensor only works with objects created using the Arena instance. This
//  * sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - numReceptors: the number of receptors of the sensor. Each receptor returns
//  *                 three values, one for each of the three colors (red, green,
//  *                 blue), but you can decide to ignore some colors (see the
//  *                 "useRedChannel", "useGreenChannel" and "useBlueChannel"
//  *                 parameters). This means that the size returned by this sensor
//  *                 is numReceptors * \<num colors used\> (default is 8)
//  * - aperture: the aperture of the camera in degrees. The real MarXbot has an
//  *             omnidirectional camera, so you can use here any value up to 360°
//  * - maxDistance: the distance above which objects are not seen by the camera in
//  *                meters
//  * - receptorsRanges: the range for each receptor. This parameter is a comma
//  *                    separated list of ranges (in the form [start, end]), each
//  *                    range being the aperture of a single receptor. The number
//  *                    of receptors is equal to the number of ranges. The start
//  *                    and end angles of the range are in degrees. Note that if
//  *                    you specify this parameter you must not specify the
//  *                    aperture neither the numReceptors parameters.
//  * - useRedChannel: whether the red component of the perceived objects should be
//  *                  used or not (default true)
//  * - useGreenChannel: whether the green component of the perceived objects
//  *                    should be used or not (default true)
//  * - useBlueChannel: whether the blue component of the perceived objects should
//  *                  be used or not (default true)
//  * - drawCamera: whether to draw the camera or not
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API MarXbotLinearCameraSensorNew : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotLinearCameraSensorNew(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotLinearCameraSensorNew();
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
// 	/**
// 	 * \brief Returns a const pointer to the inner Linear camera object
// 	 *
// 	 * \return a const pointer to the inner Linear camera object
// 	 */
// 	const LinearCameraNew::LinearCamera* getLinearCamera() const
// 	{
// 		return m_camera;
// 	}
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
// 	 * \brief The number of receptors of the sensor
// 	 *
// 	 * Each receptor returns at most three values, on for each of the three
// 	 * colors (red, green, blue). This means that the size returned by this
// 	 * sensor is m_numReceptors * \<num colors used\>
// 	 * \note This cannot be const because we have to change it in the
// 	 *       constructor
// 	 */
// 	int m_numReceptors;
//
// 	/**
// 	 * \brief Whether the red component of the perceived objects should be
// 	 *        used or not
// 	 */
// 	const bool m_useRedChannel;
//
// 	/**
// 	 * \brief Whether the green component of the perceived objects should be
// 	 *        used or not
// 	 */
// 	const bool m_useGreenChannel;
//
// 	/**
// 	 * \brief Whether the blue component of the perceived objects should be
// 	 *        used or not
// 	 */
// 	const bool m_useBlueChannel;
//
// 	/**
// 	 * \brief The number of used color channels
// 	 */
// 	const int m_usedColorChannels;
//
// 	/**
// 	 * \brief The aperture of the camera in degrees
// 	 *
// 	 * The real MarXbot has an omnidirectional camera, so you can use here
// 	 * any value up to 360°
// 	 */
// 	const real m_aperture;
//
// 	/**
// 	 * \brief The distance above which objects are not seen by the camera in
// 	 *        meters
// 	 */
// 	const real m_maxDistance;
//
// 	/**
// 	 * \brief The range of receptors
// 	 */
// 	QVector<SimpleInterval> m_receptorsRanges;
//
// 	/**
// 	 * \brief The linear camera
// 	 */
// 	LinearCameraNew::LinearCamera* m_camera;
//
// 	/**
// 	 * \brief True if we have to draw the linear camera
// 	 */
// 	const bool m_drawCamera;
// };
//
// namespace __MarXbotTractionSensor_internal {
// 	class TractionSensorGraphic;
// }
//
// /**
//  * \brief The traction sensors of the MarXbot
//  *
//  * The traction sensor of the MarXbot. This only works when in dynamic mode. The
//  * force is only sensed on the plane (x and y axes). This sensor has 4 neurons:
//  * one for positive force along the x axis, one for negative force along the x
//  * axis, one for positive force along the y axis and one for negative force
//  * along the y axis. This sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - maxForce: the value of the force on one axis over which the corresponding
//  *             neuron is activated with 1
//  * - minForce: the value of the force on one axis under which the corresponding
//  *             neuron is activated with 0
//  * - drawSensor: if true the sensor is drawn with an arrow indicating the
//  *               current direction of the force
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotTractionSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotTractionSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotTractionSensor();
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
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The value of the force on one axis over which the
// 	 *        corresponding neuron is activated with 1
// 	 */
// 	const real m_maxForce;
//
// 	/**
// 	 * \brief The value of the force on one axis under which the
// 	 *        corresponding neuron is activated with 0
// 	 */
// 	const real m_minForce;
//
// 	/**
// 	 * \brief If true the sensor is drawn with an arrow indicating the
// 	 *        current direction of the force
// 	 */
// 	const bool m_drawSensor;
//
// 	/**
// 	 * \brief The graphical representation of this sensor
// 	 */
// 	__MarXbotTractionSensor_internal::TractionSensorGraphic* m_graphics;
// };
//
// /**
//  * \brief The sampled proximity infrared sensors of the MarXbot
//  *
//  * The sampled proximity infrared sensors of the MarXbot. This version of the
//  * infrared proximity sensors is based on data sampled from the real robot. It
//  * is fast but only works with objects created using the Arena. This sensor
//  * applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - activeSensors: Which IR sensors of the robot are actually enabled. This is
//  *                  a string of exactly 24 elements. Each element can be either
//  *                  "0" or "1" to respectively disable/enable the corresponding
//  *                  proximity IR sensor. The first sensor is the one on the left
//  *                  side of the robot and the others follow counterclockwise
//  *                  (i.e. left, back, right, front). If averageNum is not zero,
//  *                  this parameter is ignored.
//  * - averageNum: how many sensors should be averaged. This can only be 0, 2, 3,
//  *               4 or 6. If 2, for each couple of consecutive sensors the
//  *               average is returned, if 3 for each triplet of consecutive
//  *               sensors the average is returned and so on. If this is zero,
//  *               no average is computed and the activeSensors parameter is used.
//  *               The default value is 0
//  * - roundSamples: the file with samples for big round objects
//  * - smallSamples: the file with samples for small round objects
//  * - wallSamples: the file with samples for walls
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API MarXbotSampledProximityIRSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotSampledProximityIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotSampledProximityIRSensor();
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
// 	const SampledIRDataLoader& getRoundSamples() const
// 	{
// 		return m_roundSamples;
// 	}
//
// 	const SampledIRDataLoader& getSmallSamples() const
// 	{
// 		return m_smallSamples;
// 	}
//
// 	const QVector<bool>& getActiveSensors() const
// 	{
// 		return m_activeSensors;
// 	}
//
// 	int getNumActiveSensors() const
// 	{
// 		return m_numActiveSensors;
// 	}
//
// 	int getAverageNum() const
// 	{
// 		return m_averageNum;
// 	}
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
// 	MarXbot* m_robot;
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
// 	 * \brief How many sensors should be averaged
// 	 *
// 	 * This can only be 0, 2, 3, 4 or 6
// 	 */
// 	const int m_averageNum;
//
// 	/**
// 	 * \brief The number of active sensors
// 	 *
// 	 * This is also the number of neurons required by this sensor
// 	 */
// 	const int m_numActiveSensors;
//
// 	/**
// 	 * \brief The object with samples for big round objects
// 	 */
// 	const SampledIRDataLoader m_roundSamples;
//
// 	/**
// 	 * \brief The object with samples for small round objects
// 	 */
// 	const SampledIRDataLoader m_smallSamples;
//
// 	/**
// 	 * \brief The object with samples for walls
// 	 */
// 	const SampledIRDataLoader m_wallSamples;
// };
//
// /**
//  * \brief The sensor providing the attachment device proprioception
//  *
//  * This sensor provides information about the current status of the attachment
//  * device. It provides several inputs (which can be disabled using the
//  * parameters described below):
//  * - the sin() of the current position of the attachment device (scaled between
//  *   0.0 and 1.0)
//  * - the cos() of the current position of the attachment device (scaled between
//  *   0.0 and 1.0)
//  * - the current status of the attachment device (0 means open, 0.5 half-closed
//  *   and 1.0 closed)
//  * - whether this robot is attached to another robot or not (i.e. the
//  *   attachement device of this robot is attached to another robot)
//  * - whether another robot is attached to this robot or not (i.e. the
//  *   attachement device of the other robot is attached to this robot)
//  * The position of the gripper is provided using two neurons (the sin() and the
//  * cos() of the angle) to alleviate the problem of 0° and 360° being the same
//  * angle with opposite activations. This sensor never applies noise
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - enablePosition: boolean value defining whether inputs with the position of
//  *                   the attachment device should be enabled or not (default:
//  *                   true)
//  * - enableStatus: boolean value defining whether the input with the status of
//  *                 the attachment device should be enabled or not (default:
//  *                 true)
//  * - enableAttached: boolean value defining whether the input telling if this
//  *                   robot is attached to another robot should be enabled or not
//  *                   (default: true)
//  * - enableOtherAttached: boolean value defining whether the input telling if
//  *                        another robot is attached to this robot should be
//  *                        enabled or not (default: true)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class.
//  */
// class SALSA_EXPERIMENTS_API MarXbotAttachmentDeviceSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotAttachmentDeviceSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotAttachmentDeviceSensor();
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
// 	MarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief Whether inputs with the position of the attachment device
// 	 *        should be enabled or not
// 	 */
// 	const bool m_enablePosition;
//
// 	/**
// 	 * \brief Whether the input with the status of the attachment device
// 	 *        should be enabled or not
// 	 */
// 	const bool m_enableStatus;
//
// 	/**
// 	 * \brief Whether the input telling if this robot is attached to another
// 	 *        robot should be enabled or not
// 	 */
// 	const bool m_enableAttached;
//
// 	/**
// 	 * \brief Whether the input telling if another robot is attached to this
// 	 *        robot should be enabled or not
// 	 */
// 	const bool m_enableOtherAttached;
//
// 	/**
// 	 * \brief The number inputs (i.e. the number of required sensors)
// 	 *
// 	 * This depends on which inputs are enabled
// 	 */
// 	const int m_numInputs;
// };
//
// /**
//  * \brief The sensor reporting the actual velocity of the wheels of the MarXbot
//  *
//  * This sensor reads the actual velocity of the wheels of the MarXbot. It can
//  * work in two modalities: absolute (the default), meaning that the senors
//  * returns the current velocity of the wheels (scaled between -1 and 1) and
//  * delta, meaning that the sensor returns the absolute value of the difference
//  * between the desired velocity and the current velocity. This sensor provides
//  * two values, one for each wheel. This sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - mode: one of "Absolute" or "Delta" (see the general description for more
//  *         information). The default is "Absolute"
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotWheelSpeedsSensor : public MarXbotSensor
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
// 	MarXbotWheelSpeedsSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotWheelSpeedsSensor();
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
// 	PhyMarXbot* m_robot;
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
//
// /**
//  * \brief A frontal distance sensor for long distances
//  *
//  * This is a simple distance sensor in the frontal part of the robot with three
//  * rays, at -a/2, 0° a/2 with respect to the frontal part of the robot where a
//  * is the aperture (configurable with a parameter). This could be implemented on
//  * the real robot using the laser scanner.
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - maxDistance: the maximum distance for the sensor (default is 1.0)
//  * - aperture: the aperture of the sensor in degrees
//  * - drawSensors: whether to draw the sensors or not
//  * - drawRays: when drawing the sensor, whether to draw the rays or not
//  * - drawRaysRange: when drawing the rays, whether to draw the real range or not
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotLaserFrontDistanceSensor : public MarXbotSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	MarXbotLaserFrontDistanceSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbotLaserFrontDistanceSensor();
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
// 	PhyMarXbot* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The low level left sensor
// 	 */
// 	SingleIR* m_sensorLeft;
//
// 	/**
// 	 * \brief The low level central sensor
// 	 */
// 	SingleIR* m_sensorCenter;
//
// 	/**
// 	 * \brief The low level right sensor
// 	 */
// 	SingleIR* m_sensorRight;
//
//
// 	/**
// 	 * \brief The maximum distance of sensors
// 	 */
// 	const real m_maxDistance;
//
// 	/**
// 	 * \brief The aperture of the sensor in degrees
// 	 */
//     real m_aperture;
//
// 	/**
// 	 * \brief Whether to draw the sensors or not
// 	 */
// 	const bool m_drawSensors;
//
// 	/**
// 	 * \brief When drawing the sensor, whether to draw the rays or not
// 	 */
// 	const bool m_drawRays;
//
// 	/**
// 	 * \brief When drawing the rays, whether to draw the real range or not
// 	 */
//     const bool m_drawRaysRange;
//
// };
//
//
// /**
//  * \brief An Omnidirectional distance sensor for long distances
//  *
//  * This is a simple distance sensor in the frontal and rear part of the robot with six
//  * rays, each 60°. This could be implemented on * the real robot using the laser scanner.
//  *
//  * In addition to all parameters defined by the parent class (MarXbotSensor),
//  * this class also defines the following parameters:
//  * - maxDistance: the maximum distance for the sensor (default is 1.0)
//  * - drawSensors: whether to draw the sensors or not
//  * - drawRays: when drawing the sensor, whether to draw the rays or not
//  * - drawRaysRange: when drawing the rays, whether to draw the real range or not
//  * - averageSensors: whether the sensors should be averaged and used as an unique input
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API MarXbotLaserOmniDistanceSensor : public MarXbotSensor
// {
// public:
//     /**
//      * \brief Constructor
//      *
//      * Creates and configures the sensor
//      * \param params the ConfigurationParameters containing the parameters
//      * \param prefix the path prefix to the paramters for this Sensor
//      */
//     MarXbotLaserOmniDistanceSensor(ConfigurationParameters& params, QString prefix);
//
//     /**
//      * \brief Destructor
//      */
//     virtual ~MarXbotLaserOmniDistanceSensor();
//
//     /**
//      * \brief Saves current parameters into the given
//      *        ConfigurationParameters object
//      *
//      * \param params the ConfigurationParameters object in which parameters
//      *               should be saved
//      * \param prefix the name and path of the group where to save parametrs
//      */
//     virtual void save(ConfigurationParameters& params, QString prefix);
//
//     /**
//      * \brief Generates a description of this class and its parameters
//      *
//      * \param type the string representation of this class name
//      */
//     static void describe(QString type);
//
//     /**
//      * \brief Performs the sensor update. This also modifies the activation
//      *        of input neurons
//      */
//     virtual void update();
//
//     /**
//      * \brief Returns the number of neurons required by this sensor
//      *
//      * \return the number of neurons required by this sensor
//      */
//     virtual int size();
//
// private:
//     /**
//      * \brief The function called when a resource used here is changed
//      *
//      * \param resourceName the name of the resource that has changed.
//      * \param chageType the type of change the resource has gone through
//      *                  (whether it was created, modified or deleted)
//      */
//     virtual void resourceChanged(QString resourceName, ResourceChangeType changeType);
//
//     /**
//      * \brief The robot to use
//      */
//     PhyMarXbot* m_robot;
//
//     /**
//      * \brief The object to iterate over neurons of the neural network
//      */
//     NeuronsIterator* m_neuronsIterator;
//
//     /**
//      * \brief The low level left sensor
//      */
//     SingleIR* m_sensorLeft;
//
//     /**
//      * \brief The low level central sensor
//      */
//     SingleIR* m_sensorCenter;
//
//     /**
//      * \brief The low level right sensor
//      */
//     SingleIR* m_sensorRight;
//
//     /**
//      * \brief The low level rear left sensor
//      */
//     SingleIR* m_sensorRearLeft;
//
//     /**
//      * \brief The low level rear sensor
//      */
//     SingleIR* m_sensorRear;
//
//     /**
//      * \brief The low level rear right sensor
//      */
//     SingleIR* m_sensorRearRight;
//
//     /**
//      * \brief The maximum distance of sensors
//      */
//     const real m_maxDistance;
//
//     /**
//      * \brief Whether to draw the sensors or not
//      */
//     const bool m_drawSensors;
//
//     /**
//      * \brief When drawing the sensor, whether to draw the rays or not
//      */
//     const bool m_drawRays;
//
//     /**
//      * \brief When drawing the rays, whether to draw the real range or not
//      */
//     const bool m_drawRaysRange;
//
//     /**
//      * \brief Whether the sensors should be averaged or not
//      */
//     const bool m_avgSensors;
// };
//
// }
//
//
// #endif
