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

#include "workerthread.h"
#include "utilitiesexceptions.h"

namespace farsa {

WorkerThread::WorkerThread( QObject* parent ) :
	QThread(parent),
	operations(),
	mutex(),
	waitForOperationsToDo(),
	waitForOperationsToFinish(),
	operation(),
	quitRequested(false),
	exceptions() {
}

WorkerThread::~WorkerThread() {
	foreach (BaseException *e, exceptions) {
		delete e;
	}
}

void WorkerThread::addOperation( ThreadOperation* newOperation, bool deleteAtEnd ) {
	mutex.lock();
	operations.enqueue( ThreadOperationInfo( newOperation, deleteAtEnd ) );
	mutex.unlock();
	if ( operations.size() == 1 ) {
		// wake the run thread because now there is something to do
		waitForOperationsToDo.wakeAll();
	}
}

void WorkerThread::stopCurrentOperation(bool wait) {
	mutex.lock();
	if ( operation.operation ) {
		operation.operation->stop();
	}
	if (wait) {
		waitForOperationsToFinish.wait(&mutex);
	}
	mutex.unlock();
}

void WorkerThread::run() {
	forever {
		mutex.lock();
		// We have to check quitRequested here (not only after waitForOperationsToDo.wait()) because quit() can
		// be called before run() starts. In that case quitRequested is already true, but we lost the "signal"
		// that should wake us when on the wait condition and so we would sleep indefinitely
		if ( quitRequested ) {
			mutex.unlock();
			return;
		}
		if ( operations.size() == 0 ) {
			// wait the adding of an operation
			waitForOperationsToDo.wait( &mutex );
		}
		if ( quitRequested ) {
			mutex.unlock();
			return;
		}
		operation = operations.dequeue();
		mutex.unlock();
		try {
			operation.operation->run();
		} catch (BaseException& e) {
			BaseException* cloned = e.clone();
			exceptions.append(cloned);

			emit exceptionDuringOperation(cloned);
		} catch (std::exception& e) {
			StandardLibraryException* cloned = new StandardLibraryException(e);
			exceptions.append(cloned);

			emit exceptionDuringOperation(cloned);
		} catch (...) {
			UnknownException* e = new UnknownException();
			exceptions.append(e);

			emit exceptionDuringOperation(e);
		}
		mutex.lock();
		if ( operation.deleteAtEnd ) {
			delete operation.operation;
		}
		operation.operation = NULL;
		mutex.unlock();
		waitForOperationsToFinish.wakeAll();
		if ( quitRequested ) {
			return;
		}
	}
}

void WorkerThread::quit() {
	mutex.lock();
	quitRequested = true;
	mutex.unlock();
	stopCurrentOperation(false); // No need to wait on operation to finish, we wait for the entire thread to complete
	waitForOperationsToDo.wakeAll();
	wait();
}

bool WorkerThread::operationRunning()
{
	mutex.lock();
	bool ret = (operation.operation != NULL);
	mutex.unlock();

	return ret;
}

} // end namespace farsa
