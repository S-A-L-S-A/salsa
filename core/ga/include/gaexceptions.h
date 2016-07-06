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

#ifndef GAEXCEPTIONS_H
#define GAEXCEPTIONS_H

#include <exception>
#include <cstring>
#include <cstdio>
#include "baseexception.h"
#include "gaconfig.h"

// All the suff below is to avoid warnings on Windows about the use of the
// unsafe function sprintf and strcpy...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace farsa {
// NOTE: I don't use snprintf instead of sprintf because it seems not to be in
// the current C++ standard (C++03, it is instead in the C99 standard). Note
// however that no buffer overflow is possible (buffer lengths are carefully
// checked)

/**
 * \brief The exception thrown if evolution could not be resumed
 */
class FARSA_GA_TEMPLATE CannotResumeEvolutionException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param reason the description of why the exception was thrown. The
	 *               buffer for this is at most 256 characters (including
	 *               the '\0' terminator)
	 */
	CannotResumeEvolutionException(const char* reason) throw() :
		BaseException()
	{
		strncpy(m_reason, reason, 256);
		m_reason[255] = '\0';
		sprintf(m_errorMessage, "Cannot resume interrupted evolution, reason: %s", m_reason);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CannotResumeEvolutionException(const CannotResumeEvolutionException& other) throw() :
		BaseException(other)
	{
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CannotResumeEvolutionException& operator=(const CannotResumeEvolutionException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CannotResumeEvolutionException() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_errorMessage;
	}

	/**
	 * \brief Returns the description of why the exception was thrown
	 *
	 * \return the description of why the exception was thrown
	 */
	const char *reason() const throw()
	{
		return m_reason;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CannotResumeEvolutionException)

private:
	/**
	 * \brief The description of why evolution could not be resumed
	 */
	char m_reason[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[512];
};

} // end namespace farsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif
