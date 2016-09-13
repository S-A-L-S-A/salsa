/*******************************************************************************
 * SALSA New Genetic Algorithm Library                                        *
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

#ifndef XNESALGO_H
#define XNESALGO_H

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

namespace salsa {

/**
 * \brief The xNES algorithm.
 *
 * The algorithm is different from a classical evolutionary algorithm since
 * it does not work on population of individuals: indeed, there is only one
 * candidate solution. A population of offspring is then derived by mutating
 * the genes of the candidate solution. Offspring are evaluated (their fitnesses
 * are computed) and ranked by utility (the fitter an offspring is, the higher
 * utility values it achieves). The algorithm uses the natural gradient
 * in order to update the candidate solution. Both the candidate solution and
 * the covariance matrix are updated. For further information, see
 * "Exponential Natural Evolution Strategies", Glasmachers et al. (2010)
 *
 * \note To avoid misunderstandings, we refer to the candidate solution as individual.
 *       Moreover, we call samples the modifications that generate offspring from the
 *       individual (i.e., offspring = individual + samples).
 * \note The covariance matrix provides the user with the information about how genes
 *       correlate with each other.
 */
class SALSA_NEWGA_API XnesAlgo : public EvoAlgo
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
	XnesAlgo(ConfigurationParameters& params, QString prefix);
	/**
	 * \brief Destructor
	 */
	~XnesAlgo();

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

protected:
	/**
	 * \brief Initialise the individual and the covariance matrix.
	 *
	 * The individual genes are drawn from a Gaussian distribution
	 * with mean 0.0 and standard deviation 1.0.
	 * The covaraiance matrix is filled with null values.
	 */
	void initialise();
	/**
	 * \brief Extract the offspring.
	 *
	 * The offspring are varied copies of the individual.
	 * In particular, the modifications are drawn from a Gaussian
	 * distribution with mean 0.0 and standard deviation 0.5 and
	 * are "weighted" by means of the exponential of the covariance
	 * matrix.
	 *
	 * \param expCovMat the exponential of the covariance matrix
	 */
	void extractOffspring(const Eigen::MatrixXf expCovMat);
	/**
	 * \brief Sort fitness.
	 *
	 * Starting from the fitness values of the offspring, the method
	 * returns the array of indices of the offspring (the array is passed
	 * as parameter to the function).
	 *
	 * \param fitness the fitness array
	 * \param offspringIdx the array of offspring (sorted by fitness)
	 */
	void sortFitness(const QVector<float> fitness, QVector<int>& offspringIdx);
	/**
	 * \brief Utility ranking.
	 *
	 * It assigns the utility values to the offspring according to their
	 * ranking (the fittest offspring will receive the highest utility value).
	 *
	 * \param offspringIdx the array of offspring (sorted by fitness)
	 * \return the array of sorted utilities (i.e., the highest utility value
	 *         is assigned to the fittest offspring)
	 */
	QVector<float> computeUtilityRanking(const QVector<int> offspringIdx);
	/**
	 * \brief Compute updates
	 *
	 * This method computes the natural gradients for both the individual
	 * and the covariance matrix. These gradients will be later used in order
	 * to update the individual and the covariance matrix.
	 *
	 * \param dCovMat tbe update of the covariance matrix
	 * \param utilityRank the utility ranking array
	 * \param expCovMat the exponential of the covariance matrix
	 */
	void computeUpdates(Eigen::MatrixXf& dCovMat, const QVector<float> utilityRank, const Eigen::MatrixXf expCovMat);
	/**
	 * \brief Update individual and covariance matrix
	 *
	 * \param dInd the update of the individual
	 * \param dCovMat the update of the covariance matrix
	 */
	void update(Genotype* dInd, const Eigen::MatrixXf dCovMat);

	/**
	 * \brief The function called when the flow controller changes
	 *
	 * The default implementation does nothing. You can use this function to
	 * propagate the flow controller to subcomponents
	 * \param flowController the new flow controller
	 */
	virtual void flowControllerChanged(FlowController* flowController)
	{
		if (m_gae != nullptr)
		{
			m_gae->setFlowController(flowController);
		}
	}

	//! The number of genes (i.e., the length of the genotype)
	int m_numGenes;
	//! The number of offspring (i.e., the offspring population size)
	int m_numOffspring;
	//! The offspring population
	QVector<Genotype*> m_offspring;
	//! The individual
	Genotype* m_individual;
	//! The update of the individual
	Genotype* m_dInd;
	//! Genotype prototype
	Genotype* m_prototype;
	//! The covariance matrix
	Eigen::MatrixXf m_covMat;
	//! The learning rate for the individual
	float m_indLrate;
	//! The learning rate for the covariance matrix
	float m_covMatLrate;
	//! The utility values
	QVector<float> m_utility;
	//! The samples
	Eigen::MatrixXf m_samples;
	//! The genotype tester
	SingleGenotypeFloatToEvonet* m_gt;
	//! The evaluator
	RobotExperiment* m_gae;
	//! The number of generations
	int m_numGenerations;
	//! Flag whether or not the fitness of all individuals shall be saved
	bool m_saveFitnessAllIndividuals;
	//! Seed for extracting random numbers
	int m_rngSeed;
};

} // end namespace salsa

#endif
