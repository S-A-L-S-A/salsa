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

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QLabel>
#include <QList>
#include <QTextEdit>
#include <QStatusBar>
#include <QComboBox>
#include <QPair>
#include <QByteArray>
#include <QSignalMapper>
#include "component.h"
#include "configurationmanager.h"
#include "groupstreeeditor.h"
#include "genericgroupeditor.h"
#include "componentui.h"
#include "typesdb.h"
#include "batchinstancesmanager.h"

class QLineEdit;
class QTextEdit;
class ViewersManager;

/*! ProjectManager open/modify/save a particular ConfigurationManager file that generate experimental setups
 *
 *  \section ProjectFile Project File Conventions
 *  The are the following assumptions on the project file:
 *  - a group called "TOTAL99" has to be present; the parameters excepted (in bold the mandatory ones) in this group are:
 *      - <b>name</b> : the name of the experimental setup (the project name)
 *      - author : the name of the author
 *      - date : the date of the last saving (automatically generated during saving)
 *      - description : an author description, comment and/or other informations about the experimental setup
 *      - <b>mainComponent</b> : the name of the group that contains the configuration of the component; VERY IMPORTANT
 *  - any plugin required for loading and configuring the component/experiment are searched into the same directory where
 *    the file has been loaded
 *  - any other resources needed by the ProjectManager are searched into the same directory where the file has been loaded
 *
 *  \section Resources How resources are stored and retrieved
 *  \subsection globalResources Global Resources
 *  The global resources are stored in the 'conf' directory where SALSA has been installed. On linux/mac, the 'conf'
 *  directory is under 'share/SALSA' directory. The content of the 'conf' directory is the following:
 *  - <em>total99.ini</em>: contains the global preferences of the application. They are considered the default settings.
 *  - <em>templates</em>: is a directory containing the graphical templates for the UI. Each template is a directory
 *                        containing icons, fonts and stylesheet for the user interface.
 *  \subsection userResources User Resources
 *  Any of the global configuration and setting can been override by the user. The executable will look inside a user
 *  configuration directory with the same exactly structure of the 'conf' directory. All the parameters setted into the
 *  total99.ini inside the user configuration directory will take priority respect to the global configuration
 *  The user configuration directory is located on OS-ware basis:
 *  - on Windows: %APPDATA%/SALSA/Total99
 *  - on Linux: ~/.SALSA/total99
 *  - on Mac OS: ~/Library/Application Support/SALSA/Total99
 *
 */
