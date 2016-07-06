/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#include "kheperasensors.h"
#include "configurationhelper.h"
#include "logger.h"
#include "graphicalmarkers.h"
#include "arena.h"
#include "utilitiesexceptions.h"

namespace farsa {

KheperaSensor::KheperaSensor(ConfigurationManager& params)
	: AbstractControllerInput(params)
	, m_kheperaResource(ConfigurationHelper::getString(configurationManager(), confPath() + "khepera"))
{
	// Declaring the resources that are needed here
	addNotifiedResource(m_kheperaResource);
}

KheperaSensor::~KheperaSensor()
{
}

void KheperaSensor::describe(RegisteredComponentDescriptor& d)
{
	AbstractControllerInput::describe(d);

	d.help("The base class for Khepera sensors");

	d.describeString("khepera").def("robot").help("The name of the resource associated with the khepera robot to use (default is \"robot\")");
}

// KheperaProximityIRSensor::KheperaProximityIRSensor(ConfigurationParameters& params, QString prefix) :
// 	KheperaSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL)
// {
// }
//
// KheperaProximityIRSensor::~KheperaProximityIRSensor()
// {
// 	// Nothing to do here
// }
//
// void KheperaProximityIRSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	KheperaSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "KheperaProximityIRSensor", this);
// }
//
// void KheperaProximityIRSensor::describe(QString type)
// {
// 	// Calling parent function
// 	KheperaSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The infrared proximity sensors of the Khepera robot", "The infrared proximity sensors of the Khepera II robot. These are the very short range IR sensors all around the base");
// }
//
// void KheperaProximityIRSensor::update()
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
// int KheperaProximityIRSensor::size()
// {
// 	return 8;
// }
//
// void KheperaProximityIRSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	KheperaSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_kheperaResource) {
// 		m_robot = getResource<PhyKhepera>();
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

KheperaSampledProximityIRSensor::KheperaSampledProximityIRSensor(ConfigurationManager& params)
	: KheperaSensor(params)
	, m_robot(NULL)
	, m_arena(NULL)
	, m_activeSensors(ConfigurationHelper::decodeListOfBools(ConfigurationHelper::getString(configurationManager(), confPath() + "activeSensors")))
	, m_numActiveSensors(m_activeSensors.count(true))
	, m_roundSamples(ConfigurationHelper::getString(configurationManager(), confPath() + "roundSamples"))
	, m_smallSamples(ConfigurationHelper::getString(configurationManager(), confPath() + "smallSamples"))
	, m_wallSamples(ConfigurationHelper::getString(configurationManager(), confPath() + "wallSamples"))
{
	if (m_activeSensors.size() != 8) {
		ConfigurationHelper::throwUserConfigError(confPath() + "activeSensors", params.getValue(confPath() + "activeSensors"), "The parameter must be a list of exactly 8 elements either equal to 1 or to 0 (do not use any space to separate elements, just put them directly one after the other)");
	}

	// Checking that the sampled files have the right number of IR sensors
	if (m_roundSamples.numIR() != 8) {
		ConfigurationHelper::throwUserConfigError(confPath() + "roundSamples", m_roundSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_roundSamples.numIR()));
	}
	if (m_smallSamples.numIR() != 8) {
		ConfigurationHelper::throwUserConfigError(confPath() + "smallSamples", m_smallSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_smallSamples.numIR()));
	}
	if (m_wallSamples.numIR() != 8) {
		ConfigurationHelper::throwUserConfigError(confPath() + "wallSamples", m_wallSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_wallSamples.numIR()));
	}

	// Here we also need the arena to work
	addNotifiedResource("arena");
}

KheperaSampledProximityIRSensor::~KheperaSampledProximityIRSensor()
{
}

void KheperaSampledProximityIRSensor::describe(RegisteredComponentDescriptor& d)
{
	// Calling parent function
	KheperaSensor::describe(d);

	// Describing our parameters
	d.help("The sampled proximity infrared sensors of the Khepera", "This is the sampled version of the proximity infrared sensors of the Khepera. This sensor only works with objects created using the Arena");

	d.describeString("activeSensors").def("11111111").help("Which IR sensors of the robot are actually enabled", "This is a string of exactly 8 elements. Each element can be either \"0\" or \"1\" to respectively disable/enable the corresponding proximity IR sensor. The first sensor is the one on the left side of the robot and the others follow counterclockwise (i.e. left, back, right, front)");
	d.describeString("roundSamples").def("round.sam").help("The name of the file with samples for big round objects");
	d.describeString("smallSamples").def("small.sam").help("The name of the file with samples for small round objects");
	d.describeString("wallSamples").def("wall.sam").help("The name of the file with samples for walls");
}

