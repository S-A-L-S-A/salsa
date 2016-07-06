// /********************************************************************************
//  *  FARSA Experiments Library                                                   *
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
// #include "marxbotsensors.h"
// #include "configurationhelper.h"
// #include "logger.h"
// #include "graphicalwobject.h"
// #include "arena.h"
// #include "mathutils.h"
// #include "utilitiesexceptions.h"
// #include <QMutex>
// #include <limits>
//
// namespace farsa {
//
// MarXbotSensor::MarXbotSensor(ConfigurationParameters& params, QString prefix) :
// 	Sensor(params, prefix),
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
// MarXbotSensor::~MarXbotSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	Sensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotSensor", this);
// 	params.createParameter(prefix, "marxbot", m_marxbotResource);
// 	params.createParameter(prefix, "neuronsIterator", m_neuronsIteratorResource);
// }
//
// void MarXbotSensor::describe(QString type)
// {
// 	// Calling parent function
// 	Sensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The base class for MarXbot sensors");
// 	d.describeString("marxbot").def("robot").help("The name of the resource associated with the MarXbot robot to use (default is \"robot\")");
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("The name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// }
//
// void MarXbotSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	Sensor::resourceChanged(resourceName, changeType);
//
// 	// Here we only check whether the resource has been deleted and reset the check flag, the
// 	// actual work is done in subclasses
// 	if (changeType == Deleted) {
// 		resetNeededResourcesCheck();
// 		return;
// 	}
// }
//
// MarXbotProximityIRSensor::MarXbotProximityIRSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_activeSensors(ConfigurationHelper::getBoolVector(params, prefix + "activeSensors", "111111111111111111111111")),
// 	m_numActiveSensors(m_activeSensors.count(true))
// {
// 	if (m_activeSensors.size() != 24) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "activeSensors", params.getValue(prefix + "activeSensors"), "The parameter must be a list of exactly 24 elements either equal to 1 or to 0 (do not use any space to separate elements, just put them directly one after the other)");
// 	}
// }
//
// MarXbotProximityIRSensor::~MarXbotProximityIRSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotProximityIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotProximityIRSensor", this);
// 	QString activeSensorsString;
// 	for (int i = 0; i < m_activeSensors.size(); i++) {
// 		activeSensorsString += (m_activeSensors[i] ? "1" : "0");
// 	}
// 	params.createParameter(prefix, "activeSensors", activeSensorsString);
// }
//
// void MarXbotProximityIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The infrared proximity sensors of the MarXbot robot", "The infrared proximity sensors of the MarXbot robot. These are the very short range IR sensors all around the base");
// 	d.describeString("activeSensors").def("111111111111111111111111").help("Which IR sensors of the robot are actually enabled", "This is a string of exactly 24 elements. Each element can be either \"0\" or \"1\" to respectively disable/enable the corresponding proximity IR sensor. The first sensor is the one on the left side of the robot and the others follow counterclockwise (i.e. left, back, right, front)");
// }
//
// void MarXbotProximityIRSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < m_activeSensors.size(); i++) {
// 		if (m_activeSensors[i]) {
// 			m_neuronsIterator->setInput(applyNoise(m_robot->proximityIRSensorController()->activation(i), 0.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 		}
// 	}
// }
//
// int MarXbotProximityIRSensor::size()
// {
// 	return m_numActiveSensors;
// }
//
// void MarXbotProximityIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Eanbling sensors
// 		m_robot->proximityIRSensorController()->setEnabled(true);
//
// 		// Now enabling/disabling individual sensors depending on the m_activeSensors vector
// 		for (int i = 0; i < m_activeSensors.size(); i++) {
// 			m_robot->proximityIRSensorController()->setSensorActive(i, m_activeSensors[i]);
// 		}
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		// We use the actual sensor ID in the neuron name
// 		for (int i = 0; i < m_activeSensors.size(); i++) {
// 			if (m_activeSensors[i]) {
// 				m_neuronsIterator->setGraphicProperties("ir" + QString::number(i), 0.0, 1.0, Qt::red);
// 				m_neuronsIterator->nextNeuron();
// 			}
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotGroundBottomIRSensor::MarXbotGroundBottomIRSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_arena(NULL),
// 	m_invertActivation(ConfigurationHelper::getBool(params, prefix + "invertActivation", false))
// {
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// MarXbotGroundBottomIRSensor::~MarXbotGroundBottomIRSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotGroundBottomIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotGroundBottomIRSensor", this);
// 	params.createParameter(prefix, "invertActivation", (m_invertActivation ? "true" : "false"));
// }
//
// void MarXbotGroundBottomIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The infrared ground bottom sensors of the MarXbot robot", "The infrared ground bottom sensors of the MarXbot robot. These are the four ground sensors below the robot battery pack.");
// 	d.describeBool("invertActivation").def(false).help("If true inverts the activation of the sensors", "If true the sensor is activated with 0.0 above white and with 1.0 above black, if false the opposite holds. The default value is false");
// }
//
// void MarXbotGroundBottomIRSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	// Setting neurons activations
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 		const wVector sensorPosition = m_robot->matrix().transformVector(m_robot->groundBottomIRSensorController()->sensors()[i].getPosition());
// 		const QColor color = getColorAtArenaGroundPosition(m_arena, sensorPosition);
// 		const real actv = real(qGray(color.rgb())) / 255.0;
//
// 		if (m_invertActivation) {
// 			m_neuronsIterator->setInput(applyNoise(1.0 - actv, 0.0, 1.0));
// 		} else {
// 			m_neuronsIterator->setInput(applyNoise(actv, 0.0, 1.0));
// 		}
// 	}
// }
//
// int MarXbotGroundBottomIRSensor::size()
// {
// 	return 4;
// }
//
// void MarXbotGroundBottomIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Eanbling sensors
// 		m_robot->groundBottomIRSensorController()->setEnabled(true);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("gb" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else if (resourceName == "arena") {
// 		// Storing the pointer to the arena
// 		m_arena = getResource<Arena>();
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotGroundAroundIRSensor::MarXbotGroundAroundIRSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_arena(NULL)
// {
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// MarXbotGroundAroundIRSensor::~MarXbotGroundAroundIRSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotGroundAroundIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotGroundAroundIRSensor", this);
// }
//
// void MarXbotGroundAroundIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The infrared ground around sensors of the MarXbot robot", "The infrared ground around sensors of the MarXbot robot. These are the eight ground sensors below the base of the robot (just above the wheels).");
// }
//
// void MarXbotGroundAroundIRSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	// Setting neurons activations
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 		const wVector sensorPosition = m_robot->matrix().transformVector(m_robot->groundAroundIRSensorController()->sensors()[i].getPosition());
// 		const QColor color = getColorAtArenaGroundPosition(m_arena, sensorPosition);
// 		const real actv = real(qGray(color.rgb())) / 255.0;
//
// 		m_neuronsIterator->setInput(applyNoise(actv, 0.0, 1.0));
// 	}
// }
//
// int MarXbotGroundAroundIRSensor::size()
// {
// 	return 8;
// }
//
// void MarXbotGroundAroundIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Eanbling sensors
// 		m_robot->groundAroundIRSensorController()->setEnabled(true);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("ga" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else if (resourceName == "arena") {
// 		// Storing the pointer to the arena
// 		m_arena = getResource<Arena>();
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotLinearCameraSensor::MarXbotLinearCameraSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_numReceptors(ConfigurationHelper::getInt(params, prefix + "numReceptors", 8)),
// 	m_useRedChannel(ConfigurationHelper::getBool(params, prefix + "useRedChannel", true)),
// 	m_useGreenChannel(ConfigurationHelper::getBool(params, prefix + "useGreenChannel", true)),
// 	m_useBlueChannel(ConfigurationHelper::getBool(params, prefix + "useBlueChannel", true)),
// 	m_usedColorChannels((m_useRedChannel ? 1 : 0) + (m_useGreenChannel ? 1 : 0) + (m_useBlueChannel ? 1 : 0)),
// 	m_aperture(ConfigurationHelper::getDouble(params, prefix + "aperture", 360.0f)),
// 	m_maxDistance(ConfigurationHelper::getDouble(params, prefix + "maxDistance", +Infinity)),
// 	m_camera(NULL),
// 	m_drawCamera(ConfigurationHelper::getBool(params, prefix + "drawCamera", true))
// {
// 	// Few sanity checks
// 	if (m_numReceptors <= 0) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "numReceptors", params.getValue(prefix + "numReceptors"), "The parameter must be an integer greater or equal to 1");
// 	}
// 	if ((m_aperture < 0.0f) || (m_aperture > 360.0f)) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "aperture", params.getValue(prefix + "aperture"), "The parameter must be a real number between 0 and 360");
// 	}
// }
//
// MarXbotLinearCameraSensor::~MarXbotLinearCameraSensor()
// {
// 	// Deleting the camera
// 	delete m_camera;
// }
//
// void MarXbotLinearCameraSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotLinearCameraSensor", this);
// 	params.createParameter(prefix, "numReceptors", QString::number(m_numReceptors));
// 	params.createParameter(prefix, "useRedChannel", (m_useRedChannel ? "true" : "false"));
// 	params.createParameter(prefix, "useGreenChannel", (m_useGreenChannel ? "true" : "false"));
// 	params.createParameter(prefix, "useBlueChannel", (m_useBlueChannel ? "true" : "false"));
// 	params.createParameter(prefix, "aperture", QString::number(m_aperture));
// 	params.createParameter(prefix, "maxDistance", QString::number(m_maxDistance));
// 	params.createParameter(prefix, "drawCamera", (m_drawCamera ? "true" : "false"));
// }
//
// void MarXbotLinearCameraSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The linear camera sensor of the MarXbot robot", "This is a 360° linear camera");
// 	d.describeInt("numReceptors").def(8).limits(1, MaxInteger).help("The number of receptors of the sensor", "Each receptor returns three values, one for each of the three colors (red, green, blue). This means that the size returned by this sensor is 3 * numReceptors (default is 8)");
// 	d.describeBool("useRedChannel").def(true).help("Whether the red component of the perceived objects should be used or not (default true)");
// 	d.describeBool("useGreenChannel").def(true).help("Whether the green component of the perceived objects should be used or not (default true)");
// 	d.describeBool("useBlueChannel").def(true).help("Whether the blue component of the perceived objects should be used or not (default true)");
// 	d.describeReal("aperture").def(360.0f).limits(0.0f, 360.0f).help("The aperture of the camera in degrees", "The real MarXbot has an omnidirectional camera, so you can use here any value up to 360 degrees (default is 360)");
// 	d.describeReal("maxDistance").def(+Infinity).limits(0.0f, +Infinity).help("The distance above which objects are not seen by the camera in meters");
// 	d.describeBool("drawCamera").def(true).help("Whether to draw the camera or not");
// }
//
// void MarXbotLinearCameraSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Updating the camera
// 	m_camera->update();
//
// 	// Reading activations: first the red one, then the green one and finally the blue one
// 	m_neuronsIterator->setCurrentBlock(name());
// 	if (m_useRedChannel) {
// 		for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).redF(), 0.0, 1.0));
// 		}
// 	}
// 	if (m_useGreenChannel) {
// 		for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).greenF(), 0.0, 1.0));
// 		}
// 	}
// 	if (m_useBlueChannel) {
// 		for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).blueF(), 0.0, 1.0));
// 		}
// 	}
// }
//
// int MarXbotLinearCameraSensor::size()
// {
// 	return m_numReceptors * m_usedColorChannels;
// }
//
// void MarXbotLinearCameraSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		// Deleting the camera if the robot was deleted
// 		if (resourceName == m_marxbotResource) {
// 			delete m_camera;
// 			m_camera = NULL;
// 		}
//
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Now we can also create the camera
// 		wMatrix mtr = wMatrix::roll(-PI_GRECO / 2.0);
// #if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
// 	#warning QUI INVECE DI UNA COSTANTE, CALCOLARSI UNA POSIZIONE DALLE DIMENSIONI DEL ROBOT
// #endif
// 		mtr.w_pos.z = 0.13f;
// 		m_camera = new LinearCamera(m_robot, mtr, toRad(m_aperture), m_numReceptors, m_maxDistance, Qt::black);
//
// 		// Sharing resources with the camera
// 		m_camera->shareResourcesWith(this);
// 		m_camera->drawCamera(m_drawCamera);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		if (m_useRedChannel) {
// 			for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 				m_neuronsIterator->setGraphicProperties("lr" + QString::number(i), 0.0, 1.0, Qt::red);
// 			}
// 		}
// 		if (m_useGreenChannel) {
// 			for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 				m_neuronsIterator->setGraphicProperties("lg" + QString::number(i), 0.0, 1.0, Qt::red);
// 			}
// 		}
// 		if (m_useBlueChannel) {
// 			for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 				m_neuronsIterator->setGraphicProperties("lb" + QString::number(i), 0.0, 1.0, Qt::red);
// 			}
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotLinearCameraSensorNew::MarXbotLinearCameraSensorNew(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_numReceptors(ConfigurationHelper::getInt(params, prefix + "numReceptors", 8)),
// 	m_useRedChannel(ConfigurationHelper::getBool(params, prefix + "useRedChannel", true)),
// 	m_useGreenChannel(ConfigurationHelper::getBool(params, prefix + "useGreenChannel", true)),
// 	m_useBlueChannel(ConfigurationHelper::getBool(params, prefix + "useBlueChannel", true)),
// 	m_usedColorChannels((m_useRedChannel ? 1 : 0) + (m_useGreenChannel ? 1 : 0) + (m_useBlueChannel ? 1 : 0)),
// 	m_aperture(ConfigurationHelper::getDouble(params, prefix + "aperture", 360.0f)),
// 	m_maxDistance(ConfigurationHelper::getDouble(params, prefix + "maxDistance", +Infinity)),
// 	m_receptorsRanges(),
// 	m_camera(NULL),
// 	m_drawCamera(ConfigurationHelper::getBool(params, prefix + "drawCamera", true))
// {
// 	// We have to get here the receptorsRanges parameter
// 	QString receptorsRangesStr = params.getValue(prefix + "receptorsRanges");
// 	if (receptorsRangesStr.isEmpty()) {
// 		// Here we use aperture and numReceptors, doing few sanity checks
// 		if (m_numReceptors <= 0) {
// 			ConfigurationHelper::throwUserConfigError(prefix + "numReceptors", params.getValue(prefix + "numReceptors"), "The parameter must be an integer greater or equal to 1");
// 		}
// 		if ((m_aperture < 0.0f) || (m_aperture > 360.0f)) {
// 			ConfigurationHelper::throwUserConfigError(prefix + "aperture", params.getValue(prefix + "aperture"), "The parameter must be a real number between 0 and 360");
// 		}
// 	} else {
// 		// Here we use receptorsRanges, checking that neither aperture nor numReceptors is set
// 		if (!params.getValue(prefix + "numReceptors").isEmpty() || !params.getValue(prefix + "aperture").isEmpty()) {
// 			ConfigurationHelper::throwUserConfigError(prefix + "receptorsRanges", params.getValue(prefix + "receptorsRanges"), "You must not specify the receptorsRanges parameter if either aperture or numReceptors has been specified");
// 		}
//
// 		// Now getting the parameter
// 		bool ok;
// 		m_receptorsRanges = SimpleInterval::vectorOfSimpleIntervalsFromString(receptorsRangesStr, &ok);
// 		if (!ok) {
// 			ConfigurationHelper::throwUserConfigError(prefix + "receptorsRanges", params.getValue(prefix + "receptorsRanges"), "The receptorsRanges parameter must be a comma separated list of simple intervals");
// 		}
//
// 		// We also have to convert angles to radiants
// 		for (int i = 0; i < m_receptorsRanges.size(); i++) {
// 			m_receptorsRanges[i].start = toRad(m_receptorsRanges[i].start);
// 			m_receptorsRanges[i].end = toRad(m_receptorsRanges[i].end);
// 		}
//
// 		// Setting m_numReceptors to the correct value
// 		m_numReceptors = m_receptorsRanges.size();
// 	}
// }
//
// MarXbotLinearCameraSensorNew::~MarXbotLinearCameraSensorNew()
// {
// 	// Deleting the camera
// 	delete m_camera;
// }
//
// void MarXbotLinearCameraSensorNew::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotLinearCameraSensorNew", this);
// 	params.createParameter(prefix, "numReceptors", QString::number(m_numReceptors));
// 	params.createParameter(prefix, "aperture", QString::number(m_aperture));
// 	params.createParameter(prefix, "maxDistance", QString::number(m_maxDistance));
// 	params.createParameter(prefix, "useRedChannel", (m_useRedChannel ? "true" : "false"));
// 	params.createParameter(prefix, "useGreenChannel", (m_useGreenChannel ? "true" : "false"));
// 	params.createParameter(prefix, "useBlueChannel", (m_useBlueChannel ? "true" : "false"));
// 	params.createParameter(prefix, "drawCamera", (m_drawCamera ? "true" : "false"));
// }
//
// void MarXbotLinearCameraSensorNew::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The linear camera sensor of the MarXbot robot (new implementation)", "This is a 360° linear camera");
// 	d.describeInt("numReceptors").def(8).limits(1, MaxInteger).help("The number of receptors of the sensor", "Each receptor returns three values, one for each of the three colors (red, green, blue). This means that the size returned by this sensor is 3 * numReceptors (default is 8)");
// 	d.describeReal("aperture").def(360.0f).limits(0.0f, 360.0f).help("The aperture of the camera in degrees", "The real MarXbot has an omnidirectional camera, so you can use here any value up to 360 degrees (default is 360)");
// 	d.describeReal("maxDistance").def(+Infinity).limits(0.0f, +Infinity).help("The distance above which objects are not seen by the camera in meters");
// 	d.describeString("receptorsRanges").def("").help("The range for each receptor", "This parameter is a comma separated list of ranges (in the form [start, end]), each range being the aperture of a single receptor. The number of receptors is equal to the number of ranges. The start and end angles of the range are in degrees. Note that if you specify this parameter you must not specify the aperture neither the numReceptors parameters.");
// 	d.describeBool("useRedChannel").def(true).help("Whether the red component of the perceived objects should be used or not (default true)");
// 	d.describeBool("useGreenChannel").def(true).help("Whether the green component of the perceived objects should be used or not (default true)");
// 	d.describeBool("useBlueChannel").def(true).help("Whether the blue component of the perceived objects should be used or not (default true)");
// 	d.describeBool("drawCamera").def(true).help("Whether to draw the camera or not");
// }
//
// void MarXbotLinearCameraSensorNew::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Updating the camera
// 	m_camera->update();
//
// 	// Reading activations: first the red one, then the green one and finally the blue one
// 	m_neuronsIterator->setCurrentBlock(name());
// 	if (m_useRedChannel) {
// 		for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).redF(), 0.0, 1.0));
// 		}
// 	}
// 	if (m_useGreenChannel) {
// 		for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).greenF(), 0.0, 1.0));
// 		}
// 	}
// 	if (m_useBlueChannel) {
// 		for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).blueF(), 0.0, 1.0));
// 		}
// 	}
// }
//
// int MarXbotLinearCameraSensorNew::size()
// {
// 	return m_numReceptors * m_usedColorChannels;
// }
//
// void MarXbotLinearCameraSensorNew::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		// Deleting the camera if the robot was deleted
// 		if (resourceName == m_marxbotResource) {
// 			delete m_camera;
// 			m_camera = NULL;
// 		}
//
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Now we can also create the camera
// 		wMatrix mtr = wMatrix::roll(-PI_GRECO / 2.0);
// #if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
// 	#warning QUI INVECE DI UNA COSTANTE, CALCOLARSI UNA POSIZIONE DALLE DIMENSIONI DEL ROBOT
// #endif
// 		mtr.w_pos.z = 0.13f;
// 		if (m_receptorsRanges.isEmpty()) {
// 			m_camera = new LinearCameraNew::LinearCamera(m_robot, mtr, toRad(m_aperture), m_numReceptors, m_maxDistance, Qt::black);
// 		} else {
// 			m_camera = new LinearCameraNew::LinearCamera(m_robot, mtr, m_receptorsRanges, m_maxDistance, Qt::black);
// 		}
//
// 		// Sharing resources with the camera
// 		m_camera->shareResourcesWith(this);
// 		m_camera->drawCamera(m_drawCamera);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		if (m_useRedChannel) {
// 			for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 				m_neuronsIterator->setGraphicProperties("lr" + QString::number(i), 0.0, 1.0, Qt::red);
// 			}
// 		}
// 		if (m_useGreenChannel) {
// 			for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 				m_neuronsIterator->setGraphicProperties("lg" + QString::number(i), 0.0, 1.0, Qt::red);
// 			}
// 		}
// 		if (m_useBlueChannel) {
// 			for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 				m_neuronsIterator->setGraphicProperties("lb" + QString::number(i), 0.0, 1.0, Qt::red);
// 			}
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// namespace __MarXbotTractionSensor_internal {
// 	#ifndef GLMultMatrix
// 	#define GLMultMatrix glMultMatrixf
// 	// for double glMultMatrixd
// 	#endif
//
// 	/**
// 	 * \brief The graphical representation of the MarXbot traction sensor
// 	 *
// 	 * The arrow is drawn as a cylinder with a cone on top
// 	 * \internal
// 	 */
// 	class TractionSensorGraphic : public GraphicalWObject
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * This also sets a default color for the piece and sets
// 		 * handPiece to be our owner. This draws an arrow representing
// 		 * a vector
// 		 * \param object the object to which this representation is
// 		 *               attached. This must not be NULL
// 		 * \param offset the offset relative to object. The arrow starts
// 		 *               from the offset in the frame of reference of
// 		 *               the object
// 		 * \param radius the radius of the arrow. The length depends on
// 		 *               the vector intensity
// 		 * \param scale the scaling factor for the vector length
// 		 * \param maxLength the maximum length of the vector in each
// 		 *                  axis. If the vector is longer than this on, the arrow is
// 		 *                  drawn this length with a different color.
// 		 *                  This is the length *BEFORE* scaling
// 		 * \param minLength the minimum length of the vector in each
// 		 *                  axis. If the vector is shorter than this,
// 		 *                  the arrow is not drawn. This is the length
// 		 *                  *BEFORE* scaling
// 		 * \param name the name of this object
// 		 */
// 		TractionSensorGraphic(WObject *object, const wVector& offset, real radius, real scale = 1.0, real maxLength = 1.0, real minLength = 0.0,  QString name = "unamed") :
// 			GraphicalWObject(object->world(), name),
// 			m_object(object),
// 			m_radius(radius),
// 			m_scale(scale),
// 			m_maxLength(maxLength),
// 			m_minLength(minLength),
// 			m_maxTipLength(m_radius * 2),
// 			m_vector(0.0, 0.0, 0.0),
// 			m_vectorMutex()
// 		{
// 			// Attaching to object (which also becomes our owner). We also build the displacement matrix
// 			wMatrix displacement = wMatrix::identity();
// 			displacement.w_pos = offset;
// 			attachToObject(m_object, true, displacement);
//
// 			// We also use our own color and texture
// 			setUseColorTextureOfOwner(false);
// 			setTexture("");
// 		}
//
// 		/**
// 		 * \brief Destructor
// 		 */
// 		~TractionSensorGraphic()
// 		{
// 		}
//
// 		/**
// 		 * \brief Sets the vector to draw. The vector must be in the
// 		 *        object frame of reference
// 		 *
// 		 * This function is thread-safe
// 		 * \param vector the vector to draw. The vector must be in the
// 		 *               object frame of reference
// 		 */
// 		void setVector(const wVector vector)
// 		{
// 			m_vectorMutex.lock();
// 				m_vector = vector;
// 			m_vectorMutex.unlock();
// 		}
//
// 	protected:
// 		/**
// 		 * \brief Performs the actual drawing
// 		 *
// 		 * Draws the vector
// 		 * \param renderer the RenderWObject object associated with this one.
// 		 *                 Use it e.g. to access the container
// 		 * \param gw the OpenGL context
// 		 */
// 		virtual void render(RenderWObject* renderer, QGLContext* gw)
// 		{
// 			// Copying the m_activations vector to a local vector to avoid concurrent accesses.
// 			m_vectorMutex.lock();
// 				const wVector vector = m_vector;
// 			m_vectorMutex.unlock();
//
// 			// Setting the matrix to draw the vector.
// 			glPushMatrix();
// 			GLMultMatrix(&tm[0][0]);
//
// 			bool xLimited;
// 			bool yLimited;
// 			const wVector vectorToDraw = wVector(limitComponent(vector.x, xLimited), limitComponent(vector.y, yLimited), vector.z).scale(m_scale);
// 			const wVector xComponent(vectorToDraw.x, 0.0, vectorToDraw.z);
// 			const wVector yComponent(0.0, vectorToDraw.y, vectorToDraw.z);
//
// 			// Drawing the three arrows (two for the axes, one for the resultant
// 			drawArrow(xComponent, m_radius / 2.0, m_maxTipLength / 2.0, (xLimited ? Qt::red : Qt::cyan), renderer, gw);
// 			drawArrow(yComponent, m_radius / 2.0, m_maxTipLength / 2.0, (yLimited ? Qt::red : Qt::cyan), renderer, gw);
// 			drawArrow(vectorToDraw, m_radius, m_maxTipLength, Qt::cyan, renderer, gw);
//
// 			// Popping twice because we pushed both our matrix and the offset matrix
// 			glPopMatrix();
// 			glPopMatrix();
// 		}
//
// 		/**
// 		 * \brief Draws an arrow
// 		 *
// 		 * This function can only be called by render(). This uses the
// 		 * current frame of reference
// 		 * \param vector the vector to draw
// 		 * \param radius the radius of the arrow to draw
// 		 * \param maxTipLength the maximum length of the tip of the
// 		 *                     arrow
// 		 * \param col the color of the arrow
// 		 * \param renderer the RenderWObject object associated with this
// 		 *                 one. Use it e.g. to access the container
// 		 * \param gw the OpenGL context
// 		 */
// 		void drawArrow(wVector vector, real radius, real maxTipLength, QColor col, RenderWObject* renderer, QGLContext* gw)
// 		{
// 			// Setting the color of the arrow
// 			setColor(col);
// 			renderer->container()->setupColorTexture(gw, renderer);
//
// 			const real vectorLength = vector.norm();
// 			const wVector axis = vector.scale(1.0 / vectorLength);
// 			const real bodyLength = vectorLength - maxTipLength;
// 			const real tipLength = min(vectorLength, maxTipLength);
// 			const real tipRadius = ((radius * 3.0) / 2.0) * (tipLength / maxTipLength);
//
// 			// First drawing the body if its length is positive
// 			if (bodyLength > 0.0) {
// 				RenderWObjectContainer::drawCylinder(axis, axis.scale(0.5 * bodyLength), bodyLength, radius, color());
// 			}
//
// 			// Now drawing the tip as a cone:
// 			{
// 				GLUquadricObj *pObj;
//
// 				// opengl cylinder are aligned alogn the z axis, we want it along our axis,
// 				// we create a rotation matrix to do the alignment. Moreover the cone must
// 				// start at the end of the cylinder we draw before
// 				const wVector tipDisplacement = (bodyLength < 0.0) ? wVector(0.0, 0.0, 0.0) : axis.scale(bodyLength);
// 				glPushMatrix();
// 				wMatrix matrix = wMatrix::grammSchmidt(axis);
// 				matrix.w_pos = tipDisplacement;
// 				GLMultMatrix(&matrix[0][0]);
//
// 				// Get a new Quadric off the stack
// 				pObj = gluNewQuadric();
// 				gluQuadricTexture(pObj, true);
// 				gluCylinder(pObj, tipRadius, 0, tipLength, 20, 2);
//
// 				// render the caps
// 				gluQuadricOrientation(pObj, GLU_INSIDE);
// 				gluDisk(pObj, 0.0f, tipRadius, 20, 1);
//
// 				gluDeleteQuadric(pObj);
// 				glPopMatrix();
// 			}
// 		}
//
// 		/**
// 		 * \brief Limits the component to be between m_minLength and
// 		 *        m_maxLength
// 		 *
// 		 * This takes the sign of the component into account
// 		 * \param v the value of the component to limit
// 		 * \param limited set to true if the value has been limited
// 		 * \return the limited component
// 		 */
// 		real limitComponent(real v, bool& limited)
// 		{
// 			real l = fabs(v);
// 			limited = false;
// 			if (l < m_minLength) {
// 				limited = true;
// 				l = m_minLength;
// 			} else if (l > m_maxLength) {
// 				limited = true;
// 				l = m_maxLength;
// 			}
// 			return (v < 0.0) ? -l : l;
// 		}
//
// 		/**
// 		 * \brief The object to which this representation is attached
// 		 */
// 		WObject *const m_object;
//
// 		/**
// 		 * \brief The radius of the arrow. The length depends on the
// 		 *        vector intensity
// 		 */
// 		const real m_radius;
//
// 		/**
// 		 * \brief The scaling factor for the vector length
// 		 */
// 		const real m_scale;
//
// 		/**
// 		 * \brief The maximum length of the vector
// 		 *
// 		 * If the vector is longer than this, the arrow is drawn this
// 		 * length with a different color. This is the length *BEFORE*
// 		 * scaling
// 		 */
// 		const real m_maxLength;
//
// 		/**
// 		 * \brief The minimum length of the vector
// 		 *
// 		 * If the vector is shorter than this, the arrow is not drawn.
// 		 * This is the length *BEFORE* scaling
// 		 */
// 		const real m_minLength;
//
// 		/**
// 		 * \brief The maximum length of the tip of the vector
// 		 *
// 		 * This is computed in the constructor
// 		 */
// 		const real m_maxTipLength;
//
// 		/**
// 		 * \brief The vector to represent
// 		 *
// 		 * The vector is in the object frame of reference
// 		 */
// 		wVector m_vector;
//
// 		/**
// 		 * \brief The mutex protecting the m_vector vector
// 		 *
// 		 * The vector could be accessed by multiple threads
// 		 * concurrently, so we protect it with a mutex
// 		 */
// 		QMutex m_vectorMutex;
// 	};
// }
//
// using namespace __MarXbotTractionSensor_internal;
//
// MarXbotTractionSensor::MarXbotTractionSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_maxForce(ConfigurationHelper::getDouble(params, prefix + "maxForce", 1.0f)),
// 	m_minForce(ConfigurationHelper::getDouble(params, prefix + "minForce", 0.0f)),
// 	m_drawSensor(ConfigurationHelper::getBool(params, prefix + "drawSensor", true)),
// 	m_graphics(NULL)
// {
// 	if (m_maxForce < 0.0) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "maxForce", params.getValue(prefix + "maxForce"), "The parameter must be a positive real number");
// 	}
// 	if (m_minForce < 0.0) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "minForce", params.getValue(prefix + "minForce"), "The parameter must be a positive real number");
// 	}
// }
//
// MarXbotTractionSensor::~MarXbotTractionSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotTractionSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotTractionSensor", this);
// 	params.createParameter(prefix, "maxForce", QString::number(m_maxForce));
// 	params.createParameter(prefix, "minForce", QString::number(m_minForce));
// 	params.createParameter(prefix, "drawSensor", m_drawSensor ? QString("true") : QString("false"));
// }
//
// void MarXbotTractionSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The traction sensors of the MarXbot robot", "The traction sensors of the MarXbot robot. It is placed between the base and the turret. Note that this sensor only works when the robot is in dynamic mode");
// 	d.describeReal("maxForce").def(1.0f).limits(0.0f, +Infinity).help("The maximum possible value of the force", "This is the value of the force on one axis over which the corresponding neuron is activated with 1");
// 	d.describeReal("minForce").def(0.0f).limits(0.0f, +Infinity).help("The minimum possible value of the force", "This is the value of the force on one axis under which the corresponding neuron is activated with 0");
// 	d.describeBool("drawSensor").def(true).help("Whether to draw the sensor", "If true the sensor is graphically represented by an arrow in the direction of the current traction");
// }
//
// void MarXbotTractionSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	m_neuronsIterator->setCurrentBlock(name());
// 	const wVector& t = m_robot->tractionSensorController()->traction();
//
// 	// The first two neurons correspond to the x axis, the second two to the y axis
// 	if (t.x > 0.0) {
// 		const real f = linearMap(t.x, m_minForce, m_maxForce, 0.0, 1.0);
// 		m_neuronsIterator->setInput(applyNoise(f, 0.0, 1.0));
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setInput(0.0);
// 		m_neuronsIterator->nextNeuron();
// 	} else {
// 		const real f = linearMap(-t.x, m_minForce, m_maxForce, 0.0, 1.0);
// 		m_neuronsIterator->setInput(0.0);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setInput(applyNoise(f, 0.0, 1.0));
// 		m_neuronsIterator->nextNeuron();
// 	}
//
// 	if (t.y > 0.0) {
// 		const real f = linearMap(t.y, m_minForce, m_maxForce, 0.0, 1.0);
// 		m_neuronsIterator->setInput(applyNoise(f, 0.0, 1.0));
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setInput(0.0);
// 		m_neuronsIterator->nextNeuron();
// 	} else {
// 		const real f = linearMap(-t.y, m_minForce, m_maxForce, 0.0, 1.0);
// 		m_neuronsIterator->setInput(0.0);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setInput(applyNoise(f, 0.0, 1.0));
// 		m_neuronsIterator->nextNeuron();
// 	}
//
// 	// Updating graphical representation if we have to
// 	if (m_drawSensor) {
// 		// We only draw the x and y components of the vector
// 		m_graphics->setVector(wVector(t.x, t.y, 0.0));
// 	}
// }
//
// int MarXbotTractionSensor::size()
// {
// 	return 4;
// }
//
// void MarXbotTractionSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<PhyMarXbot>();
//
// 		// Eanbling sensors
// 		m_robot->tractionSensorController()->setEnabled(true);
//
// 		// If graphics is enabled, creating the graphical object
// 		if (m_drawSensor) {
// 			const wVector offset(0.0, 0.0, PhyMarXbot::basez + PhyMarXbot::bodyh + 0.1);
//
// 			m_graphics = new TractionSensorGraphic(m_robot, offset, 0.005f, 0.1f / m_maxForce, m_maxForce, m_minForce, "Traction sensor");
// 		}
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		m_neuronsIterator->setGraphicProperties("t+x", 0.0, 1.0, Qt::red);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setGraphicProperties("t-x", 0.0, 1.0, Qt::red);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setGraphicProperties("t+y", 0.0, 1.0, Qt::red);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setGraphicProperties("t-y", 0.0, 1.0, Qt::red);
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotSampledProximityIRSensor::MarXbotSampledProximityIRSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_arena(NULL),
// 	m_neuronsIterator(NULL),
// 	m_activeSensors(ConfigurationHelper::getBoolVector(params, prefix + "activeSensors", "111111111111111111111111")),
// 	m_averageNum(ConfigurationHelper::getInt(params, prefix + "averageNum", 0)),
// 	m_numActiveSensors((m_averageNum == 0) ? m_activeSensors.count(true) : (24 / m_averageNum)),
// 	m_roundSamples(ConfigurationHelper::getString(params, prefix + "roundSamples", "round.sam")),
// 	m_smallSamples(ConfigurationHelper::getString(params, prefix + "smallSamples", "small.sam")),
// 	m_wallSamples(ConfigurationHelper::getString(params, prefix + "wallSamples", "wall.sam"))
// {
// 	if (m_activeSensors.size() != 24) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "activeSensors", params.getValue(prefix + "activeSensors"), "The parameter must be a list of exactly 24 elements either equal to 1 or to 0 (do not use any space to separate elements, just put them directly one after the other)");
// 	}
//
// 	if ((m_averageNum != 0) && (m_averageNum != 2) && (m_averageNum != 3) && (m_averageNum != 4) && (m_averageNum != 6)) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "averageNum", params.getValue(prefix + "averageNum"), "The parameter can only be 0, 2, 3, 4 or 6");
// 	}
//
// 	// Checking that the sampled files have the right number of IR sensors
// 	if (m_roundSamples.numIR() != 24) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "roundSamples", m_roundSamples.filename(), "The file has samples for the wrong number of sensors, expected 24, got " + QString::number(m_roundSamples.numIR()));
// 	}
// 	if (m_smallSamples.numIR() != 24) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "smallSamples", m_smallSamples.filename(), "The file has samples for the wrong number of sensors, expected 24, got " + QString::number(m_smallSamples.numIR()));
// 	}
// 	if (m_wallSamples.numIR() != 24) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "wallSamples", m_wallSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_wallSamples.numIR()));
// 	}
//
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// MarXbotSampledProximityIRSensor::~MarXbotSampledProximityIRSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotSampledProximityIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotSampledProximityIRSensor", this);
// 	QString activeSensorsString;
// 	for (int i = 0; i < m_activeSensors.size(); i++) {
// 		activeSensorsString += (m_activeSensors[i] ? "1" : "0");
// 	}
// 	params.createParameter(prefix, "activeSensors", activeSensorsString);
// 	params.createParameter(prefix, "averageNum", QString::number(m_averageNum));
// 	params.createParameter(prefix, "roundSamples", m_roundSamples.filename());
// 	params.createParameter(prefix, "smallSamples", m_smallSamples.filename());
// 	params.createParameter(prefix, "wallSamples", m_wallSamples.filename());
// }
//
// void MarXbotSampledProximityIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The sampled proximity infrared sensors of the MarXbot", "This is the sampled version of the proximity infrared sensors of the MarXbot. This sensor only works with objects created using the Arena");
// 	d.describeString("activeSensors").def("111111111111111111111111").help("Which IR sensors of the robot are actually enabled", "This is a string of exactly 24 elements. Each element can be either \"0\" or \"1\" to respectively disable/enable the corresponding proximity IR sensor. The first sensor is the one on the left side of the robot and the others follow counterclockwise (i.e. left, back, right, front)");
// 	d.describeInt("averageNum").def(0).limits(0, 6).help("How many sensors should be averaged", "This can only be 0, 2, 3, 4 or 6. If 2, for each couple of consecutive sensors the average is returned, if 3 for each triplet of consecutive sensors the average is returned and so on. If this is zero, no average is computed and the activeSensors parameter is used. The default value is 0");
// 	d.describeString("roundSamples").def("round.sam").help("The name of the file with samples for big round objects");
// 	d.describeString("smallSamples").def("small.sam").help("The name of the file with samples for small round objects");
// 	d.describeString("wallSamples").def("wall.sam").help("The name of the file with samples for walls");
// }
//
// void MarXbotSampledProximityIRSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting the list of objects in the arena
// 	const QVector<PhyObject2DWrapper*>& objectsList = m_arena->getObjects();
//
// 	// Preparing the vector with activations and setting all values to 0
// 	QVector<real> activations(m_activeSensors.size(), 0.0);
//
// 	// Cycling through the list of objects. We first need to get the current position and orientation of the robot
// 	//const wVector robotPos = m_robot->position();
// 	//const real robotAng = m_robot->orientation(m_arena->getPlane());
// 	foreach(const PhyObject2DWrapper* obj, objectsList) {
// 		// Computing angle and distance. We don't need to remove the robot to which this sensor belongs because
// 		// the calculatations will give a negative distance
// 		double distance;
// 		double angle;
//
// 		// If computeDistanceAndOrientationFromRobot returns false, we have to discard this object
// 		if (!obj->computeDistanceAndOrientationFromRobot(*(m_arena->getRobotWrapper(m_marxbotResource)), distance, angle)) {
// 			continue;
// 		}
//
// 		// Getting the activation. The switch is to understand which samples to use
// 		QVector<unsigned int>::const_iterator actIt = QVector<unsigned int>::const_iterator();
// 		switch (obj->type()) {
// 			case PhyObject2DWrapper::Wall:
// 				actIt = m_wallSamples.getActivation(distance, angle);
// 				break;
// 			case PhyObject2DWrapper::SmallCylinder:
// 				actIt = m_smallSamples.getActivation(distance, angle);
// 				break;
// 			case PhyObject2DWrapper::BigCylinder:
// 			case PhyObject2DWrapper::WheeledRobot:
// 				actIt = m_roundSamples.getActivation(distance, angle);
// 				break;
// 			default:
// 				//Logger::warning("The sampled infrared sensor only works with Small Cylinders, Big Cylinders, Walls and other Robots");
// 				continue;
// 		}
//
// 		// Adding activations in the activations vector
// 		for (QVector<real>::iterator it = activations.begin(); it != activations.end(); ++it, ++actIt) {
// 			*it = min(1.0, *it + (real(*actIt) / 1024.0));
// 		}
// 	}
//
// 	// Finally activating neurons
// 	m_neuronsIterator->setCurrentBlock(name());
// 	if (m_averageNum == 0) {
// 		QVector<real>::const_iterator it = activations.constBegin();
// 		QVector<bool>::const_iterator activeIt = m_activeSensors.constBegin();
// 		while (activeIt != m_activeSensors.constEnd()) {
// 			if (*activeIt) {
// 				m_neuronsIterator->setInput(applyNoise(*it, 0.0, 1.0));
// 				m_neuronsIterator->nextNeuron();
// 			}
//
// 			++it;
// 			++activeIt;
// 		}
// 	} else {
// 		QVector<real>::const_iterator it = activations.constBegin();
// 		while (it != activations.constEnd()) {
// 			real avg = 0.0;
// 			for (int j = 0; j < m_averageNum; ++j, ++it) {
// 				avg += *it;
// 			}
// 			avg /= real(m_averageNum);
// 			m_neuronsIterator->setInput(applyNoise(avg, 0.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 		}
// 	}
// }
//
// int MarXbotSampledProximityIRSensor::size()
// {
// 	return m_numActiveSensors;
// }
//
// void MarXbotSampledProximityIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<MarXbot>();
//
// 		// Disabling proximity IR sensors, they are not used here
// 		m_robot->proximityIRSensorController()->setEnabled(false);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		// We use the actual sensor ID in the neuron name
// 		if (m_averageNum == 0) {
// 			for (int i = 0; i < m_activeSensors.size(); i++) {
// 				if (m_activeSensors[i]) {
// 					m_neuronsIterator->setGraphicProperties("ir" + QString::number(i), 0.0, 1.0, Qt::red);
// 					m_neuronsIterator->nextNeuron();
// 				}
// 			}
// 		} else {
// 			for (int i = 0; i < m_activeSensors.size(); i += m_averageNum) {
// 				m_neuronsIterator->setGraphicProperties(QString("ir%1-%2").arg(i).arg(i + m_averageNum - 1), 0.0, 1.0, Qt::red);
// 				m_neuronsIterator->nextNeuron();
// 			}
// 		}
// 	} else if (resourceName == "arena") {
// 		// Storing the pointer to the arena
// 		m_arena = getResource<Arena>();
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// MarXbotAttachmentDeviceSensor::MarXbotAttachmentDeviceSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_enablePosition(ConfigurationHelper::getBool(params, prefix + "enablePosition", true)),
// 	m_enableStatus(ConfigurationHelper::getBool(params, prefix + "enableStatus", true)),
// 	m_enableAttached(ConfigurationHelper::getBool(params, prefix + "enableAttached", true)),
// 	m_enableOtherAttached(ConfigurationHelper::getBool(params, prefix + "enableOtherAttached", true)),
// 	m_numInputs((m_enablePosition ? 2 : 0) + (m_enableStatus ? 1 : 0) + (m_enableAttached ? 1 : 0) + (m_enableOtherAttached ? 1 : 0))
// {
// }
//
// MarXbotAttachmentDeviceSensor::~MarXbotAttachmentDeviceSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotAttachmentDeviceSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotAttachmentDeviceSensor", this);
// 	params.createParameter(prefix, "enablePosition", (m_enablePosition ? "true" : "false"));
// 	params.createParameter(prefix, "enableStatus", (m_enableStatus ? "true" : "false"));
// 	params.createParameter(prefix, "enableAttached", (m_enableAttached ? "true" : "false"));
// 	params.createParameter(prefix, "enableOtherAttached", (m_enableOtherAttached ? "true" : "false"));
// }
//
// void MarXbotAttachmentDeviceSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The sensor providing the attachment device proprioception", "This sensor provides information about the current status of the attachment device. It provides several inputs (which can be disabled using parameters). The position of the gripper is provided using two neurons (the sin() and the cos() of the angle) to alleviate the problem of 0° and 360° being the same angle with opposite activations.");
// 	d.describeBool("enablePosition").def(true).help("Whether inputs with the position of the attachment device should be enabled or not (default: true)");
// 	d.describeBool("enableStatus").def(true).help("Whether the input with the status of the attachment device should be enabled or not (default: true)");
// 	d.describeBool("enableAttached").def(true).help("Whether the input telling if this robot is attached to another robot should be enabled or not (default: true)");
// 	d.describeBool("enableOtherAttached").def(true).help("Whether the input telling if another robot is attached to this robot should be enabled or not (default: true)");
// }
//
// void MarXbotAttachmentDeviceSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting the motor controller for the attachment device
// 	const MarXbotAttachmentDeviceMotorController *const ctrl = m_robot->attachmentDeviceController();
//
// 	// Now reading values
// 	m_neuronsIterator->setCurrentBlock(name());
// 	if (m_enablePosition) {
// 		const real pos = ctrl->getPosition();
// 		m_neuronsIterator->setInput(sin(pos) * 0.5 + 0.5);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setInput(cos(pos) * 0.5 + 0.5);
// 		m_neuronsIterator->nextNeuron();
// 	}
// 	if (m_enableStatus) {
// 		switch (ctrl->getStatus()) {
// 			case MarXbotAttachmentDeviceMotorController::Open:
// 				m_neuronsIterator->setInput(0.0);
// 				break;
// 			case MarXbotAttachmentDeviceMotorController::HalfClosed:
// 				m_neuronsIterator->setInput(0.5);
// 				break;
// 			case MarXbotAttachmentDeviceMotorController::Closed:
// 				m_neuronsIterator->setInput(1.0);
// 				break;
// 		}
// 		m_neuronsIterator->nextNeuron();
// 	}
// 	if (m_enableAttached) {
// 		if (ctrl->attachedToRobot()) {
// 			m_neuronsIterator->setInput(1.0);
// 		} else {
// 			m_neuronsIterator->setInput(0.0);
// 		}
// 		m_neuronsIterator->nextNeuron();
// 	}
// 	if (m_enableOtherAttached) {
// 		if (ctrl->otherAttachedToUs()) {
// 			m_neuronsIterator->setInput(1.0);
// 		} else {
// 			m_neuronsIterator->setInput(0.0);
// 		}
// 		m_neuronsIterator->nextNeuron();
// 	}
// }
//
// int MarXbotAttachmentDeviceSensor::size()
// {
// 	return m_numInputs;
// }
//
// void MarXbotAttachmentDeviceSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<MarXbot>();
//
// 		// Enabling the attachment device and its motor controller
// 		m_robot->enableAttachmentDevice(true);
// 		m_robot->attachmentDeviceController()->setEnabled(true);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		// Creating labels depending on which inputs are activated
// 		if (m_enablePosition) {
// 			m_neuronsIterator->setGraphicProperties("aps", 0.0, 1.0, Qt::red);
// 			m_neuronsIterator->nextNeuron();
// 			m_neuronsIterator->setGraphicProperties("apc", 0.0, 1.0, Qt::red);
// 			m_neuronsIterator->nextNeuron();
// 		}
// 		if (m_enableStatus) {
// 			m_neuronsIterator->setGraphicProperties("as", 0.0, 1.0, Qt::red);
// 			m_neuronsIterator->nextNeuron();
// 		}
// 		if (m_enableAttached) {
// 			m_neuronsIterator->setGraphicProperties("aa", 0.0, 1.0, Qt::red);
// 			m_neuronsIterator->nextNeuron();
// 		}
// 		if (m_enableOtherAttached) {
// 			m_neuronsIterator->setGraphicProperties("ao", 0.0, 1.0, Qt::red);
// 			m_neuronsIterator->nextNeuron();
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// QString MarXbotWheelSpeedsSensor::modeToString(Mode mode)
// {
// 	switch (mode) {
// 		case AbsoluteMode:
// 			return "Absolute";
// 		case DeltaMode:
// 			return "Delta";
// 		default:
// 			return "Unknown";
// 	}
//
// 	return "UnknownMode";
//
// }
//
// MarXbotWheelSpeedsSensor::Mode MarXbotWheelSpeedsSensor::stringToMode(QString mode)
// {
// 	mode = mode.toUpper();
//
// 	if (mode == "ABSOLUTE") {
// 		return AbsoluteMode;
// 	} else if (mode == "DELTA") {
// 		return DeltaMode;
// 	} else {
// 		return UnknownMode;
// 	}
//
// 	return UnknownMode;
// }
//
// MarXbotWheelSpeedsSensor::MarXbotWheelSpeedsSensor(ConfigurationParameters& params, QString prefix) :
// 	MarXbotSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_mode(stringToMode(ConfigurationHelper::getString(params, prefix + "mode", "Absolute")))
// {
// 	if (m_mode == UnknownMode) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "mode", params.getValue(prefix + "mode"), "The parameter must be one of \"Absolute\" or \"Delta\" (case insensitive)");
// 	}
// }
//
// MarXbotWheelSpeedsSensor::~MarXbotWheelSpeedsSensor()
// {
// 	// Nothing to do here
// }
//
// void MarXbotWheelSpeedsSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	MarXbotSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "MarXbotWheelSpeedsSensor", this);
// 	params.createParameter(prefix, "mode", modeToString(m_mode));
// }
//
// void MarXbotWheelSpeedsSensor::describe(QString type)
// {
// 	// Calling parent function
// 	MarXbotSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The sensor reporting the actual velocity of the wheels of the MarXbot", "This sensor have three modalities, see the mode parameter");
// 	d.describeEnum("mode").def("Absolute").values(QStringList() << "Absolute" << "Delta").help("The modality of the sensor", "The possible modalities are: \"Absolute\" meaning that the senors returns the current velocity of the wheels (scaled between -1 and 1) and \"Delta\", meaning that the sensor returns the absolute value of the difference between the desired velocity and the current velocity. The default value is \"Absolute\"");
// }
//
// void MarXbotWheelSpeedsSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting the motor controller for the attachment device
// 	const WheelMotorController *const ctrl = m_robot->wheelsController();
//
// 	// Setting the current block to update the input neurons
// 	m_neuronsIterator->setCurrentBlock(name());
//
// 	// Now reading values of the current and desired velocity
// 	double curVel1;
// 	double curVel2;
// 	double desVel1;
// 	double desVel2;
// 	double minSpeed1;
// 	double minSpeed2;
// 	double maxSpeed1;
// 	double maxSpeed2;
// 	ctrl->getSpeeds(curVel1, curVel2);
// 	ctrl->getDesiredSpeeds(desVel1, desVel2);
// 	ctrl->getSpeedLimits(minSpeed1, minSpeed2, maxSpeed1, maxSpeed2);
// 	double nCurVel1 = linearMap(curVel1, minSpeed1, maxSpeed1, -1.0, 1.0);
// 	double nCurVel2 = linearMap(curVel2, minSpeed2, maxSpeed2, -1.0, 1.0);
// 	double nDesVel1 = linearMap(desVel1, minSpeed1, maxSpeed1, -1.0, 1.0);
// 	double nDesVel2 = linearMap(desVel2, minSpeed2, maxSpeed2, -1.0, 1.0);
// 	switch (m_mode) {
// 		case AbsoluteMode:
// 			m_neuronsIterator->setInput(applyNoise(nCurVel1, -1.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 			m_neuronsIterator->setInput(applyNoise(nCurVel2, -1.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 			break;
// 		case DeltaMode:
// 			m_neuronsIterator->setInput(applyNoise(min(1.0, fabs(nDesVel1 - nCurVel1)), 0.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 			m_neuronsIterator->setInput(applyNoise(min(1.0, fabs(nDesVel2 - nCurVel2)), 0.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 			break;
// 		default:
// 			// We should never get here
// 			throwUserRuntimeError("Invalid mode for MarXbotWheelSpeedsSensor");
// 			break;
// 	}
// }
//
// int MarXbotWheelSpeedsSensor::size()
// {
// 	return 2;
// }
//
// void MarXbotWheelSpeedsSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	MarXbotSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_marxbotResource) {
// 		m_robot = getResource<MarXbot>();
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		// Creating labels
// 		real minNeuronValue = 0.0;
// 		real maxNeuronValue = 1.0;
// 		switch (m_mode) {
// 			case AbsoluteMode:
// 				minNeuronValue = -1.0;
// 				maxNeuronValue = 1.0;
// 				break;
// 			case DeltaMode:
// 				minNeuronValue = 0.0;
// 				maxNeuronValue = 1.0;
// 				break;
// 			default:
// 				// We should never get here
// 				minNeuronValue = 0.0;
// 				maxNeuronValue = 1.0;
// 				break;
// 		}
// 		m_neuronsIterator->setGraphicProperties("w0", minNeuronValue, maxNeuronValue, Qt::red);
// 		m_neuronsIterator->nextNeuron();
// 		m_neuronsIterator->setGraphicProperties("w1", minNeuronValue, maxNeuronValue, Qt::red);
// 		m_neuronsIterator->nextNeuron();
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// /*FRONT*/
// MarXbotLaserFrontDistanceSensor::MarXbotLaserFrontDistanceSensor(ConfigurationParameters& params, QString prefix) :
//     MarXbotSensor(params, prefix),
//     m_robot(NULL),
//     m_neuronsIterator(NULL),
//     m_sensorLeft(NULL),
//     m_sensorCenter(NULL),
//     m_sensorRight(NULL),
//     m_maxDistance(ConfigurationHelper::getDouble(params, prefix + "maxDistance", 1.0)),
//     m_aperture(ConfigurationHelper::getDouble(params, prefix + "aperture", 60.0)),
//     m_drawSensors(ConfigurationHelper::getBool(params, prefix + "drawSensors", false)),
//     m_drawRays(ConfigurationHelper::getBool(params, prefix + "drawRays", false)),
//     m_drawRaysRange(ConfigurationHelper::getBool(params, prefix + "drawRaysRange", false))
// {
//     // Sanity check
//     if (m_maxDistance <= 0.0f) {
//         ConfigurationHelper::throwUserConfigError(prefix + "maxDistance", params.getValue(prefix + "maxDistance"), "The parameter must be a positive real number");
//     } else if ((m_aperture < 0.0f) || (m_aperture > 360.0f)) {
//         ConfigurationHelper::throwUserConfigError(prefix + "aperture", params.getValue(prefix + "aperture"), "The parameter must be a positive real number smaller than 360");
//     }
// }
//
// MarXbotLaserFrontDistanceSensor::~MarXbotLaserFrontDistanceSensor()
// {
//     delete m_sensorLeft;
//     delete m_sensorCenter;
//     delete m_sensorRight;
// }
//
// void MarXbotLaserFrontDistanceSensor::save(ConfigurationParameters& params, QString prefix)
// {
//     // Calling parent function
//     MarXbotSensor::save(params, prefix);
//
//     // Saving parameters
//     params.startObjectParameters(prefix, "MarXbotLaserFrontDistanceSensor", this);
//     params.createParameter(prefix, "maxDistance", QString::number(m_maxDistance));
//     params.createParameter(prefix, "aperture", QString::number(m_maxDistance));
//     params.createParameter(prefix, "drawSensors", m_drawSensors ? QString("true") : QString("false"));
//     params.createParameter(prefix, "drawRays", m_drawRays ? QString("true") : QString("false"));
//     params.createParameter(prefix, "drawRaysRange", m_drawRaysRange ? QString("true") : QString("false"));
// }
//
// void MarXbotLaserFrontDistanceSensor::describe(QString type)
// {
//     // Calling parent function
//     MarXbotSensor::describe(type);
//
//     // Describing our parameters
//     Descriptor d = addTypeDescription(type, "A frontal distance sensor for long distances", "This is a simple distance sensor in the frontal part of the robot with three rays, at -30��, 0�� and 30�� with respect to the frontal part of the robot. This could be implemented on the real robot using the laser scanner.");
//     d.describeReal("maxDistance").def(1.0).limits(0.0f, +Infinity).help("The maximum distance of the sensors");
//     d.describeReal("aperture").def(60.0).limits(0.0f, 360.0f).help("The sensor aperture in degrees");
//     d.describeBool("drawSensors").def(false).help("Whether to draw the sensors or not");
//     d.describeBool("drawRays").def(false).help("When drawing the sensor, whether to draw the rays or not");
//     d.describeBool("drawRaysRange").def(false).help("When drawing the rays, whether to draw the real range or not");
// }
//
// void MarXbotLaserFrontDistanceSensor::update()
// {
//     // Checking all resources we need exist
//     checkAllNeededResourcesExist();
//
//     // Acquiring the lock to get resources
//     ResourcesLocker locker(this);
//
//     // Setting the current block to update the input neurons
//     m_neuronsIterator->setCurrentBlock(name());
//
//     // Now reading values from the sensor
//     m_sensorLeft->update();
//     m_sensorCenter->update();
//     m_sensorRight->update();
//
//     m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorLeft->getRayCastHit().distance, 0.0, 1.0));
//     m_neuronsIterator->nextNeuron();
//     m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorCenter->getRayCastHit().distance, 0.0, 1.0));
//     m_neuronsIterator->nextNeuron();
//     m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorRight->getRayCastHit().distance, 0.0, 1.0));
//     m_neuronsIterator->nextNeuron();
// }
//
// int MarXbotLaserFrontDistanceSensor::size()
// {
//     return 3;
// }
//
// void MarXbotLaserFrontDistanceSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
//     // Calling parent function
//     MarXbotSensor::resourceChanged(resourceName, changeType);
//
//     if (changeType == Deleted) {
//         return;
//     }
//
//     if (resourceName == m_marxbotResource) {
//         m_robot = getResource<MarXbot>();
//
//         // Deleting the old sensors and creating a new ones
//         delete m_sensorLeft;
//         delete m_sensorCenter;
//         delete m_sensorRight;
//
//         wMatrix centralMtr = wMatrix::pitch(PI_GRECO / 2.0f);
//         centralMtr.w_pos = wVector(0.0, -PhyMarXbot::bodyr, PhyMarXbot::basez + PhyMarXbot::trackradius * 2.0f + PhyMarXbot::treaddepth);
//         m_sensorCenter = new SingleIR(m_robot, centralMtr, 0.02f, m_maxDistance, 0.0f, 1);
//         m_sensorCenter->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         wMatrix mtr = wMatrix::yaw(toRad(m_aperture / 2.0f)) * centralMtr;
//         m_sensorLeft = new SingleIR(m_robot, mtr, 0.02f, m_maxDistance, 0.0f, 1);
//         m_sensorLeft->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         mtr = wMatrix::yaw(toRad(-m_aperture / 2.0f)) * centralMtr;
//         m_sensorRight = new SingleIR(m_robot, mtr, 0.02f, m_maxDistance, 0.0f, 1);
//         m_sensorRight->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//     } else if (resourceName == m_neuronsIteratorResource) {
//         m_neuronsIterator = getResource<NeuronsIterator>();
//         m_neuronsIterator->setCurrentBlock(name());
//
//         // Creating labels
//         m_neuronsIterator->setGraphicProperties("l0", 0.0, 1.0, Qt::red);
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setGraphicProperties("l1", 0.0, 1.0, Qt::red);
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setGraphicProperties("l2", 0.0, 1.0, Qt::red);
//         m_neuronsIterator->nextNeuron();
//     } else {
//         Logger::info("Unknown resource " + resourceName + " for " + name());
//     }
// }
//
// /*OMNI*/
//
// MarXbotLaserOmniDistanceSensor::MarXbotLaserOmniDistanceSensor(ConfigurationParameters& params, QString prefix) :
//     MarXbotSensor(params, prefix),
//     m_robot(NULL),
//     m_neuronsIterator(NULL),
//     m_sensorLeft(NULL),
//     m_sensorCenter(NULL),
//     m_sensorRight(NULL),
//     m_sensorRearLeft(NULL),
//     m_sensorRear(NULL),
//     m_sensorRearRight(NULL),
//     m_maxDistance(ConfigurationHelper::getDouble(params, prefix + "maxDistance", 1.0)),
//     m_drawSensors(ConfigurationHelper::getBool(params, prefix + "drawSensors", false)),
//     m_drawRays(ConfigurationHelper::getBool(params, prefix + "drawRays", false)),
//     m_drawRaysRange(ConfigurationHelper::getBool(params, prefix + "drawRaysRange", false)),
//     m_avgSensors(ConfigurationHelper::getBool(params, prefix + "averageSensors", false))
// {
//     // Sanity check
//     if (m_maxDistance <= 0.0f) {
//         ConfigurationHelper::throwUserConfigError(prefix + "maxDistance", params.getValue(prefix + "maxDistance"), "The parameter must be a positive real number");
//     }
// }
//
// MarXbotLaserOmniDistanceSensor::~MarXbotLaserOmniDistanceSensor()
// {
//     delete m_sensorLeft;
//     delete m_sensorCenter;
//     delete m_sensorRight;
//     delete m_sensorRearLeft;
//     delete m_sensorRear;
//     delete m_sensorRearRight;
// }
//
// void MarXbotLaserOmniDistanceSensor::save(ConfigurationParameters& params, QString prefix)
// {
//     // Calling parent function
//     MarXbotSensor::save(params, prefix);
//
//     // Saving parameters
//     params.startObjectParameters(prefix, "MarXbotLaserOmniDistanceSensor", this);
//     params.createParameter(prefix, "maxDistance", QString::number(m_maxDistance));
//     params.createParameter(prefix, "drawSensors", m_drawSensors ? QString("true") : QString("false"));
//     params.createParameter(prefix, "drawRays", m_drawRays ? QString("true") : QString("false"));
//     params.createParameter(prefix, "drawRaysRange", m_drawRaysRange ? QString("true") : QString("false"));
//     params.createParameter(prefix, "averageSensors", m_avgSensors? QString("true") : QString("false"));
// }
//
// void MarXbotLaserOmniDistanceSensor::describe(QString type)
// {
//     // Calling parent function
//     MarXbotSensor::describe(type);
//
//     // Describing our parameters
//     Descriptor d = addTypeDescription(type, "An omnidirection distance sensor for long distances", "This is a simple distance sensor in the frontal and rear part of the robot with six rays, each 60° starting in the frontal part of the robot. This could be implemented on the real robot using the laser scanner.");
//     d.describeReal("maxDistance").def(1.0).limits(0.0f, +Infinity).help("The maximum distance of the sensors");
//     d.describeBool("drawSensors").def(false).help("Whether to draw the sensors or not");
//     d.describeBool("drawRays").def(false).help("When drawing the sensor, whether to draw the rays or not");
//     d.describeBool("drawRaysRange").def(false).help("When drawing the rays, whether to draw the real range or not");
//     d.describeBool("averageSensors").def(false).help("Whether the sensors should be averaged");
// }
//
// void MarXbotLaserOmniDistanceSensor::update()
// {
//     // Checking all resources we need exist
//     checkAllNeededResourcesExist();
//
//     // Acquiring the lock to get resources
//     ResourcesLocker locker(this);
//
//     // Setting the current block to update the input neurons
//     m_neuronsIterator->setCurrentBlock(name());
//
//     // Now reading values from the sensor
//     m_sensorLeft->update();
//     m_sensorCenter->update();
//     m_sensorRight->update();
//     m_sensorRearLeft->update();
//     m_sensorRear->update();
//     m_sensorRearRight->update();
//
//
//     if(m_avgSensors){
//         double avg = 0.0;
//         avg += m_sensorLeft->getRayCastHit().distance;
//         avg += m_sensorCenter->getRayCastHit().distance;
//         avg += m_sensorRight->getRayCastHit().distance;
//         avg += m_sensorRearLeft->getRayCastHit().distance;
//         avg += m_sensorRear->getRayCastHit().distance;
//         avg += m_sensorRearRight->getRayCastHit().distance;
//         avg /= 6.0;
//         m_neuronsIterator->setInput(applyNoise(1.0 - avg, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//     }else{
//         m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorLeft->getRayCastHit().distance, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorCenter->getRayCastHit().distance, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorRight->getRayCastHit().distance, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorRearLeft->getRayCastHit().distance, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorRear->getRayCastHit().distance, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//         m_neuronsIterator->setInput(applyNoise(1.0 - m_sensorRearRight->getRayCastHit().distance, 0.0, 1.0));
//         m_neuronsIterator->nextNeuron();
//     }
// }
//
// int MarXbotLaserOmniDistanceSensor::size()
// {
//     if(m_avgSensors)
//         return 1;
//     else
//         return 6;
// }
//
// void MarXbotLaserOmniDistanceSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
//     // Calling parent function
//     MarXbotSensor::resourceChanged(resourceName, changeType);
//
//     if (changeType == Deleted) {
//         return;
//     }
//
//
//     if (resourceName == m_marxbotResource) {
//         m_robot = getResource<MarXbot>();
//
//         // Deleting the old sensors and creating a new ones
//         delete m_sensorLeft;
//         delete m_sensorCenter;
//         delete m_sensorRight;
//         delete m_sensorRearLeft;
//         delete m_sensorRear;
//         delete m_sensorRearRight;
//
//         wMatrix centralMtr = wMatrix::pitch(PI_GRECO / 2.0f);
//         centralMtr.w_pos = wVector(0.0, 0.0, PhyMarXbot::basez + PhyMarXbot::trackradius * 2.0f + PhyMarXbot::treaddepth);
//         m_sensorCenter = new SingleIR(m_robot, centralMtr, PhyMarXbot::bodyr+0.008f, m_maxDistance, 0.0f, 1);
//         m_sensorCenter->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         wMatrix mtr = wMatrix::yaw(toRad(60.0)) * centralMtr;
//         m_sensorLeft = new SingleIR(m_robot, mtr, PhyMarXbot::bodyr+0.005f, m_maxDistance, 0.0f, 1);
//         m_sensorLeft->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         mtr = wMatrix::yaw(toRad(120.0)) * centralMtr;
//         m_sensorRearLeft = new SingleIR(m_robot, mtr, PhyMarXbot::bodyr+0.005f, m_maxDistance, 0.0f, 1);
//         m_sensorRearLeft->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         mtr = wMatrix::yaw(toRad(180.0)) * centralMtr;
//         m_sensorRear= new SingleIR(m_robot, mtr, PhyMarXbot::bodyr+0.005f, m_maxDistance, 0.0f, 1);
//         m_sensorRear->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         mtr = wMatrix::yaw(toRad(240)) * centralMtr;
//         m_sensorRearRight = new SingleIR(m_robot, mtr, PhyMarXbot::bodyr+0.005f, m_maxDistance, 0.0f, 1);
//         m_sensorRearRight->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//         mtr = wMatrix::yaw(toRad(300.0)) * centralMtr;
//         m_sensorRight = new SingleIR(m_robot, mtr, PhyMarXbot::bodyr+0.005f, m_maxDistance, 0.0f, 1);
//         m_sensorRight->setGraphicalProperties(m_drawSensors, m_drawRays, m_drawRaysRange);
//
//
//
//
//
//     } else if (resourceName == m_neuronsIteratorResource) {
//         m_neuronsIterator = getResource<NeuronsIterator>();
//         m_neuronsIterator->setCurrentBlock(name());
//
//         if(m_avgSensors){ //if average sensor
//             m_neuronsIterator->setGraphicProperties("AvgLR", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//         }else{
//             // Creating labels
//             m_neuronsIterator->setGraphicProperties("l0", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//             m_neuronsIterator->setGraphicProperties("l1", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//             m_neuronsIterator->setGraphicProperties("l2", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//             m_neuronsIterator->setGraphicProperties("l3", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//             m_neuronsIterator->setGraphicProperties("l4", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//             m_neuronsIterator->setGraphicProperties("l5", 0.0, 1.0, Qt::red);
//             m_neuronsIterator->nextNeuron();
//         }
//     } else {
//         Logger::info("Unknown resource " + resourceName + " for " + name());
//     }
// }
//
//
// }
