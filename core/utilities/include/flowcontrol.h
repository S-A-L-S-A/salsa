/********************************************************************************
 *  FARSA                                                                       *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include "utilitiesconfig.h"

namespace farsa {

/**
 * \brief The interface for classes controlling the flow of execution
 *
 * This class is used to control the execution flow of subclasses of
 * FlowControlled. See FlowControlled description for more information
 */
class FARSA_UTIL_TEMPLATE FlowController
{
public:
	/**
	 * \brief The function to check if execution should be terminated as
	 *        soon as possible
	 *
	 * A note on implementation: if the pause() function could wait for an
	 * external event (e.g. the user pressing a button), to stop execution
	 * you should make this function return true AND make pause return (e.g.
	 * waking the wait condition on which pause() waits)
	 * \return true if execution should be terminated as soon as possible
	 */
	virtual bool stop() = 0;

	/**
	 * \brief Performs a pause if needed
	 *
	 * This function should pause, sleep for a while or return immediately
	 * depending on how the execution flow should continue
	 */
	virtual void pause() = 0;
};

/**
 * \brief A dummy flow controller
 *
 * This class implements a dummy flw controller, in which stop() always return
 * false and pause() always returns immediately
 */
class FARSA_UTIL_TEMPLATE DummyFlowController : public FlowController
{
public:
	/**
	 * \brief The function to check if execution should be terminated as
	 *        soon as possible
	 *
	 * This always returns false
	 * \return true if execution should be terminated as soon as possible
	 */
	virtual bool stop()
	{
		return false;
	}

	/**
	 * \brief Performs a pause if needed
	 *
	 * This does nothing
	 */
	virtual void pause()
	{
	}
};

/**
 * \brief The base for classes that have a controllable flow of execution
 *
 * This can be used as the base for classes that have a controllable flow of
 * execution. Think, for example, of a robotic simulation: its execution can be
 * implemented as a sequence of discrete steps. During each step the sensors are
 * read, the motors are activated and the effects of the robot on the
 * environment is computed. It is generally useful to provide who runs the
 * simulation the possibility to stop it earlier and to control the delay
 * between steps or to request the execution of one step externally. To
 * implement these functionalities the simulation class can inherit from this
 * class and use the stopFlow() and pauseFlow() functions at regular intervals.
 * The former returns true when execution should be terminated, the latter
 * pauses execution if needed, sleeps for a certain amount of time or returns
 * immediately. Users of the simulation class, can then set the flow controller
 * object (a subclass of FlowController) which is responsible of deciding when
 * the simulation should stop or for how long it should sleep. Implementations
 * of FlowController could, for example, check the status of a GUI which lets
 * the user decide how to control the execution flow or can simply stop
 * execution if it is taking too long. Using FlowControlled as a base for
 * classes that implement long operations (possibly running in a separate
 * thread) allows to only concentrate on when to check for stop requests and on
 * how to separate the flow execution into steps (a step is what occurs between
 * two subsequent calls to pauseFlow()), while leaving to other object the
 * responsability to actually control the execution flow.
 *
 * An example of an implementation of FlowControlled subclass can be as follows:
 *
 * \code
 * class MyExperiment : public FlowControlled
 * {
 * public:
 * 	...
 *
 * 	void longOperation();
 *
 * private:
 * 	...
 * };
 *
 * // In implementation
 * MyExperiment::longOperation()
 * {
 * 	... initialization...
 *
 * 	pauseFlow();
 * 	if (stopFlow()) {
 * 		... cleanup ...
 *
 * 		return;
 * 	}
 *
 * 	while (...) {
 * 		... do step ...
 *
 * 		pauseFlow();
 * 		if (stopFlow()) {
 * 			break;
 * 		}
 * 	}
 *
 * 	... finalization ...
 * }
 * \endcode
 *
 * \note When implementing a FlowController, make sure to also make the pause()
 *       function return when you want to stop execution. In fact, if the
 *       pause() function could wait for an external event (e.g. the user
 *       pressing a button), only having stop() return true could be not enough
 *       to actually stop execution (because the FlowControlled could be waiting
 *       on pauseFlow())
 * \note This and FlowController are not thread-safe, you should make
 *       FlowController thread safe if you want to control execution of the
 *       FlowControlled subclass from a separate thread (e.g. when you have a
 *       simulation thread and a GUI thread)
 */
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning forse in farsa 2 questa roba si può mettere nella classe Component che ha una struttura gerarchica, in modo da propagare automaticamente il flow controller ai figli. In tal caso la funzione flowControllerChanged può essere forse rimossa
#endif
class FARSA_UTIL_TEMPLATE FlowControlled
{
public:
	/**
	 * \brief Constructor
	 *
	 * This sets the flow controller to a DummyFlowController instance
	 */
	FlowControlled()
		: m_dummyFlowController()
		, m_flowController(&m_dummyFlowController)
	{
	}

	/**
	 * \brief Constructor
	 *
	 * This sets the flow controller to the given flow controller
	 * \param flowController the flow controller object to use. If NULL, it
	 *                       sets the flow controller to an instance of
	 *                       DummyFlowController
	 */
	FlowControlled(FlowController* flowController)
		: m_dummyFlowController()
		, m_flowController((flowController == NULL) ? &m_dummyFlowController : flowController)
	{
	}

	/**
	 * \brief Sets the flow controller object to use
	 *
	 * The flow controller object is not deleted here,
	 * \param flowController the flow controller object to use. If NULL, it
	 *                       sets the flow controller to an instance of
	 *                       DummyFlowController
	 */
	void setFlowController(FlowController* flowController)
	{
		if (flowController != m_flowController) {
			if (flowController == NULL) {
				m_flowController = &m_dummyFlowController;
			} else {
				m_flowController = flowController;
			}

			flowControllerChanged(m_flowController);
		}
	}

protected:
	/**
	 * \brief Checks if execution should stop as soon as possible
	 *
	 * \return true if execution should stop as soon as possible
	 */
	bool stopFlow()
	{
		return m_flowController->stop();
	}

	/**
	 * \brief Performs a pause in the if needed
	 *
	 * This function pauses, sleeps for a while or return immediately
	 * depending on the current flow controller
	 */
	void pauseFlow()
	{
		m_flowController->pause();
	}

	/**
	 * \brief The function called when the flow controller changes
	 *
	 * The default implementation does nothing. You can use this function to
	 * propagate the flow controller to subcomponents
	 * \param flowController the new flow controller
	 */
	virtual void flowControllerChanged(FlowController* flowController)
	{
		Q_UNUSED(flowController)
	}

private:
	/**
	 * \brief A DummyFlowController instance to use in case no other flow
	 *        controller is set
	 */
	DummyFlowController m_dummyFlowController;

	/**
	 * \brief The flow controller object to use
	 *
	 * This is a pointer to m_dummyFlowController if not external flow
	 * controller object is set
	 */
	FlowController* m_flowController;
};

}

#endif
