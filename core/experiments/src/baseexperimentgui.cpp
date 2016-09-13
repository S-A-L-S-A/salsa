// /********************************************************************************
//  *  SALSA                                                                       *
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
// #include "baseexperimentgui.h"
// #include <QVBoxLayout>
// #include <QGridLayout>
// #include <cmath>
//
// namespace salsa {
//
// BaseExperimentGUI::BaseExperimentGUI(BaseExperiment* experiment, QWidget* parent, Qt::WindowFlags flags) :
// 	QWidget(parent, flags),
// 	DataUploaderDownloader<__BaseExperiment_internal::OperationControl, __BaseExperiment_internal::OperationStatus>(1, BlockUploader, this),
// 	m_experiment(experiment),
// 	m_operations(),
// 	m_operationSelectionGroup(nullptr),
// 	m_operationsListLabel(nullptr),
// 	m_operationsList(nullptr),
// 	m_selectedOperationInfo(nullptr),
// 	m_startSelectedOperationButton(nullptr),
// 	m_currentOperationControlGroup(nullptr),
// 	m_currentOperationInfo(nullptr),
// 	m_stopCurrentOperationButton(nullptr),
// 	m_steppableOperationControlGroup(nullptr),
// 	m_pauseOperationButton(nullptr),
// 	m_stepOperationButton(nullptr),
// 	m_delaySliderLabel(nullptr),
// 	m_delaySlider(nullptr),
// 	m_runningOperationID(-1)
// {
// 	// Associating data uploaders and downloaders
// 	GlobalUploaderDownloader::associate(this, experiment->getUploaderDownloader());
//
// 	// The main layout
// 	QVBoxLayout* mainLayout = new QVBoxLayout(this);
//
// 	{
// 		// The group of widgets to select an operation
// 		m_operationSelectionGroup = new QGroupBox("Operation Selection", this);
// 		mainLayout->addWidget(m_operationSelectionGroup);
//
// 		// The layout for the m_operationSelectionGroup
// 		QGridLayout* layout = new QGridLayout(m_operationSelectionGroup);
//
// 		// The label for the operations combo box
// 		m_operationsListLabel = new QLabel("Operation: ", m_operationSelectionGroup);
// 		layout->addWidget(m_operationsListLabel, 0, 0);
//
// 		// The combo box displaying the list of operations
// 		m_operationsList = new QComboBox(m_operationSelectionGroup);
// 		QSizePolicy operationsListSizePolicy = m_operationsList->sizePolicy();
// 		operationsListSizePolicy.setHorizontalStretch(1);
// 		m_operationsList->setSizePolicy(operationsListSizePolicy);
// 		connect(m_operationsList, SIGNAL(currentIndexChanged(int)), this, SLOT(currentOperationChanged(int)));
// 		layout->addWidget(m_operationsList, 0, 1);
//
// 		// The label displaying information about the currently selected operation
// 		m_selectedOperationInfo = new QLabel("Invalid operation", m_operationSelectionGroup);
// 		layout->addWidget(m_selectedOperationInfo, 1, 0, 1, 2);
//
// 		// The button to start the currently selected operation
// 		m_startSelectedOperationButton = new QPushButton("Start Operation", m_operationSelectionGroup);
// 		connect(m_startSelectedOperationButton, SIGNAL(clicked()), this, SLOT(startOperation()));
// 		layout->addWidget(m_startSelectedOperationButton, 2, 0, 1, 2);
// 	}
//
// 	{
// 		// The group of widgets to control the current operation
// 		m_currentOperationControlGroup = new QGroupBox("Current Operation Control", this);
// 		mainLayout->addWidget(m_currentOperationControlGroup);
//
// 		// The layout for the m_currentOperationControlGroup
// 		QVBoxLayout* layout = new QVBoxLayout(m_currentOperationControlGroup);
//
// 		// The label displaying information about the running operation
// 		m_currentOperationInfo = new QLabel("No running operation", m_currentOperationControlGroup);
// 		layout->addWidget(m_currentOperationInfo);
//
// 		// The button to stop the running operation
// 		m_stopCurrentOperationButton = new QPushButton("Stop Operation", m_currentOperationControlGroup);
// 		connect(m_stopCurrentOperationButton, SIGNAL(clicked()), this, SLOT(stopOperation()));
// 		layout->addWidget(m_stopCurrentOperationButton);
// 	}
//
// 	{
// 		// The group of widgets to control a steppable operation
// 		m_steppableOperationControlGroup = new QGroupBox("Steppable Operation Control", this);
// 		mainLayout->addWidget(m_steppableOperationControlGroup);
//
// 		// The layout for the m_steppableOperationControlGroup
// 		QGridLayout* layout = new QGridLayout(m_steppableOperationControlGroup);
//
// 		// The button to put a steppable operation in pause
// 		m_pauseOperationButton = new QPushButton("Pause", m_steppableOperationControlGroup);
// 		m_pauseOperationButton->setCheckable(true);
// 		connect(m_pauseOperationButton, SIGNAL(clicked(bool)), this, SLOT(pauseOperation(bool)));
// 		layout->addWidget(m_pauseOperationButton, 0, 0, 1, 2);
//
// 		// The button to perform a step of a steppable operation
// 		m_stepOperationButton = new QPushButton("Step", m_steppableOperationControlGroup);
// 		connect(m_stepOperationButton, SIGNAL(clicked()), this, SLOT(stepOperation()));
// 		layout->addWidget(m_stepOperationButton, 1, 0, 1, 2);
//
// 		// The label for the delay slider
// 		m_delaySliderLabel = new QLabel("Interval between steps:", m_steppableOperationControlGroup);
// 		layout->addWidget(m_delaySliderLabel, 2, 0);
//
// 		// The slider to set the delay between steps of a steppable operation
// 		m_delaySlider = new QSlider(Qt::Horizontal, m_steppableOperationControlGroup);
// 		QSizePolicy delaySliderSizePolicy = m_delaySlider->sizePolicy();
// 		delaySliderSizePolicy.setHorizontalStretch(1);
// 		m_delaySlider->setSizePolicy(delaySliderSizePolicy);
// 		m_delaySlider->setRange(0, 100);
// 		m_delaySlider->setValue(delayToSliderValue(m_experiment->currentInterval()));
// 		connect(m_delaySlider, SIGNAL(valueChanged(int)), this, SLOT(changeDelay(int)));
// 		layout->addWidget(m_delaySlider, 2, 1);
// 	}
//
// 	// Updating the list of operations
// 	updateOperationsList();
// }
//
// BaseExperimentGUI::~BaseExperimentGUI()
// {
// 	// Nothing to do here
// }
//
// bool BaseExperimentGUI::event(QEvent* e)
// {
// 	if (e->type() == salsa::NewDatumEvent<__BaseExperiment_internal::OperationStatus>::newDatumEventType) {
// 		// We need a cylce because there could be more than one datum, when the uploader is connected
// 		// (see dataexchange.h documentation for more information)
// 		const __BaseExperiment_internal::OperationStatus* d;
// 		while ((d = downloadDatum()) != nullptr) {
// 			switch (d->status) {
// 				case __BaseExperiment_internal::OperationStatus::NewOperation:
// 					updateOperationsList();
// 					break;
// 				case __BaseExperiment_internal::OperationStatus::OperationStarted:
// 					m_runningOperationID = d->operationID;
// 					m_currentOperationInfo->setText(describeOperation(m_runningOperationID, "Running operation: "));
//
// 					if (m_operations[m_runningOperationID]->getUseSeparateThread()) {
// 						if (m_operations[m_runningOperationID]->getSteppable()) {
// 							enableWidgets(false, true, true, true);
// 						} else {
// 							enableWidgets(false, true, false, false);
// 						}
// 					} else {
// 						enableWidgets(false, false, false, false);
// 					}
// 					break;
// 				case __BaseExperiment_internal::OperationStatus::OperationEnded:
// 					m_runningOperationID = -1;
// 					currentOperationChanged(m_operationsList->currentIndex());
// 					m_currentOperationInfo->setText("No running operation");
// 					break;
// 				case __BaseExperiment_internal::OperationStatus::OperationPaused:
// 					m_stepOperationButton->setEnabled(true);
// 					break;
// 				case __BaseExperiment_internal::OperationStatus::OperationResumed:
// 					m_stepOperationButton->setEnabled(false);
// 					break;
// 				case __BaseExperiment_internal::OperationStatus::OperationStepDelayChanged:
// 					// Blocking signals of the slider to avoid re-setting the value in the simulation
// 					m_delaySlider->blockSignals(true);
// 					m_delaySlider->setValue(delayToSliderValue(d->delay));
// 					m_delaySlider->blockSignals(false);
// 					break;
// 			}
// 		}
//
// 		return true;
// 	} else {
// 		return QWidget::event(e);
// 	}
// }
//
// void BaseExperimentGUI::currentOperationChanged(int index)
// {
// 	m_selectedOperationInfo->setText(describeOperation(((index < 0) ? index : (index + 1)), "Selected operation: "));
//
// 	if (m_runningOperationID == -1) {
// 		if (index == -1) {
// 			enableWidgets(false, false, false, false);
// 		} else {
// 			if (m_operations[index + 1]->getUseSeparateThread()) {
// 				if (m_operations[index + 1]->getSteppable()) {
// 					enableWidgets(true, false, true, true);
// 				} else {
// 					enableWidgets(true, false, false, false);
// 				}
// 			} else {
// 				enableWidgets(true, false, false, false);
// 			}
// 		}
// 	}
// }
//
// void BaseExperimentGUI::startOperation()
// {
// 	if (m_operationsList->currentIndex() == -1) {
// 		return;
// 	}
//
// 	// Telling the experiment to start an operation
// 	DatumToUpload d(*this);
// 	if (m_pauseOperationButton->isChecked()) {
// 		d->action = __BaseExperiment_internal::OperationControl::StartOperationPaused;
// 	} else {
// 		d->action = __BaseExperiment_internal::OperationControl::StartOperation;
// 	}
// 	d->operationID = m_operationsList->currentIndex() + 1;
//
// 	m_startSelectedOperationButton->setEnabled(false);
// }
//
// void BaseExperimentGUI::stopOperation()
// {
// 	// Telling the experiment to stop the current operation
// 	DatumToUpload d(*this);
// 	d->action = __BaseExperiment_internal::OperationControl::StopOperation;
// }
//
// void BaseExperimentGUI::pauseOperation(bool pause)
// {
// 	// Telling the experiment to pause/resume the current operation
// 	DatumToUpload d(*this);
//
// 	if (pause) {
// 		d->action = __BaseExperiment_internal::OperationControl::PauseOperation;
// 		if (m_runningOperationID != -1) {
// 			m_stepOperationButton->setEnabled(true);
// 		}
// 	} else {
// 		d->action = __BaseExperiment_internal::OperationControl::ResumeOperation;
// 		m_stepOperationButton->setEnabled(false);
// 	}
// }
//
// void BaseExperimentGUI::stepOperation()
// {
// 	// Telling the experiment to perform a step
// 	DatumToUpload d(*this);
// 	d->action = __BaseExperiment_internal::OperationControl::StepOperation;
// }
//
// void BaseExperimentGUI::changeDelay(int delay)
// {
// 	// Telling the experiment the new delay
// 	DatumToUpload d(*this);
// 	d->action = __BaseExperiment_internal::OperationControl::ChangeInterval;
// 	d->interval = sliderValueToDelay(delay);
// }
//
// void BaseExperimentGUI::updateOperationsList()
// {
// 	m_operations = m_experiment->getOperations();
//
// 	// First of all clearing the combobox
// 	m_operationsList->clear();
//
// 	// Now adding all operations except the first one that is always stop
// 	for (int i = 1; i < m_operations.size(); i++) {
// 		m_operationsList->insertItem(i - 1, m_operations[i]->getName());
// 	}
//
// 	// Selecting the first operation
// 	m_operationsList->setCurrentIndex(0);
// }
//
// void BaseExperimentGUI::enableWidgets(bool enableStart, bool enableStop, bool enablePause, bool enableDelay)
// {
// 	m_startSelectedOperationButton->setEnabled(enableStart);
// 	m_stopCurrentOperationButton->setEnabled(enableStop);
// 	m_pauseOperationButton->setEnabled(enablePause);
// 	if ((enablePause) && (m_runningOperationID != -1)) {
// 		m_stepOperationButton->setEnabled(m_pauseOperationButton->isChecked());
// 	} else {
// 		m_stepOperationButton->setEnabled(false);
// 	}
// 	m_delaySlider->setEnabled(enableDelay);
// }
//
// QString BaseExperimentGUI::describeOperation(int id, QString prefix) const
// {
// 	QString str;
//
// 	if (id < 0) {
// 		str = "Invalid operation";
// 	} else {
// 		str = prefix + m_operations[id]->getName();
// 		if (m_operations[id]->getUseSeparateThread()) {
// 			if (m_operations[id]->getSteppable()) {
// 				str += "; it is a STEPPABLE THREAD operation";
// 			} else {
// 				str += "; it is a THREAD operation";
// 			}
// 		} else {
// 			str += "; it is an IMMEDIATE operation";
// 		}
// 	}
//
// 	return str;
// }
//
// unsigned long BaseExperimentGUI::sliderValueToDelay(int value) const
// {
// 	if (value == 0) {
// 		return 0;
// 	} else {
// 		return (unsigned long) pow((double) 1.1, (double) value);
// 	}
// }
//
// int BaseExperimentGUI::delayToSliderValue(unsigned long interval) const
// {
// 	if (interval == 0) {
// 		return 0;
// 	} else {
// 		return (int) (log((double) interval) / log((double) 1.1));
// 	}
// }
//
// } // End namespace salsa
//
