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

#ifndef WENTITY_H
#define WENTITY_H

#include "worldsimconfig.h"
#include <QList>
#include <QString>
#include <QColor>
#include "updatetrigger.h"

namespace salsa {

class World;
class RenderWEntity;
class GLContextAndData;
class WObject;
class PhyJoint;
class SensorController;
class MotorController;
class WEntity;

/**
 * \brief A wrapper for SharedData objects that sets a flag in case of changes
 *
 * See the description of WEntity for more information. This class only holds
 * the pointer, it never frees memory. It does not contain the overloading of
 * the non-const -> operator so that users are forced to get a pointer to Shared
 * and trigger one update for each "block" of modifications
 */
template <class SharedDataType>
class SALSA_WSIM_TEMPLATE SharedDataWrapper
{
public:
	/**
	 * \brief The type of SharedData kept here
	 */
	typedef SharedDataType Shared;

public:
	/**
	 * \brief Constructor
	 *
	 * \param d the pointer to the the shared data member to maintain
	 */
	SharedDataWrapper(Shared* d)
		: m_data(d)
	{
	}

	/**
	 * \brief A copy constructor for initialization from child SharedData
	 *        types
	 *
	 * This is needed so that wrappers to pointers of base classes can be
	 * initialized from wrappers to pointers of child classes
	 * \param other the object to copy
	 */
	template <class OtherSharedDataType>
	SharedDataWrapper(const SharedDataWrapper<OtherSharedDataType>& other)
		: m_data(other.m_data)
	{
	}

	/**
	 * \brief The operator to access shared data read-only
	 *
	 * There is no non-const version of this function, you must use the
	 * getModifiableShared() function to get a pointer and modify that one
	 * (so that only one update is triggered for each "block" of
	 * modifications)
	 * \return a pointer to data (the compiler then knows what to do)
	 */
	const Shared* operator->() const
	{
		return m_data;
	}

	/**
	 *  \brief Returns a non-const pointer to shared data
	 *
	 * You have to use this function in case you want to modify data. You
	 * should never store the pointer in a class member, instead call this
	 * function at the beginning of every function that needs to modify
	 * data. This way only one update is triggered for every modification.
	 * \return a pointer to shared data
	 */
	Shared* getModifiableShared()
	{
		m_data->updateTrigger.triggerUpdate();
		return m_data;
	}

	/**
	 *  \brief Returns a const pointer to shared data
	 *
	 * \return a const pointer to shared data
	 */
	operator const Shared*() const
	{
		return m_data;
	}

	/**
	 * \brief Checks if the updateChecker needs an update
	 *
	 * This function takes an UpdateChecker and checks whether an update is
	 * needed
	 * \param checker the UpdateChecker to check if a copy is actually
	 *                needed
	 * \return true if an update is needed
	 */
	bool updateNeeded(UpdateCheckerLong& checker) const
	{
		return checker.updateNeeded(m_data->updateTrigger);
	}

private:
	/**
	 * \brief The pointer to the shared data object
	 */
	Shared* m_data;

	/**
	 * \brief All other SharedDataWrapper objects are friend (needed by the
	 *        copy constructor)
	 */
	template <class T>
	friend class SharedDataWrapper;
};

/**
 * \brief The shared data for the WEntity
 *
 * This class contains the part of the WEntity state that should be shared with
 * the Renderer to draw the entity. When you inherit from a class that is a
 * direct or indirect child of WEntity, you must also subclass the SharedData
 * corresponding to the class you are subclassing and add everything the
 * renderer should use to draw the entity. The requisites for this class is to
 * be copiable (i.e. to have an = operator, either implicit or explicit) and an
 * empty constructor. You should keep non-const pointers to this class wrapped
 * in SharedDataWrapper, so that changes are registered and unneeded copies can
 * be avoided (e.g. when transferring data to graphics)
 * \warning Never make a pointer to a WEntity stored in this class available as
 *          public member (or with public accessor), nor use it when rendering,
 *          as rendering may happend in a different thread than the one WEntity
 *          lives in
 */
class SALSA_WSIM_TEMPLATE WEntityShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	WEntityShared()
		: texture("tile2")
		, color(Qt::white)
	{
	}

	/**
	 * \brief The texture name
	 */
	QString texture;

