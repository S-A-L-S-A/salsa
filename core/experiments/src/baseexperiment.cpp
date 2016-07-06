// /********************************************************************************
//  *  FARSA                                                                       *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #include "baseexperiment.h"
// #include "baseexperimentgui.h"
//
// namespace farsa {
//
// namespace __BaseExperiment_internal {
// 	/**
// 	 * \brief The UI manager
// 	 *
// 	 * This simply calls functions in BaseExperiment. We have to provide a
// 	 * separate object as UI manager because it is deleted by total99
// 	 * \internal
// 	 */
// 	class BaseExperimentUIManager : public ParameterSettableUI
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 */
// 		BaseExperimentUIManager(BaseExperiment* experiment) :
// 			ParameterSettableUI(),
// 			m_experiment(experiment)
// 		{
// 		}
//
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~BaseExperimentUIManager()
// 		{
// 			// Nothing to do here
// 		}
//
// 		/**
// 		 * \brief Fills the menu "Actions" of Total99
// 		 *
// 		 * This simply calls the corresponding function of m_experiment
// 		 * \param actionsMenu the "Actions" menu in Total99
// 		 */
// 		virtual void fillActionsMenu(QMenu* actionsMenu)
// 		{
// 			m_experiment->fillActionsMenu(actionsMenu);
// 		}
//
// 		/**
// 		 * \brief Returns the list of viewers
// 		 *
// 		 * This simply calls the corresponding function of m_experiment
// 		 * \param parent the parent widget for all viewers created by this
// 		 *               function
// 		 * \param flags the window flags to specify when constructing the
// 		 *              widgets
// 		 * \return the list of widgets
// 		 */
// 		virtual QList<ParameterSettableUIViewer> getViewers(QWidget* parent, Qt::WindowFlags flags)
// 		{
// 			return m_experiment->getViewers(parent, flags);
// 		}
//
// 		/**
// 		 * \brief Adds additional menus to the menu bar of Total99
// 		 *
// 		 * This simply calls the corresponding function of m_experiment
// 		 * \param menuBar the menu bar of the Total99 application
// 		 */
// 		virtual void addAdditionalMenus(QMenuBar* menuBar)
// 		{
// 			m_experiment->addAdditionalMenus(menuBar);
// 		}
//
// 	private:
// 		/**
// 		 * \brief The experiment
// 		 */
// 		BaseExperiment* const m_experiment;
// 	};
// }
//
// BaseExperiment::Notifee::Notifee(BaseExperiment& experiment) :
// 	NewDatumNotifiable<__BaseExperiment_internal::OperationControl>(),
// 	m_experiment(experiment)
// {
// }
//
// BaseExperiment::Notifee::~Notifee()
// {
// }
//
// void BaseExperiment::Notifee::newDatumAvailable(DataDownloader<__BaseExperiment_internal::OperationControl>* downloader)
// {
// 	const __BaseExperiment_internal::OperationControl* d = downloader->downloadDatum();
//
// 	switch (d->action) {
// 		case __BaseExperiment_internal::OperationControl::StartOperation:
// 			m_experiment.runOperation(d->operationID);
// 			break;
// 		case __BaseExperiment_internal::OperationControl::StartOperationPaused:
// 			m_experiment.pause();
// 			m_experiment.runOperation(d->operationID);
// 			break;
// 		case __BaseExperiment_internal::OperationControl::StopOperation:
// 			m_experiment.stop();
// 			break;
// 		case __BaseExperiment_internal::OperationControl::PauseOperation:
// 			m_experiment.pause();
// 			break;
// 		case __BaseExperiment_internal::OperationControl::StepOperation:
// 			m_experiment.step();
// 			break;
// 		case __BaseExperiment_internal::OperationControl::ResumeOperation:
// 			m_experiment.resume();
// 			break;
// 		case __BaseExperiment_internal::OperationControl::ChangeInterval:
// 			m_experiment.changeInterval(d->interval, false);
// 			break;
// 	}
// }
//
// BaseExperiment::BaseExperimentFlowController::BaseExperimentFlowController(BaseExperiment* baseExperiment)
// 	: FlowController()
// 	, m_baseExperiment(baseExperiment)
// {
// }
//
// bool BaseExperiment::BaseExperimentFlowController::stop()
// {
// 	return m_baseExperiment->stopSimulation();
// }
//
// void BaseExperiment::BaseExperimentFlowController::pause()
// {
// 	m_baseExperiment->checkPause();
// }
//
// BaseExperiment::BaseExperiment()
// 	: Component()
// 	, ParameterSettableUI()
// 	, ThreadOperation()
// 	, FlowControlled()
// 	, m_operationsVector()
// 	, m_actionSignalsMapper(new QSignalMapper(NULL)) // parent is NULL because we take care of deleting this object by ourself
// 	, m_workerThread(new WorkerThread(NULL)) // parent is NULL because we take care of deleting this object by ourself
// 	, m_runningOperationID(-1)
// 	, m_batchRunning(false)
// 	, m_stop(false)
// 	, m_mutex()
// 	, m_waitCondition()
// 	, m_pause(false)
// 	, m_previousPauseStatus(false)
// 	, m_delay(0)
// 	, m_notifee(*this)
// 	, m_dataExchange(2, DataUploaderDownloader<__BaseExperiment_internal::OperationStatus, __BaseExperiment_internal::OperationControl>::OverrideOlder, &m_notifee)
// 	, m_flowController(this)
// {
// 	// Setting our flow controller
// 	setFlowController(&m_flowController);
//
// 	// Disabling the check of association before upload because we could never get associated (e.g. when running in batch)
// 	m_dataExchange.checkAssociationBeforeUpload(false);
//
// 	// We have to add the stop() operation
// 	addOperation("stopCurrentOperation", &BaseExperiment::stopCurrentOperation, false, false);
//
// 	// Connecting the mapped signal of m_actionSignalsMapper to our slot
// 	connect(m_actionSignalsMapper.get(), SIGNAL(mapped(int)), this, SLOT(runOperation(int)));
// 	connect(m_workerThread.get(), SIGNAL(exceptionDuringOperation(farsa::BaseException*)), this, SLOT(exceptionDuringOperation(farsa::BaseException*)), Qt::BlockingQueuedConnection);
// }
//
// BaseExperiment::~BaseExperiment()
// {
// 	// Stopping the thread
// 	m_workerThread->quit();
//
// 	// Removing all operation wrappers
// 	foreach (AbstractOperationWrapper* op, m_operationsVector) {
// 		delete op;
// 	}
// }
//
// void BaseExperiment::configure(ConfigurationParameters& params, QString)
// {
// 	// Reading whether we are running in batch mode or not. The parameter is only present if we are running in batch,
// 	// so we must use false as default value for m_batchRunning
// 	m_batchRunning = ConfigurationHelper::getBool(params, "__INTERNAL__/BatchRunning", false);
// }
//
// void BaseExperiment::save(ConfigurationParameters& params, QString prefix)
// {
// 	// This should always be called
// 	params.startObjectParameters(prefix, "BaseExperiment", this);
// }
//
// void BaseExperiment::describe(QString type)
// {
// 	Descriptor d = addTypeDescription(type, "The base class of experiments");
// }
//
// void BaseExperiment::postConfigureInitialization()
// {
// 	// Starting the inner thread
// 	m_workerThread->start();
//
// 	setStatus("Configured");
// }
//
// ParameterSettableUI* BaseExperiment::getUIManager()
// {
// 	return new __BaseExperiment_internal::BaseExperimentUIManager(this);
// }
//
// void BaseExperiment::fillActionsMenu(QMenu* actionsMenu)
// {
// 	// This simply gets the list of actions from getActionsForOperations() and then adds them to the menu
// 	QList<QAction*> actions = getActionsForOperations(actionsMenu);
//
// 	foreach (QAction* a, actions) {
// 		actionsMenu->addAction(a);
// 	}
// }
//
// QList<ParameterSettableUIViewer> BaseExperiment::getViewers(QWidget* parent, Qt::WindowFlags flags)
// {
// 	// The default implementation only adds the BaseExperimentGUI widget
// 	QList<farsa::ParameterSettableUIViewer> viewers;
//
// 	BaseExperimentGUI* gui = new BaseExperimentGUI(this, parent, flags);
// 	viewers.append(farsa::ParameterSettableUIViewer(gui, "Experiment Control"));
//
// 	return viewers;
// }
//
// void BaseExperiment::addAdditionalMenus(QMenuBar*)
// {
// 	// The default implementation does nothing
// }
//
// void BaseExperiment::run()
// {
// 	// Executing the next action. The action ID should be a positive number and be in the vector. Moreover
// 	// the action should use a separate thread
// 	Q_ASSERT(m_runningOperationID >= 0);
// 	Q_ASSERT(m_runningOperationID < m_operationsVector.size());
// 	Q_ASSERT(m_operationsVector[m_runningOperationID]->useSeparateThread);
//
// 	// Start operation. Here we also signal that the operation has started/ended. Notice that we are calling a non thread-safe function
// 	// from a thread different from the one in which this object lives, but it is OK since when we are here we are not calling this
// 	// function from other places
// 	uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationStarted, m_runningOperationID);
// 	m_operationsVector[m_runningOperationID]->executeOperation();
// 	uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationEnded, m_runningOperationID);
// }
//
// void BaseExperiment::stop()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	m_stop = true;
//
// 	// Also resuming the operation if it was sleeping
// 	m_pause = false;
// 	m_waitCondition.wakeAll();
// }
//
// void BaseExperiment::pause()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	// This is made this way so that m_pause is modified even if nothing is running (to be able to decide how the simulation starts)
// 	m_pause = true;
// }
//
// void BaseExperiment::step()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	if ((m_runningOperationID != -1) && m_operationsVector[m_runningOperationID]->steppable && m_pause) {
// 		m_waitCondition.wakeAll();
// 	}
// }
//
// void BaseExperiment::resume()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	// This is made this way so that m_pause is modified even if nothing is running (to be able to decide how the simulation starts)
// 	const bool nowPaused = m_pause;
// 	m_pause = false;
//
// 	if ((m_runningOperationID != -1) && m_operationsVector[m_runningOperationID]->steppable && nowPaused) {
// 		m_waitCondition.wakeAll();
// 	}
// }
//
// void BaseExperiment::changeInterval(unsigned long interval)
// {
// 	changeInterval(interval, true);
// }
//
// unsigned long BaseExperiment::currentInterval() const
// {
// 	return m_delay;
// }
//
// const QVector<BaseExperiment::AbstractOperationWrapper*>& BaseExperiment::getOperations() const
// {
// 	return m_operationsVector;
// }
//
// DataUploaderDownloader<__BaseExperiment_internal::OperationStatus, __BaseExperiment_internal::OperationControl>* BaseExperiment::getUploaderDownloader()
// {
// 	return &m_dataExchange;
// }
//
// void BaseExperiment::stopCurrentOperation(bool wait)
// {
// 	// Stopping the current operation, if running
// 	m_workerThread->stopCurrentOperation(wait);
// }
//
// void BaseExperiment::stopCurrentOperation()
// {
// 	stopCurrentOperation(false);
// }
//
// QList<QAction*> BaseExperiment::getActionsForOperations(QObject* actionsParent) const
// {
// 	QList<QAction*> actions;
//
// 	for (int i = 0; i < m_operationsVector.size(); i++) {
// 		// Creating the new action
// 		QAction* act = new QAction(m_operationsVector[i]->name, actionsParent);
//
// 		// Connecting the signal of the new QAction to the m_actionSignalsMapper slot and setting
// 		// the mapping
// 		connect(act, SIGNAL(triggered()), m_actionSignalsMapper.get(), SLOT(map()));
// 		m_actionSignalsMapper->setMapping(act, i);
//
// 		// Adding to the list of actions
// 		actions.push_back(act);
// 	}
//
// 	return actions;
// }
//
// bool BaseExperiment::batchRunning() const
// {
// 	return m_batchRunning;
// }
//
// bool BaseExperiment::stopSimulation()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	return m_stop;
// }
//
// void BaseExperiment::checkPause()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	if ((m_runningOperationID == -1) || (!m_operationsVector[m_runningOperationID]->steppable)) {
// 		return;
// 	}
//
// 	if (m_pause) {
// 		if (m_previousPauseStatus == false) {
// 			uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationPaused, m_runningOperationID);
// 		}
// 		m_waitCondition.wait(&m_mutex);
// 		if (m_pause == false) {
// 			uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationResumed, m_runningOperationID);
// 		}
// 	} else {
// 		if (m_previousPauseStatus == true) {
// 			uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationResumed, m_runningOperationID);
// 		}
// 		if (m_delay != 0) {
// 			m_waitCondition.wait(&m_mutex, m_delay);
// 		}
// 	}
// 	m_previousPauseStatus = m_pause;
// }
//
// void BaseExperiment::exceptionDuringOperation(BaseException *e)
// {
// 	Logger::error(QString("Error while executing the current operation, an exception was thrown. Reason: ") + e->what());
//
// 	// Here we laso have to signal the GUI that the operation has stopped
// 	uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationEnded, m_runningOperationID);
// }
//
// void BaseExperiment::runOperation(int operationID)
// {
// 	// Executing the next action. The action ID should be a positive number and be in the vector
// 	Q_ASSERT(operationID >= 0);
// 	Q_ASSERT(operationID < m_operationsVector.size());
//
// 	m_runningOperationID = operationID;
// 	if ((m_batchRunning) || (!m_operationsVector[m_runningOperationID]->useSeparateThread)) {
// 		// Resetting stop
// 		resetStop();
//
// 		// Starting operation directly. Here we also signal when the operation starts and ends
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationStarted, m_runningOperationID);
// 		m_operationsVector[m_runningOperationID]->executeOperation();
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationEnded, m_runningOperationID);
// 	} else {
// 		// Checking if another operation is running. If it is, prints a warning and doesn't do anything. Here there is
// 		// a possible race condition: if an operation is scheduled after the condition in the if is cheked but before
// 		// addOperation is executed we could end up with two operations in the queue. However this is the only function
// 		// that can schedule operations and this is never run concurrently. Moreover having two operations in the
// 		// queue of the worker thread is not a big problem.
// 		if (m_workerThread->operationRunning()) {
// 			Logger::error("Cannot run the requested operation because another action is currently running; please wait until it finish, or stop it before");
// 			return;
// 		}
//
// 		// Resetting stop
// 		resetStop();
//
// 		// Starting operation
// 		m_workerThread->addOperation(this, false);
// 	}
// }
//
// void BaseExperiment::resetStop()
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	m_stop = false;
// }
//
// void BaseExperiment::uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::Status status, unsigned int operationID, unsigned long newDelay)
// {
// 	DatumToUpload<__BaseExperiment_internal::OperationStatus> d(m_dataExchange);
// 	d->status = status;
// 	d->operationID = operationID;
// 	d->delay = newDelay;
// }
//
// void BaseExperiment::changeInterval(unsigned long interval, bool sendNotificationToGUI)
// {
// 	QMutexLocker locker(&m_mutex);
//
// 	m_delay = interval;
//
// 	if (sendNotificationToGUI) {
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::OperationStepDelayChanged, m_runningOperationID, m_delay);
// 	}
// }
//
// } // End namespace farsa
//
