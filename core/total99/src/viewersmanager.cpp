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

#include "viewersmanager.h"
#include "projectmanager.h"
#include "total99resources.h"
#include "logger.h"
#include "configurationhelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizeGrip>
#include <QToolButton>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QRegExp>

ViewerWidget::ViewerWidget(QString title, QString group, QWidget* central, ViewersManager* manager, QWidget* /*parent*/) :
	QFrame(nullptr),
	m_central(central),
	m_manager(manager),
	m_title(title),
	m_group(group)
{
	setAttribute(Qt::WA_QuitOnClose , false);
	setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	setWindowTitle(title);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	setContentsMargins(2, 2, 2, 2);

	m_central->setParent(this);
	layout->addWidget(m_central, 6);

	m_manager->addViewer(this);
}

ViewerWidget::~ViewerWidget()
{
}

QString ViewerWidget::getTitle() const
{
	return m_title;
}

QString ViewerWidget::getGroup() const
{
	return m_group;
}

ViewersManager::ViewersManager(ProjectManager* project, salsa::ConfigurationManager& params, QString prefix) :
	QObject(),
	m_project(project),
	m_viewers(),
	m_viewersStatusFiles(),
	m_viewersStatusHashes(),
	m_currentExpPath(),
	m_currentExpHash(),
	m_currentViewersStateFile(),
	m_currentViewersState()
{
	// Reading from the configuration file all the associations between configuration files paths and hashes
	// and viewers status files
	const QString filesGroup = prefix + "ViewersManager/Files/";
	if (params.groupExists(filesGroup)) {
		QStringList allParams = params.getParametersList(filesGroup);
		foreach (QString p, allParams) {
			m_viewersStatusFiles[params.getValue(filesGroup + p)] = p;
		}
	}

	const QString hashesGroup = prefix + "ViewersManager/Hashes/";
	if (params.groupExists(filesGroup)) {
		QStringList allParams = params.getParametersList(hashesGroup);
		foreach (QString p, allParams) {
			m_viewersStatusHashes[params.getValue(hashesGroup + p)] = p;
		}
	}

	// We also create the directory that will contain the viewers status if it is not present
	QDir userDir(salsa::Total99Resources::confUserPath);
	userDir.mkdir("viewersStatus");
}

ViewersManager::~ViewersManager()
{
}

void ViewersManager::addViewer(ViewerWidget* viewer)
{
	m_viewers.append(viewer);
}

void ViewersManager::clear()
{
	// Saving the status of widgets

	// We start with the status of the main window, then all the viewers
	if (!m_currentViewersState.groupExists("General")) {
		m_currentViewersState.createGroup("General");
	}
	if (!m_currentViewersState.parameterExists("General/total99Geometry")) {
		m_currentViewersState.createParameter("General", "total99Geometry");
	}
	m_currentViewersState.setValue("General/total99Geometry", m_project->saveGeometry().toBase64());

	// Saving the status of all viewers

	// This map is used to set the correct number after the GUI: group
	QMap<QString, int> numGUIGroups;
	foreach (ViewerWidget* viewer, m_viewers) {
		int guiGroupID = 0;
		if (numGUIGroups.contains(viewer->getGroup())) {
			numGUIGroups[viewer->getGroup()]++;
			guiGroupID = numGUIGroups[viewer->getGroup()];
		} else {
			numGUIGroups[viewer->getGroup()] = 0;
		}

		// Creating a group with the same name of the viewer group plus a GUI: subgroup
		const QString g = viewer->getGroup() + "/GUI:" + QString::number(guiGroupID);
		m_currentViewersState.createGroup(g);

		// Addig parameters to the new group
		m_currentViewersState.createParameter(g, "title", viewer->getTitle());
		m_currentViewersState.createParameter(g, "geometry", viewer->saveGeometry().toBase64());
		m_currentViewersState.createParameter(g, "visible", (viewer->isVisible() ? "true" : "false"));
	}

	// Saving to file
	m_currentViewersState.saveParameters(getViewersStatusFilePath(m_currentViewersStateFile));

	// Now clearing the list of viewers
	m_viewers.clear();
}

