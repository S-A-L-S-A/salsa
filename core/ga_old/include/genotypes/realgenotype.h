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

#ifndef REALGENOTYPE_H
#define REALGENOTYPE_H

#include "gaconfig.h"
#include "genotypes/doublegenotype.h"
#include <cfloat>

namespace farsa {

/*!  \brief RealGenotype class
 *
 *  \par Description
 *    A gene is a true float value
 *  \par Warnings
 *
 * \ingroup ga_genos
 */
class FARSA_GA_API RealGenotype : public DoubleGenotype {
public:
	/*! Construct a un-initialized RealGenotype <br>
	 *  \param numGenes is the number of float values contained by this RealGenotype
	 *  \param min is the minimum float number allowed
	 *  \param max is the maximum float number allowed
	 *  \warning you must explicity call randomize() to get a randomized Genotype
	 *  \note the Genotype::size() return the number of bits of underlying data, while numGenes is the actual
	 *    numbers of genes of this RealGenotype. They are related by the multiplication:
	 *    \f$ size = sizeof(float) \cdot numGenes \f$
	 */
	RealGenotype( unsigned int numGenes = 0, float min = FLT_MIN, float max = FLT_MAX );
	/*! Destructor */
	virtual ~RealGenotype();
	/*! Copy-Constructor (generate a clone of the genoma) */
	RealGenotype( const RealGenotype& genotype );
	/*! Assign operator (performs same operation of Copy-constructor) */
	virtual RealGenotype& operator=( const Genotype& genotype );
	/*! Return the i-th genes represented */
	virtual double at( unsigned int i ) const;
	/*! Set the i-th genes */
	virtual void set( unsigned int i, double value );
	/*! Randomize the value contained (attention, this method destroys previous data) */
	virtual void randomize();
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
	virtual RealGenotype* clone() const;
private:
	/*! Genes data viewed as a float array*/
	float* genes;
};

} // end namespace farsa

#endif
