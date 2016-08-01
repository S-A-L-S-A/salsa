/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#include "robots.h"
#include "configurationmanager.h"
#include "configurationhelper.h"
#include "logger.h"
#include "mathutils.h"
#include <QStringList>
#include <QRegExp>

namespace salsa {

namespace {
	QString extractRobotName(ConfigurationManager& params, QString prefix)
	{
		return ConfigurationHelper::getString(params, prefix + "name");
	}

	wMatrix extractRobotTranformation(ConfigurationManager& params, QString prefix)
	{
		wMatrix mtr = wMatrix::identity();

		QString value = ConfigurationHelper::getString(params, prefix + "transformation");

		if (value.isEmpty()) {
			return mtr;
		}

		// The values on the same row are separated by spaces, rows are separated by semicolons
		QStringList rows = value.split(";");
		if (rows.size() != 4) {
			ConfigurationHelper::throwUserConfigError(prefix + "transformation", value, "The transformation matrix must have 4 rows. The format is: a11 a12 a13 a14; a21 a22 a23 a24; a31 a32 a33 a34; a41 a42 a43 a44");
		}

		for (int i = 0; i < rows.size(); i++) {
			QStringList elements = rows[i].split(QRegExp("\\s+"));
			if (elements.size() != 4) {
				ConfigurationHelper::throwUserConfigError(prefix + "transformation", value, "The transformation matrix must have 4 columns. The format is: a11 a12 a13 a14; a21 a22 a23 a24; a31 a32 a33 a34; a41 a42 a43 a44");
			}
			for (int j = 0; j < elements.size(); j++) {
				bool ok;
				mtr[i][j] = elements[j].toDouble(&ok);
				if (!ok) {
					ConfigurationHelper::throwUserConfigError(prefix + "transformation", value, "The transformation matrix elements must be numbers");
				}
			}
		}

		return mtr;
	}
}

Robot::Robot(ConfigurationManager& params)
	: Component(params)
	, m_initialName(extractRobotName(configurationManager(), confPath()))
	, m_initialTm(extractRobotTranformation(configurationManager(), confPath()))
{
}

Robot::~Robot()
{
}

void Robot::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The abstract component for a simulated robot", "Note that to create a robot we need a resource named \"world\" with a valid World instance");

	d.describeString("name").def("robot").help("The name of the robot");

	d.describeString("transformation").def("").help("The initial transformation matrix for the robot", "The transformation matrix must be a 4x4 matrix. The format is: a11 a12 a13 a14; a21 a22 a23 a24; a31 a32 a33 a34; a41 a42 a43 a44");
}

RobotOnPlane::RobotOnPlane(ConfigurationManager& params)
	: Robot(params)
	, m_color(ConfigurationHelper::getString(configurationManager(), confPath() + "color"))
{
	if (!m_color.isValid()) {
		ConfigurationHelper::throwUserConfigError(confPath() + "color", configurationManager().getValue(confPath() + "color"), "The value of the \"color\" parameter is not a valid color");
	}
}

RobotOnPlane::~RobotOnPlane()
{
}

void RobotOnPlane::describe(RegisteredComponentDescriptor& d)
{
	Robot::describe(d);

	d.help("The base class for robots that lie on a plane", "The base class for robots that lie on a plane");

	d.describeString("color").def("#FFFFFF").help("The color of the robot.", "This is a string. Its format can be: #RGB, #RRGGBB, #RRRGGGBBB, #RRRRGGGGBBBB (each of R, G, and B is a single hex digit) or a name from the list of colors defined in the list of SVG color keyword names provided by the World Wide Web Consortium (see http://www.w3.org/TR/SVG/types.html#ColorKeywords). The default value is \"#FFFFFF\"");
}

void RobotOnPlane::setPosition(const Box2DWrapper* plane, const wVector& pos)
{
	setPosition(plane, pos.x, pos.y);
}

