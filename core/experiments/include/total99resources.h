/********************************************************************************
 *  FARSA - Total99                                                             *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef TOTAL99RESOURCES_H
#define TOTAL99RESOURCES_H

#include "experimentsconfig.h"
#include <QString>
#include <QMap>
#include <QMetaObject>
#include <QDir>
#include "configurationmanager.h"

class FarsaPlugin;

namespace farsa {

/**
 * \brief This class has static method for retrieving the resources for the
 *        total99 application
 *
 * \section Resources How resources are stored and retrieved
 *
 * \subsection globalResources Global Resources
 *
 * The global resources are stored in the 'conf' directory at the same level
 * where the 'bin' directory with the executable has been installed. The content
 * of the 'conf' directory is the following:
 * - <em>total99.ini</em>: contains the global preferences of the application.
 *                         They are considered the default settings.
 * - <em>templates</em>: is a directory containing the graphical templates for
 *                       the UI. Each template is a directory containing icons,
 *                       fonts and stylesheet for the user interface.
 *
 * \subsection userResources User Resources
 *
 * Any of the global configuration and setting can been override by the user.
 * The executable will look inside a user configuration directory with exactly
 * the same structure of the 'conf' directory. All the parameters setted into
 * the total99.ini inside the user configuration directory will take priority
 * respect to the global configuration. The user configuration directory is
 * located on OS-ware basis:
 * - on Windows: %APPDATA%/FARSA/Total99
 * - on Linux: ~/.FARSA/total99
 * - on Mac OS: ~/Library/Application Support/FARSA/Total99
 *
 * \section Plugins Total99 Plugins
 *
 * This class has also methods for handling the plugins. In particular, the
 * loadPlugin method load the data from a plugin and registers all the new
 * classes defined into the plugin. Plugins are loaded from the directories
 * listed in the TOTAL99/pluginPath parameters plus all plugins in the
 * TOTAL99/plugin parameters. The TOTAL99 group can have different pluginPath:X
 * parameters, with X being numbers. Each parameter is a directory from which
 * plugins are loaded. Non-existing directories are ignored. The TOTAL99 group
 * can also have different plugin:X paramers, with X being numbers. Each
 * parameter is either the full path of a plugin to load or the name of a plugin
 * (without the _farsaPlugin* suffix). In the latter case the plugin is loaded
 * from the system-wide plugin directory, if it exists. These parameter is valid
 * ONLY inside experiments configuration files (they are ignored when found
 * inside the local or global total99.ini file).
 */
class FARSA_EXPERIMENTS_API Total99Resources
{
public:
	/**
	 * \brief Returns the full path of the requested resource (in this case
	 *        a file name)
	 *
	 * It searches in the following order:
	 * - in the user configuration directory
	 * - in the template directory under user configuration directory
	 * - in the global configuration directory
	 * - in the template directory under global directory
	 * \param resourceName the file name of the resource to find (e.g.
	 *                     anIcon.png)
	 */
	static QString findResource(QString resourceName);

	/**
	 * \brief The path to the base (global) configuration directory
	 */
	static QString confBasePath;

	/**
	 * \brief The path to the user configuration directory
	 */
	static QString confUserPath;

	/**
	 * \brief The path to the base (global) plugin directory
	 */
	static QString pluginBasePath;

	/**
	 * \brief The path to the base (global) directory with plugin
	 *        configuration files
	 */
	static QString pluginConfigBasePath;

	/**
	 * \brief The graphical template in use for the user interface
	 */
	static QString uiTemplate;

	/**
	 * \brief The suffix for plugins on the current operating system
	 */
	static QString pluginSuffix;

	/**
	 * \brief Loads a plugin
	 *
	 * This method load the plugin, and once loaded it calls
	 * FarsaPlugin::registerTypes in order to register all types
	 * \param filename is the full path of the plugin
	 * \return true if the plugin has been loaded, false if any error occurs
	 *         during the loading
	 */
	static bool loadPlugin(QString filename);

	/**
	 * \brief Loads all plugins found in the directory
	 *
	 * \param dir the directory where to look for plugins
	 */
	static void loadPlugins(QDir dir);

	/**
	 * \brief Loads plugins from locations specified by configuration
	 *        parameters
	 *
	 * Loads all plugins from all directories listed in the
	 * TOTAL99/pluginPath parameters and all plugins in the TOTAL99/plugin
	 * parameters of the given ConfigurationManager object. See class
	 * description for more information on the format of the parameters
	 * \param params the object with configuration parameters
	 */
	static void loadPlugins(const farsa::ConfigurationManager& params);

	/**
	 * \brief Initializes data
	 */
	static void initialize();

private:
	Total99Resources();
	~Total99Resources();
	// Loads a plugin and returns an instance of the FarsaPlugin class.
	// filename is the name of the plugin to load. If this is an absolute
	// path, the file is loaded, otherwise this is considered as the name of
	// a plugin in the default plugin directory. If errorString is not NULL,
	// it is filled with the error string from the loader in case of error.
	// This function returns the instance of the FarsaPlugin class in the
	// plugin or NULL if an error occurred
	static FarsaPlugin* loadSinglePlugin(QString filename, QString* errorString = NULL);
};

}

#endif
