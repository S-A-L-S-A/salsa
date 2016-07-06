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

#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include "configurationconfig.h"
#include <QMap>
#include <QString>
#include <QRegExp>
#include <QStringList>
#include <QList>
#include <memory>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <QMutex>
#include "parametersfileloadersaver.h"
#include "configurationnode.h"

namespace farsa {

class Component;
class ConfigurationObserver;
class ResourceHandler;
class ResourceChangeNotifee;

/**
 * \brief The class containing configuration parameters.
 *
 * This class can be used to read configuration parameters from a file.
 * Parameters have a tree-like organization. They are divided into groups which
 * can have sub-groups. The main group (the one containing all the others) can
 * be referenced using "" (i.e. the empty string). Moreover values are always
 * string (classes using configuration parameters are responsible of converting
 * to the actual type - one exception is object reference, see below). Property
 * names and group names are always case sensitive. Properties and group names
 * are generally lexicographically sorted in ascending order when returned by
 * methods of this class. There is one exception, though. If a parameter name
 * or group name ends with ":" followed by a valid unsigned integer number, the
 * number is considered as a number when sorting and matching. For example if
 * two parameters are "param:3" and "param:12", in a list "param:3" will come
 * before "param:12" even if the lexicographic order is the reverse one.
 * Moreover "param:3" and "param:03" are considered to be exactly the same name
 * of a group or parameter. If however the part following ":" in the name is not
 * a valid unsigned integer the normal lexicographic ordering is used. When
 * there are parameters with a number after ":" and others that do not have
 * a valid number, the ones with number will come first (e.g. "param:3" will
 * come before "param:1a"). Of course this rule applies only if the part of the
 * name before the colon is the same for the string being compared. The function
 * ConfigurationHelper::orderByNumberAfterColon() can be used to apply this
 * sorting rule when performing external sorts (see function description for
 * more information)
 *
 * The special character '/' is used to divide groups and subgroups names into
 * path specifications. For example, "group1/subgroupA/paramName" specifies a
 * paramName contained into subgroupA of group1. Methods like getValue,
 * setValue, createGroup require a full path. You are not allowed to use '/'
 * into a name of parameter or group because it can generate confusion and
 * unexpected results in parsing groups and parameters path. Another special
 * sequence is "..". When used as the name of a group means "the parent group"
 * (just like in filesystems ".." denotes the parent directory). For example,
 * "group1/subgroupA/../paramName" indicates that paramName is inside group1:
 * more explicitly it says that paramName is inside the parent group of
 * subgroupA, that is group1. The parent group of the main group is the main
 * group itself, so, for example, "group1/../../../../paramName" indicates that
 * paramName is in the main group.
 *
 * This class is also a factory to instantiate objects of subclases of
 * Component. For each group you can specify a "type" parameter that will be
 * used as the type of the object to instantiate from that group. The type must
 * be registered with TypesDB to be used, either as a component or as an
 * interface. During registration of the type, a name must be associated to it
 * and that name can be used as the value of the type parameter (for more
 * information on type registration see the TypesDB class description and the
 * documentation on how to create a plugin). When the object is created, the
 * ConfigurationManager object and the prefix for its group will be passed to
 * its constructor to enable it to read other parameters. So, if TypeA is a
 * subclass of Component registered in the factory with the name "TypeA", you
 * could write a configuration file like the following one:
 *
 * \code
 * [somePrefix/A]
 * type = TypeA
 * param1 = 10
 * param2 = A String
 * \endcode
 *
 * and then instantiate an object from that group using the
 * getComponentFromGroup() function like this (suppose params is an instance of
 * ConfigurationManager that has loaded the configuration file above):
 *
 * \code
 * 	TypeA* obj = params.getComponentFromGroup<TypeA>("somePrefix/A");
 * \endcode
 *
 * The getComponentFromGroup() function creates an instance of class TypeA
 * passing the instance of the ConfigurationManager and the prefix of the group
 * to the constructor. The object can then read configuration parameters (either
 * in the constructor or in the configure() function, see the documentation of
 * Component for more information). For example, if TypeA configures itself in
 * the configure() function, that function could look like the following one:
 *
 * \code
 * void TypeA::configure()
 * {
 * 	int param1 = ConfigurationHelper::getInt(configurationManager(), prefixPath() + "param1", 0);
 * 	QString param2 = ConfigurationHelper::getString(configurationManager(), prefixPath() + "param2", "Default Value");
 *
 * 	...
 * }
 * \endcode
 *
 * In the example we have used functions from ConfigurationHelper to handle
 * conversion to the requested types and to specify default values (see the
 * description of that class for more information).
 *
 * The getComponentFromGroup() function has a boolean parameter to specify
 * whether a fully configured object is required or not. If the parameter is
 * true, the returned object is guaranteed to be configured, otherwise it could
 * be returned before configuration has been performed (depending on the type of
 * object). When a non-configured object is returned, it is actually configured
 * after the one who requested it has ended configuring itself. The possibility
 * to request non-configured objects is useful in case of cyclic dependencies
 * among objects. If ObjectA needs a pointer to ObjectB and vice-versa, it is
 * not possible for both of them to have a pointer to a fully configured object
 * during their configuration phase (because this would lead to an infinite
 * loop). In this case one (or both) of them has to content with a pointer to a
 * non-configured object (in this case there is no loop). If a loop is generated
 * when configuring one object, an exception is thrown and object creation is
 * aborted. Of course the possibility to have a pointer to a non-configured
 * object is only available if the object configures itself in the configure
 * function: if the object configures itself in the constructor, it can only be
 * created AND configured at the same time. If this is the case, the parameter
 * of getComponentFromGroup() specifying whether a fully configured object is
 * required or not, is ignored.
 *
 * The creation of an object using getComponentFromGroup() will generally cause
 * the recursive creation of other objects, because when object are configured,
 * they require other objects (think for example of an experiment: the genetic
 * algorithm requires the creation of the fitness function which in turn has to
 * create agents which can be made up of the physical structure, the controller,
 * sensors, motors and so on). Here is an example of this situation. Suppose you
 * have the following configuration file:
 *
 * \code
 * [somePrefix/A]
 * type = typeA
 * param1 = value1
 * param2 = value2
 * anObjectINeed = someOtherPrefix/B
 *
 * [someOtherPrefix/B]
 * type = typeB
 * param1 = value1
 * \endcode
 *
 * The configure() function of object A could look like this:
 *
 * \code
 * void configure(const ConfigurationManager& params, QString prefix)
 * {
 * 	// ... Read other parameters ...
 *
 * 	m_otherObj = params.getComponentFromParameter<typeB>(prefix + "anObjectINeed", false);
 *
 * 	// ... Do other stuffs ...
 * }
 * \endcode
 *
 * (The getComponentFromParameter() is a wrapper to getComponentFromGroup()
 * which simply reads the name of the group for the object to create from a
 * parameter.) In this situation during the call to getComponentFromParameter()
 * the somePrefix/A/anObjectINeed parameter is read and its value is used to get
 * to the someOtherPrefix/B group, from which an object is instantiated. If
 * however an object has already been created using that group, its instance is
 * returned. In fact, every time an object is created, it is associated with the
 * group from which it was created and this association lasts until the object
 * is destroyed. Subsequent calls to getComponentFromGroup() will return the
 * same instance. This also means that objects created via calls to
 * getComponentFromGroup() will have a parent-child relationship which maps
 * one-to-one with groups and subgroups. You can browse the hierarchy of objects
 * traversing groups in the configuration tree and asking for objects associated
 * to groups. If you really need to create a new object from a group without
 * destroying the one that has already been created, you must first create a
 * copy of that group and then use getComponentFromGroup() on the new group.
 *
 * Every Component has a special function named postConfigureInitialization().
 * This function is called at the end of the configuration process, that is when
 * the outermost call to getComponentFromGroup() returns. As explained above
 * there can be recursive calls to getComponentFromGroup() because an object can
 * request another object during the configuration phase. The
 * postConfigureInitialization() is called when all objects have been created
 * and configured. You can use that function to perform operations that require
 * all objects created by a single call to getComponentFromGroup() to exist and
 * be configured.
 *
 * It is possible to destry all components that have been created using
 * ConfigurationManager by calling either destroyAllComponents() or clearAll()
 * (the latter also removes all groups and parameters). Components are never
 * automatically destroyed, neither when the ConfigurationManager object is
 * destroyed: each component, in fact, has its own instance of
 * ConfigurationManager, so if there is a component there is also an instance of
 * ConfigurationManager. If you want to make sure all Components created by a
 * a ConfigurationManager are destroyed, call one of destroyAllComponents() or
 * clearAll().
 *
 * A ConfigurationManager instance is also responsible to manage resources. All
 * resource-related functions in Component interact with ConfigurationManager in
 * some way. This means that resources are shared only among objects created by
 * the same ConfigurationManager. For more information on resources and related
 * issues, see the documentation for the Component class.
 *
 * All functions in this class are thread-safe. This means that is it perfectly
 * safe to share the same ConfigurationManager object between objects in
 * different threads. Refer to the documentation of the Component class for more
 * information on multithreading and ConfigurationManager (in particular
 * regarding resources)
 *
 * \note All functions throw exceptions in case of errors (e.g. missing
 *       parameters or groups, invalid names...). See the description of
 *       individual functions for exceptions
 * \warning If you have classes that are derived from Component, use
 *          ALWAYS functions of this class to create and configure them. In
 *          particular NEVER call configure() or postConfigureInitialization()
 *          explicitly, otherwise you could experience strange problems
 * \warning All instances of ConfigurationManager created via the copy
 *          constructor or copy operator from another instance of
 *          ConfigurationManager share the same set of resources and more in
 *          general the same data structures
 *
 * \ingroup configuration_configuration
 */
class FARSA_CONF_API ConfigurationManager
{
public:
	/**
	 * \brief The function to register file formats to use when loading or
	 *        saving parameters from/to file
	 *
	 * \param format the name of the format. This will be the string with
	 *               which the format will be referred
	 * \param fileLoaderSaver the object to use to load/save files with the
	 *                        current format.
	 * \param defaultExtension the default extension for the current file
	 *                         format. If this is the empty string, the
	 *                         extension will be the same as the format name
	 *
	 * \return true if registration was successful, false otherwise
	 */
	static bool registerFileFormat(QString format, ParametersFileLoaderSaver *fileLoaderSaver, QString defaultExtension);

private:
	// The structure keeping informations about a file format. When building
	// an object using the copy constructor, the new object takes
	// responsability of inner classes destruction if the copied object had
	// it
	struct FileFormat
	{
		FileFormat(const QString &f, ParametersFileLoaderSaver *l, const QString &e) :
			formatName(f),
			fileLoaderSaver(l),
			extension(e),
			m_destructionResponsability(true)
		{
		}

