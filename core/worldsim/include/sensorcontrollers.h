/********************************************************************************
 *  SALSA                                                                       *
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

#ifndef SENSORCONTROLLERS_H
#define SENSORCONTROLLERS_H

#include "world.h"
#include "wentity.h"
#include "phyobject.h"
#include "phyjoint.h"
#include "singleir.h"

namespace salsa {

/**
 * \brief SensorController Class
 *
 * A common interface among SensorController hierachy. This has no renderer nor
 * shared data
 */
class SALSA_WSIM_API SensorController : public WEntity
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the sensorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               sensorcontroller has been deleted)
	 * \param name the name of the object
	 */
	SensorController(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed");

	/**
	 * \brief Destructor
	 *
	 * This deletes all owned object marked to be automatically destroyed
	 */
	virtual ~SensorController();

public:
	/**
	 * \brief This is the pure virtual method implemented by actual sensor
	 *        controllers that apply the logic of controller
	 */
	virtual void update() = 0;

	/**
	 * \brief Returns true if enabled (hence if it is on)
	 *
	 * \return true if enabled
	 */
	bool isEnabled()
	{
		return m_enabled;
	}

	/**
	 * \brief Enables or disables this sensorcontroller
	 *
	 * \param b if true the sensorcontroller is enabled, otherwise it is
	 *          disabled
	 */
	void setEnabled(bool b)
	{
		m_enabled = b;
	}

private:
	/**
	 * \brief If true the sensorcontroller is enabled
	 */
	bool m_enabled;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

/**
 * \brief The base abstract class of sensor controllers for infrared sensors
 *
 * This is an abstract base class providing an uniform interface for different
 * implementations, use one of its subclasses below. This class only provides a
 * vector of activations (m_activations) which must be filled in the update()
 * function by subclasses
 */
class SALSA_WSIM_API IRSensorController : public SensorController
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the sensorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               sensorcontroller has been deleted)
	 * \param numSensors the number of proximity IR sensors
	 * \param name the name of the object
	 */
	IRSensorController(World* world, SharedDataWrapper<Shared> shared, unsigned int numSensors, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	virtual ~IRSensorController();

public:
	/**
	 * \brief Returns the number of sensors
	 */
	int nSensors() const
	{
		return m_activations.size();
	}

	/**
	 * \brief Returns the activation of the i-th sensors
	 *
	 * \param i the index of the sensor whose activation value should be
	 *          returned
	 * \return the activation of the i-th sensor
	 */
	double activation(int i) const
	{
		return m_activations[i];
	}

	/**
	 * \brief Returns whether the i-th sensor is active or not
	 *
	 * \param i the index of the sensor
	 * \return true if the sensor is active, false if it is not
	 */
	bool sensorIsActive(int i) const
	{
		return m_activeSensor[i];
	}

	/**
	 * \brief Sets whether the i-th sensor is active or not
	 *
	 * This is virtual to allow subclasses to take actions depending on
	 * whether the sensor is active or not
	 * \param i the index of the sensor
	 * \param active if true the sensor is active
	 */
	virtual void setSensorActive(int i, bool active);

protected:
	/**
	 * \brief The vector with sensors activations
	 */
	QVector<double> m_activations;

	/**
	 * \brief The vector storing whether the i-th sensor should be active or
	 *        not
	 *
	 * If m_activeSensor[i] is false the i-th sensor should not be updated
	 * by subclasses
	 */
	QVector<bool> m_activeSensor;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

/**
 * \brief A collection of SingleIR
 *
 * This class implements IRSensorController by using a collection of SingleIR.
 * This takes the list of SingleIR in the constructor and uses them to obtain
 * activations.
 */
class SALSA_WSIM_API SimulatedIRProximitySensorController : public IRSensorController
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the sensorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               sensorcontroller has been deleted)
	 * \param sensors the list of single proximity IR sensors initialization
	 *                parameters
	 * \param name the name of the object
	 */
	SimulatedIRProximitySensorController(World* world, SharedDataWrapper<Shared> shared, const QVector<SingleIR::InitParams>& sensors, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	virtual ~SimulatedIRProximitySensorController();

public:
	/**
	 * \brief Updates sensor reading
	 */
	virtual void update();

	/**
	 * \brief Sets whether the i-th sensor is active or not
	 *
	 * This automatically disables graphics for sensors that are not active
	 * \param i the index of the sensor
	 * \param active if true the sensor is active
	 */
	virtual void setSensorActive(int i, bool active);

	/**
	 * \brief Sets graphical properties of all sensors
	 *
	 * By default the sensor is not drawn
	 * \param drawSensor if true the sensor is drawn
	 * \param drawRay if true sensor rays are drawn
	 * \param drawRealRay if true the actual ray range is drawn, otherwise
	 *                    only the direction of the rays is given
	 */
	void setGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

	/**
	 * \brief Returns a const reference to the list of SingleIR sensors
	 *
	 * \return a const reference to the list of SingleIR sensors
	 */
	const QVector<SingleIR*>& sensors() const
	{
		return m_sensors;
	}

private:
	/**
	 * \brief The list of single sensors
	 */
	QVector<SingleIR*> m_sensors;

	/**
	 * \brief Whether to draw sensors or not
	 */
	bool m_drawSensor;

	/**
	 * \brief When drawing sensors, whether to draw rays or not
	 */
	bool m_drawRay;

	/**
	 * \brief When drawing sensor rays, whether to use their real length or
	 *        not
	 */
	bool m_drawRealRay;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

/**
 * \brief A collection of SingleIR modelling ground sensors
 *
 * This class implements IRSensorController by using a collection of SingleIR.
 * This models ground sensors by assuming IR sensors are at a fixed distance
 * from the ground and returning activation depending on the luminance component
 * of the color of the object below the sensor. The SingleIR should have a
 * single ray (zero aperture).
 */
class SALSA_WSIM_API SimulatedIRGroundSensorController : public IRSensorController
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where the sensorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               sensorcontroller has been deleted)
	 * \param sensors the list of single proximity IR sensors initialization
	 *                parameters
	 * \param name the name of the object
	 */
	SimulatedIRGroundSensorController(World* world, SharedDataWrapper<Shared> shared, const QVector<SingleIR::InitParams>& sensors, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	~SimulatedIRGroundSensorController();

public:
	/**
	 * \brief Updates sensor reading
	 */
	virtual void update();

	/**
	 * \brief Sets whether the i-th sensor is active or not
	 *
	 * This automatically disables graphics for sensors that are not active
	 * \param i the index of the sensor
	 * \param active if true the sensor is active
	 */
	virtual void setSensorActive(int i, bool active);

	/**
	 * \brief Sets graphical properties of all sensors
	 *
	 * By default the sensor is not drawn
	 * \param drawSensor if true the sensor is drawn
	 * \param drawRay if true sensor rays are drawn
	 * \param drawRealRay if true the actual ray range is drawn, otherwise
	 *                    only the direction of the rays is given
	 */
	void setGraphicalProperties(bool drawSensor, bool drawRay = false, bool drawRealRay = false);

	/**
	 * \brief Returns a const reference to the list of SingleIR sensors
	 *
	 * \return a const reference to the list of SingleIR sensors
	 */
	const QVector<SingleIR*>& sensors() const
	{
		return m_sensors;
	}

private:
	/**
	 * \brief The list of single sensors
	 */
	QVector<SingleIR*> m_sensors;

	/**
	 * \brief Whether to draw sensors or not
	 */
	bool m_drawSensor;

	/**
	 * \brief When drawing sensors, whether to draw rays or not
	 */
	bool m_drawRay;

	/**
	 * \brief When drawing sensor rays, whether to use their real length or
	 *        not
	 */
	bool m_drawRealRay;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

/**
 * \brief The controller for traction sensor
 *
 * This class implements the controller for the traction sensor found e.g. on
 * the MarXbot robot
 */
class SALSA_WSIM_API TractionSensorController : public SensorController
{
protected:

	/**
	 * \brief Constructor
	 *
	 * \param world the world where the sensorcontroller lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               sensorcontroller has been deleted)
	 * \param j the joint where the traction sensor is attached
	 * \param name the name of the object
	 */
	TractionSensorController(World* world, SharedDataWrapper<Shared> shared, const PhyJoint* j, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	~TractionSensorController();

public:
	/**
	 * \brief Updates sensor reading
	 */
	virtual void update();

	/**
	 * \brief Returns the force applied on the joint
	 *
	 * \return the force applied on the joint
	 */
	wVector traction() const;

private:
	/**
	 * \brief The joint to which the sensor is attached
	 */
	const PhyJoint* const m_joint;

	/**
	 * \brief World is friend to be able to create us
	 */
	friend class World;
};

} // end namespace salsa

#endif
