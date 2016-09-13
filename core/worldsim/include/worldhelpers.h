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

#ifndef WORLDHELPERS_H
#define WORLDHELPERS_H

#include "worldsimconfig.h"
#include "wquaternion.h"
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

namespace salsa {

class World;
class WorldPrivate;
class PhyObject;
class PhyJoint;
class MotorController;
class SensorController;
class WEntity;
class WEntityShared;
class AbstractRendererContainer;
class AbstractRenderWEntityCreator;

/**
 * \brief Coputes an hash  for a pair of PhyObjects*
 *
 * This function is required by QSet\<NObj\> attribute of World class
 * \param pair the pair of PhyObjects
 * \return an hash for the pair
 */
inline uint SALSA_WSIM_TEMPLATE qHash( const QPair<PhyObject*, PhyObject*>& pair )
{
#ifdef _LP64
	return ( ::qHash( (long int)(pair.first) ) );
#else
	return ( ::qHash( (int)(pair.first) ) );
#endif
}

/*! \brief the MaterialDB class manages the material properties
 *
 *  It's not possibile to instantiate it, but it's automatically create when a World is created.
 *  For accessing it, use the World::materials() method.<br>
 *  there are some predefined materials:<br>
 *  # <b>Default:</b> with default properties of physic engine used
 *  # <b>nonCollidable:</b> a material that doesn't collide with anything
 */
class SALSA_WSIM_API MaterialDB {
public:
	/*! return the world associate with these materials */
	World* world() {
		return worldv;
	};

	/*! Create a new material
	 *  It return false if already exists a material with name passed
	 */
	bool createMaterial( QString name );

	/*! configure the default Friction between materials specified
	 *  \param st Static Friction coefficient
	 *  \param kn Kinetic Friction coefficient
	 *  the order doesn't matter
	 */
	void setFrictions( QString mat1, QString mat2, real st, real kn );

	/*! configure the default Elasticity between materials specified
	 *  the order doesn't matter
	 */
	void setElasticity( QString mat1, QString mat2, real );

	/*! configure the default Softness between materials specified
	 *  the order doesn't matter
	 */
	void setSoftness( QString mat1, QString mat2, real );
	/*! configure the force gravity that will be applied to object of material specified
	 *  \param force the gravity force in m/s^2
	 */
	void setGravitationalAcceleration( QString mat, real force );
	/*! Return the corresponding gravity force setted
	 *  If no setted, that World::gravityForce will be returned */
	real gravitationalAcceleration( QString mat );

	/*! Enable/Disable the collision between materials passed
	 *
	 *  the order doesn't matter
	 */
	void enableCollision( QString mat1, QString mat2, bool enable = true );

	/*! set Frictions, Elasticity, Softness and Enabled/Disable collision between materials
	 *  \param fs Static Friction coefficient
	 *  \param fk Kinetic Friction coefficient
	 *  \param el Elastiticy coefficient
	 *  \param sf Softness coefficient
	 *  \param en Enable/Disable collisions
	 */
	void setProperties( QString mat1, QString mat2, real fs, real fk, real el, real sf, bool en = true );

private:
	/*! constructor */
	MaterialDB( World* );
	// Creates the initial materials, this is called by World constructor
	void createInitialMaterials();
	/*! simple inner class for storing material properties */
	class materialPairInfo {
	public:
		real staticFriction;
		real dynamicFriction;
		real elasticity;
		real softness;
		bool collisions;
	};
	/*! material DB names */
	QSet<QString> mats;
	/*! Gravity force informations */
	QMap<QString, real> gravities;

	/*! material pair info map
	 *
	 *  Because material properties are symmetric (i.e static friction between "wood"-"metal"
	 *  is equal to static friction between "metal"-"wood"), the key for storing the information about
	 *  two material 'mat1' and 'mat2' is the string 'matA-matB' where matA is the first in alphabetical
	 *  order and matB the second one.
	 *  In this way, doesn't matter the order of material in above methods because they points to the
	 *  same information stored in pmap;
	 *
	 */
	QHash<QString, materialPairInfo> pmap;

	/*! helper function for create the key as described above */
	QString createKey( QString mat1, QString mat2 );

