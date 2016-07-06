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

#ifndef NORMLINKER_H
#define NORMLINKER_H

#include "nnfwconfig.h"
#include "matrixlinker.h"

namespace farsa {

/*! \brief NormLinker Class
 *
 */
class FARSA_NNFW_API NormLinker : public MatrixLinker {
public:
	/*! Constructor */
	NormLinker( ConfigurationManager& params, QString prefix, Component* parent = NULL );
	/*! Destructor */
	virtual ~NormLinker();
	/*! Performs the euclidean norm calculation */
	void update();
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
protected:
	// temp data
	DoubleVector temp;
};

}

#endif
