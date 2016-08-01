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

#include "updatable.h"
#include "configurationhelper.h"

namespace salsa {

Updatable::Updatable( ConfigurationManager& params, QString prefix, Component* parent ) :
	Component(params, prefix, parent)
{
	// take the name from the property if has been setted
	QString upname = ConfigurationHelper::getString( params, prefix+"name", "unamed" );
	setName( upname );
	markAsConfigured();
}

Updatable::~Updatable() {
}

void Updatable::setName( QString newname ) {
	namev = newname;
}

QString Updatable::name() const {
	return namev;
}

void Updatable::save() {
	ConfigurationManager& params = configurationManager();
	params.startObjectParameters(prefixPath(), "Updatable", this);
	if ( name() != "unamed" ) {
		params.createParameter(prefixPath(), "name", name());
	}
}

void Updatable::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "an Updatable" );
	d.describeString("name").def("unamed").help("The name of this element");
}

}
