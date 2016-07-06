/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2009 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef STEFANOSTEADYSTATEGA_H
#define STEFANOSTEADYSTATEGA_H

#include "gaconfig.h"
#include "core/geneticalgo.h"
#include "core/mutation.h"
#include <QList>

namespace farsa {

class Evaluation;
class Reproduction;

/*! \brief StefanoSteadyStateGA
 *
 *  \par Description
 *    ADD DESCRIPTION HERE
 *  \par Warnings
 *
 * \ingroup ga_gas
 */
class FARSA_GA_API StefanoSteadyStateGA : public GeneticAlgo {
public:
	/*! Constructor */
	StefanoSteadyStateGA();
	/*! Destructor */
	virtual ~StefanoSteadyStateGA();

	/*! Set the fitness function to use */
	void setEvaluation( Evaluation* fitfunc );
	/*! Returns the Evaluation object used as prototype to eventually generate
	 *  other evaluators. This object could be or could be not used to compute
	 *  the fitness of genotypes (depending on the specific genetic algorithm)
	 */
	virtual Evaluation* evaluationPrototype();
	/*! Return the Evaluation Pool containing the Evaluations used */
	virtual QVector<Evaluation*> evaluationPool();
	/*! Set the Mutation operator to use */
	void setMutation( Mutation* mutation );
	/*! Return the Mutation operator */
	Mutation* mutation();
	/*! Initializes the Genetic Algorithm Process and sets current generation to zero
	 */
	virtual void initialize();
	/*! Execute a single minor step and return */
	virtual void gaStep();
	/*! Finalize the Genetic Algorithm Process */
	virtual void finalize();

	/*! Skip the evaluation phase and put the algorithm in the state just after
	 *  the evaluation. This can be used after restarting an interrupted evolution
	 *  to prevent re-evaluation of the genome that was loaded from file.
	 */
	virtual void skipEvaluation();

	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the groupwhich contains all GA
	 *               parameters
	 */
	virtual void configure( ConfigurationParameters& params, QString prefix );
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 *
	 * \param params the configuration parameters object on which save actual parameters
	 * \param prefix the prefix to use to access the object configuration parameters.
	 */
	virtual void save( ConfigurationParameters& params, QString prefix );
	/*! Describe the parameters and subgroups necessary to configure it */
	static void describe( QString );

protected:
	/*! fitness function */
	Evaluation* fitfunc;
	/*! mutation operator */
	Mutation* muta;
	/*! used to manage GA cycle */
	typedef enum {initParent, evalParent, initOffspring, evalOffspring, compareOffspring, nextGeneration, endEvolution} GAPhases;
	/*! current Phase */
	GAPhases curPhase;
	/*! the current genotype */
	unsigned int curGenotype;
	/*! True if StefanoSteadyStateGA is initialized */
	bool isInitialized;
	/*! True if StefanoSteadyStateGA is finalized */
	bool isFinalized;
	/*! The number of times each genotype has been evaluated */
	QVector<unsigned int> numEvaluations;
	/*! The cumulated value of fitness so far (i.e. the sum of all fitness
	    values obtained in subsequent evaluations of the same genotype) */
	QVector<double> cumulatedFitness;
	/*! The offspring of the current genotype */
	Genotype *offspring;
};

} // end namespace farsa

#endif
