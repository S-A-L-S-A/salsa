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
// #ifndef BASEEXPERIMENTGUI_H
// #define BASEEXPERIMENTGUI_H
//
// #include "experimentsconfig.h"
// #include "dataexchange.h"
// #include "baseexperiment.h"
// #include <QWidget>
// #include <QGroupBox>
// #include <QLabel>
// #include <QComboBox>
// #include <QPushButton>
// #include <QSlider>
//
// namespace farsa {
//
// /**
//  * \brief The GUI to control a BaseExperiment subclass
//  *
//  * This is a GUI to control a BaseExperiment subclass. It lists all the actions
//  * (those also present by default in the Actions menu), gives some information
//  * on them (whether they are immediate operations, threaded or threaded
//  * steppable), allows to start/stop them and gives control over steppable
//  * threaded operations (pause, step, delay between steps)
//  */
// class FARSA_EXPERIMENTS_API BaseExperimentGUI : public QWidget, public DataUploaderDownloader<__BaseExperiment_internal::OperationControl, __BaseExperiment_internal::OperationStatus>
// {
// 	Q_OBJECT
//
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param experiment the experiment
// 	 * \param parent the parent widget
// 	 * \param flags window flags
// 	 */
// 	BaseExperimentGUI(BaseExperiment* experiment, QWidget* parent = NULL, Qt::WindowFlags flags = 0);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~BaseExperimentGUI();
//
// 	/**
// 	 * \brief Receives events
// 	 *
// 	 * This only processes the
// 	 * NewProductEvent\<__BaseExperiment_internal::OperationStatus\>
// 	 * \param e the event to processes
// 	 * \return true if the event has been recognized and processed
// 	 */
// 	virtual bool event(QEvent* e);
//
// private slots:
// 	/**
// 	 * \brief The slot called whenever the selected operation changes
// 	 *
// 	 * \param index the currently selected index
// 	 */
// 	void currentOperationChanged(int index);
//
// 	/**
// 	 * \brief Starts the currently selected operation
// 	 */
// 	void startOperation();
//
// 	/**
// 	 * \brief Stops the current operation
// 	 */
// 	void stopOperation();
//
// 	/**
// 	 * \brief Pauses or resumes the current operation
// 	 */
// 	void pauseOperation(bool pause);
//
// 	/**
// 	 * \brief Performs a step of the current operation
// 	 */
// 	void stepOperation();
//
// 	/**
// 	 * \brief Changes the delay between the steps of a steppable operation
// 	 *
// 	 * \param delay the new delay between steps
// 	 */
// 	void changeDelay(int delay);
//
// private:
// 	/**
// 	 * \brief Takes the list of operations from the experiment and fills the
// 	 *        combo box
// 	 */
// 	void updateOperationsList();
//
// 	/**
// 	 * \brief Enables or disables widgets
// 	 *
// 	 * \param enableStart if true enables m_startSelectedOperationButton
// 	 * \param enableStop if true enables m_stopCurrentOperationButton
// 	 * \param enablePause if true enables m_pauseOperationButton
// 	 * \param enableDelay if true enables m_delaySlider
// 	 */
// 	void enableWidgets(bool enableStart, bool enableStop, bool enablePause, bool enableDelay);
//
// 	/**
// 	 * \brief Returns a string describing the operation with the given id
// 	 *
// 	 * \param id the id of the operation. This is the ID in the list, not in
// 	 *           the combo box
// 	 * \param prefix a prefix for the returned string
// 	 * \return a string describing the operation
// 	 */
// 	QString describeOperation(int id, QString prefix = "") const;
//
// 	/**
// 	 * \brief Converts a slider value between 0 and 100 to a delay
// 	 *
// 	 * \param value the value to convert to a delay. If 0, 0 is returned
// 	 */
// 	unsigned long sliderValueToDelay(int value) const;
//
// 	/**
// 	 * \brief Converts an interval to a slider value between 0 and 100
// 	 *
// 	 * \param interval the interval to convert to a slider value. If 0, 0 is
// 	 *                 returned
// 	 */
// 	int delayToSliderValue(unsigned long interval) const;
//
// 	/**
// 	 * \brief The experiment
// 	 */
// 	BaseExperiment* const m_experiment;
//
// 	/**
// 	 * \brief The list of operations
// 	 */
// 	QVector<BaseExperiment::AbstractOperationWrapper*> m_operations;
//
// 	/**
// 	 * \brief The group of widgets to select an operation to run
// 	 */
// 	QGroupBox* m_operationSelectionGroup;
//
// 	/**
// 	 * \brief The label for the operations combo box
// 	 */
// 	QLabel* m_operationsListLabel;
//
// 	/**
// 	 * \brief The combo box displaying the list of operations
// 	 */
// 	QComboBox* m_operationsList;
//
// 	/**
// 	 * \brief The label displaying information about the currently selected
// 	 *        operation
// 	 */
// 	QLabel* m_selectedOperationInfo;
//
// 	/**
// 	 * \brief The button to start the currently selected operation
// 	 */
// 	QPushButton* m_startSelectedOperationButton;
//
// 	/**
// 	 * \brief The group of widgets to control the current operation
// 	 */
// 	QGroupBox* m_currentOperationControlGroup;
//
// 	/**
// 	 * \brief The label displaying information about the running operation
// 	 */
// 	QLabel* m_currentOperationInfo;
//
// 	/**
// 	 * \brief The button to stop the running operation
// 	 */
// 	QPushButton* m_stopCurrentOperationButton;
//
// 	/**
// 	 * \brief The group of widgets to control a steppable operation
// 	 */
// 	QGroupBox* m_steppableOperationControlGroup;
//
// 	/**
// 	 * \brief The button to put a steppable operation in pause
// 	 */
// 	QPushButton* m_pauseOperationButton;
//
// 	/**
// 	 * \brief The button to perform a step of a steppable operation
// 	 */
// 	QPushButton* m_stepOperationButton;
//
// 	/**
// 	 * \brief The label for the delay slider
// 	 */
// 	QLabel* m_delaySliderLabel;
//
// 	/**
// 	 * \brief The slider to set the delay between steps of a steppable
// 	 *        operation
// 	 */
// 	QSlider* m_delaySlider;
//
// 	/**
// 	 * \brief The ID of the running operation or -1 if no operation is
// 	 *        running
// 	 */
// 	int m_runningOperationID;
// };
//
// } // End namespace farsa
//
// #endif
