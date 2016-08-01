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

#ifndef INTERVALS_H
#define INTERVALS_H

#include "utilitiesconfig.h"

#include <QLinkedList>
#include <QString>
#include <cmath>
#include <limits>
#include "mathutils.h"

namespace salsa {

/**
 * \brief A class modelling a range of real values
 *
 * This class simply stores a range of values, with a minimum and maximum value
 * Use the Intervals class and its subclasses, which can also handle multiple
 * intervals and operations with intervals. Moreover this class doesn't perform
 * any sanity check (e.g. you are allowed to have start > end)
 */
class SALSA_UTIL_API SimpleInterval
{
public:
	/**
	 * \brief Constructor
	 *
	 * Builds an empy interval (in which start == end)
	 */
	SimpleInterval() :
		start(),
		end(start)
	{
	}

	/**
	 * \brief Builds and interval from start to end
	 *
	 * \param s the start of the interval
	 * \param e the end of the interval
	 */
	SimpleInterval(real s, real e) :
		start(s),
		end(e)
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the interval to copy
	 */
	SimpleInterval(const SimpleInterval& other) :
		start(other.start),
		end(other.end)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the interval to copy
	 * \return a reference to this
	 */
	SimpleInterval& operator=(const SimpleInterval& other)
	{
		if (&other == this) {
			return *this;
		}

		start = other.start;
		end = other.end;

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	~SimpleInterval()
	{
	}

	/**
	 * \brief Compares two intervals
	 *
	 * This function only compares the start of the two intervals. Useful
	 * for sorting by starting value
	 * \param other the interval with which we are compared
	 * \return true if the start of this interval is less than the start of
	 *         the other interval
	 */
	bool operator<(const SimpleInterval& other) const
	{
		return start < other.start;
	}

	/**
	 * \brief Returns the length of the interval
	 *
	 * \return the length of the interval
	 */
	real length() const
	{
		return end - start;
	}

	/**
	 * \brief Returns true if other is equal to this interval
	 *
	 * \param other the interval whith which we are compared
	 * \return true if the two intervals are equal
	 */
	bool equals(const SimpleInterval& other) const
	{
		return (start == other.start) && (end == other.end);
	}

	/**
	 * \brief Returns the string representation of the interval
	 *
	 * \return the string representation of the interval
	 */
	operator QString() const
	{
		return QString("[%1, %2]").arg(start).arg(end);
	}

	/**
	 * \brief Returns the string representation of the interval
	 *
	 * \return the string representation of the interval
	 */
	QString toString() const
	{
		return *this;
	}

	/**
	 * \brief Converts the given string to a SimpleInterval
	 *
	 * The string should be in the form "[start, end]" where start and end
	 * are the starting and ending values of the interval
	 * \param str the string to convert
	 * \param ok if not NULL, it is set to false if conversion was
	 *           successful, to false otherwise
	 * \return the SimpleInterval represented by str. If conversion was not
	 *         successful, returns an empty interval
	 */
	static SimpleInterval fromString(QString str, bool* ok = NULL);

	/**
	 * \brief Converts a vector of simple intervals to string
	 *
	 * \param v the vector to convert
	 * \return a string representing the vector of simple intervals
	 */
	static QString vectorOfSimpleIntervalsToString(QVector<SimpleInterval> v);

	/**
	 * \brief Converts the given string to a vector of simple intervals
	 *
	 * The string should be a comma-separated list of simple intervals
	 * \param s the string to convert
	 * \param ok if not NULL, it is set to false if conversion was
	 *           successful, to false otherwise
	 * \return a vector of SimpleIntervals. If the string is not valid, an
	 *         empty vector is returned
	 */
	static QVector<SimpleInterval> vectorOfSimpleIntervalsFromString(QString s, bool* ok = NULL);

	/**
	 * \brief The starting value of the interval
	 */
	real start;

	/**
	 * \brief The ending value of the interval
	 */
	real end;
};

/**
 * \brief The class modelling intervals of floating point values
 *
 * This class models intervals of floating point values. It is made up of
 * multiple SimpleInterval instances. The composing intervals are always
 * ordered by ascending starting value, are never overlapping and never empty
 * (i.e. for no simple interval start is equal to end. This means that single
 * points cannot belong to an interval). Functions accepting a SimpleInterval
 * take intervals with start > end as modelling the intervals [-inf, end] +
 * [start, inf]. They are internally split in two, no interval is ever returned
 * with start > end. To access the simple intervals you have to use
 * const_iterators or get the const linked list of SimpleInterval
 */
class SALSA_UTIL_API Intervals
{
public:
	/**
	 * \brief The const iterator on simple intervals
	 */
	typedef QLinkedList<SimpleInterval>::const_iterator const_iterator;

public:
	/**
	 * \brief Constructor
	 *
	 * Builds an empty interval
	 */
	Intervals() :
		m_length(0.0),
		m_intervals()
	{
	}

