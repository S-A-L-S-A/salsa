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

#include "projectmanager.h"
#include "total99resources.h"
#include "configurationhelper.h"
#include "genericgroupeditor.h"
#include "logger.h"
#include "viewersmanager.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidgetItem>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QLibrary>
#include <QFrame>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTime>
#include <QScrollArea>
#include <QTabWidget>
#include <QDirIterator>
#include <QFileInfo>
#include <QPushButton>

using namespace farsa;

ProjectManager::ProjectManager( QWidget* parent, Qt::WindowFlags f )
	: QWidget( parent, f )
	, viewActions()
	, actionsMap()
	, project()
	, projectFile()
	, projectLoaded(false)
	, projectChanged(false)
	, componentOutdated(false)
	, component(NULL)
	, allprefs()
	, userprefs()
	, recentProjects()
	, uimanagers()
	, viewersMap()
	, objsCreated()
	, batchInstancesManager(NULL)
{
	//--- read the total99.ini preferences into default directory where total99 has been installed
	//    and then merge the defaul preferences with those into total99.ini into the user directory
	allprefs.loadParameters( Total99Resources::confBasePath + "/total99.ini" );
	allprefs.loadParameters( Total99Resources::confUserPath + "/total99.ini", true );
	userprefs.loadParameters( Total99Resources::confUserPath + "/total99.ini" );
	applyUITemplate( ConfigurationHelper::getString( allprefs, "UI/template", "kids" ) );

	// Creating the widget managing batch instances
	batchInstancesManager = new BatchInstancesManager(userprefs, NULL);

	//--- create the viewers manager
	viewersManager = new ViewersManager( this, allprefs, "" );

	//--- default QAction always present
	QAction* action;
	action = new QAction( QIcon(Total99Resources::findResource("quitTotal99.png")), "Quit", this);
	connect( action, SIGNAL(triggered()), qApp, SLOT(quit()) );
	actionsMap["quitTotal99"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("loadProject.png")), "Load Project", this);
	connect( action, SIGNAL(triggered()), this, SLOT(loadProject()) );
	actionsMap["loadProject"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("newProject.png")), "New Project", this);
	connect( action, SIGNAL(triggered()), this, SLOT(newProject()) );
	actionsMap["newProject"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("closeProject.png")), "Close Project", this);
	connect( action, SIGNAL(triggered()), this, SLOT(closeProject()) );
	actionsMap["closeProject"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("saveProject.png")), "Save Project", this);
	connect( action, SIGNAL(triggered()), this, SLOT(saveProject()) );
	actionsMap["saveProject"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("saveAsProject.png")), "Save Project As...", this);
	connect( action, SIGNAL(triggered()), this, SLOT(saveAsProject()) );
	actionsMap["saveAsProject"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("createExperiment.png")), "Create/Reconfigure Experiment", this);
	connect( action, SIGNAL(triggered()), this, SLOT(configureComponent()) );
	actionsMap["createExperiment"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("closeExperiment.png")), "Close Experiment", this);
	connect( action, SIGNAL(triggered()), this, SLOT(closeComponent()) );
	actionsMap["closeExperiment"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("loadPlugin.png")), "Load Plugin", this);
	connect( action, SIGNAL(triggered()), this, SLOT(loadPlugin()) );
	actionsMap["loadPlugin"] = action;
	action = new QAction( QIcon(Total99Resources::findResource("startBatch.png")), "Manage Batch Instances", this);
	connect( action, SIGNAL(triggered()), batchInstancesManager, SLOT(show()) );
	actionsMap["startBatch"] = action;

	//--- Main Layout
	mainLay = new QStackedLayout(this);

	//--- MenuBar Widget and Layout
	QWidget* menuBar = new QWidget( this );
	mainLay->setMenuBar( menuBar );
	QVBoxLayout* menuLay = new QVBoxLayout(menuBar);
	menuLay->setContentsMargins(0, 0, 0, 0);
	menuBar->setLayout( menuLay );

	//--- creating Menus
#ifdef FARSA_MAC
	//--- the menu has to be handled in different way on Mac OS X
	//    See Qt Documentation about it
	menus = new QMenuBar( this );
#else
	menus = new QMenuBar( menuBar );
	menuLay->addWidget( menus );
#endif
	//--- populating the menus
	QMenu* amenu;
	amenu = menus->addMenu( "File" );
	amenu->setObjectName( "FileMenu" );
	amenu->addAction( actionsMap["loadProject"] );
	amenu->addAction( actionsMap["newProject"] );
	amenu->addAction( actionsMap["saveProject"] );
	amenu->addAction( actionsMap["saveAsProject"] );
	amenu->addSeparator();
	amenu->addAction( actionsMap["loadPlugin"] );
	amenu->addSeparator();
	amenu->addAction( actionsMap["createExperiment"] );
	amenu->addAction( actionsMap["closeExperiment"] );
	amenu->addSeparator();
	amenu->addAction( actionsMap["closeProject"] );
	amenu->addSeparator();
	amenu->addAction( actionsMap["startBatch"] );
	amenu->addSeparator();
	amenu->addAction( actionsMap["quitTotal99"] );

	//--- the Run menu will be populated when a project will be loaded
	runMenu = menus->addMenu( "Actions" );
	runMenu->setObjectName( "ActionsMenu" );
	//--- the Views menu will be populated when a project will be loaded
	viewMenu = menus->addMenu( "Views" );
	viewMenu->setObjectName( "ViewsMenu" );

	//--- creating the Toolbar
	toolbar = new QToolBar( menuBar );
	toolbar->setIconSize( QSize(32, 32) );
	menuLay->addWidget( toolbar );
	//--- populating the toolbar
	toolbar->addAction( actionsMap["loadProject"] );
	toolbar->addAction( actionsMap["newProject"] );
	toolbar->addAction( actionsMap["saveProject"] );
	toolbar->addAction( actionsMap["saveAsProject"] );
	toolbar->addSeparator();
	toolbar->addAction( actionsMap["loadPlugin"] );
	toolbar->addSeparator();
	toolbar->addAction( actionsMap["createExperiment"] );
	toolbar->addAction( actionsMap["closeExperiment"] );
	toolbar->addSeparator();
	toolbar->addAction( actionsMap["closeProject"] );
	toolbar->addSeparator();
	toolbar->addAction( actionsMap["startBatch"] );
	//--- the Actions toolbar will be populated when a project will be loaded
	toolbar->addSeparator();
	//--- the Views toolbar will be populated when a project will be loaded
	toolbar->addSeparator();

	updateActionsState();

	//--- creating the QWidget with the list of recent opened project and wizards
	QWidget* introWidget = new QWidget(this);
	introWidget->setObjectName( "introWidget" );
	mainLay->addWidget( introWidget );
	QGridLayout* introLay = new QGridLayout(introWidget);
	//--- recent projects list
	QGroupBox* gbox = new QGroupBox( "Recent Projects", introWidget );
	introLay->addWidget( gbox, 0, 0 );
	QVBoxLayout* gboxLay = new QVBoxLayout( gbox );
	recentProjectsList = new QListWidget( gbox );
	recentProjectsList->setObjectName( "recentProjectsList" );
	recentProjectsList->setSelectionMode( QAbstractItemView::SingleSelection );
	connect( recentProjectsList, SIGNAL(itemDoubleClicked( QListWidgetItem* )), SLOT(loadRecentProject()) );
	gboxLay->addWidget( recentProjectsList );
	//--- projects in the system directory
	gbox = new QGroupBox( "System Projects", introWidget );
	introLay->addWidget( gbox, 1, 0 );
	gboxLay = new QVBoxLayout( gbox );
	systemProjectsList = new QListWidget( gbox );
	systemProjectsList->setObjectName( "recentProjectsList" ); // This has the same name of recentProjectsList to use the same style
	systemProjectsList->setSelectionMode( QAbstractItemView::SingleSelection );
	connect( systemProjectsList, SIGNAL(itemDoubleClicked( QListWidgetItem* )), SLOT(loadSystemProject()) );
	gboxLay->addWidget( systemProjectsList );
	//--- wizard list - Nothing for the moment
