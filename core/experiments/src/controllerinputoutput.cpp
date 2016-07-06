/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#include "controllerinputoutput.h"
#include "evonet.h"
#include "configurationhelper.h"
#include "logger.h"
#include "randomgenerator.h"

namespace farsa {

AbstractControllerInput::AbstractControllerInput(ConfigurationManager& params)
	: Component(params)
	, m_blockIndex(0)
	, m_it(NULL)
{
}

AbstractControllerInput::~AbstractControllerInput()
{
}

void AbstractControllerInput::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The abstract interface for controller inputs");
}

void AbstractControllerInput::setBlockIndex(int index)
{
	const int oldIndex = m_blockIndex;

	m_blockIndex = index;

	blockIndexChanged(oldIndex);
}

void AbstractControllerInput::setIterator(AbstractControllerInputIterator* it)
{
	AbstractControllerInputIterator* const oldIt = m_it;

	m_it = it;

	m_it->setCurrentBlock(m_blockIndex);

	iteratorChanged(oldIt);
}

void AbstractControllerInput::update()
{
	m_it->setCurrentBlock(m_blockIndex);

	updateCalled();
}

void AbstractControllerInput::blockIndexChanged(int)
{
}

AbstractControllerOutput::AbstractControllerOutput(ConfigurationManager& params)
	: Component(params)
	, m_blockIndex(0)
	, m_it(NULL)
{
}

AbstractControllerOutput::~AbstractControllerOutput()
{
}

void AbstractControllerOutput::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The abstract interface for controller outputs");
}

void AbstractControllerOutput::setBlockIndex(int index)
{
	const int oldIndex = m_blockIndex;

	m_blockIndex = index;

	blockIndexChanged(oldIndex);
}

void AbstractControllerOutput::setIterator(AbstractControllerOutputIterator* it)
{
	AbstractControllerOutputIterator* const oldIt = m_it;

	m_it = it;

	m_it->setCurrentBlock(m_blockIndex);

	iteratorChanged(oldIt);
}

void AbstractControllerOutput::update()
{
	m_it->setCurrentBlock(m_blockIndex);

	updateCalled();
}

void AbstractControllerOutput::blockIndexChanged(int)
{
}

} // end namespace farsa
