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
// #ifndef EPUCKSENSORS_H
// #define EPUCKSENSORS_H
//
// #include "experimentsconfig.h"
// #include "neuroninterfaces.h"
// #include "world.h"
// #include "robots.h"
// #include "phybox.h"
// #include "phycylinder.h"
// #include "sensors.h"
//
// namespace salsa {
//
// /**
//  * \brief The base abstract class for e-puck sensors
//  *
//  * This class simply has a couple of parameters that are common to all e-puck
//  * sensors (see below). Some of the sensors work directly with Worldsim objects,
//  * while others need an instance of the Arena to work. Those that do not need an
//  * Arena generally also work with objects created using the Arena object, but in
//  * most of the cases are slower. See the description of individual sensors for
//  * more information about what each sensor needs.
//  *
//  * In addition to all parameters defined by the parent class (Sensor), this
//  * class also defines the following parameters:
//  * - epuck: the name of the resource associated with the e-puck robot to use
//  *          (default is "robot")
//  * - neuronsIterator: the name of the resource associated with the neural
//  *                    network iterator (default is "neuronsIterator")
//  *
//  * The resources required by this Sensor are:
//  * - name defined by the epuck parameter: the simulated physical e-puck robot
//  * - name defined by the neuronsIterator parameter: the object to iterate over
//  *   neurons of the neural network
//  */
// class SALSA_EXPERIMENTS_API EpuckSensor : public Sensor
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
// 	EpuckSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckSensor();
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
//  * \brief The infrared proximity sensors of the e-puck
//  *
//  * The infrared proximity sensors of the e-puck. This version of the infrared
//  * proximity sensors works with all Worldsim objects, but it is a bit slow. This
//  * sensor applies noise if requested
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (EpuckSensor)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class
//  */
// class SALSA_EXPERIMENTS_API EpuckProximityIRSensor : public EpuckSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	EpuckProximityIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckProximityIRSensor();
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
// 	PhyEpuck* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
// };
//
// /**
//  * \brief The infrared ground sensors of the e-puck
//  *
//  * The infrared ground sensors of the e-puck (1.0 for white, 0.0 for black).
//  * This only works with objects in the arena. This sensor applies noise if
//  * requested
//  *
//  * This class does not add any other parameter to those defined by the parent
//  * class (EpuckSensor)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API EpuckGroundIRSensor : public EpuckSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	EpuckGroundIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckGroundIRSensor();
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
// 	PhyEpuck* m_robot;
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
//  * \brief The linear camera sensor of the e-puck
//  *
//  * This is a linear camera with configurable aperture (using the aperture
//  * parameter). The number of photoreceptors can be configured using the
//  * numReceptors parameter. Each receptor returns three values, one for each of
//  * the three colors (red, green, blue). This means that the size returned by
//  * this sensor is 3 * numReceptors. The inputs for the controller are returned
//  * in the following order: first the values of all the red receptors, then the
//  * values for all the green receptors, finally the values for all the blue
//  * receptors.
//  * This sensor only works with objects created using the Arena instance. This
//  * sensor applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (EpuckSensor),
//  * this class also defines the following parameters:
//  * - numReceptors: the number of receptors of the sensor. Each receptor returns
//  *                 three values, one for each of the three colors (red, green,
//  *                 blue). This means that the size returned by this sensor is
//  *                 3 * numReceptors (default is 8)
//  * - aperture: the aperture of the camera in degrees. The real e-puck has a
//  *             camera with an aperture of 36°, but here you can use any value up
//  *             to 360°
//  * - drawCamera: whether to draw the camera on the robot or not (default is
//  *               true)
//  * - ignoreWalls: whether to perceive walls or not (default is false)
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API EpuckLinearCameraSensor : public EpuckSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	EpuckLinearCameraSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckLinearCameraSensor();
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
// 	 * \brief Returns the number of receptors of the camera
// 	 *
// 	 * \return the number of receptors of the camera
// 	 */
// 	int getNumReceptors() const
// 	{
// 		return m_numReceptors;
// 	}
//
// 	/**
// 	 * \brief Returns the camera sensor used internally
// 	 *
// 	 * \return the camera sensor used internally
// 	 */
// 	const LinearCamera* getCamera() const
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
// 	PhyEpuck* m_robot;
//
// 	/**
// 	 * \brief The object to iterate over neurons of the neural network
// 	 */
// 	NeuronsIterator* m_neuronsIterator;
//
// 	/**
// 	 * \brief The number of receptors of the sensor
// 	 *
// 	 * Each receptor returns three values, on for each of the three colors
// 	 * (red, green, blue). This means that the size returned by this sensor
// 	 * is 3 * m_numReceptors
// 	 */
// 	int m_numReceptors;
//
// 	/**
// 	 * \brief The aperture of the camera in degrees
// 	 *
// 	 * The real e-puck has a camera with an aperture of 36°, but here you
// 	 * can use any value up to 360°
// 	 */
// 	real m_aperture;
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
//
// 	/**
// 	 * \brief If true walls are perceived through the camera, if false they
// 	 *        are ignored
// 	 */
// 	const bool m_ignoreWalls;
// };
//
// /**
//  * \brief The sampled proximity infrared sensors of the e-puck
//  *
//  * The sampled proximity infrared sensors of the e-puck. This version of the
//  * infrared proximity sensors is based on data sampled from the real robot. It
//  * is fast but only works with objects created using the Arena. This sensor
//  * applies noise if requested
//  *
//  * In addition to all parameters defined by the parent class (EpuckSensor),
//  * this class also defines the following parameters:
//  * - roundSamples: the file with samples for big round objects
//  * - smallSamples: the file with samples for small round objects
//  * - wallSamples: the file with samples for walls
//  *
//  * The resources required by this Sensor are the same as those of the parent
//  * class plus:
//  * - arena: the instance of the Arena object where the robot lives
//  */
// class SALSA_EXPERIMENTS_API EpuckSampledProximityIRSensor : public EpuckSensor
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * Creates and configures the sensor
// 	 * \param params the ConfigurationParameters containing the parameters
// 	 * \param prefix the path prefix to the paramters for this Sensor
// 	 */
// 	EpuckSampledProximityIRSensor(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~EpuckSampledProximityIRSensor();
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
// 	Epuck* m_robot;
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
// }
//
// #endif
