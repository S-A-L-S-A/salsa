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

#include "copylinker.h"

namespace farsa {

CopyLinker::CopyLinker( ConfigurationManager& params, QString prefix, Component* parent )
	: Linker( params, prefix, parent ) {
	m_size = qMin( toVector().size(), fromVector().size() );
	markAsConfigured();
}

CopyLinker::~CopyLinker() {
}

void CopyLinker::update() {
	// check if cluster 'To' needs a reset
	if ( to()->needReset() ) {
		to()->resetInputs();
	}
	
	toVector().head(m_size) = fromVector().head(m_size);
	return;
}

unsigned int CopyLinker::size() const {
	return m_size;
}

void CopyLinker::save() {
	Linker::save();
	configurationManager().startObjectParameters( prefixPath(), "CopyLinker", this );
}

void CopyLinker::describe( QString type ) {
	Linker::describe( type );
	Descriptor d = addTypeDescription( type, "The CopyLinker copies values from one Cluster to another" );
}

}
