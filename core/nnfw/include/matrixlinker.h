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

#ifndef MATRIXLINKER_H
#define MATRIXLINKER_H

#include "nnfwconfig.h"
#include "linker.h"

namespace salsa {

/*! \brief MatrixLinker Class define a full connection between two cluster.
 *
 * This class rapresent connections between neuron's 'from' Cluster and neuron's 'to' Cluster with a matrix.
 * It allow only full connections among neurons.
 *
 * The connections have a weight and these are stored in a matrix on which
 * the row index refers to the neurons in the cluster 'from' and
 * the column index refers to the neurons in the cluster 'to'.
 *
 * Every connection is weighted, and the weight is memorized into a weight-matrix.
 */
class SALSA_NNFW_API MatrixLinker : public Linker {
public:
	/*! Constructor */
	MatrixLinker( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*!  Destructor */
	virtual ~MatrixLinker();
	/*!  Get the number of rows */
	unsigned int rows() {
		return nrows;
	};
	/*!  Get the number of cols */
	unsigned int cols() {
		return ncols;
	};
	/*!  Returns the total number of the links: rows*cols */
	unsigned int size() const;
	/*!  Randomize the weights of the MatrixLinker */
	virtual void randomize( double min, double max );
	/*!  Set the weight of the connection specified */
	virtual void setWeight( unsigned int from, unsigned int to, double weight );
	/*!  Get the weight of the connection specified */
	virtual double weight( unsigned int from, unsigned int to );
	/*!  Return the weight matrix */
	DoubleMatrix& matrix() {
		return w;
	};
	/*! const version of matrix() method */
	const DoubleMatrix& matrix() const {
		return w;
	};
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
private:
	/*! Registers the dimensions of the matrix */
	unsigned int nrows, ncols;
	/*! Weight Matrix */
	DoubleMatrix w;
};

}

#endif
