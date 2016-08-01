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

#include "evorobotcomponent.h"
#include "evorobotviewer.h"
#include "abstracttest.h"
#include "tests.h"
#include "configurationhelper.h"
#include "logger.h"

namespace salsa {

/*! the operation for evolution process */
class EvolveOperation : public ThreadOperation {
public:
	EvolveOperation( Evoga* ga, EvoRobotComponent* evorobot ) :
		ga(ga),
		evorobot(evorobot) { };
	virtual ~EvolveOperation() { };
	/*! stop the operation */
	virtual void stop() {
		ga->stop();
	};
	/*! do the evolution process */
	virtual void run() {
		ga->getEvoRobotExperiment()->setActivityPhase( EvoRobotExperiment::INEVOLUTION );
		ga->evolveAllReplicas();
		QString message;
		if ( ga->isStopped() ) {
			message = "Evolution Stopped";
		} else {
			message = "Evolution Finished";
		}
		// reset the stopped status
		ga->resetStop();
		evorobot->onThreadOperationDone( message );
	};
private:
	Evoga* ga;
	EvoRobotComponent* evorobot;
};

/*! the operation for testing process */
class TestOperation : public ThreadOperation {
public:
	TestOperation( AbstractTest* test, Evoga* ga, EvoRobotComponent* evorobot ) :
		test(test),
		ga(ga),
		evorobot(evorobot) { };
	virtual ~TestOperation() { };
	/*! stop the operation */
	virtual void stop() {
		ga->stop();
	};
	/*! do the test process */
	virtual void run() {
		test->runTest();
		QString message;
		if ( ga->isStopped() ) {
			message = "Test Stopped";
		} else {
			message = "Test Finished";
		}
		// reset the stopped status
		ga->resetStop();
		evorobot->onThreadOperationDone( message );
	};
private:
	AbstractTest* test;
	Evoga* ga;
	EvoRobotComponent* evorobot;
};

EvoRobotComponent::EvoRobotComponent(ConfigurationManager& params)
	: Component(params)
	, ga(NULL)
	, batchRunning(false)
	, runningOperation(false)
	, mutex()
{
	gaThread = new WorkerThread( this );
	testRandom = new TestRandom();
	testRandom->setComponent(this);
	testCurrent = new TestCurrent();
	testCurrent->setComponent(this);
	testIndividual = new TestIndividual();
	testIndividual->setComponent(this);

	connect(gaThread, SIGNAL(exceptionDuringOperation(salsa::BaseException*)), this, SLOT(exceptionDuringOperation(salsa::BaseException*)), Qt::BlockingQueuedConnection);
}

EvoRobotComponent::~EvoRobotComponent() {
	gaThread->quit();
	delete gaThread;
	delete ga;
}

ComponentUI* EvoRobotComponent::getUIManager() {
	return new EvoRobotViewer( this );
}

void EvoRobotComponent::configure()
{
	batchRunning = ConfigurationHelper::getBool(configurationManager(), "__INTERNAL__/BatchRunning", batchRunning);
	ga = configurationManager().getComponentFromGroup<Evoga>(confPath() + "GA");

#warning ======================================= RE-ENABLE THIS =======================================
// 	// Now we also instantiate all test classes. The configuration file should have subgroups
// 	// [TESTS:NN] where NN is a progressive number starting from zero.
// 	// Each group describe the test (in particular the group must have a type
// 	// parameter corresponding the the type of the Test to run).
// 	// The tests indicated by the following boolean flags are added by default if not present on
// 	// the configuration file
// // 	bool testAllAdded = false;
// 	bool testIndividualAdded = false;
// 	bool testRandomAdded = false;
// // 	bool testZeroAdded = false;
// 	bool testCurrentAdded = false;
// 	// clear the old content of currentTestName, it will be configured later
// 	currentTestName = QString();
// 	QStringList testGroups = params.getGroupsWithPrefixList( prefix, "TEST:" );
// 	for( int i=0; i<testGroups.size(); i++ ) {
// 		if ( i==0 ) {
// 			//--- currentTestName is initially setted to contain the name of the first test specified by the user
// 			currentTestName = testGroups[i];
// 		}
// 		AbstractTest* test = params.getObjectFromGroup<AbstractTest>(prefix+testGroups[i]);
// 		test->setComponent( this );
// 		AvailableTestList::addTest( testGroups[i], test );
// 		/*
// 		if (dynamic_cast<TestAll *>(test) != NULL) {
// 			testAllAdded = true;
// 		} else */ if (dynamic_cast<TestIndividual *>(test) != NULL) {
// 			testIndividualAdded = true;
// 		} else if (dynamic_cast<TestRandom *>(test) != NULL) {
// 			testRandomAdded = true;
// 		} else if (dynamic_cast<TestCurrent *>(test) != NULL) {
// 			testCurrentAdded = true;
// 		} /* else if (dynamic_cast<TestZero *>(test) != NULL) {
// 			testZeroAdded = true;
// 		} */
// 	}
// 	/*
// 	if (!testAllAdded) {
// 		// TestAll is not present among tests configured by the user, adding it. We know for sure
// 		// TestAll doesn't need to be configured, so we don't
// 		AbstractTest* test = new TestAll();
// 		test->setComponent( this );
// 		AvailableTestList::addTest("TestAll", test);
// 	}
// 	*/
// 	if (!testIndividualAdded) {
// 		// TestBest is not present among tests configured by the user, adding it. We know for sure
// 		// TestBest doesn't need to be configured, so we don't
// 		AbstractTest* test = new TestIndividual();
// 		test->setComponent( this );
// 		AvailableTestList::addTest("TestIndividual", test);
// 	}
// 	if (!testRandomAdded) {
// 		// TestRandom is not present among tests configured by the user, adding it. We know for sure
// 		// TestRnadom doesn't need to be configured, so we don't
// 		AbstractTest* test = new TestRandom();
// 		test->setComponent( this );
// 		AvailableTestList::addTest("TestRandom", test);
// 	}
// 	if (!testCurrentAdded) {
// 		// TestRandom is not present among tests configured by the user, adding it. We know for sure
// 		// TestRnadom doesn't need to be configured, so we don't
// 		AbstractTest* test = new TestCurrent();
// 		test->setComponent( this );
// 		AvailableTestList::addTest("TestCurrent", test);
// 	}
// 	/*
// 	if(!testZeroAdded) {
// 		// TestZero is not present among tests configured by the user, adding it. We know for sure
// 		// TestZero doesn't need to be configured, so we don't
// 		AbstractTest* test = new TestZero();
// 		test->setComponent( this );
// 		AvailableTestList::addTest("TestZero", test);
// 	}
// 	*/
//
// 	if ( currentTestName.isEmpty() ) {
// 		// there is no tests specified by the user and then the defaul currentTestName in this
// 		// case will be the automatically added "TestIndividual"
// 		currentTestName = "TestIndividual";
// 	}
// 	currentTestName = ConfigurationHelper::getString( params, prefix+"testToRun", currentTestName );

	// start the innerThread
	gaThread->start();
}

void EvoRobotComponent::postConfigureInitialization() {
}

void EvoRobotComponent::stopCurrentOperation()
{
	stop();
}

void EvoRobotComponent::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("This component allow to do evolutionary robotics experiments" );
	d.describeSubgroup( "GA" ).componentType( "Evoga" ).props( ParamIsMandatory ).help( "The Genetic Algorithm to use to perform the evolutionary process" );
	d.describeSubgroup( "TEST" ).componentType( "AbstractTest" ).props( ParamAllowMultiple ).help( "let to configure a test condition for the evolved individual" );
}