class ProjectManager : public QWidget, public salsa::ConfigurationObserver {
	Q_OBJECT
public:
	/*! Constructor */
	ProjectManager( QWidget* parent = 0, Qt::WindowFlags f = 0 );
	/*! Destructor */
	~ProjectManager();
	/*! when an object is created it look for associated "Actions" and "Viewers" */
	void onComponentCreation(salsa::Component* component);
	// This is not used
	void onComponentDestruction(salsa::Component* component);
public slots:
	/*! Save the state of the windows opened, and clean up memory */
	void onQuit();
	/*! Load a project file */
	void loadProject();
	/*! create a New project */
	void newProject();
	/*! Close the current project */
	void closeProject();
	/*! Save a project on the same file (if it is a new project ask for the filename to save in) */
	void saveProject();
	/*! Save a project on a new file */
	void saveAsProject();
	/*! close the component */
	void closeComponent();
	/*! (re-)configure the component loaded using the last parameters configuration
	 *
	 *  \note when called the first time, it will also create the component; when called again with
	 *   a component already created, it will only call configure method of the component
	 */
	void configureComponent();
	/*! Apply the graphic template for the user interface
	 *  \param templateName the name of the template
	 */
	void applyUITemplate( QString templateName );
	/*! Load a plugin */
	void loadPlugin();
signals:
	/*! emitted when the component has been configured (or re-configured) */
	void componentConfigured();
private slots:
	/*! open the i-th recent project */
	void loadRecentProject();
	/*! open the i-th system project */
	void loadSystemProject();
	/*! project changed */
	void markProjectAsChanged();
	/*! update the state of Actions (enable/disable) */
	void updateActionsState();
	/*! change project information (regarding TOTAL99 group), except plugin paths and files. */
	void changeProjectMainInfo();
	/*! change project information (regarding TOTAL99 group), only for plugin paths and files. */
	void changeProjectPluginInfo();
	/*! check the Configuration Parameters, and automatically add any
	 *  missing mandatory parameters and groups.
	 *  \param groupName is the name of the group from which the check start; it will traverse all the configurations
	 *   starting from groupName as root
	 *  \param parentPath is the full path to the parent of groupName
	 *  \note if some changes occurs inside this metod, it will reload all the groups tree and group editor
	 *  \note this method can only check the parameters and groups from which a description is present into
	 *        the typeDescriptor. The TOTAL99 group does not have such description and cannot be checked by
	 *        this method. Instead, the TOTAL99 is controlled by the GUI
	 */
	void checkConfigurationManager( QString groupName, QString parentPath );
	/*! it handle the changes of the value of a parameters
	 *
	 *  For now only the special parameter 'type' is handled, and it trigger the check of configuration parameters
	 *  for the mandatory parameters and groups
	 */
	void checkOnParameterChanges( QString parameter, QString value, QString prefix, salsa::ConfigurationManager* confParams );
	/*! checks and update all things when a group will be added
	 *  \param SubgroupName is the name of the subgroup added
	 *  \param prefix is the full path of the group containing SubgroupName
	 *  \param confParams is the ConfigurationManager where parameter belongs to
	 */
	void checkOnGroupAdding( QString SubgroupName, QString prefix, salsa::ConfigurationManager* confParams );
	/*! Update the component and configuration status labels */
	void updateStatusLabels();
	/*! Handle the trigger of a QAction associated with Viewer */
	void showViewerFromQAction();
	/*! Open a dialog to select a plugin */
	void changePlugin();
	/*! Changes a new system plugin */
	void changeSystemPlugin(const QString& pluginName);
	/*! Open a dialog for selecting a new plugin */
	void addNewPlugin();
	/*! Adds a new system plugin */
	void addSystemPlugin(const QString& pluginName);
	/*! Remove the current plugin */
	void removePlugin();
	/*! Context menu for edit/add/remove plugins */
	void contextMenuPlugin(const QPoint& pos);
	/*! Open a dialog to select a path for plugins */
	void changePluginPath();
	/*! Open a dialog for selecting a new path for plugins */
	void addNewPluginPath();
	/*! Remove the current plugin path */
	void removePluginPath();
	/*! Context menu for edit/add/remove plugin paths */
	void contextMenuPluginPath(const QPoint& pos);
private:
	/*! Add the recent project into the recentProjectsList at the top or bottom position depending
	 *  on the boolean parameter
	 *  \param onTop if it's true add the element on the top position of the list (default behaviour)
	 */
	void addRecentProjectToList( QStringList aRecentProject, bool onTop = true );
	/*! load the project ; return true on success */
	bool loadProjectConfiguration( salsa::ConfigurationManager& newProjectConfiguration );
	/*! update the recent Projects loaded */
	void updateRecentProjects();
	/*! clean and remove all the UI stuff created for the component */
	void cleanComponentUIs();
	/*! create all the UI stuff for the current component */
	void createComponentUIs();
	/*! loads a project from the given configuration file */
	void loadProjectFromFile( QString configurationFileName );

	/*! Copies data of a system plugin from system directory to target directory. relPluginPath is
	 *  the plugin directory relative to the system plugin directory, while destPath is where the
	 *  plugin directory has to be copied. If copyContent is true only the content of relPluginDir
	 *  is copied, if it is false the directory itself is copied returns false in case of errors.
	 *  This function always overwrites data
	 */
	bool copyPluginData(const QString& relPluginPath, const QString& destPath, bool copyContent);

	/*! The handler of the close event. Here we delete the batchInstancesManager window */
	void closeEvent(QCloseEvent* event);

