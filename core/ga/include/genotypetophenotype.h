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

#ifndef GENOTYPETOPHENOTYPE_H
#define GENOTYPETOPHENOTYPE_H

#include "gaconfig.h"

namespace salsa {

/**
 * \brief The interface for classes preparing genotypes for evaluation
 *
 * This is empty and exists just as the root of a hierarchy. Specific genetic
 * algorithms will have different needs that should be addressed by specific
 * subclasses of this. For example different genetic algorithms may need to
 * evaluate one genotype at a time, multiple genotpes together, multiple
 * genotypes concurrently...
 */
class SALSA_GA_TEMPLATE GenotypeToPhenotype
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~GenotypeToPhenotype()
	{
	}
};

} // end namespace salsa

#endif