	/**
	 * \brief Constructor
	 *
	 * Builds an interval made up of a single SimpleInterval
	 * \param interval the simple interval making this up
	 */
	Intervals(const SimpleInterval& interval);

	/**
	 * \brief Constructor
	 *
	 * Builds an interval from a list of Intervals or SimpleIntervals
	 * \param list the list of Intervals or SimpleIntervals making this up
	 */
	template <class List_t>
	Intervals(const List_t& list) :
		m_length(0.0),
		m_intervals()
	{
		for (typename List_t::const_iterator it = list.begin(); it != list.end(); it++) {
			unite(*it);
		}
	}

	/**
	 * \brief Copy Constructor
	 *
	 * \param other the intervals to copy
	 */
	Intervals(const Intervals& other) :
		m_length(other.m_length),
		m_intervals(other.m_intervals)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the intervals to copy
	 * \return a reference to this
	 */
	Intervals& operator=(const Intervals& other);

	/**
	 * \brief Destructor
	 */
	~Intervals()
	{
	}

	/**
	 * \brief Returns the string representation of the intervals
	 *
	 * \return the string representation of the intervals
	 */
	operator QString() const;

	/**
	 * \brief Returns the string representation of the interval
	 *
	 * \return the string representation of the interval
	 */
	QString toString() const
	{
		return *this;
	}

	/**
	 * \brief Returns the size of the intervals
	 *
	 * The size is the sum of the length of composing single intervals
	 * \return the size of the intervals
	 */
	real length() const
	{
		return m_length;
	}

	/**
	 * \brief Returns a const iterator to the beginning of the list of
	 *        simple intervals
	 *
	 * This iterator is the same as QLinkedList::const_iterator
	 * \return a const iterator to the beginning of the list of simple
	 *         intervals
	 */
	const_iterator begin() const
	{
		return m_intervals.begin();
	}

	/**
	 * \brief Returns a const iterator to the beginning of the list of
	 *        simple intervals
	 *
	 * This iterator is the same as QLinkedList::const_iterator
	 * \return a const iterator to the beginning of the list of simple
	 *         intervals
	 */
	const_iterator constBegin() const
	{
		return m_intervals.constBegin();
	}

	/**
	 * \brief Returns a const iterator to the end of the list of simple
	 *        intervals
	 *
	 * This iterator is the same as QLinkedList::const_iterator
	 * \return a const iterator to the end of the list of simple intervals
	 */
	const_iterator end() const
	{
		return m_intervals.end();
	}

	/**
	 * \brief Returns a const iterator to the end of the list of simple
	 *        intervals
	 *
	 * This iterator is the same as QLinkedList::const_iterator
	 * \return a const iterator to the end of the list of simple intervals
	 */
	const_iterator constEnd() const
	{
		return m_intervals.constEnd();
	}

	/**
	 * \brief Returns a const reference to the list of simple intervals
	 *
	 * \return a const reference to the list of simple intervals
	 */
	const QLinkedList<SimpleInterval>& getSimpleIntervalList() const
	{
		return m_intervals;
	}

	/**
	 * \brief Returns true if the list of intervals is empty
	 *
	 * \return true if the list of intervals is empty
	 */
	bool isEmpty() const
	{
		return m_intervals.isEmpty();
	}

	/**
	 * \brief Returns true if the list of intervals is empty
	 *
	 * \return true if the list of intervals is empty
	 */
	bool empty() const
	{
		return m_intervals.empty();
	}

	/**
	 * \brief Sets this to an empty interval
	 */
	void clear();