		FileFormat(const FileFormat& other) :
			formatName(other.formatName),
			fileLoaderSaver(other.fileLoaderSaver),
			extension(other.extension),
			m_destructionResponsability(other.m_destructionResponsability)
		{
			// Releasing copied object from destruction
			// responsability
			other.m_destructionResponsability = false;
		}

		FileFormat& operator=(const FileFormat &other)
		{
			if (&other == this) {
				return *this;
			}

			formatName = other.formatName;
			fileLoaderSaver = other.fileLoaderSaver;
			extension = other.extension;
			m_destructionResponsability = other.m_destructionResponsability;

			// Releasing copied object from destruction
			// responsability
			other.m_destructionResponsability = false;

			return *this;
		}

		~FileFormat()
		{
			if (m_destructionResponsability) {
				delete fileLoaderSaver;
			}
		}

		QString formatName;
		ParametersFileLoaderSaver *fileLoaderSaver;
		QString extension;

	private:
		// If true we have to destroy loader and saver in destructor.
		// This is mutable because it is changed in copy constructor
		mutable bool m_destructionResponsability;
	};

	// Returns the map of string format name and format informations.
	//
	// Here we use an accessor function instead of a static data member
	// because static data members do not have a guaranteed order of
	// creation, so we could be executing the initUtilitiesLib function
	// before the constructor for the map has been executed. If, however, we
	// use an accessor function having a static data member, we are
	// guaranteed the object is created on the first function execution
	static QMap<QString, FileFormat>& getFormatsMap();

