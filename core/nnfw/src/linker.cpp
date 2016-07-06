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

#include "linker.h"
#include "neuralnet.h"
#include "configurationhelper.h"

namespace farsa {

Linker::Linker( ConfigurationManager& params, QString prefix, Component* parent )
	: Updatable( params, prefix, parent ) {
	fromc = params.getObjectFromParameter<Cluster>( prefix + "from", true );
	if ( !fromc ) {
		// it try to cast the parent and use getByName
		NeuralNet* net = dynamic_cast<NeuralNet*>(parent);
		if ( net ) {
			net->byName( params.getValue(prefix+"from"), fromc );
		}
	}
	toc = params.getObjectFromParameter<Cluster>( prefix + "to", true );
	if ( !toc ) {
		// it try to cast the parent and use getByName
		NeuralNet* net = dynamic_cast<NeuralNet*>(parent);
		if ( net ) {
			net->byName( params.getValue(prefix+"to"), toc );
		}
	}
	if ( !fromc || !toc ) throw ClusterFromOrToMissing();
	fromVectorName = ConfigurationHelper::getString( params, prefix+"fromVector", "outputs" );
	getFromVector = fromc->getDelegateFor( fromVectorName );
	toVectorName = ConfigurationHelper::getString( params, prefix+"toVector", "inputs" );
	getToVector = toc->getDelegateFor( toVectorName );
	markAsConfigured();
}

void Linker::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	Updatable::save();
	params.startObjectParameters(prefix, "Linker", this);
	params.createParameter(prefix, "from", fromc );
	if ( fromVectorName != "outputs" ) {
		params.createParameter( prefix, "fromVector", fromVectorName );
	}
	params.createParameter(prefix, "to", toc );
	if ( toVectorName != "inputs" ) {
		params.createParameter( prefix, "toVector", toVectorName );
	}
}

void Linker::describe( QString type ) {
	Updatable::describe( type );
	Descriptor d = addTypeDescription( type, "The Linker connect two Cluster" );
	d.describeObject( "from" ).type( "Cluster" ).props( ParamIsMandatory ).help( "The Linker will get values from this Cluster" );
	d.describeString( "fromVector" ).def( "outputs" ).help( "The vector on which the Linker will get values from" );
	d.describeObject( "to" ).type( "Cluster" ).props( ParamIsMandatory ).help( "The Linker will write values to this Cluster" );
	d.describeString( "toVector" ).def( "inputs" ).help( "The vector on which the Linker will write values to" );
}

}
