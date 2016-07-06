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

#ifndef VIEWERSMANAGER_H
#define VIEWERSMANAGER_H

#include "configurationmanager.h"
#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QByteArray>

class ProjectManager;
class ViewersManager;

/**
 * \brief ViewerWidget creates a custom decoration around the viewer
 */
class ViewerWidget : public QFrame
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param title the title of the window
	 * \param group the group, in the configuration parameters, of the
	 *              component that created the widget
	 * \param central the widget that is displayed by this window
	 * \param manager the viewers manager
	 * \param parent the parent widget
	 */
	ViewerWidget(QString title, QString group, QWidget* central, ViewersManager* manager, QWidget* parent = NULL);

	/**
	 * \brief Destructor
	 */
	~ViewerWidget();

	/**
	 * \brief Returns the title of the window
	 *
	 * \return the title of the window
	 */
	QString getTitle() const;

	/**
	 * \brief Returns the group, in the configuration parameters, of the
	 *        component that created the widget
	 *
	 * \return the group, in the configuration parameters, of the component
	 *         that created the widget
	 */
	QString getGroup() const;

private:
	/**
	 * \brief The widget that is displayed by this window
	 */
	QWidget* const m_central;

	/**
	 * \brief The viewers manager
	 */
	ViewersManager* const m_manager;

	/**
	 * \brief The title of the weindow
	 */
	const QString m_title;

	/**
	 * \brief The group, in the configuration parameters, of the component
	 *        that created the widget
	 */
	const QString m_group;
};


/**
 * \brief ViewersManager arranges and visualizes the viewers around the main
 *        project widget
 *
 * This class takes a ConfigurationParameters object and a path in the
 * constructor but it is not created with the factory.
 * This class saves and restores viewers status for viewers of the current
 * experiment. The status is associated to a configuration file. The status is
 * saved in a configuration file that has the following structure:
 *
 * [General]
 * total99Geometry = \<geometry\>
 * configurationFile = \<path\>
 * configurationFileHash = \<hash\>
 *
 * [PATH_TO_SUBGROUP1/GUI:1]
 * title = \<title\>
 * geometry = \<geometry\>
 * visible = true|false
 *
 * [PATH_TO_SUBGROUP1/GUI:2]
 * title = \<title\>
 * geometry = \<geometry\>
 * visible = true|false
 *
 * [PATH_TO_SUBGROUP2/GUI:1]
 * title = \<title\>
 * geometry = \<geometry\>
 * visible = true|false
 *
 * In the file \<geometry\> is the base64 representation of the QByteArray
 * obtained with QWidget::saveGeometry() (i.e. the geometry of the widget) which
 * is restored using QWidget::restoreGeometry(). total99Geometry is the geometry
 * of the main total99 window. General/configurationFileHash is the SHA1 hash of
 * the configuration file associated with this viewers status (used for lookup,
 * see below). PATH_TO_SUBGROUPN is the path of a subgroup of the configuration
 * file which is associated to a component having some widgets. For each widget
 * of this component there is a GUI:N subgroup which is associated to a widget.
 * The associated widget is the one having \<title\> as window title (this is
 * not used as group name as it could contain forbidded characters). Files with
 * viewers status are saved in the total99 user directory (to be precise, they
 * are in Total99Resources::confBasePath + "/viewersStatus/"). The name of the
 * file is \<unique-number\>.ini where \<unique-number\> is a hex number which
 * makes the file unique inside the viewersStatus directory. The total99 user
 * configuration file has a [ViewersManager/Files] group containing the
 * association between the numbers and the absolute path of the corresponding
 * experiment configuration file. There also is a [ViewersManager/Hashes] group
 * containing the association between the numbers and the hash of the
 * corresponding experiment configuration file. When an experiment is loaded the
 * search for the file with the widget status is done as follows:
 * 	- first a viewers status file associated with the absolute path of the
 * 	  experiment configuration file is looked for;
 * 	- if no viewers status file is found, the hash of the experiment
 * 	  configuration file is searched (this is useful if you copy or move the
 * 	  configuration file to a new location);
 * 	- if the hash is not found, the file path most similar to the path of
 * 	  the experiment configuration file is used. "Most similar" means the
 * 	  one that has the greater number of consecutive directories in common
 * 	  starting from the root.
 */
class ViewersManager : public QObject
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param project the project manager
	 * \param params the configuration parameters object
	 * \param prefix the prefix under which the ViewersManager group is
	 */
	ViewersManager(ProjectManager* project, farsa::ConfigurationManager& params, QString prefix);

	/**
	 * \brief Destructor
	 */
	~ViewersManager();

	/**
	 * \brief Adds a viewer
	 *
	 * \param viewer the viewer to add to the list
	 */
	void addViewer(ViewerWidget* viewer);

	/**
	 * \brief Clears the list of all viewers
	 *
	 * This function also stores the viewers status
	 * \warning this doesn't delete the viewers
	 */
	void clear();

	/**
	 * \brief Restores the status of all current viewers
	 *
	 * \param projectFilename the absolute path of the project file
	 */
	void restoreViewersStatus(QString projectFilename);

	/**
	 * \brief Saves the status of windows on the given configuration
	 *        parameters object
	 *
	 * \param params the configuration parameters object
	 * \param prefix the prefix under which the ViewersManager group is
	 */
	void save(farsa::ConfigurationManager& params, QString prefix);

private:
	/**
	 * \brief Returns the complete path of a viewers status files
	 *
	 * This function adds the directory and .ini to the given string
	 * \param statusFileName the name of the status file without directory
	 *                       and extension
	 * \return the full path to the given viewers status file
	 */
	QString getViewersStatusFilePath(QString statusFileName) const;

	/**
	 * \brief The project manager
	 */
	ProjectManager* const m_project;

	/**
	 * \brief The list of viewers
	 */
	QList<ViewerWidget*> m_viewers;

	/**
	 * \brief The map between configuration files paths and the viewers
	 *        status file
	 */
	QMap<QString, QString> m_viewersStatusFiles;

	/**
	 * \brief The map between configuration file hashes paths and the
	 *        viewers status file
	 */
	QMap<QString, QString> m_viewersStatusHashes;

	/**
	 * \brief The path of the current experiment configuration file
	 */
	QString m_currentExpPath;

	/**
	 * \brief The hash of the current experiment configuration file
	 */
	QString m_currentExpHash;

	/**
	 * \brief The name of the file with viewers state for the current
	 *        experiment
	 *
	 * This is the name of the file without the .ini extension and the
	 */
	QString m_currentViewersStateFile;

	/**
	 * \brief The configuration parameters object with the status of widgets
	 */
	farsa::ConfigurationManager m_currentViewersState;
};

#endif