	// Returns the map of format extensions and format names.
	//
	// Here we use an accessor function instead of a static data member
	// because static data members do not have a guaranteed order of
	// creation, so we could be executing the initUtilitiesLib function
	// before the constructor for the map has been executed. If, however, we
	// use an accessor function having a static data member, we are
	// guaranteed the object is created on the first function execution
	static QMap<QString, QString>& getFileExtensionsMap();

public:
	/**
	 * \brief Constructor
	 */
	ConfigurationManager();

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 * \warning It create a ConfigurationManager that shares data with other
	 */
	ConfigurationManager(const ConfigurationManager &other);

	/**
	 * \brief Assignment operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 * \warning It create a ConfigurationManager that shares data with other
	 */
	ConfigurationManager& operator=(const ConfigurationManager &other);

	/**
	 * \brief Destructor
	 */
	~ConfigurationManager();

	/**
	 * \brief Delete all parameters in all groups and subgroups
	 *
	 * This also destroys all components that have been created, calling
	 * destroyAllComponents()
	 */
	void clearAll();

	/**
	 * \brief Destroys all components that have been created
	 */
	void destroyAllComponents();

	/**
	 * \brief Creates a copy of this object
	 *
	 * The copy will not share data with this. Also all association of
	 * existing objects with groups will not be copied. If dest is not NULL,
	 * the given object is used (first of all, its shared data will be
	 * detached, so that linked ConfigurationManager objects will not be
	 * influenced) and it is also the return value of the function. If it is
	 * NULL, a ConfigurationManager object is allocated using new and is
	 * returned.
	 * \param dest the object that will contain a deep copy of this
	 *             ConfigurationManager. If NULL a node is new-allocated and
	 *             returned
	 * \return the new object
	 * \warning If you supply an existing ConfigurationManager as dest, it
	 *          must not be accessed by another thread when this function is
	 *          called
	 */
	ConfigurationManager* createDeepCopy(ConfigurationManager* dest = NULL);

	/**
	 * \brief Returns the list of sub-groups in the given group
	 *
	 * \param group the group for which the list of sub-groups is requested.
	 *              "" means the main group.
	 * \return the list of names of groups in the given group
	 */
	QStringList getGroupsList(QString group) const;

	/**
	 * \brief Returns the list of sub-groups in the given group whose name
	 *        starts with the provided string
	 *
	 * \param group the group for which the list of sub-groups is requested.
	 *              "" means the main group.
	 * \param prefix the string that must be at the beginning of the names
	 *               of the returned groups
	 * \return the filtered list of names of groups in the given group
	 */
	QStringList getGroupsWithPrefixList(QString group, QString prefix) const;

	/**
	 * \brief Returns the list of sub-groups in the given group whose name
	 *        matches the given regular expression
	 *
	 * \param group the group for which the list of sub-groups is requested.
	 *              "" means the main group.
	 * \param filter the regular expression that must be matched by group
	 *               names
	 * \return the filtered list of names of groups in the given group
	 */
	QStringList getFilteredGroupsList(QString group, QRegExp filter) const;