	/*! World */
	World* worldv;
	friend class World;
};


/*!  Contact class
 *
 *  \par Motivation
 *  an helper class for support the implementation of contact sensors
 *  \par Description
 *  the contact is refered to one object... so there is always a duplicate informations;
 *  for example, if object A and B collide each other then two Contact object will be
 *  generated: one for registering data from the A point of view and the other respect to object B.<br>
 *  This simplify the contact look-up based on the object pointer.
 */
class SALSA_WSIM_TEMPLATE Contact {
public:
	/*! Reference Object */
	PhyObject* object;
	/*! Collide Object */
	PhyObject* collide;
	/*! position of the contact in the local object coordinates */
	wVector pos;
	/*! position in the global coordinate */
	wVector worldPos;
	/*! the normal restitution force at the contact point */
	wVector force;
};

/*! Vector of Contacts */
typedef QVector<Contact> ContactVec;
/*! the mapping object -> contact informations */
typedef QHash<PhyObject*, ContactVec> ContactMap;
/*! cons Iterator */
typedef QHashIterator<PhyObject*, ContactVec> ContactMapIterator;


/*!  Raycast intersection class
 *
 *  \par Motivation
 *  an helper class with information about a raycast hit
 *  \par Description
 *  a list of objects of this type is returned by the World::worldRayCast()
 *  function. This class contains information about a RayCast hit: the object
 *  intersecting the ray, the distance from the ray start (normalized between
 *  0 and 1), the actual point at which the ray hits the object and the normal
 *  at contact point
 */
class SALSA_WSIM_TEMPLATE RayCastHit {
public:
	/*! Object intersected by the ray */
	PhyObject* object;
	/*! The distance from start at which the ray intersects the object. This
	    is in the range [0; 1]: 0 means the ray start point, 1 the ray end
	    point. Intermediate values correspond to values along the ray */
	real distance;
	/*! The actual point at which the ray intersects the object in the
	    global frame of reference */
	wVector position;
	/*! The normal at the hit point in the global frame of reference */
	wVector normal;
};

/*! Vector of RayCastHit */
typedef QVector<RayCastHit> RayCastHitVector;

/**
 * \brief The class to specify the type of objects to create
 *
 * Use instances of this class to specify the type of object to create when
 * calling World::createEntity() or World::createRenderersContainer, passing it
 * as the first parameter. This is needed to avoid having to specify the type of
 * all parameters of constructors. See World description for more information
 */
template <class Type_t>
class SALSA_WSIM_TEMPLATE TypeToCreate
{
public:
	/**
	 * \brief The type carried by this class
	 */
	typedef Type_t Type;
};


/**
 * \brief The class to delete shared data instances
 *
 * This is the abstract class for deleting the shared data instances of
 * entities. It is needed because shared data are not virtual classes,
 * so we should delete the correct pointer type
 */
class SALSA_WSIM_TEMPLATE AbstractSharedDataHolder
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~AbstractSharedDataHolder()
	{
	}

	/**
	 * \brief Returns a pointer to data
	 *
	 * \return a pointer to data
	 */
	virtual WEntityShared* data() = 0;

	/**
	 * \brief Returns a const pointer to data
	 *
	 * \return a const pointer to data
	 */
	virtual const WEntityShared* data() const = 0;
};

/**
 * \brief The concrete template subclass of AbstractSharedDataHolder
 *
 * Type is the type of the shared data instance to delete
 */
template <class Type>
class SALSA_WSIM_TEMPLATE SharedDataHolder : public AbstractSharedDataHolder
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param data the shared data object to destroy
	 */
	SharedDataHolder(Type* data)
		: AbstractSharedDataHolder()
		, m_data(data)
	{
	}

	/**
	 * \brief Destructor
	 *
	 * Frees memory for data
	 */
	virtual ~SharedDataHolder()
	{
		delete m_data;
	}

	/**
	 * \brief Returns a pointer to data
	 *
	 * \return a pointer to data
	 */
	virtual WEntityShared* data()
	{
		return m_data;
	}

	/**
	 * \brief Returns a const pointer to data
	 *
	 * \return a const pointer to data
	 */
	virtual const WEntityShared* data() const
	{
		return m_data;
	}

private:
	/**
	 * \brief The shared data object to destroy
	 */
	Type* const m_data;
};

/**
 * \brief The structure with the entity and related objects
 *
 * This structure contains an entity, the shared data holder and the
 * factory for renderers and renderproxies for the entity
 */
