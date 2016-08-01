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

#ifndef SIGNEDRANGEGENOTYPE_H
#define SIGNEDRANGEGENOTYPE_H

#include "gaconfig.h"
#include "genotypes/doublegenotype.h"

namespace salsa {

/*!  \brief SignedRangeGenotype class
 *
 *  \par Description
 *    A gene is a double values within the symmetric range specificed in the constructor; <br>
 *    Each gene contained by DoubleGenotype is a double value within the range [-max,+max]
 *    where the zero value has two representation: -0 and +0. <br>
 *    The binary is decoded using the first value as sign and the others as absolute value.
 *  \par Warnings
 *
 * \ingroup ga_genos
 */
class SALSA_GA_API SignedRangeGenotype : public DoubleGenotype {
public:
	/*! Construct a un-initialized SignedRangeGenotype <br>
	 *  \param numGenes is the number of double values contained by this SignedRangeGenotype
	 *  \param max is the maximum absolute value represented
	 *  \param bitPrecision is the number of bit used for represent a number
	 *  \warning you must explicity call randomize() to get a randomized Genotype
	 *  \note the Genotype::size() return the number of bits of underlying data, while numGenes is the actual
	 *    numbers of genes of this SignedRangeGenotype. They are related by the multiplication:
	 *    \f$ size = bitPrecision \cdot numGenes \f$
	 */
	SignedRangeGenotype( unsigned int numGenes = 0, double max = +1, unsigned int bitPrecision = 8 );
	/*! Destructor */
	virtual ~SignedRangeGenotype();
	/*! Copy-Constructor (generate a clone of the genoma) */
	SignedRangeGenotype( const SignedRangeGenotype& genotype );
	/*! Assign operator (performs same operation of Copy-constructor) */
	virtual SignedRangeGenotype& operator=( const Genotype& genotype );
	/*! Return the i-th genes represented */
	virtual double at( unsigned int i ) const;
	/*! Set the i-th genes */
	virtual void set( unsigned int i, double value );
	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with genotype parameters
	 */
	virtual void configure( ConfigurationParameters& params, QString prefix );
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 *
	 * \param params the configuration parameters object on which save actual parameters
	 * \param prefix the prefix to use to access the object configuration parameters.
	 */
	virtual void save( ConfigurationParameters& params, QString prefix );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! Create an exact copy of this Genotype */
	virtual SignedRangeGenotype* clone() const;
private:
	/*! dx */
	double dx;
};

} // end namespace salsa

#endif