// 	gbox = new QGroupBox( "Wizards", introWidget );
// 	introLay->addWidget( gbox, 0, 1 );
// 	gboxLay = new QVBoxLayout(gbox);
// 	gboxLay->addWidget( new QLabel("Here will be the list of wizards\n for creating new Projects",gbox) );

	//--- creating the QWidget to visualize/edit project's parameters
	QWidget* projectEditor = new QWidget( this );
	projectEditor->setObjectName( "projectEditor" );
	mainLay->addWidget( projectEditor );
	QGridLayout* projectLay = new QGridLayout( projectEditor );
	projectLay->setVerticalSpacing( 0 );
	projectLay->setContentsMargins( 0, 0, 0, 0 );

	//--- frame and widgets for TOTAL99 Parameters
	QFrame* frameTotal = new QFrame( projectEditor );
	frameTotal->setObjectName( "projectMainInfo" );
	projectLay->addWidget( frameTotal, 1, 0, 1, 2 );
	projectLay->setRowStretch( 1, 0 );
	QGridLayout* frameLay = new QGridLayout( frameTotal );

	QWidget* frameTotalLeft = new QWidget( frameTotal );
	QGridLayout* frameLeftLay = new QGridLayout( frameTotalLeft );
	frameLeftLay->setContentsMargins(0,0,0,0);
	QLabel* lb = new QLabel( "Name:", frameTotalLeft );
	frameLeftLay->addWidget( lb, 0, 0 );
	projectNameEdit = new QLineEdit( frameTotalLeft );
	connect( projectNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(changeProjectMainInfo()) );
	frameLeftLay->addWidget( projectNameEdit, 0, 1 );
	lb = new QLabel( "Author:", frameTotalLeft );
	frameLeftLay->addWidget( lb, 1, 0 );
	projectAuthorEdit = new QLineEdit( frameTotalLeft );
	connect( projectAuthorEdit, SIGNAL(textEdited(const QString&)), this, SLOT(changeProjectMainInfo()) );
	frameLeftLay->addWidget( projectAuthorEdit, 1, 1 );
	QGroupBox* descriptionBox = new QGroupBox( "Project Description", frameTotalLeft );
	QVBoxLayout* descriptionBoxLay = new QVBoxLayout( descriptionBox );
	descriptionBoxLay->setContentsMargins(0,0,0,0);
	projectDescrEdit = new QTextEdit( descriptionBox );
	projectDescrEdit->setMinimumHeight( 110 );
	projectDescrEdit->setMaximumHeight( 110 );
	connect( projectDescrEdit, SIGNAL(textChanged()), this, SLOT(changeProjectMainInfo()) );
	descriptionBoxLay->addWidget( projectDescrEdit );
	frameLeftLay->addWidget( descriptionBox, 2, 0, 2, 2 );
	frameLay->addWidget(frameTotalLeft, 0, 0);

	QWidget* frameTotalRigth = new QWidget( frameTotal );
	QGridLayout* frameRightLay = new QGridLayout( frameTotalRigth );
	frameRightLay->setContentsMargins(0,0,0,0);
	QGroupBox* pluginBox = new QGroupBox( "Plugins", frameTotalRigth );
	QVBoxLayout* pluginBoxLay = new QVBoxLayout( pluginBox );
	pluginBoxLay->setContentsMargins(0,0,0,0);
	projectPluginsEdit = new QListWidget( pluginBox );
	pluginBoxLay->addWidget( projectPluginsEdit );
	projectPluginsEdit->setMinimumHeight( 50 );
	projectPluginsEdit->setMaximumHeight( 50 );
	projectPluginsEdit->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( projectPluginsEdit, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changePlugin()) );
	connect( projectPluginsEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuPlugin(const QPoint&)) );
	frameRightLay->addWidget( pluginBox, 0, 0, 1, 2 );
	systemPluginsAddActionsMapper = new QSignalMapper(this);
	connect(systemPluginsAddActionsMapper, SIGNAL(mapped(const QString &)), this, SLOT(addSystemPlugin(const QString &)));
	systemPluginsChangeActionsMapper = new QSignalMapper(this);
	connect(systemPluginsChangeActionsMapper, SIGNAL(mapped(const QString &)), this, SLOT(changeSystemPlugin(const QString &)));
	QGroupBox* pluginPathBox = new QGroupBox( "Plugin Paths", frameTotalRigth );
	QVBoxLayout* pluginPathBoxLay = new QVBoxLayout( pluginPathBox );
	pluginPathBoxLay->setContentsMargins(0,0,0,0);
	projectPPathsEdit = new QListWidget( pluginPathBox );
	pluginPathBoxLay->addWidget( projectPPathsEdit );
	projectPPathsEdit->setMinimumHeight( 50 );
	projectPPathsEdit->setMaximumHeight( 50 );
	projectPPathsEdit->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( projectPPathsEdit, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changePluginPath()) );
	connect( projectPPathsEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuPluginPath(const QPoint&)) );
	frameRightLay->addWidget( pluginPathBox, 1, 0, 1, 2 );
	lb = new QLabel( "Logging Verbosity:", frameTotalRigth );
	frameRightLay->addWidget( lb, 2, 0 );
	projectLogLevelEdit = new QComboBox( frameTotalRigth );
	projectLogLevelEdit->addItems( QStringList() << "LogAll" << "Warning" << "Quiet" << "Superquiet" );
	connect( projectLogLevelEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(changeProjectMainInfo()) );
	frameRightLay->addWidget( projectLogLevelEdit, 2, 1 );
	frameLay->addWidget(frameTotalRigth, 0, 1);

	//--- layout for handling the switch between editing and runtime views of a Project
	projectEdit2RuntimeLay = new QStackedWidget(projectEditor);
	projectLay->addWidget( projectEdit2RuntimeLay, 2, 0, 1, 2 );
	projectLay->setRowStretch( 2, 1 );

	QSplitter* projectSplitter = new QSplitter(projectEdit2RuntimeLay);
	projectSplitter->setObjectName( "projectSplitter" );
	projectEdit2RuntimeLay->addWidget( projectSplitter );
	//--- Groups Editor
	projectGroupsTree = new GroupsTreeEditor( &project, projectEditor );
	projectGroupsTree->setObjectName( "projectGroupsTree" );
	projectSplitter->addWidget( projectGroupsTree );
	QScrollArea* scrollArea = new QScrollArea( projectEditor );
	scrollArea->setWidgetResizable( true );
	projectGroupEditor = new GenericGroupEditor( scrollArea );
	scrollArea->setWidget( projectGroupEditor );
	projectGroupEditor->setObjectName( "projectGroupEditor" );
	connect( projectGroupsTree, SIGNAL(currentGroupChanged(GroupInfo, farsa::ConfigurationManager*)),
			 projectGroupEditor, SLOT(setGroup(GroupInfo, farsa::ConfigurationManager*)) );
	connect( projectGroupEditor, SIGNAL(parameterChanged(QString, QString, QString, farsa::ConfigurationManager*)),
			 this, SLOT(checkOnParameterChanges(QString, QString, QString, farsa::ConfigurationManager*)) );
	connect( projectGroupEditor, SIGNAL(groupRenamed(QString, QString, QString, farsa::ConfigurationManager*)),
			 projectGroupsTree, SLOT(groupRenamed(QString, QString, QString, farsa::ConfigurationManager*)) );
	connect( projectGroupEditor, SIGNAL(groupAdded(QString, QString, farsa::ConfigurationManager*)),
			 this, SLOT(checkOnGroupAdding(QString, QString, farsa::ConfigurationManager*)) );
	projectSplitter->addWidget( scrollArea );
	projectSplitter->setStretchFactor( 1, 2 );
	connect( projectGroupEditor, SIGNAL(parameterChanged(QString, QString, QString, farsa::ConfigurationManager*)),
			 this, SLOT(markProjectAsChanged()) );
	connect( projectGroupEditor, SIGNAL(parameterAdded(QString, QString, QString, farsa::ConfigurationManager*)),
			 this, SLOT(markProjectAsChanged()) );
	connect( projectGroupEditor, SIGNAL(parameterRemoved(QString, QString, QString, farsa::ConfigurationManager*)),
			 this, SLOT(markProjectAsChanged()) );
	connect( projectGroupEditor, SIGNAL(groupRenamed(QString, QString, QString, farsa::ConfigurationManager*)),
			 this, SLOT(markProjectAsChanged()) );

	//--- create the Tabs for LogViewer and Runtime Parameters editor
	QTabWidget* projectRuntimeTabs = new QTabWidget( projectEdit2RuntimeLay );
	projectRuntimeTabs->setObjectName( "projectRuntimeTabs" );
	projectEdit2RuntimeLay->addWidget( projectRuntimeTabs );
	logViewer = new QTextEdit( projectRuntimeTabs );
	logViewer->setObjectName( "logViewer" );
	logViewer->setReadOnly( true );
	logViewer->setLineWrapMode( QTextEdit::NoWrap );
	projectRuntimeTabs->addTab( logViewer, QIcon(Total99Resources::findResource("logViewer.png")), "Activity" );
	//--- setup the Logger to use this logViewer
	Logger::setQTextEdit( logViewer );

	//--- Status Bar
	statusBar = new QStatusBar(this);
	projectLay->addWidget( statusBar, 3, 0, 1, 2 );
	statusBar->setSizeGripEnabled( false );
	statusBar->setContentsMargins( 4, 0, 4, 0 );
	projectComLabel = new QLabel( "Experiment Status", projectEditor );
	projectComLabel->setTextFormat( Qt::RichText );
	statusBar->addPermanentWidget( projectComLabel );
	projectConfLabel = new QLabel( "Configuration Status", projectEditor );
	projectConfLabel->setTextFormat( Qt::RichText );
	statusBar->addPermanentWidget( projectConfLabel );
	projectDateLabel = new QLabel( "Last Modified" );
	statusBar->addPermanentWidget( projectDateLabel );

	//--- restore previous state
	// + Loads the list of recent projects
	maxRecentProjects = ConfigurationHelper::getInt( allprefs, "ProjectManager/maxRecentProjects", 5 );
	QString tmpl( "ProjectManager/recentProject:%1/%2" );
	QStringList infos;
	infos << "name" << "path" << "author" << "date" << "comment";
	if (allprefs.groupExists("ProjectManager")) {
		QStringList recentsList = allprefs.getGroupsWithPrefixList( "ProjectManager", "recentProject:" );
		for( int i=0; i<qMin(maxRecentProjects, recentsList.size()); i++ ) {
			QString recentGroup = recentsList[i];
			QStringList recent;
			foreach( QString field, infos ) {
				const QString n = "ProjectManager/" + recentGroup + "/" + field;
				if (allprefs.parameterExists(n)) {
					recent.append(allprefs.getValue(n));
				} else {
					recent.append("");
				}
			}
			recentProjects.append( recent );
			addRecentProjectToList( recent, false );
		}
	}
	// + Loads the list of system projects
	// Iterating the plugins system configuration path looking for configuration files
	for (QDirIterator it(Total99Resources::pluginConfigBasePath, QDirIterator::Subdirectories); it.hasNext(); it.next()) {
		const QString curFile = it.fileInfo().canonicalFilePath();

		// Trying to load the file to get information
		ConfigurationManager params;
		if (!params.loadParameters(curFile)) {
			// Silently ignoring invalid files
			continue;
		}

		// Now we have to read the main information to fill the list of projects
		const QString name = ConfigurationHelper::getString(params, "TOTAL99/name", "");
		const QString author = ConfigurationHelper::getString(params, "TOTAL99/author", "");
		const QString date = ConfigurationHelper::getString(params, "TOTAL99/date", "");
		const QString descr = ConfigurationHelper::getString(params, "TOTAL99/description", "");
		systemProjects.append(QStringList() << name << curFile << author << date << descr);
		QListWidgetItem* item = new QListWidgetItem(QIcon(Total99Resources::findResource("projectIcon.png")), name);
		item->setToolTip(descr + "\nAuthor: " + author + " Date: " + date + "\nFile: " + curFile);
		systemProjectsList->addItem(item);
	}

	//--- restore the geometry of the ProjectManager
	QString str = ConfigurationHelper::getString(userprefs, "ProjectManager/geometry", "");
	if (!str.isEmpty()) {
		restoreGeometry(QByteArray::fromBase64(str.toLatin1()));
	}
}