	/**
	 * \brief The color, it contains also alpha channel
	 */
	QColor color;

private:
	/**
	 * \brief The update trigger to check if there have been changes that
	 *        need to be copied
	 */
	UpdateTriggerLongNoRecentUpdateCheck updateTrigger;

	/**
	 * \brief SharedDataWrapper is friend to access updateTrigger
	 */
	template <class SharedDataType>
	friend class SharedDataWrapper;
};

/**
 * \brief The interface for classes that are notified when a WEntity changes
 *        owner, one of its owned objects changes owner, a new object becomes
 *        owned, the entity is destroyed or the listener for the entity is
 *        changed
 *
 * You can register an instance of this class to a WEntity to get notifications
 * using the WEntity::registerOwnertshipChangesListener() function
 */
class SALSA_WSIM_TEMPLATE OwnershipChangesListener
{
public:
	/**
	 * \brief Called when the owner changes, after the change has been
	 *        performed
	 *
	 * The new owner can be accessed using owner() of entity. The default
	 * implementation does nothing
	 * \param entity the entity whose owner changed
	 * \param oldOwner the previous owner
	 */
	virtual void ownerChanged(WEntity* /*entity*/, WEntity* /*oldOwner*/)
	{
	}

	/**
	 * \brief Called when an owned object changes owner or is destroyed
	 *
	 * This is called before the owner is actually changed or the object
	 * destroyed. The default implementation does nothing
	 * \param entity the entity that owned the entity that changed owner or
	 *               is being destroyed
	 * \param owned the entity that changed owner
	 * \param newOwner the new owner
	 * \param destroyed true if the object is being destroyed
	 */
	virtual void ownedChangedOwner(WEntity* /*entity*/, WEntity* /*owned*/, WEntity* /*newOwner*/, bool /*destroyed*/)
	{
	}

	/**
	 * \brief Called when object become owned by entity
	 *
	 * This is called after the owner is changed. The default implementation
	 * does nothing
	 * \param entity the entity that has a new owned object
	 * \param newOwned the new owned object
	 */
	virtual void ownedAdded(WEntity* /*entity*/, WEntity* /*newOwned*/)
	{
	}

	/**
	 * \brief Called when the entity is destroyed
	 *
	 * This is called in the destructor of WEntity (i.e. you can only use
	 * functions in WEntity). The default implementation does nothing
	 * \param entity the entity being destroyed
	 */
	virtual void entityDestroyed(WEntity* /*entity*/)
	{
	}

	/**
	 * \brief Called when another object is registered as the listener for
	 *        changes in ownership
	 *
	 * This is called after the listerner is changed. The default
	 * implementation does nothing
	 * \param entity the entity whose listener is changed
	 * \param newListener the new listener
	 */
	virtual void listenerChanged(WEntity* /*entity*/, OwnershipChangesListener* /*newListener*/)
	{
	}
};

