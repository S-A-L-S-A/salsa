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

#ifndef UTILITIESEXCEPTIONS_H
#define UTILITIESEXCEPTIONS_H

#include <exception>
#include <cstring>
#include <cstdio>
#include "baseexception.h"
#include "utilitiesconfig.h"

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
 * \brief A generic exception the user can throw at runtime
 *
 * You can use this when you need to throw an exception at runtime. This simply
 * has a string description of the error you can set when creating the instance.
 * You can use the utility function throwUserRuntimeError() to throw the
 * exception. The function takes a QString parameter, so it is easy to format
 * the error message.
 */
class FARSA_UTIL_TEMPLATE RuntimeUserException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param reason the description of why the exception was thrown. The
	 *               buffer for this is at most 256 characters (including
	 *               the '\0' terminator)
	 */
	RuntimeUserException(const char* reason) throw() :
		BaseException()
	{
		strncpy(m_reason, reason, 256);
		m_reason[255] = '\0';
		sprintf(m_errorMessage, "Generic runtime exception, reason: %s", m_reason);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	RuntimeUserException(const RuntimeUserException& other) throw() :
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
	RuntimeUserException& operator=(const RuntimeUserException& other) throw()
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
	virtual ~RuntimeUserException() throw()
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
	EXCEPTION_HELPER_FUNCTIONS(RuntimeUserException)

private:
	/**
	 * \brief The description of why the exception was thrown
	 */
	char m_reason[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[512];
};

/**
 * \brief The helper function to throw an exception of type RuntimeUserException
 *
 * This helper function takes a QString parameter instead of char* to ease
 * throwing an exception of type RuntimeUserException (see class description)
 * \param reason the description of why the exception was thrown
 */
inline void FARSA_UTIL_TEMPLATE throwUserRuntimeError(QString reason)
{
	throw RuntimeUserException(reason.toLatin1().data());
}

/**
 * \brief An exception thrown when we catch standard library exceptions in a
 *        worker thread
 *
 * We cannot clone standard library exceptions, so we simply copy the what()
 * message here and use this to give feedback to the user
 */
class FARSA_UTIL_TEMPLATE StandardLibraryException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param origException the standard library exception whose what()
	 *                      message we copy. The buffer for the message is
	 *                      at most 256 characters (including the '\0'
	 *                      terminator)
	 */
	StandardLibraryException(const std::exception& origException) throw() :
		BaseException()
	{
		strncpy(m_reason, origException.what(), 256);
		m_reason[255] = '\0';
		sprintf(m_errorMessage, "Standard library exception thrown, what(): %s", m_reason);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	StandardLibraryException(const StandardLibraryException& other) throw() :
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
	StandardLibraryException& operator=(const StandardLibraryException& other) throw()
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
	virtual ~StandardLibraryException() throw()
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
	 * \brief Returns the what() message of the original exception
	 *
	 * \return the what() message of the original exception
	 */
	const char *reason() const throw()
	{
		return m_reason;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(StandardLibraryException)

private:
	/**
	 * \brief The what() message of the original exception
	 */
	char m_reason[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when the association between uploader and
 *        downloader is not 1:1
 */
class UploaderDownloaderAssociationNotUniqueException : public BaseException
{
public:
	/**
	 * \brief The possible types of error
	 */
	enum ErrorType {
		DownloaderAlreadyAssociated, /**< The downloader is already
		                                  associated with another
		                                  uploader */
		UploaderAlreadyAssociated /**< The uploader is already
		                               associated with another
		                               downloader */
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param errorType the type of error
	 */
	UploaderDownloaderAssociationNotUniqueException(ErrorType errorType) throw() :
		BaseException(),
		m_errorType(errorType)
	{
		switch (m_errorType) {
			case DownloaderAlreadyAssociated:
				sprintf(m_errorMessage, "The downloader is already associated with another uploader");
				break;
			case UploaderAlreadyAssociated:
				sprintf(m_errorMessage, "The uploader is already associated with another downloader");
				break;
		}
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	UploaderDownloaderAssociationNotUniqueException(const UploaderDownloaderAssociationNotUniqueException& other) throw() :
		BaseException(other),
		m_errorType(other.m_errorType)
	{
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	UploaderDownloaderAssociationNotUniqueException& operator=(const UploaderDownloaderAssociationNotUniqueException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		m_errorType = other.m_errorType;
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~UploaderDownloaderAssociationNotUniqueException() throw()
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
	 * \brief Returns the type of error
	 *
	 * \return the type of error
	 */
	ErrorType errorType() const throw()
	{
		return m_errorType;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(UploaderDownloaderAssociationNotUniqueException)

private:
	/**
	 * \brief The type of error
	 */
	ErrorType m_errorType;

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[256];
};

/**
 * \brief The exception thrown when trying to create a datum or download without
 *        an uploader/downloader association in place
 */
class UploaderDownloaderAssociationNotPresentException : public BaseException
{
public:
	/**
	 * \brief The possible types of error
	 */
	enum ErrorType {
		DownloaderNotPresent, /**< The uploader has been asked to
		                           perform an operation on the queue but
		                           the association with the downloader
		                           is not present */
		UploaderNotPresent /**< The downloader has been asked to perform
		                        an operation on the queue but the
		                        association with the uploader is not
		                        present */
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param errorType the type of error
	 */
	UploaderDownloaderAssociationNotPresentException(ErrorType errorType) throw() :
		BaseException(),
		m_errorType(errorType)
	{
		switch (m_errorType) {
			case DownloaderNotPresent:
				sprintf(m_errorMessage, "The uploader is not associated with a downloader and cannot work correctly in this situation");
				break;
			case UploaderNotPresent:
				sprintf(m_errorMessage, "The downloader is not associated with an uploader and cannot work correctly in this situation");
				break;
		}
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	UploaderDownloaderAssociationNotPresentException(const UploaderDownloaderAssociationNotPresentException& other) throw() :
		BaseException(other),
		m_errorType(other.m_errorType)
	{
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	UploaderDownloaderAssociationNotPresentException& operator=(const UploaderDownloaderAssociationNotPresentException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		m_errorType = other.m_errorType;
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~UploaderDownloaderAssociationNotPresentException() throw()
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
	 * \brief Returns the type of error
	 *
	 * \return the type of error
	 */
	ErrorType errorType() const throw()
	{
		return m_errorType;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(UploaderDownloaderAssociationNotPresentException)

private:
	/**
	 * \brief The type of error
	 */
	ErrorType m_errorType;

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[256];
};

/**
 * \brief The exception thrown when using invalid combinations of
 *        NewDatumAvailableBehavior and objects for notifications
 */
class InvalidNewDatumAvailableBehaviorException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param description the description of the error. The buffer
	 *                    for this is at most 256 characters
	 *                    (including the '\0' terminator)
	 */
	InvalidNewDatumAvailableBehaviorException(const char* description) throw() :
		BaseException()
	{
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
		sprintf(m_errorMessage, "Invalid combination of NewDatumAvailableBehavior and objects for notification: %s", m_description);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	InvalidNewDatumAvailableBehaviorException(const InvalidNewDatumAvailableBehaviorException& other) throw() :
		BaseException(other)
	{
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	InvalidNewDatumAvailableBehaviorException& operator=(const InvalidNewDatumAvailableBehaviorException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~InvalidNewDatumAvailableBehaviorException() throw()
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
	 * \brief Returns the description of the error
	 *
	 * \return the description of the error
	 */
	const char *description() const throw()
	{
		return m_description;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(InvalidNewDatumAvailableBehaviorException)

private:
	/**
	 * \brief The description of the error
	 */
	char m_description[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when DependencySorter finds a circular dependency
 */
class CircularDependencyException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	CircularDependencyException() throw() :
		BaseException()
	{
		sprintf(m_errorMessage, "Circular dependency found");
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CircularDependencyException(const CircularDependencyException& other) throw() :
		BaseException(other)
	{
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CircularDependencyException& operator=(const CircularDependencyException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CircularDependencyException() throw()
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
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CircularDependencyException)

private:
	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[256];
};

} // end namespace farsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif
