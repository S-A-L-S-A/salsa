/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef NNFWEXCEPTIONS_H
#define NNFWEXCEPTIONS_H

/*! \file
 *  \brief This file contains all exceptions thrown by nnfw classes
 *
 */

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
 * \brief Thrown when a user attempt to call the operator= on a DoubleVector
 *        with isinternal flag on
 */
class SALSA_NNFW_TEMPLATE VectorAssignmentNotAllowed : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	VectorAssignmentNotAllowed() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	VectorAssignmentNotAllowed(const VectorAssignmentNotAllowed& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	VectorAssignmentNotAllowed& operator=(const VectorAssignmentNotAllowed& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~VectorAssignmentNotAllowed() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "It is not allowed to use the operator=() on DoubleVector used as internal data";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(VectorAssignmentNotAllowed)
};

/**
 * \brief Thrown when a user attempt to resize a DoubleVector with isinternal
 *        flag on
 */
class SALSA_NNFW_TEMPLATE VectorResizeNotAllowed : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	VectorResizeNotAllowed() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	VectorResizeNotAllowed(const VectorResizeNotAllowed& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	VectorResizeNotAllowed& operator=(const VectorResizeNotAllowed& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~VectorResizeNotAllowed() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "It is not allowed to resize a DoubleVector used as internal data";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(VectorResizeNotAllowed)
};

/**
 * \brief Thrown when a user attempt to do calculations with incompatible
 *        DoubleVectors
 */
class SALSA_NNFW_TEMPLATE IncompatibleVectors : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param description the description of the error. The buffer for this
	 *                    is at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	IncompatibleVectors(const char* description) throw() :
		BaseException()
	{
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	IncompatibleVectors(const IncompatibleVectors& other) throw() :
		BaseException(other)
	{
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	IncompatibleVectors& operator=(const IncompatibleVectors& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~IncompatibleVectors() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_description;
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
	EXCEPTION_HELPER_FUNCTIONS(IncompatibleVectors)

private:
	/**
	 * \brief The description of the error
	 */
	char m_description[256];
};

/**
 * \brief Thrown when a user attempt to access outside boundary of a
 *        DoubleVector
 */
class SALSA_NNFW_TEMPLATE OutsideVectorBoundaries : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param description the description of the error. The buffer for this
	 *                    is at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	OutsideVectorBoundaries(const char* description) throw() :
		BaseException()
	{
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	OutsideVectorBoundaries(const OutsideVectorBoundaries& other) throw() :
		BaseException(other)
	{
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	OutsideVectorBoundaries& operator=(const OutsideVectorBoundaries& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~OutsideVectorBoundaries() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_description;
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
	EXCEPTION_HELPER_FUNCTIONS(OutsideVectorBoundaries)

private:
	/**
	 * \brief The description of the error
	 */
	char m_description[256];
};

/**
 * \brief Thrown when a user attempt to create a Linker without specify the
 *        "from" or "to"
 */
class SALSA_NNFW_TEMPLATE ClusterFromOrToMissing : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	ClusterFromOrToMissing() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ClusterFromOrToMissing(const ClusterFromOrToMissing& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ClusterFromOrToMissing& operator=(const ClusterFromOrToMissing& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ClusterFromOrToMissing() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "The Cluster 'from' or 'to' is missing - Check your configuration file";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(ClusterFromOrToMissing)
};

/**
 * \brief Thrown when a user attempt to call the operator= on a DoubleMatrix
 *        with isinternal flag on
 */
class SALSA_NNFW_TEMPLATE MatrixAssignmentNotAllowed : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	MatrixAssignmentNotAllowed() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	MatrixAssignmentNotAllowed(const MatrixAssignmentNotAllowed& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	MatrixAssignmentNotAllowed& operator=(const MatrixAssignmentNotAllowed& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~MatrixAssignmentNotAllowed() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "It is not allowed to use the operator=() on DoubleMatrix used as internal data";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(MatrixAssignmentNotAllowed)
};

/**
 * \brief Thrown when a user attempt to resize a DoubleMatrix with isinternal
 *        flag on
 */
class SALSA_NNFW_TEMPLATE MatrixResizeNotAllowed : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	MatrixResizeNotAllowed() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	MatrixResizeNotAllowed(const MatrixResizeNotAllowed& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	MatrixResizeNotAllowed& operator=(const MatrixResizeNotAllowed& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~MatrixResizeNotAllowed() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "It is not allowed to resize a DoubleMatrix used as internal data";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(MatrixResizeNotAllowed)
};

/**
 * \brief Thrown when a user attempt to do calculations with incompatible
 *        DoubleMatrix
 */
class SALSA_NNFW_TEMPLATE IncompatibleMatrices : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param description the description of the error. The buffer for this
	 *                    is at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	IncompatibleMatrices(const char* description) throw() :
		BaseException()
	{
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	IncompatibleMatrices(const IncompatibleMatrices& other) throw() :
		BaseException(other)
	{
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	IncompatibleMatrices& operator=(const IncompatibleMatrices& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~IncompatibleMatrices() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_description;
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
	EXCEPTION_HELPER_FUNCTIONS(IncompatibleMatrices)

private:
	/**
	 * \brief The description of the error
	 */
	char m_description[256];
};

/**
 * \brief Thrown when a user attempt to access outside boundary of a
 *        DoubleMatrix
 */
class SALSA_NNFW_TEMPLATE OutsideMatrixBoundaries : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param description the description of the error. The buffer for this
	 *                    is at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	OutsideMatrixBoundaries(const char* description) throw() :
		BaseException()
	{
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	OutsideMatrixBoundaries(const OutsideMatrixBoundaries& other) throw() :
		BaseException(other)
	{
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	OutsideMatrixBoundaries& operator=(const OutsideMatrixBoundaries& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~OutsideMatrixBoundaries() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_description;
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
	EXCEPTION_HELPER_FUNCTIONS(OutsideMatrixBoundaries)

private:
	/**
	 * \brief The description of the error
	 */
	char m_description[256];
};

/**
 * \brief Thrown when a user attempt to call setCluster on an OutputFunction
 *        already inserted into a Cluster
 */
class SALSA_NNFW_TEMPLATE OutputFunctionSetClusterException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 */
	OutputFunctionSetClusterException() throw() :
		BaseException()
	{
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	OutputFunctionSetClusterException(const OutputFunctionSetClusterException& other) throw() :
		BaseException(other)
	{
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	OutputFunctionSetClusterException& operator=(const OutputFunctionSetClusterException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~OutputFunctionSetClusterException() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return "setCluster called on an OutputFunction already configured to be part of another Cluster!";
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(OutputFunctionSetClusterException)
};

/**
 * \brief Thrown when a user attempt to get a delegate for a state vector that
 *        does not exists
 */
class SALSA_NNFW_TEMPLATE ClusterStateVectorNotPresent : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param description the description of the error. The buffer for this
	 *                    is at most 256 characters (including the '\0'
	 *                    terminator)
	 */
	ClusterStateVectorNotPresent(const char* description) throw() :
		BaseException()
	{
		strncpy(m_description, description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	ClusterStateVectorNotPresent(const ClusterStateVectorNotPresent& other) throw() :
		BaseException(other)
	{
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	ClusterStateVectorNotPresent& operator=(const ClusterStateVectorNotPresent& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_description, other.m_description, 256);
		m_description[255] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ClusterStateVectorNotPresent() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_description;
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
	EXCEPTION_HELPER_FUNCTIONS(ClusterStateVectorNotPresent)

private:
	/**
	 * \brief The description of the error
	 */
	char m_description[256];
};

}

#endif
