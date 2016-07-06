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

#include "total99resources.h"
#include <cstdlib>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QStringList>
#include <QRegExp>
#include <QFileInfo>
#include <QSet>
#include <QStringList>
#include "typesdb.h"
#include "farsaplugin.h"
#include "logger.h"
#include "dependencysorter.h"

// We need this to set the DLL search path on Windows
#ifdef FARSA_WIN
	#include "Windows.h"
#endif

// All the suff below is to avoid warnings on Windows about the use of the
// unsafe function getenv
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace farsa {

QString Total99Resources::findResource(QString resourceName)
{
	// First searching in the user directory...
	if (QFile::exists(confUserPath + "/" + resourceName)) {
		return confUserPath + "/" + resourceName;
	}

	// ...then searching in the template user directory...
	if (QFile::exists(confUserPath + "/templates/" + uiTemplate + "/" + resourceName)) {
		return confUserPath + "/templates/" + uiTemplate + "/" + resourceName;
	}

	// ... then searching in the global directory...
	if (QFile::exists(confBasePath + "/" + resourceName)) {
		return confBasePath + "/" + resourceName;
	}

	// ... and finally searching in the global template directory
	if (QFile::exists(confBasePath + "/templates/" + uiTemplate + "/" + resourceName)) {
		return confBasePath + "/templates/" + uiTemplate + "/" + resourceName;
	}

	// If we get here we could not find anything
	return QString();
}

QString Total99Resources::confBasePath;
QString Total99Resources::confUserPath;
QString Total99Resources::pluginBasePath;
QString Total99Resources::pluginConfigBasePath;
QString Total99Resources::uiTemplate;
QString Total99Resources::pluginSuffix;

bool Total99Resources::loadPlugin(QString filename)
{
#ifdef FARSA_WIN
	// On Windows, we first have to change the DLL search path to include the directory with plugins
	SetDllDirectory(pluginBasePath.toLatin1().data());
#endif

	// Loading the plugin
	QString errorString;
	FarsaPlugin* plugin = loadSinglePlugin(filename, &errorString);

	if (plugin == NULL) {
		// Error loading the plugin
		Logger::error("ERROR LOADING PLUGIN: " + errorString);
		return false;
	}
	Logger::info("Loaded plugin \"" + filename + "\"");

	// The object to sort plugin by dependency
	DependencySorter<QString> pluginDependencies;

	// Getting plugin dependencies. We store loaded plugin in a map whose key is the plugin name (we also use this
	// to avoid loading the same plugin twice) and we keep a list of dependencies yet to load. Here it is not
	// important the loading order (while the registration order is, see below).
	QStringList dependenciesToLoad = plugin->getDependencies();
	QMap<QString, FarsaPlugin*> loadedDependencies;
	while (!dependenciesToLoad.empty()) {
		// Taking the first dependency
		QString curDep = dependenciesToLoad.takeFirst();

		// If the plugin has already been loaded, skipping it
		if (loadedDependencies.contains(curDep)) {
			continue;
		}

		// Loading the plugin
		loadedDependencies[curDep] = loadSinglePlugin(curDep, &errorString);
		if (loadedDependencies[curDep] == NULL) {
			// Error loading the plugin
			Logger::error("ERROR LOADING PLUGIN: " + errorString);
			return false;
		}
		Logger::info("Loaded plugin \"" + curDep + "\"");

		// Adding the dependencies of the plugin we have just loaded
		QStringList newDependencies = loadedDependencies[curDep]->getDependencies();
		pluginDependencies.add(curDep, newDependencies);
		foreach (QString d, newDependencies) {
			dependenciesToLoad.append(d);
		}
	}

	// Now that we have loaded all plugins, we need to sort the dependencies, so to call registration in the
	// correct order
	try {
		QStringList sortedDependencies = pluginDependencies.sort();
		foreach (QString d, sortedDependencies) {
			loadedDependencies[d]->registerTypes();
		}
	} catch (CircularDependencyException &) {
		Logger::error("ERROR LOADING PLUGIN: circular dependency found");
		return false;
	}

	// We can finally register the plugin we loaded first
	plugin->registerTypes();

#ifdef FARSA_WIN
	// Reverting to the default behaviour
	SetDllDirectory(NULL);
#endif

	return true;
}

void Total99Resources::loadPlugins(QDir dir)
{
	foreach (QString pluginfile, dir.entryList()) {
		if (!QLibrary::isLibrary(pluginfile)) {
			continue;
		}

		// Building a regular expression to actually load only files containing farsaPlugin
		QRegExp r(".*farsaPlugin.*");
		if (r.indexIn(pluginfile) == -1) {
			continue;
		}
		if (loadPlugin(dir.absoluteFilePath(pluginfile))) {
			Logger::info( "Loaded Plugin \"" + pluginfile + "\"");
		}
	}
}

void Total99Resources::loadPlugins(const farsa::ConfigurationManager& params)
{
	// First loading plugins in the pluginFile:X parameters
	QStringList plugins = params.getParametersWithPrefixList("TOTAL99", "pluginFile");
	foreach (QString param, plugins) {
		QString value = params.getValue("TOTAL99/" + param);
		if (!QFileInfo(value).isFile()) {
			// Trying to see if the plugin is in the global plugin directory
			value = pluginBasePath + "/" + value + "_farsaPlugin" + pluginSuffix;
			if (!QFileInfo(value).isFile()) {
				Logger::warning( "Ignoring un-existing plugin \"" + params.getValue("TOTAL99/" + param) + "\"" );
				continue;
			}
		}
		loadPlugin(value);
	}

	// Then loading all pugins from the pluginPath:X parameters
	QStringList pluginPaths = params.getParametersWithPrefixList( "TOTAL99", "pluginPath" );
	foreach( QString param, pluginPaths ) {
		QString value = params.getValue( "TOTAL99/"+param );
		if ( ! QFileInfo( value ).isDir() ) {
			Logger::warning( "Ignoring un-existing plugin path \"" + value + "\"" );
			continue;
		}
		loadPlugins( QDir( value ) );
	}
}

void Total99Resources::initialize()
{
#ifdef FARSA_WIN
	confBasePath = qApp->applicationDirPath() + "/../conf";
	pluginConfigBasePath = qApp->applicationDirPath() + "/../plugins";
#else
	confBasePath = qApp->applicationDirPath() + "/../share/farsa/conf";
	pluginConfigBasePath = qApp->applicationDirPath() + "/../share/farsa/plugins";
#endif

	pluginBasePath = qApp->applicationDirPath() + "/../lib/farsa/plugins";
#ifdef FARSA_LINUX
	confUserPath = QString(getenv("HOME")) + "/.farsa/total99";
	pluginSuffix = ".so";
#endif

#ifdef FARSA_MAC
	confUserPath = QString(getenv("HOME")) + "/Library/Application Support/farsa/total99";
	pluginSuffix = ".dylib";
#endif

#ifdef FARSA_WIN
	confUserPath = QString(getenv("APPDATA")) + "/farsa/total99";
#ifdef FARSA_DEBUG
	pluginSuffix = "d.dll";
#else
	pluginSuffix = ".dll";
#endif
#endif

	QDir dir;
	dir.mkpath(confUserPath);

	uiTemplate = "kids";
	Logger::info("Total99 Resources Initialized");
}

FarsaPlugin* Total99Resources::loadSinglePlugin(QString filename, QString* errorString)
{
	if (!QFileInfo(filename).isFile()) {
		filename = pluginBasePath + "/" + filename + "_farsaPlugin" + pluginSuffix;
	}

	// Trying to load the plugin
	QPluginLoader loader(filename);
	FarsaPlugin* plugin = qobject_cast<FarsaPlugin*>(loader.instance());
	if ((plugin == NULL) && (errorString != NULL)) {
		*errorString = "Error trying to load \"" + filename + "\", reason: " + loader.errorString();
	}

	return plugin;
}

}

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