	/**
	 * \brief Intersects intervals in this with other
	 *
	 * \param other the intervals to intersect to these ones
	 * \return a reference to this
	 */
	Intervals& intersect(const Intervals& other)
	{
		intersect(other.constBegin(), other.constEnd());
		return *this;
	}

	/**
	 * \brief Intersects intervals in this with the given simple interval
	 *
	 * \param i the simple interval to intersect to these ones
	 * \return a reference to this
	 */
	Intervals& intersect(const SimpleInterval& i)
	{
		intersect(&i, &i + 1);
		return *this;
	}

	/**
	 * \brief Returns the intersection of this and other
	 *
	 * \param other the intervals to intersect to these ones
	 * \return a new object resulting from the intersection
	 */
	Intervals operator&(const Intervals& other) const
	{
		Intervals i(*this);
		return i.intersect(other);
	}

	/**
	 * \brief Returns the intersection of this and the given simple interval
	 *
	 * \param i the simple interval to intersect to these ones
	 * \return a new object resulting from the intersection
	 */
	Intervals operator&(const SimpleInterval& i) const
	{
		Intervals intrv(*this);
		return intrv.intersect(i);
	}

	/**
	 * \brief Intersects intervals in this with other
	 *
	 * \param other the intervals to intersect to these ones
	 * \return a reference to this
	 */
	Intervals& operator&=(const Intervals& other)
	{
		return intersect(other);
	}

	/**
	 * \brief Intersects a simple interval with intervals in this one
	 *
	 * \param value the interval to intersect to these ones
	 * \return a reference to this
	 */
	Intervals& operator&=(const SimpleInterval& value)
	{
		return intersect(value);
	}

	/**
	 * \brief Unites intervals in this with other
	 *
	 * \param other the intervals to unite to these ones
	 * \return a reference to this
	 */
	Intervals& unite(const Intervals& other)
	{
		unite(other.constBegin(), other.constEnd());
		return *this;
	}

	/**
	 * \brief Unites intervals in this with the given simple interval
	 *
	 * \param i the simple interval to unite to these ones
	 * \return a reference to this
	 */
	Intervals& unite(const SimpleInterval& i)
	{
		unite(&i, &i + 1);
		return *this;
	}

	/**
	 * \brief Returns the union of this and other
	 *
	 * \param other the intervals to unite to these ones
	 * \return a new object resulting from the union
	 */
	Intervals operator+(const Intervals& other) const
	{
		Intervals i(*this);
		return i.unite(other);
	}

	/**
	 * \brief Returns the union of this and the given simple interval
	 *
	 * \param i the simple interval to unite to these ones
	 * \return a new object resulting from the union
	 */
	Intervals operator+(const SimpleInterval& i) const
	{
		Intervals intrv(*this);
		return intrv.unite(i);
	}

	/**
	 * \brief Unites intervals in this with other
	 *
	 * \param other the intervals to unite to these ones
	 * \return a reference to this
	 */
	Intervals& operator+=(const Intervals& other)
	{
		return unite(other);
	}

	/**
	 * \brief Unites a simple interval with intervals in this one
	 *
	 * \param value the interval to unite to these ones
	 * \return a reference to this
	 */
	Intervals& operator+=(const SimpleInterval& value)
	{
		return unite(value);
	}

	/**
	 * \brief Returns the union of this and other
	 *
	 * \param other the intervals to unite to these ones
	 * \return a new object resulting from the union
	 */
	Intervals operator|(const Intervals& other) const
	{
		Intervals i(*this);
		return i.unite(other);
	}

	/**
	 * \brief Returns the union of this and the given simple interval
	 *
	 * \param i the simple interval to unite to these ones
	 * \return a new object resulting from the union
	 */
	Intervals operator|(const SimpleInterval& i) const
	{
		Intervals intrv(*this);
		return intrv.unite(i);
	}

	/**
	 * \brief Unites intervals in this with other
	 *
	 * \param other the intervals to unite to these ones
	 * \return a reference to this
	 */
	Intervals& operator|=(const Intervals& other)
	{
		return unite(other);
	}

	/**
	 * \brief Unites a simple interval with intervals in this one
	 *
	 * \param value the interval to unite to these ones
	 * \return a reference to this
	 */
	Intervals& operator|=(const SimpleInterval& value)
	{
		return unite(value);
	}

