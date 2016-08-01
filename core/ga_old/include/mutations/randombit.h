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

#ifndef RANDOMBIT_H
#define RANDOMBIT_H

#include "gaconfig.h"
#include "core/mutation.h"

namespace salsa {

/*!  \brief RandomBit Mutation
 *
 *  \par Description
 *    Change the i-th bit with a random one with mutationRate probability.<br>
 *    The new bit is extracted with an equal probability for zero and one.<br>
 *    This differ from FlipBit for at least two points:
 *    - it requires the double of random access (extractions)
 *    - if a mutation occur, it remains a 50% probability that bit remain the same
 *  \par Warnings
 *
 * \ingroup ga_muta
 */
class SALSA_GA_API RandomBit : public Mutation {
public:
	/*! Constructor */
	RandomBit();
	/*! Destructor */
	virtual ~RandomBit();
	/*! Mutate the Genotype */
	virtual void mutate( Genotype* );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
};

} // end namespace salsa

#endif