struct SALSA_WSIM_TEMPLATE WEntityAndBuddies
{
	/**
	 * \brief The entity
	 */
	WEntity* entity;

	/**
	 * \brief The object responsible of deleting the shared data for
	 *        the entity
	 */
	AbstractSharedDataHolder* sharedDataHolder;

	/**
	 * \brief The class to generate renderers and instance of
	 *        AbstractRenderingProxy for the entity
	 */
	AbstractRenderWEntityCreator* rendererCreator;
};

/**
 * \brief The abstract base class for classes that can be used to cutomize the
 *        creation process of WEntities
 *
 * This class allows customizing the process of entity creation to a certain
 * degree. You should generally not need it, but it is used in Worldsim for some
 * special situations (e.g. when creating components of a PhyCompoundObject).
 * You can use this class directly or subclass it to implement functions called
 * at specific points during the creation of the WEntity (the default
 * implementation is empty). The template parameter Type_t is the type of the
 * WEntity that is created (can also be a superclass). The things you can
 * control using this class are:
 * 	- whether to add the entity to internal lists of World or not. If you
 * 	  decide not to add it, the preUpdate() and postUpdate() function are
 * 	  not called and the object is not present in the list of entities
 * 	  returned by World. Moreover, the object is not rendered. The object,
 * 	  however, is still deleted when World is destroyed and, if physical, it
 * 	  interacts with other objects (it is in the physical engine). If it is
 * 	  a joint it is destroyed if one of the linked object is destroyed;
 * 	- execute custom code just after the object has been created (overriding
 * 	  the immediatePostCreationAction() function) or before the
 * 	  World::createEntity() function returns (overriding the
 * 	  delayedPostCreationAction() function; this function can call
 * 	  entityAndBuddies() if needed);
 * 	- set custom parameters for PhyObject::createPrivateObject(), i.e.
 * 	  whether to create only the collision shape (not the body) and the
 * 	  offset of the collision shape;
 * 	- whether the entity should be rendered or not.
 * Moreover, after World::createEntity() returns you have access to the objects
 * in the WEntityAndBuddies structure of World (be careful! Do not delete any
 * object!)
 */