int KheperaSampledProximityIRSensor::size() const
{
	return m_numActiveSensors;
}

void KheperaSampledProximityIRSensor::iteratorChanged(AbstractControllerInputIterator*)
{
	// We use the actual sensor ID in the neuron name
	for (int i = 0; i < m_activeSensors.size(); i++) {
		if (m_activeSensors[i]) {
			it()->setProperties("ir" + QString::number(i), 0.0, 1.0, Qt::red);
			it()->next();
		}
	}
}

void KheperaSampledProximityIRSensor::updateCalled()
{
	// Getting the list of objects in the arena
	const QVector<PhyObject2DWrapper*>& objectsList = m_arena->getObjects();

	// Preparing the vector with activations and setting all values to 0
	QVector<real> activations(m_activeSensors.size(), 0.0);

	// Cycling through the list of objects. We first need to get the current position and orientation of the robot
	//const wVector robotPos = m_robot->position();
	//const real robotAng = m_robot->orientation(m_arena->getPlane());
	foreach(const PhyObject2DWrapper* obj, objectsList) {
		// Computing angle and distance. We don't need to remove the robot to which this sensor belongs because
		// the calculatations will give a negative distance
		double distance;
		double angle;

		// If computeDistanceAndOrientationFromRobot returns false, we have to discard this object
		Arena::RobotResource robotResource(m_kheperaResource, getResourceAndOwner<Khepera>(m_kheperaResource).owner);
		if (!obj->computeDistanceAndOrientationFromRobot(*(m_arena->getRobotWrapper(robotResource)), distance, angle)) {
			continue;
		}

		// Getting the activation. The switch is to understand which samples to use
		QVector<unsigned int>::const_iterator actIt = QVector<unsigned int>::const_iterator();
		switch (obj->type()) {
			case PhyObject2DWrapper::Wall:
				actIt = m_wallSamples.getActivation(distance, angle);
				break;
			case PhyObject2DWrapper::SmallCylinder:
				actIt = m_smallSamples.getActivation(distance, angle);
				break;
			case PhyObject2DWrapper::BigCylinder:
				actIt = m_roundSamples.getActivation(distance, angle);
				break;
			case PhyObject2DWrapper::WheeledRobot:
				actIt = m_roundSamples.getActivation(distance, angle);
				break;
			default:
				//Logger::warning("The sampled infrared sensor only works with Small Cylinders, Big Cylinders, Walls and other Robots");
				continue;
		}

		// Adding activations in the activations vector
		for (int i = 0; i < activations.size(); ++i, ++actIt) {
			activations[i] = min(1.0, activations[i] + (real(*actIt) / 1024.0));
		}
	}

	// Finally activating neurons
	for (int i = 0; i < m_activeSensors.size(); i++) {
		if (m_activeSensors[i]) {
			it()->setInput(activations[i]);
			it()->next();
		}
	}
}

void KheperaSampledProximityIRSensor::resourceChanged(QString name, Component*, ResourceChangeType changeType)
{
	if (name == m_kheperaResource) {
		if ((changeType == ResourceDeleted) || (changeType == ResourceDeclaredAsNull)) {
			m_robot = NULL;
		} else {
			m_robot = getResource<Khepera>();

			// Disabling proximity IR sensors, they are not used here
			m_robot->proximityIRSensorController()->setEnabled(false);
		}
	} else if (name == "arena") {
		if ((changeType == ResourceDeleted) || (changeType == ResourceDeclaredAsNull)) {
			m_arena = NULL;
		} else {
			m_arena = getResource<Arena>();
		}
	} else {
		Logger::info("Unknown resource " + name + " for KheperaSampledProximityIRSensor in configuration path \"" + confPath() + "\"");
	}
}

