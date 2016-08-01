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

#ifndef ONEPOINT_H
#define ONEPOINT_H

#include "gaconfig.h"
#include "core/crossover.h"

namespace salsa {

/*!  \brief One-Point Crossover for Genotype
 *
 *  \par Description
 *   OnePoint crossover select a random bit as split-point and replace the part
 *   of the father beyond split-point with the corresponding part of the mother:
 *  <pre>
 *                Split-point
 *  +-----------------+------------+
 *  \ 011010110101101 \ 1101101010 \    <= Father
 *  +-----------------+------------+
 *  +-----------------+------------+
 *  \ *************** \ 0001010101 \    <= Mother
 *  +-----------------+------------+
 *  +-----------------+------------+
 *  \ 011010110101101 \ 0001010101 \    <= Child
 *  +-----------------+------------+
 *  </pre>
 *  \par Warnings
 *
 * \ingroup ga_cross
 */
class SALSA_GA_API OnePoint : public Crossover {
public:
	/*! Constructor */
	OnePoint();
	/*! Destructor */
	virtual ~OnePoint();

	/*! Crossover two Genotypes and return the father modified
	 *  \note the mother will not changed, while the father will be replaced by new one
	 */
	virtual void crossover( Genotype* father, const Genotype* mother );

	/*! NOT USED */
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

};

} // end namespace salsa

#endif