ProjectManager::~ProjectManager() {
}

void ProjectManager::onComponentCreation(farsa::Component* component) {
	objsCreated.prepend(qMakePair(component, component->confPath()));
}

void ProjectManager::onComponentDestruction(farsa::Component*)
{
}

void ProjectManager::onQuit() {
	if (component == NULL) {
		// We only save geometry here if there is no component, because the geometry for the component
		// could be different from the starting geometry of the window
		geometryToSave = saveGeometry();
	}
	closeComponent();
	//--- save the current state
	// + Save the list of recent projects
	QString tmpl( "ProjectManager/recentProject:%1" );
	QStringList infos;
	infos << "name" << "path" << "author" << "date" << "comment";
	for( int i=0; i<recentProjects.size(); i++ ) {
		userprefs.createGroup( tmpl.arg(i) );
		for( int f=0; f<infos.size(); f++ ) {
			if (userprefs.parameterExists(tmpl.arg(i) + "/" + infos[f])) {
				userprefs.setValue(tmpl.arg(i) + "/" + infos[f], recentProjects[i][f]);
			} else {
				userprefs.createParameter(tmpl.arg(i), infos[f], recentProjects[i][f]);
			}
		}
	}
	//--- save the geometry of the ProjectManager
	if (!userprefs.groupExists("ProjectManager")) {
		userprefs.createGroup("ProjectManager");
	}
	if (userprefs.parameterExists("ProjectManager/geometry")) {
		userprefs.setValue("ProjectManager/geometry", geometryToSave.toBase64());
	} else {
		userprefs.createParameter("ProjectManager", "geometry", geometryToSave.toBase64());
	}

	// Saving the status of the ViewersManager
	viewersManager->save(userprefs, "");

	//--- save the file on the user directory
	userprefs.saveParameters( Total99Resources::confUserPath + "/total99.ini" );

	// Delete the batchInstancesManager
	delete batchInstancesManager;
	batchInstancesManager = NULL;
}

void ProjectManager::loadProject() {
	// --- ask for project file to load
	QString newf = QFileDialog::getOpenFileName( this, "Load Project from...", projectFile.absolutePath(), "FARSA Configuration file (*.ini *.xml)" );
	if ( newf.isEmpty() ) {
		return;
	}
	//--- load the file
	loadProjectFromFile( newf );
}