/**
 * \brief The base for all the stuffs that live in World
 *
 * This is the base class for all the stuffs that live in World. This is not
 * instantiated, only subclasses are. The constructor and destructor are private
 * and World is set as friend because these objects are created and destroyed by
 * functions in World (World::createEntity() and World::deleteEntity()), not
 * directly calling new. Any attempt to create an entity directly (i.e.
 * bypassing world functions) will cause an exception of type
 * EntityCreatedOutsideWorldException to be raised. Because of this it is
 * advisable to make constructor and destructors protected in subclasses, too.
 * Entities can have an owner. When an object has an owner, the owner is
 * responsible of freeing the owned classes when it is deleted. This can be done
 * automatically by this class destructor or can be left to the owner
 * implementation (for example when there must be a precise order in destroying
 * owned objects). Owners are other instances of this class. Setting the owner
 * to nullptr means that the object has no owner.
 * Each subclass must define two inner types: Shared and Renderer. Shared is the
 * class with the status of the object that should be shared with the Renderer,
 * so it should contain all the properties that are needed to draw the entity,
 * for example the tranformation matrix, the color, the dimensions... Renderer
 * is the class which is responsible of performing the actual rendering of the
 * entity. When subclassing from a WEntity you must subclass also the Shared
 * class, because the pointer to that structure is passed to the parent class
 * (you won't be able to compile your code if you fail to do so). The pointer to
 * the shared data is wrapped in SharedDataWrapper, so that changes are
 * registered and unneeded copies can be avoided (e.g. when tranferring data to
 * graphics). Shared data is accessible via a call to getShared() as a
 * SharedDataWrapper\<WEntityShared\> object, from which you can then obtain a
 * pointer to the actual type using suitable casts. The const version of
 * getShared() directly returns a const pointer. It is strongly advisable,
 * however, to keep a pointer to the shared data structure wrapped in
 * SharedDataWrapper as passed to the constructor, for easier access. To access
 * actual data from the SharedDataWrapper, you can do like this:
 *
 * \code
 * 	// Suppose m_shared is a SharedDataWrapper. Then, for const access you
 * 	// can do like this:
 * 	real val = m_shared->val;
 * 	const QVector<int>& vec = m_shared->vec;
 * 	...
 *
 * 	// If instead you want write access, do this:
 * 	Shared* d = m_shared.getModifiableShared();
 * 	d->val = 10;
 * 	d->vec[3] = 17;
 *
 * 	// You can also use getModifiableShared() in each assignment, but this
 * 	// is more verbose and triggers more than one update (which means that
 * 	// you have the overhead of an int that is incremented in every call)
 * 	m_shared.getModifiableShared()->val = 10;
 * 	m_shared.getModifiableShared()->vec[3] = 17;
 * \endcode
 *
 * It is very important not to keep a bare pointer to the shared data as a class
 * member: if you do this, no changes can be notified (e.g. the graphical
 * representation may not be updated). Once passed to the constructor, the
 * Shared instance is guaranteed to remain valid for the whole life of the
 * WEntity.See the documentation of WEntityShared and RenderWEntity for more
 * information about the requirements of the Shared and Renderer types.
 * \note You must not inherit directly from this class, use one of the four
 *       direct subclasses: WObject, PhyJoint, SensorController and
 *       MotorController
 */
class SALSA_WSIM_API WEntity
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef WEntityShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderWEntity Renderer;

