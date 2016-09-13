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

#ifndef PHYEPUCK_H
#define PHYEPUCK_H

#include "worldsimconfig.h"
#include "wvector.h"
#include "wmatrix.h"
#include "world.h"
#include "motorcontrollers.h"
#include "sensorcontrollers.h"
#include "graphicalmarkers.h"
#include "phycylinder.h"
#include <QVector>

namespace salsa {

class PhyObject;
class PhyJoint;
class PhyCylinder;

/**
 * \brief The shared data for the PhyEpuck
 */
class SALSA_WSIM_TEMPLATE PhyEpuckShared : public WObjectShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyEpuckShared()
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
	 * exactly 8 elements otherwise
	 */
	QList<QColor> ledColors;
};

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning il renderer dell Epuck ci vuole per disegnare l AABB e l OBB. Inoltre aggiungere funzioni statiche per calcolare i due (si potrebbe anche decidere di usare una mesh per disegnare l epuck invece dei pezzi)
#endif

/**
 * \brief The class modelling an e-puck robot
 *
 * This class models an e-puck robot. For more information about the robot go to
 * http://mobots.epfl.ch/e-puck.html
 */
class SALSA_WSIM_API PhyEpuck : public QObject, public WObject
{
	Q_OBJECT

public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyEpuckShared Shared;

	/**
	 * \brief The x dimension of the battery pack under the robot
	 */
	static const real batteryplacex;

	/**
	 * \brief The y dimension of the battery pack under the robot
	 */
	static const real batteryplacey;

	/**
	 * \brief The z dimension of the battery pack under the robot
	 */
	static const real batteryplacez;

	/**
	 * \brief The mass of the battery
	 */
	static const real batterym;

	/**
	 * \brief The distance of the battery pack from the ground
	 */
	static const real batteryplacedistancefromground;

	/**
	 * \brief The radius of the upper part of the robot (containing the main
	 *        electionic board)
	 */
	static const real bodyr;

	/**
	 * \brief The height of the upper part of the robot (containing the main
	 *        electionic board)
	 */
	static const real bodyh;

	/**
	 * \brief The mass of the robot without the wheels and the battery
	 */
	static const real wholebodym;

	/**
	 * \brief The radius of the motorized wheels
	 */
	static const real wheelr;

	/**
	 * \brief The height of the motorized wheels
	 */
	static const real wheelh;

	/**
	 * \brief The mass of one motorized wheel
	 */
	static const real wheelm;

	/**
	 * \brief The distance between the two motorized wheels
	 */
	static const real axletrack;

	/**
	 * \brief The radius of the passive wheels
	 *
	 * The real robot has no passive wheels, here we need them to reduce the
	 * friction with the ground
	 */
	static const real passivewheelr;

	/**
	 * \brief The mass of the passive wheels
	 *
	 * The real robot has no passive wheels, here we need them to reduce the
	 * friction with the ground
	 */
	static const real passivewheelm;

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
	PhyEpuck(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyEpuck();

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
	 * \brief Returns the proximity infrared sensors controller
	 *
	 * \return the proximity infrared sensors controller
	 */
	SimulatedIRProximitySensorController* proximityIRSensorController()
	{
		return m_proximityIR;
	}

	/**
	 * \brief Returns the ground IR sensors controller
	 *
	 * \return the ground IR sensors controller
	 */
	SimulatedIRGroundSensorController* groundIRSensorController()
	{
		return m_groundIR;
	}

	/**
	 * \brief Pre-updates the robot
	 *
	 * This method is called at each step of the world just before the
	 * physic update
	 */
	virtual void preUpdate();

	/**
	 * \brief Post-updates the robot
	 *
	 * This method is called at each step of the world just after the physic
	 * update
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
	 * \brief Sets whether ground IR sensors are drawn or not and how
	 *
	 * \param drawSensor whether to draw the sensors or not
	 * \param drawRay whether to draw the IR rays or not
	 * \param drawRealRay if true rays are drawn in their actual length,
	 *                    otherwise only the direction is given
	 */
	void setGroundIRSensorsGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

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
	 * \brief Changes the robot model from dynamic to kinematic and
	 *        vice-versa
	 *
	 * \param k if true switches to the kinematic e-puck model, if false to
	 *          the dynamic one (at creation the model is always dynamic)
	 */
	void doKinematicSimulation(bool k);

	/**
	 * \brief Returns true if we are using the kinematic model
	 *
	 * \return true if we are using the kinematic model
	 */
	bool isKinematic() const
	{
		return m_shared->kinematicSimulation;
	}

	/**
	 * \brief Sets the color of the leds of the epuck
	 *
	 * This function takes a list of exactly 8 colors for the leds. If this
	 * function is never called, all leds take the color set by setColor().
	 * As soon as this function is called, the color of the leds becomes
	 * indipendent of the color set by setColor()
	 * \param c the list of colors of leds. This must have exactly 8
	 *          elements, otherwise colors are not changed
	 * \param upperBaseColor the color of the upper base. This is ignored if
	 *                       it is an invalid color
	 */
	void setLedColors(QList<QColor> c, QColor upperBaseColor = QColor());

	/**
	 * \brief Returns the color of the leds
	 *
	 * \return the color of leds
	 */
	QList<QColor> ledColors() const;

	/**
	 * \brief Returns the colors of segments of the turret
	 *
	 * This is a low-level function that returns the color of segments on
	 * the turrent
	 * \return the colors of segments of the turret
	 */
	const QList<PhyCylinderSegmentColor>& segmentsColor() const;

protected slots:
	/**
	 * \brief Sets the desidered velocity of the left wheel
	 *
	 * This is used when in kinematic to move the robot. We only use
	 * velocity because robots can only move by setting wheel velocities
	 * \param velocity the desidered velocity (in radiants per second)
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
	 * \brief The object representing the robot body
	 */
	PhyObject* m_body;

	/**
	 * \brief The upper part of the body
	 *
	 * This is part of m_body, so it is not physically simulated. We store
	 * it, however, to be able to change its color
	 */
	PhyCylinder* m_turret;

	/**
	 * \brief The vector containing the wheels
	 *
	 * The order is: right wheel, left wheel, and 2 passive spherical wheels
	 */
	QVector<PhyObject*> m_wheels;

	/**
	 * \brief The relative matrices of wheels respect to body
	 *
	 * The order is: right wheel, left wheel, and 2 passive spherical wheels
	 */
	QVector<wMatrix> m_wheelsTransformation;

	/**
	 * \brief The vector containing the wheel joints
	 *
	 * The order is: right wheel, left wheel, and 2 passive spherical wheels
	 */
	QVector<PhyJoint*> m_wheelJoints;

	/**
	 * \brief The motor controller for the two motorized wheels
	 *
	 * The wheels order it (right, left)
	 */
	WheelMotorController* m_wheelsCtrl;

	/**
	 * \brief The simulated proximity IR sensors
	 */
	SimulatedIRProximitySensorController* m_proximityIR;

	/**
	 * \brief The simulated ground IR sensors
	 */
	SimulatedIRGroundSensorController* m_groundIR;

	/**
	 * \brief The graphical marker on the khepera indicating the front of
	 *        the robot
	 *
	 * If nullptr no marker is displayed
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

} // end namespace salsa

#endif