void ProjectManager::newProject() {
	//--- create a ConfigurationParameter with minimal informations
	ConfigurationManager newp;
	observe(&newp);
	QString total99Group = newp.createSubGroup( "", "TOTAL99" );
	newp.createParameter( total99Group, "name", "Name of the Project" );
	newp.createParameter( total99Group, "author", "Author Name" );
	QString datenow = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
	newp.createParameter( total99Group, "date", datenow );
	newp.createParameter( total99Group, "description", "Project Description" );
	newp.createParameter( total99Group, "mainComponent", "Component" );
	QString expGroup = newp.createSubGroup( "", "Component" );
	newp.createParameter( expGroup, "type", "Component" );
	//--- load the configuration
	loadProjectConfiguration( newp );
	projectFile = QFileInfo( "newproject.ini" );
	updateActionsState();
	updateStatusLabels();
}

void ProjectManager::closeProject() {
	if ( projectChanged ) {
		//--- TODO: ask confirmation, or to save
	}
	projectGroupEditor->unsetGroup();
	project.clearAll();
	projectGroupsTree->setConfigurationManager( &project );
	projectLoaded = false;
	projectChanged = false;
	componentOutdated = true;
	closeComponent();
	mainLay->setCurrentIndex( 0 );
	updateActionsState();
}

void ProjectManager::saveProject() {
	if ( !projectLoaded ) return;
	if ( !project.saveParameters( projectFile.absoluteFilePath() ) ) {
		QMessageBox::warning( this, QString("Error saving Project ") + projectFile.fileName(), QString("The operation of saving the project has been failed. Check if you have permission to write on the disk") );
		return;
	}
	projectChanged = false;
	updateActionsState();
	updateStatusLabels();
	projectDateLabel->setText( QString("Last Modified ")+project.getValue( "TOTAL99/date" ) );
	updateRecentProjects();
}

void ProjectManager::saveAsProject() {
	if ( !projectLoaded ) return;
	// --- ask for project file to load
	QString newf = QFileDialog::getSaveFileName( this, "Save Project as...", projectFile.absolutePath(), "FARSA Configuration file (*.ini *.xml)" );
	if ( newf.isEmpty() ) {
		return;
	}
	if ( project.saveParameters( newf ) ) {
		projectFile = QFileInfo( newf );
		QDir::setCurrent( projectFile.absolutePath() );
		projectChanged = false;
	} else {
		QMessageBox::warning( this, QString("Error saving Project ") + newf, QString("The operation of saving the project has been failed. Check if you have permission to write on the disk") );
		return;
	}
	updateActionsState();
	updateStatusLabels();
	projectDateLabel->setText( QString("Last Modified ")+project.getValue( "TOTAL99/date" ) );
	updateRecentProjects();
}

void ProjectManager::closeComponent() {
	if ( !component ) return;
	// Asking the component to terminate all actions
#warning QUALCOSA DI SIMILE SI DEVE AGGIUNGERE DOPO LE MODIFICHE A ComponentUI, VEDI WARNING IN baseexperiment.h
	// component->stopCurrentOperation();
	//disconnect( component, SIGNAL(statusChanged(QString)), this, SLOT(updateStatusLabels()) );
	cleanComponentUIs();
	delete component;
	component = NULL;
	projectEdit2RuntimeLay->setCurrentIndex( 0 );
	// when re-enable all edit widgets
	projectNameEdit->setEnabled( true );
	projectDescrEdit->setEnabled( true );
	projectAuthorEdit->setEnabled( true );
	projectPluginsEdit->setEnabled( true );
	projectPPathsEdit->setEnabled( true );
	projectLogLevelEdit->setEnabled( true );
	updateActionsState();
	updateStatusLabels();
	// Restoring the status of the main window
	restoreGeometry(geometryToSave);
}

void ProjectManager::cleanComponentUIs() {
	objsCreated.clear();
	//--- it destroy also all the viewers
	viewersManager->clear();
	foreach( QWidget* viewer, viewersMap.values() ) {
		delete viewer;
	}
	viewersMap.clear();
	runMenu->clear();
	foreach( QMenu* menu, runMenu->findChildren<QMenu*>() ) {
		delete menu;
	}
	viewMenu->clear();
	foreach( QMenu* menu, viewMenu->findChildren<QMenu*>() ) {
		delete menu;
	}
	foreach( QAction* act, viewActions ) {
		delete act;
	}
	viewActions.clear();
	// delete additionals menus - i.e.: all the menus/actions after ViewsMenu
	bool startToDelete = false;
	foreach( QObject* obj, menus->children() ) {
		if ( startToDelete ) {
			delete obj;
			continue;
		}
		if ( obj->objectName() == "ViewsMenu" ) {
			startToDelete = true;
			continue;
		}
	}
	//--- delete uimanagers object
	foreach( ComponentUI* uimanager, uimanagers ) {
		delete uimanager;
	}
	uimanagers.clear();
}

void ProjectManager::createComponentUIs() {
	//--- prefix to remove from the submenu names
	QString rootPrefix = project.getValue( "TOTAL99/mainComponent" );
	QPair<Component*, QString> p;
	foreach( p, objsCreated ) {
		ComponentUI* uimanager = p.first->getUIManager();
		if ( !uimanager ) continue;
		//--- create the Actions menu
		QString menuTitle = p.second;
		menuTitle.remove( QRegExp( rootPrefix+"|"+rootPrefix+"/" ) );
		QMenu* menu = runMenu;
		if ( !menuTitle.isEmpty() ) {
			menu = runMenu->findChild<QMenu*>( menuTitle );
		}
		if ( !menu ) {
			//--- create a new menu for this object
			menu = new QMenu( p.first->typeName()+"@"+menuTitle, runMenu );
			menu->setObjectName( menuTitle );
			runMenu->addMenu( menu );
		}
		uimanager->fillActionsMenu( menu );
		if ( menu->isEmpty() && menu != runMenu ) {
			// remove the newly added menu if there is no action defined by the current ui manager
			delete menu;
		}
		//--- create view menu and add the viewers to the map
		menu = viewMenu;
		if ( !menuTitle.isEmpty() ) {
			menu = viewMenu->findChild<QMenu*>( menuTitle );
		}
		if ( !menu ) {
			//--- create a new viewer menu for this object
			menu = new QMenu( p.first->typeName()+"@"+menuTitle, viewMenu );
			menu->setObjectName( menuTitle );
			viewMenu->addMenu( menu );
		}
		foreach( ComponentUIViewer aview, uimanager->getViewers(0, 0) ) {
			ViewerWidget* wrapper = new ViewerWidget( aview.viewer->windowTitle(), p.second, aview.viewer, viewersManager, this );
			//--- create the QAction
			QAction* action = new QAction( aview.menuText, this );
			action->setToolTip( aview.tooltip );
			connect( action, SIGNAL(triggered()), this, SLOT(showViewerFromQAction()) );
			menu->addAction( action );
			viewActions.append( action );
			viewersMap[action] = wrapper;
		}
		if ( menu->isEmpty() && menu != viewMenu ) {
			// remove the newly added menu if there is no viewer defined by the current ui manager
			delete menu;
		}
		//--- add the additional menus
		uimanager->addAdditionalMenus( menus );
		uimanagers.append( uimanager );
	}
}

