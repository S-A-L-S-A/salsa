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

#include "crossovers/onepoint.h"
#include "core/genotype.h"
#include "randomgenerator.h"
#include "configurationparameters.h"

namespace farsa {

OnePoint::OnePoint() : Crossover() {
}

OnePoint::~OnePoint() {
	//--- nothing to do
}

void OnePoint::crossover( Genotype* father, const Genotype* mother ) {
	int max = qMin( father->size(), mother->size() );
	int splitpoint = globalRNG->getInt( 0, max );
	for( int i=splitpoint; i<max; i++ ) {
		if ( father->bit(i) != mother->bit(i) ) {
			father->toggle(i);
		}
	}
}

void OnePoint::configure( ConfigurationParameters& params, QString prefix ) {
	Q_UNUSED( params );
	Q_UNUSED( prefix );
	//--- nothing to do
}

void OnePoint::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "OnePoint" );
}

void OnePoint::describe( QString type ) {
	addTypeDescription( type, "The one point crossover", "It randomly select one point where to split the two Genotypes, and create a new one combining the first part of one Genotype with the second part of the other one" );
}

} // end namespace farsa