	/*! The main layout */
	QStackedLayout* mainLay;
	/*! The layout for changing from editing and runtime project view */
	QStackedWidget* projectEdit2RuntimeLay;
	/*! The main bar of Menus */
	QMenuBar* menus;
	/*! The Run Menu */
	QMenu* runMenu;
	/*! The View Menu */
	QMenu* viewMenu;
	/*! The list of QAction created for the viewMenu */
	QList<QAction*> viewActions;
	/*! The main toolbar */
	QToolBar* toolbar;
	/*! The Map of action's name -> to it's QAction instance
	 *  \note useful for implementation of batch modality: in this way, from the command line it is possibile
	 *    to specify the action to trigger automatically by its name
	 *  \note The name of an action is retrieved by the method getQActions of the main experiment object
	 */
	QMap<QString, QAction*> actionsMap;
	/*! QListWidget for listing the recent projects */
	QListWidget* recentProjectsList;
	/*! QListWidget for listing the projects in the system directory */
	QListWidget* systemProjectsList;
	/*! GroupTreeEditor widget for the project's groups */
	GroupsTreeEditor* projectGroupsTree;
	/*! edit the parameters of a group */
	GenericGroupEditor* projectGroupEditor;
	/*! edit the project name */
	QLineEdit* projectNameEdit;
	/*! edit the description of the project */
	QTextEdit* projectDescrEdit;
	/*! edit the author of the project */
	QLineEdit* projectAuthorEdit;
	/*! edit the list of plugins for the project */
	QListWidget* projectPluginsEdit;
	/*! The signal mapper for actions to add system plugins */
	QSignalMapper* systemPluginsAddActionsMapper;
	/*! The signal mapper for actions to change system plugins */
	QSignalMapper* systemPluginsChangeActionsMapper;
	/*! edit the list of pluginPaths for the project */
	QListWidget* projectPPathsEdit;
	/*! ComboBox for the logging verbosity levels */
	QComboBox* projectLogLevelEdit;
	/*! label for showing the last modified date */
	QLabel* projectDateLabel;
	/*! label for showing the status of the ConfigurationManager of the project */
	QLabel* projectConfLabel;
	/*! label for showing the status of the Component of the project */
	QLabel* projectComLabel;
	/*! the status bar on which showing information */
	QStatusBar* statusBar;
	/*! The ConfigurationManager loaded and currently managed */
	salsa::ConfigurationManager project;
	/*! The file where the project is */
	QFileInfo projectFile;
	/*! it's true if a valid project has been loaded, false otherwise */
	bool projectLoaded;
	/*! it's true when some parameters has been changed, but these changes has not been saved on file */
	bool projectChanged;
	/*! it's true when some parameters has been changed, but these changes has not been applied to the component */
	bool componentOutdated;
	/*! the instance of the component loaded from the project */
	salsa::Component* component;
	/*! This ConfigurationManager regards to the global preferences and configurations merged with
	 *  the user preferences and configurations
	 */
	salsa::ConfigurationManager allprefs;
	/*! This ConfigurationManager regards only the user preferences and configurations
	 *  It's only used to save on the file only things different from the default ones
	 */
	salsa::ConfigurationManager userprefs;
	/*! the list of last loaded projects
	 *  Each element of this QVector contains a list of informations about the project:
	 *  0 -> name as reported in the project file
	 *  1 -> full path of the project's filename
	 *  2 -> author as reported in the project file
	 *  3 -> date of the last access to the file
	 *  4 -> comment as reported in the project file
	 */
	QList<QStringList> recentProjects;
	/*! the list of last projects in the system directory. The format is the same of
	 *  recentProjects
	 */
	QList<QStringList> systemProjects;
	/*! the maximum number of recent projects to remember */
	int maxRecentProjects;

	/*! the widget for show the log messagges */
	QTextEdit* logViewer;

	/*! the UI managers created that manage the viewers */
	QList<salsa::ComponentUI*> uimanagers;
	/*! information about the viewers created for the component created */
	QMap<QAction*, QWidget*> viewersMap;
	/*! The ViewersManager */
	ViewersManager* viewersManager;
	/*! the list of objects created and their path into the configuration file */
#warning WE CAN REMOVE THE PAIR, EACH COMPONENT ALSO KNOWS ITS PATH
	QList< QPair<salsa::Component*, QString> > objsCreated;
	/*! The geometry of the main window to be saved */
	QByteArray geometryToSave;

	/*! The widget managing instances of total99 started in batch mode */
	BatchInstancesManager* batchInstancesManager;
};

#endif
