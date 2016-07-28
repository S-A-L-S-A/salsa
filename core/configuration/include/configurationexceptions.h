/***************************************************************************
 *  SALSA Configuration Library                                            *
 *  Copyright (C) 2007-2014                                                *
 *  Gianluca Massera <emmegian@yahoo.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#ifndef CONFIGURATIONEXCEPTIONS_H
#define CONFIGURATIONEXCEPTIONS_H

#include <typeinfo>
#include <exception>
#include <cstring>
#include <cstdio>
#include "baseexception.h"

// All the suff below is to avoid warnings on Windows about the use of the
// unsafe function sprintf and strcpy...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

// NOTE: I don't use snprintf instead of sprintf because it seems not to be in
// the current C++ standard (C++03, it is instead in the C99 standard). Note
// however that no buffer overflow is possible (buffer lengths are carefully
// checked)

/**
 * \brief The exception thrown when trying to create or rename a group using an
 *        invalid name
 */
class SALSA_CONF_TEMPLATE InvalidGroupNameException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param groupName the invalid name of the group. The buffer for this
	 *                  is at most 256 characters (including the '\0'
	 *                  terminator)
	 */
	InvalidGroupNameException(const char* groupName) throw()
		: BaseException()
	{
		strncpy(m_groupName, groupName, 256);
		m_groupName[255] = '\0';
		sprintf(m_errorMessage, "Invalid name of group: \"%s\"", m_groupName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	InvalidGroupNameException(const InvalidGroupNameException& other) throw()
		: BaseException(other)
	{
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	InvalidGroupNameException& operator=(const InvalidGroupNameException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~InvalidGroupNameException() throw()
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
	 * \brief Returns the invalid name of the group
	 *
	 * \return the invalid name of the group
	 */
	const char *groupName() const throw()
	{
		return m_groupName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(InvalidGroupNameException)

private:
	char m_groupName[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to create or rename a group using a
 *        name that already exists
 */
class SALSA_CONF_TEMPLATE AlreadyExistingGroupNameException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param groupName the already existing name of the group. The buffer
	 *                  for this is at most 256 characters (including the
	 *                  '\0' terminator)
	 */
	AlreadyExistingGroupNameException(const char* groupName) throw()
		: BaseException()
	{
		strncpy(m_groupName, groupName, 256);
		m_groupName[255] = '\0';
		sprintf(m_errorMessage, "Group \"%s\" already exists", m_groupName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	AlreadyExistingGroupNameException(const AlreadyExistingGroupNameException& other) throw()
		: BaseException(other)
	{
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	AlreadyExistingGroupNameException& operator=(const AlreadyExistingGroupNameException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~AlreadyExistingGroupNameException() throw()
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
	 * \brief Returns the already existing name of the group
	 *
	 * \return the already existing name of the group
	 */
	const char *groupName() const throw()
	{
		return m_groupName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(AlreadyExistingGroupNameException)

private:
	char m_groupName[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when referring to a non-existent group
 */
class SALSA_CONF_TEMPLATE NonExistentGroupNameException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param groupName the non-existent group. The buffer for this is at
	 *                  most 256 characters (including the '\0' terminator)
	 */
	NonExistentGroupNameException(const char* groupName) throw()
		: BaseException()
	{
		strncpy(m_groupName, groupName, 256);
		m_groupName[255] = '\0';
		sprintf(m_errorMessage, "The group \"%s\" does not exist", m_groupName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	NonExistentGroupNameException(const NonExistentGroupNameException& other) throw()
		: BaseException(other)
	{
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	NonExistentGroupNameException& operator=(const NonExistentGroupNameException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~NonExistentGroupNameException() throw()
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
	 * \brief Returns the non-existent name of the group
	 *
	 * \return the non-existent name of the group
	 */
	const char *groupName() const throw()
	{
		return m_groupName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(NonExistentGroupNameException)

private:
	char m_groupName[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to create a parameter using an
 *        invalid name
 */
class SALSA_CONF_TEMPLATE InvalidParameterNameException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param parameter the invalid name of the parameter. The buffer for
	 *                  this is at most 256 characters (including the '\0'
	 *                  terminator)
	 */
	InvalidParameterNameException(const char* parameter) throw()
		: BaseException()
	{
		strncpy(m_parameter, parameter, 256);
		m_parameter[255] = '\0';
		sprintf(m_errorMessage, "Invalid name of parameter: \"%s\"", m_parameter);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	InvalidParameterNameException(const InvalidParameterNameException& other) throw()
		: BaseException(other)
	{
		strncpy(m_parameter, other.m_parameter, 256);
		m_parameter[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	InvalidParameterNameException& operator=(const InvalidParameterNameException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_parameter, other.m_parameter, 256);
		m_parameter[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~InvalidParameterNameException() throw()
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
	 * \brief Returns the invalid name of the parameter
	 *
	 * \return the invalid name of the parameter
	 */
	const char *parameter() const throw()
	{
		return m_parameter;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(InvalidParameterNameException)

private:
	char m_parameter[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to add a parameter that already
 *        exists
 */
class SALSA_CONF_TEMPLATE AlreadyExistingParameterException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param parameter the already existing parameter. The buffer for this
	 *                  is at most 256 characters (including the '\0'
	 *                  terminator)
	 */
	AlreadyExistingParameterException(const char* parameter) throw()
		: BaseException()
	{
		strncpy(m_parameter, parameter, 256);
		m_parameter[255] = '\0';
		sprintf(m_errorMessage, "Parameter \"%s\" already exists", m_parameter);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	AlreadyExistingParameterException(const AlreadyExistingParameterException& other) throw()
		: BaseException(other)
	{
		strncpy(m_parameter, other.m_parameter, 256);
		m_parameter[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	AlreadyExistingParameterException& operator=(const AlreadyExistingParameterException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_parameter, other.m_parameter, 256);
		m_parameter[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~AlreadyExistingParameterException() throw()
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
	 * \brief Returns the already existing parameter
	 *
	 * \return the already existing parameter
	 */
	const char *parameter() const throw()
	{
		return m_parameter;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(AlreadyExistingParameterException)

private:
	char m_parameter[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when referring to a non-existent parameter
 */
class SALSA_CONF_TEMPLATE NonExistentParameterException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param parameter the non-existent parameter. The buffer for this is
	 *                  at most 256 characters (including the '\0'
	 *                  terminator)
	 */
	NonExistentParameterException(const char* parameter) throw()
		: BaseException()
	{
		strncpy(m_parameter, parameter, 256);
		m_parameter[255] = '\0';
		sprintf(m_errorMessage, "The parameter \"%s\" does not exist", m_parameter);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	NonExistentParameterException(const NonExistentParameterException& other) throw()
		: BaseException(other)
	{
		strncpy(m_parameter, other.m_parameter, 256);
		m_parameter[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	NonExistentParameterException& operator=(const NonExistentParameterException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_parameter, other.m_parameter, 256);
		m_parameter[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~NonExistentParameterException() throw()
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
	 * \brief Returns the non-existent parameter
	 *
	 * \return the non-existent parameter
	 */
	const char *parameter() const throw()
	{
		return m_parameter;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(NonExistentParameterException)

private:
	char m_parameter[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to obtain the lowest common ancestor
 *        of two nodes on separated trees
 */
class SALSA_CONF_TEMPLATE NoCommonAncestorException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	NoCommonAncestorException() throw()
		: BaseException()
	{
		sprintf(m_errorMessage, "The nodes have no common ancestor (they are on separated trees)");
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	NoCommonAncestorException(const NoCommonAncestorException& other) throw()
		: BaseException(other)
	{
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	NoCommonAncestorException& operator=(const NoCommonAncestorException& other) throw()
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
	virtual ~NoCommonAncestorException() throw()
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
	EXCEPTION_HELPER_FUNCTIONS(NoCommonAncestorException)

private:
	char m_errorMessage[256];
};

/**
 * \brief The exception thrown when trying to register a class whose parent or
 *        other ancestor has not been registered yet
 */
class SALSA_CONF_TEMPLATE AncestorNotRegisteredException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param className the name of the class whose ancestor has not been
	 *                  registered. The buffer for this is at most 256
	 *                  characters(including the '\0' terminator)
	 * \param parentName the name of the ancestor that hasn't been
	 *                   registered. The buffer for this is at most 256
	 *                   characters (including the '\0' terminator)
	 */
	AncestorNotRegisteredException(const char* className, const char* ancestorName) throw()
		: BaseException()
	{
		strncpy(m_className, className, 256);
		m_className[255] = '\0';
		strncpy(m_ancestorName, ancestorName, 256);
		m_ancestorName[255] = '\0';
		sprintf(m_errorMessage, "Impossible to register class \"%s\" whose ancestor \"%s\" hasn't been registered yet", m_className, m_ancestorName);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	AncestorNotRegisteredException(const AncestorNotRegisteredException& other) throw()
		: BaseException(other)
	{
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_ancestorName, other.m_ancestorName, 256);
		m_ancestorName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	AncestorNotRegisteredException& operator=(const AncestorNotRegisteredException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_ancestorName, other.m_ancestorName, 256);
		m_ancestorName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~AncestorNotRegisteredException() throw()
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
	 * \brief Returns the name of the class whose ancestor has not been
	 *        registered
	 *
	 * \return the name of the class whose ancestor has not been registered
	 */
	const char *className() const throw()
	{
		return m_className;
	}

	/**
	 * \brief Returns the name of the ancestor that hasn't been registered
	 *
	 * \return the name of the ancestor that hasn't been registered
	 */
	const char *ancestorName() const throw()
	{
		return m_ancestorName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(AncestorNotRegisteredException)

private:
	char m_className[256];
	char m_ancestorName[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when there are incompatible configuration
 *        strategies in a class hierachy at registration time
 */
class SALSA_CONF_TEMPLATE IncompatibleConfigurationStrategiesException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param regClassName the name of the class being registered when the
	 *                     exception was thrown. The buffer for this is at
	 *                     most 256 characters (including the '\0'
	 *                     terminator)
	 * \param childName the child class whose configuration strategy is
	 *                  incompatible with the one of its parent. The buffer
	 *                  for this is at most 256 characters (including the
	 *                  '\0' terminator)
	 * \param parentName the parent class whose configuration strategy is
	 *                   incompatible with the one of one of its children.
	 *                   The buffer for this is at most 256 characters
	 *                   (including the '\0' terminator)
	 */
	IncompatibleConfigurationStrategiesException(const char* regClassName, const char* childName, const char* parentName) throw()
		: BaseException()
	{
		strncpy(m_regClassName, regClassName, 256);
		m_regClassName[255] = '\0';
		strncpy(m_childName, childName, 256);
		m_childName[255] = '\0';
		strncpy(m_parentName, parentName, 256);
		m_parentName[255] = '\0';
		sprintf(m_errorMessage, "Incompatible configuration strategies between \"%s\" and its parent \"%s\" when registering class \"%s\"", m_childName, m_parentName, m_regClassName);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	IncompatibleConfigurationStrategiesException(const IncompatibleConfigurationStrategiesException& other) throw()
		: BaseException(other)
	{
		strncpy(m_regClassName, other.m_regClassName, 256);
		m_regClassName[255] = '\0';
		strncpy(m_childName, other.m_childName, 256);
		m_childName[255] = '\0';
		strncpy(m_parentName, other.m_parentName, 256);
		m_parentName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	IncompatibleConfigurationStrategiesException& operator=(const IncompatibleConfigurationStrategiesException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_regClassName, other.m_regClassName, 256);
		m_regClassName[255] = '\0';
		strncpy(m_childName, other.m_childName, 256);
		m_childName[255] = '\0';
		strncpy(m_parentName, other.m_parentName, 256);
		m_parentName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~IncompatibleConfigurationStrategiesException() throw()
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
	 * \brief Returns the name of the class being registered when the
	 *        exception was thrown
	 *
	 * \return the name of the class being registered when the exception was
	 *         thrown
	 */
	const char *regClassName() const throw()
	{
		return m_regClassName;
	}

	/**
	 * \brief Returns the child class whose configuration strategy is
	 *        incompatible with the one of its parent
	 *
	 * \return the child class whose configuration strategy is incompatible
	 *         with the one of its parent
	 */
	const char *childName() const throw()
	{
		return m_childName;
	}

	/**
	 * \brief Returns the parent class whose configuration strategy is
	 *        incompatible with the one of one of its children
	 *
	 * \return the parent class whose configuration strategy is incompatible
	 *         with the one of one of its children
	 */
	const char *parentName() const throw()
	{
		return m_parentName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(IncompatibleConfigurationStrategiesException)

private:
	char m_regClassName[256];
	char m_childName[256];
	char m_parentName[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when trying to register a component without any
 *        registered parent component
 */
class SALSA_CONF_TEMPLATE ComponentHasNoParentComponentException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param className the name of the component without any registered
	 *                  parent component. The buffer for this is at most 256
	 *                  characters(including the '\0' terminator)
	 */
	ComponentHasNoParentComponentException(const char* className) throw()
		: BaseException()
	{
		strncpy(m_className, className, 256);
		m_className[255] = '\0';
		sprintf(m_errorMessage, "The component \"%s\" has no registered parent component", m_className);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ComponentHasNoParentComponentException(const ComponentHasNoParentComponentException& other) throw()
		: BaseException(other)
	{
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ComponentHasNoParentComponentException& operator=(const ComponentHasNoParentComponentException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ComponentHasNoParentComponentException() throw()
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
	 * \brief Returns the name of the class whose ancestor has not been
	 *        registered
	 *
	 * \return the name of the class whose ancestor has not been registered
	 */
	const char *className() const throw()
	{
		return m_className;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ComponentHasNoParentComponentException)

private:
	char m_className[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to re-register a class that is
 *        already registered and has different characteristics or parents
 */
class SALSA_CONF_TEMPLATE CannotReRegisterType : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param className the name of the class that is re-registered. The
	 *                  buffer for this is at most 256 characters (including
	 *                  the '\0' terminator)
	 * \param reason the reason why re-registration is not possible. The
	 *               buffer for this is at most 256 characters (including
	 *               the '\0' terminator)
	 */
	CannotReRegisterType(const char* className, const char* reason) throw()
		: BaseException()
	{
		strncpy(m_className, className, 256);
		m_className[255] = '\0';
		strncpy(m_reason, reason, 256);
		m_reason[255] = '\0';
		sprintf(m_errorMessage, "Impossible to re-register class \"%s\", reason: %s", m_className, m_reason);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CannotReRegisterType(const CannotReRegisterType& other) throw()
		: BaseException(other)
	{
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CannotReRegisterType& operator=(const CannotReRegisterType& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CannotReRegisterType() throw()
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
	 * \brief Returns the name of the class whose ancestor has not been
	 *        registered
	 *
	 * \return the name of the class whose ancestor has not been registered
	 */
	const char *className() const throw()
	{
		return m_className;
	}

	/**
	 * \brief Returns the reason why re-registration is not possible
	 *
	 * \return the reason why re-registration is not possible
	 */
	const char *reason() const throw()
	{
		return m_reason;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CannotReRegisterType)

private:
	char m_className[256];
	char m_reason[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when requested class name is not registered with
 *        the factory
 */
class SALSA_CONF_TEMPLATE ClassNameNotRegisteredException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param className the name of the class that couldn't be found. The
	 *                  buffer for this is at most 256 characters
	 *                  (including the '\0' terminator)
	 */
	ClassNameNotRegisteredException(const char* className) throw()
		: BaseException()
	{
		strncpy(m_className, className, 256);
		m_className[255] = '\0';
		sprintf(m_errorMessage, "No class \"%s\" registered into Factory", m_className);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ClassNameNotRegisteredException(const ClassNameNotRegisteredException& other) throw()
		: BaseException(other)
	{
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ClassNameNotRegisteredException& operator=(const ClassNameNotRegisteredException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ClassNameNotRegisteredException() throw()
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
	 * \brief Returns the name of the class that couldn't be found
	 *
	 * \return the name of the class that couldn't be found
	 */
	const char *className() const throw()
	{
		return m_className;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ClassNameNotRegisteredException)

private:
	char m_className[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when requested class name is registered but
 *        cannot be created because it is abstract
 */
class SALSA_CONF_TEMPLATE ClassNameIsAbstractException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param className the name of the class that couldn't be found. The
	 *                  buffer for this is at most 256 characters
	 *                  (including the '\0' terminator)
	 */
	ClassNameIsAbstractException(const char* className) throw()
		: BaseException()
	{
		strncpy(m_className, className, 256);
		m_className[255] = '\0';
		sprintf(m_errorMessage, "The class \"%s\" is abstract and cannot be created", m_className);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ClassNameIsAbstractException(const ClassNameIsAbstractException& other) throw()
		: BaseException(other)
	{
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ClassNameIsAbstractException& operator=(const ClassNameIsAbstractException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ClassNameIsAbstractException() throw()
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
	 * \brief Returns the name of the class that couldn't be found
	 *
	 * \return the name of the class that couldn't be found
	 */
	const char *className() const throw()
	{
		return m_className;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ClassNameIsAbstractException)

private:
	char m_className[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when casting a Component object to the
 *        requested type fails
 */
class SALSA_CONF_TEMPLATE CannotConvertToTypeException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param className the name of the class that was requested
	 * \param destTypeID the type id of the type to which the new object
	 *                   should have been converted
	 */
	CannotConvertToTypeException(const char* className, const std::type_info& destTypeId) throw()
		: BaseException()
		, m_destTypeId(&destTypeId)
	{
		strncpy(m_className, className, 256);
		m_className[255] = '\0';
		// This is to be absolutely sure there is no buffer overflow
		char tmp[256];
		strncpy(tmp, m_destTypeId->name(), 256);
		tmp[255] = '\0';
		sprintf(m_errorMessage, "Impossible to convert object of type \"%s\" to type \"%s\"", m_className, tmp);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CannotConvertToTypeException(const CannotConvertToTypeException& other) throw()
		: BaseException(other)
		, m_destTypeId(other.m_destTypeId)
	{
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CannotConvertToTypeException& operator=(const CannotConvertToTypeException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_className, other.m_className, 256);
		m_className[255] = '\0';
		m_destTypeId = other.m_destTypeId;
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CannotConvertToTypeException() throw()
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
	 * \brief Returns the name of the class that couldn't be found
	 *
	 * \return the name of the class that couldn't be found
	 */
	const char *className() const throw()
	{
		return m_className;
	}

	/**
	 * \brief Returns the type_info of the type to which the new object
	 *        should have been converted
	 *
	 * \return the type_info of the type to which the new object should have
	 *         been converted
	 */
	const std::type_info& destTypeName() const throw()
	{
		return *m_destTypeId;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CannotConvertToTypeException)

private:
	char m_className[256];
	const std::type_info* m_destTypeId;
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when requested to create an object from a group
 *        and the "type" parameter is not in the group
 */
class SALSA_CONF_TEMPLATE CannotFindTypeParameterException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param groupName the name of the group that should have had the
	 *                  "type" parameter
	 */
	CannotFindTypeParameterException(const char* groupName) throw()
		: BaseException()
	{
		strncpy(m_groupName, groupName, 256);
		m_groupName[255] = '\0';
		sprintf(m_errorMessage, "No \"type\" parameter in the given parameter object under the \"%s\" group", m_groupName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CannotFindTypeParameterException(const CannotFindTypeParameterException& other) throw()
		: BaseException(other)
	{
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CannotFindTypeParameterException& operator=(const CannotFindTypeParameterException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CannotFindTypeParameterException() throw()
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
	 * \brief Returns the name of the class that couldn't be found
	 *
	 * \return the name of the class that couldn't be found
	 */
	const char *groupName() const throw()
	{
		return m_groupName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CannotFindTypeParameterException)

private:
	char m_groupName[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to create an object from a using a
 *        prefix that is not a group
 */
class SALSA_CONF_TEMPLATE PrefixNotGroupException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param prefix the prefix that we expected to be a group but is not
	 */
	PrefixNotGroupException(const char* prefix) throw()
		: BaseException()
	{
		strncpy(m_prefix, prefix, 256);
		m_prefix[255] = '\0';
		sprintf(m_errorMessage, "The provided prefix (\"%s\") is not a group name, cannot ceate object", m_prefix);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	PrefixNotGroupException(const PrefixNotGroupException& other) throw()
		: BaseException(other)
	{
		strncpy(m_prefix, other.m_prefix, 256);
		m_prefix[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	PrefixNotGroupException& operator=(const PrefixNotGroupException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_prefix, other.m_prefix, 256);
		m_prefix[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~PrefixNotGroupException() throw()
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
	 * \brief Returns the prefix that we expected to be a group but is not
	 *
	 * \return the prefix that we expected to be a group but is not
	 */
	const char *prefix() const throw()
	{
		return m_prefix;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(PrefixNotGroupException)

private:
	char m_prefix[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when a cyclic dependency is discovered during
 *        object creation using the ConfigurationManager facilities
 */
class SALSA_CONF_TEMPLATE CyclicDependencyException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param groupName the name of the group in which the cycle has been
	 *                  discovered. The buffer for this is at most 256
	 *                  characters (including the '\0' terminator)
	 */
	CyclicDependencyException(const char* groupName) throw()
		: BaseException()
	{
		strncpy(m_groupName, groupName, 256);
		m_groupName[255] = '\0';
		sprintf(m_errorMessage, "Cyclic dependency discovered when creating/configuring object for group \"%s\"", m_groupName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CyclicDependencyException(const CyclicDependencyException& other) throw()
		: BaseException(other)
	{
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CyclicDependencyException& operator=(const CyclicDependencyException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_groupName, other.m_groupName, 256);
		m_groupName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CyclicDependencyException() throw()
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
	 * \brief Returns the name of the group in which the cycle has been
	 *        discovered
	 *
	 * \return the name of the group in which the cycle has been discovered
	 */
	const char *groupName() const throw()
	{
		return m_groupName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CyclicDependencyException)

private:
	char m_groupName[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception to throw when a user check on a parameter fails
 *
 * Use this when a user-defined check on a parameter fails when configuring an
 * object. The ConfigurationHelper::throwUserConfigError() function provides a
 * convenient way of throwing an exception of this type (see function
 * description)
 */
class SALSA_CONF_TEMPLATE UserDefinedCheckFailureException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramName the name of the parameter on which the check failed.
	 *                  The buffer for this is at most 256 characters
	 *                  (including the '\0' terminator)
	 * \param paramValue the value of the parameter which caused the check
	 *                   to fail. The buffer for this is at most 256
	 *                   characters (including the '\0' terminator)
	 * \param description a description of the error. The buffer for this is
	 *                    at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	UserDefinedCheckFailureException(const char* paramName, const char* paramValue, const char* description) throw()
		: BaseException()
	{
		strncpy(m_paramName, paramName, 256);
		m_paramName[255] = '\0';
		strncpy(m_paramValue, paramValue, 256);
		m_paramValue[255] = '\0';
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
		sprintf(m_errorMessage, "User check failed on parameter \"%s\" - parameter value: \"%s\" - error description: %s", m_paramName, m_paramValue, m_description);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	UserDefinedCheckFailureException(const UserDefinedCheckFailureException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramName, other.m_paramName, 256);
		m_paramName[255] = '\0';
		strncpy(m_paramValue, other.m_paramValue, 256);
		m_paramValue[255] = '\0';
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	UserDefinedCheckFailureException& operator=(const UserDefinedCheckFailureException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramName, other.m_paramName, 256);
		m_paramName[255] = '\0';
		strncpy(m_paramValue, other.m_paramValue, 256);
		m_paramValue[255] = '\0';
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~UserDefinedCheckFailureException() throw()
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
	 * \brief Returns the name of the parameter
	 *
	 * \return the name of the parameter
	 */
	const char *paramName() const throw()
	{
		return m_paramName;
	}

	/**
	 * \brief Returns the value of the parameter
	 *
	 * \return the value of the parameter
	 */
	const char *paramValue() const throw()
	{
		return m_paramValue;
	}

	/**
	 * \brief Returns the error description
	 *
	 * \return the error description
	 */
	const char *description() const throw()
	{
		return m_description;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(UserDefinedCheckFailureException)

private:
	char m_paramName[256];
	char m_paramValue[256];
	char m_description[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when requesting a resource never declared
 */
class SALSA_CONF_TEMPLATE ResourceNotDeclaredException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param resourceName the name of the resource not declared. The
	 *                     buffer for this is at most 256 characters
	 *                     (including the '\0' terminator)
	 */
	ResourceNotDeclaredException(const char* resourceName) throw()
		: BaseException()
	{
		strncpy(m_resourceName, resourceName, 256);
		m_resourceName[255] = '\0';
		sprintf(m_errorMessage, "No resource declared with name \"%s\"", m_resourceName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ResourceNotDeclaredException(const ResourceNotDeclaredException& other) throw()
		: BaseException(other)
	{
		strncpy(m_resourceName, other.m_resourceName, 256);
		m_resourceName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ResourceNotDeclaredException& operator=(const ResourceNotDeclaredException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_resourceName, other.m_resourceName, 256);
		m_resourceName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ResourceNotDeclaredException() throw()
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
	 * \brief Returns the name of the resource that couldn't be found
	 *
	 * \return the name of the resource that couldn't be found
	 */
	const char *resourceName() const throw()
	{
		return m_resourceName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ResourceNotDeclaredException)

private:
	char m_resourceName[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when requesting a resource never declared
 */
class SALSA_CONF_TEMPLATE ResourceCannotResolveAmbiguityException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param resourceName the name of the resource not declared. The
	 *                     buffer for this is at most 256 characters
	 *                     (including the '\0' terminator)
	 * \param numResources the number of conflicting resources
	 */
	ResourceCannotResolveAmbiguityException(const char* resourceName, int numResources) throw()
		: BaseException()
		, m_numResources(numResources)
	{
		strncpy(m_resourceName, resourceName, 256);
		m_resourceName[255] = '\0';
		sprintf(m_errorMessage, "There are %d resources declared with name \"%s\" and it is not possible to resolve the ambiguity - check your configuration to solve the ambiguity", m_numResources, m_resourceName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ResourceCannotResolveAmbiguityException(const ResourceCannotResolveAmbiguityException& other) throw()
		: BaseException(other)
		, m_numResources(other.m_numResources)
	{
		strncpy(m_resourceName, other.m_resourceName, 256);
		m_resourceName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ResourceCannotResolveAmbiguityException& operator=(const ResourceCannotResolveAmbiguityException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_resourceName, other.m_resourceName, 256);
		m_resourceName[255] = '\0';
		m_numResources = other.m_numResources;
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ResourceCannotResolveAmbiguityException() throw()
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
	 * \brief Returns the name of the resource that couldn't be found
	 *
	 * \return the name of the resource that couldn't be found
	 */
	const char *resourceName() const throw()
	{
		return m_resourceName;
	}

	/**
	 * \brief Returns the number of conflicting resources
	 *
	 * \return the number of conflicting resources
	 */
	int numResources() const throw()
	{
		return m_numResources;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ResourceCannotResolveAmbiguityException)

private:
	char m_resourceName[256];
	char m_errorMessage[512];
	int m_numResources;
};

/**
 * \brief The exception thrown when using the function
 *        ResourceChangeNotifee::getResource() improperly
 */
class SALSA_CONF_TEMPLATE CannotCallNotifeeGetResourceException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	CannotCallNotifeeGetResourceException() throw()
		: BaseException()
	{
		sprintf(m_errorMessage, "Cannot call ResourceChangeNotifee::getResource() outside the notification callback or resource not existing");
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CannotCallNotifeeGetResourceException(const CannotCallNotifeeGetResourceException& other) throw()
		: BaseException(other)
	{
		strncpy(m_errorMessage, other.m_errorMessage, 256);
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CannotCallNotifeeGetResourceException& operator=(const CannotCallNotifeeGetResourceException& other) throw()
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
	virtual ~CannotCallNotifeeGetResourceException() throw()
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
	EXCEPTION_HELPER_FUNCTIONS(CannotCallNotifeeGetResourceException)

private:
	char m_errorMessage[256];
};

/**
 * \brief The exception to throw when a user needs a resouce during
 *        configuration which doesn't exist
 *
 * Use this when a resource requested during the configuration phase is missing.
 * The ConfigurationHelper::throwUserMissingResourceError() function provides a
 * convenient way of throwing an exception of this type (see function
 * description)
 */
class SALSA_CONF_TEMPLATE UserRequiredResourceMissingException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param resourceName the name of the resource requested but not found.
	 *                     The buffer for this is at most 256 characters
	 *                     (including the '\0' terminator)
	 * \param description a description of the error. The buffer for this is
	 *                    at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	UserRequiredResourceMissingException(const char* resourceName, const char* description) throw()
		: BaseException()
	{
		strncpy(m_resourceName, resourceName, 256);
		m_resourceName[255] = '\0';
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
		sprintf(m_errorMessage, "The user requested a resource named \"%s\" which cannot be found. Error description: %s", m_resourceName, m_description);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	UserRequiredResourceMissingException(const UserRequiredResourceMissingException& other) throw()
		: BaseException(other)
	{
		strncpy(m_resourceName, other.m_resourceName, 256);
		m_resourceName[255] = '\0';
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	UserRequiredResourceMissingException& operator=(const UserRequiredResourceMissingException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_resourceName, other.m_resourceName, 256);
		m_resourceName[255] = '\0';
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~UserRequiredResourceMissingException() throw()
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
	 * \brief Returns the name of the resource
	 *
	 * \return the name of the resource
	 */
	const char *resourceName() const throw()
	{
		return m_resourceName;
	}

	/**
	 * \brief Returns the error description
	 *
	 * \return the error description
	 */
	const char *description() const throw()
	{
		return m_description;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(UserRequiredResourceMissingException)

private:
	char m_resourceName[256];
	char m_description[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when trying to obtain a parameter that has not
 *        been described using ConfigurationHelper facilities
 */
class SALSA_CONF_TEMPLATE ParameterNotRegisteredOrWithoutDefaultException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramPath the path of the parameter not described. The buffer
	 *                  for this is at most 256 characters (including the
	 *                  '\0' terminator)
	 */
	ParameterNotRegisteredOrWithoutDefaultException(const char* paramPath) throw()
		: BaseException()
	{
		strncpy(m_paramPath, paramPath, 256);
		m_paramPath[255] = '\0';
		sprintf(m_errorMessage, "No description or missing default for parameter \"%s\"", m_paramPath);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ParameterNotRegisteredOrWithoutDefaultException(const ParameterNotRegisteredOrWithoutDefaultException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ParameterNotRegisteredOrWithoutDefaultException& operator=(const ParameterNotRegisteredOrWithoutDefaultException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ParameterNotRegisteredOrWithoutDefaultException() throw()
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
	 * \brief Returns the path of the parameter that wasn't described
	 *
	 * \return the path of the parameter that wasn't described
	 */
	const char *paramPath() const throw()
	{
		return m_paramPath;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ParameterNotRegisteredOrWithoutDefaultException)

private:
	char m_paramPath[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception to throw when using the ConfigurationHelper function
 *        with the wrong type
 */
class SALSA_CONF_TEMPLATE WrongParameterTypeException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramPath the path of the parameter with the wrong type. The
	 *                  buffer for this is at most 256 characters (including
	 *                  the '\0' terminator)
	 * \param requestedType the type that was requested. The buffer for this
	 *                      is at most 256 characters (including the '\0'
	 *                      terminator)
	 * \param declaredType the declared type of the parameter. The buffer
	 *                     for this is at most 256 characters (including the
	 *                     '\0' terminator)
	 */
	WrongParameterTypeException(const char* paramPath, const char* requestedType, const char* declaredType) throw()
		: BaseException()
	{
		strncpy(m_paramPath, paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_requestedType, requestedType, 256);
		m_requestedType[255] = '\0';
		strncpy(m_declaredType, declaredType, 256);
		m_declaredType[255] = '\0';
		sprintf(m_errorMessage, "The parameter \"%s\" was requested as type %s but it was registered as having type %s", m_paramPath, m_requestedType, m_declaredType);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	WrongParameterTypeException(const WrongParameterTypeException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_requestedType, other.m_requestedType, 256);
		m_requestedType[255] = '\0';
		strncpy(m_declaredType, other.m_declaredType, 256);
		m_declaredType[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	WrongParameterTypeException& operator=(const WrongParameterTypeException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_requestedType, other.m_requestedType, 256);
		m_requestedType[255] = '\0';
		strncpy(m_declaredType, other.m_declaredType, 256);
		m_declaredType[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~WrongParameterTypeException() throw()
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
	 * \brief Returns the path of the parameter with the wrong type
	 *
	 * \return the path of the parameter with the wrong type
	 */
	const char *paramPath() const throw()
	{
		return m_paramPath;
	}

	/**
	 * \brief Returns the type that was requested
	 *
	 * \return the type that was requested
	 */
	const char *requestedType() const throw()
	{
		return m_requestedType;
	}

	/**
	 * \brief Returns the declared type of the parameter
	 *
	 * \return the declared type of the parameter
	 */
	const char *declaredType() const throw()
	{
		return m_declaredType;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(WrongParameterTypeException)

private:
	char m_paramPath[256];
	char m_requestedType[256];
	char m_declaredType[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception to throw when an enum parameter has a default value that
 *        is not among the enum values
 */
class SALSA_CONF_TEMPLATE InvalidEnumParameterDefaultException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramPath the path of the parameter with the invalid default
	 *                  value type. The buffer for this is at most 256
	 *                  characters (including the '\0' terminator)
	 * \param invalidDefault the invalid default. The buffer for this is at
	 *                       most 256 characters (including the '\0'
	 *                       terminator)
	 */
	InvalidEnumParameterDefaultException(const char* paramPath, const char* invalidDefault) throw()
		: BaseException()
	{
		strncpy(m_paramPath, paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_invalidDefault, invalidDefault, 256);
		m_invalidDefault[255] = '\0';
		sprintf(m_errorMessage, "The enum parameter \"%s\" has an invalid default value: \"%s\"", m_paramPath, m_invalidDefault);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	InvalidEnumParameterDefaultException(const InvalidEnumParameterDefaultException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_invalidDefault, other.m_invalidDefault, 256);
		m_invalidDefault[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	InvalidEnumParameterDefaultException& operator=(const InvalidEnumParameterDefaultException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_invalidDefault, other.m_invalidDefault, 256);
		m_invalidDefault[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~InvalidEnumParameterDefaultException() throw()
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
	 * \brief Returns the path of the parameter with the wrong type
	 *
	 * \return the path of the parameter with the wrong type
	 */
	const char *paramPath() const throw()
	{
		return m_paramPath;
	}

	/**
	 * \brief Returns the invalid default
	 *
	 * \return the invalid default
	 */
	const char *invalidDefault() const throw()
	{
		return m_invalidDefault;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(InvalidEnumParameterDefaultException)

private:
	char m_paramPath[256];
	char m_invalidDefault[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception to throw when an enum parameter has a value that is not
 *        among the enum values
 */
class SALSA_CONF_TEMPLATE InvalidEnumValueException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramPath the path of the parameter with the invalid default
	 *                  value type. The buffer for this is at most 256
	 *                  characters (including the '\0' terminator)
	 * \param invalidValue the invalid value. The buffer for this is at most
	 *                     256 characters (including the '\0' terminator)
	 */
	InvalidEnumValueException(const char* paramPath, const char* invalidValue) throw()
		: BaseException()
	{
		strncpy(m_paramPath, paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_invalidValue, invalidValue, 256);
		m_invalidValue[255] = '\0';
		sprintf(m_errorMessage, "The enum parameter \"%s\" has an invalid value: \"%s\"", m_paramPath, m_invalidValue);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	InvalidEnumValueException(const InvalidEnumValueException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_invalidValue, other.m_invalidValue, 256);
		m_invalidValue[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	InvalidEnumValueException& operator=(const InvalidEnumValueException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_invalidValue, other.m_invalidValue, 256);
		m_invalidValue[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~InvalidEnumValueException() throw()
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
	 * \brief Returns the path of the parameter with the wrong type
	 *
	 * \return the path of the parameter with the wrong type
	 */
	const char *paramPath() const throw()
	{
		return m_paramPath;
	}

	/**
	 * \brief Returns the invalid value
	 *
	 * \return the invalid value
	 */
	const char *invalidValue() const throw()
	{
		return m_invalidValue;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(InvalidEnumValueException)

private:
	char m_paramPath[256];
	char m_invalidValue[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception to throw when the value of a parameter cannot be
 *        converted to the requested type
 */
class SALSA_CONF_TEMPLATE CannotConvertParameterValueToTypeException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramPath the path of the parameter that couldn't be
	 *                  converted. The buffer for this is at most 256
	 *                  characters (including the '\0' terminator)
	 * \param expectedType the type towards which the value conversion was
	 *                     attempted. The buffer for this is at most 256
	 *                     characters (including the '\0' terminator)
	 */
	CannotConvertParameterValueToTypeException(const char* paramPath, const char* expectedType) throw()
		: BaseException()
	{
		strncpy(m_paramPath, paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_expectedType, expectedType, 256);
		m_expectedType[255] = '\0';
		sprintf(m_errorMessage, "The value of the parameter \"%s\" could not be converted to %s", m_paramPath, m_expectedType);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	CannotConvertParameterValueToTypeException(const CannotConvertParameterValueToTypeException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_expectedType, other.m_expectedType, 256);
		m_expectedType[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	CannotConvertParameterValueToTypeException& operator=(const CannotConvertParameterValueToTypeException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_expectedType, other.m_expectedType, 256);
		m_expectedType[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~CannotConvertParameterValueToTypeException() throw()
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
	 * \brief Returns the path of the parameter that couldn't be converted
	 *
	 * \return the path of the parameter that couldn't be converted
	 */
	const char *paramPath() const throw()
	{
		return m_paramPath;
	}

	/**
	 * \brief Returns the type towards which the value conversion was
	 *        attempted
	 *
	 * \return the type towards which the value conversion was attempted
	 */
	const char *expectedType() const throw()
	{
		return m_expectedType;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(CannotConvertParameterValueToTypeException)

private:
	char m_paramPath[256];
	char m_expectedType[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception to throw when the value of a numeric parameter is
 *        outside the declared range
 */
class SALSA_CONF_TEMPLATE ParameterValueOutOfRangeException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param paramPath the path of the parameter whose value is out of
	 *                  range. The buffer for this is at most 256 characters
	 *                  (including the '\0' terminator)
	 * \param wrongValue the value that is out of range. The buffer for this
	 *                   is at most 256 characters (including the '\0'
	 *                   terminator)
	 */
	ParameterValueOutOfRangeException(const char* paramPath, const char* wrongValue) throw()
		: BaseException()
	{
		strncpy(m_paramPath, paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_wrongValue, wrongValue, 256);
		m_wrongValue[255] = '\0';
		sprintf(m_errorMessage, "The value of the parameter \"%s\" is out of range (value: %s)", m_paramPath, m_wrongValue);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ParameterValueOutOfRangeException(const ParameterValueOutOfRangeException& other) throw()
		: BaseException(other)
	{
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_wrongValue, other.m_wrongValue, 256);
		m_wrongValue[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ParameterValueOutOfRangeException& operator=(const ParameterValueOutOfRangeException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_paramPath, other.m_paramPath, 256);
		m_paramPath[255] = '\0';
		strncpy(m_wrongValue, other.m_wrongValue, 256);
		m_wrongValue[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ParameterValueOutOfRangeException() throw()
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
	 * \brief Returns the path of the parameter whose value is out of range
	 *
	 * \return the path of the parameter whose value is out of range
	 */
	const char *paramPath() const throw()
	{
		return m_paramPath;
	}

	/**
	 * \brief Returns the value of the parameter
	 *
	 * \return the value of the parameter
	 */
	const char *wrongValue() const throw()
	{
		return m_wrongValue;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ParameterValueOutOfRangeException)

private:
	char m_paramPath[256];
	char m_wrongValue[256];
	char m_errorMessage[1024];
};

/**
 * \brief The exception thrown when trying to decode a list failed
 */
class SALSA_CONF_TEMPLATE ListDecodeFailedException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param listType the type of the list that could not be decoded. The
	 *                 buffer for this is at most 256 characters (including
	 *                 the '\0' terminator)
	 */
	ListDecodeFailedException(const char* listType) throw()
		: BaseException()
	{
		strncpy(m_listType, listType, 256);
		m_listType[255] = '\0';
		sprintf(m_errorMessage, "Could not decode into a list of %s", m_listType);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ListDecodeFailedException(const ListDecodeFailedException& other) throw()
		: BaseException(other)
	{
		strncpy(m_listType, other.m_listType, 256);
		m_listType[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ListDecodeFailedException& operator=(const ListDecodeFailedException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_listType, other.m_listType, 256);
		m_listType[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ListDecodeFailedException() throw()
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
	 * \brief Returns the type of the list that could not be decoded
	 *
	 * \return the type of the list that could not be decoded
	 */
	const char *listType() const throw()
	{
		return m_listType;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ListDecodeFailedException)

private:
	char m_listType[256];
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to obtain the descriptor of a
 *        parameter or subgroup that has not been described
 */
class SALSA_CONF_TEMPLATE ParameterOrSubgroupNotDescribedException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param name the name of the parameter or subgroup that is not
	 *             described. The buffer for this is at most 256 characters
	 *             (including the '\0' terminator)
	 */
	ParameterOrSubgroupNotDescribedException(const char* name) throw()
		: BaseException()
	{
		strncpy(m_name, name, 256);
		m_name[255] = '\0';
		sprintf(m_errorMessage, "There is no descriptor for the parameter or subgroup \"%s\"", m_name);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ParameterOrSubgroupNotDescribedException(const ParameterOrSubgroupNotDescribedException& other) throw()
		: BaseException(other)
	{
		strncpy(m_name, other.m_name, 256);
		m_name[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ParameterOrSubgroupNotDescribedException& operator=(const ParameterOrSubgroupNotDescribedException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_name, other.m_name, 256);
		m_name[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ParameterOrSubgroupNotDescribedException() throw()
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
	 * \brief Returns the name of the parameter or subgroup that is not
	 *        described
	 *
	 * \return the name of the parameter or subgroup that is not described
	 */
	const char *name() const throw()
	{
		return m_name;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ParameterOrSubgroupNotDescribedException)

private:
	char m_name[256];
	char m_errorMessage[512];
};

} // end namespace salsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif
