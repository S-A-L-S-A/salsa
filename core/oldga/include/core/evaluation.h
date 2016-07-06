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

#ifndef EVALUATION_H
#define EVALUATION_H

#include "gaconfig.h"
#include "parametersettable.h"

namespace farsa {

class Genotype;
class Genome;
class GeneticAlgo;

/*!  \brief Evaluation Process Interface
 *
 *    Interface of the Evaluation Process
 *    Evalution Process consist of one or more steps for calculating the fitness of Genotype
 *    The Evaluation Process follow this schema:
 *    - call initialize with the genotype you want to test
 *    - while( ! isEvaluationDone() ) call evaluateStep()
 *    - call finalize
 *    - query the fitness with Genotype::fitness() method
 *
 * \ingroup ga_core
 */
class FARSA_GA_API Evaluation : public ParameterSettableWithConfigureFunction {
public:
	/*! Default Constructor */
	Evaluation();
	/*! Destructor */
	virtual ~Evaluation();
	/*! Initialize the Evaluation Process
	 *  \param genotypeToEvaluate is the Genotype to evaluate
	 */
	void initialize( Genotype* genotypeToEvaluate );
	/*! Execute a single minor step and return
	 *  \warning if you forget to initialize the Evaluation, this method will return a Fatal error
	 *   blocking the execution of the application
	 */
	void evaluateStep();
	/*! It evaluate the Genotype in a single-shot calling as many times as necessary evaluateStep
	 *  \warning this is a <b>blocking</b> method
	 */
	void evaluate();
	/*! Called at the beginning of each generation
	 *
	 *  The default implementation is empty
	 */
	virtual void initGeneration(int /*generation*/) { };
	/*! Called at the end of each generation
	 *
	 *  The default implementation is empty
	 */
	virtual void endGeneration(int /*generation*/) { };
	/*! Finalize the Evaluation Process */
	void finalize();
	/*! return true if the Evaluation Process has finish */
	bool isEvaluationDone();
	/*! return the Genotype to Evaluate */
	Genotype* genotype() {
		return genotypev;
	};
	/*! Return the index of the Genotype into the Genome */
	unsigned int genotypeID() {
		return genotypeid;
	};
	/*! Set the Genome where Genotypes lives */
	void setGenome( Genome* g ) {
		genomev = g;
	};
	/*! Return the Genome */
	Genome* genome() {
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
	/*! Return the GeneticAlgo used (const version) */
	const GeneticAlgo* GA() const {
		return ga;
	};

	/*! Create an exact copy of Evaluation */
	virtual Evaluation* clone() const = 0;

protected:
	/*! Re-implemented by Users for fit to own experiments */
	virtual void init() = 0;
	/*! Re-implemented by Users for fit to own experiments */
	virtual void step() = 0;
	/*! Re-implemented by Users for fit to own experiments */
	virtual void fini() = 0;

	/*! Called by developers for signaling the end of Evaluation Process
	 *  \warning this does not automatically calls finalize()
	 */
	void evaluationDone();

private:
	/*! true if evaluation is done */
	bool isDone;
	/*! true if evaluation is initialized */
	bool isInitialized;
	/*! true if evaluation is finalized */
	bool isFinalized;
	/*! Genotype to evaluate */
	Genotype* genotypev;
	/*! The index of the Genotype inside the Genome */
	unsigned int genotypeid;
	/*! Genome where Genotypes lives */
	Genome* genomev;
	/*! GeneticAlgo used */
	GeneticAlgo* ga;
};

} // end namespace farsa

#endif
