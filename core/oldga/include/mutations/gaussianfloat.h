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

#ifndef GAUSSIANFLOAT_H
#define GAUSSIANFLOAT_H

#include "gaconfig.h"
#include "core/mutation.h"

namespace salsa {

/*!  \brief GaussianFloat Mutation
 *
 *  \par Description
 *    Change the i-th float number with a new random value according to a Gaussian distribution centered
 *    over the current value of the gene
 *  \par Warnings
 *
 * \ingroup ga_muta
 */
class SALSA_GA_API GaussianFloat : public Mutation {
public:
	/*! Constructor */
	GaussianFloat();
	/*! Destructor */
	virtual ~GaussianFloat();
	/*! Mutate the Genotype
	 *  \warning the Genotype should be a valid RealGenotype
	 */
	virtual void mutate( Genotype* );
	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with mutation parameters
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
	/**
	 * \brief Sets the variance of the gaussian distribution
	 *
	 * \param v the new variance
	 */
	void setVariance( double v )
	{
		varg = v;
	}
	/**
	 * \brief Returns the variance of the gaussian distribution
	 *
	 * \return the variance of the gaussian distribution
	 */
	double variance() const
	{
		return varg;
	}
private:
	/*! variance of Gaussian distribution */
	double varg;
};

} // end namespace salsa

#endif
