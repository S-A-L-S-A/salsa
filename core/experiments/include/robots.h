/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#ifndef ROBOTS_H
#define ROBOTS_H

#include "experimentsconfig.h"
#include "component.h"
#include "phymarxbot.h"
#include "phyepuck.h"
#include "phykhepera.h"
#include "wheeledexperimenthelper.h"
#include "configurationmanager.h"

namespace farsa {

class Box2DWrapper;

/**
 * \brief An abstract class for robots
 *
 * This only reads two parameters, the robot name and the initial transformation
 * matrix
 */
class FARSA_EXPERIMENTS_API Robot : public Component
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object with parameters to
	 *               use
	 */
	Robot(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~Robot();

	/**
	 * \brief Describes of all parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

protected:
	const QString m_initialName;
	const wMatrix m_initialTm;
};

/**
 * \brief The base class for robots that move on a plane
 *
 * This has some utility methods common to all robots that move on a plane.
 * There are also protected static helper methods which can be used when
 * re-implementing in subclasses. The only parameter of this class is the color
 * of the robot (white by default)
 */
class FARSA_EXPERIMENTS_API RobotOnPlane : public Robot
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object with parameters to
	 *               use
	 */
	RobotOnPlane(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~RobotOnPlane();

	/**
	 * \brief Describes of all parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Sets the position of the robot in the plane
	 *
	 * The Z coordinate is discarded. The robot is placed on the face of the
	 * "plane" parallel to the local XY plane and with positive z
	 * coordinate. This method simply calls setPosition(real x, real y)
	 * \param plane the plane on which the robot should be placed. You can
	 *              use the plane returned by Arena::getPlane(), here.
	 * \param pos the new position (the z coordinate is discarded)
	 */
	void setPosition(const Box2DWrapper* plane, const wVector& pos);

	/**
	 * \brief Sets the position of the robot in the plane
	 *
	 * The robot is placed on the face of the "plane" parallel to the local
	 * XY plane and with positive z coordinate.
	 * \param plane the plane on which the robot should be placed. You can
	 *              use the plane returned by Arena::getPlane(), here.
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(const Box2DWrapper* plane, real x, real y) = 0;

	/**
	 * \brief Returns the position of the robot
	 *
	 * \return the position of the robot
	 */
	virtual wVector position() const = 0;

	/**
	 * \brief Sets the orientation of the robot
	 *
	 * This modifies the angle around the z axis of the "plane". An angle of
	 * 0 means that the X axis of the plane and of the robot are coincident
	 * (positive angles follow the right-hand rule). This function wants the
	 * angle in radiants
	 * \param plane the plane on which the robot is placed. You can use the
	 *              plane returned by Arena::getPlane(), here.
	 * \param angle the new orientation in radiants
	 */
	virtual void setOrientation(const Box2DWrapper* plane, real angle) = 0;

	/**
	 * \brief Returns the orientation of the robot
	 *
	 * This returns the angle around the z axis of the "plane". An angle of
	 * 0 means that the X axis of the plane and of the robot are coincident
	 * (positive angles follow the right-hand rule). This function returns
	 * the angle in radiants
	 * \param plane the plane on which the robot is placed. You can use the
	 *              plane returned by Arena::getPlane(), here.
	 * \warning the orientation returned by this function is not the same as
	 *          the orientation set by setOrientation, there is a difference
	 *          of (pi/2)
	 */
	virtual real orientation(const Box2DWrapper* plane) const = 0;

	/**
	 * \brief Returns the height of the robot
	 *
	 * This is the height of the cylinder containing the robot. In some
	 * cases wheeled robots are modelled as cylinders to simplify
	 * calculations
	 * \return the height of the robot
	 */
	virtual real robotHeight() const = 0;

	/**
	 * \brief Returns the radius of the robot
	 *
	 * This is the radius of the cylinder containing the robot. In some
	 * cases wheeled robots are modelled as cylinders to simplify
	 * calculations
	 * \return the radius of the robot
	 */
	virtual real robotRadius() const = 0;

	/**
	 * \brief Returns true if the robot is in kinematic mode
	 *
	 * \return true if the robot is in kinematic mode
	 */
	virtual bool isKinematic() const = 0;

	/**
	 * \brief Returns the color of the robot
	 *
	 * \return the color of the robot
	 */
	virtual QColor robotColor() const = 0;

protected:
	/**
	 * \brief Returns the robot color specified by the configuration
	 *        parameter
	 *
	 * \return the robot color specified by the configuration parameter
	 */
	const QColor& configuredRobotColor() const
	{
		return m_color;
	}

private:
	const QColor m_color;
};

/**
 * \brief The creator for robots
 *
 * This is needed because robots inherit from they Phy counterpart, which must
 * be created through world. When a robot is instantiated, a "world" resource
 * must exist
 */
template <class RobotType>
class FARSA_CONF_TEMPLATE RobotCreator : public ComponentCreator
{
private:
	// This uses the World instance provided by the "world" resource to
	// create the robot
	virtual Component* instantiate(ConfigurationManager& settings, QString, ResourceAccessor* accessorForResources)
	{
		World* w = accessorForResources->getResource<World>("world");

		return w->createEntity(TypeToCreate<RobotType>(), settings);
	}
};

// /**
//  * \brief The class modelling the marXbot robot
//  *
//  * This inherits from PhyMarXbot, so you can use all its methods
//  */
// class FARSA_EXPERIMENTS_API MarXbot : public RobotOnPlane, public PhyMarXbot
// {
// public:
//
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param params the configuration parameters object with parameters to
// 	 *               use
// 	 * \param prefix the prefix to use to access the object configuration
// 	 *               parameters. This is guaranteed to end with the
// 	 *               separator character when called by the factory, so you
// 	 *               don't need to add one
// 	 */
// 	MarXbot(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Saves the actual status of parameters into the
// 	 *        ConfigurationParameters object passed
// 	 *
// 	 * This is not implemented, calling this causes an abort
// 	 * \param params the configuration parameters object on which save
// 	 *               actual parameters
// 	 * \param prefix the prefix to use to access the object configuration
// 	 *               parameters.
// 	 */
// 	virtual void save(ConfigurationParameters& params, QString prefix);
//
// 	/*!
// 	 * \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups
// 	 * \param type is the name of the type regarding the description. The type is used when a subclass
// 	 * reuse the description of its parent calling the parent describe method passing the type of
// 	 * the subclass. In this way, the result of the method describe of the parent will be the addition
// 	 * of the description of the parameters of the parent class into the type of the subclass
// 	 */
// 	static void describe(QString type);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~MarXbot();
//
// 	/**
// 	 * \brief Sets the position of the robot in the plane
// 	 *
// 	 * The robot is placed on the face of the "plane" parallel to the local
// 	 * XY plane and with positive z coordinate.
// 	 * \param plane the plane on which the robot should be placed. You can
// 	 *              use the plane returned by Arena::getPlane(), here.
// 	 * \param x the new x coordinate
// 	 * \param y the new y coordinate
// 	 */
// 	virtual void setPosition(const Box2DWrapper* plane, real x, real y);
//
// 	/**
// 	 * \brief Returns the position of the robot
// 	 *
// 	 * \return the position of the robot
// 	 */
// 	virtual wVector position() const;
//
// 	/**
// 	 * \brief Sets the orientation of the robot
// 	 *
// 	 * This modifies the angle around the z axis of the "plane". An angle of
// 	 * 0 means that the X axis of the plane and of the robot are coincident
// 	 * (positive angles follow the right-hand rule). This function wants the
// 	 * angle in radiants
// 	 * \param plane the plane on which the robot is placed. You can use the
// 	 *              plane returned by Arena::getPlane(), here.
// 	 * \param angle the new orientation in radiants
// 	 */
// 	virtual void setOrientation(const Box2DWrapper* plane, real angle);
//
// 	/**
// 	 * \brief Returns the orientation of the robot
// 	 *
// 	 * This returns the angle around the z axis of the "plane". An angle of
// 	 * 0 means that the X axis of the plane and of the robot are coincident
// 	 * (positive angles follow the right-hand rule). This function returns
// 	 * the angle in radiants
// 	 * \param plane the plane on which the robot is placed. You can use the
// 	 *              plane returned by Arena::getPlane(), here.
// 	 * \warning the orientation returned by this function is not the same as
// 	 *          the orientation set by setOrientation, there is a difference
// 	 *          of (pi/2)
// 	 */
// 	virtual real orientation(const Box2DWrapper* plane) const;
//
// 	/**
// 	 * \brief Returns the height of the robot
// 	 *
// 	 * This is the height of the cylinder containing the robot. In some
// 	 * cases wheeled robots are modelled as cylinders to simplify
// 	 * calculations
// 	 * \return the height of the robot
// 	 */
// 	virtual real robotHeight() const;
//
// 	/**
// 	 * \brief Returns the radius of the robot
// 	 *
// 	 * This is the radius of the cylinder containing the robot. In some
// 	 * cases wheeled robots are modelled as cylinders to simplify
// 	 * calculations
// 	 * \return the radius of the robot
// 	 */
// 	virtual real robotRadius() const;
//
// 	/**
// 	 * \brief Returns true if the robot is in kinematic mode
// 	 *
// 	 * \return true if the robot is in kinematic mode
// 	 */
// 	virtual bool isKinematic() const;
//
// 	/**
// 	 * \brief Returns the color of the robot
// 	 *
// 	 * \return the color of the robot
// 	 */
// 	virtual QColor robotColor() const;
// };
//
// /**
//  * \brief The class modelling the e-puck robot
//  *
//  * This inherits from PhyEpuck, so you can use all its methods
//  */
// class FARSA_EXPERIMENTS_API Epuck : public RobotOnPlane, public PhyEpuck
// {
// public:
//
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param params the configuration parameters object with parameters to
// 	 *               use
// 	 * \param prefix the prefix to use to access the object configuration
// 	 *               parameters. This is guaranteed to end with the
// 	 *               separator character when called by the factory, so you
// 	 *               don't need to add one
// 	 */
// 	Epuck(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Saves the actual status of parameters into the
// 	 *        ConfigurationParameters object passed
// 	 *
// 	 * This is not implemented, calling this causes an abort
// 	 * \param params the configuration parameters object on which save
// 	 *               actual parameters
// 	 * \param prefix the prefix to use to access the object configuration
// 	 *               parameters.
// 	 */
// 	virtual void save(ConfigurationParameters& params, QString prefix);
//
// 	/*!
// 	 * \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups
// 	 * \param type is the name of the type regarding the description. The type is used when a subclass
// 	 * reuse the description of its parent calling the parent describe method passing the type of
// 	 * the subclass. In this way, the result of the method describe of the parent will be the addition
// 	 * of the description of the parameters of the parent class into the type of the subclass
// 	 */
// 	static void describe(QString type);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~Epuck();
//
// 	/**
// 	 * \brief Sets the position of the robot in the plane
// 	 *
// 	 * The robot is placed on the face of the "plane" parallel to the local
// 	 * XY plane and with positive z coordinate.
// 	 * \param plane the plane on which the robot should be placed. You can
// 	 *              use the plane returned by Arena::getPlane(), here.
// 	 * \param x the new x coordinate
// 	 * \param y the new y coordinate
// 	 */
// 	virtual void setPosition(const Box2DWrapper* plane, real x, real y);
//
// 	/**
// 	 * \brief Returns the position of the robot
// 	 *
// 	 * \return the position of the robot
// 	 */
// 	virtual wVector position() const;
//
// 	/**
// 	 * \brief Sets the orientation of the robot
// 	 *
// 	 * This modifies the angle around the z axis of the "plane". An angle of
// 	 * 0 means that the X axis of the plane and of the robot are coincident
// 	 * (positive angles follow the right-hand rule). This function wants the
// 	 * angle in radiants
// 	 * \param plane the plane on which the robot is placed. You can use the
// 	 *              plane returned by Arena::getPlane(), here.
// 	 * \param angle the new orientation in radiants
// 	 */
// 	virtual void setOrientation(const Box2DWrapper* plane, real angle);
//
// 	/**
// 	 * \brief Returns the orientation of the robot
// 	 *
// 	 * This returns the angle around the z axis of the "plane". An angle of
// 	 * 0 means that the X axis of the plane and of the robot are coincident
// 	 * (positive angles follow the right-hand rule). This function returns
// 	 * the angle in radiants
// 	 * \param plane the plane on which the robot is placed. You can use the
// 	 *              plane returned by Arena::getPlane(), here.
// 	 * \warning the orientation returned by this function is not the same as
// 	 *          the orientation set by setOrientation, there is a difference
// 	 *          of (pi/2)
// 	 */
// 	virtual real orientation(const Box2DWrapper* plane) const;
//
// 	/**
// 	 * \brief Returns the height of the robot
// 	 *
// 	 * This is the height of the cylinder containing the robot. In some
// 	 * cases wheeled robots are modelled as cylinders to simplify
// 	 * calculations
// 	 * \return the height of the robot
// 	 */
// 	virtual real robotHeight() const;
//
// 	/**
// 	 * \brief Returns the radius of the robot
// 	 *
// 	 * This is the radius of the cylinder containing the robot. In some
// 	 * cases wheeled robots are modelled as cylinders to simplify
// 	 * calculations
// 	 * \return the radius of the robot
// 	 */
// 	virtual real robotRadius() const;
//
// 	/**
// 	 * \brief Returns true if the robot is in kinematic mode
// 	 *
// 	 * \return true if the robot is in kinematic mode
// 	 */
// 	virtual bool isKinematic() const;
//
// 	/**
// 	 * \brief Returns the color of the robot
// 	 *
// 	 * \return the color of the robot
// 	 */
// 	virtual QColor robotColor() const;
// };

/**
 * \brief The class modelling the Khepera robot
 *
 * This inherits from PhyKhepera, so you can use all its methods
 */
class FARSA_EXPERIMENTS_API Khepera : public RobotOnPlane, public PhyKhepera
{
public:
	/**
	 * \brief The type of the class creating this robot
	 */
	typedef RobotCreator<Khepera> Creator;

public:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where robot lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param params the configuration parameters object with parameters to
	 *               use
	 */
	Khepera(World* world, SharedDataWrapper<Shared> shared, ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~Khepera();

	/**
	 * \brief Describes of all parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Sets the position of the robot in the plane
	 *
	 * The robot is placed on the face of the "plane" parallel to the local
	 * XY plane and with positive z coordinate.
	 * \param plane the plane on which the robot should be placed. You can
	 *              use the plane returned by Arena::getPlane(), here.
	 * \param x the new x coordinate
	 * \param y the new y coordinate
	 */
	virtual void setPosition(const Box2DWrapper* plane, real x, real y);

	/**
	 * \brief Returns the position of the robot
	 *
	 * \return the position of the robot
	 */
	virtual wVector position() const;

	/**
	 * \brief Sets the orientation of the robot
	 *
	 * This modifies the angle around the z axis of the "plane". An angle of
	 * 0 means that the X axis of the plane and of the robot are coincident
	 * (positive angles follow the right-hand rule). This function wants the
	 * angle in radiants
	 * \param plane the plane on which the robot is placed. You can use the
	 *              plane returned by Arena::getPlane(), here.
	 * \param angle the new orientation in radiants
	 */
	virtual void setOrientation(const Box2DWrapper* plane, real angle);

	/**
	 * \brief Returns the orientation of the robot
	 *
	 * This returns the angle around the z axis of the "plane". An angle of
	 * 0 means that the X axis of the plane and of the robot are coincident
	 * (positive angles follow the right-hand rule). This function returns
	 * the angle in radiants
	 * \param plane the plane on which the robot is placed. You can use the
	 *              plane returned by Arena::getPlane(), here.
	 * \warning the orientation returned by this function is not the same as
	 *          the orientation set by setOrientation, there is a difference
	 *          of (pi/2)
	 */
	virtual real orientation(const Box2DWrapper* plane) const;

	/**
	 * \brief Returns the height of the robot
	 *
	 * This is the height of the cylinder containing the robot. In some
	 * cases wheeled robots are modelled as cylinders to simplify
	 * calculations
	 * \return the height of the robot
	 */
	virtual real robotHeight() const;

	/**
	 * \brief Returns the radius of the robot
	 *
	 * This is the radius of the cylinder containing the robot. In some
	 * cases wheeled robots are modelled as cylinders to simplify
	 * calculations
	 * \return the radius of the robot
	 */
	virtual real robotRadius() const;

	/**
	 * \brief Returns true if the robot is in kinematic mode
	 *
	 * \return true if the robot is in kinematic mode
	 */
	virtual bool isKinematic() const;

	/**
	 * \brief Returns the color of the robot
	 *
	 * \return the color of the robot
	 */
	virtual QColor robotColor() const;
};

} //end namespace farsa

#endif