void ProjectManager::configureComponent() {
	if ( component ) {
		// Asking the component to terminate all actions
#warning QUALCOSA DEL GENERE VA FATTO DOPO LE MODIFICHE A ComponentUI, VEDI WARNING IN baseexperiment.h
// 		component->stopCurrentOperation();
// 		disconnect( component, SIGNAL(statusChanged(QString)), this, SLOT(updateStatusLabels()) );
		cleanComponentUIs();
		delete component;
	} else {
		// If there currently is no component, saving the main window geometry
		geometryToSave = saveGeometry();
	}
	//--- try to create the component as specified by mainComponent
	QString pexpg = project.getValue( "TOTAL99/mainComponent" );
	try {
		component = project.getComponentFromGroup<Component>( pexpg );
#warning QUALCOSA DEL GENERE VA FATTO DOPO LE MODIFICHE A ComponentUI, VEDI WARNING IN baseexperiment.h
		// connect( component, SIGNAL(statusChanged(QString)), this, SLOT(updateStatusLabels()) );
		Logger::info( "Created Component "+project.getValue( pexpg+"/type")+" from group "+pexpg );
		createComponentUIs();
	} catch( std::exception& e ) {
		cleanComponentUIs();
		component = NULL;
		updateActionsState();
		updateStatusLabels();
		QMessageBox::warning( this, QString("Error creating the Component"), QString("The Component ") + pexpg + QString(" cannot be created; The error is:\n") + e.what() + QString("\nCheck your configuration file.") );
		return;
	}
	componentOutdated = false;
	projectEdit2RuntimeLay->setCurrentIndex( 1 );
	// when running the component will disable some modification of parameters
	projectNameEdit->setEnabled( false );
	projectDescrEdit->setEnabled( false );
	projectAuthorEdit->setEnabled( false );
	projectPluginsEdit->setEnabled( false );
	projectPPathsEdit->setEnabled( false );
	projectLogLevelEdit->setEnabled( false );
	updateActionsState();
	updateStatusLabels();

	// Restoring the status of all viewers
	viewersManager->restoreViewersStatus(projectFile.absoluteFilePath());

	emit componentConfigured();
}

void ProjectManager::loadPlugin() {
	// --- ask for file to load
	QString newf = QFileDialog::getOpenFileName( this, "Load Plugin from...", projectFile.absolutePath(), "FARSA Plugins (*.so *.dylib *.bundle *.dll)" );
	if ( newf.isEmpty() ) {
		return;
	}
	//--- load the file
	Total99Resources::loadPlugin( newf );
	if ( projectGroupsTree ) {
		// --- we need to update all information of groups after loading plugins
		//     some type in typeDescriptions may be changed
		projectGroupsTree->updateGroupsInformation();
		projectGroupEditor->reloadGroup();
	}
}

void ProjectManager::applyUITemplate( QString templateName ) {
	Total99Resources::uiTemplate = templateName;
	//--- check if the template is in the user directory or global one
	QDir templateDir( Total99Resources::confUserPath + "/templates/" + Total99Resources::uiTemplate );
	if ( !templateDir.exists() ) {
		templateDir = QDir( Total99Resources::confBasePath + "/templates/" + Total99Resources::uiTemplate );
		if ( !templateDir.exists() ) {
			// ERROR !! It's not possibile to find the template requested
			Logger::error( "Failed to load Total99 UI Template "+templateName+ " - Template Not Found" );
			return;
		}
	}
	//--- load any fonts in the template directory
	QStringList fontTypes;
	fontTypes << "*.ttf" << "*.otf";
	foreach( QString font, templateDir.entryList( fontTypes ) ) {
		int fid = QFontDatabase::addApplicationFont( templateDir.absoluteFilePath( font ) );
		if ( fid != -1 ) {
			Logger::info( "Applying Total99 UI Template "+templateName+" - Loaded Fonts: " + QFontDatabase::applicationFontFamilies( fid ).join(", ") );
		}
	}
	//--- setting the style sheet of the application
	QFile appStyleFile( templateDir.absoluteFilePath( "qAppStyle.css" ) );
	if ( appStyleFile.open( QFile::ReadOnly ) ) {
		QTextStream in( &appStyleFile );
		qApp->setStyleSheet( in.readAll().replace( "${TemplateDir}", templateDir.absolutePath() ) );
	}
}

void ProjectManager::loadRecentProject() {
	//--- load the file
	loadProjectFromFile( recentProjects[recentProjectsList->currentRow()][1] );
}

void ProjectManager::loadSystemProject()
{
	// Getting the local destination of system plugins (where they are copied before being used). If
	// the path is not absolute, adding the user home directory as prefix. We read userprefs first
	// because we update it if the default destination directory changes
	QString localPluginDestination = ConfigurationHelper::getString(userprefs, "ProjectManager/localPluginDestination", "");
	if (localPluginDestination.isEmpty()) {
		localPluginDestination = ConfigurationHelper::getString(allprefs, "ProjectManager/localPluginDestination", "farsaPlugins");
	}
	if (QFileInfo(localPluginDestination).isRelative()) {
		localPluginDestination = QDir::homePath() + "/" + localPluginDestination;
	}

	// Now all extracting the path of the system plugin relative to the system plugin path
	const QString pluginFilename = systemProjects[systemProjectsList->currentRow()][1];
	const QString pluginPath = QFileInfo(pluginFilename).canonicalPath();
	const QDir systemPluginPath(Total99Resources::pluginConfigBasePath);
	const QString relativePluginFilename = systemPluginPath.relativeFilePath(pluginFilename);
	const QString relativePluginPath = systemPluginPath.relativeFilePath(pluginPath);

	// Creating the messagebox to ask the user what to do
	QMessageBox msgBox(this);

	// Setting icon and title
	msgBox.setIcon(QMessageBox::NoIcon);
	msgBox.setWindowTitle("Copying system plugin");

	// Checking if the plugin exists in the current localPluginDestination and setting the messagebox accordingly
	const QString localPluginPath = localPluginDestination + "/" + relativePluginPath;
	QString pluginConfigurationFile; // The plugin configuration file to use
	QPushButton *useLocalCopy = NULL;
	QPushButton *copyToDefaultDestination = NULL;
	if (QFileInfo(localPluginPath).exists()) {
		msgBox.setText(QString("The selected system plugin (%1) will be copied to a writable location. The default location is \"%2\", and already exists. You can choose to use data in the existing directory, overwrite it or choose another directory.").arg(systemProjects[systemProjectsList->currentRow()][0]).arg(localPluginDestination + "/" + relativePluginFilename));

		useLocalCopy = msgBox.addButton("Use local copy", QMessageBox::ActionRole);
		copyToDefaultDestination = msgBox.addButton("Overwrite local copy", QMessageBox::ActionRole);
	} else {
		msgBox.setText(QString("The selected system plugin (%1) will be copied to a writable location. The default location is \"%2\", but you can choose another directory if you want.").arg(systemProjects[systemProjectsList->currentRow()][0]).arg(localPluginDestination + "/" + relativePluginFilename));

		copyToDefaultDestination = msgBox.addButton("Copy to default location", QMessageBox::ActionRole);
	}


	// Adding common buttons
	QPushButton *copyToAnotherDestination = msgBox.addButton("Select another location", QMessageBox::ActionRole);
	QPushButton *cancel = msgBox.addButton(QMessageBox::Cancel);

	// Executing the message box
	msgBox.exec();

	// Checking what to do
	if (msgBox.clickedButton() != cancel) {
		if (msgBox.clickedButton() == useLocalCopy) {
			pluginConfigurationFile = localPluginDestination + "/" + relativePluginFilename;
		} else if (msgBox.clickedButton() == copyToDefaultDestination) {
			if (copyPluginData(relativePluginPath, localPluginDestination, false)) {
				pluginConfigurationFile = localPluginDestination + "/" + relativePluginFilename;
			} else {
				QMessageBox::critical(this, QString("Error copying system project"), QString("Error copying the system plugin to a writable location. You should try to select a different directory."));
			}
		} else if (msgBox.clickedButton() == copyToAnotherDestination) {
			// Let the user select the directory to use. We copy the content of the plugin directory
			// in the chosen directory and set localPluginDestination to the parent of the directory
			const QString newDestination = QFileDialog::getExistingDirectory(this, "Select new plugin destination", localPluginDestination);

			if (!newDestination.isEmpty()) {
				// The new localPluginDestination is the parent of the selected directory
				QDir newDir(newDestination);
				newDir.cdUp();
				localPluginDestination = newDir.canonicalPath();

				if (copyPluginData(relativePluginPath, newDestination, true)) {
					pluginConfigurationFile = newDestination + "/" + relativePluginFilename.mid(relativePluginFilename.indexOf("/"));
				} else {
					QMessageBox::critical(this, QString("Error copying system project"), QString("Error copying the system plugin to a writable location. You should try to select a different directory."));
				}
			}
		}
	}

	// If the used hasn't aborted the process, loading the project and updating configuration
	if (!pluginConfigurationFile.isEmpty()) {
		if (userprefs.parameterExists("ProjectManager/localPluginDestination")) {
			userprefs.setValue("ProjectManager/localPluginDestination", localPluginDestination);
		} else {
			userprefs.createParameter("ProjectManager", "localPluginDestination", localPluginDestination);
		}
		loadProjectFromFile(pluginConfigurationFile);
	}
}

