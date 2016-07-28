/***************************************************************************
 *  SALSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "configurationconfig.h"
#include "configurationmanager.h"
#include <QThread>
#include <QVector>
#include <QStack>

/**
 * \brief An operation to be run in a thread
 *
 * Inherit and implement the run() function. T is the object to share among all
 * concurrent operation objects (see the Runner class description)
 */
template <class T>
class Operation
{
public:
	/**
	 * \brief The type of the object to be shared
	 */
	typedef T ObjectType;

public:
	/**
	 * \brief The function implementing the operation
	 *
	 * \param object an object shared among different operations
	 * \param startIndex the starting unique value for this instance
	 * \param endIndex the end of unique values for this instance (endIndex
	 *                 is NOT included)
	 */
	virtual void run(T& object, int startIndex, int endIndex) = 0;

	/**
	 * \brief Returns the number of runners that are being run
	 *
	 * \return the number of runners that are being run
	 */
	int numRunners() const
	{
		return m_numRunners;
	}

	/**
	 * \brief Returns how many indexes each runner has
	 *
	 * \return how many indexes each runner has
	 */
	int numIndexesPerRunner() const
	{
		return m_numIndexesPerRunner;
	}

	/**
	 * \brief Returns how much time each runner can last at most
	 *
	 * \return how much time each runner can last at most
	 */
	unsigned long time() const
	{
		return m_time;
	}

private:
	int m_numRunners;
	int m_numIndexesPerRunner;
	unsigned long m_time;

	// The Runner class is friend to set the private fields. We do not pass
	// them to the constructor so that subclasses can use the default
	// constructor (making the operation easier to write)
	template <class OperationType>
	friend class Runner;
};

/**
 * \brief An operation to be run in a thread
 *
 * The difference with the Operation class above (from which this inherit) is
 * that the function to re-implement (runSingle) gets one index to test, the
 * cycle is implemented here
 */
template <class T>
class SingleOperation : public Operation<T>
{
public:
	/**
	 * \brief The function implementing the operation
	 *
	 * This is implemented in this class, override the runSingle() function
	 * \param object an object shared among different operations
	 * \param startIndex the starting unique value for this instance
	 * \param endIndex the end of unique values for this instance (endIndex
	 *                 is NOT included)
	 */
	virtual void run(T& object, int startIndex, int endIndex)
	{
		for (int i = startIndex; i < endIndex; ++i) {
			runSingle(object, i);
		}
	}

	/**
	 * \brief The function implementing the operation
	 *
	 * \param object an object shared among different operations
	 * \param index the index to test
	 */
	virtual void runSingle(T& object, int index) = 0;
};

/**
 * \brief A class to run multithreaded test
 *
 * When instantiating this class, pass an instance of an object that should be
 * shared among all Operations plus two integers. Values from startIndex
 * included to endIndex not included must to be unique for an instance of this
 * class. When run, this creates and instance of an Operation object of type
 * OperationType and calls the Operation::run() function
 */
template <class OperationType>
class Runner : public QThread
{
public:
	/**
	 * \brief The type of the object to be shared
	 */
	typedef typename OperationType::ObjectType ObjectType;

public:
	/**
	 * \brief Constructor
	 *
	 * \param object the object to share among all Runners
	 * \param startIndex the starting unique value for this instance
	 * \param endIndex the end of unique values for this instance (endIndex
	 *                 is NOT included)
	 * \param numRunners the number of runners that are being run
	 * \param numIndexesPerRunner how many indexes each runner has
	 * \param time how much time each runner can last at most
	 */
	Runner(ObjectType& object, int startIndex, int endIndex, int numRunners, int numIndexesPerRunner, unsigned long time)
		: m_object(object)
		, m_startIndex(startIndex)
		, m_endIndex(endIndex)
		, m_numRunners(numRunners)
		, m_numIndexesPerRunner(numIndexesPerRunner)
		, m_time(time)
	{
	}

private:
	// Creates an operation and starts it
	void run()
	{
		OperationType t;
		t.m_numRunners = m_numRunners;
		t.m_numIndexesPerRunner = m_numIndexesPerRunner;
		t.m_time = m_time;

		t.run(m_object, m_startIndex, m_endIndex);
	}

	ObjectType& m_object;
	const int m_startIndex;
	const int m_endIndex;
	const int m_numRunners;
	const int m_numIndexesPerRunner;
	const unsigned long m_time;
};

