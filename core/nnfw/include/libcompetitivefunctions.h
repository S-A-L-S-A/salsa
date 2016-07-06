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

#ifndef LIBCOMPETITIVEFUNCTIONS_H
#define LIBCOMPETITIVEFUNCTIONS_H

#include "nnfwconfig.h"

#include "outputfunction.h"

namespace farsa {

/*! \brief WinnerTakeAllFunction
 *
 */
class FARSA_NNFW_API WinnerTakeAllFunction : public OutputFunction {
public:
	/*! Construct */
	WinnerTakeAllFunction( ConfigurationManager& params, QString prefix, Component* parent = NULL );
	/*! Destructor */
	virtual ~WinnerTakeAllFunction() { /* Nothing to do */ };
	/*! Set the value */
	bool setValue( double v );
	/*! Return the value */
	double value();
	/*! Implement the WinnerTakeAllFunction function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 */
	virtual void configure();
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! describe the parameters and subgroups for configure it */
	static void describe( QString );
private:
	double valuev;
};

}

#endif

