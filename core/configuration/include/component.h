/***************************************************************************
 *  FARSA Configuration Library                                            *
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

#ifndef COMPONENT_H
#define COMPONENT_H

#include "configurationconfig.h"
#include "configurationexceptions.h"
#include "componentdescriptors.h"
#include "componentresource.h"
#include <QList>
#include <QMap>
#include <QSet>
#include <cstdlib>
#include <memory>

namespace farsa {

class ComponentUI;
class ConfigurationNode;
class ConfigurationManager;
class ResourceHandler;

/**
 * \brief The base for classes that can be configured/saved using a
 *        ConfigurationManager object
 *
 * This is the base for classes that can be instantiated from configuration
 * files using the ConfigurationManager. If you inherit from this (directly or
 * not) and register the class with the type database (see TypesDB class),
 * instances of your class can be created directly from the configuration file
 * specifying the type parameter in a group (see ConfigurationManager for more
 * information). When an instance of a subclass of this class is created using
 * ConfigurationManager, the ConfigurationManager object is passed to the
 * constructor of the class along with the prefix of the group from which the
 * object is being generated (refer to the documentation of ConfigurationManager
 * for an example). This way you can access configuration parameters you should
 * use to configure the object either directly using member functions of
 * ConfigurationManager or functions in namespace ConfigurationHelper. The
 * configuration of the object can be performed either in the constructor or in
 * the configure function. You can get the ConfigurationManager object
 * containing configuration parameters using the configurationManager() function
 * (this is the same as the param parameter of constructor) and the prefix for
 * the group in the configuration file containing configuration parameters with
 * the confPath() function. Your class must declare whether configuration is
 * performed in the constructor or in the configure() function by implementing
 * the static function configuresInConstructor(). This function must return true
 * if configuration is performed in the constructor, false if configuration is
 * performed in the configure() function. Notice that if that function returns
 * true, the configure() function is *NOT* called. The default implementation in
 * this class returns true (so that you can change configuration strategy in
 * subclasses, see below). If you are inheriting from a subclass of Component
 * you should try to use the same configuration strategy of the parent (i.e. if
 * the parent configured in the constructor, you should perform configuration in
 * the constructor, if it configures itself in the configure() function, do the
 * same in the subclass). If you do this, you can avoid re-implementing the
 * configuresInConstructor function (the parent one will be used). It is
 * possible to change the configuration strategy only if the parent configures
 * itself in the constructor: in this case the subclass can decide to do its
 * configuration in the configure() function and so it must override the
 * configuresInConstructor() function to return false (notice that subclasses
 * of that subclass can no longer revert to the behaviour of configuring
 * themselves in the constructor). If you try to configure in constructor a
 * subclass of a class that configures itself in the configure function, an
 * exception is thrown at registration time.
 *
 * Every subclass must implement the static describe() function, which is used
 * to declare the list of parameters used in the class and to set their
 * description. The standard way of implementing this function is to first
 * declare a Descriptor object and then to add parameters, like in this example:
 *
 * \code
 * void MyComponent::describe(QString type)
 * {
 * 	Descriptor d(type, "MyComponent", "Bla bla...");
 * 	d.describeString("p1").def("default1").help("Bla bla...", "More bla...");
 * 	...
 *
 * 	// If there is a custom editor for parameters in this group, also set it
 * 	// here like so:
 * 	TypesDB::getInstance().registerEditorForType<MyComponentEditor>(type);
 * }
 * \endcode
 *
 * In the example also the widget to modify parameters of the class is
 * registered using the TypesDB::getInstance().registerEditorForType() function.
 * See ConfigurationWidget for more information on this topic. Also refer to
 * the description of the Descriptor class for more information on how to
 * describe parameters.
 *
 * Subclasses can also implement the postConfigureInitialization() method that
 * is called after all objects deriving from the creation of one object have
 * been created and configured (see the description of ConfigurationManager for
 * more information).
 *
 * All objects that are created by the same initial call to factory methods of
 * ConfigurationManager have a parent-child relationship which resembles the
 * structure of groups and subgroups in the configuration file. This hierarchy
 * can be browsed using functions of ConfigurationManager to browse the
 * configuration parameters hierarchy and to get objects associated to a group
 * (see ConfigurationManager documentation for more information).
 *
 * All subclasses of Component can also use the resource-related facilities (the
 * functions to access resources are defined in the ResourceAccessor class, but
 * will be discussed here). A resource is a pointer to a variable of a primitive
 * type (int, float, double, or bool) or to an object, that is associated to a
 * resource name. The resource name is a string that can be used to access the
 * resource. The purpouse of the resource mechanism is to allow objects to
 * declare certain resources at runtime and then let other objects use them
 * using the resource name. If you want an instance of a class to be used as a
 * resource, the class must be a subclass (direct or indirect) of one of the
 * following classes:
 * 	- Resource
 * 	- Component
 * 	- QObject
 * When you declare a resource, you give it a name and specify the pointer to an
 * object/variable of a primitive type, using the declareResource() function.
 * When you need to get the resource (i.e the pointer), you can access it
 * calling getResource() or similar methods and passing the resource name (more
 * information about the different ways to get resources are given below). Once
 * you declare a resource, you can change the pointer over time, simply
 * re-declaring a resource with the same resource name. Once declared, a
 * resource lives until the declaring object is not deleted. This means that you
 * cannot really delete a resource, but you can declare it a NULL using
 * declareResourceAsNull() (see below for important things to keep in mind when
 * changing or deleting a resource). After declareResourceAsNull() is called any
 * call to getResource will return NULL for that resource. Resources are
 * actually deleted only if their owner is deleted. Memory for the resource is
 * never explicity freed (not even when declareResourceAsNull() is called), so
 * this must be managed externally (by the object owning the resource).
 *
 * As stated above, resources are meant to give objects the possibility to share
 * data with other objects. All objects created using the same
 * ConfigurationManager object share resources, meaning that they can access
 * resources of other objects or create resources which will then be available
 * to the others. Only the Component who declared a resource, however, can
 * change it using declareResourceAsNull() or declareResource(). It is possible
 * (indeed very likely) that two objects will declare two resources with the
 * same name. This is not necessarily a problem and there is a mechanism to
 * disambiguate between different resources with the same name. First of all
 * every resource has an "owner", which simply is the object that declared that
 * resource. Also, as stated above, all objects created by the same
 * ConfigurationManager have a parent-child relationship. To explain how
 * resources name clashes are resolved, suppose you have the following
 * situation, in which resources names are between "":
 *
 * Experiment (resources: "Experiment", "globalres")
 *  |
 *  +--+ Robot0 (resources: "Robot0", "nnit")
 *  |  |
 *  |  +-- Sensor0 (resources: "Sensor0")
 *  |  |
 *  |  +-- Sensor1 (resources: "Sensor1")
 *  |  |
 *  |  +-- Motor0 (resources: "Motor0")
 *  |  |
 *  |  +-- NeuralNetwork (resources: "NeuralNetwork")
 *  |
 *  +--+ Robot1 (resources: "Robot1", "nnit")
 *  |  |
 *  |  +-- Sensor0 (resources: "Sensor0")
 *  |  |
 *  |  +-- Sensor1 (resources: "Sensor1")
 *  |  |
 *  |  +-- Motor0 (resources: "Motor0")
 *  |  |
 *  |  +-- NeuralNetwork (resources: "NeuralNetwork")
 *  |
 *  +-- Arena (resources: "Arena")
 *
 * First of all notice that every object has a resource with its same name: this
 * is because a resource with the same name of the group from which an object is
 * created is automatically declared when an object is created. So, in the above
 * example, there is no need for Experiment to declare a resource for e.g. the
 * Arena, because a resource named "Arena" already exists and is owned by the
 * Arena object. When an object uses getResource\<Type\>(name) to obtain a
 * pointer to a resource, various things can happend, depending on which is the
 * object that requests the resource and where the resource is. Here are some
 * key examples:
 * 	- Robot1 tries to get the resource "globalres": the resource "globalres"
 * 	  owned by Experiment is returned (it is the only resource with that
 * 	  name);
 * 	- Arena tries to get the resource "dummy": an exception is thrown
 * 	  because no such resource exists
 * 	- Sensor0 inside Robot0 tries to get the resource "nnit". There are two
 * 	  "nnit" resources, one owned by "Robot0" and one owned by "Robot1", but
 * 	  the one owned by Robot0 is returned because it is nearer to the
 * 	  requesting object (in terms of distance between tree nodes)
 * 	- Arena tries to get the resource "nnit": in this case the two "nnit"
 * 	  resources in Robot0 and Robot1 are at the same distance from Arena, so
 * 	  an exception is thrown
 * Another factor coming into play when resolving resource name ambiguities is
 * the type of the requested resource: if a resource with the requested name
 * exists, but has not the correct type, it is not considered in the list of
 * possible resources to return. The description of the
 * getResource\<Type\>(name) function gives some more detail on how the
 * resolution algorithm works.
 *
 * As explained above, the process by which resources to return are identified
 * can be rather complex and, in the case of a lot of name clashes, relatively
 * computationally expensive. Because of this, a better approach is to keep a
 * pointer to the resource and update it in the notification function (described
 * below) or obtain a ResourceHolder object (see that class description for more
 * information), which automatically changes the resource pointer if the
 * resource changes.
 *
 * There are other functions that can be used to obtain a resource or
 * resource-related information. The getResource\<Type\>(name, owner) function
 * returns the resource owned by the given owner. If owner has no such resource,
 * an exception is thrown (the resource is not found). The
 * getAllResources\<Type\>(name) function returns all resources with the given
 * name and type (regardless of their distance from the requesting object),
 * along with their respective owners. The getResourcesOwners(name) function
 * returns a list of owners of resources with the given name, regardless of
 * their type and distance from the requesting object. The
 * getResourcesOwners\<Type\>(name) is similar to the preceeding one, but
 * filters resources by type (i.e. only returns owners of resources that have
 * the given type). Finally, the function getConflictingResources\<Type\>(name)
 * returns the list of conflicting resources in case of ambiguities.
 *
 * A notification mechanism for resource changes is available (see also the
 * description of the ResourceChangeNotifee class, from which this inherit). You
 * can decide for which resoures you want notifications using the
 * addNotifiedResource(), removeNotifiedResource(), and
 * removeAllNotifiedResources() functions. These functions allow specifying the
 * owner of the resource for which notifications are requested. If the owner is
 * not specified (or set to NULL), an algorithm like the one described for the
 * getResource\<Type\>(name) function is used to resolve name ambiguities (this
 * means that an exception could be thrown in case the ambiguity cannot be
 * resolved); if the owner is specified the resource of that owner is used (see
 * the description of each function for more information). When a resource in
 * the list changes, the resourceChanged() function is called with information
 * about the resource that changed and how it changed i.e. whether it was added,
 * modified, declared as NULL or deleted (a resource is deleted if its owner is
 * deleted). Please note that the resourceChanged() function is called in the
 * thread of the resource owner, see below for more information on possible
 * multithreading issues and for exceptions to this rule. Moreover, if resources
 * are used during the configuration phase, there are some things to be aware of
 * (see below).
 *
 * The list of resources for which notifications are to be received can contain
 * non-existing resources: the notification function will be called as soon as
 * the resource is created (see below for more information on what happends
 * during the configuration phase). In this case, however, it is compulsory to
 * also specify the owner of the resource, i.e. notifications will be sent if
 * the given owner creates a resource with the given name (we need the owner
 * because it would otherwise not be clear to which resource we should refer). A
 * notification will be sent also when a resource is declared as NULL (using
 * declareResourceAsNull()), but this doesn't influence the list of resources
 * for which notifications have to be sent. This means that when the resource is
 * re-declared (by the same owner), you will be notified again. When however a
 * resource is actually deleted because the owner has beeen destroyed (i.e. the
 * status you receive in the notification function is ResourceDeleted), the
 * resource is removed from the notification list. When a resource is added to
 * the list of resources for which a notification is expected, a notification is
 * sent if the resource already exists. So, for example, if a resource named "A"
 * exists and you call setNotifiedResources() declaring you want notifications
 * for resources "A" and "B", the resourceChanged() function will be called for
 * "A" with changeType set to ResourceCreated (during the call to
 * setNotifiedResources()). The same holds if a resource is removed from the
 * list of resources for which notification is requested: if the resource exists
 * by the time it is removed from the list, a notification will be sent with
 * changeType set to ResourceDeleted
 *
 * The resource mechanism works as described above during the normal life cycle
 * of objects. When however they are used during the configuration phase (i.e.
 * during calls to the constructor, the configure() function or the
 * postConfigureInitialization() function), they behave in a slightly different
 * way. The difference is in the notification mechanism: no notification is sent
 * during the configuration phase. This means that if you register a resource
 * whose changes you want to be notified of during the configuration phase, all
 * notifications will be sent after the postConfigureInitialization() function
 * of all objects has been called (i.e. just before returning from the outermost
 * call to getComponentFromGroup() or similar functions). During configuration,
 * then, you can use resources but you should not keep pointers and always call
 * getResource(). It is your responsability to ensure that the resources you
 * need actually exist before you attempt to use them (if not sure, check if a
 * resource exists before using it or try to delay its use until the end of the
 * configuration phase). One other exception which holds during the
 * configuration phase is that you need not to specify the owner of a resource
 * that doesn't exists when you ask for notifications: the actual registration
 * of the notification is done at the end of the configuration phase, so the
 * resource must exists by that time. In other words, if you are sure that the
 * resource will be declared by some object before the configuration phase ends,
 * you can safely ask for notifications regarding that resource at any time
 * during the configuration phase without specifying the owner of the resource.
 * When the configuration phase ends, the registration will take place using the
 * rules described above (in particular, if you do not specify the owner and the
 * resource hasn't been declared or there is a conflict while resolving the
 * resource name, an exception will be thrown).
 *
 * Some considerations about multithreading. The ConfigurationManager is thread
 * safe (see its documentation for more information) and, as it also holds
 * resources, accessing resources is also thread safe. It is important to
 * understand, however, that using resources is NOT thread safe unless the
 * resource itself is thread safe. In other words, once you get the pointer to a
 * resource, there is not an automatic way to ensure that working with the
 * pointer is thread safe. If you need to share the same resource across
 * different threads, it is up to you to ensure that concurrent accesses are
 * regulated. A further limitation is that once you get the pointer, the pointer
 * itself cannot be guaranteed to remain valid. If, for instance, the owner of a
 * resource and an object using it live in different threads, it is possible
 * that the user takes the pointer of the resource and, upon return from
 * getResource, the resource is changed by the owner, rendering the pointer just
 * obtained by the user invalid without it knowing this. To cope with this
 * situation, you must re-implement the resourceChanged() function and make it
 * thread-safe: that function will be called in the thread of the owner of the
 * resource that changed (apart from two exceptions described below). So, to be
 * sure to use a valid pointer to a resource shared between different threads,
 * you could use a mutex to protect the pointer and change it in the
 * notification function: this way if the owner tries to change the pointer to
 * the resource, it will wait in the notification function of the user if the
 * pointer is being used by the user. If you use a ResourceHolder object to keep
 * the resource pointer, you can specify whether you want a mutex to be locked
 * when the resource is changed or not (see the class description for more
 * information).
 *
 * The two exceptions to the rule asserting that the resourceChanged is called
 * in the thread of the owner of the resource regard registration and
 * de-registration of a resource notifications. When using one of the
 * addNotifiedResource() functions, if the resource already exists, the
 * resourceChanged handler is called immediately (before returning from
 * addNotifiedResource()) with changeType set to ResourceCreated. Simmetrically,
 * when using one of the removeNotifiedResource() functions, if the resource
 * still exists, the resourceChanged handler is called immediately (before
 * returning from removeNotifiedResource()) with changeType set to
 * ResourceDeleted.
 *
 * An important notice about changing a resource: it is possible that objects
 * using a given resource keep a pointer to it (because they need to do some
 * cleanup with the old resource before start using the new one, or for
 * efficiency reasons or they keep the pointer implicitly because they use
 * ResourceHolder). Because of this it is important that when a resource is
 * re-declared, you first call declareResource() with the new istance of the
 * resource and only later eventually delete the old one. The same holds for
 * declareResourceAsNull(): first call declareResourceAsNull() and then free
 * memory for the old resource, if you need to. Further care should be used if a
 * resource owner is destroyed. Unless you are sure that by the time the object
 * is destroyed its resources are not being used, you should use
 * declareResourceAsNull() in the destructor before deleting the resource. This
 * way objects using the resource will first be notified that the pointer has
 * been set to NULL and then that the resource has been deleted (this is needed
 * because the notification that a resource has been deleted will be sent by the
 * destructor of Component and by that time the resource should have been
 * already destroyed).
 *
 * \warning All classes that have Component as an ancestor should be only
 *          created and configured using ConfigurationManager. Calling
 *          configure() or postConfigureInitialization() by hand is highly
 *          discouraged (you could experience strange problems...)
 *
 * \ingroup configuration_factory
 */