/**
 * \brief Creates and instance of a Runner and returns it
 *
 * OperationType is the concrete type of the operation to run
 * \param object the object to share among all Runners
 * \param curRunnerIndex the index of the runner to create. This is used to
 *                       calculate the start and end index for the runner
 * \param numRunners the number of runners that are being run
 * \param numIndexesPerRunner how many indexes each runner has
 * \param time how much time each runner can last at most
 * \return a new Runner instance for the given operation
 */
template <class OperationType>
Runner<OperationType>* createRunner(typename OperationType::ObjectType& object, int curRunnerIndex, int numRunners, int numIndexesPerRunner, unsigned long time)
{
	const int startIndex = curRunnerIndex * numIndexesPerRunner;
	const int endIndex = (curRunnerIndex + 1) * numIndexesPerRunner;
	return new Runner<OperationType>(object, startIndex, endIndex, numRunners, numIndexesPerRunner, time);
}

/**
 * \brief Executes parallel runners on the same object
 *
 * OperationType is the concrete type of the operation to run. The startIndex
 * and endIndex of each runner are computed so that the first runner takes
 * values from 0 to numIndexesPerRunner, the second runner values from
 * numIndexesPerRunner to (2 * numIndividuals), the i-th runner values from
 * ((i - 1) * numIndexesPerRunner) to (i * numIndexesPerRunner)
 * \param object the object to share among all Runners
 * \param numRunners the number of runners to create
 * \param numIndexesPerRunner how many index to give to runners
 * \param time how much to wait for each thread to finish in milliseconds
 * \return true if all threads finish in time, false otherwise
 */
template <class OperationType>
bool executeParallelRunners(typename OperationType::ObjectType& object, int numRunners, int numIndexesPerRunner, unsigned long time = ULONG_MAX)
{
	typedef Runner<OperationType> RunnerType;

	// RAII for Runners
	class RunnersVector : public QVector<RunnerType*>
	{
	public:
		~RunnersVector()
		{
			for (int i = 0; i < this->size(); ++i) {
				delete this->value(i);
			}
			this->clear();
		}
	} runners;

	// Creating all runners
	for (int i = 0; i < numRunners; ++i) {
		runners.append(createRunner<OperationType>(object, i, numRunners, numIndexesPerRunner, time));
	}

	// Starting all runners
	for (int i = 0; i < runners.size(); ++i) {
		runners[i]->start();
	}

	// Waiting for all runners to complete. allFinished will stay true if
	// all threads finish in time
	bool allFinished = true;
	for (int i = 0; i < runners.size(); ++i) {
		allFinished = allFinished && runners[i]->wait(time);
	}

	return allFinished;
}

/**
 * \brief A class to record a sequence of nested function calls
 *
 * This class can be used to record a sequence of nested function calls and then
 * compare it with an expected sequence. All function calls should create an
 * instance of the FunctionCall class at the beginning to be registered. The
 * sequence is recorded in the last FunctionCallsSequenceRecorder object that is
 * created (we have a static member pointing at it).
 *
 * \code
 * void f()
 * {
 * 	FunctionCall call("f");
 * }
 *
 * void g()
 * {
 * 	FunctionCall call("g");
 *
 * 	f();
 * }
 *
 * void test()
 * {
 * 	FunctionCallsSequenceRecorder expectedSequence;
 * 	expectedSequence.callBegin("g");
 * 		expectedSequence.callBegin("f");
 * 		expectedSequence.callEnd();
 * 	expectedSequence.callEnd();
 *
 * 	FunctionCallsSequenceRecorder actualCalls;
 * 	g();
 *
 * 	QCOMPARE(expectedSequence, actualCalls);
 * }
 * \endcode
 *
 * \note The mechanism is NOT thread-safe NOR reentrant
 */
class FunctionCallsSequenceRecorder
{
public:
	/**
	 * \brief Stores a single function call
	 *
	 * Stores a single function call with all the nested calls to other
	 * functions and the parent function
	 */
	struct SingleCall {
		/**
		 * \brief Comparison operator
		 *
		 * This ignores the pointer to the parent
		 * \param other the other object to compare
		 * \return true if the two instances are equal
		 */
		bool operator==(const SingleCall& other) const;

		/**
		 * \brief Comparison operator
		 *
		 * This ignores the pointer to the parent
		 * \param other the other object to compare
		 * \return true if the two instances are different
		 */
		bool operator!=(const SingleCall& other) const;

