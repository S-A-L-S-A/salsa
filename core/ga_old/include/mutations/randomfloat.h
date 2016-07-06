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

#ifndef RANDOMFLOAT_H
#define RANDOMFLOAT_H

#include "gaconfig.h"
#include "core/mutation.h"

namespace farsa {

/*!  \brief RandomFloat Mutation
 *
 *  \par Description
 *    Change the i-th float number with a new random value within range specified with mutationRate probability.
 *  \par Warnings
 *
 * \ingroup ga_muta
 */
class FARSA_GA_API RandomFloat : public Mutation {
public:
	/*! Constructor */
	RandomFloat();
	/*! Destructor */
	virtual ~RandomFloat();
	/*! Mutate the Genotype
	 *  \warning the Genotype should be a valid RealGenotype
	 */
	virtual void mutate( Genotype* );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
};

} // end namespace farsa

#endif