public:
	/**
	 * \brief The structure with information about an owned object
	 *
	 * This keeps the owned object and whether the owner should destroy it
	 * automatically or not
	 */
	struct SALSA_WSIM_TEMPLATE Owned {
		/**
		 * \brief Constructor
		 *
		 * \param o the owned object
		 * \param d if true the object is automatically destroyed by
		 *          the owner destructor
		 */
		Owned(WEntity *o, bool d = true) :
			object(o),
			destroy(d)
		{
		}

		/**
		 * \brief Comparison operator overload
		 *
		 * \return true if other contains the same object as this one
		 *         (the destroy property is not taken into account)
		 */
		bool operator==(const Owned& other)
		{
			return (object == other.object);
		}

		/**
		 * \brief The owned object
		 */
		WEntity *object;

		/**
		 * \brief If true the object is automatically destroyed by the
		 *        owner destructor
		 */
		bool destroy;
	};

	/**
	 * \brief The type for the list of owned objects
	 */
	typedef QList<Owned> OwnedList;

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where entity lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               entity has been deleted)
	 * \param name the name of the object
	 */
	WEntity(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed");

	/**
	 * \brief Destructor
	 *
	 * This deletes all owned object marked to be automatically destroyed
	 */
	virtual ~WEntity();

public:
	/**
	 * \brief Sets the name of the WEntity
	 *
	 * \param name the name of the WEntity
	 */
	void setName(QString name);

	/**
	 * \brief Returns the name of this object
	 *
	 * \return the name of the object
	 */
	const QString& name() const
	{
		return m_name;
	}

	/**
	 * \brief Sets the owner of this object
	 *
	 * This also fixes our texture and color and that of our owned depending
	 * on the value of useColorTextureOfOwner()
	 * \param owner the owner of this object
	 * \param destroy if true this object is automatically destroyed by the
	 *                owner destructor
	 */
	void setOwner(WEntity* owner, bool destroy = true);

	/**
	 * \brief Returns the owner of this object
	 *
	 * \return the owner of this object
	 */
	WEntity* owner()
	{
		return m_owner;
	}

	/**
	 * \brief Returns the owner of this object (const version)
	 *
	 * \return the owner of this object
	 */
	const WEntity* owner() const
	{
		return m_owner;
	}

	/**
	 * \brief Returns the list of objects owned by this one
	 *
	 * \return the list of objects owned by this one
	 */
	const OwnedList& owned() const
	{
		return m_owned;
	}

	/**
	 * \brief Sets the texture to use for this WEntity when rendered
	 *
	 * This actually changes the texture used for rendering only if we don't
	 * use the texture of the owner, otherwise the texture is stored to be
	 * used when m_useColorTextureOfOwner is false
	 * \param textureName the name of the texture to use
	 */
	void setTexture(QString textureName);

	/**
	 * \brief Returns the texture name
	 *
	 * \param actual if true returns the actual texture in used (i.e. if we
	 *               use the texture of the owner, returns that one, not the
	 *               one set by setTexture)
	 * \return the name of the texture to use
	 */
	const QString& texture(bool actual = false) const;

	/**
	 * \brief Sets the color of the entity
	 *
	 * When the texture is set, the color is behind the texture. To display
	 * the entity with only color set the texture to "none" (i.e. call
	 * setTexture("none")). This actually changes the color used for
	 * rendering only if we don't use the color of the owner, otherwise the
	 * color is stored to be used when m_useColorTextureOfOwner is false
	 * \param c the color of the entity
	 */
	void setColor(QColor c);

	/**
	 * \brief Sets the value of alpha channel (the transparency)
	 *
	 * This actually changes the color used for rendering only if we don't
	 * use the color of the owner, otherwise the color is stored to be
	 * used when m_useColorTextureOfOwner is false
	 * \param alpha the value of the alpha channel (between 0 and 255)
	 */
	void setAlpha(int alpha);

	/**
	 * \brief Returns the color of this entity
	 *
	 * \param actual if true returns the actual color in used (i.e. if we
	 *               use the color of the owner, returns that one, not the
	 *               one set by setColor)
	 * \return the color of the entity
	 */
	const QColor& color(bool actual = false) const;

	/**
	 * \brief Returns whether to use the color and texture of our owner or
	 *        our own
	 *
	 * \return true if we use the color and texture of our owner, false
	 *         otherwise
	 */
	bool useColorTextureOfOwner() const;

	/**
	 * \brief Sets whether we are rendered with the color and texture of the
	 *        owner or our own
	 *
	 * \param b if true this entity will use the color and texture of the
	 *          owner
	 */
	void setUseColorTextureOfOwner(bool b);

	/**
	 * \brief Returns a const pointer to the object with shared data
	 *
	 * \return a const pointer to the object with shared data
	 */
	const Shared* getShared() const
	{
		return m_shared;
	}

	/**
	 * \brief Checks if the updateChecker needs an update agains the update
	 *        trigger in the shared data object
	 *
	 * This function takes an UpdateChecker and checks whether an update is
	 * needed using the SharedDataWrapper.
	 * \param checker the UpdateChecker to check if a copy is actually
	 *                needed
	 * \return true if an update is needed
	 */
	bool updateNeeded(UpdateCheckerLong& checker) const
	{
		return m_shared.updateNeeded(checker);
	}

	/**
	 * \brief The method called at each step of the world just before the
	 *        physic update
	 *
	 * The default implementation does nothing
	 */
	virtual void preUpdate();

	/**
	 * \brief The method called at each step of the world just after the
	 *        physic update
	 *
	 * The default implementation does nothing
	 */
	virtual void postUpdate();

	/**
	 * \brief Registers an object that is notified when this object changes
	 *        owner, an owned object changes ower or is destroyed or when
	 *        a new object becomes owned by this
	 *
	 * There can be only one listener at a time. Setting the listener to
	 * nullptr removes the current listener. This function returns the previous
	 * listerner or nullptr if no listener was registered before
	 * \param listener the new listener
	 * \return the old listener
	 */
	OwnershipChangesListener* registerOwnertshipChangesListener(OwnershipChangesListener* listener);

protected:
	/**
	 * \brief Returns a const pointer to the world
	 *
	 * \return a const pointer to the world
	 */
	const World* world() const
	{
		return m_world;
	}

	/**
	 * \brief Returns a pointer to the world
	 *
	 * \return a pointer to the world
	 */
	World* world()
	{
		return m_world;
	}

	/**
	 * \brief Returns the object with shared data
	 *
	 * This is protected because only this object should be able to
	 * manipulate the shared data
	 * \return the object with shared data
	 */
	SharedDataWrapper<Shared> getShared()
	{
		return m_shared;
	}

private:
	/**
	 * \brief Updates the color and texture of owned WObjects
	 */
	void updateColorTextureOfOwned();

	/**
	 * \brief Updates the color and texture taking them from our owner
	 *
	 * This also calls updateColorTextureOfOwned() to update owned objects
	 */
	void updateColorTextureFromOwner();

	/**
	 * \brief Adds an object to the list of owned objects
	 *
	 * \param obj the object to add to the list
	 * \param destroy if true obj is automatically destroyed by this object
	 *                destructor
	 */
	void addToOwned(WEntity* obj, bool destroy);

	/**
	 * \brief Removes the given object from the list of owned objects
	 *
	 * \param obj the object to remove from the list
	 * \param newOwner the new owner of the object
	 * \param destroyed true if the owned object is being destroyed
	 */
	void removeFromOwned(WEntity* obj, WEntity* newOwner, bool destroyed);

	/**
	 * \brief The world in which we live
	 */
	World* const m_world;

	/**
	 * \brief The wrapper of the pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief The name of the WEntity
	 */
	QString m_name;

	/**
	 * \brief The onwer of this object
	 */
	WEntity* m_owner;

	/**
	 * \brief The list of objects owned by this one
	 */
	OwnedList m_owned;

	/**
	 * \brief If true we inherit the color and texture of the owner
	 */
	bool m_useColorTextureOfOwner;

	/**
	 * \brief The texture name
	 *
	 * This is always the value set by the setTexture() function, whether
	 * this or the owner's is used depends on m_useColorTextureOfOwner
	 */
	QString m_myTexture;

	/**
	 * \brief The color, it contains also alpha channel
	 *
	 * This is always the value set by the setColor() function, whether this
	 * or the owner's is used depends on m_useColorTextureOfOwner
	 */
	QColor m_myColor;

	/**
	 * \brief The object notified of changes in ownership of this or owned
	 *        entities
	 */
	OwnershipChangesListener* m_ownershipChangesListener;

	/**
	 * \brief World is friend to be able to call constructor, destructor and
	 *        set m_hitman
	 */
	friend class World;

	/**
	 * \brief WObject is friend to be able to inherit from WEntity
	 */
	friend class WObject;

	/**
	 * \brief PhyJoint is friend to be able to inherit from WEntity
	 */
	friend class PhyJoint;

	/**
	 * \brief SensorController is friend to be able to inherit from WEntity
	 */
	friend class SensorController;

	/**
	 * \brief MotorController is friend to be able to inherit from WEntity
	 */
	friend class MotorController;

private:
	/**
	 * \brief Copy constructor
	 *
	 * Here to disallow usage
	 */
	WEntity(const WEntity& other);

	/**
	 * \brief Copy operator
	 *
	 * Here to disallow usage
	 */
	WEntity& operator=(const WEntity& other);
};