// KheperaSampledLightSensor::KheperaSampledLightSensor(ConfigurationParameters& params, QString prefix) :
// 	KheperaSensor(params, prefix),
// 	m_robot(NULL),
// 	m_arena(NULL),
// 	m_neuronsIterator(NULL),
// 	m_activeSensors(ConfigurationHelper::getBoolVector(params, prefix + "activeSensors", "11111111")),
// 	m_numActiveSensors(m_activeSensors.count(true)),
// 	m_lightSamples(ConfigurationHelper::getString(params, prefix + "lightSamples", "light.sam"))
// {
// 	if (m_activeSensors.size() != 8) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "activeSensors", params.getValue(prefix + "activeSensors"), "The parameter must be a list of exactly 8 elements either equal to 1 or to 0 (do not use any space to separate elements, just put them directly one after the other)");
// 	}
//
// 	// Checking that the sampled files have the right number of IR sensors
// 	if (m_lightSamples.numIR() != 8) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "lightSamples", m_lightSamples.filename(), "The file has samples for the wrong number of sensors, expected 8, got " + QString::number(m_lightSamples.numIR()));
// 	}
//
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// KheperaSampledLightSensor::~KheperaSampledLightSensor()
// {
// 	// Nothing to do here
// }
//
// void KheperaSampledLightSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	KheperaSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "KheperaSampledLightSensor", this);
// 	QString activeSensorsString;
// 	for (int i = 0; i < m_activeSensors.size(); i++) {
// 		activeSensorsString += (m_activeSensors[i] ? "1" : "0");
// 	}
// 	params.createParameter(prefix, "activeSensors", activeSensorsString);
// 	params.createParameter(prefix, "lightSamples", m_lightSamples.filename());
// }
//
// void KheperaSampledLightSensor::describe(QString type)
// {
// 	// Calling parent function
// 	KheperaSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The sampled light sensors of the Khepera", "This is the sampled version of the light sensors of the Khepera. This sensor only works with objects created using the Arena");
// 	d.describeString("activeSensors").def("11111111").help("Which light sensors of the robot are actually enabled", "This is a string of exactly 8 elements. Each element can be either \"0\" or \"1\" to respectively disable/enable the corresponding light sensor. The first sensor is the one on the left side of the robot and the others follow counterclockwise (i.e. left, back, right, front)");
// 	d.describeString("lightSamples").def("light.sam").help("The name of the file with samples for light sensors");
// }
//
// void KheperaSampledLightSensor::update()
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
// 		// If the object is not a LightBulb, discarding it
// 		if (obj->type() != PhyObject2DWrapper::LightBulb) {
// 			continue;
// 		}
//
// 		// Computing angle and distance
// 		double distance;
// 		double angle;
//
// 		// If computeDistanceAndOrientationFromRobot returns false, we have to discard this object
// 		if (!obj->computeDistanceAndOrientationFromRobot(*(m_arena->getRobotWrapper(m_kheperaResource)), distance, angle)) {
// 			continue;
// 		}
//
// 		// Getting the activation (we already checked that the object is a LightBulb)
// 		QVector<unsigned int>::const_iterator actIt = m_lightSamples.getActivation(distance, angle);
//
// 		// Adding activations in the activations vector
// 		for (int i = 0; i < activations.size(); ++i, ++actIt) {
// 			activations[i] = min(1.0, activations[i] + (real(*actIt) / 1024.0));
// 		}
// 	}
//
// 	// Finally activating neurons
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < m_activeSensors.size(); i++) {
// 		if (m_activeSensors[i]) {
// 			m_neuronsIterator->setInput(applyNoise(activations[i], 0.0, 1.0));
// 			m_neuronsIterator->nextNeuron();
// 		}
// 	}
// }
//
// int KheperaSampledLightSensor::size()
// {
// 	return m_numActiveSensors;
// }
//
// void KheperaSampledLightSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	KheperaSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_kheperaResource) {
// 		m_robot = getResource<Khepera>();
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		// We use the actual sensor ID in the neuron name
// 		for (int i = 0; i < m_activeSensors.size(); i++) {
// 			if (m_activeSensors[i]) {
// 				m_neuronsIterator->setGraphicProperties("l" + QString::number(i), 0.0, 1.0, Qt::red);
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
// KheperaGroundSensor::KheperaGroundSensor(ConfigurationParameters& params, QString prefix) :
// 	KheperaSensor(params, prefix),
// 	m_robot(NULL),
// 	m_arena(NULL),
// 	m_neuronsIterator(NULL)
//
// {
// 	// Here we also need the arena to work
// 	addUsableResource("arena");
// }
//
// KheperaGroundSensor::~KheperaGroundSensor()
// {
// 	// Nothing to do here
// }
//
// void KheperaGroundSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	KheperaSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "KheperaGroundSensor", this);
//
// }
//
// void KheperaGroundSensor::describe(QString type)
// {
// 	// Calling parent function
// 	KheperaSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The ground sensor of the khepera robot", "The ground sensors of the khepera robot. It is an infrared sensor that can be mounted on the bottom part of the robot.");
// }
//
// void KheperaGroundSensor::update()
// {
// 	// Checking all resources we need exist
// 	checkAllNeededResourcesExist();
//
// 	// Acquiring the lock to get resources
// 	ResourcesLocker locker(this);
//
// 	// Getting the color under the robot
// 	const QColor color = getColorAtArenaGroundPosition(m_arena, m_robot->position());
// 	const real actv = 1.0 - real(qGray(color.rgb())) / 255.0;
//
// 	// Finally activating neurons
// 	m_neuronsIterator->setCurrentBlock(name());
// 	for (int i = 0; i < size(); i++) {
// 		m_neuronsIterator->setInput(applyNoise(actv, 0.0, 1.0));
// 		m_neuronsIterator->nextNeuron();
// 	}
// }
//
// int KheperaGroundSensor::size()
// {
// 	return 1;
// }
//
// void KheperaGroundSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	KheperaSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_kheperaResource) {
// 		m_robot = getResource<Khepera>();
//
//
// 	} else if (resourceName == m_neuronsIteratorResource) {
// 		m_neuronsIterator = getResource<NeuronsIterator>();
// 		m_neuronsIterator->setCurrentBlock(name());
//
// 		// We use the actual sensor ID in the neuron name
// 		for (int i = 0; i < size(); i++) {
// 				m_neuronsIterator->setGraphicProperties("g" + QString::number(i), 0.0, 1.0, Qt::red);
// 				m_neuronsIterator->nextNeuron();
// 		}
// 	} else if (resourceName == "arena") {
// 		// Storing the pointer to the arena
// 		m_arena = getResource<Arena>();
// 	} else {
// 		Logger::info("Unknown resource " + resourceName + " for " + name());
// 	}
// }
//
// QString KheperaWheelSpeedsSensor::modeToString(Mode mode)
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
// KheperaWheelSpeedsSensor::Mode KheperaWheelSpeedsSensor::stringToMode(QString mode)
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
// KheperaWheelSpeedsSensor::KheperaWheelSpeedsSensor(ConfigurationParameters& params, QString prefix) :
// 	KheperaSensor(params, prefix),
// 	m_robot(NULL),
// 	m_neuronsIterator(NULL),
// 	m_mode(stringToMode(ConfigurationHelper::getString(params, prefix + "mode", "Absolute")))
// {
// 	if (m_mode == UnknownMode) {
// 		ConfigurationHelper::throwUserConfigError(prefix + "mode", params.getValue(prefix + "mode"), "The parameter must be one of \"Absolute\" or \"Delta\" (case insensitive)");
// 	}
// }
//
// KheperaWheelSpeedsSensor::~KheperaWheelSpeedsSensor()
// {
// 	// Nothing to do here
// }
//
// void KheperaWheelSpeedsSensor::save(ConfigurationParameters& params, QString prefix)
// {
// 	// Calling parent function
// 	KheperaSensor::save(params, prefix);
//
// 	// Saving parameters
// 	params.startObjectParameters(prefix, "KheperaWheelSpeedsSensor", this);
// 	params.createParameter(prefix, "mode", modeToString(m_mode));
// }
//
// void KheperaWheelSpeedsSensor::describe(QString type)
// {
// 	// Calling parent function
// 	KheperaSensor::describe(type);
//
// 	// Describing our parameters
// 	Descriptor d = addTypeDescription(type, "The sensor reporting the actual velocity of the wheels of the Khepera", "This sensor have three modalities, see the mode parameter");
// 	d.describeEnum("mode").def("Absolute").values(QStringList() << "Absolute" << "Delta").help("The modality of the sensor", "The possible modalities are: \"Absolute\" meaning that the senors returns the current velocity of the wheels (scaled between -1 and 1) and \"Delta\", meaning that the sensor returns the absolute value of the difference between the desired velocity and the current velocity. The default value is \"Absolute\"");
// }
//
// void KheperaWheelSpeedsSensor::update()
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
// 			throwUserRuntimeError("Invalid mode for KheperaWheelSpeedsSensor");
// 			break;
// 	}
// }
//
// int KheperaWheelSpeedsSensor::size()
// {
// 	return 2;
// }
//
// void KheperaWheelSpeedsSensor::resourceChanged(QString resourceName, ResourceChangeType changeType)
// {
// 	// Calling parent function
// 	KheperaSensor::resourceChanged(resourceName, changeType);
//
// 	if (changeType == Deleted) {
// 		return;
// 	}
//
// 	if (resourceName == m_kheperaResource) {
// 		m_robot = getResource<PhyKhepera>();
//
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

}
