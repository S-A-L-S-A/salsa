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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "experimentsconfig.h"
#include "component.h"
#include "baseexception.h"
#include "mathutils.h"
#include "controlleriterator.h"
#include "controllerinputoutput.h"
#include <QString>
#include <QColor>
#include <QMap>
#include <QPair>
#include <memory>

namespace salsa {

/**
 * \brief The interface to obtain the list of inputs
 *
 * The controller uses the list to set the iterator and the block index in the
 * inputs. See the description of Controller for more information
 */
class SALSA_EXPERIMENTS_TEMPLATE ControllerInputsList
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ControllerInputsList()
	{
	}

	/**
	 * \brief Returns the number of controller inputs
	 *
	 * \return the number of controller inputs
	 */
	virtual int numControllerInputs() const = 0;

	/**
	 * \brief Returns the i-th input
	 *
	 * \param i the input to return (between 0 and numInputBlocks() - 1)
	 * \return the input
	 */
	virtual AbstractControllerInput* getControllerInput(int i) const = 0;
};

/**
 * \brief The interface to obtain the list of outputs
 *
 * The controller uses the list to set the iterator and the block index in the
 * outputs. See the description of Controller for more information
 */
class SALSA_EXPERIMENTS_TEMPLATE ControllerOutputsList
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ControllerOutputsList()
	{
	}

	/**
	 * \brief Returns the number of controller outputs
	 *
	 * \return the number of controller outputs
	 */
	virtual int numControllerOutputs() const = 0;

	/**
	 * \brief Returns the i-th output
	 *
	 * \param i the output to return (between 0 and numOutputBlocks() - 1)
	 * \return the output
	 */
	virtual AbstractControllerOutput* getControllerOutput(int i) const = 0;
};

/**
 * \brief The base class for controllers
 *
 * This is the base class for robot controllers. A Controller receives inputs
 * from one or more ControllerInput components and produces outputs that are
 * used by one or more ControllerOutput components. These do not interact
 * directly with the controller, they use ControllerIterators. When a controller
 * is created, it must also create input and output iterators (returned
 * respectively by the inputIterator() and outputIterator() functions). These
 * must be created and returned by the createIterators() method and, once
 * created, their lieftime is managed by this class (i.e. do not delete them!).
 * You also do not need to keep a reference to them as they will be accessible
 * via the inputIterator() and outputIterator() methods.
 *
 * This class also performs the association between controller inputs and
 * outputs and the iterators. For this to work, you also have to implement two
 * more pure virtual functions, getIndexForInput() and getIndexForOutput(). They
 * take a pointer to an input/output and should return an index which is then
 * set and the block index of the input/output (via the setBlockIndex()
 * function of inputs and outputs). The index you return should also be
 * understood by the iterator as the index of that block. To better understand
 * what is going on, here is how the iterator, input and controller interact
 * (for outputs the machanisms is similar):
 * 	- when built, the controller creates its input iterators (calling the
 * 	  Controller::createIterators() function) and then assigns a unique
 * 	  block index to every input (using the Controller::getIndexForInput()
 * 	  function to obtain the index). It also sets the iterator for the input
 * 	  via the AbstractControllerInput::setIterator() function;
 * 	- when the input is updated (via a call to
 * 	  AbstractControllerInput::update()), it performs the following actions:
 * 		- sets the current block of the iterator to which it is
 * 		  associated to its own index (using the
 * 		  AbstractControllerIterator::setCurrentBlock() function);
 * 		- fills the iterator with values using the function
 * 		  AbstractControllerInputIterator::setInput(), moving the
 * 		  iterator input index forward calling
 * 		  AbstractControllerIterator::next().
 * For this to work it is necessary that the iterator the controller uses knows
 * how to handle different input blocks when a the current block index is set.
 * This mechanism must be implemented in subclasses because it strongly depends
 * on the particular controller and iterators.
 *
 * The mechanism described above needs two components implementing the
 * ControllerInputsList and ControllerOutputsList interfaces. These interfaces
 * are used to get the list of inputs and outputs so that we can configure them
 * with the correct iterator and block index. The components can be set
 * using the inputsList and outputsList configuration parameters (the parameters
 * should be set to the name of the groups associated to the components). These
 * components are only used during the creation phase (in the
 * postConfigureInitialization function) and are not deleted by this class in
 * any case. For an example of a Component implementing both the interfaces, see
 * EmbodiedAgent.
 *
 * The final method that this class must implement is update(), which should
 * compute the new output given the inputs that have been set.
 *
 * \note You can safely use one single object as both input and output iterator,
 *       this class checks whether they are actually the same object before
 *       deleting them (so to avoid double-frees, this is possible because of
 *       the virtual inheritance in AbstractControllerInputIterator and
 *       AbstractControllerOutputIterator)
 */
