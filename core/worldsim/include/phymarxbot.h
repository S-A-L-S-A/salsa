/********************************************************************************
 *  WorldSim -- library for robot simulations                                   *
 *  Copyright (C) 2012-2013                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Fabrizio Papi <erkito87@gmail.com>                                          *
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

#ifndef PHYMARXBOT_H
#define PHYMARXBOT_H

#include "worldsimconfig.h"
#include "wvector.h"
#include "wmatrix.h"
#include "world.h"
#include "motorcontrollers.h"
#include "sensorcontrollers.h"
#include "graphicalmarkers.h"
#include "phyhinge.h"
#include "phyfixed.h"
#include "phycylinder.h"
#include <QVector>

namespace farsa {

class PhyObject;
class PhyJoint;
class PhyCylinder;
class PhyFixed;
class PhyHinge;

/**
 * \brief The shared data for the PhyMarXbot
 */
class FARSA_WSIM_TEMPLATE PhyMarXbotShared : public WObjectShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyMarXbotShared()
		: WObjectShared()
		, kinematicSimulation(false)
		, leftWheelVelocity(0.0f)
		, rightWheelVelocity(0.0f)
		, ledColors()
	{
	}

	/**
	 * \brief True if we are doing a kinematic simulation
	 */
	bool kinematicSimulation;

	/**
	 * \brief The speed of the left wheel (used when in kinematic)
	 */
	real leftWheelVelocity;

	/**
	 * \brief The speed of the left wheel (used when in kinematic)
	 */
	real rightWheelVelocity;

	/**
	 * \brief The vector with colors of the leds
	 *
	 * This is empty if the leds color hasn't been explicitly set, has
	 * exactly 12 elements otherwise
	 */
	QList<QColor> ledColors;
};

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning il renderer del MarXbot ci vuole per disegnare l AABB e l OBB. Inoltre aggiungere funzioni statiche per calcolare i due (si potrebbe anche decidere di usare una mesh per disegnare il MarXbot invece dei pezzi)
#endif


/**
 * \brief The MarXbot robot
 *
 * This class represents a MarXbot robot (http://mobots.epfl.ch/marxbot.html)
 * The robot has a base with two motorize wheels and an upper part. Moreover
 * the attachring module (the one used to connect two MarXbot robots) can be
 * enabled or disabled. Conversely from the real robot the traction sensor on
 * the attachring can be used even if the attachring module is disabled. This
 * allows to have slightly faster simulations when the traction sensor is used
 * but the possibility to attach two robots is not allowed
 */
class FARSA_WSIM_API PhyMarXbot : public QObject, public WObject
{
	Q_OBJECT

public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyMarXbotShared Shared;

	/**
	 * \brief The x dimension of the chassis containing the battery in
	 *        metres
	 */
	static const real basex;

	/**
	 * \brief The y dimension of the chassis containing the battery in
	 *        metres
	 */
	static const real basey;

	/**
	 * \brief The z dimension of the chassis containing the battery in
	 *        metres
	 */
	static const real basez;

	/**
	 * \brief The mass of the chassis containing the battery in kilograms
	 */
	static const real basem;

	/**
	 * \brief The radius of the cylindrical body supporting infrared sensors
	 *        and other modules and turrets in metres
	 */
	static const real bodyr;

	/**
	 * \brief The height of the cylindrical body supporting infrared sensors
	 *        and other modules and turrets in metres
	 */
	static const real bodyh;

	/**
	 * \brief The mass of the cylindrical body supporting infrared sensors
	 *        and other modules and turrets in kilograms
	 */
	static const real bodym;

	/**
	 * \brief The length of the axle of the rubber tracks in metres
	 */
	static const real axledistance;

	/**
	 * \brief The internal radius of the rubber tracks in metres
	 */
	static const real trackradius;

	/**
	 * \brief The height of the rubber tracks in metres
	 */
	static const real trackheight;

