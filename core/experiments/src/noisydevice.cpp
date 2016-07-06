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

#include "noisydevice.h"
#include "configurationhelper.h"
#include "logger.h"
#include "randomgenerator.h"
#include "configurationmanager.h"

#warning NEI NOISE DEVICE, AL MOMENTO USIAMO SENSOR/MOTOR COME TIPO PER IL SOTTOGRUPPO, SOSTITUIRE CON AbstractControllerInput/AbstractControllerOutput QUANDO CONFIGURATION MANAGER SUPPORTERÀ LE CLASSI BASE MULTIPLE

#warning NoisyDevice AL MOMENTO USA globalRNG PER IL RUMORE, BISOGNEREBBE USARE RNG PRIVATI (PENSARE A COME FARE)

namespace farsa {

NoisyDevice::NoisyDevice(ConfigurationManager& params, QString prefix)
	: m_noiseType(NoNoise)
	, m_noiseRange(0.0)
	, m_noiseParameter(0.0)
{
	QString noiseTypeStr = ConfigurationHelper::getEnum(params, prefix + "noiseType").toUpper();
	if (noiseTypeStr == "NONOISE") {
		m_noiseType = NoNoise;
	} else if (noiseTypeStr == "UNIFORM") {
		m_noiseType = Uniform;
	} else if (noiseTypeStr == "GAUSSIAN") {
		m_noiseType = Gaussian;
	}

	m_noiseRange = ConfigurationHelper::getReal(params, prefix + "noiseRange");

	// Computing the parameter used when applying noise. It depends on the type of noise to apply
	switch (m_noiseType) {
		case NoNoise:
			m_noiseParameter = 0.0;
			break;
		case Uniform:
			m_noiseParameter = m_noiseRange / 2.0;
			break;
		case Gaussian:
			m_noiseParameter = (m_noiseRange * m_noiseRange) / 16.0; // i.e. (noiseRange/4.0)^2
			break;
	}
}

NoisyDevice::~NoisyDevice()
{
}

void NoisyDevice::describe(RegisteredComponentDescriptor& d)
{
	d.describeEnum("noiseType").def("NoNoise").values(QStringList() << "NoNoise" << "Uniform" << "Gaussian").help("The type of noise to add to sensor readings", "This must be one of NoNoise, Uniform or Gaussian. Notice that not all sensors respect the value of this parameter (some may never add noise). See sensor description for more information");

	d.describeReal("noiseRange").def(0.0).limits(0.0, +Infinity).help("The range of noise", "For uniform noise this is the actual range, (the distribution has zero mean and goes from -noiseRange/2 to noiseRange/2). For gaussian noise, this is four times the standard deviation (the distribution has zero mean and noiseRange/4 standard deviation: this means that about 95% of the values taken from the distribution will be between -noiseRange/2 and noiseRange/2)");
}

real NoisyDevice::applyNoise(real v, real minValue, real maxValue) const
{
	// Adding noise
	switch (m_noiseType) {
		case NoNoise:
			// Nothing to do
			break;
		case Uniform:
			v += globalRNG->getDouble(-m_noiseParameter, m_noiseParameter);
			break;
		case Gaussian:
			v += globalRNG->getGaussian(m_noiseParameter, 0.0);
			break;
	}

	// Restricting v between minValue and maxValue if we have to
	if (minValue <= maxValue) {
		if (v < minValue) {
			v = minValue;
		} else if (v > maxValue) {
			v = maxValue;
		}
	}

	return v;
}

NoisyInput::NoisyInput(ConfigurationManager& params)
	: AbstractControllerInput(params)
	, NoisyDevice(params, confPath())
	, AbstractControllerInputIterator()
	, m_input(configurationManager().getComponentFromGroup<AbstractControllerInput>(confPath() + "Input"))
{
	m_input->setIterator(this);
	m_input->setBlockIndex(0);
}

NoisyInput::~NoisyInput()
{
}

void NoisyInput::describe(RegisteredComponentDescriptor& d)
{
	AbstractControllerInput::describe(d);

	d.help("A controller input that adds noise to another controller input");

	NoisyDevice::describe(d);

	d.describeSubgroup("Input").props(ParamIsMandatory).componentType("Sensor").help("The controller input to which noise is added", "Noise will be added to the input values produced by the controller input in this group, before values are sent to the controller");
}

int NoisyInput::size() const
{
	return m_input->size();
}

void NoisyInput::setCurrentBlock(int)
{
}

bool NoisyInput::next()
{
	return it()->next();
}

void NoisyInput::setProperties(QString label, real minValue, real maxValue, QColor color)
{
	it()->setProperties(label, minValue, maxValue, color);
}

QString NoisyInput::label() const
{
	return it()->label();
}

real NoisyInput::minValue() const
{
	return it()->minValue();
}

real NoisyInput::maxValue() const
{
	return it()->maxValue();
}

QColor NoisyInput::color() const
{
	return it()->color();
}

void NoisyInput::setInput(real value)
{
	it()->setInput(applyNoise(value));
}

void NoisyInput::iteratorChanged(AbstractControllerInputIterator*)
{
	// This is needed so that the properties will be set in the new iterator
	// that we received
	m_input->setIterator(this);
}

void NoisyInput::updateCalled()
{
	m_input->update();
}

real NoisyInput::applyNoise(real v) const
{
	return NoisyDevice::applyNoise(v, minValue(), maxValue());
}

NoisyOutput::NoisyOutput(ConfigurationManager& params)
	: AbstractControllerOutput(params)
	, NoisyDevice(params, confPath())
	, AbstractControllerOutputIterator()
	, m_output(configurationManager().getComponentFromGroup<AbstractControllerOutput>(confPath() + "Output"))
{
	m_output->setIterator(this);
	m_output->setBlockIndex(0);
}

NoisyOutput::~NoisyOutput()
{
}

void NoisyOutput::describe(RegisteredComponentDescriptor& d)
{
	AbstractControllerOutput::describe(d);

	d.help("A controller output that adds noise to another controller output");

	NoisyDevice::describe(d);

	d.describeSubgroup("Output").props(ParamIsMandatory).componentType("Motor").help("The controller output to which noise is added", "Noise will be added to the output values produced by the controller, before values are sent to the controller output in this group");
}

int NoisyOutput::size() const
{
	return m_output->size();
}

void NoisyOutput::setCurrentBlock(int)
{
}

bool NoisyOutput::next()
{
	return it()->next();
}

void NoisyOutput::setProperties(QString label, real minValue, real maxValue, QColor color)
{
	it()->setProperties(label, minValue, maxValue, color);
}

QString NoisyOutput::label() const
{
	return it()->label();
}

real NoisyOutput::minValue() const
{
	return it()->minValue();
}

real NoisyOutput::maxValue() const
{
	return it()->maxValue();
}

QColor NoisyOutput::color() const
{
	return it()->color();
}

real NoisyOutput::getOutput() const
{
	return applyNoise(it()->getOutput());
}

void NoisyOutput::iteratorChanged(AbstractControllerOutputIterator*)
{
	// This is needed so that the properties will be set in the new iterator
	// that we received
	m_output->setIterator(this);
}

void NoisyOutput::updateCalled()
{
	m_output->update();
}

real NoisyOutput::applyNoise(real v) const
{
	return NoisyDevice::applyNoise(v, minValue(), maxValue());
}

} // end namespace farsa
