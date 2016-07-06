/********************************************************************************
 *  FARSA Experimentes Library                                                  *
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

#include "arena.h"
#include "abstracttest.h"
#include "baseexperiment.h"
#include "controller.h"
#include "controllerinputoutput.h"
#include "controlleriterator.h"
#include "component.h"
#include "epuckmotors.h"
#include "epucksensors.h"
#include "evoga.h"
#include "evonet.h"
#include "evorobotcomponent.h"
#include "evorobotexperiment.h"
#include "experimentsconfig.h"
#include "kheperamotors.h"
#include "kheperasensors.h"
#include "logger.h"
#include "marxbotmotors.h"
#include "marxbotsensors.h"
#include "motors.h"
#include "noisydevice.h"
#include "robots.h"
#include "sensors.h"
#include "tests.h"
#include "typesdb.h"

namespace farsa {

bool FARSA_EXPERIMENTS_API initExperimentsLib()
{
	static bool calledOnce = false;

	if (calledOnce) {
		return true;
	}
	calledOnce = true;

	// Registering the controller and related interfaces
	TypesDB::instance().registerType<Controller>("Controller", QStringList() << "Component");
	TypesDB::instance().registerType<ControllerInputsList>("ControllerInputsList", QStringList());
	TypesDB::instance().registerType<ControllerOutputsList>("ControllerOutputsList", QStringList());
	TypesDB::instance().registerType<AbstractControllerInput>("AbstractControllerInput", QStringList() << "Component");
	TypesDB::instance().registerType<AbstractControllerOutput>("AbstractControllerOutput", QStringList() << "Component");

	// Registering Evonet
	TypesDB::instance().registerType<Evonet>("Evonet", QStringList() << "Component");

	// Registering evorobot component
	TypesDB::instance().registerType<Evoga>("Evoga", QStringList() << "Component");

	// Registering the EmbodiedAgent component
	TypesDB::instance().registerType<EmbodiedAgent>("EmbodiedAgent", QStringList() << "Component" << "ControllerInputsList" << "ControllerOutputsList");

	// Registering robots
	TypesDB::instance().registerType<Robot>("Robot", QStringList() << "Component");
	TypesDB::instance().registerType<RobotOnPlane>("RobotOnPlane", QStringList() << "Robot");
	TypesDB::instance().registerType<Khepera, Khepera::Creator>("Khepera", QStringList() << "RobotOnPlane");

	// Registering the arena
	TypesDB::instance().registerType<Arena>("Arena", QStringList() << "Component");

	// Registering the noisy device
	TypesDB::instance().registerType<NoisyDevice>("NoisyDevice", QStringList());

	// Registering sensors
	TypesDB::instance().registerType<NoisyInput>("NoisyInput", QStringList() << "AbstractControllerInput" << "NoisyDevice");
	TypesDB::instance().registerType<KheperaSensor>("KheperaSensor", QStringList() << "Component");
	TypesDB::instance().registerType<KheperaSampledProximityIRSensor>("KheperaSampledProximityIRSensor", QStringList() << "KheperaSensor");

	// Registering motors
	TypesDB::instance().registerType<NoisyOutput>("NoisyOutput", QStringList() << "AbstractControllerOutput" << "NoisyDevice");
	TypesDB::instance().registerType<KheperaMotor>("KheperaMotor", QStringList() << "Component");
	TypesDB::instance().registerType<KheperaWheelVelocityMotor>("KheperaWheelVelocityMotor", QStringList() << "KheperaMotor");

	// Registering all types for evorobot component
	TypesDB::instance().registerType<EvoRobotComponent>("EvoRobotComponent", QStringList() << "Component");
	TypesDB::instance().registerType<Evoga>("Evoga", QStringList() << "Component");
	TypesDB::instance().registerType<EvoRobotExperiment>("EvoRobotExperiment", QStringList() << "Component");

// ============================= OLD REGISTRATIONS =============================
// 	// Registering main types to factory
// 	Factory::getInstance().registerClass<Component>( "Component", "ParameterSettableWithConfigureFunction" );
// 	Factory::getInstance().registerClass<BaseExperiment>( "BaseExperiment", "Component" );
//
// 	// Evonet does not comply with ConfigurationParameters
// 	Factory::getInstance().registerClass<Evonet>( "Evonet", "ParameterSettableWithConfigureFunction" );
//
// 	// Registering all robots
// 	Factory::getInstance().registerClass<Robot>( "Robot", "ParameterSettableInConstructor" );
// 	Factory::getInstance().registerClass<MarXbot>( "MarXbot", "Robot" );
// 	Factory::getInstance().registerClass<Epuck>( "Epuck", "Robot" );
// 	Factory::getInstance().registerClass<Khepera>( "Khepera", "Robot" );
//
// 	// Registering the arena
// 	Factory::getInstance().registerClass<Arena>( "Arena", "ParameterSettableInConstructor" );
//
// 	// Registering all types for evorobot component
// 	Factory::getInstance().registerClass<EvoRobotComponent>( "EvoRobotComponent", "Component" );
// 	Factory::getInstance().registerClass<Evoga>("Evoga", "ParameterSettableWithConfigureFunction");
// 	Factory::getInstance().registerClass<AbstractTest>("AbstractTest", "ParameterSettableWithConfigureFunction");
// 	Factory::getInstance().registerClass<EvoRobotExperiment>("EvoRobotExperiment", "ParameterSettableWithConfigureFunction");
// 	Factory::getInstance().registerClass<TestRandom>("TestRandom", "AbstractTest");
// 	Factory::getInstance().registerClass<TestIndividual>("TestIndividual", "AbstractTest");
// 	Factory::getInstance().registerClass<TestCurrent>("TestCurrent", "AbstractTest");
//
// 	// Registering sensors
// 	Factory::getInstance().registerClass<Sensor>("Sensor", "ParameterSettableInConstructor");
// 	Factory::getInstance().registerClass<FakeSensor>("FakeSensor", "Sensor");
// 	Factory::getInstance().registerClass<ObjectPositionSensor>("ObjectPositionSensor", "Sensor");
// 	Factory::getInstance().registerClass<MarXbotSensor>("MarXbotSensor", "Sensor");
// 	Factory::getInstance().registerClass<MarXbotProximityIRSensor>("MarXbotProximityIRSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotGroundBottomIRSensor>("MarXbotGroundBottomIRSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotGroundAroundIRSensor>("MarXbotGroundAroundIRSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotLinearCameraSensor>("MarXbotLinearCameraSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotLinearCameraSensorNew>("MarXbotLinearCameraSensorNew", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotTractionSensor>("MarXbotTractionSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotSampledProximityIRSensor>("MarXbotSampledProximityIRSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotAttachmentDeviceSensor>("MarXbotAttachmentDeviceSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotWheelSpeedsSensor>("MarXbotWheelSpeedsSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotLaserFrontDistanceSensor>("MarXbotLaserFrontDistanceSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<MarXbotLaserOmniDistanceSensor>("MarXbotLaserOmniDistanceSensor", "MarXbotSensor");
// 	Factory::getInstance().registerClass<EpuckSensor>("EpuckSensor", "Sensor");
// 	Factory::getInstance().registerClass<EpuckProximityIRSensor>("EpuckProximityIRSensor", "EpuckSensor");
// 	Factory::getInstance().registerClass<EpuckGroundIRSensor>("EpuckGroundIRSensor", "EpuckSensor");
// 	Factory::getInstance().registerClass<EpuckLinearCameraSensor>("EpuckLinearCameraSensor", "EpuckSensor");
// 	Factory::getInstance().registerClass<EpuckSampledProximityIRSensor>("EpuckSampledProximityIRSensor", "EpuckSensor");
// 	Factory::getInstance().registerClass<KheperaSensor>("KheperaSensor", "Sensor");
// 	Factory::getInstance().registerClass<KheperaProximityIRSensor>("KheperaProximityIRSensor", "KheperaSensor");
// 	Factory::getInstance().registerClass<KheperaSampledProximityIRSensor>("KheperaSampledProximityIRSensor", "KheperaSensor");
//
// 	Factory::getInstance().registerClass<KheperaSampledLightSensor>("KheperaSampledLightSensor", "KheperaSensor");
// 	Factory::getInstance().registerClass<KheperaGroundSensor>("KheperaGroundSensor", "KheperaSensor");
// 	Factory::getInstance().registerClass<KheperaWheelSpeedsSensor>("KheperaWheelSpeedsSensor", "KheperaSensor");
//
// 	Factory::getInstance().registerClass<KheperaWheelSpeedsSensor>("KheperaWheelSpeedsSensor", "KheperaSensor");
//
// 	// Registering motors
// 	Factory::getInstance().registerClass<Motor>("Motor", "ParameterSettableInConstructor");
// 	Factory::getInstance().registerClass<FakeMotor>("FakeMotor", "Motor");
// 	Factory::getInstance().registerClass<MarXbotMotor>("MarXbotMotor", "Motor");
// 	Factory::getInstance().registerClass<MarXbotWheelVelocityMotor>("MarXbotWheelVelocityMotor", "MarXbotMotor");
// 	Factory::getInstance().registerClass<MarXbotWheelVelOrient>("MarXbotWheelVelOrient", "MarXbotMotor");
// 	Factory::getInstance().registerClass<MarXbotColorMotor>("MarXbotColorMotor", "MarXbotMotor");
// 	Factory::getInstance().registerClass<MarXbotAttachmentDeviceMotor>("MarXbotAttachmentDeviceMotor", "MarXbotMotor");
// 	Factory::getInstance().registerClass<EpuckMotor>("EpuckMotor", "Motor");
// 	Factory::getInstance().registerClass<EpuckWheelVelocityMotor>("EpuckWheelVelocityMotor", "EpuckMotor");
// 	Factory::getInstance().registerClass<KheperaMotor>("KheperaMotor", "Motor");
// 	Factory::getInstance().registerClass<KheperaWheelVelocityMotor>("KheperaWheelVelocityMotor", "KheperaMotor");
//
	Logger::info( "Library Experiments Initialized" );
	return true;
}

} // end namespace farsa
