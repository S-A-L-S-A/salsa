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

#ifndef LINKERS_H
#define LINKERS_H

#include "nnfwconfig.h"
#include "cluster.h"
#include "updatable.h"

namespace farsa {

/*! \brief Abstract Linker Class. This define the common interface among Linkers
 *
 */
class FARSA_NNFW_API Linker : public Updatable {
public:
	/*! Constructor */
	Linker( ConfigurationManager& params, QString prefix, Component* parent = NULL );
	/*! Return the Cluster From */
	Cluster* from() const {
		return fromc;
	};
	/*! Return the Cluster to */
	Cluster* to() const {
		return toc;
	};
	/*! Configure on which state vector of 'from' Cluster this Linker is attached to
	 *  \param vectorName is the name assigned to the state vector
	 *  \warning it raise an exception is the vectorName does not exists
	 */
	void configureFromVector( QString vectorName ) {
		getFromVector = fromc->getDelegateFor( vectorName );
		fromVectorName = vectorName;
	}
	/*! Configure on which state vector of 'to' Cluster this Linker is attached to
	 *  \param vectorName is the name assigned to the state vector
	 *  \warning it raise an exception is the vectorName does not exists
	 */
	void configureToVector( QString vectorName ) {
		getToVector = toc->getDelegateFor( vectorName );
		toVectorName = vectorName;
	}
	/*! Returns the 'size' of the linker<br>
	 * The actual means of 'size' will be specified by sub-classes that implementes concrete linkers
	 */
	virtual unsigned int size() const = 0;
	/*! Randomize the parameters of the Linker<br>
	 * The parameters randomized by this method will be specified by sub-classes
	 */
	virtual void randomize( double min, double max ) = 0;
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
protected:
	/*! Return a reference to the DoubleVector of 'from' on which this is attached to */
	DoubleVector& fromVector() const {
		return (*getFromVector)(fromc);
	};
	/*! Return a reference to the DoubleVector of 'to' on which this is attached to */
	DoubleVector& toVector() const {
		return (*getToVector)(toc);
	};
private:
	/*! incoming Cluster */
	Cluster* fromc;
	/*! delegate for getting the vector of 'from' Cluster */
	Cluster::getStateVectorFuncPtr getFromVector;
	/*! name of vector corresponding to the delegate getFromVector */
	QString fromVectorName;
	/*! outgoing Cluster */
	Cluster* toc;
	/*! delegate for getting the vector of 'to' Cluster */
	Cluster::getStateVectorFuncPtr getToVector;
	/*! name of vector corresponding to the delegate getToVector */
	QString toVectorName;
};

}

#endif
