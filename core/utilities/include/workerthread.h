/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "utilitiesconfig.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QList>
#include "baseexception.h"

namespace farsa {

/*!
 * \brief The exception stored when an unknown exception is thrown by a
 *        ThreadOperation
 *
 * This is the exception that is stored instead of the real exception when the
 * exception thrown by ThreadOperation is not a subclass of BaseException (and
 * so cannot be cloned). This only contains a fixed error message
 */
class FARSA_UTIL_TEMPLATE UnknownException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	UnknownException() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	UnknownException(const UnknownException& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	UnknownException& operator=(const UnknownException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~UnknownException() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "Unknown exception thrown by a ThreadOperation, cannot provide further information";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(UnknownException)
};

/*! interface for describing an operation to do for the WorkerThread
 *  \ingroup utilities_misc
 */
class FARSA_UTIL_TEMPLATE ThreadOperation {
public:
	virtual ~ThreadOperation() { };
	/*! stop the operation
	 *  \note this is called outside the WorkerThread, so the subclasses
	 *  has to implements this method thread-safetly
	 */
	virtual void stop() = 0;
	/*! run the operation
	 *  \note this is called inside the run loop of the WorkerThread,
	 *  subclasses can implementes here the blocking code that do the job
	 */
	virtual void run() = 0;
};

/*! the supporting thread in order to run operations
 *  on a different thread instead of the GUI thread
 */
class FARSA_UTIL_API WorkerThread : public QThread {
	Q_OBJECT

public:
	/*! constructor */
	WorkerThread( QObject* parent );
	/*! destructor */
	~WorkerThread();
	/*! Add an operation to the queue. If deleteAtEnd is true the operation
	    is deleted when it has finished executing, otherwise it is not */
	void addOperation( ThreadOperation* operation, bool deleteAtEnd = true );
	/*! Stop the current operation. If wait is true, it waits until the
	 *  operation actually stops
	 */
	void stopCurrentOperation(bool wait);
	/*! implement the main run routine */
	void run();
	/*! stop the current operation and quit from the thread
	 *  terminating its cycle.
	 *  \note this is the safe way to close the thread
	 *  \warning this is a blocking method
	 */
	void quit();
	/*! Returns true if an operation is running */
	bool operationRunning();

signals:
	/**
	 * \brief The signal emitted when a ThreadOperation throws as exception
	 *
	 * This signal is emitted when a ThreadOperation object throws an
	 * exception and terminates. The exception is cloned (or an instance of
	 * UnknownException is created) and stored here. We keep the list of all
	 * exceptions ever thrown to avoid problems (with the current
	 * implementation we don't know when it is safe to deleted the cloned
	 * object)
	 * \param e a clone of the exception thrown in the ThreadOperation
	 * \note The connection to this signal must be of type
	 *       Qt::QueuedConnection or Qt::BlockingQueuedConnection because
	 *       the signal is emitted from another thread. This should be fixed
	 *       (use a QObject with the correct thread affinity)
	 */
	void exceptionDuringOperation(farsa::BaseException *e);

private:
	/*! The structure with an operation and the flag for automatic delete */
	struct ThreadOperationInfo
	{
		ThreadOperationInfo() :
			operation(NULL),
			deleteAtEnd(false)
		{
		}

		ThreadOperationInfo(ThreadOperation* o, bool d) :
			operation(o),
			deleteAtEnd(d)
		{
		}

		ThreadOperation* operation;
		bool deleteAtEnd;
	};
	/*! The Queue of operations to do */
	QQueue<ThreadOperationInfo> operations;
	/*! the mutex used for controlling the access to the operations queue */
	QMutex mutex;
	/*! the wait condition for waiting new operations to do */
	QWaitCondition waitForOperationsToDo;
	/*! the wait condition for waiting that an operation has concluded */
	QWaitCondition waitForOperationsToFinish;
	/*! current operation running */
	ThreadOperationInfo operation;
	/*! when true it quit from the run loop */
	bool quitRequested;
	/*! The list of all exceptions thrown by thread operations. We keep it
	 *  here because we don't know when is safe to delete them (this should
	 *  be fixed). All objects are deleted in the destructor
	 */
	QList<BaseException *> exceptions;
};

} // end namespace farsa

#endif
