/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef NSGA2_H
#define NSGA2_H

#include "gaconfig.h"
#include "core/geneticalgo.h"
#include "core/genotype.h"
#include "core/genome.h"
#include <QList>

namespace farsa {

class Evaluation;
class Reproduction;

/*! \brief NSGA-II 
 *
 *  \par Description
 *   NSGA-II implements the Nondominated Sorting (multi-objective) Genetic Algorithm version 2
 *    - it use a multi-thread approach for parallel evaluation of Genotypes (see numThreads)
 *  \par Warnings
 *
 * \ingroup ga_gas
 */
class FARSA_GA_API NSGA2 : public GeneticAlgo {
public:
	/*! Constructor */
	NSGA2();
	/*! Destructor */
	virtual ~NSGA2();
	/*! Set the number of thread to use; or in other words the number of parallel
	 *  evaluation of Genotypes
	 */
	void setNumThreads( int numThreads );
	/*! Return the number of thread currently used */
	int numThreads() const;
	/*! Set the fitness function to use */
	void setEvaluation( Evaluation* fitfunc );
	/*! Returns the Evaluation object used as prototype to eventually generate
	 *  other evaluators. This object could be or could be not used to compute
	 *  the fitness of genotypes (depending on the specific genetic algorithm)
	 */
	virtual Evaluation* evaluationPrototype();
	/*! Return the Evaluation Pool containing the Evaluations used */
	virtual QVector<Evaluation*> evaluationPool();
	/*! Set the Reproduction operator to use */
	void setReproduction( Reproduction* reproduct );
	/*! Return the Reproduction operator */
	Reproduction* reproduction();
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
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );

protected:
	/*! fitness function */
	Evaluation* fitfunc;
	/*! reproduction operator */
	Reproduction* reprod;
	/*! used for manage GA cycle */
	typedef enum { initEvaluation, evaluating, nextGeneration_pass1, nextGeneration_pass2, endEvolution } GAPhases;
	/*! current Phase */
	GAPhases currPhase;
	/*! True if SimpleGA is initialized */
	bool isInitialized;
	/*! True if SimpleGA is finalized */
	bool isFinalized;

private:
	/*! True when all Genotypes has been evaluated */
	bool nextGeneration;
	
	/*! For implementing NSGA-II each genotype needs three more attributes
	 *  necessary for the crowding distance calculation and sorting
	 *  This class is a simple holder for such attributes
	 */
	class nsgaGenotype {
	public:
		nsgaGenotype( Genotype* g=NULL, int rank=0, double distance=0 ) {
			genotype = g;
			this->rank = rank;
			this->distance = distance;
		};
		//! The genotype
		Genotype* genotype;
		//! the pareto-front on which this genotype belongs to; Rank==0 is the top pareto-front
		int rank;
		//! the crowding distance of this genotype regarding the pareto-front on which it belongs to
		double distance;
		//! domination counter: how many genotypes dominate this
		int dominationCounter;
		//! operator needed by QMap for using nsgaGenotype has keys
		bool operator<( const nsgaGenotype& g ) const {
			return this->distance < g.distance;
		};
		//! operator== is true if they point to the same Genotype
		bool operator==( const nsgaGenotype& g ) const {
			return this->genotype == g.genotype;
		};
	};
	typedef QVector<nsgaGenotype*> nsgaGenome;
	/*! Last Pareto-fronts */
	Genome lastPareto;
	/*! Calculate the Crowding Distance */
	void crowdingDistanceAssignment( nsgaGenome& genome );
	/*! Calculate the pareto-fronts using the Fast NonDominated Sort */
	QVector<nsgaGenome> fastNonDominatedSort( nsgaGenome& pareto );
	/*! Utility function for comparing elements by crowding distance */
	static bool crowdingDistanceGreaterThan( const nsgaGenotype* g1, const nsgaGenotype* g2 ) {
		return g1->distance > g2->distance;
	};
	/*! Utility class for generating a function for comparing genotype of an objective */
	class nObjectiveGreaterThan {
	public:
		bool operator()( const nsgaGenotype* g1, const nsgaGenotype* g2 ) {
			return g1->genotype->objective( currentObjective ) > g2->genotype->objective( currentObjective );
		};
		int currentObjective;	
	};

	/*! \internal
	 *  This implement the code to be executed on each thread for parallel evaluation of Genotypes;<br>
	 *  Actual is used also when the thread is just one for simplicity
	 */
	class evaluationThread {
	public:
		//--- Constructor
		evaluationThread( NSGA2* p, Evaluation* eProto );
		//--- Destructor
		~evaluationThread();
		//--- LaralGA parent
		NSGA2* parent;
		//--- evaluator used by this object
		Evaluation* eval;
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
	static void runStepWrapper( NSGA2::evaluationThread* e );
};

} // end namespace farsa

#endif
