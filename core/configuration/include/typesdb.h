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

#ifndef TYPESDB_H
#define TYPESDB_H

#include "configurationconfig.h"
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QVector>
#include <typeinfo>
#include <memory>
#include <cstdlib>

namespace farsa {

class ConfigurationWidget;
class Component;
class ConfigurationManager;
class ResourceAccessor;
class RegisteredComponentDescriptor;

/**
 * \brief A class storing information on a type
 *
 * A type can be:
 * 	- non-abstract subclass of farsa::Component. In this case canBeCreated
 * 	  is true;
 * 	- abstract subclass of farsa::Component. In this case canBeCreated is
 * 	  false and isInterface is false;
 * 	- a class not inheriting from farsa::Component (neither directly nor
 * 	  indirectly), also known as an "interface". In this case canBeCreated
 * 	  is false and isInterface is true.
 */
class FARSA_CONF_TEMPLATE RegisteredTypeInfo
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param name the name of the type
	 * \param parents the parent classes
	 * \param canBeCreated whether this type can be created (i.e. is a
	 *                     non-abstract subclass of farsa::Component) or not
	 * \param isInterface when this type cannot be created, whether it is an
	 *                    interface or an abstract subclass of
	 *                    farsa::Component
	 * \param configuresInConstructor when this is a subclass of
	 *                                farsa::Component, whether this
	 *                                performs configuration in constructor
	 *                                or not
	 */
	RegisteredTypeInfo(const QString& name, const QStringList& parents, bool canBeCreated, bool isInterface, bool configuresInConstructor)
		: name(name)
		, parents(parents)
		, canBeCreated(canBeCreated)
		, isInterface(isInterface)
		, configuresInConstructor(configuresInConstructor)
	{
	}

	/**
	 * \brief Constructor
	 *
	 * All parameters are set to default values
	 */
	RegisteredTypeInfo()
		: name("")
		, parents()
		, canBeCreated(false)
		, isInterface(true)
		, configuresInConstructor(true)
	{
	}

	/**
	 * \brief The name of the type
	 */
	QString name;

	/**
	 * \brief The list of parent classes
	 */
	QStringList parents;

	/**
	 * \brief Whether the type can be instantiated or not
	 */
	bool canBeCreated;

	/**
	 * \brief Whether the type if an interface or not
	 *
	 * This is only valid if canBeCreated is false
	 */
	bool isInterface;

	/**
	 * \brief Whether the type configures itself in the constructor or not
	 *
	 * This is only valid if (canBeCreated == true || isInterface == false)
	 */
	bool configuresInConstructor;
};

/**
 * \brief The base class for object creators. The implementation is in the
 *        template class below
 *
 * If you want to provide a custom creator for a type, create a subclass of this
 */
class FARSA_CONF_TEMPLATE ComponentCreator
{
public:
	/**
	 * \brief Constructor
	 */
	ComponentCreator()
		: m_typeInfo(NULL)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ComponentCreator()
	{
	}

	/**
	 * \brief Returns type info
	 *
	 * \return type info
	 */
	const RegisteredTypeInfo& typeInfo() const
	{
		return *m_typeInfo;
	}

	/**
	 * \brief Creates an instance and configures it
	 *
	 * \param settings the object with configuration settings for
	 *                 initializing the newly created object
	 * \param prefix the prefix for configuration parameters (i.e. the group
	 *               containing parameters)
	 * \param configure if true the object is created and configured, if
	 *                  false only the creation step is performed. This is
	 *                  only taken into account if the
	 *                  configuresInConstructor() function below returns
	 *                  false, otherwise is ignored (and the object is
	 *                  created and configured)
	 * \return the new object
	 */
	Component* create(ConfigurationManager& settings, QString prefix, bool configure);

private:
	/**
	 * \brief Creates an instance
	 *
	 * This function must instantiate an object and return a pointer to it.
	 * The object must be created using "new", because it is destroyed using
	 * "delete". It is called by create(). The implementation is in
	 * subclasses.
	 * \param settings the object with configuration settings for
	 *                 initializing the newly created object
	 * \param prefix the prefix for configuration parameters (i.e. the group
	 *               containing parameters). This can be used to access
	 *               parameters of the Component to create before the
	 *               Component itself is instantiated
	 * \param accessorForResources an instance of ResourceAccessor that can
	 *                             be used to access resources. This is
	 *                             provided to be able to get resources with
	 *                             the same results one would obtain with
	 *                             the Component that is being created, but
	 *                             before the Component actually exists. Do
	 *                             not store or use this object in other
	 *                             ways, as it is destroyed when this
	 *                             function exits
	 * \return the new object
	 */
	virtual Component* instantiate(ConfigurationManager& settings, QString prefix, ResourceAccessor* accessorForResources) = 0;