		/**
		 * \brief The name of the function call
		 */
		QString name;

		/**
		 * \brief The list of subcalls
		 */
		QList<SingleCall> subcalls;

		/**
		 * \brief True if the call ends correctly
		 */
		bool callEnded;

		/**
		 * \brief A pointer to the parent call
		 */
		SingleCall* parent;
	};

public:
	/**
	 * \brief Constructor
	 *
	 * This makes the instantiated object the one used by FunctionCall until
	 * a new instance is created
	 */
	FunctionCallsSequenceRecorder();

	/**
	 * \brief Destructor
	 *
	 * If the current instance is the one used by FunctionCall instances,
	 * resets the pointer to NULL (NOT to the previous instance!)
	 */
	~FunctionCallsSequenceRecorder();

	/**
	 * \brief Compares two sequences
	 *
	 * \param other the other sequence to compare
	 * \return true if the two sequences are equal
	 */
	bool operator==(const FunctionCallsSequenceRecorder& other) const;

	/**
	 * \brief Compares two sequences
	 *
	 * \param other the other sequence to compare
	 * \return true if the two sequences are different
	 */
	bool operator!=(const FunctionCallsSequenceRecorder& other) const;

	/**
	 * \brief Records the begin of a function call
	 *
	 * \param funcName the name of the function. This should be unique for
	 *                 the function call
	 */
	void callBegin(QString funcName);

	/**
	 * \brief Records the end of a function call
	 */
	void callEnd();

	/**
	 * \brief Returns the sequence of calls
	 *
	 * \return the sequence of calls
	 */
	const SingleCall& calls() const
	{
		return m_root;
	}

	/**
	 * \brief Returns a map with key being a function name and value the
	 *        number of times the function was called
	 *
	 * This can be useful to compare two sequences of function calls in
	 * which the order of calls is not important. The map is built every
	 * time this function is called, so it is advisable to store it
	 * \return a map of function calls with the number of times they have
	 *         been called
	 */
	QMap<QString, int> functionsMap() const;

	/**
	 * \brief Returns a string representation of the sequence of calls
	 *
	 * \return a string representation of the sequence of calls
	 */
	QString toString() const;

private:
	// Returns the string representation of the call. This is a recursive
	// function. tabLevel is used to indentate the nested calls, use 0 as
	// the value for the initial call to this function
	QString singleCallToString(const SingleCall& call, int tabLevel = 0) const;

	// Fills a map like the one returned by functionsMap() recursively
	// starting from the given call
	void recursivelyFillFunctionMap(QMap<QString, int>& map, const SingleCall& call) const;

	// This stores the most external call to functions. The name is the
	// empty string (we only use the list of subcalls)
	SingleCall m_root; // empty name

	SingleCall* m_curCall;

	// The instance of this object used by FunctionCall instances
	static FunctionCallsSequenceRecorder* m_curFunctionCallsSequenceRecorder;

	// FunctionCall is friend to access m_curFunctionCallsSequenceRecorder
	friend class FunctionCall;
};

/**
 * \brief Identifies a function call
 *
 * See the description of FunctionCallsSequenceRecorder for more information
 */
class FunctionCall
{
public:
	/**
	 * \brief Constructor
	 *
	 * Create instances at the beginning of functions to record. The
	 * parameters are the same as those of the function
	 * FunctionCallsSequenceRecorder::callBegin(). This actually call the
	 * callBegin() function of the current FunctionCallsSequenceRecorder
	 * instance (i.e. of the latest instance that was created)
	 * \param funcName the name of the function. This should be unique for
	 *                 the function call
	 */
	FunctionCall(QString funcName);

	/**
	 * \brief Destructor
	 *
	 * This calls the FunctionCallsSequenceRecorder::callEnd() function of
	 * the current FunctionCallsSequenceRecorder instance (i.e. of the
	 * latest instance that was created)
	 */
	~FunctionCall();
};

/**
 * \brief Creates a temporary file, fills it, creates a ConfigurationManager
 *        object and reads the file
 *
 * This function throws an exception in case of errors
 * \param content the content to use to fill the file
 * \return the ConfigurationManger object that has loaded the file
 */
salsa::ConfigurationManager fillTemporaryConfigurationFileAndLoadParameters(const char* content);

#endif
