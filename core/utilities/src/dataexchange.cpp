/********************************************************************************
 *  SALSA Utilities Library                                                     *
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

#include "dataexchange.h"

namespace salsa {

	void GlobalUploaderDownloader::stopAllDataExchanges()
	{
		getInstance().internalStopAllDataExchanges();
	}

	GlobalUploaderDownloader::GlobalUploaderDownloader()
		: m_queueHolders()
		, m_mutex()
	{
		// Nothing to do here
	}

	GlobalUploaderDownloader& GlobalUploaderDownloader::getInstance()
	{
		// The meyer singleton
		static GlobalUploaderDownloader instance;

		return instance;
	}

	void GlobalUploaderDownloader::internalStopAllDataExchanges()
	{
		QMutexLocker locker(&m_mutex);

		// For each queue holder we have to set its dataExchangeStopped member to true and then call
		// wakeAll() on the wait condition
		foreach (__DataExchange_internal::QueueHolderBase* q, m_queueHolders) {
			q->dataExchangeStopped = true;
			q->waitCondition.wakeAll();
		}
	}

	void GlobalUploaderDownloader::addQueueHolder(__DataExchange_internal::QueueHolderBase* queueHolder)
	{
		QMutexLocker locker(&m_mutex);

		// Adding the queue holder to the set
		m_queueHolders.insert(queueHolder);
	}

	void GlobalUploaderDownloader::removeQueueHolder(__DataExchange_internal::QueueHolderBase* queueHolder)
	{
		// Here we don't acquire the mutex because this function is only called when an uploader or a
		// downloader is destroyed  and the functions called when one of those things happends already take
		// the lock (we could have made the mutex recursive, but it is not needed here)

		// Removing the queue holder from the set
		m_queueHolders.remove(queueHolder);
	}

} // end namespace salsa