Evoga* EvoRobotComponent::getGA() {
	return ga;
}

void EvoRobotComponent::stop() {
	gaThread->stopCurrentOperation(false);
}

void EvoRobotComponent::evolve()
{
	if ( batchRunning ) {
		ga->getEvoRobotExperiment()->setActivityPhase( EvoRobotExperiment::INEVOLUTION );
		ga->evolveAllReplicas();
		ga->resetStop();
	} else {
		mutex.lock();
		if ( runningOperation ) {
			mutex.unlock();
			Logger::error( "EvoRobotComponent - Cannot run evolve because another action is currently running; please wait until it finish, or stop it before run evolve action" );
			return;
		}
		runningOperation = true;
		mutex.unlock();
		gaThread->addOperation( new EvolveOperation( ga, this ) );
	}
}

void EvoRobotComponent::runTest() {
#warning ======================================= RE-ENABLE THIS =======================================
// 	AbstractTest* test = AvailableTestList::getTest( currentTestName );
// 	if ( !test ) {
// 		Logger::error( QString("EvoRobotComponent - Test ") + currentTestName + " is not present into the list of available Tests" );
// 		return;
// 	}
// 	if ( batchRunning ) {
// 		test->runTest();
// 		ga->resetStop();
// 	} else {
// 		mutex.lock();
// 		if ( runningOperation ) {
// 			mutex.unlock();
// 			Logger::error( QString("EvoRobotComponent - Cannot run test %1 because another action is currently running; please wait until it finish, or stop it before run test %1").arg(currentTestName) );
// 			return;
// 		}
// 		runningOperation = true;
// 		mutex.unlock();
// 		gaThread->addOperation( new TestOperation( test, ga, this ) );
// 	}
}

void EvoRobotComponent::runTestFromQAction() {
#warning ======================================= RE-ENABLE THIS =======================================
// 	// get the name of the test from the data into QAction
// 	QAction* action = dynamic_cast<QAction*>( sender() );
// 	if ( !action ) {
// 		Logger::error( "EvoRobotComponent - runTestFromQAction can only be called by a QAction" );
// 		return;
// 	}
// 	currentTestName = action->data().toString();
// 	runTest();
}

void EvoRobotComponent::exceptionDuringOperation(BaseException *e) {
	// This is never called in batch, exception are catched in total99 main
	Logger::error( QString("EvoRobotComponent - Error while executing the current operation, an exception was thrown. Reason: ") + e->what() );
}

void EvoRobotComponent::onThreadOperationDone( QString message ) {
	mutex.lock();
	runningOperation = false;
	mutex.unlock();
	emit actionFinished();
}

void EvoRobotComponent::runTestRandom()
{
	runTest(testRandom);
}

void EvoRobotComponent::runTestCurrent()
{
	runTest(testCurrent);
}

void EvoRobotComponent::runTestIndividual()
{
	runTest(testIndividual);
}

void EvoRobotComponent::runTest(AbstractTest* test)
{
	if ( batchRunning ) {
		Logger::warning("Tests in batch not working");
	} else {
		mutex.lock();
		if ( runningOperation ) {
			mutex.unlock();
			Logger::error( QString("EvoRobotComponent - Cannot run Test because another action is currently running; please wait until it finish, or stop it before running Test"));
			return;
		}
		runningOperation = true;
		mutex.unlock();
		gaThread->addOperation( new TestOperation( test, ga, this ) );
	}
}

} // end namespace salsa