class SALSA_EXPERIMENTS_API Controller : public Component
{
public:
	/**
	 * \brief The function returning whether instances of this class
	 *        configure themselves in the constructor or not
	 *
	 * \return false, this class configures in configure()
	 */
	static bool configuresInConstructor()
	{
		return false;
	}

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	Controller(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~Controller();

	/**
	 * \brief Configures the object using the ConfigurationManager
	 *
	 */
	virtual void configure();

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 *
	 * Here we associate inputs/outputs and iterators
	 */
	virtual void postConfigureInitialization();

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
	 * \brief Updates the controller
	 *
	 * This read the inputs and produces the outputs
	 */
	virtual void update() = 0;

	/**
	 * \brief Resets the internal status of the controller
	 */
	virtual void resetControllerStatus() = 0;

protected:
	/**
	 * \brief Creates and returns an input iterator and an output iterator
	 *
	 * See class description for more information
	 * \return the input and output iterators. These objects, after
	 *         creation, are managed by Controller, do not delete them!
	 */
	virtual QPair<AbstractControllerInputIterator*, AbstractControllerOutputIterator*> createIterators() = 0;

	/**
	 * \brief Returns the input iterator
	 *
	 * This returns the object that has been created by createIterators()
	 * \return the input iterator
	 */
	AbstractControllerInputIterator* inputIterator();

	/**
	 * \brief Returns the output iterator
	 *
	 * This returns the object that has been created by createIterators()
	 * \return the output iterator
	 */
	AbstractControllerOutputIterator* outputIterator();

	/**
	 * \brief Returns the input iterator (const version)
	 *
	 * This returns the object that has been created by createIterators()
	 * \return the input iterator
	 */
	const AbstractControllerInputIterator* inputIterator() const;

	/**
	 * \brief Returns the output iterator (const version)
	 *
	 * This returns the object that has been created by createIterators()
	 * \return the output iterator
	 */
	const AbstractControllerOutputIterator* outputIterator() const;

	/**
	 * \brief Returns the block index for the given input
	 *
	 * \param input the input for which the index is requested
	 * \return the block index for the given input
	 */
	virtual int getIndexForControllerInput(AbstractControllerInput* input) = 0;

	/**
	 * \brief Returns the block index for the given output
	 *
	 * \param output the output for which the index is requested
	 * \return the block index for the given output
	 */
	virtual int getIndexForControllerOutput(AbstractControllerOutput* output) = 0;

	/**
	 * \brief Returns the object with the list of inputs
	 *
	 * \return the object with the list of inputs
	 */
	const ControllerInputsList* controllerInputsList() const;

	/**
	 * \brief Returns the object with the list of outputs
	 *
	 * \return the object with the list of outputs
	 */
	const ControllerOutputsList* controllerOutputsList() const;

private:
	std::auto_ptr<AbstractControllerInputIterator> m_inputIterator;
	std::auto_ptr<AbstractControllerOutputIterator> m_outputIterator;
	ControllerInputsList* m_inputsList;
	ControllerOutputsList* m_outputsList;
};

} // end namespace salsa

#endif