	/**
	 * \brief Creates a group in the specified path
	 *
	 * If the group already exists, this function does nothing
	 * \param groupPath the full group path to add to ConfigurationManager
	 */
	void createGroup(QString groupPath);

	/**
	 * \brief Creates a sub-group of the specified parent group
	 *
	 * \param parentPath the full path of the parent group to which the
	 *                   sub-group will be added
	 * \param groupName the name of the sub-group
	 * \return the full path of the sub-group created
	 */
	QString createSubGroup(QString parentPath, QString groupName);

	/**
	 * \brief Returns true if the group exists
	 *
	 * \param groupPath the group to check
	 * \return true if the group exists
	 * \note It is possible to have a parameter and a subgroup of the same
	 *       group with identical names. In that case the same path will
	 *       cause both parameterExists() and groupExists() to return true
	 */
	bool groupExists(QString groupPath) const;

	/**
	 * \brief Deletes the last group in the specified path
	 *
	 * Suppose you have the following configuration file (in INI format):
	 *
	 * \code
	 * [GROUP_A]
	 * param1 = value1
	 *
	 * [GROUP_A/GROUP_B]
	 * param2 = value2
	 *
	 * [GROUP_A/GROUP_B/GROUP_C]
	 * param3 = value3
	 * \endcode
	 *
	 * When this methos is called in the following way:
	 *
	 * \code
	 * params.deleteGroup("GROUP_A/GROUP_B");
	 * \endcode
	 *
	 * The result is (in INI format):
	 *
	 * \code
	 * [GROUP_A]
	 * param1 = value1
	 * \endcode
	 *
	 * The GROUP_B and all its parameters and subgroups has been removed
	 * from the ConfigurationManager, while the GROUP_A remains untouched.
	 * \param groupPath the full group path of the group to delete from
	 *                  ConfigurationManager
	 * \note deleteGroup(""), deleteGroup("/") or deleteGroup("..") raise
	 *       an exception. If you want to remove all groups see clearAll()
	 */
	void deleteGroup(QString groupPath);

	/**
	 * \brief Renames the last group in the specified path
	 *
	 * Suppose you have the following configuration file (in INI format):
	 *
	 * \code
	 * [GROUP_A]
	 * param1 = value1
	 *
	 * [GROUP_A/GROUP_B]
	 * param2 = value2
	 *
	 * [GROUP_A/GROUP_B/GROUP_C]
	 * param3 = value3
	 * \endcode
	 *
	 * When this method is called in the following way:
	 *
	 * \code
	 * params.renameGroup("GROUP_A/GROUP_B", "MY_GROUP");
	 * \endcode
	 *
	 * The results are (in INI format):
	 *
	 * \code
	 * [GROUP_A]
	 * param1 = value1
	 *
	 * [GROUP_A/MY_GROUP]
	 * param2 = value2
	 *
	 * [GROUP_A/MY_GROUP/GROUP_C]
	 * param3 = value3
	 * \endcode
	 *
	 * The GROUP_B has been renamed to MY_GROUP, everything else remains
	 * untouched
	 * \param oldGroupPath the full group path of the group to rename
	 * \param newGroupName the new name of the group to rename
	 */
	void renameGroup(QString oldGroupPath, QString newGroupName);

	/**
	 * \brief Copies a group tree to another location
	 *
	 * Suppose you have the following configuration file (in INI format):
	 *
	 * \code
	 * [GROUP_A]
	 * param1 = value1
	 *
	 * [GROUP_A/GROUP_B]
	 * param2 = value2
	 * \endcode
	 *
	 * When this methos is called in the following way:
	 *
	 * \code
	 *
	 * params.copyGroup("GROUP_A", "GROUP_C");
	 *
	 * \endcode
	 *
	 * The result is (in INI format):
	 *
	 * \code
	 * [GROUP_A]
	 * param1 = value1
	 *
	 * [GROUP_A/GROUP_B]
	 * param2 = value2
	 *
	 * [GROUP_C]
	 * param1 = value1
	 *
	 * [GROUP_C/GROUP_B]
	 * param2 = value2
	 * \endcode
	 *
	 * If the parent of the destination group does not exists, it is
	 * created. If the destination group already exists, an error occurs
	 * \param sourceGroup the full path to the group to copy
	 * \param destGroup  the full path of the location of the copy
	 * \note copying the root node is not allowed.
	 */
	void copyGroup(QString sourceGroup, QString destGroup);

	/**
	 * \brief Creates a parameter in the specified group
	 *
	 * This throws an exception if the parameter already exists
	 * \param groupPath the full path to the group where to add the new
	 *                  parameter
	 * \param parameter the name of the parameter
	 * \warning This doesn't automatically create groups
	 */
	void createParameter(QString groupPath, QString parameter);

	/**
	 * \brief Creates a parameter in the specified group and sets its value
	 *
	 * \param groupPath the full path to the group where to add the new
	 *                  parameter
	 * \param parameter the name of the parameter
	 * \param value the value of parameter
	 * \warning It doesn't automatically create groups
	 */
	void createParameter(QString groupPath, QString parameter, QString value);