	// Type information. This is set by TypesDB (which is friend)
	const RegisteredTypeInfo* m_typeInfo;

	friend class TypesDB;
};

/**
 * \brief The class implementing the create function of the class above
 *
 * This is the default creator. It works for classes whose constructor only
 * takes an instance of the ConfigurationManager object and that have a static
 * configuresInConstructor() function that returns true or false depending on
 * whether the component configuration is performed in the constructor or in the
 * configure() function. This is the class used for classes that can be
 * instantiated.
 * \note All methods are reentrant
 *
 * \internal
 */
template <class T>
class FARSA_CONF_TEMPLATE ComponentCreatorT : public ComponentCreator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ComponentCreatorT()
	{
	}

private:
	virtual Component* instantiate(ConfigurationManager& settings, QString prefix, ResourceAccessor* accessorForResources);
};

/**
 * \brief The base class for configuration widgets creators. The implementation
 *        is in the template class below
 *
 * \internal
 */
class FARSA_CONF_TEMPLATE ConfigurationWidgetCreator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ConfigurationWidgetCreator()
	{
	}

	/**
	 * \brief Creates an instance
	 *
	 * The implementation is in the template class below
	 * \param params the configuration parameters object to edit
	 * \param prefix the group to edit
	 * \param parent the parent widget of the editor
	 * \param f windows flags for the editor
	 * \return the editor for the group
	 */
	virtual ConfigurationWidget* create(ConfigurationManager& params, QString prefix, QWidget* parent, Qt::WindowFlags f) const = 0;
};

/**
 * \brief The class implementing the create function of the class above
 *
 * \note All methods are reentrant
 *
 * \internal
 */
template <class T>
class FARSA_CONF_TEMPLATE ConfigurationWidgetCreatorT : public ConfigurationWidgetCreator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ConfigurationWidgetCreatorT()
	{
	}

	/**
	 * \brief Creates an instance
	 *
	 * The implementation is in the template class below
	 * \param params the configuration parameters object to edit
	 * \param prefix the group to edit
	 * \param parent the parent widget of the editor
	 * \param f windows flags for the editor
	 * \return the editor for the group
	 */
	virtual ConfigurationWidget* create(ConfigurationManager& params, QString prefix, QWidget* parent, Qt::WindowFlags f) const;
};

/**
 * \brief The class that registers various types used in FARSA
 *
 * This is only used to register types (Component and ConfigurationWidget
 * subclasses, plus interfaces) that can later be created using a
 * ConfigurationManager object. The difference between the a component and an
 * interface is that a component is always a subclass (direct or indirect) of
 * farsa::Component, while an interface can be any class. Interfaces cannot be
 * instantiated, but can be used to constrain the type of a group (see
 * Component::describe() and ComponentDescriptor class). When registering a
 * type, all the component and interfaces from which it inherits must be already
 * registered. To register new types use one of the registerComponent functions,
 * they automatically distinguish between component and interfaces (interfaces
 * are simply classes that are not derived from farsa::Component)
 *
 * This class also has some methods to get meta-information on registered types
 * and their parameters.
 *
 * Note that this class IS NOT THREAD-SAFE, so types registration must never be
 * performed in parallel. Moreover no creation using ConfigurationManager
 * methods must be performed in parallel to types registrations as both
 * registration and creation access the same structure that are not protected
 * from concurrent accesses.
 *
 * \note It is only possible to re-register a class if it has the same parents
 *       and the same characteristics (i.e. being a component, an abstract
 *       component or an interface and having the same configuration strategy)
 *       of the class already registered
 * \note This is a singleton, use getInstance to get the only instance of this
 *       class
 * \note This class it NOT thread-safe
 *
 * \ingroup configuration_factory
 */
