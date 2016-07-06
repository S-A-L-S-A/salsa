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

#ifndef SELECTION_H
#define SELECTION_H

#include "gaconfig.h"
#include "parametersettable.h"

namespace farsa {

class Genotype;
class Genome;
class GeneticAlgo;

/*!  \brief Selection Operator Interface
 *
 *  \par Description
 *    Interface of the Selection Operator
 *  \par Warnings
 *
 * \ingroup ga_core
 */
class FARSA_GA_TEMPLATE Selection : public ParameterSettableWithConfigureFunction {
public:
	/*! Constructor */
	Selection() {
		genomev = 0;
		ga = 0;
	};
	/*! Destructor */
	virtual ~Selection() {
		//--- nothing to do
	};
	/*! Select a Genotype */
	virtual const Genotype* select() = 0;
	/*! Set the Genome from which to select Genotypes
	 *  \note subclasses may re-implement this method to perform initialization
	 *   of the selection process
	 */
	virtual void setGenome( const Genome* g ) {
		genomev = g;
	};
	/*! Return the Genome on which it selects */
	const Genome* genome() const {
		return genomev;
	};
	/*! Set the GeneticAlgo used */
	void setGA( GeneticAlgo* g ) {
		ga = g;
	};
	/*! Return the GeneticAlgo used */
	GeneticAlgo* GA() {
		return ga;
	};

protected:
	/*! Genome on which it selects Genotypes */
	const Genome* genomev;
	/*! GeneticAlgo used */
	GeneticAlgo* ga;
};

} // end namespace farsa

#endif
