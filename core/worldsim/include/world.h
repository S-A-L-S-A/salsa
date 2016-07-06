/********************************************************************************
 *  FARSA                                                                       *
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

#ifndef WORLD_H
#define WORLD_H

#include "worldsimconfig.h"
#include "wquaternion.h"
#include "componentresource.h"
#include <QString>
#include <QHash>
#include <QSet>
#include <QList>
#include <QLinkedList>
#include <QVector>
#include <QPair>
#include <QEvent>
#include <QTimerEvent>
#include <QColor>
#include <QImage>
#include <QMap>
#include "simpletimer.h"
#include "worldhelpers.h"

namespace farsa {

class World;
class WorldPrivate;
class PhyObject;
class PhyCompoundObject;
class PhyJoint;
class MotorController;
class SensorController;
class WEntity;
class AbstractRendererContainer;
class AbstractRenderWEntityCreator;

/**
 * \brief The class modelling the World
 *
 * This class represents the environment in which robots live. It contains all
 * objects, both physical and non physical. The time is discretized and it is
 * possible to set the time step of integration. This class has the methods to
 * tune the physical simulation and some utility functions to get contacts,
 * check collisions, perform ray casting and so on. All objects that live in the
 * World are subclasses of WEntity and MUST be created using the createEntity()
 * method of this class. Likewise, destruction MUST always happend through the
 * deleteEntity() function. This is needed so that entities receive a pointer to
 * the shared data object and to be able to create renderers. See the
 * description of those two methods for more information on how to use them.
 * This class is also needed to create container of renderers in a way similar
 * to WEntities. You must use the createRenderersContainer() function to create
 * them and the deleteRenderersContainer() to delete them.
 * This class is also needed to register textures. There are some pre-defined
 * textures that are always available, but you register more to be used on
 * entities. Note that pre-defined textures can be deleted; in particular
 * removeAllTextures() removes them, too.
 * Please note that this class is not thread safe and is not meant to be
 * inheritable!
 */
