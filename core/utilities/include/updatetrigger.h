/********************************************************************************
 *  SALSA                                                                       *
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

#ifndef UPDATETRIGGER_H
#define UPDATETRIGGER_H

#include "utilitiesconfig.h"

namespace salsa {

template <class CounterType>
class UpdateChecker;

/**
 * \brief The default policy for recent updates triggered notifications
 *
 * See the description of UpdateTrigger for more information
 */
class RecentUpdateTriggered
{
public:
	/**
	 * \brief Constructor
	 */
	RecentUpdateTriggered()
		: m_recentTrigger(false)
	{
	}

	/**
	 * \brief Returns true if there was a recent update
	 *
	 * This function returns true on the first call after updateTriggered()
	 * and false for subsequent calls until updateTriggered() is called
	 * again. This is not const because it resets a flag
	 * \return true if there was a recent trigger, false otherwise
	 */
	bool recentUpdateTriggered()
	{
		const bool ret = m_recentTrigger;

		m_recentTrigger = false;

		return ret;
	}

protected:
	/**
	 * \brief Triggers an update
	 *
	 * Call this whenever an update is triggered
	 */
	void updateTriggered()
	{
		m_recentTrigger = true;
	}

private:
	/**
	 * \brief The flag used to check if there was a recent trigger
	 */
	bool m_recentTrigger;
};

/**
 * \brief An utility class to signal whether an update is needed
 *
 * This and UpdateChecker can be used alone or with data exchange classes to
 * decide when to perform complex updates. In general it works like this (see
 * below for more information on how to use it with uploaders/downladers):
 * suppose class A performs from time to time some updates to data needed to
 * class B and you want to check if data in A has actually changed before
 * attempting to update B (e.g. to avoid wasting time on complex updates). The
 * class A would have an instance of UpdateTrigger and B an instance of
 * UpdateChecker, say upA and ckB respectively. Then A would call
 * upA.triggerUpdate() whenever data has changed to signal that an update is
 * needed. To check if an update is needed, B uses ckB, calling
 * ckB.updateNeeded(upA). If the return value is true, an update is needed,
 * otherwise it is not. Internally UpdateTrigger and UpdateChecker use counters.
 * The counter in UpdateTrigger is incremented by the triggerUpdate() function,
 * while the updateNeeded() function checks whether the internal counter of the
 * UpdateChecker is equal to the counter of the UpdateTrigger. If it is, returns
 * false (no update needed), otherwise it updates its internal counter so that
 * it is the same as the one of the other object and returns true (update
 * needed).
 * To use these classes with uploaders/downloaders you can proceed as follows.
 * Suppose class A is an uploader, class B the associated downloader and class D
 * the data exchanged. Then A and D should have a data member that is an
 * instance of UpdateTrigger, say upA and upD, while B should have an instance
 * of UpdateChecker, say ckB. Then if you want B to check if A has changed
 * something requiring a complex update, do the following in the function of A
 * that performs data upload write:
 *
 * \code
 * void A::doImportantUpdate()
 * {
 * 	// Do something that has to be explicitly signalled to the downloader
 * 	...
 *
 * 	upA.triggerUpdate();
 * }
 *
 * void A::sendData()
 * {
 * 	DatumToUpload<D> d(m_uploader);
 *
 * 	// Check if an important update is needed. This is not a compulsory
 * 	// step, but it can be useful if A needs to send additional data when
 * 	// the update is triggered. The recentUpdateTriggered() function returns
 * 	// true on the first call after triggerUpdate() and false for subsequent
 * 	// calls until triggerUpdate() is called again. See below for more
 * 	// information
 * 	if (upA.recentUpdateTriggered()) {
 * 		// Add more data to what we sent
 * 		d->...
 * 	}
 *
 * 	// Fill data and update the UpdateTrigger in D (we use the UpdateTrigger
 * 	// object in D to transfer the status of upA, so that B can do the
 * 	// check)
 * 	d->upD = upA;
 * 	d->...
 * 	d->...
 * }
 * \endcode
 *
 * In the function in B which checks for updates, you can do this:
 *
 * \code
 * void B::receiveData()
 * {
 * 	const D* d = m_downloader.downloadDatum();
 *
 * 	// Check if an update is needed
 * 	if (ckB.updateNeeded(d->upD)) {
 * 		// Do complex update
 * 		...
 * 	}
 *
 * 	d->...
 * 	d->...
 * }
 * \endcode
 *
 * The CounterType template parameter is the type of the counter used
 * internally. By default the type is unsigned long and that should be enough
 * for normal use (see the note below for possible problems). If you want to
 * provide your own counter, you have to provide a type that respects the
 * following requisites:
 * 	- it must provide a prefix ++ operator. Every time the operator is
 * 	  called the counter must change to a state that is different from all
 * 	  its previous ones (at least for a long enough number of updates, see
 * 	  the note below)
 * 	- it must provide an == operator
 * 	- it must provide a constructor taking no parameters that initializes
 * 	  the counter to a consistent state (the same for all counter objects)
 * 	- it must provide copy operator
 * The RecentUpdateTriggeredPolicy template parameter is the policy to use to be
 * notified when there is a recent update. The default value is
 * RecentUpdateTriggered, that works as described in the example, where the
 * recentUpdateTriggered() function is used. You can set it to void in case you
 * don't need the check and want to avoid any unnecessary overhead. If you want
 * to provide your custom implementation you must provide a type that respects
 * the following requisites:
 * 	- it must have a constructor taking no parameters that initializes the
 * 	  object so that calls to recentUpdateTriggered() return false
 * 	- it should provide a public bool recentUpdateTriggered() function for
 * 	  users to check if there was a recent update. You can use different
 * 	  mechanisms, if you want, but the example above would no longer be
 * 	  valid
 * 	- it must provide a (possibly protected) function void updateTriggered()
 * 	  function called by UpdateTrigger whenever an update is triggered
 * \note It is theoretically possible that UpdateChecker::updateNeeded() returns
 *       false even if there actually is the need of an update. This may happend
 *       if you use for example an unsigned int as the CounterType because the
 *       internal counter can overflow and become 0 again. It means, however
 *       that A LOT of updates have happened since the last check. If you fear
 *       this possibility, use a custom type as counter that cannot go back to
 *       the initial state or that has a longer cycle.
 * \warning You should use an UpdateTrigger object always with the same
 *          UpdateChecker object to check for triggers (see examples above).
 */