class FARSA_CONF_API Component : public ResourceAccessor, protected ResourceChangeNotifee
{
public:
	/**
	 * \brief The function returning whether instances of this class
	 *        configure themselves in the constructor or not
	 *
	 * \return the default implementation returns true (see class
	 *         description)
	 */
	static bool configuresInConstructor()
	{
		return true;
	}

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters; this will be used during configuration and
	 *               shared amongst all Component created
	 */
	Component(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~Component();

	/**
	 * \brief Configures the object using the ConfigurationManager
	 *
	 */
	virtual void configure()
	{
	}

	/**
	 * \brief Add to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * It is mandatory in all subclasses where new parameters are defined to
	 * also implement this method
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 *
	 * See the description of the ConfigurationManager class for more
	 * information. The default implementation does nothing
	 */
	virtual void postConfigureInitialization()
	{
	}

	/**
	 * \brief Returns an instance of the ComponentUI class that handles the
	 *        viewers of this component
	 *
	 * The returned ComponentUI instance is handled externally, you must not
	 * delete it. The default implementation returns NULL.
	 * \return a new-allocated instance of the class handling the viewers of
	 *         this component or NULL if this component has no viewers
	 */
	virtual ComponentUI* getUIManager()
	{
		return NULL;
	}

	/**
	 * \brief Returns the type name of this object
	 *
	 * This function returns the name which the object has been registered
	 * into the factory; set the value of the parameter "type" to this in a
	 * configuration file in order to create an instance of this class
	 * \return the name of the type of this object
	 * \note It is possible that this does not correspond to the C++ class
	 *       name of the object; this happens when the class is registered
	 *       into the factory with a name different from the C++ class name
	 *       (possible but discouraged...)
	 */
	QString typeName() const
	{
		return m_type;
	}

	/**
	 * \brief Returns the ConfigurationManager instance from which this
	 *        object has been created and configured (const version)
	 *
	 * \return the ConfigurationManager instance from which this object has
	 *         been created and configured
	 */
	const ConfigurationManager& configurationManager() const
	{
		return *m_confManager;
	}

	/**
	 * \brief Returns the path of the group from which this object has been
	 *        created
	 *
	 * \return the path of the group from which this object has been created
	 */
	const QString& confPath() const
	{
		return m_prefix;
	}

	/**
	 * \brief Declares the name for a resource
	 *
	 * This method overwrites any previous declaration with the same name.
	 * \param name the name of the resource to declare
	 * \param resource the pointer to the object/variable resource (this
	 *                 must not be NULL, see declareResourceAsNull)
	 */
	template<class T>
	void declareResource(QString name, T* resource);

	/**
	 * \brief Declares a resource as being NULL
	 *
	 * You cannot use declareResource to set the resource to NULL, use this
	 * function instead. A resource declared as NULL exists and all attempts
	 * to get it return NULL, no matter the type you specify when calling
	 * e.g. getResource.
	 * \param name the name of the resource to declare as NULL
	 */
	void declareResourceAsNull(QString name);

	/**
	 * \brief Deletes the resource owned by us with the specified name
	 *
	 * \param name the name of the resource to delete
	 * \warning it will raise an exception if the resource doesn't exist
	 */
	void deleteResource(QString name);

	/**
	 * \brief Deletes all the resources owned by us
	 */
	void deleteAllResources();

	/**
	 * \brief Adds a resource to the list of resources for which
	 *        notifications are requested
	 *
	 * If changes to the resource are already notified, this function does
	 * nothing. If the resource does not exists or multiple resources with
	 * the same name exist, an exception is thrown
	 * \param name the name of the resource for which notifications are
	 *             requested
	 */
	void addNotifiedResource(QString name);

	/**
	 * \brief Adds a resource to the list of resources for which
	 *        notifications are requested
	 *
	 * This throws an exception if the resource doesn't exists or there is
	 * an ambiguity. If changes to the resource are already notified, this
	 * function does nothing
	 * \param name the name of the resource for which notifications are
	 *             requested
	 * \param owner the owner of the resource for which notifications are
	 *              requested
	 */
	void addNotifiedResource(QString name, Component* owner);

	/**
	 * \brief Removes a resource from the list of resources for which
	 *        notifications are requested
	 *
	 * This throws an exception if the resource doesn't exists or there is
	 * an ambiguity. If changes to the resource are not notified already,
	 * this function does nothing
	 * \param name the name of the resource for which notifications are
	 *             no longer requested
	 * \warning Be careful using this function: if after the call to
	 *          addNotifiedResource a new resource with the same name is
	 *          declared which is selected automatically because it is
	 *          nearer to this node, a call to this function will not remove
	 *          the association. For example, suppose the hierarchy of
	 *          components is like this:
	 *                  ComponentA
	 *                       |
	 *                       +-> ComponentB
	 *                                |
	 *                                +-> ComponentC
	 *          ComponentC declares the resource "res", then ComponentA
	 *          calls addNotifiedResource("res"). Afterward, ComponentB
	 *          declares the resource "res". Now, if ComponentA calls
	 *          removeNotifiedResource("res"), the function will attempt to
	 *          remove notifications for resource "res" in ComponentB (which
	 *          is nearer) and so the association with the resource "res" in
	 *          ComponentC will not be removed. Call the version of this
	 *          function taking also the owner of the resource in this case.
	 */
	void removeNotifiedResource(QString name);

	/**
	 * \brief Removes a resource from the list of resources for which
	 *        notifications are requested
	 *
	 * This throws an exception if the resource doesn't exists or there is
	 * an ambiguity. If changes to the resource are not notified already,
	 * this function does nothing
	 * \param name the name of the resource for which notifications are
	 *             no longer requested
	 * \param owner the owner of the resource for which notifications are
	 *               no longer requested
	 */
	void removeNotifiedResource(QString name, Component* owner);

	/**
	 * \brief Stops receiving all notifications for all resources
	 */
	void removeAllNotifiedResources();

	// These are needed to be able to resolve ambiguities when calling
	// getResource()
	using ResourceAccessor::getResource;
	using ResourceChangeNotifee::getResource;

protected:
	/**
	 * \brief Returns the ConfigurationManager instance from which this
	 *        object has been created and configured
	 *
	 * \return the ConfigurationManager instance from which this object has
	 *         been created and configured
	 */
	ConfigurationManager& configurationManager()
	{
		return *m_confManager;
	}

private:
	// Returns the resource handler for a resource we own, creating it if it doesn't exists
	ResourceHandler* getResourceHandlerForOurResource(QString name);

	// Returns true if we are in the configuration phase
	bool inConfigurationPhase() const;

	// Records a request for notification of resource change
	void recordRequestOfResourceChangeNotification(QString resourceName, Component* owner);

	// Removes a previously recorded request for notification of resource change
	void removeRequestOfResourceChangeNotification(QString resourceName, Component* owner);

	// Removes all request for notification of resource change previously recorded by us
	void removeAllOurRequestOfResourceChangeNotification();

	// The name of the type of this object
	const QString m_type;

	// The ConfigurationNode to which we are associated. We have to keep a
	// copy here even if another one is in ResourceAccessor because this
	// need to be non-const
	ConfigurationNode* const m_configurationNode;

	// The prefix to the group from which this object has been created
	const QString m_prefix;
};

}

// Implementation of template functions
#include "configurationnode.h"

namespace farsa {

template<class T>
void Component::declareResource(QString name, T* resource)
{
	ConfigurationManagerLocker locker(m_confManager);

	ResourceHandler* h = getResourceHandlerForOurResource(name);

	// Now setting the resource. If this throws an exception we could have simply created a non-existing
	// resource above that doesn't modify the external behaviour of this and other classes (so we can say
	// that this function is strongly exception-safe)
	h->set(resource);
}

} // end namespace farsa

#endif
