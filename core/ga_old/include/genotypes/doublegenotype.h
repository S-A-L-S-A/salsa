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

#ifndef DOUBLEGENOTYPE_H
#define DOUBLEGENOTYPE_H

#include "gaconfig.h"
#include "core/genotype.h"

namespace salsa {

/*!  \brief DoubleGenotype class
 *
 *  \par Description
 *   A gene is a double value within the range specificed by minValue and maxValue methods.<br>
 *   This is an abstract class defining the common interface among different encoding of double numbers
 *   inside a gene.<br>
 *  \par Warnings
 *
 * \ingroup ga_genos
 */
class SALSA_GA_API DoubleGenotype : public Genotype {
public:
	/*! Construct a un-initialized DoubleGenotype <br>
	 *  \param numGenes is the number of double values contained by this DoubleGenotype
	 *  \param max is the maximum absolute value represented
	 *  \param min is the minimum absolute value represented
	 *  \param bitPrec is the bit precision used for encode a double value
	 *  \warning you must explicity call randomize() to get a randomized Genotype
	 *  \note the Genotype::size() return the number of bits of underlying data, while numGenes is the actual
	 *    numbers of genes of this DoubleGenotype. They are related by the multiplication:
	 *    \f$ size = bitPrecision \cdot numGenes \f$
	 */
	DoubleGenotype( unsigned int numGenes, double min, double max, unsigned int bitPrec );
	/*! Destructor */
	virtual ~DoubleGenotype();
	/*! Copy-constructor */
	DoubleGenotype( const DoubleGenotype& genotype );
	/*! return the numGenes */
	unsigned int numGenes() const;
	/*! Return the max absolute value of genes */
	double maxValue();
	/*! Return the min absolute value of genes */
	double minValue();
	/*! Return the bit precision, i.e. the number of bit used to encode one double value */
	unsigned int bitPrecision();
	/*! Return the i-th genes represented */
	virtual double at( unsigned int i ) const = 0;
	/*! Set the i-th genes */
	virtual void set( unsigned int i, double value ) = 0;

	/*!
	 * Convert the index of the given gene to the index of the start of the
	 * bit(s) that encode this gene.
	 */
	virtual unsigned int geneToBitIndex( unsigned int gene ) const;
	/*!
	 * Convert the index of the given bit to the index of the gene that this
	 * bit is part of.
	 */
	virtual unsigned int bitToGeneIndex( unsigned int bit ) const;
protected:
	/*! num of Genes */
	unsigned int numgenes;
	/*! max absolute value */
	double maxv;
	/*! max absolute value */
	double minv;
	/*! bit precision */
	unsigned int bitprec;
};

} // end namespace salsa

#endif
