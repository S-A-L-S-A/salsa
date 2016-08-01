/********************************************************************************
 *  SALSA Experimentes Library                                                  *
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

#ifndef CONTROLLERINPUTOUTPUT_H
#define CONTROLLERINPUTOUTPUT_H

#include "experimentsconfig.h"
#include "component.h"
#include "baseexception.h"
#include "mathutils.h"
#include "controlleriterator.h"
#include <QString>
#include <QColor>
#include <QMap>

namespace salsa {

/**
 * \brief The base class for sensors and other objects that can set the input of
 *        a controller
 *
 * See the description of the Controller class for more information
 */
class SALSA_EXPERIMENTS_API AbstractControllerInput : public Component
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	AbstractControllerInput(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~AbstractControllerInput();

	/**
	 * \brief Add to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * It is mandatory in all subclasses where new parameters are defined to
	 * also implement this method
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns the number of inputs set by this iteratable
	 *
	 * \return the number of inputs set by this iteratable
	 * \note The value returned by this function must stay the same for the
	 *       whole lifetime of the object
	 */
	virtual int size() const = 0;

	/**
	 * \brief Sets the index of the block which we fill
	 *
	 * \param index the index of the block which we fill
	 */
	void setBlockIndex(int index);

	/**
	 * \brief Sets the iterator to use to fill the controller input block
	 *
	 * Call this only after the block index has been set by setBlockIndex()
	 * \param it the iterator to use
	 */
	void setIterator(AbstractControllerInputIterator* it);

	/**
	 * \brief Performs an update setting the inputs of the controller block
	 *
	 * This calls the setCurrentBlock() function of the iterator and then
	 * calls the virtual update() function passing the iterator to use
	 */
	void update();

protected:
	/**
	 * \brief Returns the index of the block we have to fill
	 *
	 * You should not need to call this function, AbstractControllerInput
	 * takes care of calling setCurrentBlock before updateCalled() is
	 * executed
	 * \return the index of the block we have to fill
	 */
	int blockIndex() const
	{
		return m_blockIndex;
	}

	/**
	 * \brief Returns the iterator
	 *
	 * Returns the iterator to use to fill the controller inputs
	 * \return the iterator
	 */
	AbstractControllerInputIterator* it()
	{
		return m_it;
	}

	/**
	 * \brief Returns the iterator (const version)
	 *
	 * Returns the iterator to use to fill the controller inputs
	 * \return a const pointer to the iterator
	 */
	const AbstractControllerInputIterator* it() const
	{
		return m_it;
	}

	/**
	 * \brief The function called when the index of the block to fill
	 *        changes
	 *
	 * The default implementation does nothing. You should almost never need
	 * to implement this function
	 * \param oldIndex the previous index. The new index is accessible using
	 *                 blockIndex()
	 */
	virtual void blockIndexChanged(int oldIndex);

	/**
	 * \brief The function called when the iterator is set
	 *
	 * Use this function to set the properties of input units, calling
	 * setProperties(). Do this even if oldIt == it(). The setCurrentBlock()
	 * function of the iterator must not be called (it is called by
	 * setIterator())
	 * \param oldIt the previous iterator. The new one is accessible using
	 *              it()
	 */
	virtual void iteratorChanged(AbstractControllerInputIterator* oldIt) = 0;

	/**
	 * \brief This function is called by update() and should be implemented
	 *        to actually set the inputs
	 *
	 * Use the iterator returned by the it() function to set inputs. The
	 * setCurrentBlock() function of the iterator must not be called (it is
	 * called by update())
	 */
	virtual void updateCalled() = 0;

private:
	int m_blockIndex;
	AbstractControllerInputIterator* m_it;
};

/**
 * \brief The base class for motors and other objects that read the outputs of
 *        a controller
 *
 * See the description of the Controller class for more information
 */
class SALSA_EXPERIMENTS_TEMPLATE AbstractControllerOutput : public Component
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	AbstractControllerOutput(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~AbstractControllerOutput();

	/**
	 * \brief Add to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * It is mandatory in all subclasses where new parameters are defined to
	 * also implement this method
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns the number of outputs read by this iteratable
	 *
	 * \return the number of outputs read by this iteratable
	 * \note The value returned by this function must stay the same for the
	 *       whole lifetime of the object
	 */
	virtual int size() const = 0;

	/**
	 * \brief Sets the index of the block which we read
	 *
	 * \param index the index of the block which we read
	 */
	void setBlockIndex(int index);

	/**
	 * \brief Sets the iterator to use to read the controller output block
	 *
	 * Call this only after the block index has been set by setBlockIndex()
	 * \param it the iterator to use
	 */
	void setIterator(AbstractControllerOutputIterator* it);

	/**
	 * \brief Performs an update reading the outputs of the controller block
	 *
	 * This calls the setCurrentBlock() function of the iterator and then
	 * calls the virtual update() function passing the iterator to use
	 */
	void update();

protected:
	/**
	 * \brief Returns the index of the block we have to fill
	 *
	 * You should not need to call this function, AbstractControllerOutput
	 * takes care of calling setCurrentBlock before updateCalled() is
	 * executed
	 * \return the index of the block we have to fill
	 */
	int blockIndex() const
	{
		return m_blockIndex;
	}

	/**
	 * \brief Returns the iterator
	 *
	 * Returns the iterator to use to fill the controller outputs
	 * \return the iterator
	 */
	AbstractControllerOutputIterator* it()
	{
		return m_it;
	}

	/**
	 * \brief Returns the iterator (const version)
	 *
	 * Returns the iterator to use to fill the controller outputs
	 * \return a const pointer to the iterator
	 */
	const AbstractControllerOutputIterator* it() const
	{
		return m_it;
	}

	/**
	 * \brief The function called when the index of the block to fill
	 *        changes
	 *
	 * The default implementation does nothing. You should almost never need
	 * to implement this function
	 * \param oldIndex the previous index. The new index is accessible using
	 *                 blockIndex()
	 */
	virtual void blockIndexChanged(int oldIndex);

	/**
	 * \brief The function called when the iterator is set
	 *
	 * Use this function to set the properties of output units, calling
	 * setProperties(). Do this even if oldIt == it(). The setCurrentBlock()
	 * function of the iterator must not be called (it is called by
	 * setIterator())
	 * \param oldIt the previous iterator. The new one is accessible using
	 *              it()
	 */
	virtual void iteratorChanged(AbstractControllerOutputIterator* oldIt) = 0;

	/**
	 * \brief This function is called by update() and should be implemented
	 *        to actually set the outputs
	 *
	 * Use the iterator returned by the it() function to set outptus. The
	 * setCurrentBlock() function of the iterator must not be called (it is
	 * called by the update())
	 */
	virtual void updateCalled() = 0;

private:
	int m_blockIndex;
	AbstractControllerOutputIterator* m_it;
};

} // end namespace salsa

#endif
