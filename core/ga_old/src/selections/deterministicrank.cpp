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

#include "gaconfig.h"
#include "core/genome.h"
#include "selections/deterministicrank.h"
#include "configurationparameters.h"

namespace farsa {

DeterministicRank::DeterministicRank()
	: Selection() {
	nTruncation = 1;
}

DeterministicRank::~DeterministicRank() {
	//--- nothing to do
}

const Genotype* DeterministicRank::select() {
	return genome()->at( idNext++ % nTruncation );
}

void DeterministicRank::setGenome( const Genome* g ) {
	Selection::setGenome( g );
	idNext = 0;
}

void DeterministicRank::setTruncation( int nTruncation ) {
	this->nTruncation = nTruncation;
}

int DeterministicRank::truncation() {
	return nTruncation;
}

void DeterministicRank::configure( ConfigurationParameters& params, QString prefix ) {
	nTruncation = params.getValue( prefix + QString( "nTruncation" ) ).toInt();
}

void DeterministicRank::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "DeterministicRank" );
	params.createParameter( prefix, QString("nTruncation"), QString("%1").arg( nTruncation ) );
}

void DeterministicRank::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Selects the individual with a deterministic round-robin" );
	d.describeInt( "nTruncation" ).limits( 1, INT_MAX ).def( 20 ).help( "Only the first nTruncation individuals will be select for reproduce. The individuals will be sorted on their rank" );
}

} // end namespace farsa
