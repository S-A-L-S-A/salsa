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

#ifndef BATCHINSTANCESMANAGER_H
#define BATCHINSTANCESMANAGER_H

#include <QWidget>
#include <QProcess>
#include <QTextEdit>
#include <QTabWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "configurationmanager.h"

/**
 * \brief The namespace with helper classes for the BatchInstancesManager
 *
 * \internal
 */
namespace __BatchInstancesManager_internal {
	/**
	 * \brief The object which stores the batch process. This is also the
	 *        widget with information about the process (log, status, ...)
	 *
	 * \internal
	 */
	class BatchProcess : public QWidget
	{
		Q_OBJECT

	public:
		/**
		 * \brief Constructor
		 *
		 * \param confFilePath the path of the configuration file
		 * \param action the action to execute
		 * \param options additional command line options
		 * \param parent the parent widget
		 */
		BatchProcess(QString confFilePath, QString action, QStringList options, QWidget* parent = NULL);

		/**
		 * \brief Destructor
		 */
		~BatchProcess();

		/**
		 * \brief Returns true if the instance is running
		 */
		bool isRunning() const;

		/**
		 * \brief Terminates the process
		 */
		void terminate();

		/**
		 * \brief Waits until the instance terminates
		 *
		 * \param delay how much to wait at most
		 */
		void waitForFinished(unsigned int delay);

		/**
		 * \brief Kills the process (force quit)
		 */
		void kill();

	private slots:
		/**
		 * \brief Terminate the execution of the process if the user
		 *        confirms
		 */
		void askAndTerminate();

		/**
		 * \brief The slot called when an error occurs
		 *
		 * \param error the error code
		 */
		void processError(QProcess::ProcessError error);

		/**
		 * \brief The slot called when the process finishes
		 *
		 * \param exitCode the exit code of the program
		 * \param exitStatus the exit status of the program
		 */
		void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

		/**
		 * \brief The slot called when there is data on the standard
		 *        error stream of the process
		 */
		void processReadyReadStandardError();

		/**
		 * \brief The slot called when there is data on the standard
		 *        output stream of the process
		 */
		void processReadyReadStandardOutput();

		/**
		 * \brief The slot called when the process starts
		 */
		void processStarted();

	private:
		/**
		 * \brief The path of the configuration file
		 */
		const QString m_confFilePath;

		/**
		 * \brief The action to execute
		 */
		const QString m_action;

		/**
		 * \brief Additional command line options
		 */
		const QStringList m_options;

		/**
		 * \brief The process
		 */
		QProcess* const m_process;

		/**
		 * \brief The widget with the output of the process
		 */
		QTextEdit* m_log;

		/**
		 * \brief The status of the process
		 */
		QLabel* m_statusLabel;

		/**
		 * \brief The button to terminate the execution of the instance
		 */
		QPushButton* m_terminationButton;
	};
}

/**
 * \brief The window that manages instances of total99 started in batch mode
 */
class BatchInstancesManager : public QWidget
{
	Q_OBJECT
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameter object we use to restore
	 *               and save the status of the widget
	 * \param parent the parent widget
	 * \param flags the window flags
	 */
	BatchInstancesManager(farsa::ConfigurationManager& params, QWidget* parent = NULL, Qt::WindowFlags flags = 0);

	/**
	 * \brief Destructor
	 */
	~BatchInstancesManager();

	/**
	 * \brief Terminates all running instances
	 */
	void terminateAll();

private slots:
	/**
	 * \brief Lets the user select the configuration file to use
	 */
	void chooseConfigurationFile();

	/**
	 * \brief Starts a new batch instance
	 */
	void startNewInstace();

	/**
	 * \brief Removes the instance of the current tab
	 */
	void removeCurrentInstance();

	/**
	 * \brief Removes all instances
	 */
	void removeAllInstances();

private:
	/**
	 * \brief The configuration parameter object we use to restore and save
	 *        the status of the widget
	 */
	farsa::ConfigurationManager& m_configurationParameters;

	/**
	 * \brief The lineedit with the name of the instance
	 */
	QLineEdit* m_instanceName;

	/**
	 * \brief The lineedit with the path of the configuration file to use
	 *        for the next batch instance
	 */
	QLineEdit* m_confFilePathEdit;

	/**
	 * \brief The lineedit with the action to run in the next batch instance
	 */
	QLineEdit* m_actionEdit;

	/**
	 * \brief The lineedit with the additional options to use for the next
	 *        batch instance
	 */
	QLineEdit* m_optionsEdit;

	/**
	 * \brief The tab widget with information about the various batch
	 *        instances that are runnning or have finished
	 */
	QTabWidget* m_instancesTabs;

	/**
	 * \brief The button to create a new instance
	 */
	QPushButton* m_startNewInstanceButton;

	/**
	 * \brief The button to create a new instance
	 */
	QPushButton* m_removeCurrentInstanceButton;

	/**
	 * \brief The button to create a new instance
	 */
	QPushButton* m_removeAllInstancesButton;

	/**
	 * \brief An counter to generate the automatic instance name
	 */
	unsigned int m_instanceIndex;
};

#endif