class FARSA_CONF_API TypesDB
{
public:
	/**
	 * \brief Returns the only instance of this class
	 *
	 * \return the only instance of this class
	 */
	static TypesDB& instance();

	/**
	 * \brief Returns a const reference to the descriptor for the given type
	 *
	 * If the type is not registered, an exception is thrown. Descriptors
	 * are only available for types that are registered and either can be
	 * created or cannot but are not interfaces (i.e. for subclasses of
	 * Component). Use the isTypeRegisteredAndComponent() function to check
	 * that the type has a descriptor
	 * \param typeName the name of the type
	 * \return a const reference to the type description
	 */
	const RegisteredComponentDescriptor& typeDescriptor(QString typeName) const;

	/**
	 * \brief Registers a new type
	 *
	 * The type being registered can be an instantiable Component or not.
	 * If it is, it must have a constructor that only takes an instance of
	 * the ConfigurationManager object and that have a static
	 * configuresInConstructor() function that returns true or false
	 * depending on whether the component configuration is performed in the
	 * constructor or in the configure() function. If this is an abstract
	 * Component subclass, only the requirement of the existence of the
	 * configuresInConstructor() function must be respected. If this is not
	 * a Component subclass, there is no particular requirement. If your
	 * class does not meet these requirements, use on of the other
	 * registration function and provide a custom creator.
	 * \param className the name of class used when requesting object
	 *                  creation
	 * \param parents the list of the parent classes. All elements must
	 *                already be registered (even if they are not
	 *                farsa::Component subclasses), otherwise an exception
	 *                is thrown
	 *
	 * \note It's IMPORTANT to register also all abstract classes and
	 *       interfaces even if it's not possibile to create them (as they
	 *       could be the parent of other concrete classes)
	 */
	template <class NewClass>
	void registerType(QString className, QStringList parents);

	/**
	 * \brief Registers a new instatiable class with a custom creator
	 *
	 * The ObjectCreator template parameter is the type of the creator that
	 * is used to generate instances of NewClass (it must be a subclass of
	 * ComponentCreator with a constructor taking no arguments). If the type
	 * being registered cannot be instantiated, and exception is thrown
	 * \param className the name of class used when requesting object
	 *                  creation
	 * \param parents the list of the parent classes. All elements must
	 *                already be registered (even if they are not
	 *                farsa::Component subclasses), otherwise an exception
	 *                is thrown
	 *
	 * \note It's IMPORTANT to register also all abstract classes and
	 *       interfaces even if it's not possibile to create them (as they
	 *       could be the parent of other concrete classes)
	 */
	template <class NewClass, class ObjectCreator>
	void registerType(QString className, QStringList parents);

	/**
	 * \brief Registers a new instatiable class with a custom creator
	 *
	 * If the type being registered cannot be instantiated, and exception is
	 * thrown
	 * \param className the name of class used when requesting object
	 *                  creation
	 * \param parents the list of the parent classes. All elements must
	 *                already be registered (even if they are not
	 *                farsa::Component subclasses), otherwise an exception
	 *                is thrown
	 * \param creator a pointer to the instance of the creator to use. You
	 *                must pass a new-allocated object which is then managed
	 *                by this class (you MUST NOT delete it externally)
	 *
	 * \note It's IMPORTANT to register also all abstract classes and
	 *       interfaces even if it's not possibile to create them (as they
	 *       could be the parent of other concrete classes)
	 */
	template <class NewClass>
	void registerType(QString className, QStringList parents, ComponentCreator* creator);

	/**
	 * \brief Returns true if the class has been registered
	 *
	 * \param className the class to check
	 * \returns true if the class has been registered, false otherwise
	 */
	bool isTypeRegistered(QString className) const;

	/**
	 * \brief Returns true if the class has been registered and is a
	 *        Component subclass
	 *
	 * If this returns true, the type has a descriptor
	 * \param className the class to check
	 * \returns true if the class has been registered and is a Component
	 *          subclass
	 */
	bool isTypeRegisteredAndComponent(QString className) const;

	/**
	 * \brief Returns information on a registered type
	 *
	 * This throws an exception if the type has not been registered
	 * \param className the name of the class to check
	 * \return true if the class is abstract
	 */
	const RegisteredTypeInfo& typeInfo(QString className) const;

