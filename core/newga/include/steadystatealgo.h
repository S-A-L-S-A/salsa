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

#ifndef STEADYSTATEALGO_H
#define STEADYSTATEALGO_H

#include "newgaconfig.h"
#include "parametersettable.h"
#include "evoalgo.h"
#include "genotype.h"
#include "genotypetester.h"
#include "robotexperiment.h"
#include "randomgenerator.h"
#include <cstdlib>
#include <QVector>
#include <Eigen/Core>

namespace farsa {

/**
 * \brief The steady state algorithm.
 *
 */
class FARSA_NEWGA_API SteadyStateAlgo : public EvoAlgo
{
public:
	/**
	 * \brief Constructor.
	 *
	 * It recovers parameters from the corrresponding group in the
	 * configuration.ini file. Furthermore, it initialises the parameters
	 * of the algorithm:
	 * - offspring population size
	 * - learning rates (for both the individual and the covariance matrix)
	 * - utility values (to be later assigned to the offspring depending on
	 *   their fitness scores)
	 *
	 * \param params the configuration parameters object with parameters to
	 *               use
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters. This is guaranteed to end with the
	 *               separator character when called by the factory, so you
	 *               don't need to add one
	 */
	SteadyStateAlgo(ConfigurationParameters& params, QString prefix);
	/**
	 * \brief Destructor
	 */
	~SteadyStateAlgo();

	static void describe(QString type);
	/**
	 * \brief Runs the evolutionary process
	 */
	void runEvolution();
	/**
	 * \brief Saves the fitness statistics
	 *
	 * It writes the statistics (maximum, average and minimum fitness)
	 * to the file associated to the output stream. Furthermore, it might
	 * save the fitness of all the individuals (if the corresponding parameter
	 * <saveFitnessAllIndividuals> is set to true in the configuration file).
	 *
	 * \param fit the array containing the fitness values
	 * \param out the output stream (i.e., where the fitness values will be saved)
	 */
	void saveFitStats(const QVector<float> fit, QTextStream& out);

private:
	/**
	 * \brief Randomises the population
	 */
	void randomisePopulation();
	/**
	 * \brief Mutate a value
	 *
	 * \param w the value to be mutated
	 * \param mut the mutation percentage
	 * \return the mutated value
	 */
	int mutate(int w, double mut);
	/**
	 * \brief Sorts the fitness array and returns the indices
	 *        of the individuals in the corresponding order
	 *
	 * \param fit the fitness array
	 * \return the array of indices
	 */
	QVector<int> sortFit(QVector<float> fit);

	/**
	 * \brief The function called when the flow controller changes
	 *
	 * The default implementation does nothing. You can use this function to
	 * propagate the flow controller to subcomponents
	 * \param flowController the new flow controller
	 */
	virtual void flowControllerChanged(FlowController* flowController)
	{
		if (m_gae != NULL)
		{
			m_gae->setFlowController(flowController);
		}
	}

	//! The number of genes (i.e., the length of the genotype)
	int m_numGenes;
	//! The number of individuals belonging to the population (population size)
	int m_popSize;
	//! The population
	QVector<Genotype*> m_population;
	//! Genotype prototype
	Genotype* m_prototype;
	//! The genotype tester
	SingleGenotypeIntToEvonet* m_gt;
	//! The evaluator
	RobotExperiment* m_gae;
	//! The number of generations
	int m_numGenerations;
	//! Mutation rate
	double m_mutRate;
	//! Mutation rate decay
	double m_mutDecay;
	//! Initial mutation rate
	double m_initMutRate;
	//! Flag whether or not the fitness of all individuals shall be saved
	bool m_saveFitnessAllIndividuals;
	//! Seed for extracting random numbers
	int m_rngSeed;
};

} // end namespace farsa

#endif
