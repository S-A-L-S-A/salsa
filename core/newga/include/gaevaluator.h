/*******************************************************************************
 * FARSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#ifndef GAEVALUATOR_H
#define GAEVALUATOR_H

#include "newgaconfig.h"
#include "experimentoutput.h"
#include "mathutils.h"

namespace farsa {

/**
 * \brief The interface for evaluators in ga experiments
 *
 * This inherits ExperimentOutput because it must also provide functions to
 * access the result of evaluation (e.g. fitness values). Generally speaking,
 * subclasses must also provide a means to set the genotypes to test. This means
 * that generally subclasses of this also inherit from an ExperimentInput
 * subclass.
 */
class FARSA_NEWGA_TEMPLATE GAEvaluator : public ExperimentOutput
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~GAEvaluator()
	{
	}

	/**
	 * \brief The function performing the evaluation
	 */
	virtual void evaluate() = 0;
};

/**
 * \brief The interface for evaluators returning a single fitness value
 */
class FARSA_NEWGA_TEMPLATE SingleFitnessGAEvaluator : public GAEvaluator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~SingleFitnessGAEvaluator()
	{
	}

	/**
	 * \brief Returns the computed fitness value
	 *
	 * \return the computed fitness value
	 */
	virtual real getFitness() = 0;
};

/**
 * \brief The interface for evaluators returning a vector of fitness values
 *
 * This is mostly used by multi-objective genetic algorithms and the whole
 * vector should refer to the same genotype. See
 * FitnessForMultipleGenotypesGAEvaluator if you need to return different
 * fitness values for different genotypes.
 */
class FARSA_NEWGA_TEMPLATE FitnessVectorGAEvaluator : public GAEvaluator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~FitnessVectorGAEvaluator()
	{
	}

	/**
	 * \brief Returns the computed vector of fitness values
	 *
	 * \return the computed vector of fitness values
	 */
	virtual QVector<real> getFitnessVector() = 0;
};

/**
 * \brief The interface for evaluators returning fitness values for a list of
 *        genotypes
 *
 * This should be used by algorithms that need to test multiple genotypes
 * together (e.g. when multiple genotypes are used to generate the various
 * individuals of a team). It is up to the implementer to make sure that the
 * index have the correct meaning (e.g. to ensure the the i-th value returned by
 * getFitnessForGenotype() is the fitness value for the i-th genotype passed to
 * the experiment). See FitnessVectorGAEvaluator if you need to return a vector
 * of fitness values for the same genotype (e.g. for multi-objective genetic
 * algorithms)
 */
class FARSA_NEWGA_TEMPLATE FitnessForMultipleGenotypesGAEvaluator : public GAEvaluator
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~FitnessForMultipleGenotypesGAEvaluator()
	{
	}

	/**
	 * \brief Returns the fitness for the given genotype
	 *
	 * \return the fitness for the given genotype
	 */
	virtual real getFitnessForGenotype(int genotypeIndex) = 0;
};

} // end namespace farsa

#endif