class FARSA_WSIM_API World : public Resource
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param worldname the reference name for the World
	 */
	World(QString worldname);

	/**
	 * \brief Destructor
	 */
	~World();

	/**
	 * \brief Returns the name of the world
	 *
	 * \return the name of the world
	 */
	QString name() const;

	/**
	 * \brief Resets the world
	 *
	 * This destroys everything in the world, then destroys the Newton world
	 * and recreates it. This will not remove renderers containers (but all
	 * renderers and textures will be removed)
	 */
	void reset();

	/**
	 * \brief Returns the time elapsed since world was create or the last
	 *        time the timer was reset
	 *
	 * \return the elapsed time
	 */
	real elapsedTime() const;

	/**
	 * \brief Resets the time counter and restart from zero seconds
	 */
	void resetElapsedTime();

	/**
	 * \brief Sets the time step in seconds
	 *
	 * \param timestep the timestep in seconds
	 */
	void setTimeStep(real timestep);

	/**
	 * \brief Sets the minimum frame rate
	 *
	 * This is the number of integration steps per second regardless the
	 * time step value
	 * \param frames the number of integration steps per second
	 */
	void setMinimumFrameRate(unsigned int frames);

	/**
	 * \brief Returns the time step
	 *
	 * \return the time step
	 */
	real timeStep() const;

	/**
	 * \brief Sets the gravitational acceleration
	 *
	 * \param g the value of the gravitational acceleration. This is the
	 *          value of the acceleration along the Z axis (positive
	 *          accelarations upward)
	 */
	void setGravitationalAcceleration(real g);

	/**
	 * \brief Returns the gravitational acceleration
	 *
	 * \return the value of the gravitational acceleration. This is the
	 *         value of the acceleration along the Z axis (positive
	 *         accelarations upward)
	 */
	real gravitationalAcceleration() const;

	/**
	 * \brief Returns the list of WEntities present in this world
	 *
	 * \return the list of WEntities present in this world
	 */
	QList<WEntity*> entities() const;

	/**
	 * \brief Returns the WEntity with specified name
	 *
	 * This is slow, its time complexity is O(n). Moreover this only returns
	 * the first object it finds with the given name, even if there are more
	 * than one. You should try to avoid using this function
	 * \param name the name to look for
	 * \return the WEntity with specified name, or NULL if no entity has
	 *         that name
	 */
	WEntity* getEntity(const QString& name);

	/**
	 * \brief Returns the map from PhyObjects to joints
	 *
	 * \return the map from PhyObjects to joints
	 */
	const QHash<PhyObject*, QList<PhyJoint*> >& mapObjectsToJoints() const;

	/**
	 * \brief Disables collisions between bodies
	 *
	 * This disables collisions between the given pair of bodies. Note that
	 * if you want create an object that doesn't collide with anything, you
	 * can set the object material to "nonCollidable"
	 * \param obj1 the first object
	 * \param obj2 the second object
	 */
	void disableCollisions(PhyObject* obj1, PhyObject* obj2);

	/**
	 * \brief Enables collisions between bodies
	 *
	 * This is the opposite of the disableCollision() function. If
	 * collisions between obj1 and obj2 are disabled by other means (e.g.
	 * with materials) the two objects will not collide
	 * \param obj1 the first object
	 * \param obj2 the second object
	 */
	void enableCollisions(PhyObject* obj1, PhyObject* obj2);

	/**
	 * \brief Returns the contact map
	 *
	 * \return the contact map
	 */
	const ContactMap& contacts() const;

	/**
	 * \brief Calculates the two closest points between the two objects
	 *
	 * If objects are collidings, points will be set to zero
	 * \param objA first object
	 * \param objB second object
	 * \param pointA the point on objA's shape closest to objB
	 * \param pointB the point on objB's shape closest to objA
	 * \return true is the calculation has been done, false if the object
	 *         are colliding
	 */
	bool closestPoints(PhyObject* objA, PhyObject* objB, wVector& pointA, wVector& pointB);

	/**
	 * \brief Checks whether two objects collide or not
	 *
	 * \param obj1 first object
	 * \param obj2 second object
	 * \param maxContacts the maxium number of contacts to report
	 * \param contacts if not null, the vector is filled with collision
	 *                 contact points in the world frame of reference
	 * \param normals if not null, the vector is filled with collision
	 *                contact normals in the world frame of reference
	 * \param penetra if not null, the vector is filled with collision
	 *                penetration at each point
	 * \return true if the two object are in contacts
	 */
	bool checkContacts(PhyObject* obj1, PhyObject* obj2, int maxContacts = 4, QVector<wVector>* contacts = NULL, QVector<wVector>* normals = NULL, QVector<real>* penetra = NULL);

	/**
	 * \brief Checks whether two objects collide or not
	 *
	 * This function uses checkContacts if at least one object is kinematic
	 * or if both objects are static, otherwise collision information are
	 * taken from contactMap
	 * \param obj1 first object
	 * \param obj2 second object
	 * \param maxContacts the maxium number of contacts to report
	 * \param contacts if not null, the vector is filled with collision
	 *                 contact points in the world frame of reference
	 * \return true if the two object are in contacts
	 */
	bool smartCheckContacts(PhyObject* obj1, PhyObject* obj2, int maxContacts = 4, QVector<wVector>* contacts = NULL);

	/**
	 * \brief Checks whether the ray from start to end intersects obj
	 *
	 * \param obj the object whose collision with the ray we have to check
	 * \param start the starting point of the ray (in the global frame of
	 *              reference)
	 * \param end the ending point of the ray (in the global frame of
	 *            reference)
	 * \param normal if not null the pointed wVector will store the normal
	 *               to the contact point
	 * \return a value between 0.0 and 1.0 indicating the point along the
	 *         ray which intersects the object (with 0.0 being the starting
	 *         point and 1.0 the ending point). If the ray misses the
	 *         objects, the returned value is greater than 1.0
	 */
	real collisionRayCast(PhyObject* obj, wVector start, wVector end, wVector* normal = NULL);

	/**
	 * \brief Returns the bodyes which intersect the ray from start to end
	 *
	 * \param start the starting point of the ray (in the global frame of
	 *              reference)
	 * \param end the ending point of the ray (in the global frame of
	 *            reference)
	 * \param onlyClosest if true only the object closest to the ray start
	 *                    is returned, otherwise all objects which intersect
	 *                    the ray are returned
	 * \param ignoredObjs the set of objects to ignore when checking
	 *                    collisions. They are never inserted in returned
	 *                    list of intersected objects. This means that if
	 *                    onlyClosest is true, the first colliding object
	 *                    that doesn't belong to this set is returned
	 * \return the list of objects intersected by the ray. If onlyClosest is
	 *         true the list contains at most one object (the intersected
	 *         one closest to the ray start). If no object is intersected by
	 *         the ray, an empty list is returned
	 */
	RayCastHitVector worldRayCast(wVector start, wVector end, bool onlyClosest, const QSet<PhyObject*>& ignoredObjs = QSet<PhyObject*>());

	/**
	 * \brief Returns the MaterialDB object managing World's materials
	 *
	 * \return the MaterialDB object managing World's materials
	 */
	MaterialDB& materials();

	/**
	 * \brief Returns the MaterialDB object managing World's materials
	 *        (const version)
	 *
	 * \return a const reference to the MaterialDB object managing World's
	 *         materials
	 */
	const MaterialDB& materials() const;

	/**
	 * \brief Sets the solver model
	 *
	 * See Physic Engine Documentation
	 * \param model the solver model, one of "exact" or "linear"
	 */
	void setSolverModel(QString model);

	/**
	 * \brief Sets the friction model
	 *
	 * See Physic Engine Documentation
	 * \param mdoel the friction model, one of "exact" or "linear"
	 */
	void setFrictionModel(QString model);

	/**
	 * \brief Sets the number of threads used internally by the physics
	 *        engine
	 *
	 * \param numThreads the number of threads used internally by the
	 *                   physics engine
	 */
	void setMultiThread(int numThreads);

	/**
	 * \brief Sets the size of the world
	 *
	 * See Physic Engine Documentation
	 * \param minPoint the minimum point of the world
	 * \param maxPoint the maximum point of the world
	 */
	void setSize(const wVector& minPoint, const wVector& maxPoint);

	/**
	 * \brief Gets the bounding box of the world
	 *
	 * \param minPoint the minimum point of the world
	 * \param maxPoint the maximum point of the world
	 */
	void size(wVector &minPoint, wVector &maxPoint);

	/**
	 * \brief Cleans up memory
	 */
	void cleanUpMemory();

	/**
	 * \brief Initialize the World
	 *
	 * This doesn't do anything if the world is already initialized.
	 * Moreover the world is automatically initialized the first time
	 * advance() is called if it hasn't been initialized yet
	 */
	void initialize();

	/**
	 * \brief Does a step of the World
	 *
	 * \note The sequence of actions here is: call preUpdate on all
	 *       Entities; perform the actual physical simulation step; call
	 *       postUpdate on all entities
	 */
	void advance();

	/**
	 * \brief The function to create an entity
	 *
	 * Use this function to create an entity that lives in this world. This
	 * function can be used when the entity constructor has only two
	 * parameters, the world and the pointer to the shared data. Use the
	 * overloads below if the entity constructor has more parameters. For
	 * example if you have an entity whose constructor is like the
	 * following:
	 *
	 * 	MyEntity(World* world, Shared* shared, float f, int k,
	 * 	         QString name = "unamed");
	 *
	 * you can create it in the following way:
	 *
	 * 	MyEntity* e = world->createEntity(TypeToCreate\<MyEntity\>(),
	 * 	                                  14.5, 7);
	 *
	 * or you can also specify the optional parameter name in the following
	 * way;
	 *
	 * 	MyEntity* e = world->createEntity(TypeToCreate\<MyEntity\>(),
	 * 	                                  14.5, 7, "entityName");
	 *
	 * Whe you have to delete the entity use the deleteEntity() function
	 * like this:
	 *
	 * 	world->deleteEntity(e);
	 *
	 * After a call to deleteEntity the pointer passed as argument ("e" in
	 * the example) is no longer valid
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \return a pointer to the new entity
	 */
	template <class T>
	T* createEntity(TypeToCreate<T> type)
	{
		// We cannot remove TypeToCreate because the customizer needs to be an l-value (cannot be a r-value,
		// i.e. we cannot call createEntity(WEntityCreationCustomizer<T>(), ...)
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \return a pointer to the new entity
	 */
	template <class T, class P0>
	T* createEntity(TypeToCreate<T> type, P0 p0)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1, class P2>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1, p2);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1, class P2, class P3>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1, p2, p3);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1, p2, p3, p4);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \param p5 the eighth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4, class P5>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1, p2, p3, p4, p5);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \param p5 the eighth parameter of the entity constructor
	 * \param p6 the ninth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1, p2, p3, p4, p5, p6);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \param p5 the eighth parameter of the entity constructor
	 * \param p6 the ninth parameter of the entity constructor
	 * \param p7 the tenth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	T* createEntity(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		WEntityCreationCustomizer<T> customizer;
		return createEntity(customizer, type, p0, p1, p2, p3, p4, p5, p6, p7);
	}

	/**
	 * \brief The function to create an entity
	 *
	 * This is the same ad the version that do not take a pointer to a
	 * WEntityCreationCustomizer object, but uses properties of the
	 * WEntityCreationCustomizer object to customize the creation process.
	 * You should rarely need this function. This function is for entities
	 * whose constructor only takes two parameters: the world and the shared
	 * data object
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \return a pointer to the new entity
	 */
	template <class CT, class T>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1, class P2>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1, class P2, class P3>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1, class P2, class P3, class P4>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \param p5 the eighth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1, class P2, class P3, class P4, class P5>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \param p5 the eighth parameter of the entity constructor
	 * \param p6 the ninth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

	/**
	 * \brief The function to create an entity
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param customizer the object that allows to customize object creation
	 * \param type an instance of the class TypeToCreate with the WEntity
	 *             subclass to create as the template parameter.
	 * \param p0 the third parameter of the entity constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the entity constructor
	 * \param p2 the fifth parameter of the entity constructor
	 * \param p3 the sixth parameter of the entity constructor
	 * \param p4 the seventh parameter of the entity constructor
	 * \param p5 the eighth parameter of the entity constructor
	 * \param p6 the ninth parameter of the entity constructor
	 * \param p7 the tenth parameter of the entity constructor
	 * \return a pointer to the new entity
	 */
	template <class CT, class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	T* createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

	/**
	 * \brief The function to delete an entity
	 *
	 * To delete an entity you either use this function or directly do
	 * delete entity. When this function returns, the pointer is no longer
	 * valid (the object has been destroyed). Note that the destruction of
	 * an entity may cause the destruction of other entities: notably owned
	 * objects may be deleted and if a PhyObject is attached to a joint, the
	 * joint is also destroyed.
	 * \param entity the entity to destroy
	 */
	void deleteEntity(WEntity* entity);

	/**
	 * \brief The function to create renderers containers
	 *
	 * Use this function to create a renderer container. This function can
	 * be used for classes that only take a pointer to the world in the
	 * constructor, for classes with more parameters use overloads.
	 * Containers created in this way must be deleted using the
	 * deleteRenderersContainer() function and are deleted if world is
	 * deleted
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \return a pointer to the new render container
	 */
	template<class T>
	T* createRenderersContainer(TypeToCreate<T> type);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \return a pointer to the new render container
	 */
	template<class T, class P0>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \param p2 the fourth parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1, class P2>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \param p2 the fourth parameter of the renderer container constructor
	 * \param p3 the fifth parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1, class P2, class P3>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \param p2 the fourth parameter of the renderer container constructor
	 * \param p3 the fifth parameter of the renderer container constructor
	 * \param p4 the sixth parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1, class P2, class P3, class P4>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \param p2 the fourth parameter of the renderer container constructor
	 * \param p3 the fifth parameter of the renderer container constructor
	 * \param p4 the sixth parameter of the renderer container constructor
	 * \param p5 the seventh parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1, class P2, class P3, class P4, class P5>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \param p2 the fourth parameter of the renderer container constructor
	 * \param p3 the fifth parameter of the renderer container constructor
	 * \param p4 the sixth parameter of the renderer container constructor
	 * \param p5 the seventh parameter of the renderer container constructor
	 * \param p6 the eighth parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

	/**
	 * \brief The function to create renderers containers
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the
	 *             AbstractRendererContainer subclass to create as the
	 *             template parameter.
	 * \param p0 the second parameter of the renderer container constructor
	 *           (the one after World)
	 * \param p1 the third parameter of the renderer container constructor
	 * \param p2 the fourth parameter of the renderer container constructor
	 * \param p3 the fifth parameter of the renderer container constructor
	 * \param p4 the sixth parameter of the renderer container constructor
	 * \param p5 the seventh parameter of the renderer container constructor
	 * \param p6 the eighth parameter of the renderer container constructor
	 * \param p7 the ninth parameter of the renderer container constructor
	 * \return a pointer to the new render container
	 */
	template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	T* createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

	/**
	 * \brief Deletes a renderer container
	 *
	 * To delete a renderer container you must use this function. When this
	 * function returns, the pointer is no longer valid (the object has been
	 * destroyed)
	 * \param rendererContainer the renderer container to destroy
	 */
	void deleteRenderersContainer(AbstractRendererContainer* c);

	/**
	 * \brief Adds one texture to the list of available ones
	 *
	 * If the texture already exists it is replaced
	 * \param name the name of the texture
	 * \param image the image for the texture
	 */
	void addTexture(const QString& name, const QImage& image);

	/**
	 * \brief Removes the texture with the given name
	 *
	 * If the texture doesn't exist, nothing happends
	 * \param name the name of the texture to remove
	 */
	void deleteTexture(const QString& name);

	/**
	 * \brief Removes all textures
	 */
	void deleteAllTextures();

