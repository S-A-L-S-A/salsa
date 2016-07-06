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

#include "mutations/flipbit.h"
#include "core/genotype.h"
#include "randomgenerator.h"

namespace farsa {

FlipBit::FlipBit()
	: Mutation() {
}

FlipBit::~FlipBit() {
	//--- nothing to do
}

void FlipBit::mutate( Genotype* gen ) {
	Genotype& genref = *gen;
	for( unsigned int i=0; i<genref.size(); i++ ) {
		if ( globalRNG->getBool( mutationRate( i ) ) ) {
			genref.toggle(i);
		}
	}
}

void FlipBit::describe( QString type ) {
	Mutation::describe( type );
	addTypeDescription( type, "Mutation operator", "Flip a bit with the probability specified by mutation_rate parameter" );
}

} // end namespace farsa