	/**
	 * \brief Returns the ComponentCreator for the specified className
	 *
	 * This throws an exception if the type has not been registered or
	 * cannot be instantiated
	 * \param className the class for which the creator is requested
	 * \return the creator object for the given class
	 */
	ComponentCreator* getComponentCreator(QString className);

	/**
	 * \brief Returns all subclasses of a class up to the specified
	 *        inheritance level
	 *
	 * This throws an exception of type ClassNameNotRegisteredException if
	 * the type has not been registered
	 * \param className the name of the parent class of the subclasses
	 *                  returned
	 * \param levelToStop if -1 it returns all the subclasses on all level
	 *                    of inheritance; otherwise it returns the
	 *                    subclasses up to the specified level.
	 * \param noAbstractClasses if true the abstract subclasses and the
	 *                          interfaces will be filtered out, otherwise
	 *                          they will be included into the returned list
	 * \return the list of subclasses of the given class
	 *
	 * \note levelToStop == 1 means all direct children are returned; so
	 *       levelToStop == 0 means no subclasses will be returned!
	 */
	QStringList getAllTypeSubclasses(QString className, int levelToStop = -1, bool noAbstractClasses = false);

	/**
	 * \brief Registers a class to be the editor for the give type
	 *
	 * The class being registered must be a subclass of ConfigurationWidget.
	 * Moreover the type must be already present in typeDescriptions,
	 * otherwise an exception is thrown
	 * \param type the type whose editor is being registered. It must be
	 *             already present in typeDescriptions, otherwise an
	 *             exception is thrown
	 */
	template <class EditorType>
	void registerEditorForType(QString type);

	/**
	 * \brief Returns an editor for the given group
	 *
	 * The ConfigurationWidget object receives the given
	 * ConfigurationManager instance and the group (prefix) and can be
	 * used to edit that group parameters. Use the
	 * ConfigurationWidget::configuringSubgroups() function to understand if
	 * the widget also manages subgroups of prefix or not. For this function
	 * to work the group must be of a type that is present in
	 * typeDescriptions or have no type, otherwise an exception is thrown.
	 * If no editor is explicitly set NULL is returned.
	 * \param params the configuration parameters object to edit
	 * \param prefix the group to edit
	 * \param parent the parent widget of the editor
	 * \param f windows flags for the editor
	 * \return the editor for the group or NULL
	 */
	ConfigurationWidget* getEditorForType(ConfigurationManager& params, QString prefix, QWidget* parent = NULL, Qt::WindowFlags f = 0);

	/**
	 * \brief Dumps the description of all registered types to a file
	 *
	 * This calls ConfigurationManager::saveParameters()
	 * \param filename the name of the file to which parameters should be
	 *                 saved
	 * \param format the format of the file to save. If "" format is guessed
	 *               from filename extension. If guessing fails, loading
	 *               fails
	 * \return false if saving fails, true otherwise
	 */
	bool dumpTypeDescription(QString filename, QString format = "") const;

private:
	template <class T>
	std::auto_ptr<const RegisteredTypeInfo> generateInfoForType(const QString& name, const QStringList& parents);

	// This actually performs registration. creator should be NULL if the type cannot be instantiated
	template <class NewClass>
	void registerType(std::auto_ptr<const RegisteredTypeInfo> info, std::auto_ptr<ComponentCreator> creator);

	// Checks if re-registration of a class is possible. A class can be re-registered only if it has
	// the same parents and the same characteristics of the already registered class. If the class is
	// not registered, the function does nothing. If the class cannot be re-registered, an exception
	// is thrown
	void checkReRegistrationIsPossible(const RegisteredTypeInfo* info);

	/// Checks that parents are registered
	void checkParentsRegistered(const RegisteredTypeInfo* info);

	// Performs checks on the parents of a component being registered. Only call this after
	// checkParentsRegistered (i.e. if you are sure all parents exist) and  only for components
	// (i.e. not for interfaces)
	void componentRegistrationChecks(const RegisteredTypeInfo* info);

	// Returns a reference to the descriptor for the given type. If the descriptor does not exists
	// creates it. All created descriptors are kept in a map
	RegisteredComponentDescriptor& editableTypeDescriptor(QString typeName);