// MarXbot::MarXbot(ConfigurationParameters& params, QString prefix) :
// 	RobotOnPlane(params, prefix),
// 	PhyMarXbot(extractWorld(params), extractRobotName(params, prefix, "marXbot"), extractRobotTranformation(params, prefix))
// {
// 	doKinematicSimulation(ConfigurationHelper::getBool(params, prefix + "kinematicRobot", false));
//
// 	const bool enableWheels = ConfigurationHelper::getBool(params, prefix + "enableWheels", true);
// 	const bool enableProximityIR = ConfigurationHelper::getBool(params, prefix + "enableProximityIR", false);
// 	const bool enableGroundBottomIR = ConfigurationHelper::getBool(params, prefix + "enableGroundBottomIR", false);
// 	const bool enableGroundAroundIR = ConfigurationHelper::getBool(params, prefix + "enableGroundAroundIR", false);
// 	const bool enableAttachDev = ConfigurationHelper::getBool(params, prefix + "enableAttachmentDevice", false);
// 	const bool drawProximityIR = ConfigurationHelper::getBool(params, prefix + "drawProximityIR", false);
// 	const bool drawGroundBottomIR = ConfigurationHelper::getBool(params, prefix + "drawGroundBottomIR", false);
// 	const bool drawGroundAroundIR = ConfigurationHelper::getBool(params, prefix + "drawGroundAroundIR", false);
// 	const bool drawIRRays =ConfigurationHelper::getBool(params, prefix + "drawIRRays", false);
// 	const bool drawIRRaysRange = ConfigurationHelper::getBool(params, prefix + "drawIRRaysRange", false);
//
// 	wheelsController()->setEnabled(enableWheels);
// 	proximityIRSensorController()->setEnabled(enableProximityIR);
// 	setProximityIRSensorsGraphicalProperties(drawProximityIR, drawIRRays, drawIRRaysRange);
// 	groundBottomIRSensorController()->setEnabled(enableGroundBottomIR);
// 	setGroundBottomIRSensorsGraphicalProperties(drawGroundBottomIR, drawIRRays, drawIRRaysRange);
// 	groundAroundIRSensorController()->setEnabled(enableGroundAroundIR);
// 	setGroundAroundIRSensorsGraphicalProperties(drawGroundAroundIR, drawIRRays, drawIRRaysRange);
// 	enableAttachmentDevice(enableAttachDev);
//
// 	QString ledColorsString = ConfigurationHelper::getString(params, prefix + "ledColors", "").replace("+", "#");
// 	if (!ledColorsString.isEmpty()) {
// 		// Converting all colors
// 		QList<QColor> ledColors;
// 		QStringList c = ledColorsString.split(" ", QString::SkipEmptyParts);
// 		if (c.size() != 12) {
// 			ConfigurationHelper::throwUserConfigError(prefix + "ledColors", params.getValue(prefix + "ledColors"), "The ledColors parameter must be a list of exactly 12 elements");
// 		}
// 		for (int i = 0; i < c.size(); ++i) {
// 			const QColor curColor(c[i]);
// 			if (!curColor.isValid()) {
// 				ConfigurationHelper::throwUserConfigError(prefix + "ledColors", params.getValue(prefix + "ledColors"), QString("The value of the %1th color is not a valid color").arg(i));
// 			}
// 			ledColors << curColor;
// 		}
// 		// Finally setting the led colors
// 		setLedColors(ledColors);
// 	}
//
// 	// Setting the color of the robot
// 	setColor(configuredRobotColor());
// }
//
// void MarXbot::save(ConfigurationParameters& /*params*/, QString /*prefix*/) {
// 	Logger::error("NOT IMPLEMENTED (MarXBot::save)");
// 	abort();
// }
//
// void MarXbot::describe(QString type) {
// 	RobotOnPlane::describe(type);
//
// 	Descriptor d = addTypeDescription(type, "The simulated wheeled MarXBot robot in a physical world", "This type models the wheeled MarXBot robot in a physical world. Note that to create a robot we need a resource named \"world\" with a valid World instance");
//
// 	d.describeBool("kinematicRobot").def(false).help("Whether only the kinematic behaviour of the robot has to be simulated or not", "If true only the kinematic properties of the robot are simulated (i.e. velocity); is false (the default) the whole dynamic of the robot is simulated");
//
// 	d.describeString("name").def("marXbot").help("The name of the MarXBot robot", "The name of the MarXBot robot");
//
// 	d.describeString("transformation").def("").help("The initial transformation matrix for the robot", "The transformation matrix must be a 4x4 matrix. The format is: a11 a12 a13 a14; a21 a22 a23 a24; a31 a32 a33 a34; a41 a42 a43 a44");
//
// 	d.describeBool("enableWheels").def(true).help("If true enables the wheel motors", "When set to false the wheel motors are not enabled, so the robot cannot move by itself");
//
// 	d.describeBool("enableProximityIR").def(false).help("If true enables the proximity IR sensors", "When set to false the proximity infrared sensors are not enabled, thus making simulation a bit faster. To increase simulation speed try to enable only the needed parts of the robot");
//
// 	d.describeBool("enableGroundBottomIR").def(false).help("If true enables the bottom ground IR sensors", "When set to false the ground infrared sensors below the battery pack are not enabled, thus making simulation a bit faster. To increase simulation speed try to enable only the needed parts of the robot");
//
// 	d.describeBool("enableGroundAroundIR").def(false).help("If true enables the around ground IR sensors", "When set to false the ground infrared sensors on the bottom part of the base (just above the wheels) are not enabled, thus making simulation a bit faster. To increase simulation speed try to enable only the needed parts of the robot");
//
// 	d.describeBool("enableAttachmentDevice").def(false).help("If true enables the attachement device of the MarXbot", "When set to true the attachement device is enabled. This device can be used by the robot to attach to another robot. The default is false");
//
// 	d.describeBool("drawProximityIR").def(false).help("If true draws the proximity IR sensors", "When set to true draws the proximity infrared sensors");
//
// 	d.describeBool("drawGroundBottomIR").def(false).help("If true draws the bottom ground IR sensors", "When set to true draws the ground infrared sensors below the battery pack");
//
// 	d.describeBool("drawGroundAroundIR").def(false).help("If true draws the around ground IR sensors", "When set to true draws the ground infrared sensors on the bottom part of the base (just above the wheels)");
//
// 	d.describeBool("drawIRRays").def(false).help("If true draws rays of enabled IR sensors", "When set to true rays of the enabled IR sensors (i.e. proximity, ground bottom and ground around infrared sensors) are drawn");
//
// 	d.describeBool("drawIRRaysRange").def(false).help("If true rays of enabled IR sensors are shown in their real range", "When drawIRRays is true, this parameter sets whether rays are drawn in their real range (the parameter is true) or instead only their direction is shown (the parameter is false)");
//
// 	d.describeString("ledColors").def("+FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF +FFFFFF").help("The color of the leds of the robot.", "This is a list of colors separated by white spaces. Each color can be in one of the following formats: +RGB, +RRGGBB, +RRRGGGBBB, +RRRRGGGGBBBB (each of R, G, and B is a single hex digit) or a name from the list of colors defined in the list of SVG color keyword names provided by the World Wide Web Consortium (see http://www.w3.org/TR/SVG/types.html#ColorKeywords). The list must have exactly 12 elements. The default value is white for all leds");
// }
//
// MarXbot::~MarXbot() {
// 	// Nothing to do here
// }
//
// void MarXbot::setPosition(const Box2DWrapper* plane, real x, real y)
// {
// 	// The frame of reference of the MarXbot lies on the plane, we can simply set its position here
// 	PhyMarXbot::setPosition(positionOnPlane(plane, x, y));
// }
//
// wVector MarXbot::position() const
// {
// 	return matrix().w_pos;
// }
//
// void MarXbot::setOrientation(const Box2DWrapper* plane, real angle)
// {
// 	wMatrix mtr = matrix();
//
// 	// Using the helper function to compute the new matrix
// 	orientationOnPlane(plane, angle, mtr);
//
// 	setMatrix(mtr);
// }
//
// #if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
// 	#warning LE FUNZIONI orientation() E setOrientation() DEI ROBOT NON USANO LO STESSO RIFERIMENTO, OSSIA setOrientation(orientation()) RUOTA IL ROBOT ANCHE SE NON DOVREBBE! CORREGGERE IN SALSA2
// #endif
//
// real MarXbot::orientation(const Box2DWrapper* plane) const
// {
// 	// The -PI_GRECO / 2.0 term is due to the fact that the robot head is towards -y
// 	return angleBetweenXAxes(plane->phyObject()->matrix(), matrix()) - PI_GRECO / 2.0;
// }
//
// real MarXbot::robotHeight() const
// {
// 	return PhyMarXbot::basez + PhyMarXbot::bodyh;
// }
//
// real MarXbot::robotRadius() const
// {
// 	return PhyMarXbot::bodyr;
// }
//
// bool MarXbot::isKinematic() const
// {
// 	return PhyMarXbot::isKinematic();
// }
//
// QColor MarXbot::robotColor() const
// {
// 	return PhyMarXbot::color();
// }
//
// Epuck::Epuck(ConfigurationParameters& params, QString prefix) :
// 	RobotOnPlane(params, prefix),
// 	PhyEpuck(extractWorld(params), extractRobotName(params, prefix, "epuck"), extractRobotTranformation(params, prefix))
// {
// 	doKinematicSimulation(ConfigurationHelper::getBool(params, prefix + "kinematicRobot", false));
//
// 	const bool enableWheels = ConfigurationHelper::getBool(params, prefix + "enableWheels", true);
// 	const bool enableProximityIR = ConfigurationHelper::getBool(params, prefix + "enableProximityIR", false);
// 	const bool enableGroundIR = ConfigurationHelper::getBool(params, prefix + "enableGroundIR", false);
// 	const bool drawProximityIR = ConfigurationHelper::getBool(params, prefix + "drawProximityIR", false);
// 	const bool drawGroundIR = ConfigurationHelper::getBool(params, prefix + "drawGroundIR", false);
// 	const bool drawIRRays =ConfigurationHelper::getBool(params, prefix + "drawIRRays", false);
// 	const bool drawIRRaysRange = ConfigurationHelper::getBool(params, prefix + "drawIRRaysRange", false);
//
// 	wheelsController()->setEnabled(enableWheels);
// 	proximityIRSensorController()->setEnabled(enableProximityIR);
// 	setProximityIRSensorsGraphicalProperties(drawProximityIR, drawIRRays, drawIRRaysRange);
// 	groundIRSensorController()->setEnabled(enableGroundIR);
// 	setGroundIRSensorsGraphicalProperties(drawGroundIR, drawIRRays, drawIRRaysRange);
//
// 	// Setting the color of the robot
// 	setColor(configuredRobotColor());
// }
//
// void Epuck::save(ConfigurationParameters& /*params*/, QString /*prefix*/) {
// 	Logger::error("NOT IMPLEMENTED (Epuck::save)");
// 	abort();
// }
//
// void Epuck::describe(QString type) {
// 	RobotOnPlane::describe(type);
//
// 	Descriptor d = addTypeDescription(type, "The simulated wheeled Epuck robot in a physical world", "This type models the wheeled Epuck robot in a physical world. Note that to create a robot we need a resource named \"world\" with a valid World instance");
//
// 	d.describeBool("kinematicRobot").def(false).help("Whether only the kinematic behaviour of the robot has to be simulated or not", "If true only the kinematic properties of the robot are simulated (i.e. velocity); is false (the default) the whole dynamic of the robot is simulated");
//
// 	d.describeString("name").def("epuck").help("The name of the Epuck robot", "The name of the Epuck robot");
//
// 	d.describeString("transformation").def("").help("The initial transformation matrix for the robot", "The transformation matrix must be a 4x4 matrix. The format is: a11 a12 a13 a14; a21 a22 a23 a24; a31 a32 a33 a34; a41 a42 a43 a44");
//
// 	d.describeBool("enableWheels").def(true).help("If true enables the wheel motors", "When set to false the wheel motors are not enabled, so the robot cannot move by itself");
//
// 	d.describeBool("enableProximityIR").def(false).help("If true enables the proximity IR sensors", "When set to false the proximity infrared sensors are not enabled, thus making simulation a bit faster. To increase simulation speed try to enable only the needed parts of the robot");
//
// 	d.describeBool("enableGroundIR").def(false).help("If true enables the ground IR sensors", "When set to false the ground infrared sensors are not enabled, thus making simulation a bit faster. To increase simulation speed try to enable only the needed parts of the robot");
//
// 	d.describeBool("drawProximityIR").def(false).help("If true draws the proximity IR sensors", "When set to true draws the proximity IR sensors");
//
// 	d.describeBool("drawGroundIR").def(false).help("If true draws the ground IR sensors", "When set to true draws the ground IR sensors");
//
// 	d.describeBool("drawIRRays").def(false).help("If true draws rays of enabled IR sensors", "When set to true rays of the enabled IR sensors (i.e. proximity and ground infrared sensors) are drawn");
//
// 	d.describeBool("drawIRRaysRange").def(false).help("If true rays of enabled IR sensors are shown in their real range", "When drawIRRays is true, this parameter sets whether rays are drawn in their real range (the parameter is true) or instead only their direction is shown (the parameter is false)");
//
// }
//
// Epuck::~Epuck() {
// 	// Nothing to do here
// }
//
// void Epuck::setPosition(const Box2DWrapper* plane, real x, real y)
// {
// 	// The frame of reference of the e-puck lies on the plane, we can simply set its position here
// 	PhyEpuck::setPosition(positionOnPlane(plane, x, y));
// }
//
// wVector Epuck::position() const
// {
// 	return matrix().w_pos;
// }
//
// void Epuck::setOrientation(const Box2DWrapper* plane, real angle)
// {
// 	wMatrix mtr = matrix();
//
// 	// Using the helper function to compute the new matrix
// 	orientationOnPlane(plane, angle, mtr);
//
// 	setMatrix(mtr);
// }
//
// real Epuck::orientation(const Box2DWrapper* plane) const
// {
// 	// The -PI_GRECO / 2.0 term is due to the fact that the robot head is towards -y
// 	return angleBetweenXAxes(plane->phyObject()->matrix(), matrix()) - PI_GRECO / 2.0;
// }
//
// real Epuck::robotHeight() const
// {
// 	return PhyEpuck::batteryplacedistancefromground + PhyEpuck::batteryplacez + PhyEpuck::bodyh;
// }
//
// real Epuck::robotRadius() const
// {
// 	return PhyEpuck::bodyr;
// }
//
// bool Epuck::isKinematic() const
// {
// 	return PhyEpuck::isKinematic();
// }
//
// QColor Epuck::robotColor() const
// {
// 	return PhyEpuck::color();
// }