void ViewersManager::restoreViewersStatus(QString projectFilename)
{
	// We set the current viewers status file to the empty one. We only set it to an existing file if we
	// find a matching configuration file in m_viewersStatusFiles, otherwise we create a new one
	m_currentViewersStateFile = QString();

	// Storing the project filename and computing its hash
	m_currentExpPath = projectFilename;
	QFile projectFile(projectFilename);
	projectFile.open(QIODevice::ReadOnly);
	m_currentExpHash = QCryptographicHash::hash(projectFile.readAll(), QCryptographicHash::Sha1).toHex();

	QString viewersStatusFile;

	// We have to find the file with the viewers status. First checking if there is an association in the
	// m_viewersStatusFiles map
	if (m_viewersStatusFiles.contains(m_currentExpPath)) {
		viewersStatusFile = m_viewersStatusFiles[m_currentExpPath];
		m_currentViewersStateFile = viewersStatusFile;
	}

	// If we didn't find any file, trying with the hash
	if (viewersStatusFile.isEmpty() && m_viewersStatusHashes.contains(m_currentExpHash)) {
		viewersStatusFile = m_viewersStatusHashes[m_currentExpHash];
	}

	// Finally we use the viewers status with the nearest path
	if (viewersStatusFile.isEmpty()) {
		QStringList files = m_viewersStatusFiles.keys();

		QDir currentDir = QFileInfo(projectFilename).dir();
		while (!currentDir.isRoot()) {
			const QRegExp rx = QRegExp(QRegExp::escape(currentDir.absolutePath()) + ".*");
			const int i = files.indexOf(rx);

			if (i != -1) {
				viewersStatusFile = m_viewersStatusFiles[files[i]];

				break;
			}

			if (!currentDir.cdUp()) {
				break;
			}
		}
	}

	// Clearing the current status
	m_currentViewersState.clearAll();

	// If we have found a status file, loading it
	if (!viewersStatusFile.isEmpty()) {
		m_currentViewersState.loadParameters(getViewersStatusFilePath(viewersStatusFile), false);
	}

	// If the file with the current viewers configuration is empty, it means that we have to create a new file, even if
	// viewersStatusFile is not empty
	if (m_currentViewersStateFile.isEmpty()) {
		QDir viewersStatusDir(salsa::Total99Resources::confUserPath + "/viewersStatus");

		// Getting the list of all .ini files in ascending order so that we can add 1 to the number of
		// the last file and
		QStringList dirFiles = viewersStatusDir.entryList(QStringList() << "*.ini", QDir::Files | QDir::Readable, QDir::Name);

		unsigned int index = 0;
		if (dirFiles.size() != 0) {
			QString lastFileName = dirFiles.last();
			lastFileName.chop(4); // This removes the final .ini
			index = lastFileName.toUInt(nullptr, 16) + 1;
		}
		do {
			m_currentViewersStateFile = QString("%1").arg(index, 16, 16, QChar('0'));
			++index;
		} while (QFileInfo(m_currentViewersStateFile + ".ini").exists());
	}

	// Now actually restoring viewers status

	// The first thing is to restore the main window geometry
	const QString str = salsa::ConfigurationHelper::getString(m_currentViewersState, "General/total99Geometry", "");
	if (!str.isEmpty()) {
		m_project->restoreGeometry(QByteArray::fromBase64(str.toLatin1()));
	}

	// Now restoring the viewers status
	foreach (ViewerWidget* viewer, m_viewers) {
		if (!m_currentViewersState.groupExists(viewer->getGroup())) {
			continue;
		}

		QStringList guisForComponent = m_currentViewersState.getGroupsWithPrefixList(viewer->getGroup(), "GUI:");

		// Now searching a group for the current viewer by comparing titles
		foreach(QString guiGroup, guisForComponent) {
			const QString curGuiGroup = viewer->getGroup() + "/" + guiGroup + "/";
			QString title = salsa::ConfigurationHelper::getString(m_currentViewersState, curGuiGroup + "title", "");

			if (title == viewer->getTitle()) {
				// We have found the right group, restoring state
				const QString str = salsa::ConfigurationHelper::getString(m_currentViewersState, curGuiGroup + "geometry", "");
				if (!str.isEmpty()) {
					viewer->restoreGeometry(QByteArray::fromBase64(str.toLatin1()));
				}
				const bool visible = salsa::ConfigurationHelper::getBool(m_currentViewersState, curGuiGroup + "visible", false);
				viewer->setVisible(visible);
			}
		}
	}

	// Finally, we add to the status the current file and the hash. We have to add stuffs also to maps
	if (!m_currentViewersState.groupExists("General")) {
		m_currentViewersState.createGroup("General");
	}
	if (m_currentViewersState.parameterExists("General/configurationFile")) {
		m_currentViewersState.setValue("General/configurationFile", m_currentExpPath);
	} else {
		m_currentViewersState.createParameter("General", "configurationFile", m_currentExpPath);
	}
	if (m_currentViewersState.parameterExists("General/configurationFileHash")) {
		m_currentViewersState.setValue("General/configurationFileHash", m_currentExpHash);
	} else {
		m_currentViewersState.createParameter("General", "configurationFileHash", m_currentExpHash);
	}
	m_viewersStatusFiles[m_currentExpPath] = m_currentViewersStateFile;
	m_viewersStatusHashes[m_currentExpHash] = m_currentViewersStateFile;
}

void ViewersManager::save(salsa::ConfigurationManager& params, QString prefix)
{
	// Saving associations, first with files, then with hashes
	const QString filesGroup = prefix + "ViewersManager/Files";
	params.createGroup(filesGroup);
	for (QMap<QString, QString>::const_iterator i = m_viewersStatusFiles.constBegin(); i != m_viewersStatusFiles.constEnd(); ++i) {
		params.createParameter(filesGroup, i.value(), i.key());
	}

	const QString hashesGroup = prefix + "ViewersManager/Hashes";
	params.createGroup(hashesGroup);
	for (QMap<QString, QString>::const_iterator i = m_viewersStatusHashes.constBegin(); i != m_viewersStatusHashes.constEnd(); ++i) {
		params.createParameter(hashesGroup, i.value(), i.key());
	}
}

QString ViewersManager::getViewersStatusFilePath(QString statusFileName) const
{
	return salsa::Total99Resources::confUserPath + "/viewersStatus/" + statusFileName + ".ini";
}
