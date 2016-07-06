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

#ifndef PHYCOMPOUNDOBJECT_H
#define PHYCOMPOUNDOBJECT_H

#include "phyobject.h"
#include "wentity.h"
#include "world.h"
#include "renderingproxy.h"
#include <QList>
#include <QAtomicInt>
#include <QExplicitlySharedDataPointer>

namespace farsa {

class AbstractPhyObjectComponent;
class PhyCompoundObject;
class RenderPhyCompoundObject;

/**
 * \brief A structure with an object an the corresponding
 *        AbstractPhyObjectComponent
 */
struct PhyCompoundComponentAndAbstractComponent
{
	/**
	 * \brief The object
	 */
	PhyObject* object;

	/**
	 * \brief The abstract PhyObjectComponent associated to the
	 *        object
	 */
	AbstractPhyObjectComponent* abstractComponent;

	/**
	 * \brief The shared data holder for the abstract component
	 */
	AbstractSharedDataHolder* sharedDataHolder;

	/**
	 * \brief The class to generate renderers and instance of
	 *        AbstractRenderingProxy for the entity
	 */
	AbstractRenderWEntityCreator* rendererCreator;
};

/**
 * \brief The class with the list of components of a compound object
 *
 * This class is used to generate the components of a compound object. Once
 * created you can add objects using the createComponent() method, which returns
 * a pointer to the newly created object. The object has only the collision
 * shape and cannot be moved once created (only its graphical properties can be
 * changed). Moreover you should not attempt to delete it, change its owner (the
 * owner will be set to the PhyCompoundObject when the latter is created) or
 * change the listener for changes in ownership. An attempt to do one of these
 * things will result in program termination. If you create objects using
 * createComponent() and then they are not used in a compound object, the
 * objects are deleted when the PhyCompoundComponentsList is destroyed.
 * Moreover, you can only use a list for one compound object
 * \note A PhyCompoundObject cannot be a component of another PhyCompoundObject
 */
class FARSA_WSIM_API PhyCompoundComponentsList : private OwnershipChangesListener
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param world the world in which we live
	 */
	PhyCompoundComponentsList(World* world);

	/**
	 * \brief Destructor
	 *
	 * This deletes all created objects if they have not been used to create
	 * a compound object.
	 */
	~PhyCompoundComponentsList();

	/**
	 * \brief The function to create a component object
	 *
	 * Use this function to create a component of a PhyCompoundObject. This
	 * internally uses world->createEntity(). Object created by this
	 * function only have the collision shape and no physical body attached.
	 * This function can only create sublcasses of PhyObject, not generic
	 * entities.
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \return a pointer to the new component
	 */
	template <class T>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \return a pointer to the new component
	 */
	template <class T, class P0>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \param p2 the fifth parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1, class P2>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \param p2 the fifth parameter of the component constructor
	 * \param p3 the sixth parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1, class P2, class P3>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \param p2 the fifth parameter of the component constructor
	 * \param p3 the sixth parameter of the component constructor
	 * \param p4 the seventh parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \param p2 the fifth parameter of the component constructor
	 * \param p3 the sixth parameter of the component constructor
	 * \param p4 the seventh parameter of the component constructor
	 * \param p5 the eighth parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4, class P5>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \param p2 the fifth parameter of the component constructor
	 * \param p3 the sixth parameter of the component constructor
	 * \param p4 the seventh parameter of the component constructor
	 * \param p5 the eighth parameter of the component constructor
	 * \param p6 the ninth parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

	/**
	 * \brief The function to create a component object
	 *
	 * See the documentation for the version with no parameters for more
	 * information
	 * \param type an instance of the class TypeToCreate with the PhyObject
	 *             subclass to create as the template parameter.
	 * \param offset the displacement matrix of the object in the frame of
	 *               reference of the compound object
	 * \param p0 the third parameter of the component constructor (the one
	 *           after World and SharedData)
	 * \param p1 the fourth parameter of the component constructor
	 * \param p2 the fifth parameter of the component constructor
	 * \param p3 the sixth parameter of the component constructor
	 * \param p4 the seventh parameter of the component constructor
	 * \param p5 the eighth parameter of the component constructor
	 * \param p6 the ninth parameter of the component constructor
	 * \param p7 the tenth parameter of the component constructor
	 * \return a pointer to the new component
	 */
	template <class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	T* createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

private:
	/**
	 * \brief Called when the owner changes, after the change has been
	 *        performed
	 *
	 * The new owner can be accessed using owner() of entity
	 * \param entity the entity whose owner changed
	 * \param oldOwner the previous owner
	 */
	virtual void ownerChanged(WEntity* entity, WEntity* oldOwner);

	/**
	 * \brief Called when the entity is destroyed
	 *
	 * This is called in the destructor of WEntity (i.e. you can only use
	 * functions in WEntity)
	 * \param entity the entity being destroyed
	 */
	virtual void entityDestroyed(WEntity* entity);

	/**
	 * \brief Called when another object is registered as the listener for
	 *        changes in ownership
	 *
	 * This is called after the listerner is changed
	 * \param entity the entity whose listener is changed
	 * \param newListener the new listener
	 */
	virtual void listenerChanged(WEntity* entity, OwnershipChangesListener* newListener);

	/**
	 * \brief The world in which we live
	 */
	World* const m_world;

	/**
	 * \brief The structure with data shared among different instances of
	 *        this class
	 *
	 * This is needed because if we pass instances of this to
	 * world->createEntity(), a copy is performed and this is not what we
	 * want (the copy is performed because the deducted template type is
	 * PhyCompoundComponentsList and not PhyCompoundComponentsList&). So we
	 * use a QExplicitlySharedDataPointer
	 */
	struct SharedStuffs : public QSharedData
	{
		/**
		 * \brief Constructor
		 */
		SharedStuffs()
			: QSharedData()
			, components()
			, creatingPhyCompound(false)
		{
		}

		/**
		 * \brief The list of components
		 */
		QList<PhyCompoundComponentAndAbstractComponent> components;

		/**
		 * \brief True when the PhyCompoundObject is being created.
		 *
		 * This is set to true by the PhyCompoundObject constructor to prevent
		 * checks on ownership changes
		 */
		bool creatingPhyCompound;
	};

	/**
	 * \brief The object sharing data across different instances of this
	 *        class
	 */
	QExplicitlySharedDataPointer<SharedStuffs> m_shared;

	/**
	 * \brief PhyCompoundObject is friend to access the list of components
	 */
	friend class PhyCompoundObject;
};

/**
 * \brief The abstract base class to call the functions to compute the AABB and
 *        OBB of a component and to copy its shared data
 */
class FARSA_WSIM_TEMPLATE AbstractPhyObjectComponent
{
public:
	/**
	 * \brief Constructor
	 */
	AbstractPhyObjectComponent()
		: referenceCounter(0)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~AbstractPhyObjectComponent()
	{
	}

	/**
	 * \brief Returns the min and max points of the Axis-Aligned Bounding
	 *        Box (AABB, aligned to axes of the given frame of reference)
	 *
	 * This calls the calculateAABB function of the component
	 * \param sharedData the object with data from WObject
	 * \param minPoint the minimum point of the AABB
	 * \param maxPoint the maximum point of the AABB
	 * \param tm the frame of reference in which the AABB is to be computed
	 */
	virtual void calculateAABB(const WEntityShared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const = 0;

	/**
	 * \brief Returns the dimension of the Minimum Bounding Box (MBB) of the
	 *        object
	 *
	 * This calls the calculateMBBSize function of the component
	 * \param sharedData the object with data from WObject
	 * \return the size of the MBB
	 */
	virtual wVector calculateMBBSize(const WEntityShared* sharedData) const = 0;

	/**
	 * \brief Copies data of the component
	 *
	 * This internally must cast to the actual type
	 * \param from data to copy
	 * \param to the destination of the copy
	 */
	virtual void copyData(const WEntityShared* from, WEntityShared* to) const = 0;

	/**
	 * \brief Creates and instance of the object with shared data
	 *
	 * \return an instance of the object with shared data
	 */
	virtual AbstractSharedDataHolder* generateSharedData() const = 0;

	/**
	 * \brief Performs the rendering
	 *
	 * This function performs the casts to the correct types when rendering.
	 * You must be sure to pass the correct objects
	 * \param renderer the renderer
	 * \param sharedData shared data to use when rendering
	 * \param contextAndData the OpenGL context and data
	 */
	virtual void render(RenderWEntity* renderer, const WEntityShared* sharedData, GLContextAndData* contextAndData) const = 0;

	/**
	 * \brief An atomic reference counter
	 *
	 * This can be used to share objects of this type and delete them when
	 * no one uses them
	 */
	QAtomicInt referenceCounter;
};

/**
 * \brief The concrete class to call the functions to compute the AABB and OBB
 *        of a component and to copy its shared data
 *
 * All functions are reentrant and this class does not have any member variable
 * (this means you can safely share it across threads as long as you call
 * functions with different parameters). Moreover you can use the
 * referenceCounter member of AbstractPhyObjectComponent to implement reference
 * counting and share instances of this class
 */
template <class ComponentType_t>
class FARSA_WSIM_TEMPLATE PhyObjectComponent : public AbstractPhyObjectComponent
{
public:
	/**
	 * \brief The actual type of the component
	 */
	typedef ComponentType_t ComponentType;

public:
	/**
	 * \brief Destructor
	 */
	virtual ~PhyObjectComponent()
	{
	}

	/**
	 * \brief The function to get the AABB of the component
	 *
	 * This calls the calculateAABB function of the component
	 * \param sharedData the object with data from WObject
	 * \param minPoint the minimum point of the AABB
	 * \param maxPoint the maximum point of the AABB
	 */
	virtual void calculateAABB(const WEntityShared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const
	{
		ComponentType::calculateAABB(static_cast<const typename ComponentType::Shared*>(sharedData), minPoint, maxPoint, tm);
	}

	/**
	 * \brief The function to get the OBB of the component
	 *
	 * This calls the calculateMBBSize function of the component
	 * \param sharedData the object with data from WObject
	 * \param minPoint the minimum point of the OBB
	 * \param maxPoint the maximum point of the OBB
	 */
	virtual wVector calculateMBBSize(const WEntityShared* sharedData) const
	{
		return ComponentType::calculateMBBSize(static_cast<const typename ComponentType::Shared*>(sharedData));
	}

	/**
	 * \brief Copies data of the component
	 *
	 * This internally must cast to the actual type
	 * \param from data to copy
	 * \param to the destination of the copy
	 */
	virtual void copyData(const WEntityShared* from, WEntityShared* to) const
	{
		*(static_cast<typename ComponentType::Shared*>(to)) = *(static_cast<const typename ComponentType::Shared*>(from));
	}

	/**
	 * \brief Creates and instance of the object with shared data and wraps
	 *        it in an AbstractSharedDataHolder object
	 *
	 * \return an instance of AbstractSharedDataHolder with shared data
	 */
	virtual AbstractSharedDataHolder* generateSharedData() const
	{
		return new SharedDataHolder<typename ComponentType::Shared>(new typename ComponentType::Shared);
	}

	/**
	 * \brief Performs the rendering
	 *
	 * This function performs the casts to the correct types when rendering.
	 * You must be sure to pass the correct objects
	 * \param renderer the renderer
	 * \param sharedData shared data to use when rendering
	 * \param contextAndData the OpenGL context and data
	 */
	virtual void render(RenderWEntity* renderer, const WEntityShared* sharedData, GLContextAndData* contextAndData) const
	{
		__RenderingProxy_internal::render<ComponentType>(static_cast<typename ComponentType::Renderer*>(renderer), static_cast<const typename ComponentType::Shared*>(sharedData), contextAndData);
	}
};

/**
 * \brief The concrete class to call the functions to compute the AABB and OBB
 *        of a component and to copy its shared data
 *
 * This is the template specialization for PhyCompoundObject. It is empty so
 * that an error is generated at compile-time if an attempt is made to use a
 * PhyCompoundObject as a component of another object
 */
template <>
class FARSA_WSIM_TEMPLATE PhyObjectComponent<PhyCompoundObject>
{
private:
	/**
	 * \brief Constructor
	 *
	 * Private to prevent the creation of this kind of objects
	 */
	PhyObjectComponent()
	{
	}

	/**
	 * \brief Destructor
	 *
	 * Private to prevent the creation of this kind of objects
	 */
	virtual ~PhyObjectComponent()
	{
	}
};

/**
 * \brief The shared data for the PhyCompoundObject
 *
 * You can only copy instances of this which refer to the same
 * PhyCompoundObject. If you attempt to copy instances from different
 * PhyCompoundObjects the program is terminated
 */
class FARSA_WSIM_API PhyCompoundObjectShared : public PhyObjectShared
{
public:
	/**
	 * \brief A structure with an abstract component and the associated
	 *        shared data
	 */
	struct AbstractComponentAndData
	{
		/**
		 * \brief The abstract component
		 */
		AbstractPhyObjectComponent* component;

