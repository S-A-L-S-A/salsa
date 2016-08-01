/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
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

#ifndef SIMPLEGA_H
#define SIMPLEGA_H

#include "gaconfig.h"
#include "core/geneticalgo.h"
#include <QList>

namespace salsa {

class Evaluation;
class Reproduction;

/*! \brief SimpleGA
 *
 *  \par Description
 *   SimpleGA implements a Simple Genetic Algorithm, some features are:
 *    - you can customize Reproduction process using setReproduction
 *    - you can customize, of course, the Fitness function using setEvaluation
 *    - it use a multi-thread approach for parallel evaluation of Genotypes (see numThreads)
 *  \par Warnings
 *
 * \ingroup ga_gas
 */
class SALSA_GA_API SimpleGA : public GeneticAlgo {
public:
	/*! Constructor */
	SimpleGA();
	/*! Destructor */
	virtual ~SimpleGA();
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

	/*! \internal
	 *  This implement the code to be executed on each thread for parallel evaluation of Genotypes;<br>
	 *  Actual is used also when the thread is just one for simplicity
	 */
	class evaluationThread {
	public:
		//--- Constructor
		evaluationThread( SimpleGA* p, Evaluation* eProto );
		//--- Destructor
		~evaluationThread();
		//--- LaralGA parent
		SimpleGA* parent;
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
	static void runStepWrapper( SimpleGA::evaluationThread* e );
};

} // end namespace salsa

#endif
