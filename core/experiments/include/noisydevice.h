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

#ifndef NOISYDEVICE_H
#define NOISYDEVICE_H

#include "experimentsconfig.h"
#include "controllerinputoutput.h"
#include "component.h"
#include "baseexception.h"
#include "mathutils.h"
#include "sensors.h"
#include "motors.h"
#include <memory>

namespace farsa {

/**
 * \brief The base class for inputs and outputs that add noise
 *
 * This is the base class for controller inputs and outputs that add noise. You
 * can add either uniform noise or gaussian noise in a range. This is not a
 * component, however this class reads some parameters from the
 * ConfigurationManager object passed to the constructor and has a describe
 * method to describe those parameters. The parameters are:
 * 	- noiseType: set to either NoNoise, Uniform or Gaussian to select the
 * 	             type of distribution of noise (respectively no noise,
 * 	             uniform noise in a range or gaussian noise)
 * 	- noiseRange: the range of noise. For uniform noise this is the actual
 * 	              range, (the distribution has zero mean and goes from
 * 	              -noiseRange/2 to noiseRange/2). For gaussian noise, this
 * 	              is four times the standard deviation (the distribution has
 * 	              zero mean and noiseRange/4 standard deviation: this means
 * 	              that about 95% of the values taken from the distribution
 * 	              will be between -noiseRange/2 and noiseRange/2)
 *
 * \note In subclasses, use Component as the first subclass, then this one
 */
class FARSA_EXPERIMENTS_API NoisyDevice
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 * \param prefix the path to parameters
	 */
	NoisyDevice(ConfigurationManager& params, QString prefix);

	/**
	 * \brief Destructor
	 */
	virtual ~NoisyDevice();

	/**
	 * \brief Describes of all parameters and subgroups
	 *
	 * \param d the type descriptor
	 */
	static void describe(RegisteredComponentDescriptor& d);

protected:
	/**
	 * \brief Adds noise to the value
	 *
	 * This function applies the noise as configured by configuration
	 * parameters. If minValue > maxValue no check on boundaries is
	 * performed (i.e. we don't restrict the value obtained after applying
	 * noise)
	 * \param v the value to which noise should be added
	 * \param minValue the minimum allowed value
	 * \param maxValue the maximum allowed value
	 * \return the value with added noise
	 */
	real applyNoise(real v, real minValue, real maxValue) const;

private:
	// The selected type of noise
	enum {
		NoNoise, /**< No noise */
		Uniform, /**< Uniform noise */
		Gaussian /**< Gaussian noise */
	} m_noiseType;

	// See class description of the meaning of this value depending on the
	// type of noise
	real m_noiseRange;

	// This is (noiseRange/2.0) for uniform noise and (noiseRange/4.0)^2 for
	// gaussian noise (i.e. the variance). This is computed in the
	// constructor to speed up calculations
	real m_noiseParameter;
};

/**
 * \brief An input block that adds noise to input data
 *
 * This component is a controller input that takes input from another controller
 * input (in the subgroup Input) and adds noise
 */
class FARSA_EXPERIMENTS_API NoisyInput : public AbstractControllerInput, public NoisyDevice, protected AbstractControllerInputIterator
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	NoisyInput(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~NoisyInput();

	/**
	 * \brief Describes of all parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns the number of inputs
	 *
	 * This is the same as the number of inputs of the input from which
	 * we take data
	 * \return the number of inputs
	 */
	virtual int size() const;

protected:
	// This does nothing (not needed because we only have one controller
	// input from which we take values)
	virtual void setCurrentBlock(int index);

	// Functions re-implemented from AbstractControllerInputIterator
	virtual bool next();
	virtual void setProperties(QString label, real minValue, real maxValue, QColor color);
	virtual QString label() const;
	virtual real minValue() const;
	virtual real maxValue() const;
	virtual QColor color() const;
	virtual void setInput(real value);

private:
	// From AbstractControllerInput
	virtual void iteratorChanged(AbstractControllerInputIterator* oldIt);
	virtual void updateCalled();

	// Adds noise to the value. The value is constrained to stay between the
	// minValue and the maxValue (the values for the current unit is used).
	// Internally uses NoisyDevice::applyNoise
	real applyNoise(real v) const;

	// The block to which noise is added
	std::auto_ptr<AbstractControllerInput> m_input;
};

/**
 * \brief An output block that adds noise to output data
 *
 * This component is a controller output that takes output from the controller
 * and feeds it to another controller output (in the subgroup Output)
 */
class FARSA_EXPERIMENTS_API NoisyOutput : public AbstractControllerOutput, public NoisyDevice, protected AbstractControllerOutputIterator
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	NoisyOutput(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~NoisyOutput();

	/**
	 * \brief Describes of all parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns the number of outputs
	 *
	 * This is the same as the number of outputs of the output which we
	 * feed with data
	 * \return the number of outputs
	 */
	virtual int size() const;

protected:
	// This does nothing (not needed because we only have one controller
	// input from which we take values)
	virtual void setCurrentBlock(int index);

	// Functions re-implemented from AbstractControllerOutputIterator
	virtual bool next();
	virtual void setProperties(QString label, real minValue, real maxValue, QColor color);
	virtual QString label() const;
	virtual real minValue() const;
	virtual real maxValue() const;
	virtual QColor color() const;
	virtual real getOutput() const;

private:
	// From AbstractControllerOutput
	virtual void iteratorChanged(AbstractControllerOutputIterator* oldIt);
	virtual void updateCalled();

	// Adds noise to the value. The value is constrained to stay between the
	// minValue and the maxValue (the values for the current unit is used).
	// Internally uses NoisyDevice::applyNoise
	real applyNoise(real v) const;

	// The block to feed with noisy data
	std::auto_ptr<AbstractControllerOutput> m_output;
};

} // end namespace farsa

#endif
