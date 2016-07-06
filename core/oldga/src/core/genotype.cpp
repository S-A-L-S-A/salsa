/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
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

#include "core/genotype.h"
#include "randomgenerator.h"
#include <cmath>
#include <cstring>
#include "configurationparameters.h"

namespace farsa {

Genotype::Genotype( unsigned int size ) {
	sizev = size;
	//--- allocate a bit more that necessary for safety
	allocated = (unsigned int)( (size+1)/8 ) + 1;
	data = new unsigned char[allocated];
	fitnessv.resize(1);
	fitnessv[0] = 0.0;
	rankv = 0.0;
	notesv = "";
}

Genotype::~Genotype() {
	delete []data;
}

Genotype::Genotype( QString str, bool compressed ) {
	if ( !compressed ) {
		sizev = str.size();
		//--- allocate a bit more that necessary for safety
		allocated = (unsigned int)( (sizev+1)/8 ) + 1;
		data = new unsigned char[allocated];
		for( unsigned int i=0; i<sizev; i++ ) {
			if ( str[i] == '1' ) {
				set( i );
			} else {
				unset( i );
			}
		}
	} else {
		QByteArray temp2 = QByteArray::fromBase64( str.toLatin1() );
		QByteArray temp  = qUncompress( temp2 );
		sizev = temp.size();
		//--- allocate a bit more that necessary for safety
		allocated = (unsigned int)( (sizev+1)/8 ) + 1;
		data = new unsigned char[allocated];
		for( unsigned int i=0; i<sizev; i++ ) {
			if ( temp[i] == '1' ) {
				set( i );
			} else {
				unset( i );
			}
		}
	}
	fitnessv.resize(1);
	fitnessv[0] = 0.0;
	rankv = 0.0;
	notesv = "";
}

Genotype::Genotype( const Genotype& genotype ) : ParameterSettableWithConfigureFunction() {
	sizev = genotype.sizev;
	allocated = genotype.allocated;
	data = new unsigned char[allocated];
	memcpy( data, genotype.data, allocated );
	fitnessv = genotype.fitnessv;
	rankv = genotype.rankv;
	notesv = genotype.notesv;
}

Genotype& Genotype::operator=( const Genotype& genotype ) {
	//--- even if constructed with default constructor at least 1 char is allocated
	delete []data;
	sizev = genotype.sizev;
	allocated = genotype.allocated;
	data = new unsigned char[allocated];
	memcpy( data, genotype.data, allocated );
	fitnessv = genotype.fitnessv;
	rankv = genotype.rankv;
	notesv = genotype.notesv;
	return (*this);
}

void Genotype::configure( ConfigurationParameters& params, QString prefix ) {
	int newsize = params.getValue( prefix + QString( "bitsize" ) ).toInt();
	Q_ASSERT_X( newsize > 0,
				"Genotype::configure",
				"The bitsize must be present in the config file and its value must be greater than zero" );
	resize( newsize );
	QString zipdata = params.getValue( prefix + QString( "data" ) );
	if ( !zipdata.isNull() ) {
		fromCompressedString( zipdata );
	}
	QStringList valuesList = params.getValue( prefix + QString( "fitness" ) )
									.split( QRegExp("\\s+"), QString::SkipEmptyParts );
	if ( valuesList.size() > 0 ) {
		// read the values of fitness
		fitnessv.resize(0);
		foreach( QString avalue, valuesList ) {
			fitnessv << avalue.toDouble();
		}
		// safe check
		if ( fitnessv.size() == 0 ) {
			fitnessv.append( 0 );
		}
	}
	rankv = params.getValue( prefix + QString( "rank" ) ).toDouble();
	notesv = params.getValue( prefix + QString( "notes" ) );
}

void Genotype::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "Genotype" );
	params.createParameter( prefix, QString("bitsize"), QString("%1").arg(sizev) );
	QString fitstring;
	foreach( double avalue, fitnessv ) {
		fitstring.append( QString("%1 ").arg(avalue) );
	}
	params.createParameter( prefix, QString("fitness"), fitstring );
	params.createParameter( prefix, QString("data"), toCompressedString() );
	params.createParameter( prefix, QString("rank"), QString("%1").arg(rankv) );
	params.createParameter( prefix, QString("notes"), notesv );
}

