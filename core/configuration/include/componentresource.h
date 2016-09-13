/***************************************************************************
 *  SALSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Gianluca Massera <emmegian@yahoo.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#ifndef COMPONENTRESOURCE_H
#define COMPONENTRESOURCE_H

#include "configurationconfig.h"
#include <QSet>
#include <QObject>
#include <memory>

namespace salsa {

class Component;
class ResourceHandler;
class ConfigurationNode;
class ConfigurationManager;

/**
 * \brief The change that happened to a resource
 */
enum ResourceChangeType {
	ResourceCreated, /**< The resource has been created */
	ResourceModified, /**< The resource has been modified */
	ResourceDeclaredAsNull, /**< The resource has been declared as nullptr */
	ResourceDeleted /**< The resource has been deleted (i.e. the owner has
	                     been deleted) */
};

/**
 * \brief Resource is a class representing a generic resource owned by a
 *        Component
 *
 * If you are creating a class and you want to allow it to be registered as a
 * Resource into a Component then you need to subclass from Resource
 * only if your class is not a subclass (indirect or direct) of one of the
 * following classes:
 * 	- Component
 * 	- QObject
 */
class SALSA_CONF_TEMPLATE Resource
{
public:
	/*! \brief Destructor */
	virtual ~Resource() { }
};

/**
 * \brief The base for classes accessing resources
 *
 * This is the parent class of Component, providing all functions to access
 * resources (but no means to create them). An instance of this class is passed
 * to ComponentCreator to be able to access resources before a component is
 * created. Refer to the documentation of the Component class for more
 * informtion on resources.
 */
class SALSA_CONF_API ResourceAccessor
{
public:
	/**
	 * \brief A structure keeping a resource and its owner
	 *
	 * The resource has type T
	 */
	template <class T>
	struct ResourceAndOwner
	{
		/**
		 * \brief The resource
		 */
		T* resource;

		/**
		 * \brief The owner
		 */
		Component* owner;
	};

private:
	// Helper class to implement RAII for the lock on ConfigurationManager
	class ConfigurationManagerLocker
	{
	public:
		ConfigurationManagerLocker(const std::unique_ptr<ConfigurationManager>& manager);
		~ConfigurationManagerLocker();

	private:
		const ConfigurationManager* const m_manager;
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the shared
	 *               information on resources
	 * \param configurationNode the configuration node to which we are
	 *                          associated. This is needed to resolve
	 *                          ambiguities when looking for a resource
	 */
	ResourceAccessor(ConfigurationManager& params, const ConfigurationNode* configurationNode);

	/**
	 * \brief Destructor
	 */
	virtual ~ResourceAccessor();

	/**
	 * \brief Returns a pointer to the resource declared with the specified
	 *        name
	 *
	 * In case there is more than one resource declared with the same name,
	 * this method tries to resolve the ambiguity in the following way:
	 * - it calculates the distance between the requester (this) and the
	 *   owner of the resource (who declared it) for any resource with the
	 *   same name (the distance is computed as d=lev(R)+lev(D)-2*lev(LCA);
	 *   see http://en.wikipedia.org/wiki/Lowest_common_ancestor for
	 *   information on what LCA is);
	 * - if the nearest resource is unique, it returns it;
	 * - otherwise, it fails with an exception.
	 * \param name the name of the resource to return
	 * \return the pointer to the resource
	 * \warning it will raise an exception if the type does not correspond
	 *          to what requested or the resource doesn't exist
	 */
	template <class T>
	T* getResource(QString name);

	/**
	 * \brief Returns a pointer to the resource declared with the specified
	 *        name and its owner
	 *
	 * This works like getResource(QString), but also returns a pointer to
	 * the owner of the resource.
	 * \param name the name of the resource to return
	 * \return the pointer to the resource and its owner
	 * \warning it will raise an exception if the type does not correspond
	 *          to what requested or the resource doesn't exist
	 */
	template <class T>
	ResourceAndOwner<T> getResourceAndOwner(QString name);

	/**
	 * \brief Returns a list of resources and owners with the specified
	 *        name
	 *
	 * This is like getResource(QString name), but does not throw an
	 * exception in case there is more than one "nearest" resource; instead
	 * it returns the list of nearest resources.
	 * \param name the name of the resouce to return
	 * \return the list of resources that are nearest to this along with
	 *         their owners
	 */
	template <class T>
	QList<ResourceAndOwner<T> > getAllCandidateResources(QString name);

	/**
	 * \brief Returns a pointer to the resource declared with the specified
	 *        name and the provided owner
	 *
	 * This only checks that the given owner has a resource with the given
	 * name and type. An exception is thrown if such a resource does not
	 * exists
	 * \param name the name of the resource
	 * \param owner the owner of the resource
	 * \return a pointer to the resource
	 */
	template <class T>
	T* getResource(QString name, Component* owner);

	/**
	 * \brief Returns all resources with the given name along with their
	 *        owner
	 *
	 * This returns an empty list if the resource does not exist
	 * \param name the name of the resource
	 * \return The list of resources with the given name and their owner
	 */
	template <class T>
	QList<ResourceAndOwner<T> > getAllResources(QString name);

	/**
	 * \brief Returns the list of owners of the resource with the given name
	 *
	 * This returns an empty list if the resource does not exist
	 * \param name the name of the resource
	 * \return the list of owners of the resource with the given name
	 */
	QList<Component*> getResourcesOwners(QString name);

	/**
	 * \brief Returns the list of owners of the resource with the given name
	 *        and the given type
	 *
	 * This returns an empty list if the resource does not exist
	 * \param name the name of the resource
	 * \return the list of owners of the resource with the given name and
	 *         the given type
	 */
	template <class T>
	QList<Component*> getResourcesOwners(QString name);

	/**
	 * \brief Returns the number of resources with the given name
	 *
	 * \param name the name of the resource
	 * \return the number of resources with the given name
	 */
	int resourcesCount(QString name) const;

	/**
	 * \brief Returns the number of resources with the given name and the
	 *        given type
	 *
	 * \param name the name of the resource
	 * \return the number of resources with the given name
	 */
	template <class T>
	int resourcesCount(QString name) const;

	/**
	 * \brief Returns true if a resource with the given name exists
	 *
	 * \param name the name of the resource
	 * \return true if a resource with the given name exists
	 */
	bool resourceExists(QString name) const;

	/**
	 * \brief Returns true if a resource with the given name and type exists
	 *
	 * \param name the name of the resource
	 * \return true if a resource with the given name and type exists
	 */
	template <class T>
	bool resourceExists(QString name) const;

	/**
	 * \brief Returns true if a resource with the given name and owner
	 *        exists
	 *
	 * \param name the name of the resource
	 * \param owner the owner of the resource
	 * \return true if a resource with the given name and owner exists
	 */
	bool resourceExists(QString name, Component* owner) const;

	/**
	 * \brief Returns true if a resource with the given name, type and owner
	 *        exists
	 *
	 * \param name the name of the resource
	 * \param owner the owner of the resource
	 * \return true if a resource with the given name, type and owner exists
	 */
	template <class T>
	bool resourceExists(QString name, Component* owner) const;

private:
	// Returns all resource handlers for candidate resources (those at
	// minimal distance from this).
	QList<ResourceHandler*> getAllCandidateResourceHandlers(QString name);

	// Returns all resource handlers for candidate resources (those at
	// minimal distance from this). This also filters by type
	template <class T>
	QList<ResourceHandler*> getAllCandidateResourceHandlers(QString name);

	// Filters the list so that only the resource handlers nearest to this
	// are returned
	QList<ResourceHandler*> getNearestResourceHandlers(QList<ResourceHandler*> list) const;

	// Return the map of resources (const and non-const)
	const QMap<QString, QMap<Component*, ResourceHandler*> >& resources() const;
	QMap<QString, QMap<Component*, ResourceHandler*> >& resources();

	// Returns the resource with the given name and owner or nullptr if it
	// doesn't exist
	ResourceHandler* getResourceHandlerWithOwner(QString name, Component* owner) const;

	// The ConfigurationNode to which we are associated. This is only needed
	// to resource resource name ambiguities
	const ConfigurationNode* const m_configurationNodeForContext;

	// The ConfigurationManager object from which we have been created. All
	// ConfigurationManager instances created via copy operations from the
	// same object share the same data structures. We use a pointer here so
	// that we do not have to include the ConfigurationManager header
	std::unique_ptr<ConfigurationManager> m_confManager;