void ProjectManager::markProjectAsChanged() {
	projectChanged = true;
	componentOutdated = true;
	// update the date label
	QString datenow = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
	project.createParameter( "TOTAL99", "date", datenow );
	projectDateLabel->setText( QString("Last Modified ")+datenow+" - <b>Not Saved</b>" );
	updateActionsState();
	updateStatusLabels();
}

void ProjectManager::updateActionsState() {
	actionsMap["loadProject"]->setEnabled( component==NULL );
	actionsMap["newProject"]->setEnabled( component==NULL );
	actionsMap["saveProject"]->setEnabled( component==NULL && projectChanged && projectFile.exists() );
	actionsMap["saveAsProject"]->setEnabled( component==NULL && projectLoaded /*|| !projectFile.exists()*/ );
	actionsMap["loadPlugin"]->setEnabled( component==NULL );
	actionsMap["closeProject"]->setEnabled( projectLoaded );
	actionsMap["createExperiment"]->setEnabled( projectLoaded );
	actionsMap["closeExperiment"]->setEnabled( component!=NULL );
}

void ProjectManager::changeProjectMainInfo() {
	projectChanged = true;
	if (project.groupExists("TOTAL99")) {
		project.deleteGroup("TOTAL99");
	}
	project.createGroup("TOTAL99");
	project.createParameter( "TOTAL99", "name", projectNameEdit->text() );
	project.createParameter( "TOTAL99", "author", projectAuthorEdit->text() );
	if ( projectLogLevelEdit->currentIndex() != 2 ) {
		project.createParameter( "TOTAL99", "logLevel", projectLogLevelEdit->currentText() );
	}
	project.createParameter( "TOTAL99", "description", projectDescrEdit->toPlainText() );
	// !!! for now, mainComponent is fixed !!!
	QString datenow = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
	project.createParameter( "TOTAL99", "date", datenow );
	projectDateLabel->setText( QString("Last Modified ")+datenow+" - <b>Not Saved</b>" );

	//--- set the logging level
	Logger::setLogLevel( Logger::stringToLogLevel(projectLogLevelEdit->currentText()) );

	updateActionsState();
}

void ProjectManager::changeProjectPluginInfo()
{
	projectChanged = true;
	// --- in order to get a correct update of the pluginPath and plugin
	//     parameters, first we  remove all current parameters in project
	//     configuration and then create new ones
	foreach( QString path, project.getParametersWithPrefixList( "TOTAL99", "pluginFile" ) ) {
		project.deleteParameter( "TOTAL99", path );
	}
	foreach( QString path, project.getParametersWithPrefixList( "TOTAL99", "pluginPath" ) ) {
		project.deleteParameter( "TOTAL99", path );
	}
	if ( projectPluginsEdit->count() == 1 ) {
		// --- if there is only one path, does not use the suffix
		project.createParameter( "TOTAL99", "pluginFile", projectPluginsEdit->item(0)->text() );
	} else {
		// --- in case of multiple paths use the suffix number
		for( int i=0; i<projectPluginsEdit->count(); i++ ) {
			project.createParameter( "TOTAL99", QString("pluginFile:%1").arg(i), projectPluginsEdit->item(i)->text() );
		}
	}
	if ( projectPPathsEdit->count() == 1 ) {
		// --- if there is only one path, does not use the suffix
		project.createParameter( "TOTAL99", "pluginPath", projectPPathsEdit->item(0)->text() );
	} else {
		// --- in case of multiple paths use the suffix number
		for( int i=0; i<projectPPathsEdit->count(); i++ ) {
			project.createParameter( "TOTAL99", QString("pluginPath:%1").arg(i), projectPPathsEdit->item(i)->text() );
		}
	}

	//--- load any plugin found into the directories specified by the pluginPath parameter
	Total99Resources::loadPlugins( project );

	updateActionsState();

	if ( projectGroupsTree ) {
		// --- we need to update all information of groups after loading plugins
		//     some type in typeDescriptions may be changed
		projectGroupsTree->updateGroupsInformation();
		projectGroupEditor->reloadGroup();
	}
}

void ProjectManager::checkConfigurationManager( QString groupName, QString parentPath ) {
	QString groupPath = parentPath+"/"+groupName;

	try {
		const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(project, groupPath);

		//--- get the list of all parameters from type descriptions and check if a mandatory one is missing
		const QStringList params = dc.parameters();

		foreach( QString aParam, params ) {
			const AbstractDescriptor& d = dc.parameterDescriptor(aParam);
			const bool isMandatory = d.props().testFlag(ParamIsMandatory);
			if ( !isMandatory ) continue;
			// when the param is mandatory, check if it is present and eventually add it
			if (!project.parameterExists(groupPath+"/"+aParam)) {
				QString defValue = ConfigurationHelper::getDefaultForDescriptorAsString(d);
				project.createParameter( groupPath, aParam, defValue );
			}
		}

		//--- get the list of all subgroups from type descriptions and check if a mandatory one is missing
		const QStringList subgroups = dc.subgroups();

		foreach( QString aGroup, subgroups ) {
			const SubgroupDescriptor& d = dc.subgroupDescriptor(aGroup);
			const bool isMandatory = d.props().testFlag(ParamIsMandatory);
			if ( !isMandatory ) continue;
			// when the group is mandatory, check if it present and eventually add it
			if ( !project.groupExists(groupPath+"/"+aGroup ) ) {
				project.createGroup( groupPath+"/"+aGroup );
				// check if type parameter is needed
				const QString className = d.componentType();
				if ( className != QString() ) {
					project.createParameter( groupPath+"/"+aGroup, "type", className );
				}
			}
		}

		//--- call recursively on all subgroups
		foreach( QString aGroup, project.getGroupsList( groupPath ) ) {
			checkConfigurationManager( aGroup, groupPath );
		}
	} catch (ParameterOrSubgroupNotDescribedException&) {
	} catch (ClassNameNotRegisteredException&) {
	}
}

void ProjectManager::checkOnParameterChanges( QString param, QString /*value*/, QString /*prefix*/, farsa::ConfigurationManager* confs ) {
	//--- if the slot is regarding something different the configuration parameters setted on this object
	//    it will do nothing
	if ( confs != &project ) return;
	//--- handle only type parameter
	if ( param.toLower() == "type" ) {
		// in this case, check all the configuration parameters and groups and update information
		checkConfigurationManager( project.getValue( "TOTAL99/mainComponent" ), "/" );
		projectGroupsTree->updateGroupsInformation();
	}
	return;
}