private:
	void createWorld();
	void destroyWorld();

	// Checks if we are inside a call to createEntity() or to
	// createRenderersContainer() and resets the flag. When an entity is
	// created using the createEntity() or a RenderersContainer is created
	// using the createRenderersContainer() function, a flag is set to true.
	// This function returns the current value of the flag and then sets it
	// to false. This is used inside the constructor of WEntity and of
	// AbstractRendererContainer to check that they are created using World
	// functions and not directly (on the stack or the heap). The flag is
	// set to false also on exit from createEntity() or
	// createRenderersContainer()
	bool checkCreatingFromWorldAndResetFlag();

	// This function is called at the end of the creation of an entity to
	// perform type-specific initialization. This implementation does
	// nothing, only overloadings do useful things. This is not declared
	// like this:
	// 	template <class CT, class T>
	// 	void postCreationActionForType(WEntityCreationCustomizer<CT>& customizer, T* entity)
	// because if it is the compiler calls this instead of the correct
	// function. The ellipsis causes this to have the lowest possible
	// priority.
	void postCreationActionForType(...)
	{
	}

	// This function is called at the end of the creation of an entity to
	// perform type-specific initialization. This is used when joints are
	// created
	template <class CT>
	void postCreationActionForType(WEntityCreationCustomizer<CT>& customizer, PhyJoint* entity);

	// This is the non-template version of postCreationActionForType for
	// PhyJoints, needed because we cannot implement that function here
	// (there are problems with headers)
	void postCreationActionForType(PhyJoint* entity);

	// This function is called at the end of the creation of an entity to
	// perform type-specific initialization. This is used when PhyObjects
	// are created
	template <class CT>
	void postCreationActionForType(WEntityCreationCustomizer<CT>& customizer, PhyObject* entity);

	// This is the non-template version of postCreationActionForType for
	// PhyObjects, needed because we cannot implement that function here
	// (there are problems with headers). If onlyCreateCollisionShape is
	// true, only the collision shape should be created.
	// collisionShapeOffset the offset of the collision shape (can be NULL
	// if there is no offset)
	void postCreationActionForType(PhyObject* entity, bool onlyCreateCollisionShape, const wMatrix* collisionShapeOffset);

	// Notifies all RendererContainers that an entity has just been created.
	// rendererCreator is the renderer creator for the new entity
	void notifyRendererContainersOfNewEntity(AbstractRenderWEntityCreator* rendererCreator);

	// Notifies the renderer container of all entities and textures.
	// rendererContainer is the renderer container to be notified
	void notifyRendererContainerOfAllEntitiesAndTextures(AbstractRendererContainer* rendererContainer);

	// The name of the world
	const QString m_name;

	// The elapsed simulation time
	real m_time;

	// The time step
	real m_timestep;

	// The gravitational acceleration. This is the value of the acceleration
	// along the Z axis (positive accelerations upward)
	real m_g;

	// The minimum point of the Newton world
	wVector m_minP;

	// The maximum point of the Newton world
	wVector m_maxP;

	// The list of entities inside the World with related objects. Here we
	// use QLinkedList because we need to modify the list while iterating on
	// it. It could happend that during World destruction we destroy an
	// entity that owns other entities which follow it in the list. In this
	// situation the destructor of the entity would take care of destroying
	// owned entities. We must be able, then, to remove the entities from
	// this list, otherwise World destructor would try to access already
	// freed objects. If instead owned entities precede their owner they are
	// automatically skipped
	QLinkedList<WEntityAndBuddies> m_entities;

	// A map from entities to elements in the m_entities linked list
	QMap<WEntity*, QLinkedList<WEntityAndBuddies>::iterator> m_entitiesMap;

	// The list of entities inside the World with related objects that are
	// not "officially" in the list of World objects. There are the objects
	// for which at construction time the customizer addToWorldLists()
	// function returned false. We need to keep a list of them anyway to be
	// able to delete them when needed
	QLinkedList<WEntityAndBuddies> m_shadowEntities;

	// A map from entities to elements in the m_shadowEntities linked list
	QMap<WEntity*, QLinkedList<WEntityAndBuddies>::iterator> m_shadowEntitiesMap;

	// The list of renderers containers
	QList<AbstractRendererContainer*> m_renderersContainers;

	// The map of PhyObjects to joints
	QHash<PhyObject*, QList<PhyJoint*> > m_mapObjJoints;

	// The flag which is set to true in calls to createEntity() and
	// createRenderersContainer(). See the description of
	// checkCreatingFromWorldAndResetFlag() for more information
	bool m_creatingSomething;

	// The map of contacts
	ContactMap m_cmap;

	// The type for a couple of objects that doesn't collide each other
	typedef QPair<PhyObject*, PhyObject*> NObj;

	// The pairs of non colliding objects
	QSet<NObj> m_nobjs;

	// The database of materials
	std::auto_ptr<MaterialDB> m_mats;

	// This is true if we have been initialized
	bool m_isInitialized;

	// Engine encapsulation
	std::auto_ptr<WorldPrivate> m_priv;

	// The map of textures (the key is the name of the texture)
	QMap<QString, QImage> m_textures;

	// AbstractRendererContainer is friend to call
	// checkCreatingFromWorldAndResetFlag()
	friend class AbstractRendererContainer;

	// WEntity is friend to call checkCreatingFromWorldAndResetFlag()
	friend class WEntity;

	// MaterialDB is friend to access the physical engine
	friend class MaterialDB;

	// PhyObject is friend to access the physical engine
	friend class PhyObject;

	// PhyCompoundObject is friend to call
	// onlyCreateCollisionOfNextPhyObject()
	friend class PhyCompoundObject;

	// PhyJoint is friend to access the physical engine
	friend class PhyJoint;

	// WorldPrivate is friend to access the physical engine
	friend class WorldPrivate;
};

} // end namespace farsa

