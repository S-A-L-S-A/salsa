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

#include "evonetiterator.h"
#include "evonet.h"
#include "logger.h"
#include "randomgenerator.h"

namespace salsa {

EvonetIterator::EvonetIterator(Evonet* evonet)
	: AbstractControllerIterator()
	, AbstractControllerInputIterator()
	, AbstractControllerOutputIterator()
	, m_evonet(evonet)
	, m_blocks()
	, m_curBlock(NULL)
	, m_curIndex(0)
{
}

EvonetIterator::~EvonetIterator()
{
}

Evonet* EvonetIterator::getEvonet()
{
	return m_evonet;
}

int EvonetIterator::defineBlock(Layer layer, int startIndex, int size)
{
	BlockInfo info;

	info.layer = layer;
	info.startIndex = startIndex;
	info.endIndex = startIndex + size;

	m_blocks.append(info);

	return m_blocks.size() - 1;
}

void EvonetIterator::setCurrentBlock(int index)
{
	if (index >= m_blocks.size()) {
		throw EvonetIteratorInvalidStatusException("setCurrentBlock", QString("EvonetIterator - the block index %1 does not exist").arg(index).toLatin1().data());
	}

	m_curBlock = &(m_blocks[index]);
	m_curIndex = m_curBlock->startIndex;
}

bool EvonetIterator::next()
{
	checkCurrentStatus("nextNeuron");

	m_curIndex++;

	if (m_curIndex >= m_curBlock->endIndex) {
		return false;
	} else {
		return true;
	}
}

void EvonetIterator::setProperties(QString label, real minValue, real maxValue, QColor color)
{
	checkCurrentStatus("setProperties");

	const int index = layerIndexToLinearIndex(m_curIndex, m_curBlock->layer);

	label.truncate(9);
	sprintf(m_evonet->neuronl[index], "%s", label.toLatin1().data());

	m_evonet->neuronrange[index][0] = minValue;

	m_evonet->neuronrange[index][1] = maxValue;

	m_evonet->neurondcolor[index] = color;

	m_evonet->updateNeuronMonitor = true;
}

QString EvonetIterator::label() const
{
	checkCurrentStatus("label");

	const int index = layerIndexToLinearIndex(m_curIndex, m_curBlock->layer);

	return QString(m_evonet->neuronl[index]);
}

real EvonetIterator::minValue() const
{
	checkCurrentStatus("minValue");

	const int index = layerIndexToLinearIndex(m_curIndex, m_curBlock->layer);

	return m_evonet->neuronrange[index][0];
}

real EvonetIterator::maxValue() const
{
	checkCurrentStatus("maxValue");

	const int index = layerIndexToLinearIndex(m_curIndex, m_curBlock->layer);

	return m_evonet->neuronrange[index][1];
}

QColor EvonetIterator::color() const
{
	checkCurrentStatus("color");

	const int index = layerIndexToLinearIndex(m_curIndex, m_curBlock->layer);

	return m_evonet->neurondcolor[index];
}

void EvonetIterator::setInput(real value)
{
	checkCurrentStatus("setInput");

	m_evonet->setInput(m_curIndex, value);
}

real EvonetIterator::getOutput() const
{
	checkCurrentStatus("getOutput");

	return m_evonet->getOutput(m_curIndex);
}

void EvonetIterator::checkCurrentStatus(const QString& funcName) const
{
	if (m_curBlock == NULL) {
		throw EvonetIteratorInvalidStatusException(funcName.toLatin1().data(), "you should call setCurrentBlock first");
	}
	if (m_curIndex >= m_curBlock->endIndex) {
		throw EvonetIteratorInvalidStatusException(funcName.toLatin1().data(), "attempt to access beyond the size of the current block");
	}
}

int EvonetIterator::layerIndexToLinearIndex(int index, Layer layer) const
{
	int linearIndex = index;
	if (layer == OutputLayer) {
		linearIndex += m_evonet->getNoInputs() + m_evonet->getNoHiddens();
	} else if (layer == HiddenLayer) {
		linearIndex += m_evonet->getNoInputs();
	}

	return linearIndex;
}

} // end namespace salsa
