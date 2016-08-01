/********************************************************************************
 *  SALSA - Total99                                                             *
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

#include "optionparser.h"
#include "projectmanager.h"
#include "total99resources.h"
#include "component.h"
#include "configurationhelper.h"
#include "logger.h"
#include "baseexception.h"
#include <QCoreApplication>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
#include <QMetaMethod>
#include <QDebug>

using namespace salsa;

#ifdef SALSA_WIN
//--- the directory where the .DLL will be installed is added to the search paths
//#include <windows.h>
// TODO: da testare la creazione del path e decidere come passare CMAKE_INSTALL_PREFIX qui !!
//SetDllDirectory( PATH_TO_LIB );
//SetDllDirectory( getenv( SALSA_DIR )/lib... )

//--- to insert inside the .exe the icon and other resource add to the source of Total99 target
//    also the resource file .rc
//
//    ADD_EXECUTABLE( Target $SOURCES win32_resources.rc )
//
//   http://msdn.microsoft.com/en-us/library/7zxb70x7%28v=vs.80%29.aspx
//   http://msdn.microsoft.com/en-us/library/aa271953%28v=vs.60%29.aspx
#endif

// print the help for the command line
void printCommandLineHelp() {
	qDebug() << "Total99 command line usage (all the options are valid only if --batch is specified):";
	qDebug() << "total99 --batch --file=projectFile --action=actionToRun";
	qDebug() << "  --batch\t\t\tActivate the batch modality. If you omit it, total99 will run in graphic mode";
	qDebug() << "  --file=projectFile\t\tLoad the configuration of the experiment to run from projectFile";
	qDebug() << "  --action=actionToRun\t\tStart the actionToRun in batch";
	qDebug() << "  -P<full/Path/Parameter>=<value>\t\tset the value of Parameter overriding any previous values";
}

int main( int argc, char* argv[] ) {
	bool batch = false;
	QString wdir;
	QString fileToLoad;
	QString actionToRun;
	QStringList paramValues;
	OptionParser opt( argc, argv );
	opt.addSwitch( "batch", &batch );
	opt.addOptionalOption( "file", &fileToLoad, QString("project.ini") );
	opt.addOptionalOption( "action", &actionToRun, QString() );
	opt.addRepeatableOption( 'P', &paramValues );
	opt.parse();

	if ( !batch ) {
		QApplication a( argc, argv );
		a.setOrganizationName( "SALSA" );
		a.setApplicationName( "Total99" );
		Total99Resources::initialize();
		ProjectManager* manager = new ProjectManager();
		manager->setWindowTitle( QString( "Total99 - a SALSA application" ) );
		manager->show();
		QObject::connect( &a, SIGNAL(aboutToQuit()), manager, SLOT(onQuit()) );
		return a.exec();
	} else {
		salsa::Logger::error("BATCH EXECUTION NOT IMPLEMENTED FOR THE MOMENT");
#warning BISOGNA PRIMA FARE DEI CAMBIAMENTI A ComponentUI, VEDI WARNING IN baseexperiment.h
// 		try {
// 			//--- check the command line parameters passed in batch modality
// 			QFileInfo projectFile( fileToLoad );
// 			if ( !projectFile.exists() ) {
// 				qDebug() << "The project file" << fileToLoad << "does not exists";
// 				printCommandLineHelp();
// 				return 1;
// 			}
// 			if ( actionToRun.isEmpty() ) {
// 				qDebug() << "You must specify the action to run in batch modality";
// 				printCommandLineHelp();
// 				return 1;
// 			}
// 			//--- change the current working directory
// 			QDir::setCurrent( projectFile.absolutePath() );
// 			// Here we instantiate a QCoreApplication without GUI
// 			QCoreApplication a( argc, argv );
// 			a.setOrganizationName( "SALSA" );
// 			a.setApplicationName( "Total99" );
// 			Total99Resources::initialize();
// 			ConfigurationManager projectConf;
// 			if ( ! projectConf.loadParameters( fileToLoad ) ) {
// 				qDebug() << "Error loading Project" << fileToLoad;
// 				return 1;
// 			}
// 			//--- override the parameters if one ore more -P options has been specified
// 			foreach( QString str, paramValues ) {
// 				QStringList paramValue = str.split('=');
// 				//--- extract the param name
// 				QString param = paramValue[0].section( '/', -1 );
// 				//--- extract the path
// 				QString groupPath = paramValue[0].section( '/', 0, -2 );
// 				QString value = paramValue[1];
// 				projectConf.createParameter( groupPath, param, value );
// 			}
// 			//--- load any plugin found into the directories spcified by the pluginPath parameter
// 			Total99Resources::loadPlugins( projectConf );
// 			//--- load the configuration
// 			//--- check TOTAL99 group and mandatory parameters
// 			if ( !projectConf.groupExists("TOTAL99") ) {
// 				qDebug() << "Error loading Project" << fileToLoad << ": the TOTAL99 group is not present into the project file. This group is mandatory.";
// 				return 1;
// 			}
// 			QString componentGroup = projectConf.getValue( "TOTAL99/mainComponent" );
// 			if ( componentGroup.isEmpty() ) {
// 				qDebug() << "Error loading Project" << fileToLoad << ": the parameter TOTAL99/mainComponent is not present into the project file. This parameter is mandatory.";
// 				return 1;
// 			}
// 			if ( ! projectConf.groupExists(componentGroup) ) {
// 				qDebug() << "Error loading Project" << fileToLoad << ": the parameter TOTAL99/mainComponent specify a group that it is not present into the project file.";
// 				return 1;
// 			}
// 			//--- create an internal parameter for informing that it is running in batch
// 			projectConf.createSubGroup("", "__INTERNAL__");
// 			projectConf.createParameter("__INTERNAL__", "BatchRunning", "true");
// 			// Setting the log level
// 			Logger::setLogLevel(Logger::stringToLogLevel(ConfigurationHelper::getString(projectConf, "TOTAL99/logLevel", Logger::logLevelToString(Logger::Quiet))));
// 			//--- create the component and configure it
// 			Component* component = projectConf.getComponentFromGroup<Component>( componentGroup );
// 			//--- launch the action to run
// 			const QMetaObject* metaComponent = component->metaObject();
// 			QString sigMethod( "%1()" );
// 			int idMethod = metaComponent->indexOfMethod( sigMethod.arg(actionToRun).toLatin1().data() );
// 			if ( idMethod < 0 ) {
// 				qDebug() << "Error Starting Batch Modality: the action" << actionToRun << "is not one of the actions of the component" << projectConf.getValue( componentGroup+"/type" );
// 			}
// 			metaComponent->method( idMethod ).invoke( component );
// 		} catch ( const std::exception &e ) {
// 			Logger::error( QString("Exception thrown during configuration or execution. Reason: ") + e.what() );
// 			return 1;
// 		}
	}

	return 0;
}

