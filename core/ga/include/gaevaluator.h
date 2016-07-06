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

#ifndef GAEVALUATOR_H
#define GAEVALUATOR_H

#include "gaconfig.h"

namespace farsa {

/**
 * \brief The interface of evaluators
 *
 * This is the interface all evaluators must implement. A call to evaluate()
 * should perform the evaluation of the genotype(s) that have been prepared by
 * the GenotypesPreparer object. Specific genetic algorithms will require more
 * specific evaluators. For example here there is no hypothesis on which is the
 * result of evaluation (e.g. one fitness value, one fitness value per each
 * evaluated genotype, a single fitness value for a set of genotypes, multiple
 * fitness values for each genotype...).
 */
class FARSA_GA_TEMPLATE GAEvaluator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~GAEvaluator()
	{
	}

	/**
	 * \brief Performs the evaluation of the current genotype(s)
	 *
	 * This must evaluate the last genotype(s) that have been prepared by
	 * the GenotypesPreparer object
	 */
	virtual void evaluate() = 0;
};

} // end namespace farsa

#endif