Khepera::Khepera(World* world, SharedDataWrapper<Shared> shared, ConfigurationManager& params)
	: RobotOnPlane(params)
	, PhyKhepera(world, shared)
{
	setName(m_initialName);
	setMatrix(m_initialTm);

	doKinematicSimulation(ConfigurationHelper::getBool(configurationManager(), confPath() + "kinematicRobot"));

	const bool enableWheels = ConfigurationHelper::getBool(configurationManager(), confPath() + "enableWheels");
	const bool enableProximityIR = ConfigurationHelper::getBool(configurationManager(), confPath() + "enableProximityIR");
	const bool drawProximityIR = ConfigurationHelper::getBool(configurationManager(), confPath() + "drawProximityIR");
	const bool drawIRRays = ConfigurationHelper::getBool(configurationManager(), confPath() + "drawIRRays");
	const bool drawIRRaysRange = ConfigurationHelper::getBool(configurationManager(), confPath() + "drawIRRaysRange");

	wheelsController()->setEnabled(enableWheels);
	proximityIRSensorController()->setEnabled(enableProximityIR);
	setProximityIRSensorsGraphicalProperties(drawProximityIR, drawIRRays, drawIRRaysRange);

	setDrawFrontMarker(true);

	// Setting the color of the robot
	setColor(configuredRobotColor());
}