	/**
	 * \brief Checks whether a parameter exists
	 *
	 * \param path the path to the parameter
	 * \return true if the parameter exists
	 * \note It is possible to have a parameter and a subgroup of the same
	 *       group with identical names. In that case the same path will
	 *       cause both parameterExists() and groupExists() to return true
	 */
	bool parameterExists(QString path) const;

	/**
	 * \brief Deletes a parameter in the specified group path
	 *
	 * \param groupPath the full group path to the group whose parameter to
	 *                  remove
	 * \param parameter the name of the parameter to delete
	 */
	void deleteParameter(QString groupPath, QString parameter);

	/**
	 * \brief Returns the value of a parameter
	 *
	 * \param path the path of the parameter
	 * \return the parameter value
	 */
	QString getValue(QString path) const;

	/**
	 * \brief Returns the value of a parameter, searching the parameter also
	 *        in parent groups
	 *
	 * The parameter is first searched in the given path then, if it is not
	 * found, it is searched back in parent groups until found or the main
	 * group is reached
	 * \param path the path of the parameter
	 * \return the parameter value
	 */
	QString getValueAlsoMatchParents(QString path) const;

	/**
	 * \brief Sets the value of the parameter with the given path
	 *
	 * \param path the path of the parameter
	 * \param value the new value for the parameter
	 * \note if the value is an empty string, setValue will remove the
	 *       parameter
	 */
	void setValue(QString path, QString value);

	/**
	 * \brief Returns the list of parameters in the given group
	 *
	 * \param group the group for which the list of parameters is requested
	 * \return the list of names of parameters in the given group
	 */
	QStringList getParametersList(QString group) const;

	/**
	 * \brief Returns the list of parameters in the given group whose name
	 *        starts with the provided string
	 *
	 * \param group the group for which the list of parameters is requested.
	 *              "" means the main group.
	 * \param prefix the string that must be at the beginning of the names
	 *               of the returned parameters
	 * \return the filtered list of names of parameters in the given group
	 */
	QStringList getParametersWithPrefixList(QString group, QString prefix) const;

	/**
	 * \brief Returns the list of parameters in the given group whose name
	 *        matches the given regular expression
	 *
	 * \param group the group for which the list of parameters is requested.
	 *              "" means the main group.
	 * \param filter the regular expression that must be matched by
	 *               parameters names
	 * \return the filtered list of names of parameters in the given group
	 */
	QStringList getFilteredParametersList(QString group, QRegExp filter) const;

	/**
	 * \brief Returns the object for the given group, creating it if it
	 *        doesn't exist
	 *
	 * See class description for more information on object creation
	 * \param group the group corresponding to the wanted object
	 * \param configure if true the object is returned configured, if false
	 *                  it could be returned configured or not configured
	 * \return the object
	 *
	 * \note This uses the factory to create the object, so a factory
	 *       exception could be thrown (e.g. if group is not found)
	 */
	template <class TypeToCreate>
	TypeToCreate* getComponentFromGroup(QString group, bool configure = true);

	/**
	 * \brief Returns the component for the given parameter, creating it if
	 *        it doesn't exist
	 *
	 * This calls getComponentFromGroup(), so everything said with regard to
	 * that function also applies here
	 * \param param the parameter whose value is the group corresponding to
	 *              the wanted object
	 * \param configure if true the object is returned configured, if false
	 *                  it could be returned non configured
	 * \return the object
	 */
	template <class TypeToCreate>
	TypeToCreate* getComponentFromParameter(QString param, bool configure = true);

	/**
	 * \brief Loads a configuration file
	 *
	 * \param filename the name of the file from which parameters should be
	 *                 loaded
	 * \param keepOld if false old parameters are deleted before loading the
	 *                new ones, otherwise they are kept
	 * \param format the format of the file to load. If "" format is guessed
	 *               from filename extension. If guessing fails, loading
	 *               fails
	 * \return false if loading fails, true otherwise
	 */
	bool loadParameters(QString filename, bool keepOld = false, QString format = "");

	/**
	 * \brief Saves configuration to file
	 *
	 * \param filename the name of the file to which parameters should be
	 *                 saved
	 * \param format the format of the file to save. If "" format is guessed
	 *               from filename extension. If guessing fails, loading
	 *               fails
	 * \return false if saving fails, true otherwise
	 */
	bool saveParameters(QString filename, QString format = "") const;

private:
	// Calls postConfigureInitialization on all objects for the given
	// ConfigurationManager object. This also clears the list
	void callPostConfigureInitializationForConfiguredComponents();

	// Performs regitration of all resource change notifications requests
	// that were stored during a call to getComponentFromGroup(). This also
	// clears the list
	void registerAllStoredNotifications();

	// Returns the format associated with the extension of the given
	// filename
	static QString formatFromFilenameExtension(QString filename);

	void setComponentFromGroupStatusToCreating(QString group, Component *object);
	void setComponentFromGroupStatusToCreatedNotConfigured(QString group, Component *object);
	void setComponentFromGroupStatusToConfiguring(QString group, Component* object);
	void setComponentFromGroupStatusToCreatedAndConfigured(QString group, Component *object);

