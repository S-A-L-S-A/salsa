/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2008 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "core/genome.h"
#include "factory.h"
#include <QtAlgorithms>

namespace salsa {

Genome::Genome() {
	prototype = NULL;
}

Genome::Genome( unsigned int numGenotype, unsigned int numBits ) {
	prototype = new Genotype( numBits );
	data.resize( numGenotype );
	for( unsigned int i=0; i<numGenotype; i++ ) {
		data[i] = new Genotype( numBits );
		data[i]->randomize();
	}
}

Genome::Genome( unsigned int numGenotype, const Genotype* prototype ) : ParameterSettableWithConfigureFunction() {
	this->prototype = prototype->clone();
	data.resize( numGenotype );
	for( unsigned int i=0; i<numGenotype; i++ ) {
		data[i] = prototype->clone();
		data[i]->randomize();
	}
}

Genome::Genome( const Genome& genome ) : ParameterSettableWithConfigureFunction() {
	this->prototype = genome.prototype->clone();
	data.resize( genome.data.size() );
	for( int i=0; i<data.size(); i++ ) {
		data[i] = genome.data[i]->clone();
	}
}

Genome::~Genome() {
	if ( prototype ) {
		delete prototype;
	}
	// The destruction of genotypes was added by Tomassino. It is not certain that
	// this won't create any problem...
	for( int i=0; i<data.size(); i++ ) {
		delete (data[i]);
	}
}

Genome& Genome::operator=( const Genome& right ) {
	this->prototype = right.prototype->clone();
	//--- destroy all memory allocated
	for( int i=0; i<data.size(); i++ ) {
		delete (data[i]);
	}
	data.resize( right.data.size() );
	for( unsigned int i=0; i<right.size(); i++ ) {
		data[i] = right.data[i]->clone();
	}
	return (*this);
}

void Genome::configure( ConfigurationParameters& params, QString prefix ) {
	for( int i=0; i<data.size(); i++ ) {
		delete (data[i]);
	}
	data.clear();
	this->prototype = params.getObjectFromGroup<Genotype>( prefix + QString( "GENOTYPE:prototype" ) );
	foreach( QString group, params.getGroupsWithPrefixList(prefix, QString( "GENOTYPES" )) ) {
		data.append( params.getObjectFromGroup<Genotype>( prefix + "/" + group ) );
	}
	int nindividuals = params.getValue( prefix + QString( "nindividuals" ) ).toInt();
	if ( data.size() != 0 && nindividuals != data.size() ) {
		qWarning() << "Genome configuration: nindividuals parameter should match the number of GENOTYPES specified.";
	}
	int loadedgens = data.size();
	data.resize( nindividuals );
	for( int i=loadedgens; i<nindividuals; i++ ) {
		data[i] = prototype->clone();
		data[i]->randomize();
	}
}

void Genome::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "Genome" );
	params.createParameter( prefix, QString("nindividuals"), QString("%1").arg(data.size()) );

	this->prototype->save( params, params.createSubGroup( prefix, "GENOTYPE:prototype" ) );

	for( int i=0; i<data.size(); i++ ) {
		data[i]->save( params, params.createSubGroup( prefix, QString("GENOTYPES:%1").arg(i) ) );
	}
}

void Genome::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A population of Genotypes" );
	d.describeInt( "nindividuals" ).limits( 1, INT_MAX ).def( 100 ).props( IsMandatory ).help( "The number of Genotypes contained by the Genome" );
	d.describeSubgroup( "GENOTYPE:prototype" ).type( "Genotype" ).props( IsMandatory ).help( "The prototype of Genotypes contained by the Genome" );
	d.describeSubgroup( "GENOTYPE" ).type( "Genotype" ).props( AllowMultiple ).help( "The vector of all Genotypes contained by the Genome" );
}

unsigned int Genome::size() const {
	return data.size();
}

void Genome::clearAll() {
	data.clear();
}

void Genome::append( const Genotype* genotype ) {
	data.append( genotype->clone() );
	return;
}

Genotype* Genome::at( unsigned int i ) {
	return data[i];
}

const Genotype* Genome::at( unsigned int i ) const {
	return data[i];
}

Genotype* Genome::last() {
	return data.last();
}

const Genotype* Genome::last() const {
	return data.last();
}

unsigned int Genome::find( const Genotype* g ) {
	for( int i=0; i<data.size(); i++ ) {
		if ( g == data[i] ) return i;
	}
	return -1;
}

void Genome::set( unsigned int i, Genotype* g ) {
	data[i] = g->clone();
}

const Genotype* Genome::operator[]( unsigned int i ) const {
	return data[i];
}

Genotype* Genome::operator[]( unsigned int i ) {
	return data[i];
}

void Genome::randomize() {
	//--- can be parallelizable
	for( int i=0; i<data.size(); i++ ) {
		data[i]->randomize();
	}
}

} // end namespace salsa
