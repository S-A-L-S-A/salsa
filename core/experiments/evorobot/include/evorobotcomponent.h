/********************************************************************************
 *  SALSA Experiments Library                                                   *
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

#ifndef EVOROBOTCOMPONENT_H
#define EVOROBOTCOMPONENT_H

#include "component.h"
#include "evoga.h"
#include "workerthread.h"
#include "baseexception.h"
#include "tests.h"
#include <QMutex>

namespace salsa {

/*! \brief This class setup an evolutionary experiment
 *
 *
 *  \ingroup experiments_utils
 */
class SALSA_EXPERIMENTS_API EvoRobotComponent : public QObject, public Component
{
	Q_OBJECT

public:
	static bool configuresInConstructor()
	{
		return false;
	}

public:
	/*! Constructor */
	EvoRobotComponent(ConfigurationManager& params);
	/*! Destructor */
	~EvoRobotComponent();
	/*! Return an instance of EvoRobotViewer that manage the viewers */
	ComponentUI* getUIManager();
	/*! Configure function */
	virtual void configure();
	/*! declare resources and initialize sensors */
	virtual void postConfigureInitialization();
	/**
	 * \brief This stops the current running operation
	 */
	virtual void stopCurrentOperation();
	/*! Description of all parameters */
	static void describe(RegisteredComponentDescriptor& d);
	/*! Return the Evoga */
	Evoga* getGA();

	TestIndividual* getTestIndividual()
	{
		return testIndividual;
	}
public slots:
	/*! Suspend the evolutionary process */
	void stop();
	/*! Start the evolutionary process (it automatically disable "step-by-step" modality) */
	void evolve();
	/*! Run the test associated to the parameter testToRun */
	void runTest();
	/*! Run a Test associated to the QAction whom triggered this slot */
	void runTestFromQAction();
	/*! Warns the user about the exception thrown by evolution or tests
	 *  (only if not in batch running) */
	void exceptionDuringOperation(salsa::BaseException *e);
	/*! executed when the current thread operation finish
	 *  \param message is the message about the operation done
	 */
	void onThreadOperationDone( QString message );

	void runTestRandom();
	void runTestCurrent();
	void runTestIndividual();
signals:
	/*! emitted when the action (evolve, test, ...) has been finished/stopped */
	void actionFinished();
private:
	void runTest(AbstractTest* test);
	/*! The Genetic Algorithm */
	Evoga* ga;
	/*! the thread for running the evolution */
	WorkerThread* gaThread;
	/*! batchRunning - if true EvoRobotComponent will not use the gaThread */
	bool batchRunning;
	/*! a ThreadOperation is currently running */
	bool runningOperation;
	/*! mutex for thread-safety */
	QMutex mutex;
	/*! the name of the current test selected to run */
	QString currentTestName;

	// Tests, added directly, for the moment
	TestRandom* testRandom;
	TestCurrent* testCurrent;
	TestIndividual* testIndividual;
};

} // end namespace salsa

#endif