	// Returns the configuration node for the object being created at the
	// moment. This function is called by Component constructor to set the
	// ConfigurationNode to which it is associated
	ConfigurationNode* getConfigurationNodeForCurrentComponent();

	// Returns the type (string representation) of the object being created
	// at the moment. This function is called by Component constructor to
	// set the string representation of the object that is being created
	QString getTypeForCurrentComponent();

	// Returns the prefix of the group for the object being created at the
	// moment. This function is called by Component constructor to set the
	// prefix of the group from which the object is being created
	QString getPrefixForCurrentComponent();

	// Recursively destroys all components associated to nodes. The
	// components in parent nodes are destroyed first
	void recursivelyDestroyComponents(ConfigurationNode* node);

	// Adds an observer. This is called by ConfigurationObserver
	void addObserver(ConfigurationObserver* observer);

	// Removes an observer. This is called by ConfigurationObserver
	void removeObserver(ConfigurationObserver* observer);

	void notifyObserversOfComponentCreation(Component* component);

	// This is called by component destruction
	void notifyObserversOfComponentDestruction(Component* component);

	// A structure keeping an object and the recursion level in which it was
	// created
	struct ComponentAndRecursionLevel
	{
		ComponentAndRecursionLevel(Component* o, unsigned int r)
			: component(o)
			, recursionLevel(r)
		{
		}

		Component* component;
		unsigned int recursionLevel;
	};

	// A structure to keep requests for notifications of resource change
	// that are performed during the configuration phase (we store them to
	// perform all registration after all components have been created)
	struct RegisteredRequestForResourceChangeNotification
	{
		RegisteredRequestForResourceChangeNotification(Component* n, QString r, Component* o = NULL)
			: notifee(n)
			, resourceName(r)
			, resourceOwner(o)
		{
		}

		bool operator==(const RegisteredRequestForResourceChangeNotification& other) const
		{
			return (notifee == other.notifee) && (resourceName == other.resourceName) && (resourceOwner == other.resourceOwner);
		}

		Component* notifee;
		QString resourceName;
		// This is NULL if no owner was specified during the
		// regustration request
		Component* resourceOwner;
	};

	// The class with data shared among copied instances of
	// ConfigurationManager
	class SharedData : public QSharedData
	{
	public:
		SharedData()
			: QSharedData()
			, mutex(QMutex::Recursive)
			, root()
			, getComponentFromGroupRecursionLevel(0)
			, componentsToConfigure()
			, componentsConfiguredNotInitialized()
			, nodeForComponentBeingCreated(NULL)
			, typeForComponentBeingCreated()
			, prefixForComponentBeingCreated()
			, requestedNotifications()
			, observers()
			, resources()
		{
		}

		// This detaches all observers
		~SharedData();

		// The recursive mutex protecting accesses to shared data
		QMutex mutex;

		// The root of the tree with configuration parameters
		std::auto_ptr<ConfigurationNode> root;

		// The level of recursion for calls to getComponentFromGroup.
		// This is incremented every time the getComponentFromGroup
		// starts its execution and decremented on function exit
		unsigned int getComponentFromGroupRecursionLevel;

		// A stack of objects which have been created but not yet
		// configured
		QList<ComponentAndRecursionLevel> componentsToConfigure;

		// The list of objects for which postConfigureInitialization
		// must be called
		QList<Component*> componentsConfiguredNotInitialized;

		// The node from which the object is being created. We set this
		// before constructing a Component so that its constructor will
		// get it
		ConfigurationNode* nodeForComponentBeingCreated;

		// The type of the group from which the object is being created.
		// We set this before constructing a Component so that its
		// constructor will get it
		QString typeForComponentBeingCreated;

		// The prefix of the group from which the object is being
		// created. We set this before constructing a Component so that
		// its constructor will get it
		QString prefixForComponentBeingCreated;

		// The list of requests for notifications of resource change
		// that are performed during the configuration phase (we store
		// them to perform all registration after all components have
		// been created)
		QList<RegisteredRequestForResourceChangeNotification> requestedNotifications;

		// The list of observers
		QList<ConfigurationObserver*> observers;

		// The map containing all Resources declared by Components
		// created using this ConfigurationManager. The key is the name
		// of the resource, the value is a map with the resource owner
		// as the key. Multiple resources with the same name can coexist
		// as long as their owner is different. The list of resources is
		// here because it is shared, but it is mainly used by Component
		QMap<QString, QMap<Component*, ResourceHandler*> > resources;

	private:
		// Copy constructor. Here to prevent usage
		SharedData(const SharedData& other);

		// Copy operator. Here to prevent usage
		SharedData& operator=(const SharedData& other);
	};

	// Data shared among all copied instances of ConfigurationManager
	QExplicitlySharedDataPointer<SharedData> m_shared;

	// ComponentCreator is friend to be able to call the
	// setObjectFromGroupStatusTo* functions
	friend class ComponentCreator;

	// ResourceAccessor is friend to access resources
	friend class ResourceAccessor;

