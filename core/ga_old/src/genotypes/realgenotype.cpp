/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2009 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "genotypes/realgenotype.h"
#include "randomgenerator.h"
#include <cmath>
#include "configurationparameters.h"

namespace farsa {

RealGenotype::RealGenotype( unsigned int numGenes, float min, float max )
	: DoubleGenotype( numGenes, min, max, 32 ) {
	genes = (float*)( data );
}

RealGenotype::~RealGenotype() {
	//--- nothing to do
}

RealGenotype::RealGenotype( const RealGenotype& genotype )
	: DoubleGenotype( genotype ) {
	genes = (float*)( data );
}

RealGenotype& RealGenotype::operator=( const Genotype& genotype ) {
	RealGenotype* genoreal = (RealGenotype*)(&genotype);
	copyDataFrom( genoreal );
	this->numgenes = genoreal->numgenes;
	this->minv = genoreal->minv;
	this->maxv = genoreal->maxv;
	genes = (float*)( data );
	return *this;
}

void RealGenotype::configure( ConfigurationParameters& params, QString prefix ) {
	numgenes = params.getValue( prefix + QString( "ngenes" ) ).toInt();
	Q_ASSERT_X( numgenes > 0,
				"RealGenotype::configure",
				"The ngenes must be present in the config file and its value must be greater than zero" );
	minv = params.getValue( prefix + QString( "minValue" ) ).toDouble();
	maxv = params.getValue( prefix + QString( "maxValue" ) ).toDouble();
	Q_ASSERT_X( minv < maxv,
				"RealGenotype::configure",
				"The minValue and maxValue must be different!! Check you config file" );
	resize( numgenes*32 );
	QString zipdata = params.getValue( prefix + QString( "data" ) );
	if ( !zipdata.isNull() ) {
		fromCompressedString( zipdata );
	}
	genes = (float*)( data );
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

void RealGenotype::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "RealGenotype" );
	params.createParameter( prefix, QString("ngenes"), QString("%1").arg( numgenes ) );
	params.createParameter( prefix, QString("minValue"), QString("%1").arg( minv ) );
	params.createParameter( prefix, QString("maxValue"), QString("%1").arg( maxv ) );
	QString fitstring;
	foreach( double avalue, fitnessv ) {
		fitstring.append( QString("%1 ").arg(avalue) );
	}
	params.createParameter( prefix, QString("fitness"), fitstring );
	params.createParameter( prefix, QString("data"), toCompressedString() );
	params.createParameter( prefix, QString("rank"), QString("%1").arg(rankv) );
	params.createParameter( prefix, QString("notes"), notesv );
}

void RealGenotype::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "RealGenotype encode a vector of real double values, each double correspond to a gene" );
	d.describeInt( "ngenes" ).limits( 1, MaxInteger ).def( 8 ).props( IsMandatory ).help( "The number of real values represented by the Genotype" );
	d.describeReal( "minValue" ).def( -5.0 ).help( "The minimum value representable" );
	d.describeReal( "maxValue" ).def( +5.0 ).help( "The maximum value representable" );
	d.describeReal( "fitness" ).props( IsList ).help( "The fitness of the Genotype", "The fitness of a Genotype support multi objective fitness; if you specify a vector of values they are considered different objectives of the fitness" );
	d.describeString( "data" ).help( "The bits composing the Genotype stored in a compressed string" );
	d.describeReal( "rank" ).def( 0 ).help( "The rank indicate who is more fitted that others and how much; the values are dependent on the kind of GeneticAlgo used" );
}

double RealGenotype::at( unsigned int i ) const {
	Q_ASSERT_X( (i+1)*32 <= size(),
				"RealGenotype::at",
				"The value requested is beyond the dimension of this Genotype" );
	return (double) ( genes[i] );
}

void RealGenotype::set( unsigned int i, double value ) {
	Q_ASSERT_X( (i+1)*32 <= size(),
				"RealGenotype::set",
				"The value to be set is beyond the dimension of this Genotype" );
	genes[i] = (float) ( value );
	return;
}

void RealGenotype::randomize() {
	for( unsigned int i=0; i<numgenes; i++ ) {
		genes[i] = (float)( globalRNG->getDouble( minv, maxv ) );
	}
}

RealGenotype* RealGenotype::clone() const {
	return ( new RealGenotype( *this ) );
}

} // end namespace farsa