void Genotype::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Genotype is a binary string, each bit correspond to a gene" );
	d.describeInt( "bitsize" ).limits( 1, INT_MAX ).def( 8 ).props( IsMandatory ).help( "The number of bits of the Genotype", "The Genotype is represented by a binary string of bitsize length" );
	d.describeReal( "fitness" ).props( IsList ).help( "The fitness of the Genotype", "The fitness of a Genotype support multi objective fitness; if you specify a vector of values they are considered different objectives of the fitness" );
	d.describeString( "data" ).help( "The bits composing the Genotype stored in a compressed string" );
	d.describeReal( "rank" ).help( "The rank indicate who is more fitted that others and how much; the values are dependent on the kind of GeneticAlgo used" );
}

void Genotype::assign( const Genotype* genotype ) {
	Genotype& self = *this;
	self = *genotype;
}

Genotype* Genotype::clone() const {
	return new Genotype( *this );
}

unsigned int Genotype::size() const {
	return sizev;
}

void Genotype::resize( unsigned int newsize ) {
	//--- even if constructed with default constructor at least 1 char is allocated
	unsigned int old_allocated = allocated;
	sizev = newsize;
	allocated = (unsigned int)( (sizev+1)/8 ) + 1;
	unsigned char* newdata = new unsigned char[allocated];
	memcpy( newdata, data, ( allocated < old_allocated ) ? allocated : old_allocated );
	delete []data;
	data = newdata;
	return;
}

void Genotype::setFitness( double value ) {
	setObjective( 0, value );
}

double Genotype::fitness() const {
	return objective(0);
}

void Genotype::setObjective( int i, double value ) {
	fitnessv.resize( qMax( fitnessv.size(), i+1 ) );
	fitnessv[i] = value;
}

double Genotype::objective( int i ) const {
	return fitnessv[i];
}

int Genotype::numOfObjectives() const {
	return fitnessv.size();
}

bool Genotype::dominatedBy( const Genotype* g ) {
	int numObjToCompare = qMin( fitnessv.size(), g->fitnessv.size() );
	bool allLessEq = true;
	bool oneLessStrictly = false;
	for( int i=0; i<numObjToCompare; i++ ) {
		if ( fitnessv[i] > g->fitnessv[i] ) {
			allLessEq = false;
		}
		if ( fitnessv[i] < g->fitnessv[i] ) {
			oneLessStrictly = true;
		}
	}
	return (allLessEq && oneLessStrictly);
}

void Genotype::setRank( double r ) {
	rankv = r;
}

double Genotype::rank() const {
	return rankv;
}

bool Genotype::bit( unsigned int i ) const {
	//--- find the index into data array that contains bit request
	int id = i >> 3; //i/8;
	//--- create the mask for extract the bit requested
	//--- 0x80 is binary 1000.0000
	unsigned char mask = 0x80 >> (i&7); //(i%8);
	return ( data[id] & mask );
}

void Genotype::set( unsigned int i ) {
	//--- find the index into data array that contains bit request
	int id = i >> 3; //i/8;
	//--- create the mask
	//--- 0x80 is binary 1000.0000
	unsigned char mask = 0x80 >> (i&7); //(i%8);
	//--- setting bit to 1
	data[id] = data[id] | mask;
}

void Genotype::unset( unsigned int i ) {
	//--- find the index into data array that contains bit request
	int id = i >> 3; //i/8;
	//--- create the mask
	//--- 0x80 is binary 1000.0000
	unsigned char mask = 0x80 >> (i&7); //(i%8);
	//--- setting bit to 0, require the inverted mask
	data[id] = data[id] & (~mask);
}

