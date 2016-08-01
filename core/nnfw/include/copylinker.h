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

#ifndef COPYLINKER_H
#define COPYLINKER_H

#include "nnfwconfig.h"
#include "linker.h"

namespace salsa {

/*! \brief CopyLinker Class. This linker copies the outputs of a cluster to inputs of another cluster
 *
 *   The CopyLinker is useful in networks when one requires that data be
 *   transferred from one Cluster's terminal to another without computation.
 *   For example, in Elmann networks the values of hidden layer inputs are
 *   copied to the context layer Cluster (which acts like a short-term memory
 *   of former inputs).
 *
 *   The CopyLinker is an object that 'literally' copies the outputs from one
 *   Cluster to another.
 *   Use the fromVector and toVector for specifying the source and 
 *   target vector for the Cluster
 *
 *   If there is a mis-match in the dimensionality of the terminals connected
 *   by the CopyLinker, as much data as possible is copied.  For example; a
 *   CopyLinker between a 'from' and 'to' Cluster with 8
 *   and 5 inputs respectively, will copy the first 5 data elements for
 *   'from' into the inputs for 'to'.  Vice versa, if the dimensions of
 *   'from' and 'to' are 5 and 8 respectively, then the 5 inputs of the
 *   'from' cluster (all of the source data) are copied into the first 5
 *   inputs of the 'to' Cluster (with 3 neurons receiving no input).
 */
class SALSA_NNFW_API CopyLinker : public Linker {
public:
	/*! Constructor */
	CopyLinker( ConfigurationManager& params, QString prefix, Component* parent = NULL );
	/*! Destructor */
	virtual ~CopyLinker();
	/*! Update the linker */
	void update();
	/*! Returns the number of neurons connected */
	unsigned int size() const;
	/*! Randomize (i.e. do nothing !!) */
	void randomize( double , double ) {
		// --- Do Nothing
	};
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
private:
	/*! the number of neurons that I will copy from one cluster to the other */
	int m_size;
};

}

#endif