template <class CounterType = unsigned long, class RecentUpdateTriggeredPolicy = RecentUpdateTriggered>
class UpdateTrigger : public RecentUpdateTriggeredPolicy
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initialized the Counter to the default state
	 */
	UpdateTrigger()
		: RecentUpdateTriggeredPolicy()
		, m_counter()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	UpdateTrigger(const UpdateTrigger& other)
		: RecentUpdateTriggeredPolicy(other)
		, m_counter(other.m_counter)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 */
	UpdateTrigger& operator=(const UpdateTrigger& other)
	{
		if (this == &other) {
			return *this;
		}

		RecentUpdateTriggeredPolicy::operator=(other);

		m_counter = other.m_counter;

		return *this;
	}

	/**
	 * \brief Triggers an update
	 *
	 * Call this to signal that an update is needed
	 */
	void triggerUpdate()
	{
		// Incrementing the counter and setting the recentTrigger flag to true
		++m_counter;
		RecentUpdateTriggeredPolicy::updateTriggered();
	}

private:
	/**
	 * \brief The internal counter
	 */
	CounterType m_counter;

	/**
	 * \brief UpdateChecker is friend to access our counter
	 */
	template <class T>
	friend class UpdateChecker;
};

/**
 * \brief The specialization of the UpdateTrigger class without the recent
 *        update triggered policy
 */
template <class CounterType>
class UpdateTrigger<CounterType, void>
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initialized the Counter to the default state
	 */
	UpdateTrigger()
		: m_counter()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	UpdateTrigger(const UpdateTrigger& other)
		: m_counter(other.m_counter)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 */
	UpdateTrigger& operator=(const UpdateTrigger& other)
	{
		if (this == &other) {
			return *this;
		}

		m_counter = other.m_counter;

		return *this;
	}

	/**
	 * \brief Triggers an update
	 *
	 * Call this to signal that an update is needed
	 */
	void triggerUpdate()
	{
		// Incrementing the counter (no recent update trigger policy, here)
		++m_counter;
	}

private:
	/**
	 * \brief The internal counter
	 */
	CounterType m_counter;

	/**
	 * \brief UpdateChecker is friend to access our counter
	 */
	template <class T>
	friend class UpdateChecker;
};

/**
 * \brief An alternate name for the UpdateTrigger using unsigned long counter
 *        and the standard recent update trigger policy (to avoid using the <>
 *        notation)
 */
typedef UpdateTrigger<> UpdateTriggerLong;

/**
 * \brief An alternate name for the UpdateTrigger using unsigned long counter
 *        and no recent update trigger policy
 */
typedef UpdateTrigger<unsigned long, void> UpdateTriggerLongNoRecentUpdateCheck;

/**
 * \brief The class checking if an update is needed
 *
 * See the description of UpdateTrigger for more information
 */
template <class CounterType = unsigned long>
class UpdateChecker
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initialized the Counter to the default state
	 */
	UpdateChecker()
		: m_counter()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	UpdateChecker(const UpdateChecker& other)
		: m_counter(other.m_counter)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 */
	UpdateChecker& operator=(const UpdateChecker& other)
	{
		if (this == &other) {
			return *this;
		}

		m_counter = other.m_counter;

		return *this;
	}

	/**
	 * \brief Checks if an update was triggered in trigger
	 *
	 * Use this function to check if an update is needed, i.e. if the value
	 * of the internal counter of trigger has changed since the last time
	 * this function was called.
	 * \param trigger the UpdateTrigger object where updates are triggered
	 * \return true if an update is needed, false otherwise
	 * \warning You should use an UpdateTrigger object always with the same
	 *          UpdateChecker object (see the UpdateTrigger class
	 *          description)
	 */
	template <class RecentUpdateTriggeredPolicy>
	bool updateNeeded(const UpdateTrigger<CounterType, RecentUpdateTriggeredPolicy>& trigger)
	{
		if (trigger.m_counter == m_counter) {
			return false;
		}

		m_counter = trigger.m_counter;

		return true;
	}

private:
	/**
	 * \brief The internal counter
	 */
	CounterType m_counter;
};

/**
 * \brief An alternate name for the UpdateChecker using unsigned long counter
 *        (to avoid using the <> notation)
 */
typedef UpdateChecker<> UpdateCheckerLong;

}  // end namespace salsa

#endif