	// Calls the describe function of the component. This must not be called for non-components
	template <class T>
	void describeComponent(QString className);

	// Called for classes that are not Components (aborts, should never be called)
	template <class T>
	void callDescribeIfComponentSubclass(QString, ...);

	// Actually describes the component
	template <class T>
	void callDescribeIfComponentSubclass(QString className, Component*);

	// Private constructor to prevent instantiation of this class, this is a singleton
	TypesDB();

	// Private copy constructor to prevent multiple instantiation of this class
	TypesDB(const TypesDB &other);

	// Private copy operator to prevent multiple instantiation of this class
	TypesDB& operator=(const TypesDB &other);

	~TypesDB();

	// The map between names and type information
	QMap<QString, const RegisteredTypeInfo*> m_typesMap;

	// The map between names and creators. This only contains classes that
	// can actually be instantiated
	QMap<QString, ComponentCreator*> m_creators;

	// The map between type names and their children classes name
	QMap<QString, QStringList> m_childrenMap;

	// The map between names of types and their editors
	QMap<QString, ConfigurationWidgetCreator*> m_editorsMap;

	// The description of registered types
	std::auto_ptr<ConfigurationManager> m_typeDescriptions;

	// The map with all registered descriptors for types
	QMap<QString, RegisteredComponentDescriptor*> m_typeDescriptors;
};

} // end namespace farsa

// Implementation of template functions
#include "configurationexceptions.h"
#include "componentdescriptors.h"

namespace farsa {

// This namespace contains helper code. We put it in an inner namespace to avoid polluting the farsa namespace
namespace __Factory_internal {
	// this local template check if a class is Abstract
	// it's is needed because to register a complete hierarchy it's necessary
	// also to check if a class is abstract so that it is possible to register also
	// abstract classes
	template<class T>
	struct checkClass {
		// Inspired by boost/type_traits/is_abstract.hpp
		// Deduction fails if T is void, function type,
		// reference type (14.8.2/2)or an abstract class type
		// according to review status issue #337
		template<class U>
		static char check_sig(U (*)[1]);
		template<class U>
		static short check_sig(...);
		static const bool isAbstract = (sizeof(check_sig<T>(0)) != sizeof(char));
		// corresponds to !isAbstract
		static const bool canBeCreated = (sizeof(check_sig<T>(0)) == sizeof(char));
	};

	// Checks if the given class inherits from farsa::Component
	template<class T>
	struct isComponent {
		static char check_convert(...);
		static short check_convert(farsa::Component*);
		static const bool value = (sizeof(check_convert((T*) 0)) == sizeof(short));
	};

	template <class T>
	bool isInterface()
	{
		return !isComponent<T>::value;
	}

	template <class T>
	bool callConfiguresInConstructorIfComponentSubclass(...)
	{
		// This aborts if it is called when dealing with a class that is not a component
		abort();

		return false;
	}

	template <class T>
	bool callConfiguresInConstructorIfComponentSubclass(farsa::Component*)
	{
		return T::configuresInConstructor();
	}

	template <class T>
	bool configuresInConstructor()
	{
		return callConfiguresInConstructorIfComponentSubclass<T>((T*) 0);
	}

	template <bool b>
	class Bool2Type
	{
	};

	template <class T>
	std::auto_ptr<ComponentCreator> createGenericCreator(Bool2Type<true>)
	{
		return std::auto_ptr<ComponentCreator>(new ComponentCreatorT<T>());
	}

	template <class T>
	std::auto_ptr<ComponentCreator> createGenericCreator(Bool2Type<false>)
	{
		abort();

		return std::auto_ptr<ComponentCreator>();
	}

