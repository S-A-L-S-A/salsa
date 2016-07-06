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

#include "genotypes/signedrangegenotype.h"
#include <cmath>
#include "configurationparameters.h"

namespace farsa {

#if defined(_WIN32) && !defined(__MINGW32__)
/* replacement of Unix rint() for Windows */
static int rint (double x) {
	char *buf;
	int i,dec,sig;
	buf = _fcvt(x, 0, &dec, &sig);
	i = atoi(buf);
	if( sig == 1 ) {
		i = i * -1;
	}
	return i;
}
#endif

SignedRangeGenotype::SignedRangeGenotype( unsigned int numGenes, double max, unsigned int bitPrec )
	: DoubleGenotype( numGenes, -max, +max, bitPrec ) {
	Q_ASSERT_X( bitPrec > 1,
				"SignedRangeGenotype Constructor",
				"bitPrecision parameters must be greater than 1" );
	dx = ( maxValue() ) / ( std::pow(2.0f, (int)(bitPrec-1)) - 1 );
}

SignedRangeGenotype::~SignedRangeGenotype() {
	//--- nothing to do
}

SignedRangeGenotype::SignedRangeGenotype( const SignedRangeGenotype& genotype )
	: DoubleGenotype( genotype ) {
	this->dx = genotype.dx;
}

SignedRangeGenotype& SignedRangeGenotype::operator=( const Genotype& genotype ) {
	SignedRangeGenotype* genodouble = (SignedRangeGenotype*)( &genotype );
	copyDataFrom( genodouble );
	this->numgenes = genodouble->numgenes;
	this->bitprec = genodouble->bitprec;
	this->maxv = genodouble->maxv;
	this->minv = genodouble->minv;
	this->dx = genodouble->dx;
	return *this;
}

void SignedRangeGenotype::configure( ConfigurationParameters& params, QString prefix ) {
	numgenes = params.getValue( prefix + QString( "ngenes" ) ).toInt();
	Q_ASSERT_X( numgenes > 0,
				"SignedRangeGenotype::configure",
				"The ngenes must be present in the config file and its value must be greater than zero" );
	bitprec = params.getValue( prefix + QString( "nbits" ) ).toInt();
	Q_ASSERT_X( bitprec > 1,
				"SignedRangeGenotype::configure",
				"The nbits must be greater than 1 !! Check your config file" );
	maxv = params.getValue( prefix + QString( "maxValue" ) ).toDouble();
	minv = -maxv;
	Q_ASSERT_X( maxv > 0,
				"SignedRangeGenotype::configure",
				"The maxValue must be greater than zero!! Check you config file" );
	resize( numgenes*bitprec );
	dx = ( maxValue() ) / ( std::pow(2.0f, (int)(bitprec-1)) - 1 );
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

void SignedRangeGenotype::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "SignedRangeGenotype" );
	params.createParameter( prefix, QString("ngenes"), QString("%1").arg( numgenes ) );
	params.createParameter( prefix, QString("nbits"), QString("%1").arg( bitprec ) );
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

void SignedRangeGenotype::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "SignedRangeGenotype encode a vector of real values, each value correspond to a gene" );
	d.describeInt( "ngenes" ).limits( 1, INT_MAX ).def( 8 ).props( IsMandatory ).help( "The number of real values represented by the Genotype" );
	d.describeInt( "nbits" ).limits( 4, 32 ).def( 16 ).help( "The number of bits used to encode one real value", "The SignedRangeGenotype encode the real value in a discrete way inside a boundary; Supposing nbits is 4, then 4 bits are used to encode a real value where the first bit represent the sign, and the other three represents the number of discretization of the interval from -maxValue to +maxValue. In this way, the representation of zero is always present and has two encoding: 0000 (-0) and 1000 (+0)" );
	d.describeReal( "maxValue" ).def( 5.0 ).help( "The absolute maximum value representable. Look at the help of nbits for details" );
	d.describeReal( "fitness" ).props( IsList ).help( "The fitness of the Genotype", "The fitness of a Genotype support multi objective fitness; if you specify a vector of values they are considered different objectives of the fitness" );
	d.describeString( "data" ).help( "The bits composing the Genotype stored in a compressed string" );
	d.describeReal( "rank" ).help( "The rank indicate who is more fitted that others and how much; the values are dependent on the kind of GeneticAlgo used" );
}

double SignedRangeGenotype::at( unsigned int i ) const {
	Q_ASSERT_X( (i+1)*bitprec <= size(),
				"SignedRangeGenotype::at",
				"The value requested is beyond the dimension of this Genotype" );
	double ret = extractUInt( i*bitprec+1, (i+1)*bitprec );
	//--- first bit (MSB) represent the sign: 0 positive, 1 negative
	if ( bit( i*bitprec ) ) {
		return -(dx*ret);
	} else {
		return +(dx*ret);
	}
}

void SignedRangeGenotype::set( unsigned int i, double value ) {
	Q_ASSERT_X( (i+1)*bitprec <= size(),
				"SignedRangeGenotype::set",
				"The value to be set is beyond the dimension of this Genotype" );
	value = qMin( maxv, value );
	value = qMax( -maxv, value );
	unsigned int intP = (unsigned int)( rint( qAbs(value)/dx ) );
	insertUInt( (unsigned int)intP, i*bitprec+1, (i+1)*bitprec );
	if ( value < 0 ) {
		Genotype::set( i*bitprec );
	} else {
		Genotype::unset( i*bitprec );
	}
	return;
}

SignedRangeGenotype* SignedRangeGenotype::clone() const {
	return ( new SignedRangeGenotype( *this ) );
}

} // end namespace farsa