	// Component is friend to use private methods to get prefix, typename
	// and configuration node and to access resources
	friend class Component;

	// ConfigurationObserver is friend to call the functions to add or
	// remove itself from the list of observers
	friend class ConfigurationObserver;
};

} // end namespace farsa

// Implementation of template functions
#include "typesdb.h"
#include "component.h"
#include "configurationobserver.h"
#include <memory>

namespace farsa {

template <class TypeToCreate>
TypeToCreate* ConfigurationManager::getComponentFromGroup(QString group, bool configure)
{
	// This is a simple class to implement RAII for the getComponentFromGroupRecursionLevel variable
	// and the m_objectsToConfigure list so that they are properly finalized on getComponentFromGroup()
	// exit. Note that to configure objects you have to explicitly call "release", the destructor
	// doesn't call configure on objects. This also resets the value of the variables
	// nodeForComponentBeingCreated, typeForComponentBeingCreated and prefixForComponentBeingCreated
	// at destruction
	class RecursionLevelRAII
	{
	public:
		RecursionLevelRAII(ConfigurationManager &params)
			: m_params(params)
			, m_releaseCalled(false)
		{
			++(m_params.m_shared->getComponentFromGroupRecursionLevel);
		}

		// This doesn't throw because in the release function we don't call configure
		~RecursionLevelRAII() throw()
		{
			release(false);

			// Resetting variables relative to the current node. Hopefully truncate will not throw...
			m_params.m_shared->nodeForComponentBeingCreated = NULL;
			m_params.m_shared->typeForComponentBeingCreated.truncate(0);
			m_params.m_shared->prefixForComponentBeingCreated.truncate(0);
		}

		void release(bool doConfigure = true)
		{
			// This is to avoid calling this function twice (e.g. explicitly and then again in the destructor)
			if (m_releaseCalled) {
				return;
			}

			m_releaseCalled = true;

			while ((!m_params.m_shared->componentsToConfigure.isEmpty()) &&
			       (m_params.m_shared->componentsToConfigure.back().recursionLevel > m_params.m_shared->getComponentFromGroupRecursionLevel)) {
				// Doing this here because the call to configure inside the list may add more elements to the list
				// (and if we remove the last element after the if, we could be removing the wrong thing)
				Component* const component = m_params.m_shared->componentsToConfigure.takeLast().component;
				if (doConfigure) {
					// Calling configure. Classes that do configuration in che constructor are not in the list
					component->configure();
					// The object has been configured, we must call postConfigureInitialization at the end
					m_params.m_shared->componentsConfiguredNotInitialized.append(component);
				}
			}

			(m_params.m_shared->getComponentFromGroupRecursionLevel)--;
		}

	private:
		ConfigurationManager& m_params;
		bool m_releaseCalled;

		// We put implementations to avoid warning under windows. Anyway, these are private, so
		// nobody can call them. The initialization of m_params is to avoid compilation errors
		// under MSVC
		RecursionLevelRAII(const RecursionLevelRAII& /*other*/) : m_params(*((ConfigurationManager *) NULL)) {}
		RecursionLevelRAII& operator=(const RecursionLevelRAII& /*other*/) { return *this; }
	};

	// First of all acquiring a lock on the mutex for shared data
	QMutexLocker locker(&(m_shared->mutex));

	// First of all getting the node for the group
	ConfigurationNode *node = m_shared->root->getNode(group);

	// Getting the type to create
	const QString typeToCreate = node->getValue("type");

	// If the recursion level is 0 (this is the most external call to getComponentFromGroup), we always configure
	// the object. This check is needed because otherwise, if the most external call to this function has
	// the configure parameter set to false, the object returned by that call is never configured
	if (m_shared->getComponentFromGroupRecursionLevel == 0) {
		configure = true;
		// As this is the first call to getComponentFromGroup for the current m_settings, we remove the list
		// of objects, if present (there could be some if the previous set of calls ended with an exception)
		m_shared->componentsConfiguredNotInitialized.clear();
		// We also clear the list of objects to configure, even though this should always be in sync (because
		// objects are removed by RecursionLevelRAII)
		m_shared->componentsToConfigure.clear();
	}

	// Incrementing the recursion level for this function
	RecursionLevelRAII recursionLevelRAII(*this);

	// The pointer to return
	TypeToCreate* retObj = NULL;
	// This is used only if the object is created during this call
	std::auto_ptr<Component> retObjRAII(NULL);

	// Setting properties that will be read by the new component
	m_shared->nodeForComponentBeingCreated = node;
	m_shared->typeForComponentBeingCreated = typeToCreate;
	m_shared->prefixForComponentBeingCreated = group.endsWith(GroupSeparator) ? group : (group + GroupSeparator);

	// Getting the object associated with the node and checking its status
	ComponentAndStatus component = node->getComponentForNode("");
	switch (component.status) {
		case ComponentNotCreated:
			// Checking that the class map contains the class to create
			if (!TypesDB::instance().isTypeRegistered(typeToCreate)) {
				throw ClassNameNotRegisteredException(typeToCreate.toLatin1().data());
			}

			// Creating object. We need to define a block to be able to declare the creator variable inside it
			{
				const RegisteredTypeInfo& info = TypesDB::instance().typeInfo(typeToCreate);
				if (!info.canBeCreated) {
					throw ClassNameIsAbstractException(typeToCreate.toLatin1().data());
				}
				ComponentCreator* const creator = TypesDB::instance().getComponentCreator(typeToCreate);
				retObjRAII.reset(creator->create(*this, group, configure));
				retObj = dynamic_cast<TypeToCreate*>(retObjRAII.get());

				if (retObj == NULL) {
					throw CannotConvertToTypeException(typeToCreate.toLatin1().data(), typeid(TypeToCreate));
				}

				// Notifying observers
				notifyObserversOfComponentCreation(retObjRAII.get());

				// Putting the object in the list of configured ones or not depending on whether it has been
				// configured or not
				if (configure || info.configuresInConstructor) {
					// The object has been configured, adding it to the list of objects to be initialized
					m_shared->componentsConfiguredNotInitialized.append(retObjRAII.get());
				} else {
					// The object hasn't been configured
					m_shared->componentsToConfigure.push_back(ComponentAndRecursionLevel(retObjRAII.get(), m_shared->getComponentFromGroupRecursionLevel));
				}
			}
			break;
		case CreatingComponent:
			// Object is in creation phase, cannot return it
			throw CyclicDependencyException(group.toLatin1().data());
			break;
		case ComponentCreatedNotConfigured:
			// We configure the object if we have to and then return the object
			retObj = dynamic_cast<TypeToCreate *>(component.component);
			if (retObj == NULL) {
				throw CannotConvertToTypeException(typeid(component.component).name(), typeid(TypeToCreate));
			}
			if (configure) {
				node->setComponentForNode("", component.component, ConfiguringComponent);
				component.component->configure();
				// Removing the object from the list of objects to configure
				QList<ComponentAndRecursionLevel>::iterator it;
				for (it = m_shared->componentsToConfigure.begin(); it != m_shared->componentsToConfigure.end(); ++it) {
					if (it->component == component.component) {
						break;
					}
				}
				// Safety check
				if (Q_UNLIKELY(it == m_shared->componentsToConfigure.end())) {
					// Internal error, aborting
					qFatal("INTERNAL ERROR: Cannot find a yet-to-configure object in the list of objects to configure, group: %s", m_shared->prefixForComponentBeingCreated.toLatin1().data());
				}
				m_shared->componentsToConfigure.erase(it);
				// Storing the object as configured
				m_shared->componentsConfiguredNotInitialized.append(component.component);
				node->setComponentForNode("", component.component, ComponentCreatedAndConfigured);
			}
			break;
		case ConfiguringComponent:
			if (configure) {
				// Object is in configuration phase, cannot return it
				throw CyclicDependencyException(group.toLatin1().data());
			} else {
				// Ok, returning object
				retObj = dynamic_cast<TypeToCreate *>(component.component);
				if (retObj == NULL) {
					throw CannotConvertToTypeException(typeid(component.component).name(), typeid(TypeToCreate));
				}
			}
			break;
		case ComponentCreatedAndConfigured:
			// Ok, returning object
			retObj = dynamic_cast<TypeToCreate *>(component.component);
			if (retObj == NULL) {
				throw CannotConvertToTypeException(typeid(component.component).name(), typeid(TypeToCreate));
			}
			break;
	}

	// Releasing resources (explicitly, so that unconfigured objects that can be configured at this recursion level
	// will actually be configured)
	recursionLevelRAII.release();

	// Now checking if we have to call postConfigureInitialization() and register resource change notifications for
	// all configured objects
	if (m_shared->getComponentFromGroupRecursionLevel == 0) {
		callPostConfigureInitializationForConfiguredComponents();

		// Safety check, just to be sure: when we get here, there should be no object in the list of objects yet to configure
		if (Q_UNLIKELY(m_shared->componentsToConfigure.size() != 0)) {
			qFatal( "INTERNAL ERROR: The list of components yet to be configured is not empty at the exit of the most external call to ConfigurationManager::getComponentFromGroup(). The number of componenets still in the list is %d", m_shared->componentsToConfigure.size());
		}

		registerAllStoredNotifications();
	}

#warning SEE THIS COMMENT
	// This function is not completely exception safe: if a configured object is created and then an exception is thrown, the object is not destroyed (unless its parent component uses auto_ptr or similar stuffs). It would be better to make sure all components are destroyed, even when the exception is thrown during postConfigureInitialization or resource registration.

	retObjRAII.release();
	return retObj;
}

template <class TypeToCreate>
TypeToCreate* ConfigurationManager::getComponentFromParameter(QString param, bool configure)
{
	// Getting the value for the parameter and calling getComponentFromGroup
	QString componentGroup = getValue(param);

	// If componentGroup is relative, making absolute
	if (!componentGroup.startsWith(GroupSeparator)) {
		componentGroup = ConfigurationNode::separateLastElement(param).elementPath + GroupSeparator + componentGroup;
	}

	return getComponentFromGroup<TypeToCreate>(componentGroup, configure);
}

} // end namespace farsa

#endif
