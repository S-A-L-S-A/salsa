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

#ifndef LIBRADIALFUNCTIONS_H
#define LIBRADIALFUNCTIONS_H

#include "nnfwconfig.h"

#include "outputfunction.h"

namespace salsa {

/*! \brief GaussFunction
 *
 */
class SALSA_NNFW_API GaussFunction : public OutputFunction {
public:
	/*! Construct */
	GaussFunction( ConfigurationManager& params, QString prefix, Component* parent=nullptr );
	/*! Destructor */
	virtual ~GaussFunction() { /* Nothing to do */ };
	/*! Implement the Gaussian function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! derivate of Gauss function */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
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
	/*! set the coefficients for this function */
	void setCoefficients( double max, double variance, double centre );
	/*! Return the variance */
	double getVariance();
	/*! return the centre */
	double getCentre();
	/*! return the max value */
	double getMax();
private:
	/*! Centre of GaussFunction */
	double centre;
	/*! Maximum value of GaussFunction */
	double max;
	// variance
	double variancev;
	// minus squared-variance
	double msqrvar;
};

}

#endif

