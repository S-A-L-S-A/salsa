/********************************************************************************
 *  SALSA Utilities Library                                                     *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef DEPENDENCYSORTER_H
#define DEPENDENCYSORTER_H

#include "utilitiesconfig.h"
#include "utilitiesexceptions.h"
#include <QMap>
#include <QSet>
#include <QList>
#include <QVector>

namespace salsa {

/**
 * \brief A class to return data sorted by dependency
 *
 * This class is filled with a list of elements and their dependencies and then
 * returns a list of sorted elements. It is guaranteed that an element in the
 * sorted list comes after all the elements on which it depends (and that are
 * present). The elements can be of any type provided that they can be put into
 * a QMap and QSet (which are used internally). To insert elements use one of
 * the add() functions. If an element is already present, only the list of its
 * dependencies is modified (so that new dependencies are added). To get the
 * sorted list call sort() or sortWithDependencies(). The difference between the
 * two functions is simply that the latter returns a list where each element is
 * the sorted element plus the list of its dependencies, whereas the former
 * simply returns a list of sorted elements
 *
 * \note This class is not very efficient, the implementation is intentionally
 *       simple
 */
template <class ElementType_t>
class DependencySorter
{
public:
	/**
	 * \brief The type of elements used in the class
	 */
	typedef ElementType_t ElementType;

	/**
	 * \brief A structure containing an element and the list of its
	 *        dependencies
	 *
	 * A list of structured of this type is returned by the function
	 * sortWithDependencies()
	 */
	struct ElementAndDepencies
	{
		/**
		 * \brief Constructor
		 *
		 * \param e the element
		 * \param d the list of dependencies
		 */
		ElementAndDepencies(ElementType e, QList<ElementType> d) :
			element(e),
			dependencies(d)
		{
		}

		/**
		 * \brief The element
		 */
		ElementType element;

		/**
		 * \brief The list of dependencies
		 */
		QList<ElementType> dependencies;
	};

	/**
	 * \brief The type for the list of elements and dependencies
	 */
	typedef QList<ElementAndDepencies> ElementAndDepenciesList;
public:
	/**
	 * \brief Constructor
	 */
	DependencySorter() :
		m_elements()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	DependencySorter(const DependencySorter<ElementType>& other) :
		m_elements(other.m_elements)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 */
	DependencySorter& operator=(const DependencySorter<ElementType>& other)
	{
		if (&other == this) {
			return *this;
		}

		m_elements = other.m_elements;

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	~DependencySorter()
	{
	}

	/**
	 * \brief Returns the list of elements and their dependencies
	 *
	 * \return a map having the elements as keys and the set of dependencies
	 *         of each element as value
	 */
	const QMap<ElementType, QSet<ElementType> > elements() const
	{
		return m_elements;
	}

	/**
	 * \brief Adds an element with a single dependency
	 *
	 * \param e the element to add
	 * \param d the element on which e depends
	 */
	void add(ElementType e, ElementType d)
	{
		m_elements[e].insert(d);
	}

	/**
	 * \brief Adds an element with a list of dependencies
	 *
	 * \param e the element to add
	 * \param d the list of elements on which e depends
	 */
	void add(ElementType e, QList<ElementType> d)
	{
		m_elements[e].unite(QSet<ElementType>::fromList(d));
	}

	/**
	 * \brief Adds an element with a list of dependencies
	 *
	 * \param e the element to add
	 * \param d the list of elements on which e depends
	 */
	void add(ElementType e, QSet<ElementType> d)
	{
		m_elements[e].unite(d);
	}

	/**
	 * \brief Returns the sorted list of elements
	 *
	 * \return the sorted list of elements
	 */
	QList<ElementType> sort() const
	{
		// If there are no elements in the map, returning an empty list
		if (m_elements.empty()) {
			return QList<ElementType>();
		}

		// We copy the map of elements because we remove from the map elements that
		// have been analyzed
		QMap<ElementType, QSet<ElementType> > e = m_elements;

		// The sorted list
		QList<ElementType> sortedList;
		// The set with elements we are analyzing (this is needed to check whether cycles exist)
		QSet<ElementType> elementsBeingAnalyzed;
		// The stack of elements being analyzed (this is used to prevent restarting from scratch every cycle)
		QList<ElementType> stackElementsBeingAnalyzed;

		// Now we keep cycling until the map with elements contains something
		ElementType nextToTake = e.keys()[0];
		elementsBeingAnalyzed.insert(nextToTake);
		while (!e.empty()) {
			// Checking whether the element to register has any dependency which is present
			// in the list of elements
			bool dependencyInList = false;
			foreach(ElementType d, e[nextToTake]) {
				if (e.contains(d)) {
					// We have to add the dependency first
					stackElementsBeingAnalyzed.push_back(nextToTake);
					nextToTake = d;
					dependencyInList = true;

					// Checking if there is a cycle
					if (elementsBeingAnalyzed.contains(nextToTake)) {
						throw CircularDependencyException();
					} else {
						elementsBeingAnalyzed.insert(nextToTake);
					}
				}
			}

			if (!dependencyInList) {
				// We can add the element to the sorted list and remove it from the map
				sortedList.append(nextToTake);
				e.remove(nextToTake);
				elementsBeingAnalyzed.remove(nextToTake);
				if (!stackElementsBeingAnalyzed.empty()) {
					ElementType tmp = nextToTake;
					nextToTake = stackElementsBeingAnalyzed.takeLast();

					// Also removing the element just added to the list from the list of dependencies
					// (to avoid checking it in the next cycles)
					e[nextToTake].remove(tmp);
				} else if (!e.empty()) {
					nextToTake = e.keys()[0];
				}
			}
		}

		return sortedList;
	}

	/**
	 * \brief Returns the sorted list of elements with dependencies
	 *
	 * \return the sorted list of elements with dependencies
	 */
	ElementAndDepenciesList sortWithDependencies() const
	{
		// We first use sort() and then fill the list to return
		QList<ElementType> s = sort();

		ElementAndDepenciesList r;
		for (int i = 0; i < s.size(); i++) {
			r.append(ElementAndDepencies(s[i], m_elements[s[i]].toList()));
		}

		return r;
	}

private:
	/**
	 * \brief The map containing the elements and the list of their
	 *        dependencies
	 */
	QMap<ElementType, QSet<ElementType> > m_elements;
};

} // end namespace salsa

#endif