#include "renderingproxy.h"
#include "wentity.h"
#include "rendererscontainer.h"

// Implementation of template functions
namespace farsa
{
template <class CT, class T>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type)
{
#define __FARSA_WORLD_PRE_CREATE_ENTITY Q_UNUSED(type);\
                                        m_creatingSomething = true;\
                                        WEntityAndBuddies e;\
                                        typename T::Shared* sharedData = new typename T::Shared();\
                                        e.sharedDataHolder = new SharedDataHolder<typename T::Shared>(sharedData); \
                                        SharedDataWrapper<typename T::Shared> sharedDataWrapper(sharedData);
#define __FARSA_WORLD_POST_CREATE_ENTITY customizer.immediatePostCreationAction(entity);\
                                         e.entity = entity;\
                                         e.rendererCreator = new RenderWEntityCreator<T>(entity);\
                                         if (customizer.addToWorldLists()) {\
                                                 QLinkedList<WEntityAndBuddies>::iterator it = m_entities.insert(m_entities.end(), e);\
                                                 m_entitiesMap[e.entity] = it;\
                                         } else {\
                                                 QLinkedList<WEntityAndBuddies>::iterator it = m_shadowEntities.insert(m_entities.end(), e);\
                                                 m_shadowEntitiesMap[e.entity] = it;\
                                         }\
                                         postCreationActionForType(customizer, entity);\
                                         m_creatingSomething = false;\
                                         if (customizer.addToWorldLists()) {\
                                                 notifyRendererContainersOfNewEntity(e.rendererCreator);\
                                         }\
                                         customizer.setWEntityAndBuddies(e);\
                                         customizer.delayedPostCreationAction(entity);\
                                         return entity;

	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1, class P2>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1, p2);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1, class P2, class P3>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1, p2, p3);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1, class P2, class P3, class P4>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1, p2, p3, p4);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1, class P2, class P3, class P4, class P5>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1, p2, p3, p4, p5);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1, p2, p3, p4, p5, p6);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template <class CT, class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
