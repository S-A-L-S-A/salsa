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

#include "genotypes/doublegenotype.h"
#include <cmath>

namespace farsa {

DoubleGenotype::DoubleGenotype( unsigned int numGenes, double min, double max, unsigned int bitPrecision )
	: Genotype( numGenes*bitPrecision ) {
	numgenes = numGenes;
	bitprec = bitPrecision;
	maxv = max;
	minv = min;
}

DoubleGenotype::~DoubleGenotype() {
	//--- nothing to do
}

DoubleGenotype::DoubleGenotype( const DoubleGenotype& genotype )
	: Genotype( genotype ) {
	this->numgenes = genotype.numgenes;
	this->bitprec = genotype.bitprec;
	this->maxv = genotype.maxv;
	this->minv = genotype.minv;
}

unsigned int DoubleGenotype::numGenes() const {
	return numgenes;
}

double DoubleGenotype::maxValue() {
	return maxv;
}

double DoubleGenotype::minValue() {
	return minv;
}

unsigned int DoubleGenotype::bitPrecision() {
	return bitprec;
}

unsigned int DoubleGenotype::geneToBitIndex( unsigned int gene ) const {
	return gene * bitprec;
}

unsigned int DoubleGenotype::bitToGeneIndex( unsigned int bit ) const {
	return bit / bitprec;
}
} // end namespace farsa
