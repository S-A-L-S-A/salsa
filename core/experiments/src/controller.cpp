/********************************************************************************
 *  SALSA Experiments Library                                                   *
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

#include "controller.h"
#include "evonet.h"
#include "configurationhelper.h"
#include "logger.h"
#include "randomgenerator.h"
#include "configurationmanager.h"

namespace salsa {

Controller::Controller(ConfigurationManager& params)
	: Component(params)
	, m_inputIterator()
	, m_outputIterator()
	, m_inputsList(NULL)
	, m_outputsList(NULL)
{
}

Controller::~Controller()
{
	// Checking that the input and output iterators are not the same object. If it is so,
	// releasing one auto_ptr to prevent a double-free
	if (static_cast<AbstractControllerIterator*>(m_inputIterator.get()) == static_cast<AbstractControllerIterator*>(m_outputIterator.get())) {
		m_outputIterator.release();
	}
}

void Controller::configure()
{
	m_inputsList = configurationManager().getComponentFromParameter<ControllerInputsList>(confPath() + "inputsList", false);
	m_outputsList = configurationManager().getComponentFromParameter<ControllerOutputsList>(confPath() + "outputsList", false);
}

void Controller::postConfigureInitialization()
{
	// Generating iterators
	QPair<AbstractControllerInputIterator*, AbstractControllerOutputIterator*> iterators = createIterators();
	m_inputIterator.reset(iterators.first);
	m_outputIterator.reset(iterators.second);

	// Setting block index and iterator for inputs
	for (int i = 0; i < m_inputsList->numControllerInputs(); ++i) {
		AbstractControllerInput* const ci = m_inputsList->getControllerInput(i);

		const int controllerIndex = getIndexForControllerInput(ci);
		ci->setBlockIndex(controllerIndex);

		ci->setIterator(m_inputIterator.get());
	}

	// Setting block index and iterator for outputs
	for (int i = 0; i < m_outputsList->numControllerOutputs(); ++i) {
		AbstractControllerOutput* const co = m_outputsList->getControllerOutput(i);

		const int controllerIndex = getIndexForControllerOutput(co);
		co->setBlockIndex(controllerIndex);

		co->setIterator(m_outputIterator.get());
	}
}

void Controller::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The abstract interface for controllers");

	d.describeComponent("inputsList").props(ParamIsMandatory).componentType("ControllerInputsList").help("The component with the list of inputs", "Set this to the group of the component that contains the list of inputs (sensors).");
	d.describeComponent("outputsList").props(ParamIsMandatory).componentType("ControllerOutputsList").help("The component with the list of outputs", "Set this to the group of the component that contains the list of outputs (motors).");
}

AbstractControllerInputIterator* Controller::inputIterator()
{
	return m_inputIterator.get();
}

AbstractControllerOutputIterator* Controller::outputIterator()
{
	return m_outputIterator.get();
}

const AbstractControllerInputIterator* Controller::inputIterator() const
{
	return m_inputIterator.get();
}

const AbstractControllerOutputIterator* Controller::outputIterator() const
{
	return m_outputIterator.get();
}

const ControllerInputsList* Controller::controllerInputsList() const
{
	return m_inputsList;
}

const ControllerOutputsList* Controller::controllerOutputsList() const
{
	return m_outputsList;
}

} // end namespace salsa