T* World::createEntity(WEntityCreationCustomizer<CT>& customizer, TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
	__FARSA_WORLD_PRE_CREATE_ENTITY

	T* const entity = new T(this, sharedDataWrapper, p0, p1, p2, p3, p4, p5, p6, p7);

	__FARSA_WORLD_POST_CREATE_ENTITY
}

template<class T>
T* World::createRenderersContainer(TypeToCreate<T> type)
{
#define __FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER Q_UNUSED(type);\
                                                    m_creatingSomething = true;
#define __FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER m_renderersContainers.append(c);\
                                                     m_creatingSomething = false;\
                                                     notifyRendererContainerOfAllEntitiesAndTextures(c);\
                                                     return c;

	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1, class P2>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1, p2);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1, class P2, class P3>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1, p2, p3);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1, class P2, class P3, class P4>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1, p2, p3, p4);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1, class P2, class P3, class P4, class P5>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1, p2, p3, p4, p5);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1, p2, p3, p4, p5, p6);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
T* World::createRenderersContainer(TypeToCreate<T> type, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
	__FARSA_WORLD_PRE_CREATE_RENDERER_CONTAINER

	T* const c = new T(this, p0, p1, p2, p3, p4, p5, p6, p7);

	__FARSA_WORLD_POST_CREATE_RENDERER_CONTAINER
}

template <class CT>
void World::postCreationActionForType(WEntityCreationCustomizer<CT>&, PhyJoint* entity)
{
	// Calling the non-template function
	postCreationActionForType(entity);
}

template <class CT>
void World::postCreationActionForType(WEntityCreationCustomizer<CT>& customizer, PhyObject* entity)
{
	// Calling the non-template function
	postCreationActionForType(entity, customizer.onlyCreateCollisionShape(), customizer.usingCustomCollisionShapeOffset() ? &(customizer.customCollisionShapeOffset()) : NULL);
}

}

#endif