void ProjectManager::checkOnGroupAdding( QString Subgroup, QString prefix, farsa::ConfigurationManager* confs ) {
	//--- if the slot is regarding something different the configuration parameters setted on this object
	//    it will do nothing
	if ( confs != &project ) return;
	checkConfigurationManager( project.getValue( "TOTAL99/mainComponent" ), "/" );
	projectGroupsTree->addNewGroup( Subgroup, prefix );
}

void ProjectManager::updateStatusLabels() {
	if ( !component ) {
		projectComLabel->setText( "Component NOT Created yet" );
	} else {
		QString expstatus = "Component Created";
		if ( componentOutdated ) {
			expstatus.append( " - Need Reconfiguration" );
		}
#warning QUALCOSA DI SIMILE VA FATTO DOPO AVER CAMBIATO ComponentUI, VEDI WARNING IN baseexperiment.h
// 		expstatus.append( " - Status: "+component->status() );
		projectComLabel->setText( expstatus );
//		Logger::info( "Total99 - Component Status: "+component->status() );
	}
}

void ProjectManager::showViewerFromQAction() {
	QAction* action = dynamic_cast<QAction*>( sender() );
	if ( !action ) return;
	if ( viewersMap.contains( action ) ) {
		QWidget* viewer = viewersMap[action];
		//QRect viewFrame = viewer->frameGeometry();
		//QRegion reg( viewFrame );
		//reg.translate( -viewFrame.topLeft() );
		//viewer->setMask( reg );
		viewer->show();
		viewer->activateWindow();
	}
}

void ProjectManager::changePlugin() {
	QListWidgetItem* item = projectPluginsEdit->currentItem();
	QString newp = QFileDialog::getOpenFileName( this, "Select a plugin", QString(), "FARSA Plugin (*.so *.dylib *.dll)" );
	if ( newp.isEmpty() ) return;
	item->setText( QDir::current().relativeFilePath( newp ) );
	// change the configuration accordingly
	changeProjectPluginInfo();
}

void ProjectManager::changeSystemPlugin(const QString& pluginName) {
	QListWidgetItem* item = projectPluginsEdit->currentItem();
	item->setText(pluginName);
	// change the configuration accordingly
	changeProjectPluginInfo();
}

void ProjectManager::addNewPlugin() {
	QString newp = QFileDialog::getOpenFileName( this, "Select a plugin", QString(), "FARSA Plugin (*.so *.dylib *.dll)" );
	if ( newp.isEmpty() ) return;
	/*QListWidgetItem* item =*/ new QListWidgetItem( QDir::current().relativeFilePath( newp ), projectPluginsEdit );
	// change the configuration accordingly
	changeProjectPluginInfo();
}

void ProjectManager::addSystemPlugin(const QString& pluginName) {
	new QListWidgetItem(pluginName, projectPluginsEdit);
	// change the configuration accordingly
	changeProjectPluginInfo();
}

