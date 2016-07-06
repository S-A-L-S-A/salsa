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

#include "matrixlinker.h"
#include "randomgenerator.h"
#include "logger.h"

using namespace Eigen;

namespace farsa {

MatrixLinker::MatrixLinker( ConfigurationManager& params, QString prefix, Component* parent )
	: Linker( params, prefix, parent ),
	nrows(fromVector().size()),
	ncols(toVector().size()),
	w(MatrixXd::Zero(nrows, ncols)) {
	// the weights are read from a single vector of data supposing they are row-major arranged
	QString str = params.getValue(prefix + "weights");
	if (!str.isEmpty()) {
		QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		int cycleLength = nrows*ncols;
		if (list.size() != cycleLength) {
			Logger::warning( QString("The number of elements of the weights in configuration file (%1) is different from the total number of weights (%2)").arg(list.size()).arg(cycleLength) );
			cycleLength = std::min(list.size(), cycleLength);
		}
		for( int i=0; i<cycleLength; i++) {
			bool ok;
			unsigned int r = i/ncols; //--- division may be expensive
			unsigned int c = i%ncols; //--- module may be expensive
			w(r,c) = list[i].toDouble(&ok);
			if (!ok) {
				w(r,c) = 0.0;
			}
		}
	}
	markAsConfigured();
}

MatrixLinker::~MatrixLinker() {
}

unsigned int MatrixLinker::size() const {
	return nrows*ncols;
}

void MatrixLinker::randomize( double min, double max ) {
	for ( unsigned int i = 0; i<nrows; i++ ) {
		for ( unsigned int j = 0; j<ncols; j++ ) {
			w(i,j) = globalRNG->getDouble( min, max );
		}
	}
}

void MatrixLinker::setWeight( unsigned int from, unsigned int to, double weight ) {
	w(from,to) = weight;
}

double MatrixLinker::weight( unsigned int from, unsigned int to ) {
	return w(from,to);
}

void MatrixLinker::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	Linker::save();
	params.startObjectParameters( prefix, "MatrixLinker", this );
	// First creating a string list, then transforming to a single string
	QStringList list;
	for( unsigned int r=0; r<rows(); r++ ) {
		for( unsigned int c=0; c<cols(); c++ ) {
			list.push_back(QString::number(w(r,c)));
		}
	}
	params.createParameter(prefix, "weights", list.join(" "));
}

void MatrixLinker::describe( QString type ) {
	Linker::describe( type );
	Descriptor d = addTypeDescription( type, "The MatrixLinker calculate a weighted sum of values from one cluster and put the result into the other cluster" );
	d.describeReal("weights").props( ParamIsList ).help("The matrix of weights written as a row-major arranged list");
}

}
