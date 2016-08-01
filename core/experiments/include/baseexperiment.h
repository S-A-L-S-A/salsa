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
#warning SPOSTARE DENTRO ComponentUI LA PARTE DI DEFINIZIONE DELLE AZIONI, IN MODO CHE TUTTI I COMPONENTI POSSANO DEFINIRLE E SI POSSA ACCEDERE A LORO PROGRAMMATICAMENTE (OSSIA NON SOLO TRAMITE MENU). DECIDERE DOVE METTERE STEPPABLE/THREADED/IMMEDIATE ACTIONS E QUALCOSA DI SIMILE A stopCurrentOperation. LASCIARE QUI LE ALTRE COSE (AD ESEMPIO LA GUI). INOLTRE FORSE total99 DEVE ASPETTARSI UN BaseExperiment INVECE DI UN Component E BaseExperiment DEVE ANCHE AVERE SEGNALI TIPO statusChanged (O FORSE METTERLI IN ComponentUI?)
// Riguardo il warning sopra: in generale bisogna fare in modo che ogni component possa definire delle azioni (probabilmente con il supporto di steppable/threaded/immediate) e queste
// azioni devono comparire in total99. In questo modo cose come ad esempio i test possono essere implementati come semplici component (i test in particolare poi dovrebbero avere dei
// parametri per prendere esperimento, agente o altro, usando direttamente ConfigurationManager, senza funzioni ad hoc come setComponent). Pensare anche a come generalizzare la gui che
// sta qui (per step/pause, sarebbe meglio avere una GUI unica invece che una per componente)
//
// #ifndef BASEEXPERIMENT_H
// #define BASEEXPERIMENT_H
//
// #include "experimentsconfig.h"
// #include "dataexchange.h"
// #include "component.h"
// #include "workerthread.h"
// #include "parametersettable.h"
// #include "parametersettableui.h"
// #include "baseexception.h"
// #include "configurationhelper.h"
// #include "logger.h"
// #include "flowcontrol.h"
// #include <QMutex>
// #include <QMutexLocker>
// #include <QWaitCondition>
// #include <QSignalMapper>
// #include <QList>
// #include <QVector>
// #include <QAction>
// #include <QMenu>
// #include <QMenuBar>
// #include <memory>
// #include <QtGlobal>
//
// namespace salsa {
//
// class BaseExperimentGUI;
//
// /**
//  * \brief The namespace with helper code for the BaseExperiment class
//  *
//  * \internal
//  */
// namespace __BaseExperiment_internal {
// 	class BaseExperimentUIManager;
//
// 	/**
// 	 * \brief The data sent by a BaseExperiment instance to the
// 	 *        corresponding GUI
// 	 */
// 	struct SALSA_EXPERIMENTS_TEMPLATE OperationStatus {
// 		/**
// 		 * \brief The list of possible status
// 		 */
// 		enum Status {
// 			NewOperation, /**< A new operation has been declared */
// 			OperationStarted, /**< An operation has started */
// 			OperationEnded, /**< The current operation has ended */
// 			OperationPaused, /**< The current operation is pauses */
// 			OperationResumed, /**< The current operation is running
// 			                       again after a pause */
// 			OperationStepDelayChanged /**< The delay for steps of
// 			                               has been changed */
// 		};
//
// 		/**
// 		 * \brief The current status
// 		 */
// 		Status status;
//
// 		/**
// 		 * \brief The id of the operation that was
// 		 *        added/started/ended...
// 		 *
// 		 * This is ignored if status if OperationStepDelayChanged
// 		 */
// 		unsigned int operationID;
//
// 		/**
// 		 * \brief The new delay for steps of the current operation
// 		 *
// 		 * This is only taken into account if status is
// 		 * OperationStepDelayChanged
// 		 */
// 		unsigned long delay;
// 	};
//
// 	/**
// 	 * \brief The data sent by a BaseExperimentGUI instance to the
// 	 *        corresponding experiment
// 	 */
// 	struct SALSA_EXPERIMENTS_TEMPLATE OperationControl {
// 		/**
// 		 * \brief The list of possible actions to perform
// 		 */
// 		enum Action {
// 			StartOperation, /**< Start an operation */
// 			StartOperationPaused, /**< Start an operation and
// 			                           immediately puts it in pause.
// 			                           If the operation is not
// 			                           steppable, this is the same
// 			                           as StartOperation */
// 			StopOperation, /**< Stop the current operation */
// 			PauseOperation, /**< Pause the current operation */
// 			StepOperation, /**< Perform a step of the current
// 			                    operation */
// 			ResumeOperation, /**< Resumes the current operation if
// 			                      paused */
// 			ChangeInterval /**< Change the interval between steps
// 			                    for the current operation*/
// 		};
//
// 		/**
// 		 * \brief The action to perform
// 		 */
// 		Action action;
//
// 		/**
// 		 * \brief The id of the operation to start
// 		 *
// 		 * This only makes sense if action is StartOperation
// 		 */
// 		unsigned int operationID;
//
// 		/**
// 		 * \brief The new interval in milliseconds
// 		 *
// 		 * This only makes sense if action is ChangeInterval
// 		 */
// 		unsigned long interval;
// 	};
// }
//
// /**
//  * \brief The base class for experiments
//  *
//  * Use this class as parent of experiments. This only provides some helper code
//  * to run operations in a worker thread and to add menus and guis to Total99.
//  * By using this you don't need to create a component and uimanager separately
//  * and you also have some ready-to-use code that supports different operations.
//  * When using this class you have to define which actions the class provides and
//  * to implement the function which execute the actions as QT slots. The code in
//  * this class takes care of adding the actions to Total99 menus and of starting
//  * the functions in a worker thread if they are supposed to. To add operations
//  * you can either call addOperation() or use the DECLARE_THREAD_OPERATION,
//  * DECLARE_STEPPABLE_THREAD_OPERATION or DECLARE_IMMEDIATE_OPERATION macro
//  * (see below for more information) and then implement the slot with the
//  * operation. Both addOperation and the macros should be called in the
//  * experiment constructor, in the configure method or in the
//  * postConfigureInitialization method (because the list of operations is
//  * generally asked after these functions are called and there is no way to send
//  * notifications if the list changes). Here is an example:
//  *
//  * \code
//  * class MyExperiment : public BaseExperiment
//  * {
//  * 	Q_OBJECT
//  *
//  * public:
//  * 	MyExperiment();
//  * 	~MyExperiment();
//  * 	virtual void configure(ConfigurationParameters& params, QString prefix);
//  * 	virtual void save(ConfigurationParameters& params, QString prefix);
//  * 	static void describe(QString type);
//  * 	virtual void postConfigureInitialization();
//  *
//  * public slots:
//  * 	void run();
//  * 	void test();
//  * 	void otherOperation();
//  *
//  * private:
//  * 	...
//  * };
//  *
//  * // In implementation
//  * MyExperiment::MyExperiment() :
//  * 	BaseExperiment(),
//  * 	...
//  * {
//  * 	DECLARE_STEPPABLE_THREAD_OPERATION(MyExperiment, run)
//  * 	DECLARE_THREAD_OPERATION(MyExperiment, test)
//  * 	DECLARE_IMMEDIATE_OPERATION(MyExperiment, otherOperation)
//  *
//  * 	...
//  * }
//  * \endcode
//  *
//  * The difference between using the addOperation() function and the macros is
//  * that the function lets you define a custom name for the operation to be used
//  * in the Actions menù of Total99 but has a slightly more complex sintax, while
//  * the macros have a clearer syntax at the expense of some flexibility. Moreover
//  * the addOperation() function has overloads to define operations that take one
//  * or more parameters, binding the parameters to fixed values (see the
//  * description of the addOperation function). As you can see from the example
//  * above, your experiment must inherit from BaseExperiment. The Experiment class
//  * must have a default constructor and the Q_OBJECT macro must be present. This
//  * class always adds a stop operation to stop the current actions as the first
//  * operation in the list. Operations can run in three possible modalities, each
//  * corresponding to one of the DECLARE_*_OPERATION macro (see the description of
//  * the addOperation() function for information about which parameters to use to
//  * obtain the various execution modalities):
//  * 	- run operation in a different thread (DECLARE_THREAD_OPERATION). In
//  * 	  this case the function associated with the operation is run in a
//  * 	  thread different from the one in which the BaseExperiment subclass
//  * 	  instance lives (in Total99 components like BaseExperiment live in the
//  * 	  GUI thread). This means that care has to be taken when exchanging
//  * 	  data with the GUI (e.g. using the facilities of the classes described
//  * 	  in dataexchange.h), but also that the operation doesn't lock the GUI
//  * 	  thread, When implementing this kind of operations, the stopFlow()
//  * 	  function should be called regularly to check if a premature end of the
//  * 	  operation has been requested;
//  * 	- run operation in a different thread and provide the possibility to run
//  * 	  step-by-step (DECLARE_STEPPABLE_THREAD_OPERATION). This is the same
//  * 	  as the modality above (DECLARE_THREAD_OPERATION) with the additional
//  * 	  possiblity to pause the operation, perform single steps or put a delay
//  * 	  between two consecutive steps. How steps are defined is left to the
//  * 	  implementation: a step is the portion of code between two subsequent
//  * 	  calls to the pauseFlow() function;
//  * 	- run operation immediately (DECLARE_IMMEDIATE_OPERATION). In this case
//  * 	  the operation is run in the same thread of the BaseExperiment subclass
//  * 	  instance. This means that the execution of the operation is like a
//  * 	  classical function call: if the object lives in the GUI thread the GUI
//  * 	  will be non-responsive until the operation has finished executing.
//  * 	  Because of this, this modality should be used only for short
//  * 	  operations
//  * The stopFlow() and pauseFlow() functions are present because this inherits
//  * FlowControlled (they are protected functions of this class, even if
//  * FlowControlled itself is inherited as private to disallow changing the
//  * FlowController). This class also provides a public function flowController()
//  * that returns the FlowController object used by this class (that can be passed
//  * to other objects to share the same control logic).
//  * If the experiment needs one or more GUIs you can re-implement the
//  * getViewers() function (which is the same as ParameterSettableUI::getViewers)
//  * to return the list of available widgets. The default implementation of this
//  * function creates BaseExperimentGUI to control the base experiment execution,
//  * so if you want that GUI remember to call BaseExperiment::getViewers() when
//  * reimplementing that function. If more menus are required, the experiment can
//  * re-implement the addAdditionalMenus() function (which is the same as
//  * ParameterSettableUI::addAdditionalMenus())
//  *
//  * \note Most of the functions of this class are NOT thread-safe, so you should
//  *       not call them from slots implementing thread operations. If a function
//  *       is thread safe, the function documentation states it explicitly
//  * \note When in subclasses you have threaded operations, you must be sure that
//  *       no operation is running (e.g. by calling stopCurrentOperation()) before
//  *       destroying the object. In fact the thread is stopped in BaseExperiment
//  *       destructor, but subclasses destructor is called before the one of
//  *       BaseExperiment, so you could end up deleting stuffs while an operation
//  *       is still running.
//  */
// class SALSA_EXPERIMENTS_API BaseExperiment : public Component, public ParameterSettableUI, public ThreadOperation, private FlowControlled
// {
// 	Q_OBJECT
//
// private:
// 	/**
// 	 * \brief The class to receive notification of status changes
// 	 *
// 	 * We have this class instead of making BaseExperiment directly inherit
// 	 * NewDatumNotifiable to make the life easier to users in case
// 	 * subclasses also inherit from NewDatumNotifiable (there could be
// 	 * ambigiuties in data type names)
// 	 * \internal
// 	 */
// 	class Notifee : public NewDatumNotifiable<__BaseExperiment_internal::OperationControl>
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param experiment the experiment
// 		 */
// 		Notifee(BaseExperiment& experiment);
//
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~Notifee();
//
// 		/**
// 		 * \brief The function called when a new OperationControl datum
// 		 *        is available
// 		 *
// 		 * \param downloader the downloader with the available datum
// 		 */
// 		virtual void newDatumAvailable(DataDownloader<__BaseExperiment_internal::OperationControl>* downloader);
//
// 	private:
// 		/**
// 		 * \brief The experiment
// 		 */
// 		BaseExperiment& m_experiment;
// 	};
//
// 	/**
// 	 * \brief The Notifee class is friend to access our private members and
// 	 *        functions
// 	 */
// 	friend class Notifee;
//
// 	/**
// 	 * \brief The flow controller for the BaseExperiment
// 	 *
// 	 * An instance of this is returned by flowController(). It simply uses
// 	 * the functions of the BaseExperiment instance to control flow
// 	 * \internal
// 	 */
// 	class BaseExperimentFlowController : public FlowController
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param baseExperiment the instance of BaseExperiment whose
// 		 *                       flow we control
// 		 */
// 		BaseExperimentFlowController(BaseExperiment* baseExperiment);
//
// 		/**
// 		 * \brief The function to check if execution should be
// 		 *        terminated as soon as possible
// 		 *
// 		 * This simply calls stopSimulation() of the base experiment
// 		 * \return true if execution should be terminated as soon as
// 		 *         possible
// 		 */
// 		virtual bool stop();
//
// 		/**
// 		 * \brief Performs a pause if needed
// 		 *
// 		 * This simply calls checkPause
// 		 */
// 		virtual void pause();
//
// 	private:
// 		/**
// 		 * \brief The instance of BaseExperiment whose flow we control
// 		 */
// 		BaseExperiment* m_baseExperiment;
// 	};
//
// 	/**
// 	 * \brief The BaseExperimentFlowController class is friend to access our
// 	 *        private members and functions
// 	 */
// 	friend class BaseExperimentFlowController;
//
// public:
// 	/**
// 	 * \brief The base abstract class for operation wrappers
// 	 *
// 	 * This and the template class below are needed to be able to add and
// 	 * use operations from derived classes
// 	 */
// 	class AbstractOperationWrapper
// 	{
// 	public:
// 		/**
// 		 * \brief Returns the name of the operation
// 		 */
// 		QString getName() const
// 		{
// 			return name;
// 		}
//
// 		/**
// 		 * \brief Returns true if the operation is run in a separate
// 		 *        thread
// 		 */
// 		bool getUseSeparateThread() const
// 		{
// 			return useSeparateThread;
// 		}
//
// 		/**
// 		 * \brief Returns true if the operation can be run step-by-step
// 		 */
// 		bool getSteppable() const
// 		{
// 			return steppable;
// 		}
//
// 	private:
// 		/**
// 		 * \brief Destructor
// 		 *
// 		 * This does nothing. This is private so that only
// 		 * BaseExperiment can delete us
// 		 */
// 		virtual ~AbstractOperationWrapper()
// 		{
// 		}
//
// 		/**
// 		 * \brief The function calling the operation
// 		 *
// 		 * This is private so that only BaseExperiment (which is friend)
// 		 * can call it
// 		 */
// 		virtual void executeOperation() = 0;
//
// 		/**
// 		 * \brief The name of the operation
// 		 */
// 		QString name;
//
// 		/**
// 		 * \brief If true the operation should be run in a separate
// 		 *        thread
// 		 */
// 		bool useSeparateThread;
//
// 		/**
// 		 * \brief If true the operation can be run step-by-step
// 		 */
// 		bool steppable;
//
// 		/**
// 		 * \brief BaseExperiment is friend to be able to call
// 		 *        executeOperation() and to modify member data
// 		 */
// 		friend class BaseExperiment;
//
// 		/**
// 		 * \brief auto_ptr is friend to be able to call the destructor
// 		 */
// 		template <class T>
// 		friend class std::auto_ptr;
// 	};
//
// 	/**
// 	 * \brief The class for operation wrappers
// 	 *
// 	 * This class is needed to be able to add and use operations from
// 	 * derived classes
// 	 */
// 	template <class T>
// 	class OperationWrapper : public AbstractOperationWrapper
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param experiment the pointer to the experiment
// 		 * \param operation the pointer to the member function of
// 		 *                  experiment implementing the operation
// 		 */
// 		OperationWrapper(T* experiment, void (T::*operation)())
// 			: AbstractOperationWrapper()
// 			, m_experiment(experiment)
// 			, m_operation(operation)
// 		{
// 		}
//
// 	private:
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~OperationWrapper()
// 		{
// 		}
//
// 		/**
// 		 * \brief The function calling the operation
// 		 */
// 		virtual void executeOperation()
// 		{
// 			(m_experiment->*m_operation)();
// 		}
//
// 		/**
// 		 * \brief A pointer to the experiment
// 		 */
// 		T* const m_experiment;
//
// 		/**
// 		 * \brief A pointer to the member function of experiment
// 		 *        implementing the operation
// 		 */
// 		void (T::*m_operation)();
// 	};
//
// 	/**
// 	 * \brief The class for operation wrappers taking one parameter
// 	 *
// 	 * This class is needed to be able to add and use operations from
// 	 * derived classes that take one parameter
// 	 */
// 	template <class T, class P0>
// 	class OperationWrapperOneParameter : public AbstractOperationWrapper
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param experiment the pointer to the experiment
// 		 * \param operation the pointer to the member function of
// 		 *                  experiment implementing the operation
// 		 * \param p0 the parameter that is passed to the function
// 		 */
// 		OperationWrapperOneParameter(T* experiment, void (T::*operation)(P0), P0 p0)
// 			: AbstractOperationWrapper()
// 			, m_experiment(experiment)
// 			, m_operation(operation)
// 			, m_p0(p0)
// 		{
// 		}
//
// 		/**
// 		 * \brief Returns the parameter that is passed to the function
// 		 *
// 		 * \return the parameter that is passed to the function
// 		 */
// 		const P0& p0() const
// 		{
// 			return m_p0;
// 		}
//
// 	private:
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~OperationWrapperOneParameter()
// 		{
// 		}
//
// 		/**
// 		 * \brief The function calling the operation
// 		 */
// 		virtual void executeOperation()
// 		{
// 			(m_experiment->*m_operation)(m_p0);
// 		}
//
// 		/**
// 		 * \brief A pointer to the experiment
// 		 */
// 		T* const m_experiment;
//
// 		/**
// 		 * \brief A pointer to the member function of experiment
// 		 *        implementing the operation
// 		 */
// 		void (T::*m_operation)(P0);
//
// 		/**
// 		 * \brief The parameter that is passed to the function
// 		 */
// 		const P0 m_p0;
// 	};
//
// 	/**
// 	 * \brief The class for operation wrappers taking two parameters
// 	 *
// 	 * This class is needed to be able to add and use operations from
// 	 * derived classes that take two parameters
// 	 */
// 	template <class T, class P0, class P1>
// 	class OperationWrapperTwoParameters : public AbstractOperationWrapper
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param experiment the pointer to the experiment
// 		 * \param operation the pointer to the member function of
// 		 *                  experiment implementing the operation
// 		 * \param p0 the first parameter that is passed to the function
// 		 * \param p1 the second parameter that is passed to the function
// 		 */
// 		OperationWrapperTwoParameters(T* experiment, void (T::*operation)(P0, P1), P0 p0, P1 p1)
// 			: AbstractOperationWrapper()
// 			, m_experiment(experiment)
// 			, m_operation(operation)
// 			, m_p0(p0)
// 			, m_p1(p1)
// 		{
// 		}
//
// 		/**
// 		 * \brief Returns the first parameter that is passed to the
// 		 *        function
// 		 *
// 		 * \return the first parameter that is passed to the function
// 		 */
// 		const P0& p0() const
// 		{
// 			return m_p0;
// 		}
//
// 		/**
// 		 * \brief Returns the second parameter that is passed to the
// 		 *        function
// 		 *
// 		 * \return the second parameter that is passed to the function
// 		 */
// 		const P1& p1() const
// 		{
// 			return m_p1;
// 		}
//
// 	private:
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~OperationWrapperTwoParameters()
// 		{
// 		}
//
// 		/**
// 		 * \brief The function calling the operation
// 		 */
// 		virtual void executeOperation()
// 		{
// 			(m_experiment->*m_operation)(m_p0, m_p1);
// 		}
//
// 		/**
// 		 * \brief A pointer to the experiment
// 		 */
// 		T* const m_experiment;
//
// 		/**
// 		 * \brief A pointer to the member function of experiment
// 		 *        implementing the operation
// 		 */
// 		void (T::*m_operation)(P0, P1);
//
// 		/**
// 		 * \brief The first parameter that is passed to the function
// 		 */
// 		const P0 m_p0;
//
// 		/**
// 		 * \brief The second parameter that is passed to the function
// 		 */
// 		const P1 m_p1;
// 	};
//
// 	/**
// 	 * \brief The class for operation wrappers taking three parameters
// 	 *
// 	 * This class is needed to be able to add and use operations from
// 	 * derived classes that take three parameters
// 	 */
// 	template <class T, class P0, class P1, class P2>
// 	class OperationWrapperThreeParameters : public AbstractOperationWrapper
// 	{
// 	public:
// 		/**
// 		 * \brief Constructor
// 		 *
// 		 * \param experiment the pointer to the experiment
// 		 * \param operation the pointer to the member function of
// 		 *                  experiment implementing the operation
// 		 * \param p0 the first parameter that is passed to the function
// 		 * \param p1 the second parameter that is passed to the function
// 		 * \param p1 the third parameter that is passed to the function
// 		 */
// 		OperationWrapperThreeParameters(T* experiment, void (T::*operation)(P0, P1, P2), P0 p0, P1 p1, P2 p2)
// 			: AbstractOperationWrapper()
// 			, m_experiment(experiment)
// 			, m_operation(operation)
// 			, m_p0(p0)
// 			, m_p1(p1)
// 			, m_p2(p2)
// 		{
// 		}
//
// 		/**
// 		 * \brief Returns the first parameter that is passed to the
// 		 *        function
// 		 *
// 		 * \return the first parameter that is passed to the function
// 		 */
// 		const P0& p0() const
// 		{
// 			return m_p0;
// 		}
//
// 		/**
// 		 * \brief Returns the second parameter that is passed to the
// 		 *        function
// 		 *
// 		 * \return the second parameter that is passed to the function
// 		 */
// 		const P1& p1() const
// 		{
// 			return m_p1;
// 		}
//
// 		/**
// 		 * \brief Returns the third parameter that is passed to the
// 		 *        function
// 		 *
// 		 * \return the third parameter that is passed to the function
// 		 */
// 		const P2& p2() const
// 		{
// 			return m_p2;
// 		}
//
// 	private:
// 		/**
// 		 * \brief Destructor
// 		 */
// 		virtual ~OperationWrapperThreeParameters()
// 		{
// 		}
//
// 		/**
// 		 * \brief The function calling the operation
// 		 */
// 		virtual void executeOperation()
// 		{
// 			(m_experiment->*m_operation)(m_p0, m_p1, m_p2);
// 		}
//
// 		/**
// 		 * \brief A pointer to the experiment
// 		 */
// 		T* const m_experiment;
//
// 		/**
// 		 * \brief A pointer to the member function of experiment
// 		 *        implementing the operation
// 		 */
// 		void (T::*m_operation)(P0, P1, P2);
//
// 		/**
// 		 * \brief The first parameter that is passed to the function
// 		 */
// 		const P0 m_p0;
//
// 		/**
// 		 * \brief The second parameter that is passed to the function
// 		 */
// 		const P1 m_p1;
//
// 		/**
// 		 * \brief The third parameter that is passed to the function
// 		 */
// 		const P1 m_p2;
// 	};
//
// public:
// 	/**
// 	 * \brief Constructor
// 	 */
// 	BaseExperiment();
//
// 	/**
// 	 * \brief Destructor
// 	 *
// 	 * Before destroyng subclasses, you must be sure no operation is
// 	 * running. You can call stopCurrentOperation() to make sure this is
// 	 * true
// 	 */
// 	virtual ~BaseExperiment();
//
// 	/**
// 	 * \brief Configures the object using a ConfigurationParameters object
// 	 *
// 	 * \param params the configuration parameters object with parameters to
// 	 *               use
// 	 * \param prefix the prefix to use to access the object configuration
// 	 *               parameters. This is guaranteed to end with the
// 	 *               separator character when called by the factory, so you
// 	 *               don't need to add one
// 	 */
// 	virtual void configure(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Saves the actual status of parameters into the
// 	 *        ConfigurationParameters object passed
// 	 *
// 	 * \param params the configuration parameters object on which to save
// 	 *               the actual parameters
// 	 * \param prefix the prefix to use to access the object configuration
// 	 *               parameters
// 	 */
// 	virtual void save(ConfigurationParameters& params, QString prefix);
//
// 	/**
// 	 * \brief Add to Factory::typeDescriptions() the descriptions of all
// 	 *        parameters and subgroups
// 	 *
// 	 * It's mandatory in all subclasses where configure and save methods
// 	 * have been re-implemented for dealing with new parameters and
// 	 * subgroups to also implement the describe method
// 	 * \param type is the name of the type regarding the description. The
// 	 *             type is used when a subclass reuse the description of its
// 	 *             parent calling the parent describe method passing the
// 	 *             type of the subclass. In this way, the result of the
// 	 *             method describe of the parent will be the addition of the
// 	 *             description of the parameters of the parent class into
// 	 *             the type of the subclass
// 	 */
// 	static void describe(QString type);
//
// 	/**
// 	 * \brief This function is called after all linked objects have been
// 	 *        configured
// 	 *
// 	 * See the description of the ConfigurationParameters class for more
// 	 * information. This starts the inner thread
// 	 */
// 	virtual void postConfigureInitialization();
//
// 	/**
// 	 * \brief Returns an instance of the class handling our viewers
// 	 *
// 	 * This returns a pointer to the ParameterSettableUI object. That object
// 	 * simply calls functions in this class
// 	 * \return a pointer to the ParameterSettableUI object
// 	 * \note You should not override this function
// 	 */
// 	virtual ParameterSettableUI* getUIManager();
//
// 	/**
// 	 * \brief Fills the menu "Actions" of Total99
// 	 *
// 	 * This function takes care of filling the menu "Actions" of Total99
// 	 * with the actions declared by Experiment
// 	 * \param actionsMenu the "Actions" menu in Total99
// 	 * \note You should not override this function unless you have
// 	 *       particular needings
// 	 */
// 	virtual void fillActionsMenu(QMenu* actionsMenu);
//
// 	/**
// 	 * \brief Returns the list of viewers
// 	 *
// 	 * Override this function to return the list of all viewers for this
// 	 * experiment. The default implementation adds the BaseExperimentGUI
// 	 * widget, so call this function in subclasses if you want that window
// 	 * \param parent the parent widget for all viewers created by this
// 	 *               function
// 	 * \param flags the window flags to specify when constructing the
// 	 *              widgets
// 	 * \return the list of widgets
// 	 */
// 	virtual QList<ParameterSettableUIViewer> getViewers(QWidget* parent, Qt::WindowFlags flags);
//
// 	/**
// 	 * \brief Adds additional menus to the menu bar of Total99
// 	 *
// 	 * \param menuBar the menu bar of the Total99 application
// 	 */
// 	virtual void addAdditionalMenus(QMenuBar* menuBar);
//
// 	/**
// 	 * \brief Runs the next operation
// 	 *
// 	 * This simply runs the next operation
// 	 * \note You should not override this method
// 	 */
// 	virtual void run();
//
// 	/**
// 	 * \brief Forces the end of the experiment
// 	 *
// 	 * This is the implementation of the stop() function of ThreadOperation.
// 	 * If the opeation is sleeping, it is also resumed
// 	 * \note You should not override this method
// 	 */
// 	virtual void stop();
//
// 	/**
// 	 * \brief Puts the current operation in pause
// 	 *
// 	 * This only works for steppable operations
// 	 * \note this function is thread-safe
// 	 */
// 	void pause();
//
// 	/**
// 	 * \brief Performs a single step for the current steppable operation
// 	 *
// 	 * The steppable operation must be paused, otherwise this function does
// 	 * nothing
// 	 * \note this function is thread-safe
// 	 */
// 	void step();
//
// 	/**
// 	 * \brief Resumes the current operation if paused
// 	 *
// 	 * The operation must be paused, otherwise this function does nothing
// 	 * \note this function is thread-safe
// 	 */
// 	void resume();
//
// 	/**
// 	 * \brief Changes the delay between subsequent steps of a steppable
// 	 *        operation
// 	 *
// 	 * This only affects steppable operations. The default value of the
// 	 * delay is 0
// 	 * \param interval the new delay in milliseconds
// 	 * \note this function is thread-safe
// 	 */
// 	void changeInterval(unsigned long interval);
//
// 	/**
// 	 * \brief Returns the current delay for steppable operations
// 	 *
// 	 * \return the current delay for steppable operations
// 	 */
// 	unsigned long currentInterval() const;
//
// 	/**
// 	 * \brief Returns the list of operations
// 	 *
// 	 * \return the list of operations
// 	 */
// 	const QVector<AbstractOperationWrapper*>& getOperations() const;
//
// 	/**
// 	 * \brief Returns the object to exchange data
// 	 *
// 	 * This is public so that it is possible to use a custom GUI (in that
// 	 * case the getViewers() function must be overridden to prevent
// 	 * returning a BaseExperimentGUI)
// 	 * \return a pointer to the object to exchange data
// 	 */
// 	DataUploaderDownloader<__BaseExperiment_internal::OperationStatus, __BaseExperiment_internal::OperationControl>* getUploaderDownloader();
//
// 	/**
// 	 * \brief Returns the flow controller
// 	 *
// 	 * You can use it with other objects so that they share the same control
// 	 * strategy of this class
// 	 * \return the flow controller used by this class. You MUST not delete
// 	 *         it!
// 	 */
// 	FlowController* flowController()
// 	{
// 		return &m_flowController;
// 	}
//
// public slots:
// 	/**
// 	 * \brief Forces the end of the current operation, if threaded
// 	 *
// 	 * This is a slot
// 	 * \param wait if true waits for operation to actually stop, otherwise
// 	 *             returns immediately
// 	 */
// 	virtual void stopCurrentOperation(bool wait);
//
// 	/**
// 	 * \brief Forces the end of the current operation, if threaded
// 	 *
// 	 * This is a slot. This calls stopCurrentOperation(false)
// 	 */
// 	virtual void stopCurrentOperation();
//
// private slots:
// 	/**
// 	 * \brief Warns the user about an exception thrown by the experiment
// 	 *
// 	 * This is a slot. We have to use salsa::BaseException instead of simply
// 	 * BaseException because otherwise moc doesn't work as expected
// 	 * \param e the exception that has been thrown
// 	 */
// 	void exceptionDuringOperation(salsa::BaseException *e);
//
// 	/**
// 	 * \brief The slot connected to the signal mapper to start an operation
// 	 *
// 	 * This function receives a different integer depending on which
// 	 * operation has been triggered and starts it in the worker thread if it
// 	 * has to. Moreover this is a slot
// 	 * \param operationID the id of the operation to execute
// 	 */
// 	void runOperation(int operationID);
//
// protected:
// 	/**
// 	 * \brief Returns a list of actions, one for each operation declared by
// 	 *        the experiment
// 	 *
// 	 * This function can be used if you decide to re-implement the
// 	 * fillActionsMenu function to obtain the list of actions without
// 	 * filling the menu
// 	 * \param actionsParent the parent of generated QActions
// 	 * \return The list of actions, one for each operation declared by the
// 	 *         experiment
// 	 */
// 	QList<QAction*> getActionsForOperations(QObject* actionsParent) const;
//
// 	/**
// 	 * \brief The function that adds the given operation to the list of
// 	 *        all operations declared by the experiment
// 	 *
// 	 * This function should be called in the constructor, in the configre
// 	 * method or in the postConfigureInitialization method to declare all
// 	 * operations provided by the class. The DECLARE_*_OPERATION macros
// 	 * simply call this functions with the proper parameters. Here is the
// 	 * correspondence between parameters of this functions and the
// 	 * DECLARE_*_OPERATION macros:
// 	 * 	- useSeparateThread = false is like using the
// 	 * 	  DECLARE_IMMEDIATE_OPERATION
// 	 * 	- useSeparateThread = true AND steppable = false is like using
// 	 * 	  DECLARE_THREAD_OPERATION
// 	 * 	- useSeparateThread = true AND steppable = true is like using
// 	 * 	  DECLARE_STEPPABLE_THREAD_OPERATION
// 	 * \param name the name of the operations
// 	 * \param func the function implementing the operation (which should be
// 	 *             a slot)
// 	 * \param useSeparateThread if true the action is run in a separate
// 	 *                          thread, if false it is run in the thread of
// 	 *                          the caller
// 	 * \param steppable if useSeparateThread is true and this is also true,
// 	 *                  the operation is steppable, if useSeparateThread is
// 	 *                  false this value is ignored.
// 	 */
// 	template <class T>
// 	void addOperation(QString name, void (T::*func)(), bool useSeparateThread, bool steppable)
// 	{
// 		std::auto_ptr<AbstractOperationWrapper> newOp(new OperationWrapper<T>(static_cast<T*>(this), func));
// 		newOp->name = name;
// 		newOp->useSeparateThread = useSeparateThread;
// 		newOp->steppable = steppable;
//
// 		m_operationsVector.push_back(newOp.release());
//
// 		// Signalling a new operation has been added
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::NewOperation, m_operationsVector.size() - 1);
// 	}
//
// 	/**
// 	 * \brief The function that adds the given operation that takes one
// 	 *        parameter to the list of all operations declared by the
// 	 *        experiment
// 	 *
// 	 * This is an overload that allows using a function that takes one
// 	 * parameter as an operation by binding the parameter to a given value
// 	 * (passed as the p0 parameter to this function)
// 	 * \param name the name of the operations
// 	 * \param func the function implementing the operation (which should be
// 	 *             a slot)
// 	 * \param p0 the value of the parameter of the function
// 	 * \param useSeparateThread if true the action is run in a separate
// 	 *                          thread, if false it is run in the thread of
// 	 *                          the caller
// 	 * \param steppable if useSeparateThread is true and this is also true,
// 	 *                  the operation is steppable, if useSeparateThread is
// 	 *                  false this value is ignored.
// 	 */
// 	template <class T, class P0>
// 	void addOperation(QString name, void (T::*func)(P0), P0 p0, bool useSeparateThread, bool steppable)
// 	{
// 		std::auto_ptr<AbstractOperationWrapper> newOp(new OperationWrapperOneParameter<T, P0>(static_cast<T*>(this), func, p0));
// 		newOp->name = name;
// 		newOp->useSeparateThread = useSeparateThread;
// 		newOp->steppable = steppable;
//
// 		m_operationsVector.push_back(newOp.release());
//
// 		// Signalling a new operation has been added
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::NewOperation, m_operationsVector.size() - 1);
// 	}
//
// 	/**
// 	 * \brief The function that adds the given operation that takes two
// 	 *        parameters to the list of all operations declared by the
// 	 *        experiment
// 	 *
// 	 * This is an overload that allows using a function that takes two
// 	 * parameters as an operation by binding the parameters to given values
// 	 * (passed as the p0 and p1 parameters to this function)
// 	 * \param name the name of the operations
// 	 * \param func the function implementing the operation (which should be
// 	 *             a slot)
// 	 * \param p0 the value of the first parameter of the function
// 	 * \param p1 the value of the second parameter of the function
// 	 * \param useSeparateThread if true the action is run in a separate
// 	 *                          thread, if false it is run in the thread of
// 	 *                          the caller
// 	 * \param steppable if useSeparateThread is true and this is also true,
// 	 *                  the operation is steppable, if useSeparateThread is
// 	 *                  false this value is ignored.
// 	 */
// 	template <class T, class P0, class P1>
// 	void addOperation(QString name, void (T::*func)(P0, P1), P0 p0, P1 p1, bool useSeparateThread, bool steppable)
// 	{
// 		std::auto_ptr<AbstractOperationWrapper> newOp(new OperationWrapperTwoParameters<T, P0, P1>(static_cast<T*>(this), func, p0, p1));
// 		newOp->name = name;
// 		newOp->useSeparateThread = useSeparateThread;
// 		newOp->steppable = steppable;
//
// 		m_operationsVector.push_back(newOp.release());
//
// 		// Signalling a new operation has been added
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::NewOperation, m_operationsVector.size() - 1);
// 	}
//
// 	/**
// 	 * \brief The function that adds the given operation that takes three
// 	 *        parameters to the list of all operations declared by the
// 	 *        experiment
// 	 *
// 	 * This is an overload that allows using a function that takes three
// 	 * parameters as an operation by binding the parameters to given values
// 	 * (passed as the p0, p1 and p2 parameters to this function)
// 	 * \param name the name of the operations
// 	 * \param func the function implementing the operation (which should be
// 	 *             a slot)
// 	 * \param p0 the value of the first parameter of the function
// 	 * \param p1 the value of the second parameter of the function
// 	 * \param p2 the value of the third parameter of the function
// 	 * \param useSeparateThread if true the action is run in a separate
// 	 *                          thread, if false it is run in the thread of
// 	 *                          the caller
// 	 * \param steppable if useSeparateThread is true and this is also true,
// 	 *                  the operation is steppable, if useSeparateThread is
// 	 *                  false this value is ignored.
// 	 */
// 	template <class T, class P0, class P1, class P2>
// 	void addOperation(QString name, void (T::*func)(P0, P1, P2), P0 p0, P1 p1, P2 p2, bool useSeparateThread, bool steppable)
// 	{
// 		std::auto_ptr<AbstractOperationWrapper> newOp(new OperationWrapperThreeParameters<T, P0, P1, P2>(static_cast<T*>(this), func, p0, p1, p2));
// 		newOp->name = name;
// 		newOp->useSeparateThread = useSeparateThread;
// 		newOp->steppable = steppable;
//
// 		m_operationsVector.push_back(newOp.release());
//
// 		// Signalling a new operation has been added
// 		uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::NewOperation, m_operationsVector.size() - 1);
// 	}
//
// 	/**
// 	 * \brief Returns true if we are running in batch
// 	 *
// 	 * \return true if we are running in batch
// 	 */
// 	bool batchRunning() const;
//
// 	/**
// 	 * \brief Making the stopFlow function from FlowControlled protected
// 	 */
// 	using FlowControlled::stopFlow;
//
// 	/**
// 	 * \brief Making the pauseFlow function from FlowControlled protected
// 	 */
// 	using FlowControlled::pauseFlow;
//
// private:
// 	/**
// 	 * \brief Returns true if the simulation should stop as soon as possible
// 	 *
// 	 * This is used by the flow controller
// 	 * \return true if the simulation should stop as soon as possible
// 	 * \note This function is thread-safe
// 	 */
// 	bool stopSimulation();
//
// 	/**
// 	 * \brief Performs a pause in the simulation if needed
// 	 *
// 	 * Use this function to implement a steppable operation, calling it at
// 	 * the end of each step. This function will pause, sleep for a while or
// 	 * return immediately depending on how the simulation is run. This is
// 	 * called by the flow controller
// 	 * \note This function is thread-safe
// 	 */
// 	void checkPause();
//
// 	/**
// 	 * \brief Resets the status of m_stop to false
// 	 */
// 	void resetStop();
//
// 	/**
// 	 * \brief Uploads a new operation status
// 	 *
// 	 * \param status the new operation status
// 	 * \param opeationID the ID of the operation
// 	 * \param newDelay the new delay for steppable operations
// 	 * \note This function is not thread-safe, but it is called from
// 	 *       different threads, so be careful not calling it concurrently
// 	 */
// 	void uploadNewOperationStatus(__BaseExperiment_internal::OperationStatus::Status status, unsigned int operationID, unsigned long newDelay = 0);
//
// 	/**
// 	 * \brief Changes the delay between subsequent steps of a steppable
// 	 *        operation
// 	 *
// 	 * This only affects steppable operations. The default value of the
// 	 * delay is 0
// 	 * \param interval the new delay in milliseconds
// 	 * \param sendNotificationToGUI if true a notification of the change is
// 	 *                              sent to the GUI
// 	 * \note this function is thread-safe
// 	 */
// 	void changeInterval(unsigned long interval, bool sendNotificationToGUI);
//
// 	/**
// 	 * \brief The vector of operations
// 	 */
// 	QVector<AbstractOperationWrapper*> m_operationsVector;
//
// 	/**
// 	 * \brief The signal mapper
// 	 *
// 	 * This is used to map all signals from QActions to the runOperation()
// 	 * function so that it can run the selected slot in the worker thread
// 	 */
// 	std::auto_ptr<QSignalMapper> m_actionSignalsMapper;
//
// 	/**
// 	 * \brief The thread running the operations
// 	 */
// 	std::auto_ptr<WorkerThread> const m_workerThread;
//
// 	/**
// 	 * \brief The ID of the operation to run
// 	 *
// 	 * This is the ID of the operation to execute. If it is less than 0 then
// 	 * it is an invalid operation ID
// 	 */
// 	int m_runningOperationID;
//
// 	/**
// 	 * \brief True if we are run in batch
// 	 */
// 	bool m_batchRunning;
//
// 	/**
// 	 * \brief If true the simulation must stop as soon as possible
// 	 */
// 	bool m_stop;
//
// 	/**
// 	 * \brief The mutex protecting from concurrent accesses to data of this
// 	 *        class
// 	 */
// 	QMutex m_mutex;
//
// 	/**
// 	 * \brief The wait condition on which steppable threaded operation wait
// 	 *        when in pause
// 	 */
// 	QWaitCondition m_waitCondition;
//
// 	/**
// 	 * \brief If true the current steppable operation is paused
// 	 */
// 	bool m_pause;
//
// 	/**
// 	 * \brief The status of m_pause at the previous step
// 	 *
// 	 * We need this to understand if the operation was resumed
// 	 */
// 	bool m_previousPauseStatus;
//
// 	/**
// 	 * \brief The delay between steps in steppable operations
// 	 */
// 	unsigned long m_delay;
//
// 	/**
// 	 * \brief The object that receives notifications from the GUI
// 	 */
// 	Notifee m_notifee;
//
// 	/**
// 	 * \brief The object to exchange data with the GUI
// 	 *
// 	 * We don't Inherit directly from DataUploaderDownloader to make the
// 	 * life easier to users in case subclasses also inherit from
// 	 * NewDatumNotifiable (there could be ambigiuties in data type names).
// 	 * Moreover we use a quele length for the producer of 2 because of
// 	 * possible problems with QT event notification (see the note at the end
// 	 * of dataexchange.h documentation for a description of the possible
// 	 * problem)
// 	 */
// 	DataUploaderDownloader<__BaseExperiment_internal::OperationStatus, __BaseExperiment_internal::OperationControl> m_dataExchange;
//
// 	/**
// 	 * \brief The flow controller object
// 	 */
// 	BaseExperimentFlowController m_flowController;
// };
//
// } // End namespace salsa
//
// /**
//  * \brief The macro to add an operation to an experiment that runs in a separate
//  *        thread
//  *
//  * \param classname the name of the class defining the operation
//  * \param op the name of the operation (i.e. the name of the slot implementing
//  *           the operation)
//  */
// #define DECLARE_THREAD_OPERATION(classname, op) addOperation(#op, &classname::op, true, false);
//
// /**
//  * \brief The macro to add an operation to an experiment that runs in a separate
//  *        thread and is steppable
//  *
//  * \param classname the name of the class defining the operation
//  * \param op the name of the operation (i.e. the name of the slot implementing
//  *           the operation)
//  */
// #define DECLARE_STEPPABLE_THREAD_OPERATION(classname, op) addOperation(#op, &classname::op, true, true);
//
// /**
//  * \brief The macro to add an operation to an experiment that runs in the
//  *        calling thread
//  *
//  * \param classname the name of the class defining the operation
//  * \param op the name of the operation (i.e. the name of the slot implementing
//  *           the operation)
//  */
// #define DECLARE_IMMEDIATE_OPERATION(classname, op) addOperation(#op, &classname::op, false, false);
//
// #endif