	// Component is friend to access private members. Component inherits
	// from this, but we cannot make the private section protected because
	// otherwise all subclasses of Component would be able to access it
	friend class Component;
};

/**
 * \brief The interface for classes that want to receive notifications of
 *        resource changes
 *
 * See the description of the Component class for more information on resource
 * change notifications
 */
class SALSA_CONF_TEMPLATE ResourceChangeNotifee
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ResourceChangeNotifee()
	{
	}

	/**
	 * \brief The function called when an observed resource changes
	 *
	 * To get the resource, you can use the getResource() function in this
	 * class, unless the changeType is ResourceDeleted
	 * \param name the name of the resource that has changed
	 * \param owner the owner of the resource that has changed
	 * \param changeType the type of change
	 */
	virtual void resourceChanged(QString name, Component* owner, ResourceChangeType changeType)
	{
		Q_UNUSED(name)
		Q_UNUSED(owner)
		Q_UNUSED(changeType)
	}

	/**
	 * \brief Returns the resource that has changed
	 *
	 * This function can only be called from inside resourceChanged to
	 * obtain a pointer to the resource that changed when the changeType is
	 * not ResourceDeleted. It returns a pointer to the resource after the
	 * change. If called ouside resourceChanged or when the changeType is
	 * ResourceDeleted, an exception is thrown
	 */
	template <class T>
	T* getResource();

private:
	// The handler of the resource for which a notification is sent
	ResourceHandler* m_notifiedResourceHandler;

	// ResourceHandler is friend to set m_notifiedResourceHandler
	friend class ResourceHandler;
};

/**
 * \brief The class representing a resource
 *
 * This class represents a resource. It also contains the list of objects that
 * have to be notified when the resource changes
 *
 * \internal
 */
class SALSA_CONF_API ResourceHandler
{
public:
	/**
	 * \brief Constructor
	 *
	 * This creates a non-existing resource
	 * \param name the name of the resource. This cannot be changed once
	 *             set here in the constructor
	 * \param owner is the Component owning this Resource (the Component
	 *              which declared it)
	 */
	ResourceHandler(QString name, Component* owner);

	/**
	 * \brief Destructor
	 */
	~ResourceHandler();

	/**
	 * \brief Returns the name of the resource
	 *
	 * \return the name of the resource
	 */
	QString name() const
	{
		return m_name;
	}

	/**
	 * \brief Returns the component owning this resource
	 *
	 * \return the component owning this resource
	 */
	Component* owner() const
	{
		return m_owner;
	}

	/**
	 * \brief The set method for an int resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the int resource to set
	 */
	void set(int* res);

	/**
	 * \brief The set method for a float resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the float resource to set
	 */
	void set(float* res);

	/**
	 * \brief The set method for a double resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the double resource to set
	 */
	void set(double* res);

	/**
	 * \brief The set method for a bool resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the bool resource to set
	 */
	void set(bool* res);

	/**
	 * \brief The set method for a Resource resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the Resource resource to set
	 */
	void set(Resource* res);

	/**
	 * \brief The set method for a Component resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the Component resource to set
	 */
	void set(Component* res);

	/**
	 * \brief The set method for a QObject resource
	 *
	 * This also notifes all observers of this resource
	 * \param res the QObject resource to set
	 */
	void set(QObject* res);

	/**
	 * \brief Marks this resource as nullptr
	 *
	 * This also notifes all observers of this resource
	 */
	void declareAsNull();

	/**
	 * \brief The function to obtain the resource
	 *
	 * Specialization for primitive types are implemented after the class
	 * because we cannot explitly specialize a function in a non-namespace.
	 * If the resource is declared as nullptr or does not exists, returns nullptr
	 * \return the resource or nullptr if the type is not compatible
	 */
	template<class T>
	T* get() const
	{
		switch(m_type) {
			case t_resource:
				return dynamic_cast<T*>(m_pointers.res);
			case t_component:
				return dynamic_cast<T*>(m_pointers.par);
			case t_qObject:
				return dynamic_cast<T*>(m_pointers.obj);
			default:
				return nullptr;
		}
	}

	/**
	 * \brief Returns true if the resource has the given type
	 *
	 * Specialization for primitive types are implemented after the class
	 * because we cannot explitly specialize a function in a non-namespace.
	 * If the resource is delared as nullptr, this function returns true, if
	 * it doesn't exist, returns false.
	 * \return true if the type is compatible with the one of the resource
	 */
	template <class T>
	bool isType() const
	{
		if (m_type == t_declaredNull) {
			return true;
		}

		switch(m_type) {
			case t_resource:
				return (dynamic_cast<T*>(m_pointers.res) != nullptr);
			case t_component:
				return (dynamic_cast<T*>(m_pointers.par) != nullptr);
			case t_qObject:
				return (dynamic_cast<T*>(m_pointers.obj) != nullptr);
			default:
				return false;
		}
	}

