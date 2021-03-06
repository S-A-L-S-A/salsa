/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "dotlinker.h"

namespace salsa {

DotLinker::DotLinker( ConfigurationManager& params, QString prefix, Component* parent )
	: MatrixLinker( params, prefix, parent ) {
	// there are no extra parameters to configure
	markAsConfigured();
}

DotLinker::~DotLinker() {
}

void DotLinker::update() {
	// check if cluster 'To' needs a reset
	if ( to()->needReset() ) {
		to()->resetInputs();
	}
	toVector().noalias() = matrix().transpose()*fromVector();
	return;
}

void DotLinker::save() {
	MatrixLinker::save();
	configurationManager().startObjectParameters( prefixPath(), "DotLinker", this );
}

void DotLinker::describe( QString type ) {
	MatrixLinker::describe( type );
	addTypeDescription( type, "The DotLinker use the dot product to calculate the values to write on outgoing Cluster" );
}

}