void ProjectManager::removePlugin() {
	QListWidgetItem* item = projectPluginsEdit->currentItem();
	if ( QMessageBox::question( this, "Deleting a Plugin", QString("Are you sure to delete the plugin %1 from the list of plugins ??").arg( item->text() ), QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes ) {
		return;
	}
	// !! OK DELETING !!
	delete item;
	// change the configuration accordingly
	changeProjectPluginInfo();
}

void ProjectManager::contextMenuPlugin(const QPoint& pos) {
	QListWidgetItem* item = projectPluginsEdit->itemAt( pos );
	QMenu menu(this);

	// Generating the list of system plugins
	QStringList systemPlugins;
	QDir systemPluginsDir(Total99Resources::pluginBasePath);
	const QString pluginSuffix = "_farsaPlugin" + Total99Resources::pluginSuffix;
	foreach (QString pluginFile, systemPluginsDir.entryList()) {
		if (!QLibrary::isLibrary(pluginFile)) {
			continue;
		}

		// Checks that the file has the right suffix
		if (pluginFile.endsWith(pluginSuffix)) {
			systemPlugins.append(pluginFile.left(pluginFile.length() - pluginSuffix.length()));
		}
	}

	// Generating menus
	menu.addAction( "Add New Plugin", this, SLOT(addNewPlugin()) );
	QMenu* systemPluginsMenu = menu.addMenu( "Add System Plugin" );
	foreach(QString p, systemPlugins) {
		QAction* a = systemPluginsMenu->addAction(p, systemPluginsAddActionsMapper, SLOT(map()));
		systemPluginsAddActionsMapper->setMapping(a, p);
	}
	if ( item ) {
		projectPluginsEdit->setCurrentItem( item );
		menu.addAction( "Edit", this, SLOT(changePlugin()) );
		QMenu* systemPluginsMenu = menu.addMenu( "Change To System Plugin" );
		foreach(QString p, systemPlugins) {
			QAction* a = systemPluginsMenu->addAction(p, systemPluginsChangeActionsMapper, SLOT(map()));
			systemPluginsChangeActionsMapper->setMapping(a, p);
		}
		menu.addAction( "Remove Plugin", this, SLOT(removePlugin()) );
	}
	menu.exec( projectPluginsEdit->mapToGlobal(pos) );
}

void ProjectManager::changePluginPath() {
	QListWidgetItem* item = projectPPathsEdit->currentItem();
	QString newdir = QFileDialog::getExistingDirectory( this, "Select a path for plugins" );
	if ( newdir.isEmpty() ) return;
	if ( newdir == QDir::current().absolutePath() ) {
		item->setText( "." );
	} else {
		item->setText( QDir::current().relativeFilePath( newdir ) );
	}
	// change the configuration accordlying
	changeProjectPluginInfo();
}

void ProjectManager::addNewPluginPath() {
	QString newdir = QFileDialog::getExistingDirectory( this, "Select a path for plugins" );
	if ( newdir.isEmpty() ) return;
	QString path;
	if ( newdir == QDir::current().absolutePath() ) {
		path = ".";
	} else {
		path = QDir::current().relativeFilePath( newdir );
	}
	/*QListWidgetItem* item =*/ new QListWidgetItem( QApplication::style()->standardIcon(QStyle::SP_DirIcon), path, projectPPathsEdit );
	// change the configuration accordlying
	changeProjectPluginInfo();
}

void ProjectManager::removePluginPath() {
	QListWidgetItem* item = projectPPathsEdit->currentItem();
	if ( QMessageBox::question( this, "Deleting a Plugin Path", QString("Are you sure to delete the path %1 from the list of plugin paths ??").arg( item->text() ), QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes ) {
		return;
	}
	// !! OK DELETING !!
	delete item;
	// change the configuration accordlying
	changeProjectPluginInfo();
}

void ProjectManager::contextMenuPluginPath(const QPoint& pos) {
	QListWidgetItem* item = projectPPathsEdit->itemAt( pos );
	QMenu menu(this);
	menu.addAction( "Add New Path", this, SLOT(addNewPluginPath()) );
	if ( item ) {
		projectPPathsEdit->setCurrentItem( item );
		menu.addAction( "Edit", this, SLOT(changePluginPath()) );
		menu.addAction( "Remove Path", this, SLOT(removePluginPath()) );
	}
	menu.exec( projectPPathsEdit->mapToGlobal(pos) );
}

void ProjectManager::addRecentProjectToList( QStringList aRecentProject, bool onTop ) {
	QListWidgetItem* item = new QListWidgetItem( QIcon(Total99Resources::findResource("projectIcon.png")), aRecentProject[0] );
	item->setToolTip( aRecentProject[4] + "\nAuthor: " + aRecentProject[2] + " Date: " + aRecentProject[3] + "\nFile: " + aRecentProject[1] );
	// FOR FUTURE: see the setItemWidget for a future styling with a custom QWidget for display the item
	// --- now it use the stylesheet for customizing the aspect
	if ( onTop ) {
		recentProjectsList->insertItem( 0, item );
	} else {
		recentProjectsList->addItem( item );
	}
}

bool ProjectManager::loadProjectConfiguration( ConfigurationManager& newp ) {
	//--- check TOTAL99 group and mandatory parameters
	if ( ! newp.groupExists("TOTAL99" ) ) {
		QMessageBox::warning( this, QString("Error loading Project"), QString("The TOTAL99 group is not present into the project file. This group is mandatory.") );
		return false;
	}
	QString pname = newp.parameterExists("TOTAL99/name") ? newp.getValue("TOTAL99/name") : "";
	QString pauthor = newp.parameterExists("TOTAL99/author") ? newp.getValue("TOTAL99/author") : "";
	QString pdate = newp.parameterExists("TOTAL99/date") ? newp.getValue("TOTAL99/date") : "";
	QString pdescr = newp.parameterExists("TOTAL99/description") ? newp.getValue("TOTAL99/description") : "";
	QString pexpg = newp.parameterExists("TOTAL99/mainComponent") ? newp.getValue("TOTAL99/mainComponent") : "";
	if ( pname.isEmpty() ) {
		QMessageBox::warning( this, QString("Error loading Project"), QString("The parameter TOTAL99/name is not present into the project file. This parameter is mandatory.") );
		return false;
	}
	if ( pexpg.isEmpty() ) {
		QMessageBox::warning( this, QString("Error loading Project"), QString("The parameter TOTAL99/mainComponent is not present into the project file. This parameter is mandatory.") );
		return false;
	}
	if ( ! newp.groupExists( pexpg ) ) {
		QMessageBox::warning( this, QString("Error loading Project"), QString("The parameter TOTAL99/mainComponent specify a group that it is not present into the project file.") );
		return false;
	}

	project = newp;
	checkConfigurationManager( pexpg, "/" );
	projectLoaded = true;
	projectChanged = false;
	componentOutdated = true;
	projectGroupsTree->setConfigurationManager( &project );
	projectNameEdit->setText( pname );
	projectAuthorEdit->setText( pauthor );
	projectPluginsEdit->clear();
	QStringList pluginFiles = project.getParametersWithPrefixList( "TOTAL99", "pluginFile" );
	foreach( QString plugin, pluginFiles ) {
		QString value = project.getValue( "TOTAL99/"+plugin );
		new QListWidgetItem( value, projectPluginsEdit );
	}
	projectPPathsEdit->clear();
	QStringList pluginPaths = project.getParametersWithPrefixList( "TOTAL99", "pluginPath" );
	foreach( QString path, pluginPaths ) {
		QString value = project.getValue( "TOTAL99/"+path );
		new QListWidgetItem( QApplication::style()->standardIcon(QStyle::SP_DirIcon), value, projectPPathsEdit );
	}
	int index = 2;
	if (project.parameterExists("TOTAL99/logLevel")) {
		index = projectLogLevelEdit->findText( project.getValue("TOTAL99/logLevel") );
	}
	if ( index < 0 ) {
		index = 2;
	}
	projectLogLevelEdit->setCurrentIndex( index );
	projectDateLabel->setText( QString("Last Modified ")+pdate );
	projectDescrEdit->blockSignals( true );
	projectDescrEdit->setPlainText( pdescr );
	projectDescrEdit->blockSignals( false );
	mainLay->setCurrentIndex( 1 );
	updateStatusLabels();
	return true;
}

void ProjectManager::updateRecentProjects() {
	// --- update the recent project loaded
	// 0 -> name as reported in the project file
	// 1 -> full path of the project's filename
	// 2 -> author as reported in the project file
	// 3 -> date of the last access to the file
	// 4 -> comment as reported in the project file
	bool addIt = true;
	QString pname = project.parameterExists("TOTAL99/name") ? project.getValue("TOTAL99/name") : "";
	QString pauthor = project.parameterExists("TOTAL99/author") ? project.getValue("TOTAL99/author") : "";
	QString pdate = project.parameterExists("TOTAL99/date") ? project.getValue("TOTAL99/date") : "";
	QString pdescr = project.parameterExists("TOTAL99/description") ? project.getValue("TOTAL99/description") : "";
	QString pfile = projectFile.absoluteFilePath();
	QStringList newrecent;
	newrecent << pname << pfile << pauthor << pdate << pdescr;
	for( int i=0; i<recentProjects.size(); i++ ) {
		if ( recentProjects[i][1] == pfile ) {
			//--- update informations already into recent project
			addIt = false;
			recentProjects[i] = newrecent;
			recentProjects.move( i, 0 );
			recentProjectsList->takeItem( i );
			addRecentProjectToList( newrecent );
			break;
		}
	}
	if ( addIt ) {
		recentProjects.prepend( newrecent );
		addRecentProjectToList( recentProjects.first() );
	}
}

void ProjectManager::loadProjectFromFile( QString configurationFileName ) {
	//--- load the file
	ConfigurationManager newp;
	observe(&newp);
	if ( ! newp.loadParameters( configurationFileName ) ) {
		QMessageBox::warning( this, QString("Error loading Project"), QString("An error occured during the loading of the project ") + configurationFileName + QString("\nCheck if the file contains a valid project configuration") );
		return;
	}
	//--- load the configuration
	if ( loadProjectConfiguration( newp ) ) {
		projectFile = QFileInfo( configurationFileName );
		QDir::setCurrent( projectFile.absolutePath() );
		//--- load any plugin found into the directories specified by the pluginPath parameter
		Total99Resources::loadPlugins( newp );
		// --- we need to update all information of groups after loading plugins
		//     some type in typeDescriptions may be changed
		projectGroupsTree->updateGroupsInformation();
		projectGroupEditor->reloadGroup();
		Logger::setLogLevel(Logger::stringToLogLevel(ConfigurationHelper::getString(newp, "TOTAL99/logLevel", Logger::logLevelToString(Logger::Quiet))));
		updateRecentProjects();
	}
	updateActionsState();
	updateStatusLabels();
}

bool ProjectManager::copyPluginData(const QString& relPluginPath, const QString& destPath, bool copyContent)
{
	if (copyContent) {
		QDir sourceDir(Total99Resources::pluginConfigBasePath + "/" + relPluginPath);
		QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
		foreach (const QString &fileName, fileNames) {
			const QString sourceFilePath = sourceDir.absolutePath() + "/" + fileName;
			const QString destFilePath = destPath + "/" + fileName;
			if (QFileInfo(sourceFilePath).isDir()) {
				if (!copyPluginData(relPluginPath + "/" + fileName, destPath, false)) {
					return false;
				}
			} else {
				QFile::remove(destFilePath);
				if (!QFile::copy(sourceFilePath, destFilePath)) {
					return false;
				}
			}
		}
	} else {
		// Creating a directory in destDir and calling self recursively
		QDir destDir(destPath);
		if (!destDir.mkpath(relPluginPath)) {
			return false;
		}
		copyPluginData(relPluginPath, destPath + "/" + relPluginPath, true);
	}

	return true;
}

void ProjectManager::closeEvent(QCloseEvent* event)
{
	delete batchInstancesManager;
	batchInstancesManager = NULL;

	QWidget::closeEvent(event);
}
