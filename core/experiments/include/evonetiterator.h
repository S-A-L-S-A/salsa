/********************************************************************************
 *  FARSA Experimentes Library                                                  *
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

#ifndef EVONETITERATOR_H
#define EVONETITERATOR_H

#include "experimentsconfig.h"
#include "baseexception.h"
#include "mathutils.h"
#include "controlleriterator.h"
#include <QString>
#include <QColor>
#include <QMap>

// All the suff below is to avoid warnings on Windows about the use of the
// unsafe function sprintf and strcpy...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace farsa {

class Evonet;

/**
 * \brief The exception thrown when EvonetIterator is not in a valid status
 */
class FARSA_CONF_TEMPLATE EvonetIteratorInvalidStatusException : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param function the name of the function where the error occurred.
	 *                 The buffer for this is at most 256 characters
	 *                 (including the '\0' terminator)
	 * \param reason the string explaining why the object is not in a valid
	 *               status. The buffer for this is at most 256 characters
	 *               (including the '\0' terminator)
	 */
	EvonetIteratorInvalidStatusException(const char* function, const char* reason) throw() :
		BaseException()
	{
		strncpy(m_function, function, 256);
		m_function[255] = '\0';
		strncpy(m_reason, reason, 256);
		m_reason[255] = '\0';
		sprintf(m_errorMessage, "Invalid status for EvonetItarator in function \"%s\", reason: %s", m_function, m_reason);
		m_errorMessage[1023] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	EvonetIteratorInvalidStatusException(const EvonetIteratorInvalidStatusException& other) throw() :
		BaseException(other)
	{
		strncpy(m_function, other.m_function, 256);
		m_function[255] = '\0';
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
	EvonetIteratorInvalidStatusException& operator=(const EvonetIteratorInvalidStatusException& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_function, other.m_function, 256);
		m_function[255] = '\0';
		strncpy(m_reason, other.m_reason, 256);
		m_reason[255] = '\0';
		strncpy(m_errorMessage, other.m_errorMessage, 1024);
		m_errorMessage[1023] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~EvonetIteratorInvalidStatusException() throw()
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
	 * \brief Returns the name of the function where the error occurred
	 *
	 * \return the name of the function where the error occurred
	 */
	const char *function() const throw()
	{
		return m_function;
	}

	/**
	 * \brief Returns the string explaining why the object is not in a valid
	 *        status
	 *
	 * \return the string explaining why the object is not in a valid status
	 */
	const char *reason() const throw()
	{
		return m_reason;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(EvonetIteratorInvalidStatusException)

private:
	/**
	 * \brief The name of the function where the error occurred
	 */
	char m_function[256];

	/**
	 * \brief The string explaining why the object is not in a valid status
	 */
	char m_reason[256];

	/**
	 * \brief The message describing the error
	 */
	char m_errorMessage[1024];
};

/*! \brief This class iterate over the neurons of a Evonet neural network
 *
 *  The blocks are identified by specifing the start and the end indexes
 *  and to which layer are related to. The indexes of blocks must be consecutive
 *
 * \ingroup experiments_utils
 */
class FARSA_EXPERIMENTS_API EvonetIterator : public AbstractControllerInputIterator, public AbstractControllerOutputIterator
{
public:
	/**
	 * \brief The possible layers on which the blocks can be defined
	 */
	enum Layer {
		InputLayer,
		HiddenLayer,
		OutputLayer
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param evonet the network on which we iterate
	 */
	EvonetIterator(Evonet* evonet);

	/**
	 * \brief Destructor
	 */
	virtual ~EvonetIterator();

	/**
	 * \brief Returns the Evonet on which we iterate
	 *
	 * \return the Evonet on which we iterate
	 */
	Evonet* getEvonet();

	/**
	 * \brief Defines a block
	 *
	 * \param layer is the layer on which the neurons lay
	 * \param startIndex is the index of the neuron from which this block
	 *                   starts
	 * \param size is the number of neurons for this block
	 * \return the index of the newly defined block
	 */
	int defineBlock(Layer layer, int startIndex, int size);

	/**
	 * \brief Sets the current block of inputs/outputs to iterate
	 *
	 * This methosd must be called before starting to set inputs/get outputs
	 * of the controller. When called, the first input/output of the block
	 * is selected, call next() to move to the subsequent one. This method
	 * throws an exception if the index does not exist
	 * \param index the index of the block to select
	 */
	virtual void setCurrentBlock(int index);

	/**
	 * \brief Increments the current input/output of the controller
	 *
	 * \return false when the current block has no more inputs/outputs, true
	 *         otherwise
	 */
	virtual bool next();

	/**
	 * \brief Sets the properties of the current input/output
	 *
	 * This should be used by controller inputs and outputs when the
	 * iterator changes (i.e. in the iteratorChanged() function)
	 * \param label is the label of the input/output
	 * \param minValue is the minimum allowed value
	 * \param maxValue is the maximum allowed value
	 * \param color the color of the input/output (used by guis)
	 */
	virtual void setProperties(QString label, real minValue, real maxValue, QColor color);

	/**
	 * \brief Returns the label of the current input/output
	 *
	 * \return the label of the input/output
	 */
	virtual QString label() const;

	/**
	 * \brief Returns the minimum allowed value of the current input/output
	 *
	 * \return the minimum allowed value of the input/output
	 */
	virtual real minValue() const;

	/**
	 * \brief Returns the maximum allowed value of the current input/output
	 *
	 * \return the maximum allowed value of the input/output
	 */
	virtual real maxValue() const;

	/**
	 * \brief Returns the color of the current input/output
	 *
	 * \return the color of the input/output
	 */
	virtual QColor color() const;

	/**
	 * \brief Sets the value of the current input
	 *
	 * \param value the value of the current input
	 */
	virtual void setInput(real value);

	/**
	 * \brief Gets the value of the current output
	 *
	 * \return the value of the current output
	 */
	virtual real getOutput() const;

private:
	// Checks the user is not attempting to do something nasty (e.g. access
	// values outside range). This is called by setInput(), getOutput() and
	// setGraphicProperties() to check everything is ok. If something goes
	// wrong an exception is thrown. funcName is the name of the calling
	// function (just to write a more informational message)
	void checkCurrentStatus(const QString& funcName) const;

	int layerIndexToLinearIndex(int index, Layer layer) const;

	struct BlockInfo
	{
		Layer layer;
		int startIndex;
		int endIndex;
	};

	Evonet* const m_evonet;
	QVector<BlockInfo> m_blocks;
	BlockInfo* m_curBlock;
	int m_curIndex;
};

} // end namespace farsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif
