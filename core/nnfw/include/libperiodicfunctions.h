/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2014 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef LIBPERIODICFUNCTIONS_H
#define LIBPERIODICFUNCTIONS_H

#include "nnfwconfig.h"

#include "outputfunction.h"

namespace salsa {

/*! \brief PeriodicFunction
 *
 */
class SALSA_NNFW_API PeriodicFunction : public OutputFunction {
public:
	/*! Construct */
	PeriodicFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~PeriodicFunction() { /* Nothing to do */ };
	/*! Implement the Periodic function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs ) = 0;
	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 */
	virtual void configure();
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! Set the coefficients of the PeriodicFunction
	 */
	void setCoefficients( double phase, double span, double amplitude );
	/*! return the phase of the periodic function */
	double getPhase();
	/*! return the span of the periodic function */
	double getSpan();
	/*! return the amplitude of the periodic function */
	double getAmplitude();
protected:
	double phase;
	double span;
	double amplitude;
};

/*! \brief SawtoothFunction
 *
 */
class SALSA_NNFW_API SawtoothFunction : public PeriodicFunction {
public:
	/*! Construct */
	SawtoothFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~SawtoothFunction() { /* Nothing to do */ };
	/*! Implement the Sawtooth function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
};

/*! \brief TriangleFunction
 *
 */
class SALSA_NNFW_API TriangleFunction : public PeriodicFunction {
public:
	/*! Construct */
	TriangleFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~TriangleFunction() { /* Nothing to do */ };
	/*! Implement the Triangle function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
};

/*! \brief SinFunction
 *
 */
class SALSA_NNFW_API SinFunction : public PeriodicFunction {
public:
	/*! Construct */
	SinFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~SinFunction() { /* Nothing to do */ };
	/*! Return the frequency of the sinusoidal wave
	 *  \param frequency is equal to 2*pi_greco/span
	 */
	double getFrequency();
	/*! Implement the Sin function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
};

/*! \brief PseudoGaussFunction
 *
 */
class SALSA_NNFW_API PseudoGaussFunction : public PeriodicFunction {
public:
	/*! Construct */
	PseudoGaussFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~PseudoGaussFunction() { /* Nothing to do */ };
	/*! Implement the Triangle function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
};

}

#endif

