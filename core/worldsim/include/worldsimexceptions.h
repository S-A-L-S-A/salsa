/********************************************************************************
 *  FARSA                                                                       *
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

#ifndef WORLDSIMEXCEPTIONS_H
#define WORLDSIMEXCEPTIONS_H

#include "worldsimconfig.h"
#include "baseexception.h"

namespace farsa {

/**
 * \brief The exception thrown when trying to create a WEntity without using the
 *        World::createEntity function
 */
class FARSA_WSIM_TEMPLATE EntityCreatedOutsideWorldException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param objectName the name of the object that was created without
	 *                   using World::createEntity. The buffer for this is
	 *                   at most 256 characters (including the '\0'
	 *                   terminator)
	 */
	EntityCreatedOutsideWorldException(const char* objectName) throw() :
		BaseException()
	{
		strncpy(m_objectName, objectName, 256);
		m_objectName[255] = '\0';
		sprintf(m_errorMessage, "Error attempting to create the WEntity \"%s\" outside World: you must create WEntity using World::createEntity", m_objectName);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	EntityCreatedOutsideWorldException(const EntityCreatedOutsideWorldException& other) throw() :
		BaseException(other)
	{
		strncpy(m_objectName, other.m_objectName, 256);
		m_objectName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	EntityCreatedOutsideWorldException& operator=(const EntityCreatedOutsideWorldException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_objectName, other.m_objectName, 256);
		m_objectName[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 512);
		m_errorMessage[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~EntityCreatedOutsideWorldException() throw()
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
	 * \brief Returns the name of the object that was created without using
	 *        World::createEntity
	 *
	 * \return the name of the object that was created without using
	 *         World::createEntity
	 */
	const char *objectName() const throw()
	{
		return m_objectName;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(EntityCreatedOutsideWorldException)

private:
	/**
	 * \brief The name of the object that was created without using
	 *        World::createEntity
	 */
	char m_objectName[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[512];
};

/**
 * \brief The exception thrown when trying to create a RendererContainer without
 *        using the World::createRenderersContainer function
 */
class FARSA_WSIM_TEMPLATE RendererContainerCreatedOutsideWorldException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	RendererContainerCreatedOutsideWorldException() throw() :
		BaseException()
	{
		sprintf(m_errorMessage, "Error attempting to create an AbstractRendererContainer outside World: you must create AbstractRendererContainer using World::createRenderersContainer");
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	RendererContainerCreatedOutsideWorldException(const RendererContainerCreatedOutsideWorldException& other) throw() :
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
	RendererContainerCreatedOutsideWorldException& operator=(const RendererContainerCreatedOutsideWorldException& other) throw()
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
	virtual ~RendererContainerCreatedOutsideWorldException() throw()
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
	EXCEPTION_HELPER_FUNCTIONS(RendererContainerCreatedOutsideWorldException)

private:
	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[256];
};

/**
 * \brief The exception thrown when trying to create a compound object with an
 *        empty component list
 */
class FARSA_WSIM_TEMPLATE PhyCompoundComponentsListEmptyException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	PhyCompoundComponentsListEmptyException() throw() :
		BaseException()
	{
		sprintf(m_errorMessage, "Cannot create a PhyCompound object with an empty components list");
		m_errorMessage[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	PhyCompoundComponentsListEmptyException(const PhyCompoundComponentsListEmptyException& other) throw() :
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
	PhyCompoundComponentsListEmptyException& operator=(const PhyCompoundComponentsListEmptyException& other) throw()
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
	virtual ~PhyCompoundComponentsListEmptyException() throw()
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
	EXCEPTION_HELPER_FUNCTIONS(PhyCompoundComponentsListEmptyException)

private:
	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[256];
};

}

#endif