		/**
		 * \brief The shared data for the abstract component
		 */
		AbstractSharedDataHolder* sharedData;
	};

public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyCompoundObjectShared();

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	PhyCompoundObjectShared(const PhyCompoundObjectShared& other);

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 */
	PhyCompoundObjectShared& operator=(const PhyCompoundObjectShared& other);

	/**
	 * \brief Destructor
	 *
	 * This is needed to correctly delete data for all components
	 */
	~PhyCompoundObjectShared();

	/**
	 * \brief Returns the list of components
	 *
	 * \return the list of components
	 */
	const QList<AbstractComponentAndData>& components() const
	{
		return m_components;
	}

private:
	/**
	 * \brief The list of components (abstract component instances) and the
	 *        associated shared data
	 */
	QList<AbstractComponentAndData> m_components;

	/**
	 * \brief If true the shared data holders are deleted in the destructor,
	 *        otherwise they aren't
	 *
	 * This is needed because we must not delete data for components that
	 * was created by world, but we must delete data created in copies of
	 * this (e.g. when a new instance of this is created to share data with
	 * the gui thread)
	 */
	bool m_destroySharedDataHolders;

	/**
	 * \brief The PhyCompoundObject to which we are associated
	 *
	 * This is only needed to check that a copy is not performed between
	 * instances which are relative to difference PhyCompoundObjects
	 */
	PhyCompoundObject* m_phyCompoundObject;

	/**
	 * \brief PhyCompoundObject is friend to access and modify m_components
	 *        and m_destroySharedDataHolders
	 */
	friend class PhyCompoundObject;
};

/**
 * \brief Represents a collection of object merged togheter
 *
 * You should not attempt to delete a component, change its owner (the owner
 * will be set to this object) or change the listener for changes in ownership.
 * An attempt to do one of these things will result in program termination
 * \note In Newton compound collision shapes cannot have a collision shape
 *       offset matrix
 */
class FARSA_WSIM_API PhyCompoundObject : public PhyObject, private OwnershipChangesListener
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyCompoundObjectShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyCompoundObject Renderer;

	/**
	 * \brief Returns the min and max points of the Axis-Aligned Bounding
	 *        Box (AABB, aligned to axes of the given frame of reference)
	 *
	 * The box is aligned with the axes of the frame of reference described
	 * by the tm matrix. If you pass the object transformation matrix as tm
	 * you get the AABB aligned to world axes
	 * \param sharedData the object with data from WObject
	 * \param minPoint the minimum point of the AABB
	 * \param maxPoint the maximum point of the AABB
	 * \param tm the frame of reference in which the AABB is to be computed
	 */
	static void calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm);

	/**
	 * \brief Returns the dimension of the Minimum Bounding Box (MBB) of the
	 *        object
	 *
	 * This should returns the dimensions of the arbitrarily oriented
	 * minimum bounding box. In other words this should return the size of
	 * the smallest possible bounding box containing the object. Here,
	 * however the returned box is not guaranteed to be the minimum one (it
	 * is computed as AABB in the local frame of reference, which is not
	 * always correct).
	 * \param sharedData the object with data from WObject
	 * \return the size of the MBB
	 */
	static wVector calculateMBBSize(const Shared* sharedData);

protected:
	/**
	 * \brief Creates a compound object merging all components
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param objs the list of components
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PhyCompoundObject(World* world, SharedDataWrapper<Shared> shared, PhyCompoundComponentsList& objs, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyCompoundObject();

public:
	/**
	 * \brief Returns the number of components
	 *
	 * \return the number of components
	 */
	int numComponents() const
	{
		return m_components.size();
	}

	/**
	 * \brief Returns a pointer to the i-th component
	 *
	 * \param i the index of the component
	 * \return the i-th component
	 */
	PhyObject* component(int i)
	{
		return m_components[i].object;
	}

	// Adding code for virtual bounding box functions
	FARSA_IMPLEMENT_VIRTUAL_BB

protected:
	/**
	 * \brief Creates the objects needed by the underlying physics engine
	 *
	 * This is called by world after the object has been constructed (so
	 * that the correct virtual function is called)
	 * \param onlyCollisionShape if true only the collision shape is created
	 * \param collisionShapeOffset if not NULL, the offset of the collision
	 *                             shape is set to this matrix
	 */
	virtual void createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset);