	/**
	 * \brief Returns true if this holds an existing resource
	 *
	 * \return true if this holds an existing resource
	 */
	bool exists() const
	{
		return (m_type != t_nonExistent);
	}

	/**
	 * \brief Returns true if the resource is declared as null
	 *
	 * \return true if the resource is declared as null
	 */
	bool isNull() const
	{
		return (m_type == t_declaredNull);
	}

	/**
	 * \brief Adds a new observer
	 *
	 * If the resource already exists, this immediately calls the
	 * resourceChanged of the observer. If the observer is already
	 * observing, this does nothing
	 * \param n the object that will be notified when this resource changes
	 */
	void addNotifee(ResourceChangeNotifee* n);

	/**
	 * \brief Removes an observer
	 *
	 * If the resource still exists, this immediately calls the
	 * resourceChanged of the observer. If the observer is already
	 * not observing, this does nothing
	 * \param n the object that will be no longer notified when this
	 *          resource changes
	 */
	void removeNotifee(ResourceChangeNotifee* n);

	/**
	 * \brief Returns true if n is an observer for this resource
	 *
	 * \param n the object that will be searched in the set of observers
	 * \return true if n is an observer for this resource
	 */
	bool isNotifee(ResourceChangeNotifee* n) const;

private:
	// Notifies all observers that the resource has changed
	void notifyAll(ResourceChangeType changeType);

	// Notifies one observer that the resource has changed
	void notifyOne(ResourceChangeNotifee* notifee, ResourceChangeType changeType);

	// The name of this resource
	const QString m_name;

	// The component owning this Resource
	Component* const m_owner;

	// The enum with all the possible types for resources. t_nonExistent
	// means that there is no resource
	enum Type
	{
		t_resource,
		t_component,
		t_qObject,
		t_int,
		t_float,
		t_double,
		t_bool,
		t_declaredNull,
		t_nonExistent
	} m_type;

	// The union of all possible pointers
	union Pointers
	{
		Resource* res;
		Component* par;
		QObject* obj;
		int* intp;
		float* floatp;
		double* doublep;
		bool* boolp;
	} m_pointers;

	// The set of all objects that will be notified when this resource
	// changes
	QSet<ResourceChangeNotifee*> m_notifees;

	// Copy constructor (here to prevent usage)
	ResourceHandler(const ResourceHandler&);

	// Copy operator (here to prevent usage)
	ResourceHandler& operator=(const ResourceHandler&);
};

}

// Implementation of template functions
#include "configurationnode.h"