	/**
	 * \brief Unites a simple interval with intervals in this one
	 *
	 * \param value the interval to unite to these ones
	 * \return a reference to this
	 */
	Intervals& operator<<(const SimpleInterval& value)
	{
		return unite(value);
	}

	/**
	 * \brief Subtracts intervals from other to these ones
	 *
	 * \param other the intervals to subtract from these ones
	 * \return a reference to this
	 */
	Intervals& subtract(const Intervals& other)
	{
		subtract(other.constBegin(), other.constEnd());
		return *this;
	}

	/**
	 * \brief Subtracts intervals in this with the given simple interval
	 *
	 * \param i the simple interval to subtract from these ones
	 * \return a reference to this
	 */
	Intervals& subtract(const SimpleInterval& i)
	{
		subtract(&i, &i + 1);
		return *this;
	}

	/**
	 * \brief Returns the subtraction of this and other
	 *
	 * \param other the intervals to subtract from these ones
	 * \return a new object resulting from the subtraction
	 */
	Intervals operator-(const Intervals& other) const
	{
		Intervals i(*this);
		return i.subtract(other);
	}

	/**
	 * \brief Returns the subtraction of this and the given simple interval
	 *
	 * \param i the simple interval to subtract from these ones
	 * \return a new object resulting from the subtraction
	 */
	Intervals operator-(const SimpleInterval& i) const
	{
		Intervals intrv(*this);
		return intrv.subtract(i);
	}

	/**
	 * \brief Subtracts intervals in this with other
	 *
	 * \param other the intervals to subtract from these ones
	 * \return a reference to this
	 */
	Intervals& operator-=(const Intervals& other)
	{
		return subtract(other);
	}

	/**
	 * \brief Subtracts a simple interval with intervals in this one
	 *
	 * \param value the interval to subtract from these ones
	 * \return a reference to this
	 */
	Intervals& operator-=(const SimpleInterval& value)
	{
		return subtract(value);
	}

	/**
	 * \brief Returns true if this and other are different
	 *
	 * Two Intervals are different if at least one SimpleInterval differs
	 * \param other the intervals to compare with this one
	 * \return true is this and other are different
	 */
	bool operator!=(const Intervals& other) const
	{
		return !(*this == other);
	}

	/**
	 * \brief Returns true if this and other are the same
	 *
	 * Two Intervals are equal if all SimpleIntervals are the same
	 * \param other the intervals to compare with this one
	 * \return true is this and other are equal
	 */
	bool operator==(const Intervals& other) const;

	/**
	 * \brief Returns true if the given value belongs to these intervals
	 *
	 * \param v the value to check
	 * \return true if the value belongs to these intervals
	 */
	bool valueIn(real v) const;

protected:
	/**
	 * \brief Recomputes the length of the interval
	 */
	void recomputeLength();

	/**
	 * \brief Performs the intersection of the intervals in this object and
	 *        the intervals in the list delimited by the start and end
	 *        iterators
	 *
	 * \param otherBegin the start iterator to the the other intervals
	 * \param otherEnd the end iterator to the other intervals
	 */
	template <class OtherIterator_t>
	void intersect(OtherIterator_t otherBegin, OtherIterator_t otherEnd);

	/**
	 * \brief Performs the union of the intervals in this object and the
	 *        intervals in the list delimited by the start and end iterators
	 *
	 * \param otherBegin the start iterator to the the other intervals
	 * \param otherEnd the end iterator to the other intervals
	 */
	template <class OtherIterator_t>
	void unite(OtherIterator_t otherBegin, OtherIterator_t otherEnd);

	/**
	 * \brief Performs the subtraction of the intervals in this object and
	 *        the intervals in the list delimited by the start and end
	 *        iterators
	 *
	 * \param otherBegin the start iterator to the the other intervals
	 * \param otherEnd the end iterator to the other intervals
	 */
	template <class OtherIterator_t>
	void subtract(OtherIterator_t otherBegin, OtherIterator_t otherEnd);

	/**
	 * \brief The total length of intervals
	 */
	real m_length;

	/**
	 * \brief The list of simple intervals
	 *
	 * This list is always sorted by ascending start of the simple
	 * intervals. Moreover no two intervals ever intersect
	 */
	QLinkedList<SimpleInterval> m_intervals;
};

} // end namespace salsa

#endif
