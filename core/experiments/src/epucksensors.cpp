// /********************************************************************************
//  *  SALSA Experiments Library                                                   *
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
// #include "epucksensors.h"
// #include "configurationhelper.h"
// #include "logger.h"
// #include "graphicalwobject.h"
// #include "arena.h"
// #include <limits>
//
// namespace salsa {
//
// EpuckSensor::EpuckSensor(ConfigurationParameters& params, QString prefix) :
// 	Sensor(params, prefix),
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
// EpuckSensor::~EpuckSensor()
// {
// 	// Nothing to do here
// }
//
// void EpuckSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	Sensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckSensor", this);
// 	params.createParameter(prefix, "epuck", m_epuckResource);
// 	params.createParameter(prefix, "neuronsIterator", m_neuronsIteratorResource);
// }
//
// void EpuckSensor::describe(QString type)
// {
// 	// Calling parent function
// 	Sensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The base class for e-puck sensors");
// 	d.describeString("epuck").def("robot").help("the name of the resource associated with the e-puck robot to use (default is \"robot\")");
// 	d.describeString("neuronsIterator").def("neuronsIterator").help("the name of the resource associated with the neural network iterator (default is \"neuronsIterator\")");
// }
//
// void EpuckSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
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
// EpuckProximityIRSensor::EpuckProximityIRSensor(ConfigurationParameters& params, QString prefix) :
// 	EpuckSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL)
// {
// }
//
// EpuckProximityIRSensor::~EpuckProximityIRSensor()
// {
// 	// Nothing to do here
// }
//
// void EpuckProximityIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	EpuckSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckProximityIRSensor", this);
// }
//
// void EpuckProximityIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	EpuckSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The infrared proximity sensors of the e-puck robot", "The infrared proximity sensors of the e-puck robot. These are the very short range IR sensors all around the base");
// }
//
// void EpuckProximityIRSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 		m_neuronsIterator->setInput(applyNoise(m_robot->proximityIRSensorController()->activation(i), 0.0, 1.0));
// 	}
// }
//
// int EpuckProximityIRSensor::size()
// {
// 	return 8;
// }
//
// void EpuckProximityIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	EpuckSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_epuckResource) {
// 		m_robot = getResource<PhyEpuck>();
//
// 		// Eanbling sensors
// 		m_robot->proximityIRSensorController()->setEnabled(true);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("ir" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// EpuckGroundIRSensor::EpuckGroundIRSensor(ConfigurationParameters& params, QString prefix) :
// 	EpuckSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_arena(NULL)
// {
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// EpuckGroundIRSensor::~EpuckGroundIRSensor()
// {
// 	// Nothing to do here
// }
//
// void EpuckGroundIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	EpuckSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckGroundIRSensor", this);
// }
//
// void EpuckGroundIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	EpuckSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The infrared ground sensors of the e-puck robot", "The infrared ground sensors of the e-puck robot. These are the three ground sensors in the frontal part of the robot.");
// }
//
// void EpuckGroundIRSensor::update()
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
// 		const wVector sensorPosition = m_robot->matrix().transformVector(m_robot->groundIRSensorController()->sensors()[i].getPosition());
// 		const QColor color = getColorAtArenaGroundPosition(m_arena, sensorPosition);
// 		const real actv = real(qGray(color.rgb())) / 255.0;
//
// 		m_neuronsIterator->setInput(applyNoise(actv, 0.0, 1.0));
// 	}
// }
//
// int EpuckGroundIRSensor::size()
// {
// 	return 3;
// }
//
// void EpuckGroundIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	EpuckSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_epuckResource) {
// 		m_robot = getResource<PhyEpuck>();
//
// 		// Eanbling sensors
// 		m_robot->groundIRSensorController()->setEnabled(true);
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
// EpuckLinearCameraSensor::EpuckLinearCameraSensor(ConfigurationParameters& params, QString prefix) :
// 	EpuckSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_numReceptors(ConfigurationHelper::getInt(params, prefix + "numReceptors", 8)),
// 	m_aperture(ConfigurationHelper::getDouble(params, prefix + "aperture", 360.0f)),
// 	m_camera(NULL),
// 	m_drawCamera(ConfigurationHelper::getBool(params, prefix + "drawCamera", true)),
// 	m_ignoreWalls(ConfigurationHelper::getBool(params, prefix + "ignoreWalls", false))
// {
// }
//
// EpuckLinearCameraSensor::~EpuckLinearCameraSensor()
// {
// 	// Deleting the camera
// 	delete m_camera;
// }
//
// void EpuckLinearCameraSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	EpuckSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckLinearCameraSensor", this);
// 	params.createParameter(prefix, "numReceptors", QString::number(m_numReceptors));
// 	params.createParameter(prefix, "aperture", QString::number(m_aperture));
// 	params.createParameter(prefix, "drawCamera", (m_drawCamera ? "true" : "false"));
// 	params.createParameter(prefix, "ignoreWalls", (m_ignoreWalls ? "true" : "false"));
// }
//
// void EpuckLinearCameraSensor::describe(QString type)
// {
// 	// Calling parent function
// 	EpuckSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The linear camera sensor of the e-puck robot", "This is a linear camera with configurable aperture");
// 	d.describeInt("numReceptors").def(8).limits(1, MaxInteger).help("The number of receptors of the sensor", "Each receptor returns three values, one for each of the three colors (red, green, blue). This means that the size returned by this sensor is 3 * numReceptors (default is 8)");
// 	d.describeReal("aperture").def(360.0f).limits(0.0f, 360.0f).help("The aperture of the camera in degrees", "The real e-puck has a camera with an aperture of 36 degrees, but here you can use any value up to 360° (default is 36)");
// 	d.describeBool("drawCamera").def(true).help("Whether to draw the camera or not");
// 	d.describeBool("ignoreWalls").def(false).help("whether to perceive walls or not (default is false)");
// }
//
// void EpuckLinearCameraSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker( this );
//
// 	// Updating the camera
// 	m_camera->update();
//
// 	// Reading activations: first the red one, then the green one and finally the blue one
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 		m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).redF(), 0.0, 1.0));
// 	}
// 	for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 		m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).greenF(), 0.0, 1.0));
// 	}
// 	for (int i = 0; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 		m_neuronsIterator->setInput(applyNoise(m_camera->colorForReceptor(i).blueF(), 0.0, 1.0));
// 	}
// }
//
// int EpuckLinearCameraSensor::size()
// {
// 	return m_numReceptors * 3;
// }
//
// void EpuckLinearCameraSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	EpuckSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		// Deleting the camera if the robot was deleted
// 		if (resourceName == m_epuckResource) {
// 			delete m_camera;
// 			m_camera = NULL;
// 		}
//
// 		return;
// 	}
//
// 	if (resourceName == m_epuckResource) {
// 		m_robot = getResource<PhyEpuck>();
//
// 		// Now we can also create the camera
// 		wMatrix mtr = wMatrix::roll(-PI_GRECO / 2.0);
// #if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
// 	#warning QUI INVECE DI UNA COSTANTE, CALCOLARSI UNA POSIZIONE DALLE DIMENSIONI DEL ROBOT
// #endif
// 		mtr.w_pos.z = 0.06f;
// 		m_camera = new LinearCamera(m_robot, mtr, toRad(m_aperture), m_numReceptors, std::numeric_limits<double>::infinity(), Qt::black);
//
// 		// Sharing resources with the camera
// 		m_camera->shareResourcesWith(this);
// 		m_camera->drawCamera(m_drawCamera);
// 		m_camera->ignoreWalls(m_ignoreWalls);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		int i = 0;
// 		for (; i < m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("lr" + QString::number(i % m_numReceptors), 0.0, 1.0, Qt::red);
// 		}
// 		for (; i < 2 * m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("lg" + QString::number(i % m_numReceptors), 0.0, 1.0, Qt::red);
// 		}
// 		for (; i < 3 * m_numReceptors; i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("lb" + QString::number(i % m_numReceptors), 0.0, 1.0, Qt::red);
// 		}
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// EpuckSampledProximityIRSensor::EpuckSampledProximityIRSensor(ConfigurationParameters& params, QString prefix) :
// 	EpuckSensor(params, prefix),
// 	m_robot(NULL),
// 	m_arena(NULL),
// 	m_neuronsIterator(NULL),
// 	m_roundSamples(ConfigurationHelper::getString(params, prefix + "roundSamples", "round.sam")),
// 	m_smallSamples(ConfigurationHelper::getString(params, prefix + "smallSamples", "small.sam")),
// 	m_wallSamples(ConfigurationHelper::getString(params, prefix + "wallSamples", "wall.sam"))
// {
// 	// Checking that the sampled files have the right number of IR sensors
// 	if (m_roundSamples.numIR() != 8) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "roundSamples", m_roundSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_roundSamples.numIR()));
// 	}
// 	if (m_smallSamples.numIR() != 8) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "smallSamples", m_smallSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_smallSamples.numIR()));
// 	}
// 	if (m_wallSamples.numIR() != 8) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "wallSamples", m_wallSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_wallSamples.numIR()));
// 	}
//
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// EpuckSampledProximityIRSensor::~EpuckSampledProximityIRSensor()
// {
// 	// Nothing to do here
// }
//
// void EpuckSampledProximityIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	EpuckSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "EpuckSampledProximityIRSensor", this);
// 	params.createParameter(prefix, "roundSamples", m_roundSamples.filename());
// 	params.createParameter(prefix, "smallSamples", m_smallSamples.filename());
// 	params.createParameter(prefix, "wallSamples", m_wallSamples.filename());
// }
//
// void EpuckSampledProximityIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	EpuckSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The sampled proximity infrared sensors of the e-puck", "This is the sampled version of the proximity infrared sensors of the e-puck. This sensor only works with objects created using the Arena");
// 	d.describeString("roundSamples").def("round.sam").help("The name of the file with samples for big round objects");
// 	d.describeString("smallSamples").def("small.sam").help("The name of the file with samples for small round objects");
// 	d.describeString("wallSamples").def("wall.sam").help("The name of the file with samples for walls");
// }
//
// void EpuckSampledProximityIRSensor::update()
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
// 	QVector<real> activations(size(), 0.0);
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
// 		if (!obj->computeDistanceAndOrientationFromRobot(*(m_arena->getRobotWrapper(m_epuckResource)), distance, angle)) {
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
// 		for (int i = 0; i < size(); ++i, ++actIt) {
// 			activations[i] = min(1.0, activations[i] + (real(*actIt) / 1024.0));
// 		}
// 	}
//
// 	// Finally activating neurons
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 		m_neuronsIterator->setInput(applyNoise(activations[i], 0.0, 1.0));
// 	}
// }
//
// int EpuckSampledProximityIRSensor::size()
// {
// 	return 8;
// }
//
// void EpuckSampledProximityIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	EpuckSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_epuckResource) {
// 		m_robot = getResource<Epuck>();
//
// 		// Disabling proximity IR sensors, they are not used here
// 		m_robot->proximityIRSensorController()->setEnabled(false);
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
// 		for (int i = 0; i < size(); i++, m_neuronsIterator->nextNeuron()) {
// 			m_neuronsIterator->setGraphicProperties("ir" + QString::number(i), 0.0, 1.0, Qt::red);
// 		}
// 	} else if (resourceName == "arena") {
// 		// Storing the pointer to the arena
// 		m_arena = getResource<Arena>();
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// }
