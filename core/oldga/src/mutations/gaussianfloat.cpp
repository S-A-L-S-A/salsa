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

#include "mutations/gaussianfloat.h"
#include "core/genotype.h"
#include "randomgenerator.h"
#include "genotypes/doublegenotype.h"
#include "configurationparameters.h"

namespace farsa {

GaussianFloat::GaussianFloat()
	: Mutation() {
	varg = 0.1;
}

GaussianFloat::~GaussianFloat() {
	//--- nothing to do
}

void GaussianFloat::mutate( Genotype* gen ) {
	DoubleGenotype* rgen = dynamic_cast<DoubleGenotype*>( gen );
	Q_ASSERT_X( rgen != 0,
				"GaussianFloat::mutate",
				"In order to use GaussianFloat mutation the Genotypes must be a subclass of DoubleGenotype" );
	double min = rgen->minValue();
	double max = rgen->maxValue();
	for( unsigned int i=0; i<rgen->numGenes(); i++ ) {
		if ( globalRNG->getBool( mutationRate( rgen->geneToBitIndex(i) ) ) ) {
			double newvalue = globalRNG->getGaussian( varg, rgen->at(i) );
			if ( newvalue > max ) newvalue = max;
			if ( newvalue < min ) newvalue = min;
			rgen->set( i, newvalue );
		}
	}
}

void GaussianFloat::configure( ConfigurationParameters& params, QString prefix ) {
	Mutation::configure( params, prefix );
	double nvarg = params.getValue( prefix + QString( "variance" ) ).toDouble();
	if ( nvarg <= 0 ) {
		qWarning() << "GaussianFloat configuration: variance parameter must be positive";
	} else {
		varg = nvarg;
	}
}

void GaussianFloat::describe( QString type ) {
	Mutation::describe( type );
	Descriptor d = addTypeDescription( type, "Mutate a value accordlying a Gaussian distribution", "The mutated value is extracted stochastically from a Gaussian distribution centered on the current value of the gene with a variance specified by the variance parameter" );
	d.describeReal( "variance" ).limits( 0, 1 ).def( 0.2 ).help( "The variance of the Gaussian distribution" );
}

void GaussianFloat::save( ConfigurationParameters& params, QString prefix ) {
	Mutation::save( params, prefix );
	params.createParameter( prefix, QString("variance"), QString("%1").arg(varg) );
}

} // end namespace farsa
