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

#include "batchinstancesmanager.h"
#include <cassert>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QMargins>
#include "configurationhelper.h"

namespace __BatchInstancesManager_internal {
	BatchProcess::BatchProcess(QString confFilePath, QString action, QStringList options, QWidget* parent)
		: QWidget(parent)
		, m_confFilePath(confFilePath)
		, m_action(action)
		, m_options(options)
		, m_process(new QProcess(this))
		, m_log(NULL)
		, m_statusLabel(NULL)
		, m_terminationButton(NULL)
	{
		// The main layout
		QGridLayout* mainLayout = new QGridLayout(this);

		// The label and read-only line edit with the commandline
		QHBoxLayout* l = new QHBoxLayout();
		l->setContentsMargins(0, 0, 0, 0);
		mainLayout->addLayout(l, 0, 0, 1, 2);

		QLabel* a = new QLabel("Commandline:", this);
		l->addWidget(a);

		QLineEdit* cmdLineLabel = new QLineEdit(this);
		cmdLineLabel->setReadOnly(true);
		l->addWidget(cmdLineLabel);

		// The editbox with the log
		m_log = new QTextEdit(this);
		m_log->setObjectName("logViewer");
		m_log->setReadOnly(true);
		mainLayout->addWidget(m_log, 1, 0, 1, 2);

		// The label with the status of the process
		m_statusLabel = new QLabel("Status: starting", this);
		mainLayout->addWidget(m_statusLabel, 2, 0);

		// The button to terminate the execution of the process early
		m_terminationButton = new QPushButton("Terminate instance", this);
		m_terminationButton->setEnabled(false);
		mainLayout->addWidget(m_terminationButton, 2, 1);
		connect(m_terminationButton, SIGNAL(clicked()), this, SLOT(askAndTerminate()));

		// Connecting all signals from m_process
		connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
		connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
		connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(processReadyReadStandardError()));
		connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processReadyReadStandardOutput()));
		connect(m_process, SIGNAL(started()), this, SLOT(processStarted()));

		// Starting process
		const QStringList arguments = QStringList() << "--batch" << ("--file=" + m_confFilePath) << ("--action=" + m_action) << m_options;
		m_process->start(QCoreApplication::applicationFilePath(), arguments);

		// Printing commandline
		QString cmdLine = "\"" + QCoreApplication::applicationFilePath() + "\"";
		for (int i = 0; i < arguments.size(); ++i) {
			cmdLine += " \"" + arguments[i] + "\"";
		}
		cmdLineLabel->setText(cmdLine);
	}

	BatchProcess::~BatchProcess()
	{
	}

	bool BatchProcess::isRunning() const
	{
		return (m_process->state() == QProcess::Starting) || (m_process->state() == QProcess::Running);

	}

	void BatchProcess::terminate()
	{
		m_process->terminate();
	}

	void BatchProcess::waitForFinished(unsigned int delay)
	{
		m_process->waitForFinished(delay);
	}

	void BatchProcess::kill()
	{
		m_process->kill();
	}

	void BatchProcess::askAndTerminate()
	{
		if (!isRunning()) {
			return;
		}

		// Asking the user if he is sure he wants to terminate the process
		if (QMessageBox::question(this, "Are you sure?", "Do you really want to terminate the process?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes) {
			return;
		}

		// Terminating the process and waiting at most a couple of seconds
		terminate();
		waitForFinished(2000);

		if (isRunning()) {
			// The process is still running, asking if we have to kill it
			if (QMessageBox::question(this, "Process still alive", "The instance is still alive. Do you want to kill it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
				kill();
			}
		}
	}

	void BatchProcess::processError(QProcess::ProcessError error)
	{
		QString status;
		if (error == QProcess::FailedToStart) {
			status = "program failed to start";
		} else if (error == QProcess::Crashed) {
			status = "program crashed";
		} else if (error == QProcess::UnknownError) {
			status = "unknown error";
		}
		m_statusLabel->setText("Status: " + status);
	}

	void BatchProcess::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
	{
		m_terminationButton->setEnabled(false);

		QString status = "program finished with exit code " + QString::number(exitCode);
		if (exitStatus == QProcess::CrashExit) {
			status += " - program crashed";
		}
		m_statusLabel->setText("Status: " + status);
	}

	void BatchProcess::processReadyReadStandardError()
	{
		// Reading all available data
		const QString data = m_process->readAllStandardError();

		// Printing it
		m_log->append(QString("<pre style=\"margin-top: 0px; margin-bottom: 0px; color: #ff4500\">%1</pre>").arg(data));
		m_log->moveCursor(QTextCursor::End);
		m_log->moveCursor(QTextCursor::StartOfLine);
	}

	void BatchProcess::processReadyReadStandardOutput()
	{
		// Reading all available data
		const QString data = m_process->readAllStandardOutput();

		// Printing it
		m_log->append(QString("<pre style=\"margin-top: 0px; margin-bottom: 0px; color: #afeeee\">%1</pre>").arg(data));
		m_log->moveCursor(QTextCursor::End);
		m_log->moveCursor(QTextCursor::StartOfLine);
	}

	void BatchProcess::processStarted()
	{
		m_terminationButton->setEnabled(true);

		m_statusLabel->setText("Status: running");
	}
}

BatchInstancesManager::BatchInstancesManager(salsa::ConfigurationManager& params, QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, m_configurationParameters(params)
	, m_instanceName(NULL)
	, m_confFilePathEdit(NULL)
	, m_actionEdit(NULL)
	, m_optionsEdit(NULL)
	, m_instancesTabs(NULL)
	, m_startNewInstanceButton(NULL)
	, m_removeCurrentInstanceButton(NULL)
	, m_removeAllInstancesButton(NULL)
	, m_instanceIndex(0)
{
	setWindowTitle("Total99 - Batch instances manager");

	// Creating all widgets

	// The main layout
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// The container with fields for a new instance
	{
		QGroupBox* g = new QGroupBox("New instance parameters", this);
		mainLayout->addWidget(g);

		// The layout of the container
		QGridLayout* l = new QGridLayout(g);

		// The label and editbox with the new instance name
		QLabel* a = new QLabel("Instance name:", g);
		l->addWidget(a, 0, 0);
		m_instanceName = new QLineEdit(g);
		l->addWidget(m_instanceName, 0, 1, 1, 2);
		m_instanceName->setText("Instance " + QString::number(m_instanceIndex));

		// The label, editbox and button to select the configuration file
		a = new QLabel("Configuration file:", g);
		l->addWidget(a, 1, 0);
		m_confFilePathEdit = new QLineEdit(g);
		l->addWidget(m_confFilePathEdit, 1, 1);
		QPushButton* b = new QPushButton("Choose file", g);
		l->addWidget(b, 1, 2);
		connect(b, SIGNAL(clicked()), this, SLOT(chooseConfigurationFile()));

		// The label and editbox for the action
		a = new QLabel("Action to run:", g);
		l->addWidget(a, 2, 0);
		m_actionEdit = new QLineEdit(g);
		l->addWidget(m_actionEdit, 2, 1, 1, 2);

		// The label and editbox for the options
		a = new QLabel("Additional options:", g);
		l->addWidget(a, 3, 0);
		m_optionsEdit = new QLineEdit(g);
		l->addWidget(m_optionsEdit, 3, 1, 1, 2);
	}

	// The container with buttons
	{
		// The layout for this container
		QHBoxLayout* l = new QHBoxLayout();
		QMargins m = l->contentsMargins();
		m.setLeft(0);
		m.setRight(0);
		l->setContentsMargins(m);
		mainLayout->addLayout(l);

		// The buttons: start new instance, ...
		m_startNewInstanceButton = new QPushButton("Run new instance", this);
		l->addWidget(m_startNewInstanceButton);
		connect(m_startNewInstanceButton, SIGNAL(clicked()), this, SLOT(startNewInstace()));

		// ... close current instance, ...
		m_removeCurrentInstanceButton = new QPushButton("Close current instance", this);
		m_removeCurrentInstanceButton->setEnabled(false);
		l->addWidget(m_removeCurrentInstanceButton);
		connect(m_removeCurrentInstanceButton, SIGNAL(clicked()), this, SLOT(removeCurrentInstance()));

		// ... close all instances
		m_removeAllInstancesButton = new QPushButton("Close all instances", this);
		m_removeAllInstancesButton->setEnabled(false);
		l->addWidget(m_removeAllInstancesButton);
		connect(m_removeAllInstancesButton, SIGNAL(clicked()), this, SLOT(removeAllInstances()));
	}

	// The tab widget with information about the running/finished instances
	{
		m_instancesTabs = new QTabWidget(this);
		mainLayout->addWidget(m_instancesTabs);
	}

	// Restoring our status if present on the configuration parameters object
	QString str = salsa::ConfigurationHelper::getString(m_configurationParameters, "BatchInstancesManager/geometry", "");
	if (!str.isEmpty()) {
		restoreGeometry(QByteArray::fromBase64(str.toLatin1()));
	}
	bool wasVisible = salsa::ConfigurationHelper::getBool(m_configurationParameters, "BatchInstancesManager/visible", false);
	if (wasVisible) {
		show();
	}
}

BatchInstancesManager::~BatchInstancesManager()
{
	// Terminating all instances
	terminateAll();

	// Saving our geometry
	if (!m_configurationParameters.groupExists("BatchInstancesManager")) {
		m_configurationParameters.createGroup("BatchInstancesManager");
	}
	if (m_configurationParameters.parameterExists("BatchInstancesManager/geometry")) {
		m_configurationParameters.setValue("BatchInstancesManager/geometry", saveGeometry().toBase64());
	} else {
		m_configurationParameters.createParameter("BatchInstancesManager", "geometry", saveGeometry().toBase64());
	}
	if (m_configurationParameters.parameterExists("BatchInstancesManager/visible")) {
		m_configurationParameters.setValue("BatchInstancesManager/visible", (isVisible() ? "true" : "false"));
	} else {
		m_configurationParameters.createParameter("BatchInstancesManager", "visible", (isVisible() ? "true" : "false"));
	}
}

void BatchInstancesManager::terminateAll()
{
	// Getting the list of instances to terminate and terminating them
	QList<__BatchInstancesManager_internal::BatchProcess*> processes;
	for (int i = 0; i < m_instancesTabs->count(); ++i) {
		__BatchInstancesManager_internal::BatchProcess* curProcess = dynamic_cast<__BatchInstancesManager_internal::BatchProcess*>(m_instancesTabs->widget(i));

		// Safety check
		assert(curProcess != NULL);

		processes.append(curProcess);

		if (curProcess->isRunning()) {
			curProcess->terminate();
		}
	}

	// Waiting for them to actually terminate, for at most two seconds
	QElapsedTimer timer;
	timer.start();
	for (int i = 0; i < processes.size(); ++i) {
		if (processes[i]->isRunning()) {
			processes[i]->waitForFinished(2000);;
		}

		if (timer.elapsed() > 2000) {
			return;
		}
	}

	// Killing instances that are still running
	for (int i = 0; i < processes.size(); ++i) {
		if (processes[i]->isRunning()) {
			processes[i]->kill();
		}
	}

	// Removing all tabs and deleting all instances
	m_instancesTabs->clear();
	for (int i = 0; i < processes.size(); ++i) {
		delete processes[i];
	}

	// Disabling buttons to remove instances
	m_removeCurrentInstanceButton->setEnabled(false);
	m_removeAllInstancesButton->setEnabled(false);
}

void BatchInstancesManager::chooseConfigurationFile()
{
	// Ask the user which file to load
	QString confFilePath = QFileDialog::getOpenFileName(this, "Select configuration file", QString(), "SALSA Configuration file (*.ini *.xml)");

	// Filling the m_confFilePathEdit with the selected file
	if (!confFilePath.isEmpty()) {
		m_confFilePathEdit->setText(confFilePath);
	}
}

void BatchInstancesManager::startNewInstace()
{
	// Creating the new batch instance
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI PARSARE MEGLIO LE OPZIONI, NON BASTA SPLIT (ci sono problemi nel caso di opzioni che contengono spazi)
#endif
	__BatchInstancesManager_internal::BatchProcess* newProcess = new __BatchInstancesManager_internal::BatchProcess(m_confFilePathEdit->text(), m_actionEdit->text(), m_optionsEdit->text().split(" ", QString::SkipEmptyParts), this);

	// Creating the tab and showing it
	int tabIndex = m_instancesTabs->addTab(newProcess, m_instanceName->text());
	m_instancesTabs->setCurrentIndex(tabIndex);

	// Setting the new instance name
	++m_instanceIndex;
	m_instanceName->setText("Instance " + QString::number(m_instanceIndex));

	// Enabling buttons to remove instances
	m_removeCurrentInstanceButton->setEnabled(true);
	m_removeAllInstancesButton->setEnabled(true);
}

void BatchInstancesManager::removeCurrentInstance()
{
	__BatchInstancesManager_internal::BatchProcess* curProcess = dynamic_cast<__BatchInstancesManager_internal::BatchProcess*>(m_instancesTabs->currentWidget());

	// Safety check
	assert(curProcess != NULL);

	// If the instance is running, asking the user
	if (curProcess->isRunning()) {
		if (QMessageBox::question(this, "Instance still running", "The current instance is still running. Are you sure you want to close it? Note that it will be killed if it doesn't terminate within two seconds", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes) {
			return;
		}

		curProcess->terminate();
		curProcess->waitForFinished(2000);
		if (curProcess->isRunning()) {
			curProcess->kill();
		}
	}

	// Removing the current instance and deleting it
	m_instancesTabs->removeTab(m_instancesTabs->currentIndex());
	delete curProcess;

	// Disabling buttons to remove instances if no instance is present
	if (m_instancesTabs->count() == 0) {
		m_removeCurrentInstanceButton->setEnabled(false);
		m_removeAllInstancesButton->setEnabled(false);
	}
}

void BatchInstancesManager::removeAllInstances()
{
	// Checking if any instance is still running. We also get the list of instances to delete them later
	QList<__BatchInstancesManager_internal::BatchProcess*> processes;
	bool someRunning = false;
	for (int i = 0; i < m_instancesTabs->count(); ++i) {
		__BatchInstancesManager_internal::BatchProcess* curProcess = dynamic_cast<__BatchInstancesManager_internal::BatchProcess*>(m_instancesTabs->widget(i));

		// Safety check
		assert(curProcess != NULL);

		if (curProcess->isRunning()) {
			someRunning = true;
		}

		processes.append(curProcess);
	}

	if (someRunning) {
		if (QMessageBox::question(this, "Some instances still running", "Some instances are still running. Are you sure you want to close them? Note that they will be killed if they don't terminate within two seconds", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes) {
			return;
		}

		// Telling all instances to terminate
		for (int i = 0; i < processes.size(); ++i) {
			if (processes[i]->isRunning()) {
				processes[i]->terminate();
			}
		}

		// Waiting for them to actually terminate, for at most two seconds
		QElapsedTimer timer;
		timer.start();
		for (int i = 0; i < processes.size(); ++i) {
			if (processes[i]->isRunning()) {
				processes[i]->waitForFinished(2000);;
			}

			if (timer.elapsed() > 2000) {
				return;
			}
		}

		// Killing instances that are still running
		for (int i = 0; i < processes.size(); ++i) {
			if (processes[i]->isRunning()) {
				processes[i]->kill();
			}
		}
	}

	// Removing all tabs and deleting all instances
	m_instancesTabs->clear();
	for (int i = 0; i < processes.size(); ++i) {
		delete processes[i];
	}

	// Disabling buttons to remove instances
	m_removeCurrentInstanceButton->setEnabled(false);
	m_removeAllInstancesButton->setEnabled(false);
}