	template <class T>
	std::auto_ptr<ComponentCreator> createGenericCreator()
	{
		return createGenericCreator<T>(Bool2Type<isComponent<T>::value && checkClass<T>::canBeCreated>());
	}
}


template <class T>
Component* ComponentCreatorT<T>::instantiate(ConfigurationManager& settings, QString, ResourceAccessor*)
{
	return new T(settings);
}

template <class T>
ConfigurationWidget* ConfigurationWidgetCreatorT<T>::create(ConfigurationManager& params, QString prefix, QWidget* parent, Qt::WindowFlags f) const
{
	// Addind a group separator at the end of the prefix
	const QString terminatedPrefix = prefix + GroupSeparator;

	// Creating the new editor
	std::auto_ptr<T> t(new T(params, terminatedPrefix, parent, f));

	return t.release();
}

template <class NewClass>
void TypesDB::registerType(QString className, QStringList parents)
{
	std::auto_ptr<const RegisteredTypeInfo> info(generateInfoForType<NewClass>(className, parents));
	std::auto_ptr<ComponentCreator> creator = info->canBeCreated ? __Factory_internal::createGenericCreator<NewClass>() : std::auto_ptr<ComponentCreator>();

	registerType<NewClass>(info, creator);
}

template <class NewClass, class ObjectCreator>
void TypesDB::registerType(QString className, QStringList parents)
{
	std::auto_ptr<const RegisteredTypeInfo> info = generateInfoForType<NewClass>(className, parents);
	if (!info->canBeCreated) {
		throw ClassNameIsAbstractException(className.toLatin1().data());
	}
	std::auto_ptr<ComponentCreator> creator(new ObjectCreator());

	registerType<NewClass>(info, creator);
}

template <class NewClass>
void TypesDB::registerType(QString className, QStringList parents, ComponentCreator* creator)
{
	std::auto_ptr<const RegisteredTypeInfo> info = generateInfoForType<NewClass>(className, parents);
	if (!info->canBeCreated && (creator != NULL)) {
		throw ClassNameIsAbstractException(className.toLatin1().data());
	}
	std::auto_ptr<ComponentCreator> creatorPtr(creator);

	registerType<NewClass>(info, creatorPtr);
}

template <class EditorType>
void TypesDB::registerEditorForType(QString type)
{
	// Removing the old creator, if it existed
	if (m_editorsMap.contains(type)) {
		delete m_editorsMap[type];

		m_editorsMap[type] = NULL;
	}

	// Creating the new creator
	m_editorsMap[type] = new ConfigurationWidgetCreatorT<EditorType>();
}
template <class T>
std::auto_ptr<const RegisteredTypeInfo> TypesDB::generateInfoForType(const QString& name, const QStringList& parents)
{
	const bool canBeCreated = __Factory_internal::isComponent<T>::value && __Factory_internal::checkClass<T>::canBeCreated;
	const bool isInterface = __Factory_internal::isInterface<T>();
	const bool configuresInConstructor = (canBeCreated || !isInterface) ? __Factory_internal::configuresInConstructor<T>() : false;

	return std::auto_ptr<const RegisteredTypeInfo>(new RegisteredTypeInfo(name, parents, canBeCreated, isInterface, configuresInConstructor));
}

template <class NewClass>
void TypesDB::registerType(std::auto_ptr<const RegisteredTypeInfo> info, std::auto_ptr<ComponentCreator> creator)
{
	checkParentsRegistered(info.get());

	if (info->canBeCreated || !info->isInterface) {
		componentRegistrationChecks(info.get());
	}

	checkReRegistrationIsPossible(info.get());

	// Deleting the old creator if present
	if (m_creators.contains(info->name)) {
		delete m_creators[info->name];
	}

	// Doing the actual registration
	const RegisteredTypeInfo* pInfo = info.release();
	m_typesMap.insert(pInfo->name, pInfo);
	foreach (QString parent, pInfo->parents) {
		m_childrenMap[parent].append(pInfo->name);
	}
	if (pInfo->canBeCreated) {
		m_creators[pInfo->name] = creator.get();
		creator->m_typeInfo = m_typesMap[pInfo->name];

		// Releasing the pointer to the creator from auto_ptr
		creator.release();
	}
	if (pInfo->canBeCreated || !pInfo->isInterface) {
		describeComponent<NewClass>(pInfo->name);
	}
}

template <class T>
void TypesDB::describeComponent(QString className)
{
	callDescribeIfComponentSubclass<T>(className, (T*) 0);
}

template <class T>
void TypesDB::callDescribeIfComponentSubclass(QString, ...)
{
	// This aborts if it is called when dealing with a class that is not a component
	abort();
}

template <class T>
void TypesDB::callDescribeIfComponentSubclass(QString className, Component*)
{
	T::describe(editableTypeDescriptor(className));
}

} // end namespace farsa

#endif