void Khepera::describe(RegisteredComponentDescriptor& d)
{
	RobotOnPlane::describe(d);

	d.help("The simulated wheeled Khepera II robot in a physical world", "This type models the wheeled Khepera II robot in a physical world. Note that to create a robot we need a resource named \"world\" with a valid World instance");

	d.describeBool("kinematicRobot").def(false).help("Whether only the kinematic behaviour of the robot has to be simulated or not", "If true only the kinematic properties of the robot are simulated (i.e. velocity); is false (the default) the whole dynamic of the robot is simulated");

	d.describeBool("enableWheels").def(true).help("If true enables the wheel motors", "When set to false the wheel motors are not enabled, so the robot cannot move by itself");

	d.describeBool("enableProximityIR").def(false).help("If true enables the proximity IR sensors", "When set to false the proximity infrared sensors are not enabled, thus making simulation a bit faster. To increase simulation speed try to enable only the needed parts of the robot");

	d.describeBool("drawProximityIR").def(false).help("If true draws the proximity IR sensors", "When set to true draws the proximity IR sensors");

	d.describeBool("drawIRRays").def(false).help("If true draws rays of enabled IR sensors", "When set to true rays of the enabled IR sensors (i.e. proximity and ground infrared sensors) are drawn");

	d.describeBool("drawIRRaysRange").def(false).help("If true rays of enabled IR sensors are shown in their real range", "When drawIRRays is true, this parameter sets whether rays are drawn in their real range (the parameter is true) or instead only their direction is shown (the parameter is false)");

}

