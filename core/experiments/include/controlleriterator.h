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

#ifndef CONTROLLERITERATOR_H
#define CONTROLLERITERATOR_H

#include "experimentsconfig.h"
#include "component.h"
#include "baseexception.h"
#include "mathutils.h"
#include <QString>
#include <QColor>
#include <QMap>

namespace farsa {

/**
 * \brief The abstract interface for iterators interfacing the controller with
 *        inputs/outputs
 *
 * This contains function common to both input and output iterators. This way it
 * is possible to implement both input and output iterators for a controller in
 * the same class or in separate classes. See the description of the Controller
 * class for more information
 */
class FARSA_EXPERIMENTS_TEMPLATE AbstractControllerIterator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~AbstractControllerIterator()
	{
	}

	/**
	 * \brief Sets the current block of inputs/outputs to iterate
	 *
	 * This methosd must be called before starting to set inputs/get outputs
	 * of the controller. When called, the first input/output of the block
	 * is selected, call next() to move to the subsequent one. This method
	 * should throw an exception if the index does not exist
	 * \param index the index of the block to select
	 */
	virtual void setCurrentBlock(int index) = 0;

	/**
	 * \brief Increments the current input/output of the controller
	 *
	 * \return false when the current block has no more inputs/outputs, true
	 *         otherwise
	 */
	virtual bool next() = 0;

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
	virtual void setProperties(QString label, real minValue, real maxValue, QColor color) = 0;

	/**
	 * \brief Returns the label of the current input/output
	 *
	 * \return the label of the input/output
	 */
	virtual QString label() const = 0;

	/**
	 * \brief Returns the minimum allowed value of the current input/output
	 *
	 * \return the minimum allowed value of the input/output
	 */
	virtual real minValue() const = 0;

	/**
	 * \brief Returns the maximum allowed value of the current input/output
	 *
	 * \return the maximum allowed value of the input/output
	 */
	virtual real maxValue() const = 0;

	/**
	 * \brief Returns the color of the current input/output
	 *
	 * \return the color of the input/output
	 */
	virtual QColor color() const = 0;
};

/**
 * \brief The abstract base class for controller input iterators
 *
 * This adds means to set the input of a block to AbstractControllerIterator. We
 * use virtual inheritance because we could have classes inheriting from both
 * this and AbstractControllerOutputIterator (both inherit from
 * AbstractControllerIterator).
 */
class FARSA_EXPERIMENTS_TEMPLATE AbstractControllerInputIterator : virtual public AbstractControllerIterator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~AbstractControllerInputIterator()
	{
	}

	/**
	 * \brief Sets the value of the current input
	 *
	 * \param value the value of the current input
	 */
	virtual void setInput(real value) = 0;
};

/**
 * \brief The abstract base class for controller output iterators
 *
 * This adds means to get the output of a block to AbstractControllerIterator. We
 * use virtual inheritance because we could have classes inheriting from both
 * this and AbstractControllerInputIterator (both inherit from
 * AbstractControllerIterator).
 */
class FARSA_EXPERIMENTS_TEMPLATE AbstractControllerOutputIterator : virtual public AbstractControllerIterator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~AbstractControllerOutputIterator()
	{
	}

	/**
	 * \brief Gets the value of the current output
	 *
	 * \return the value of the current output
	 */
	virtual real getOutput() const = 0;
};

} // end namespace farsa

#endif
