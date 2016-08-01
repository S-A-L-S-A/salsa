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

#ifndef EVOALGO_H
#define EVOALGO_H

#include "gaconfig.h"
#include "component.h"
#include "randomgenerator.h"
#include "flowcontrol.h"
#include <memory>

namespace salsa {

class GenotypeToPhenotype;
class GAEvaluator;

/**
 * \brief The base class for all genetic algorithms
 *
 * This is the base class for all genetic algorithms. This class contains all
 * the pieces every GA must have, but implements no logic. In particular this
 * class contains:
 * 	- a private random number generator, that should be used when
 * 	  implementing a ga;
 * 	- a pointer to an instance of GenotypesPreparer. This is here to force
 * 	  all gas to use one, but subclasses should keep a pointer to a more
 * 	  specific type depending on the ga needs. The GenotypesPreparer
 * 	  instance is destroyed by this class;
 * 	- a pointer to an instance of GAEvaluator. This is here to force all gas
 * 	  to use one, but subclasses should keep a pointer to a more specific
 * 	  type depending on the ga needs. The GAEvaluator instance is destroyed
 * 	  by this class.
 * Subclasses are expected to reimplement the runEvolution method. This is a
 * blocking method that performs one evolution. When implementing the method,
 * use methods of FlowControlled so that the execution can be slowed down or
 * stopped by external threads. The resumeEvolution() function can also be
 * implemented for gas that can resume evolution after a stop. The default
 * implementation provided here simply returns false (see function description
 * for more information)
 *
 * The configuration parameters of this component are:
 * 	- seed: the random number generator seed to use. This is not mandatory
 * 	        and can also be set programmatically using the setSeed function;
 * 	- genotypesPreparer: the group with the component that implements the
 * 	                     GenotypesPreparer interface. This parameter is
 * 	                     mandatory;
 * 	- gaEvaluator: the group with the component that implements the
 * 	               GAEvaluator interface. This parameter is mandatory.
 */
class SALSA_GA_API EvoAlgo : public Component, public FlowControlled
{
public:
	/**
	 * \brief This class configures in the configure function
	 *
	 * \return always false
	 */
	static bool configuresInConstructor()
	{
		return false;
	}

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object storing the
	 *               parameters
	 */
	EvoAlgo(ConfigurationManager& params);

	/**
	 * \brief Destructor
	 */
	virtual ~EvoAlgo();

	/**
	 * \brief Configures the object using the ConfigurationManager
	 *
	 */
	virtual void configure();

	/**
	 * \brief Add to Factory::typeDescriptions() the descriptions of all
	 *        parameters and subgroups
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Sets the seed of the random number generator used by this ga
	 *
	 * \param seed the seed to use
	 */
	void setSeed(unsigned int seed);

	/**
	 * \brief Returns the seed of the random number generator
	 *
	 * \return the seed of the random number generator
	 */
	unsigned int seed() const;

	/**
	 * \brief The function running the evolutionary process
	 *
	 * This function should run the evolutionary process and return when the
	 * process has ended. To allow external processes to pause or stop
	 * earlier the process, frequently call the FlowControlled::pauseFlow
	 * and FlowControlled::stopFlow functions
	 */
	virtual void runEvolution() = 0;

	/**
	 * \brief The function to resume an interrupted evolution
	 *
	 * This function should try to resume an interrupted evolution. Call
	 * this before runEvolution(): if this function returns true an
	 * interrupted evolution was found and the subsequent call to
	 * runEvolution() shall continue that evolution process; if this
	 * function returns false there is no evolution process to resume and
	 * the subsequent call to runEvolution will start a new evolution
	 * process. The default implementation always returns false. If there
	 * is an error in the resume process an exception of type
	 * CannotResumeEvolutionException should be thrown
	 * \return true if an evolution process has been resumed, false
	 *         otherwise
	 */
	virtual bool resumeEvolution();

protected:
	/**
	 * \brief Returns the object preparing genotypes for evaluation
	 *
	 * \return a pointer to the object preparing genotypes for evaluation
	 */
	GenotypeToPhenotype* genotypeToPhenotype()
	{
		return m_genotypeToPhenotype.get();
	}

	/**
	 * \brief Returns the object preparing genotypes for evaluation (const
	 *        version)
	 *
	 * \return a const pointer to the object preparing genotypes for
	 *         evaluation
	 */
	const GenotypeToPhenotype* genotypeToPhenotype() const
	{
		return m_genotypeToPhenotype.get();
	}

	/**
	 * \brief Returns the object performing the evaluation of the currently
	 *        prepared genotypes
	 *
	 * \return a pointer to the object performing the evaluation of the
	 *         currently prepared genotypes
	 */
	GAEvaluator* gaEvaluator()
	{
		return m_gaEvaluator.get();
	}

	/**
	 * \brief Returns the object performing the evaluation of the currently
	 *        prepared genotypes (const version)
	 *
	 * \return a const pointer to the object performing the evaluation of
	 *         the currently prepared genotypes
	 */
	const GAEvaluator* gaEvaluator() const
	{
		return m_gaEvaluator.get();
	}

	/**
	 * \brief Returns the random number generator to use in this ga
	 */
	RandomGenerator& rng()
	{
		return m_rng;
	}

	/**
	 * \brief The function called when the seed changes
	 *
	 * The new seed can be obtained using the seed() functions. The default
	 * implementation does nothing
	 * \param oldSeed the previous value of the seed
	 */
	virtual void seedChanged(unsigned int oldSeed);

private:
	/**
	 * \brief The random number generator to use
	 */
	RandomGenerator m_rng;

	/**
	 * \brief The object preparing genotypes for evaluation
	 */
	std::auto_ptr<GenotypeToPhenotype> m_genotypeToPhenotype;

	/**
	 * \brief The object performing the evaluation of the currently prepared
	 *        genotypes
	 */
	std::auto_ptr<GAEvaluator> m_gaEvaluator;
};

} // end namespace salsa

#endif