namespace salsa {

template <class T>
T* ResourceAccessor::getResource(QString name)
{
	return getResourceAndOwner<T>(name).resource;
}

template <class T>
ResourceAccessor::ResourceAndOwner<T> ResourceAccessor::getResourceAndOwner(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	QList<ResourceHandler*> candidates = getAllCandidateResourceHandlers<T>(name);

	if (candidates.size() != 1) {
		throw ResourceCannotResolveAmbiguityException(name.toLatin1().data(), candidates.size());
	}

	ResourceAndOwner<T> r;
	r.resource = candidates[0]->get<T>();
	r.owner = candidates[0]->owner();

	return r;
}

template <class T>
QList<ResourceAccessor::ResourceAndOwner<T> > ResourceAccessor::getAllCandidateResources(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	QList<ResourceHandler*> candidates = getAllCandidateResourceHandlers<T>(name);

	QList<ResourceAndOwner<T> > l;

	foreach (ResourceHandler* h, candidates) {
		ResourceAndOwner<T> ro;
		ro.resource = h->get<T>();
		ro.owner = h->owner();

		l.append(ro);
	}

	return l;
}

template <class T>
T* ResourceAccessor::getResource(QString name, Component* owner)
{
	ConfigurationManagerLocker locker(m_confManager);

	// Trying to retrieve the resource
	ResourceHandler* h = getResourceHandlerWithOwner(name, owner);

	if ((h == nullptr) || !(h->isType<T>())) {
		throw ResourceNotDeclaredException(name.toLatin1().data());
	}

	return h->get<T>();
}

template <class T>
QList<ResourceAccessor::ResourceAndOwner<T> > ResourceAccessor::getAllResources(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	QList<ResourceAndOwner<T> > l;

	QList<ResourceHandler*> resList = resources().value(name).values();

	foreach (ResourceHandler* h, resList) {
		if (h->isType<T>()) {
			ResourceAndOwner<T> ro;
			ro.resource = h->get<T>();
			ro.owner = h->owner();

			l.append(ro);
		}
	}

	return l;
}

template <class T>
QList<Component*> ResourceAccessor::getResourcesOwners(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	QList<Component*> o;

	QList<ResourceHandler*> resList = resources().value(name).values();

	foreach (ResourceHandler* h, resList) {
		if (h->isType<T>()) {
			o.append(h->owner());
		}
	}

	return o;
}

template <class T>
int ResourceAccessor::resourcesCount(QString name) const
{
	ConfigurationManagerLocker locker(m_confManager);

	if (resources().contains(name)) {
		const QList<ResourceHandler*>& handlers = resources()[name].values();

		int count = 0;
		foreach (const ResourceHandler* h, handlers) {
			if (h->isType<T>()) {
				++count;
			}
		}

		return count;
	} else {
		return 0;
	}
}

template <class T>
bool ResourceAccessor::resourceExists(QString name) const
{
	ConfigurationManagerLocker locker(m_confManager);

	return (resourcesCount<T>(name) != 0);
}

template <class T>
bool ResourceAccessor::resourceExists(QString name, Component* owner) const
{
        ConfigurationManagerLocker locker(m_confManager);

        ResourceHandler* h = getResourceHandlerWithOwner(name, owner);

        if (h != nullptr) {
                return h->isType<T>();
        } else {
                return false;
        }
}

template <class T>
QList<ResourceHandler*> ResourceAccessor::getAllCandidateResourceHandlers(QString name)
{
	// Trying to retrieve the resource
	QList<ResourceHandler*> resList = resources().value(name).values();

	// Filtering all Resource with wrong type and non-existing
	QMutableListIterator<ResourceHandler*> it(resList);
	while(it.hasNext()) {
		ResourceHandler* h = it.next();
		if (!(h->isType<T>())) {
			it.remove();
		}
	}

	QList<ResourceHandler*> candidates;

	if (resList.isEmpty()) {
		throw ResourceNotDeclaredException(name.toLatin1().data());
	} else if (resList.size() == 1) {
		// the casting to type T has been already checked during the
		// filtering above and also the existence has been already
		// checked during filtering so here we are sure to return a
		// valid pointer
		candidates.append(resList[0]);
	} else {
		candidates = getNearestResourceHandlers(resList);
	}

	return candidates;
}

template <class T>
T* ResourceChangeNotifee::getResource()
{
	if ((m_notifiedResourceHandler == nullptr) || !m_notifiedResourceHandler->isType<T>()) {
		throw CannotCallNotifeeGetResourceException();
	}

	return m_notifiedResourceHandler->get<T>();
}

template<>
inline int* ResourceHandler::get<int>() const
{
	if (m_type == t_int) {
		return m_pointers.intp;
	} else {
		return nullptr;
	}
}

template<>
inline float* ResourceHandler::get<float>() const
{
	if (m_type == t_float) {
		return m_pointers.floatp;
	} else {
		return nullptr;
	}
}

template<>
inline double* ResourceHandler::get<double>() const
{
	if (m_type == t_double) {
		return m_pointers.doublep;
	} else {
		return nullptr;
	}
}

template<>
inline bool* ResourceHandler::get<bool>() const
{
	if (m_type == t_bool) {
		return m_pointers.boolp;
	} else {
		return nullptr;
	}
}

template<>
inline bool ResourceHandler::isType<int>() const
{
	return (m_type == t_int) || (m_type == t_declaredNull);
}

template<>
inline bool ResourceHandler::isType<float>() const
{
	return (m_type == t_float) || (m_type == t_declaredNull);
}

template<>
inline bool ResourceHandler::isType<double>() const
{
	return (m_type == t_double) || (m_type == t_declaredNull);
}

template<>
inline bool ResourceHandler::isType<bool>() const
{
	return (m_type == t_bool) || (m_type == t_declaredNull);
}

} // end namespace salsa

#endif