template <class Type_t>
class SALSA_WSIM_TEMPLATE WEntityCreationCustomizer
{
public:
	/**
	 * \brief The type carried by this class
	 */
	typedef Type_t Type;

public:
	/**
	 * \brief Constructor
	 */
	WEntityCreationCustomizer()
		: m_addToWorldLists(true)
		, m_onlyCreateCollisionShape(false)
		, m_customCollisionShapeOffset(wMatrix::identity())
		, m_usingCustomCollisionShapeOffset(false)
		, m_entityAndBuddies()
	{
		m_entityAndBuddies.entity = nullptr;
		m_entityAndBuddies.sharedDataHolder = nullptr;
		m_entityAndBuddies.rendererCreator = nullptr;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~WEntityCreationCustomizer()
	{
	}

	/**
	 * \brief Sets whether the entity should be added to the internal lists
	 *        of World or not
	 *
	 * See class description for more information
	 * \param addToLists whether to add the entity to the internal lists of
	 *                   World or not
	 */
	void setAddToWorldLists(bool addToLists)
	{
		m_addToWorldLists = addToLists;
	}

	/**
	 * \brief Returns whether the entity should be added to the internal
	 *        lists of World or not
	 *
	 * See class description for more information
	 * \return whether to add the entity to the internal lists of World or
	 *         not
	 */
	bool addToWorldLists() const
	{
		return m_addToWorldLists;
	}

	/**
	 * \brief The function called immediately after the entity is created
	 *
	 * See class description for more information. The default
	 * implementation does nothing
	 * \param entity the entity just created
	 */
	virtual void immediatePostCreationAction(Type* entity)
	{
		Q_UNUSED(entity)
	}

	/**
	 * \brief The function called before World::createEntity() returns
	 *
	 * See class description for more information. The default
	 * implementation does nothing
	 * \param entity the entity just created
	 */
	virtual void delayedPostCreationAction(Type* entity)
	{
		Q_UNUSED(entity)
	}

	/**
	 * \brief If the object being created is a PhyObject, this sets whether
	 *        to only create the collision shape or not
	 *
	 * See class description for more information
	 * \param onlyCollision if true only the collision shape of the
	 *                      PhyObject is created
	 */
	void setOnlyCreateCollisionShape(bool onlyCollision)
	{
		m_onlyCreateCollisionShape = onlyCollision;
	}

	/**
	 * \brief If the object being created is a PhyObject, this returns
	 *        whether to only create the collision shape or not
	 *
	 * See class description for more information
	 * \return true if only the collision shape of the PhyObject is created
	 */
	bool onlyCreateCollisionShape() const
	{
		return m_onlyCreateCollisionShape;
	}

	/**
	 * \brief Sets a custom collision shape offset
	 *
	 * See class description for more information
	 * \param offset the offset of the collision shape
	 */
	void setCustomCollisionShapeOffset(const wMatrix& offset)
	{
		m_usingCustomCollisionShapeOffset = true;
		m_customCollisionShapeOffset = offset;
	}

	/**
	 * \brief Returns the custom collision shape offset
	 *
	 * See class description for more information
	 * \return the custom collision shape offset
	 */
	const wMatrix& customCollisionShapeOffset() const
	{
		return m_customCollisionShapeOffset;
	}

	/**
	 * \brief Resets the collision shape offset to the default one
	 *
	 * See class description for more information
	 */
	void resetToDefaultCollisionShapeOffset()
	{
		m_usingCustomCollisionShapeOffset = false;
	}

	/**
	 * \brief Returns true if we are using a custom collision shape offset
	 *
	 * See class description for more information
	 * \return true if we are using a custom collision shape offset
	 */
	bool usingCustomCollisionShapeOffset() const
	{
		return m_usingCustomCollisionShapeOffset;
	}

	/**
	 * \brief Sets the structure with the entity, the object to create the
	 *        renderer and the object to destroy shared data
	 *
	 * This function is called by World::createEntity() to set the
	 * structure, you should not use this function
	 * \param entityAndBuddies the structure with the entity and related
	 *                         objects
	 */
	void setWEntityAndBuddies(WEntityAndBuddies entityAndBuddies)
	{
		m_entityAndBuddies = entityAndBuddies;
	}

	/**
	 * \brief Returns the structure with the entity, the object to create
	 *        the renderer and the object to destroy shared data
	 *
	 * You can call this after a call to World::createEntity to which you
	 * passed this object. You should be very careful what you do with
	 * objects in the structure. In particular, do not delete them. See
	 * class description for more information
	 * \return the structure with the entity, the object to create the
	 *         renderer and the object to destroy shared data
	 */
	WEntityAndBuddies& entityAndBuddies()
	{
		return m_entityAndBuddies;
	}

	/**
	 * \brief Returns the structure with the entity, the object to create
	 *        the renderer and the object to destroy shared data (const
	 *        version)
	 *
	 * You can call this after a call to World::createEntity to which you
	 * passed this object. You should be very careful what you do with
	 * objects in the structure. In particular, do not delete them. See
	 * class description for more information
	 * \return the structure with the entity, the object to create the
	 *         renderer and the object to destroy shared data
	 */
	const WEntityAndBuddies& entityAndBuddies() const
	{
		return m_entityAndBuddies;
	}

private:
	/**
	 * \brief Whether to add the entity to the internal lists of World or
	 *        not
	 */
	bool m_addToWorldLists;

	/**
	 * \brief True if only the collision shape of the PhyObject is created
	 */
	bool m_onlyCreateCollisionShape;

	/**
	 * \brief The custom collision shape offset
	 */
	wMatrix m_customCollisionShapeOffset;

	/**
	 * \brief True if we are using a custom collision shape offset
	 */
	bool m_usingCustomCollisionShapeOffset;

	/**
	 * \brief The structure with the entity, the object to create the
	 *        renderer and the object to destroy shared data
	 */
	WEntityAndBuddies m_entityAndBuddies;
};

/**
 * \brief A class containig information about the world which needs to be
 *        communicated to classes rendering object
 *
 * For the moment this only contains the world size
 */
class SALSA_WSIM_TEMPLATE WorldGraphicalInfo
{
public:
	/**
	 * \brief The minimum point of the Newton world
	 */
	wVector minP;

	/**
	 * \brief The maximum point of the Newton world
	 */
	wVector maxP;
};

} // end namespace salsa

#endif