/**
 * \brief The class rendering the WEntity
 *
 * This class has the responsability to render the WEntity. Subclasses must
 * implement the render() method (non virtual). The implementation of that
 * function in this class is empty. When you inherit from a class that is a
 * direct or indirect child of WEntity, you must also subclass the Renderer
 * corresponding to the class you are subclassing. A note on the constructor: it
 * has one parameter, namely the entity it will render. That pointer, however,
 * should only be used in the constructor, you MUST NOT STORE IT! Renderers, in
 * fact, are created in the simulation thread but could be used in a different
 * thread. Because of this, you can use the entity for initializations, but you
 * must not use it when rendering (a pointer to the data to use is passed to the
 * render() function).
 */
class SALSA_WSIM_API RenderWEntity
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity the entity we render. YOU MUST NOT KEEP A REFERENCE TO
	 *               IT!!! This is only passed in case you need to do custom
	 *               initializations, the render() function will be passed
	 *               updated data at each call externally
	 */
	RenderWEntity(const WEntity* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderWEntity();

	/**
	 * \brief The function performing the rendering
	 *
	 * This implementation does nothing
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const WEntityShared* sharedData, GLContextAndData* contextAndData);

protected:
	/**
	 * \brief Setups the color and texture into the OpenGL context
	 *
	 * You can call this function in your rendering implementations to set
	 * up the color and texture (this calls applyTexture())
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void setupColorTexture(const WEntityShared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief Only setups the texture
	 *
	 * You can call this function in your rendering implementations to set
	 * up the texture
	 * \param texture the name of the texture to apply
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void applyTexture(const QString& texture, GLContextAndData* contextAndData);
};

}

#endif