	/**
	 * \brief The mass of the rubber tracks in kilograms
	 */
	static const real trackm;

	/**
	 * \brief The height of the treads on tracks and wheels in metres
	 */
	static const real treaddepth;

	/**
	 * \brief The radius of the external wheels (excluding the tread height)
	 *        in metres
	 */
	static const real wheelr;

	/**
	 * \brief The height of the external wheels in metres
	 */
	static const real wheelh;

	/**
	 * \brief The mass of the external wheels in kilograms
	 */
	static const real wheelm;

	/**
	 * \brief The height of the turret in metres
	 */
	static const real turreth;

	/**
	 * \brief The mass of the turret in kilograms
	 */
	static const real turretm;

	/**
	 * \brief The radius of the attachment ring in metres
	 */
	static const real attachdevr;

	/**
	 * \brief The x dimension of the attachment device in metres
	 */
	static const real attachdevx;

	/**
	 * \brief The y dimension of the attachment device in metres
	 */
	static const real attachdevy;

	/**
	 * \brief The z dimension of the attachment device in metres
	 */
	static const real attachdevz;

	/**
	 * \brief The mass of the attachment device in kilograms
	 */
	static const real attachdevm;

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PhyMarXbot(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyMarXbot();

public:
	/**
	 * \brief Returns the motor controller for the wheels
	 *
	 * \return the motor controller for the wheels
	 */
	WheelMotorController* wheelsController()
	{
		return m_wheelsCtrl;
	}

	/**
	 * \brief Returns the motor controller for the attachment device
	 *
	 * \return the motor controller for the attachment device
	 */
	MarXbotAttachmentDeviceMotorController* attachmentDeviceController()
	{
		return m_attachdevCtrl;
	}

	/**
	 * \brief Returns the proximity infrared sensors controller
	 *
	 * \return the proximity infrared sensors controller
	 */
	SimulatedIRProximitySensorController* proximityIRSensorController()
	{
		return m_proximityIR;
	}

	/**
	 * \brief Returns the controller of the ground IR sensors below the
	 *        battery pack
	 *
	 * \return the controller of the ground IR sensors below the battery
	 *         pack
	 */
	SimulatedIRGroundSensorController* groundBottomIRSensorController()
	{
		return m_groundBottomIR;
	}

	/**
	 * \brief Returns the controller of the ground IR sensors on the base
	 *        (just above the wheels)
	 *
	 * \return the controller of the ground IR sensors on the base (just
	 *         above the wheels)
	 */
	SimulatedIRGroundSensorController* groundAroundIRSensorController()
	{
		return m_groundAroundIR;
	}

	/**
	 * \brief Returns the controller of the traction sensor of the robot
	 *
	 * \return the controller of the traction sensor of the robot
	 */
	TractionSensorController* tractionSensorController()
	{
		return m_tractionSensor;
	}

	/**
	 * \brief Pre-updates the robot
	 *
	 * This method is called at each step of the world just before the
	 * physic update. It updates motors
	 */
	virtual void preUpdate();

	/**
	 * \brief Post-updates the robot
	 *
	 * This method is called at each step of the world just after the physic
	 * update. It updates sensors
	 */
	virtual void postUpdate();

	/**
	 * \brief Sets whether proximity IR sensors are drawn or not and how
	 *
	 * \param drawSensor whether to draw the sensors or not
	 * \param drawRay whether to draw the IR rays or not
	 * \param drawRealRay if true rays are drawn in their actual length,
	 *                    otherwise only the direction is given
	 */
	void setProximityIRSensorsGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

	/**
	 * \brief Sets whether ground bottom IR sensors are drawn or not and how
	 *
	 * \param drawSensor whether to draw the sensors or not
	 * \param drawRay whether to draw the IR rays or not
	 * \param drawRealRay if true rays are drawn in their actual length,
	 *                    otherwise only the direction is given
	 */
	void setGroundBottomIRSensorsGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

	/**
	 * \brief Sets whether ground around IR sensors are drawn or not and how
	 *
	 * \param drawSensor whether to draw the sensors or not
	 * \param drawRay whether to draw the IR rays or not
	 * \param drawRealRay if true rays are drawn in their actual length,
	 *                    otherwise only the direction is given
	 */
	void setGroundAroundIRSensorsGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

	/**
	 * \brief Whether to draw a marker in the front part of the robot or not
	 *
	 * \param drawMarker if true draws a marker in the front part of the
	 *                   robot
	 */
	void setDrawFrontMarker(bool drawMarker);

	/**
	 * \brief Returns whether a marker in the front part of the robot is
	 *        drawn or not
	 *
	 * \return true if a marker in the front part of the robot is drawn
	 */
	bool getDrawFrontMarker() const;

	/**
	 * \brief Enables or disables the attachment device
	 *
	 * \param enable if true enables the attachment device, if false
	 *               disables it
	 * \note When the attachment device is disabled, the parts making it up
	 *       are deleted
	 */
	void enableAttachmentDevice(bool enable);

	/**
	 * \brief Returns true if the attachment device is enabled
	 *
	 * \return true if the attachment device is enabled
	 */
	bool attachmentDeviceEnabled() const
	{
		return (m_attachdev != NULL);
	}

	/**
	 * \brief Resets the attachment device
	 *
	 * This function completely resets the device: the status is set to open
	 * (possibly detaching an attached robot), the position to 0 and all
	 * robots attached to us are detached
	 */
	void resetAttachmentDevice();

	/**
	 * \brief Returns the solid modelling the base of the robot
	 *
	 * \return the solid modelling the base of the robot
	 */
	PhyObject* base()
	{
		return m_base;
	}

	/**
	 * \brief Returns the solid modelling the turret of the robot
	 *
	 * \return the solid modelling the turret of the robot
	 */
	PhyObject* turret()
	{
		return m_turret;
	}

	/**
	 * \brief Returns the solid modelling the attachment device
	 *
	 * \return the solid modelling the attachment device or NULL if the
	 *         attachment device is not enabled
	 */
	PhyObject* attachmentDevice()
	{
		return m_attachdev;
	}

	/**
	 * \brief Returns the joint of the attachment device
	 *
	 * \return the joint of the attachment device or NULL if the attachment
	 *         device is not enabled
	 */
	PhyHinge* attachmentDeviceJoint()
	{
		return m_attachdevjoint;
	}

	/**
	 * \brief Changes the robot model from dynamic to kinematic and
	 *        vice-versa
	 *
	 * \param k if true switches to the kinematic MarXbot model, if false to
	 *          the dynamic one (at creation the model is always dynamic)
	 */
	void doKinematicSimulation(bool k);

	/**
	 * \brief Returns true if we are using the kinematic model
	 *
	 * \returns true if we are using the kinematic model
	 */
	bool isKinematic() const
	{
		return m_shared->kinematicSimulation;
	}

	/**
	 * \brief Sets the color of the leds on the attachring of the MarXbot
	 *
	 * This function takes a list of exactly 12 colors for the leds on the
	 * attachring. If this function is never called, all leds take the color
	 * set by setColor(). As soon as this function is called, the color of
	 * the leds becomes indipendent of the color set by setColor()
	 * \param c the list of colors of leds. This must have exactly 12
	 *          elements, otherwise colors are not changed
	 */
	void setLedColors(QList<QColor> c);

	/**
	 * \brief Returns the color of the leds on the attachring
	 *
	 * \return the color of leds on the attachring
	 */
	QList<QColor> ledColors() const;

	/**
	 * \brief Returns the colors of segments of the attachring
	 *
	 * This is a low-level function that returns the color of segments on
	 * the attachring modelling the led colors
	 * \return the colors of segments of the attachring
	 */
	const QList<PhyCylinderSegmentColor>& segmentsColor() const;

protected slots:
	/**
	 * \brief Sets the desidered velocity of the left wheel
	 *
	 * This is used when in kinematic to move the robot. We only use
	 * velocity because robots can only move by setting wheel velocities
	 * \param velocity the desidered velocity (in radiants per
	 *                 second)
	 */
	void setLeftWheelDesideredVelocity(real velocity);

	/**
	 * \brief Sets the desidered velocity of the right wheel
	 *
	 * This is used when in kinematic to move the robot. We only use
	 * velocity because robots can only move by setting wheel velocities
	 * \param velocity the desidered velocity (in radiants per second)
	 */
	void setRightWheelDesideredVelocity(real velocity);

protected:
	/**
	 * \brief The function called when the transformation matrix of the
	 *        robot is changed
	 *
	 * This updates the matrix of all sub-objects
	 */
	virtual void changedMatrix();

private:
	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief The object representing the base
	 */
	PhyObject* m_base;

	/**
	 * \brief The vector containing the wheels
	 *
	 * The sequence is [right, left, and 4 spheres for the supporting
	 * tracks]
	 */
	QVector<PhyObject*> m_wheels;

	/**
	 * \brief The relative matrices of the wheels respect to base
	 *
	 * The sequence is [right, left, and 4 spheres for the supporting
	 * tracks]
	 */
	QVector<wMatrix> m_wheelstm;

	/**
	 * \brief The vector containing the wheel joints
	 *
	 * The sequence is [right, left, and 4 ball-and-socket for the tracks]
	 */
	QVector<PhyJoint*> m_wheelJoints;

	/**
	 * \brief The motor controllers for the wheels
	 *
	 * The first wheel is the right one, the second wheel is the left one
	 */
	WheelMotorController* m_wheelsCtrl;

	/**
	 * \brief The turret (i.e. the upper part of the robot)
	 *
	 * Conversely from the real MarXbot the traction sensor is between the
	 * turret and the base (instead of being between the base and the
	 * attachring)
	 */
	PhyCylinder* m_turret;

	/**
	 * \brief The relative matrix of the turret
	 */
	wMatrix m_turrettm;

	/**
	 * \brief The joint for sensing force between attachment module and the
	 *        rest of robot
	 */
	PhyFixed* m_forceSensor;

	/**
	 * \brief The attachment device
	 *
	 * This is only a box modelling the three-fingers attachment device.
	 * This is NULL if the attachment device is disabled
	 */
	PhyObject* m_attachdev;

	/**
	 * \brief The relative matrix of the attachment device
	 */
	wMatrix m_attachdevtm;

	/**
	 * \brief The rotational joint of the attachment device
	 */
	PhyHinge* m_attachdevjoint;

	/**
	 * \brief The controller for the attachment device
	 */
	MarXbotAttachmentDeviceMotorController* m_attachdevCtrl;

	/**
	 * \brief The simulated proximity IR sensors
	 */
	SimulatedIRProximitySensorController* m_proximityIR;

	/**
	 * \brief The simulated ground IR sensors
	 *
	 * These are the 4 sensors below the battery pack
	 */
	SimulatedIRGroundSensorController* m_groundBottomIR;

	/**
	 * \brief The simulated ground IR sensors
	 *
	 * These are the 8 sensors on the base (just above the wheels)
	 */
	SimulatedIRGroundSensorController* m_groundAroundIR;

	/**
	 * \brief The traction sensor of the robot
	 */
	TractionSensorController* m_tractionSensor;

	/**
	 * \brief The graphical marker on the MarXbot indicating the front of
	 *        the robot
	 *
	 * If NULL no marker is displayed
	 */
	PlanarArrowGraphicalMarker* m_frontMarker;

	/**
	 * \brief The color of cylinder segments when leds are not set
	 *
	 * This is set and returned in segmentsColor() when leds are not set
	 */
	mutable QList<PhyCylinderSegmentColor> m_uniformColor;

	/**
	 * \brief World is friend to be able to create and destroy this object
	 */
	friend class World;
};

} // end namespace farsa

#endif
