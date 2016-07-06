/********************************************************************************
 *  FARSA Utilities Library                                                     *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "intervals.h"
#include <QRegExp>

namespace farsa {

SimpleInterval SimpleInterval::fromString(QString str, bool* ok)
{
	SimpleInterval ret;

	// If not NULL ok is set to false (will be set to true at the end of the function
	// if everything goes well)
	if (ok != NULL) {
		*ok = false;
	}

	// The regular expression the string should match
	QRegExp re("\\s*\\[(.+),\\s*(.+)\\]\\s*");
	if (!re.exactMatch(str)) {
		return SimpleInterval();
	}

	// The string matched, now we have to convert the strings for start and end
	bool numConv;
	ret.start = re.cap(1).toDouble(&numConv);
	if (!numConv) {
		return SimpleInterval();
	}
	ret.end = re.cap(2).toDouble(&numConv);
	if (!numConv) {
		return SimpleInterval();
	}

	// The string is a valid interval, setting ok to true if we have to
	if (ok != NULL) {
		*ok = true;
	}
	return ret;
}

QString SimpleInterval::vectorOfSimpleIntervalsToString(QVector<SimpleInterval> v)
{
	QString str;

	for (int i = 0; i < v.size(); i++) {
		if (i != 0) {
			str += ", ";
		}
		str += v[i];
	}

	return str;
}

QVector<SimpleInterval> SimpleInterval::vectorOfSimpleIntervalsFromString(QString s, bool* ok)
{
	QVector<SimpleInterval> intervals;

	const QString str = s.simplified();
	int curPos = 0;
	bool conversionOk = true;

	// We look for ] and exit when there is an error or when the position of the ] is the end
	// of the string (the string is simplified(), so there should be no character after the last ])
	do {
		const int lastPos = curPos + 1;
		curPos = str.indexOf(']', lastPos);

		// No ] found but not at the end of the string, there is an error
		if (curPos == -1) {
			conversionOk = false;
			break;
		}

		// Looking for [
		int startInterval = str.lastIndexOf('[', curPos);
		if (startInterval == -1) {
			// Cannot find the opening parenthesis, exiting
			conversionOk = false;
			break;
		}

		// Checking that the only characters between the previous interval and the current one are
		// one comma and spaces. Of course this check is skipped if this is the first interval
		if ((lastPos != 1) && (str.mid(lastPos, (startInterval - lastPos)).simplified() != ",")) {
			// Invalid format
			conversionOk = false;
			break;
		}

		intervals.append(SimpleInterval::fromString(str.mid(startInterval, (curPos - startInterval + 1)), &conversionOk));
	} while ((curPos != (str.size() - 1)) && conversionOk);

	if (ok != NULL) {
		*ok = conversionOk;
	}

	return (conversionOk ? intervals : QVector<SimpleInterval>());
}

Intervals::Intervals(const SimpleInterval& interval) :
	m_length(0.0),
	m_intervals()
{
	// We have to check if start == end or start > end
	if (interval.start == interval.end) {
		// Nothing to do, this is an empty interval
		return;
	} else if (interval.start > interval.end) {
		SimpleInterval i1(-std::numeric_limits<real>::infinity(), interval.end);
		SimpleInterval i2(interval.start, std::numeric_limits<real>::infinity());

		m_intervals << i1 << i2;
	} else {
		m_intervals << interval;
	}

	recomputeLength();
}

Intervals& Intervals::operator=(const Intervals& other)
{
	if (&other == this) {
		return *this;
	}

	m_length = other.m_length;
	m_intervals = other.m_intervals;

	return *this;
}

Intervals::operator QString() const
{
	if (isEmpty()) {
		return QString("[]");
	}

	QString s;

	for (const_iterator it = begin(); it != end(); it++) {
		if (it != begin()) {
			s += ", ";
		}
		s += *it;
	}

	return s;
}

void Intervals::clear()
{
	m_length = 0.0;
	m_intervals.clear();
}

bool Intervals::operator==(const Intervals& other) const
{
	const_iterator it1 = begin();
	const_iterator it2 = other.begin();
	while (true) {
		if ((it1 == end()) && (it2 == other.end())) {
			// Both ended, if we get here the intervals are equal
			break;
		} else if ((it1 == end()) || (it2 == other.end())) {
			// Different length, so intervals are different
			return false;
		} else if (!(it1->equals(*it2))) {
			// Different intervals
			return false;
		}

		++it1;
		++it2;
	}

	return true;
}

bool Intervals::valueIn(real v) const
{
	for (const_iterator it = begin(); it != end(); ++it) {
		if ((it->start <= v) && (it->end >= v)) {
			return true;
		}
	}

	return false;
}

void Intervals::recomputeLength()
{
	m_length = 0.0;
	for (const_iterator it = begin(); it != end(); ++it) {
		m_length += it->length();
	}
}

template <class OtherIterator_t>
void Intervals::intersect(OtherIterator_t otherBegin, OtherIterator_t otherEnd)
{
	// We modify m_intervals in place, so it1 needs to be a non-const interator
	QLinkedList<SimpleInterval>::iterator it1 = m_intervals.begin();
	OtherIterator_t it2 = otherBegin;
	while (it1 != m_intervals.end()) {
		// Possible relationships between the segments represented by it1 and it2:
		// a)
		// it1     -----
		// it2 ---
		// No intersection, move it2 forward
		//
		// b)
		// it1 -----
		// it2       ---
		// No intersection, remove current it1 and move to the next one
		//
		// c)
		// it1 ----
		// it2 ----
		// Intersection, move both it1 and it2 forward (this condition could be handled in
		// one of the following ones, but we explicitate it to avoid creating zero-length
		// intervals)
		//
		// d)
		// it1   -----
		// it2 ---
		// Intersection, split it1 in two, set it1 to the second part of the original it1
		// and move it2 forward
		//
		// e)
		// it1 -----
		// it2     ---
		// Intersection, restrict it1 to the intersecting part and move it1 forward
		//
		// f)
		// it1   -----
		// it2 ----------
		// Intersection, keep the current it1 as is and move it1 forward
		//
		// g)
		// it1 --------
		// it2   ----
		// Intersection, split it1 in three parts: remove the leftmost, keep the central one
		// and set it1 to the rightmost; then increment it2
		//
		// The conditions above are written near the corresponding if branch
		if (it2 == otherEnd) {
			// We have to remove all intervals from it1 to the end
			it1 = m_intervals.erase(it1, m_intervals.end());
		} else if (it1->start >= it2->end) { // Condition a
			++it2;
		} else if (it2->start >= it1->end) { // Condition b
			it1 = m_intervals.erase(it1);
		} else { // All other conditions
			// There is an intersection, checking what kind
			if ((it1->start == it2->start) && (it1->end == it2->end)) { // Condition c
				++it1;
				++it2;
			} else if ((it1->start >= it2->start) && (it1->start <= it2->end) && (it1->end >= it2->end)) { // Condition d
				const real origEnd = it1->end;
				it1->end = it2->end;
				++it1;
				it1 = m_intervals.insert(it1, SimpleInterval(it2->end, origEnd));
			} else if ((it1->start <= it2->start) && (it1->end >= it1->start) && (it1->end <= it2->end)) { // Condition e
				it1->start = it2->start;
				++it1;
			} else if ((it1->start >= it2->start) && (it1->end <= it2->end)) { // Condition f
				++it1;
			} else if ((it1->start <= it2->start) && (it2->end >= it2->end)) { // Condition g
				const real origEnd = it1->end;
				it1->start = it2->start;
				it1->end = it2->end;
				++it1;
				it1 = m_intervals.insert(it1, SimpleInterval(it2->end, origEnd));
			} else { // We should never get here
				qFatal("Unexpected condition in %s at line %d (segments intersection). it1->start = %f, it2->start = %f, it1->end = %f, it2->end = %f", __FILE__, __LINE__, it1->start, it2->start, it1->end, it2->end);
			}
		}
	}

	// Recomputing length
	recomputeLength();
}

template <class OtherIterator_t>
void Intervals::unite(OtherIterator_t otherBegin, OtherIterator_t otherEnd)
{
	// We modify m_intervals in place, so it1 needs to be a non-const interator
	QLinkedList<SimpleInterval>::iterator it1 = m_intervals.begin();
	OtherIterator_t it2 = otherBegin;
	while (it2 != otherEnd) {
		// Possible relationships between the segments represented by it1 and it2:
		// a)
		// it1     -----
		// it2 ---
		// No overlapping, add it2 to the list and move it2 forward
		//
		// b)
		// it1 -----
		// it2       ---
		// No overlapping, simply move it1 forward (because the current it2 could be
		// overlapping with the next it1; if it is not, we will fall in condition a and
		// add the interval)
		//
		// c)
		// In all other cases there is an overlap: the resulting segment has
		// start = min(it1->start, it2->start), while for the end:
		// 	- if it1->end == it2->end, the end is it1->end; then move both it1 and it2
		// 	  forward
		// 	- if it1->end > it2->end, the end is it1->end; then move it2 forward until
		// 	  you get it2->end > it1->end
		// 	- if it1->end < it2->end, move it1 forward until you get it1->start > it2->end;
		// 	  remove all it1 that do not satisfy this condition and set the end to the max
		// 	  between it2->end and the end of the last removed it1. Then move it1 and it2
		// 	  forward
		//
		// The conditions above are written near the corresponding if branch
		if (it1 == m_intervals.end()) {
			// We have to add all intervals still in it2 to the list
			for (; it2 != otherEnd; ++it2) {
				m_intervals.push_back(*it2);
			}
		} else if (it1->start > it2->end) { // Condition a
			m_intervals.insert(it1, *it2); // it1 is not incremented
			++it2;
		} else if (it2->start >= it1->end) { // Condition b
			++it1;
		} else { // Condition c
			it1->start = min(it1->start, it2->start);
			if (it1->end == it2->end) {
				++it1;
				++it2;
			} else if (it1->end > it2->end) {
				// it1->end is ok as it is, moving it2 forward
				for (; (it2 != otherEnd) && (it2->end <= it1->end); ++it2);
			} else { // it1->end < it2->end
				real prevIt1End = it1->end;
				QLinkedList<SimpleInterval>::iterator origIt1 = it1;
				++it1;
				while ((it1 != m_intervals.end()) && (it1->start <= it2->end)) {
					prevIt1End = it1->end;

					it1 = m_intervals.erase(it1);
				}
				origIt1->end = max(prevIt1End, it2->end);
				++it2;
			}
		}
	}

	// Recomputing length
	recomputeLength();
}

template <class OtherIterator_t>
void Intervals::subtract(OtherIterator_t otherBegin, OtherIterator_t otherEnd)
{
	// We modify m_intervals in place, so it1 needs to be a non-const interator
	QLinkedList<SimpleInterval>::iterator it1 = m_intervals.begin();
	OtherIterator_t it2 = otherBegin;
	while ((it1 != m_intervals.end()) && (it2 != otherEnd)){
		// Possible relationships between the segments represented by it1 and it2:
		// a)
		// it1     -----
		// it2 ---
		// No intersection, move it2 forward
		//
		// b)
		// it1 -----
		// it2       ---
		// No intersection, move it1 forward
		//
		// c)
		// it1   -----
		// it2 ----------
		// Intersection, remove it1 and move to the next one (this case also
		// comprises it1 equal to it2)
		//
		// d)
		// it1   -----
		// it2 ---
		// Intersection, set it1->start to it2->end and move it2 forward
		//
		// e)
		// it1 -----
		// it2     ---
		// Intersection, set it1->end to it2->start and move it1 forward
		//
		// f)
		// it1 --------
		// it2   ----
		// Intersection, split it1 in three parts and remove the central one;
		// then move it1 to the rightmost part and it2 forward
		//
		// The conditions above are written near the corresponding if branch
		if (it1->start >= it2->end) { // Condition a
			++it2;
		} else if (it2->start >= it1->end) { // Condition b
			++it1;
		} else { // All other conditions
			// There is an intersection, checking what kind
			if ((it1->start >= it2->start) && (it1->end <= it2->end)) { // Condition c
				it1 = m_intervals.erase(it1);
			} else if ((it1->start >= it2->start) && (it1->start < it2->end) && (it1->end > it2->end)) { // Condition d
				it1->start = it2->end;
				++it2;
			} else if ((it1->start < it2->start) && (it1->end > it1->start) && (it1->end <= it2->end)) { // Condition e
				it1->end = it2->start;
				++it1;
			} else if ((it1->start < it2->start) && (it1->end > it2->end)) { // Condition f
				const real origEnd = it1->end;
				it1->end = it2->start;
				++it1;
				it1 = m_intervals.insert(it1, SimpleInterval(it2->end, origEnd));
				++it2;
			} else { // We should never get here (if we get here, try to check whether the equality is checked correctly)
				qFatal("Unexpected condition in %s at line %d (segments subtraction). it1->start = %f, it2->start = %f, it1->end = %f, it2->end = %f", __FILE__, __LINE__, it1->start, it2->start, it1->end, it2->end);
			}
		}
	}

	// Recomputing length
	recomputeLength();
}

// Explicit template instantiation of the functions above
template void Intervals::intersect<QLinkedList<farsa::SimpleInterval>::const_iterator>(QLinkedList<farsa::SimpleInterval>::const_iterator otherBegin, QLinkedList<farsa::SimpleInterval>::const_iterator otherEnd);
template void Intervals::intersect<const farsa::SimpleInterval*>(const farsa::SimpleInterval* otherBegin, const farsa::SimpleInterval* otherEnd);
template void Intervals::unite<QLinkedList<farsa::SimpleInterval>::const_iterator>(QLinkedList<farsa::SimpleInterval>::const_iterator otherBegin, QLinkedList<farsa::SimpleInterval>::const_iterator otherEnd);
template void Intervals::unite<const farsa::SimpleInterval*>(const farsa::SimpleInterval* otherBegin, const farsa::SimpleInterval* otherEnd);
template void Intervals::subtract<QLinkedList<farsa::SimpleInterval>::const_iterator>(QLinkedList<farsa::SimpleInterval>::const_iterator otherBegin, QLinkedList<farsa::SimpleInterval>::const_iterator otherEnd);
template void Intervals::subtract<const farsa::SimpleInterval*>(const farsa::SimpleInterval* otherBegin, const farsa::SimpleInterval* otherEnd);

} // end namespace farsa