Khepera::~Khepera()
{
}

void Khepera::setPosition(const Box2DWrapper* plane, real x, real y)
{
	// The frame of reference of the e-puck lies on the plane, we can simply set its position here
	PhyKhepera::setPosition(positionOnPlane(plane, x, y));
}

wVector Khepera::position() const
{
	return matrix().w_pos;
}

void Khepera::setOrientation(const Box2DWrapper* plane, real angle)
{
	wMatrix mtr = matrix();

	// Using the helper function to compute the new matrix
	orientationOnPlane(plane, angle, mtr);

	setMatrix(mtr);
}

real Khepera::orientation(const Box2DWrapper* plane) const
{
	// The -PI_GRECO / 2.0 term is due to the fact that the robot head is towards -y
	return angleBetweenXAxes(plane->phyObject()->matrix(), matrix()) - PI_GRECO / 2.0;
}

real Khepera::robotHeight() const
{
	return PhyKhepera::bodydistancefromground + PhyKhepera::bodyh;
}

real Khepera::robotRadius() const
{
	return PhyKhepera::bodyr;
}

bool Khepera::isKinematic() const
{
	return PhyKhepera::isKinematic();
}

QColor Khepera::robotColor() const
{
	return PhyKhepera::color();
}

} //end namespace salsa
