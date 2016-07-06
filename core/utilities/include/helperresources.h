/***************************************************************************
 *   Copyright (C) 2008 by Tomassino Ferrauto                              *
 *   t_ferrauto@yahoo.it                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef HELPERRESOURCES_H
#define HELPERRESOURCES_H

#include "resource.h"
#include "utilitiesconfig.h"

namespace farsa {

/**
 * \brief A vector that can be used as a resource
 *
 * The number of elements must be set in the constructor and cannot be changed.
 */
template<class T>
class FARSA_UTIL_TEMPLATE ResourceVector : public Resource
{
public:
	/**
	 * \brief Constructor
	 *
	 * Note that the elem
	 * \param size The number of elements in the vector
	 */
	ResourceVector(unsigned int size) :
		Resource(),
		m_size(size),
		m_vector(new T[m_size])
	{
	}

	/**
	 * \brief Destructor
	 */
	~ResourceVector()
	{
		delete[] m_vector;
	}

	/**
	 * \brief Returns the size of the vector
	 */
	unsigned int size() const
	{
		return m_size;
	}

	/**
	 * \brief Returns the i-th element (const version)
	 *
	 * \param i the index of the element to return
	 * \return the value of the element
	 */
	const T& operator[](unsigned int i) const
	{
		return m_vector[i];
	}

	/**
	 * \brief Returns the i-th element
	 *
	 * \param i the index of the element to return
	 * \return the value of the element
	 */
	T& operator[](unsigned int i)
	{
		return m_vector[i];
	}

protected:
	/**
	 * \brief The number of elements
	 */
	const unsigned int m_size;

	/**
	 * \brief The vector containing the elements
	 */
	T* const m_vector;
};

} // end namespace farsa

#endif
