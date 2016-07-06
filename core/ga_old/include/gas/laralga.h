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

#ifndef FARSAGA_H
#define FARSAGA_H

#include "gaconfig.h"
#include "core/geneticalgo.h"
#include <QList>

namespace farsa {

class MultiTrials;
class Mutation;

/*! \brief Common FARSA setup of GA
 *
 *  \par Description
 *    This GA implements the common characteristics of FARSA setup:
 *    - Rank Selection with Truncation
 *    - Use MultiTrials mechanism for Evaluating the Fitness of a Genotype
 *    - Possibility to set one of Mutations availables (or a custom one)
 *    - No Crossover
 *  \par Warnings
 *
 * \ingroup ga_gas
 */
class FARSA_GA_API LaralGA : public GeneticAlgo {
public:
	/*! Constructor */
	LaralGA();
	/*! Destructor */
	virtual ~LaralGA();
	/*! Set the number of thread to use; or in other words the number of parallel
	 *  evaluation of Genotypes
	 */
	void setNumThreads( int numThreads );
	/*! Return the number of thread currently used */
	int numThreads() const;
	/*! Set the fitness function to use */
	void setFitnessFunction( MultiTrials* fitfunc );
	/*! Returns the Evaluation object used as prototype to eventually generate
	 *  other evaluators. This object could be or could be not used to compute
	 *  the fitness of genotypes (depending on the specific genetic algorithm)
	 */
	virtual Evaluation* evaluationPrototype();
	/*! Return the Evaluation Pool containing the Evaluations used */
	virtual QVector<Evaluation*> evaluationPool();
	/*! Return the fitness function */
	MultiTrials* fitnessFunction();
	/*! Set the Mutation operator to use */
	void setMutation( Mutation* mutate );
	/*! Return the Mutation operator */
	Mutation* mutation();
	/*! Set Reproducing parameters <br>
	 *  \note The number of offsprings are automatically calculated dividing nreproducinb by Genome::size()
	 *  \param nreproducing is the number of Genotypes to select
	 *  \param useElitism if true use elitism, othersiwe not
	 *  \param nelitism is the number of Genotypes selected to copy unthouched to next generation
	 */
	void setReproduceParams( int nreproducing, bool useElitism = false, int nelitism = 0 );
	/*! Return the number of selected Genotypes */
	int numReproducing();
	/*! Return the number of offspring for each selected Genotype */
	int numOffspring();
	/*! Return the number of elited Genotypes */
	int numElitism();
	/*! Return true if elitism is enabled */
	bool isElitismEnabled();
	/*! Initialize the Genetic Algorithm Process:
	 *  - set current replication to zero
	 *  - set current generation to zero
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
	 *               receive as prefix the group which contains all GA
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
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );

protected:
	/*! fitness function */
	MultiTrials* fitfunc;
	/*! mutation operator */
	Mutation* muta;
	/*! current Genotype in evaluation */
	unsigned int currGenotype;
	/*! number of selected genotypes */
	int nreproducing;
	/*! number of offspring */
	int noffspring;
	/*! number of elited genotypes */
	int nelitism;
	/*! true is elitism is enabled */
	bool elitismEnabled;
	/*! used for manage GA cycle */
	typedef enum { initEvaluation, evaluating, nextGeneration_pass1, nextGeneration_pass2, endEvolution } GAPhases;
	/*! current Phase */
	GAPhases currPhase;
	/*! True if LaralGA is initialized */
	bool isInitialized;
	/*! True if LaralGA is finalized */
	bool isFinalized;

private:
	/*! create next generation */
	void createNextGeneration();
	/*! True when all Genotypes has been evaluated */
	bool nextGeneration;

	/*! \internal
	 *  This implement the code to be executed on each thread for parallel evaluation of Genotypes;<br>
	 *  Actual is used also when the thread is just one for simplicity
	 */
	class evaluationThread {
	public:
		//--- Constructor
		evaluationThread( LaralGA* p, MultiTrials* eProto );
		//--- Destructor
		~evaluationThread();
		//--- LaralGA parent
		LaralGA* parent;
		//--- evaluator used by this object
		MultiTrials* eval;
		//--- evaluating genoma
		int id;
		//--- true when it cannot increment id because the end is reached
		bool blocked;
		//--- run a step of evaluation
		void runStep();
		//--- sequence of Genoma to evaluate
		QVector<int> sequence;
		//--- actual id inside sequence in evaluating
		int idSeq;
	};

	/*! List of Evaluation Threads */
	QList<evaluationThread*> evalThreads;
	/*! Number of Thread used */
	int numThreadv;
	/*! Static wrapper function for Parallel evaluations */
	static void runStepWrapper( LaralGA::evaluationThread* e );
};

} // end namespace farsa

#endif
