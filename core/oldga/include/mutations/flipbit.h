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

#ifndef FLIPBIT_H
#define FLIPBIT_H

#include "gaconfig.h"
#include "core/mutation.h"

namespace salsa {

/*!  \brief FlipBit Mutation
 *
 *  \par Description
 *    Flip the bits with probability given by mutationRate
 *  \par Warnings
 *
 * \ingroup ga_muta
 */
class SALSA_GA_API FlipBit : public Mutation {
public:
	/*! Constructor */
	FlipBit();
	/*! Destructor */
	virtual ~FlipBit();
	/*! Mutate the Genotype */
	virtual void mutate( Genotype* );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
};

} // end namespace salsa

#endif