private:
	/**
	 * \brief Called when the owner changes, after the change has been
	 *        performed
	 *
	 * The new owner can be accessed using owner() of entity
	 * \param entity the entity whose owner changed
	 * \param oldOwner the previous owner
	 */
	virtual void ownerChanged(WEntity* entity, WEntity* oldOwner);

	/**
	 * \brief Called when the entity is destroyed
	 *
	 * This is called in the destructor of WEntity (i.e. you can only use
	 * functions in WEntity)
	 * \param entity the entity being destroyed
	 */
	virtual void entityDestroyed(WEntity* entity);

	/**
	 * \brief Called when another object is registered as the listener for
	 *        changes in ownership
	 *
	 * This is called after the listerner is changed
	 * \param entity the entity whose listener is changed
	 * \param newListener the new listener
	 */
	virtual void listenerChanged(WEntity* entity, OwnershipChangesListener* newListener);

	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief The list of components
	 */
	QList<PhyCompoundComponentAndAbstractComponent> m_components;

	/**
	 * \brief Whether to skip ownership checks for components or not
	 *
	 * If true the ownership checks are skipped (needed during destruction
	 * to avoid throwing exceptions)
	 */
	bool m_skipComponentOwnershipChecks;

	/**
	 * \brief World is friend to access m_priv and call both
	 *        createPrivateObject() and postCreatePrivateObject() and to
	 *        create instances
	 */
	friend class World;

	/**
	 * \brief RenderPhyCompoundObject is friend to access the list of
	 *        components
	 */
	friend class RenderPhyCompoundObject;
};

/**
 * \brief The class rendering the PhyCompoundObject
 */
class FARSA_WSIM_API RenderPhyCompoundObject : public RenderPhyObject
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
	RenderPhyCompoundObject(const PhyCompoundObject* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyCompoundObject();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const PhyCompoundObjectShared* sharedData, GLContextAndData* contextAndData);

	// Adding code for virtual bounding box functions
	FARSA_IMPLEMENT_VIRTUAL_RENDERER_BB(PhyCompoundObject)

private:
	/**
	 * \brief A structure with renderers and the abstract components
	 */
	struct ComponentAndRenderer
	{
		/**
		 * \brief The abstract component
		 */
		AbstractPhyObjectComponent* component;

		/**
		 * \brief The renderer for component
		 */
		RenderWEntity* renderer;
	};

	/**
	 * \brief The list of renderers for components
	 */
	QList<ComponentAndRenderer> m_componentsRenderers;
};

} // end namespace farsa

// Implementation of template functions
namespace farsa {

template <class T>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset)
{
#define __FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT WEntityCreationCustomizer<T> customizer;\
                                                              customizer.setAddToWorldLists(false);\
                                                              customizer.setOnlyCreateCollisionShape(true);\
                                                              customizer.setCustomCollisionShapeOffset(offset);

#define __FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT obj->registerOwnertshipChangesListener(this);\
                                                               PhyCompoundComponentAndAbstractComponent c;\
                                                               c.object = obj;\
                                                               c.abstractComponent = new PhyObjectComponent<T>();\
                                                               c.sharedDataHolder = customizer.entityAndBuddies().sharedDataHolder;\
                                                               c.rendererCreator = customizer.entityAndBuddies().rendererCreator;\
                                                               m_shared->components.append(c);\
                                                               return obj;

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT

}

template <class T, class P0>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1, class P2>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1, p2);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1, class P2, class P3>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1, p2, p3);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1, class P2, class P3, class P4>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1, p2, p3, p4);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1, class P2, class P3, class P4, class P5>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1, p2, p3, p4, p5);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1, p2, p3, p4, p5, p6);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

template <class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
T* PhyCompoundComponentsList::createComponent(TypeToCreate<T> type, const wMatrix& offset, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
	__FARSA_PHYCOMPOUNDCOMPONENTLIST_PRE_CREATE_COMPONENT

	T* obj = m_world->createEntity(customizer, type, p0, p1, p2, p3, p4, p5, p6, p7);

	__FARSA_PHYCOMPOUNDCOMPONENTLIST_POST_CREATE_COMPONENT
}

}

#endif