void Genotype::toggle( unsigned int i ) {
	//--- find the index into data array that contains bit request
	int id = i >> 3; //i/8;
	//--- create the mask
	//--- 0x80 is binary 1000.0000
	unsigned char mask = 0x80 >> (i&7); //(i%8);
	//--- toggling a bit using XOR
	data[id] = data[id] ^ mask;
}

QString Genotype::notes() const {
	return notesv;
}

void Genotype::setNotes( QString notes ) {
	notesv = notes;
}

int Genotype::hammingDistance( const Genotype* other ) {
	int ret = 0;
	for( unsigned int i=0; i<sizev; i++ ) {
		ret += ( bit(i) == other->bit(i) );
	}
	return ret;
}

void Genotype::randomize() {
	for( unsigned int i=0; i<sizev; i++ ) {
		if ( globalRNG->getBool( 0.5 ) ) {
			set( i );
		} else {
			unset( i );
		}
	}
}

QString Genotype::toString() const {
	QString ret;
	ret.resize( sizev );
	for( unsigned int i=0; i<sizev; i++ ) {
		ret[i] = ( bit(i) ) ? '1' : '0';
	}
	return ret;
}

void Genotype::fromString( QString str ) {
	int dim = qMin( (int)sizev, str.size() );
	for( int i=0; i<dim; i++ ) {
		if ( str[i] == '1' ) {
			set( i );
		} else {
			unset( i );
		}
	}
}

QString Genotype::toCompressedString() const {
	QByteArray temp;
	temp.resize( sizev );
	for( unsigned int i=0; i<sizev; i++ ) {
		temp[i] = ( bit(i) ) ? '1' : '0';
	}
	QByteArray temp2 = qCompress( temp, 9 );
	return temp2.toBase64();
}

bool Genotype::fromCompressedString( QString str ) {
	QByteArray temp2 = QByteArray::fromBase64( str.toLatin1() );
	QByteArray temp  = qUncompress( temp2 );
	if ( temp.isEmpty() ) {
		return false;
	}
	int dim = qMin( (int)sizev, temp.size() );
	for( int i=0; i<dim; i++ ) {
		if ( temp[i] == '1' ) {
			set( i );
		} else {
			unset( i );
		}
	}
	return true;
}

unsigned int Genotype::extractUInt( unsigned int startPos, unsigned int stopPos ) const {
	if ( startPos >= sizev ) return 0;

	stopPos = qMin( stopPos, sizev );
	unsigned int len = stopPos - startPos;
	unsigned int ret = 0;
	for( ; startPos<stopPos; startPos++ ) {
		len--;
		if ( bit(startPos) ) {
			ret += (unsigned int)( pow( 2.0f, (int)len ) );
		}
	}
	return ret;
}

void Genotype::insertUInt( unsigned int value, unsigned int startPos, unsigned int stopPos ) {
	if ( startPos >= sizev ) return ;

	stopPos = qMin( stopPos, sizev );
	for( ; stopPos!=startPos; stopPos-- ) {
		if ( value%2 == 1 ) {
			set( stopPos-1 );
		} else {
			unset( stopPos-1 );
		}
		value /= 2;
	}
	return;
}

unsigned int Genotype::geneToBitIndex( unsigned int gene ) const {
	return gene;
}

unsigned int Genotype::bitToGeneIndex( unsigned int bit ) const {
	return bit;
}

void Genotype::copyDataFrom( Genotype* source ) {
	//--- even if constructed with defaul constructor at least 1 char is allocated
	delete []data;
	sizev = source->sizev;
	allocated = source->allocated;
	data = new unsigned char[allocated];
	memcpy( data, source->data, allocated );
	fitnessv = source->fitnessv;
	rankv = source->rankv;
	notesv = source->notesv;
	return;
}

} // end namespace farsa
