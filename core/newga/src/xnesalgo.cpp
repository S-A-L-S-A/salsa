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

#include "xnesalgo.h"
#include "evodataviewer.h"
#include "configurationhelper.h"
#include <QTextStream>
#include <QFile>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>

namespace farsa {

XnesAlgo::XnesAlgo(ConfigurationParameters& params, QString prefix)
	: EvoAlgo(params, prefix)
	, m_numGenes(1)
	, m_numOffspring(1)
	, m_offspring()
	, m_individual(NULL)
	, m_dInd(NULL)
	, m_prototype(NULL)
	, m_covMat()
	, m_indLrate(1.0)
	, m_covMatLrate(1.0)
	, m_utility()
	, m_samples()
	, m_gt(NULL)
	, m_gae(NULL)
	, m_numGenerations(1)
	, m_saveFitnessAllIndividuals(false)
	, m_rngSeed(0)
{
	// Get parameters' values from the configuration file (.ini), if some of these parameters
	// are not in the file, the default values will be used
	m_gt = params.getObjectFromParameter<SingleGenotypeFloatToEvonet>(prefix + QString("genotypeTester"));
	m_gae = params.getObjectFromParameter<RobotExperiment>(prefix + QString("gaEvaluator"));
	m_numGenerations = ConfigurationHelper::getInt(params, prefix + QString("numGenerations"), 1000);
	m_saveFitnessAllIndividuals = ConfigurationHelper::getBool(params, prefix + QString("saveFitnessAllIndividuals"), false);
	m_numGenes = m_gt->requestedGenotypeLength();
	// Compute the number of offspring
	const float nGenes = (const float)m_numGenes;
	float numOff = 4.0 + floor(3.0 * log(nGenes));
	m_numOffspring = (int)numOff;
	m_utility.resize(m_numOffspring);
	// Compute the learning rates
	m_indLrate = 1.0;
	m_covMatLrate = 0.25;
	if (m_covMatLrate > (1.0 / nGenes))
	{
		m_covMatLrate = (1.0 / nGenes);
	}
	m_covMatLrate *= 0.5;
	// Initialise the utility vector
	float utilitySum = 0.0;
	float tmpUtility;
	for (int i = 0; i < m_numOffspring; i++)
	{
		tmpUtility = log(numOff / 2.0 + 1.0) - log((float)i + 1.0);
		// Utility must not be negative
		if (tmpUtility < 0.0)
		{
			tmpUtility = 0.0;
		}
		m_utility[i] = tmpUtility;
		utilitySum += tmpUtility;
	}
	// Normalise utility vector so that the sum of the elements is equal to 1.0
	for (int i = 0; i < m_numOffspring; i++)
	{
		if (utilitySum != 0.0) // It is a useless check, only for safety purposes
		{
			m_utility[i] /= utilitySum;
		}
	}
	m_prototype = params.getObjectFromGroup<Genotype>( prefix + "Genotype" );
	GenotypeInt* g = dynamic_cast<GenotypeInt*>(m_prototype);
	g->setLength(m_numGenes);
	m_individual = m_prototype->clone();
	m_dInd = m_prototype->clone();
	m_offspring.resize(m_numOffspring);
	for (int i = 0; i < m_numOffspring; i++)
	{
		m_offspring[i] = m_prototype->clone();
	}
	m_covMat.resize(m_numGenes, m_numGenes);
	m_samples.resize(m_numGenes, m_numOffspring);
}

XnesAlgo::~XnesAlgo()
{
	// Delete all objects (i.e., pointers) in order to free memory.
	delete m_gt;
	delete m_gae;
	delete m_individual;
	delete m_dInd;
	for (int i = 0; i < m_numOffspring; i++)
	{
		delete m_offspring[i];
	}
}

void XnesAlgo::describe(QString type)
{
	Descriptor d = addTypeDescription(type, "xNES Algorithm");
	d.describeInt("numGenerations").limits(1, INT_MAX).def(1000).help("Number of the generations of the evolutionary process");
	d.describeObject("gaEvaluator").type("RobotExperiment").props(IsMandatory).help("Object that calculate the fitness");
	d.describeObject("genotypeTester").type("SingleGenotypeFloatToEvonet").props(IsMandatory).help("Object that sets the genotype to be tested");
	d.describeSubgroup("Genotype").type("GenotypeFloat").props(IsMandatory).help("Object containing the individual under evolution");
}

void XnesAlgo::runEvolution()
{
	Eigen::MatrixXf expCovMat(m_numGenes,m_numGenes);
	int startGeneration = 0;
	char statfile[64];
	char inFilename[128];
	char outFilename[128];
	char bestOutFilename[128];
	int bestGeneration = -1;
	int bestFitness = -1;
	// Set the seed
	setSeed(m_rngSeed);
	// Initialise the individual and the covariance matrix
	initialise();
	// Check whether to recover a previous evolution
	sprintf(statfile,"S%d.fit", seed());
	// Check if the file exists
	DataChunk statTest(QString("stattest"),Qt::blue,2000,false);
	if (statTest.loadRawData(QString(statfile),0))
	{
		startGeneration = statTest.getIndex();
		sprintf(inFilename, "S%dG%d.gen", (seed()), startGeneration);
		Logger::info("Recovering from startGeneration: " + QString::number(startGeneration));
		Logger::info(QString("Loading file: ") + inFilename);
		QFile inData(inFilename);
		if (inData.open(QFile::ReadOnly))
		{
			QTextStream in(&inData);
			bool loaded = m_individual->loadGen(in);
			// Check possible errors during the loading operation
			if (!loaded)
			{
				Logger::error("Error in the loading of the genotype");
			}
			// Check the consistency of the genotype (i.e., the number of genes)
			if (m_individual->getLength() != m_numGenes)
			{
				Logger::error("Wrong genotype length!");
			}

			// Load the covariance matrix
			char covMatFilename[64];
			sprintf(covMatFilename, "S%dG%d.cvm", (seed()), startGeneration);
			QFile covMatData(covMatFilename);
			if (covMatData.open(QFile::ReadOnly))
			{
				QTextStream covMatInput(&covMatData);
				for (int i = 0; i < m_numGenes; i++)
				{
					for (int j = 0; j < m_numGenes; j++)
					{
						QString str;
						covMatInput >> str;
						bool ok = false;
						float elem = str.toFloat(&ok);
						if (!ok || (covMatInput.status() != QTextStream::Ok))
						{
							Logger::error("Error in the loading of the covariance matrix");
						}
						m_covMat(i,j) = elem;
						expCovMat(i,j) = 0.0;
					}
				}
				covMatData.close();
			}
			inData.close();
		}
	}
	// Flow control
	pauseFlow();
	if (stopFlow())
	{
		// Cleanup
		return;
	}
	// Do all generations
	for (int gn = startGeneration; gn < m_numGenerations; gn++)
	{
		m_gae->setIndividualCounter();
		m_gae->initGeneration(gn);
		// Compute the exponential of the covariance matrix
		Eigen::MatrixExponential<Eigen::MatrixXf>(m_covMat).compute(expCovMat);
		// Extract the offspring
		extractOffspring(expCovMat);
		// Prepare a vector for the fitnesses
		QVector<float> offspringFitness(m_numOffspring);
		QVector<int> offspringIdx(m_numOffspring);
		// Evaluate all the offspring
		for (int g = 0; g < m_numOffspring; g++)
		{
			// Set the genotype to be tested
			m_gt->setGenotype(m_offspring[g]);
			// Evaluate the genotype
			m_gae->evaluate();
			// Get the fitness
			offspringFitness[g] = m_gae->getFitness();
			// Set the fitness of the current genotype
			m_offspring[g]->setFitness(offspringFitness[g]);
		}
		m_gae->resetIndividualCounter();
		// Sort the fitnesses
		sortFitness(offspringFitness, offspringIdx);
		// Compute the utility ranking
		QVector<float> utilityRank = computeUtilityRanking(offspringIdx);
		// Compute the updates
		Eigen::MatrixXf dCovMat(m_numGenes,m_numGenes);
		computeUpdates(dCovMat, utilityRank, expCovMat);
		// Update both the individual and the covariance matrix
		update(m_dInd, dCovMat);
		// Save the best genotype (i.e., the best offspring)
		sprintf(bestOutFilename, "S%dB%d.gen", seed(), 0);
		QFile bestOutData(bestOutFilename);
		bool operation = false;
		if (gn == 0)
		{
			// Open the QFile in write mode
			operation = bestOutData.open(QIODevice::WriteOnly);
		}
		else
		{
			// Open the QFile in append mode
			operation = bestOutData.open(QFile::Append);
		}
		if (operation)
		{
			QTextStream bestOut(&bestOutData);
			m_offspring[offspringIdx[0]]->saveGen(bestOut);
			bestOutData.close();
		}

		// Save all the genotypes (i.e., both the individual and the offspring)
		sprintf(outFilename, "S%dG%d.gen", (seed()), (gn + 1));
		QFile outData(outFilename);
		if (outData.open(QIODevice::WriteOnly))
		{
			QTextStream out(&outData);
			// First save the individual
			m_individual->saveGen(out);
			// Then save the offspring
			for (int j = 0; j < m_numOffspring; j++)
			{
				m_offspring[j]->saveGen(out);
			}
			outData.close();
		}
		
		// Save the covariance matrix
		char covMatFilename[64];
		sprintf(covMatFilename, "S%dG%d.cvm", (seed()), (gn + 1));
		QFile covMatData(covMatFilename);
		if (covMatData.open(QIODevice::WriteOnly))
		{
			QTextStream covMatOutput(&covMatData);
			for (int i = 0; i < m_numGenes; i++)
			{
				for (int j = 0; j < m_numGenes; j++)
				{
					QString str;
					str = QString::number(m_covMat(i,j));
					covMatOutput << " " << str;
				}
				covMatOutput << endl;
			}
			covMatData.close();
		}
		// Flow control
		pauseFlow();
		if (stopFlow())
		{
			break;
		}
		
		// Save fitness statistics
		char fitStatFilename[64];
		sprintf(fitStatFilename, "S%d.fit", (seed()));
		QFile fitStatData(fitStatFilename);
		bool openOk = false;
		if (gn == 0)
		{
			openOk = fitStatData.open(QIODevice::WriteOnly);
		}
		else
		{
			openOk = fitStatData.open(QIODevice::Append);
		}
		if (openOk)
		{
			QTextStream fitStat(&fitStatData);
			// Compute maximum, minimum and average fitness and store them in a vector
			QVector<float> fitArray(3);
			// Max (already found by the algorithm)
			float maxFit = m_offspring[offspringIdx[0]]->getFitness();
			fitArray[0] = maxFit;
			// Average
			float avgFit = 0.0;
			for (int j = 0; j < m_numOffspring; j++)
			{
				avgFit += offspringFitness[j];
			}
			avgFit /= m_numOffspring;
			fitArray[1] = avgFit;
			// Min
			float minFit = offspringFitness[0];
			for (int j = 1; j < m_numOffspring; j++)
			{
				if (offspringFitness[j] < minFit)
				{
					minFit = offspringFitness[j];
				}
			}
			fitArray[2] = minFit;
			saveFitStats(fitArray, fitStat);
			fitStatData.close();
		}
		
		// Save all fitness statistics if <saveFitnessAllIndividuals> flag is set to true
		if (m_saveFitnessAllIndividuals)
		{
			char allFitStatFilename[64];
			sprintf(allFitStatFilename, "S%dall.fit", (seed()));
			QFile allFitStatData(allFitStatFilename);
			openOk = false;
			if (gn == 0)
			{
				openOk = allFitStatData.open(QIODevice::WriteOnly);
			}
			else
			{
				openOk = allFitStatData.open(QIODevice::Append);
			}
			if (openOk)
			{
				QTextStream allFitStat(&allFitStatData);
				saveFitStats(offspringFitness, allFitStat);
				allFitStatData.close();
			}
		}

		// Check the best generation
		float bestFit = m_offspring[offspringIdx[0]]->getFitness();
		if (bestFit > bestFitness)
		{
			bestFitness = bestFit;
			bestGeneration = gn;
		}

		m_gae->endGeneration(gn);
	}

	// Save the information about the best generation
	char bestGenFileName[64];
	sprintf(bestGenFileName, "S%dbest.fit", seed());
	QFile bestGenData(bestGenFileName);
	// Open the QFile in write mode
	if (bestGenData.open(QIODevice::WriteOnly))
	{
		QTextStream bestGenOut(&bestGenData);
		QString outStr;
		//! Write the best generation index
		outStr = QString::number(bestGeneration);
		bestGenOut << outStr;
		//! Write the fitness of the best generation
		outStr = QString::number(bestFitness);
		bestGenOut << " " << outStr;
		bestGenOut << endl;
		bestGenData.close();
	}
}

void XnesAlgo::saveFitStats(QVector<float> fit, QTextStream& out)
{
	QString str;
	for (int i = 0; i < fit.size(); i++)
	{
		str = QString::number(fit[i]);
		if (i == 0)
		{
			out << str;
		}
		else
		{
			out << " " << str;
		}
	}
	out << endl;
}

void XnesAlgo::initialise()
{
	GenotypeFloat* ind = dynamic_cast<GenotypeFloat*>(m_individual);
	// Initialise the individual
	ind->initGaussian(0.0, 1.0);
	// Initialise the covariance matrix
	for (int i = 0; i < m_numGenes; i++)
	{
		for (int j = 0; j < m_numGenes; j++)
		{
			m_covMat(i,j) = 0.0;
		}
	}
}

void XnesAlgo::extractOffspring(const Eigen::MatrixXf expCovMat)
{
	Eigen::MatrixXf dSamples(m_numGenes, m_numOffspring);
	for (int j = 0; j < m_numOffspring; j++)
	{
		GenotypeFloat* offspring = dynamic_cast<GenotypeFloat*>(m_offspring[j]);
		offspring->initGaussian(0.0,0.5); // Use a gaussian distribution with mean 0.0 and standard deviation 0.5
		for (int i = 0; i < m_numGenes; i++)
		{
			// Store the samples (they are needed to compute the updates)
			m_samples(i,j) = offspring->getGene(i);
		}
	}
	dSamples = expCovMat * m_samples;
	// Now extract the offspring
	for (int j = 0; j < m_numOffspring; j++)
	{
		GenotypeFloat* offspring = dynamic_cast<GenotypeFloat*>(m_offspring[j]);
		for (int i = 0; i < m_numGenes; i++)
		{
			GenotypeFloat* ind = dynamic_cast<GenotypeFloat*>(m_individual);
			const float elem = ind->getGene(i) + dSamples(i,j);
			offspring->setGene(i, elem);
		}
	}
}

void XnesAlgo::sortFitness(const QVector<float> fitness, QVector<int>& offspringIdx)
{	
	int startIndex = 0;
	QVector<bool> examined(m_numOffspring);
	for (int j = 0; j < m_numOffspring; j++)
	{
		examined[j] = false;
	}
	int countExamined = 0;
	// Sort fitnesses
	while (countExamined < m_numOffspring)
	{
		bool found = false;
		int i = 0;
		float maxf;
		int indMaxf;
		// Look for the first not analysed offspring
		while ((i < m_numOffspring) && !found)
		{
			if (!examined[i])
			{
				maxf = fitness[i];
				indMaxf = i;
				found = true;
			}
			i++;
		}
		// Check whether there is an offspring better than the current one
		for (int j = 0; j < m_numOffspring; j++)
		{
			if ((fitness[j] > maxf) && !examined[j])
			{
				maxf = fitness[j];
				indMaxf = j;
			}
		}
		// Store the index
		offspringIdx[startIndex] = indMaxf;
		examined[indMaxf] = true;
		countExamined++;
		startIndex++;
	}
}

QVector<float> XnesAlgo::computeUtilityRanking(const QVector<int> offspringIdx)
{
	// Utility ranking array (utilities are assigned to the right offspring)
	QVector<float> utilityRank(m_numOffspring);
	// Now fill the array with the utility values
	for (int j = 0; j < m_numOffspring; j++)
	{
		utilityRank[offspringIdx[j]] = m_utility[j];
	}
	return utilityRank;
}

void XnesAlgo::computeUpdates(Eigen::MatrixXf& dCovMat, const QVector<float> utilityRank, const Eigen::MatrixXf expCovMat)
{
	GenotypeFloat* deltaInd = dynamic_cast<GenotypeFloat*>(m_dInd);
	// Create a matrix whose rows contain weights * samples
	Eigen::MatrixXf sortedUtility(m_numGenes,m_numOffspring);
	Eigen::MatrixXf sortedUtilityPerSamples(m_numGenes,m_numOffspring);
	Eigen::MatrixXf trSamples(m_numOffspring,m_numGenes);
	// The sum of utility values
	float utilitySum = 0.0;
	// Define the identity matrix
	Eigen::MatrixXf eye(m_numGenes,m_numGenes);
	for (int i = 0; i < m_numGenes; i++)
	{
		for (int j = 0; j < m_numGenes; j++)
		{
			if (i == j)
			{
				eye(i,j) = 1.0;
			}
			else
			{
				eye(i,j) = 0.0;
			}
		}
	}
	// Compute the sum of utility values
	for (int j = 0; j < m_numOffspring; j++)
	{
		utilitySum += utilityRank[j];
	}
	// Compute the matrices necessary to calculate the gradients
	for (int i = 0; i < m_numGenes; i++)
	{
		for (int j = 0; j < m_numOffspring; j++)
		{
			sortedUtility(i,j) = utilityRank[j];
		}
	}
	for (int i = 0; i < m_numGenes; i++)
	{
		for (int j = 0; j < m_numOffspring; j++)
		{
			sortedUtilityPerSamples(i,j) = sortedUtility(i,j) * m_samples(i,j);
			trSamples(j,i) = m_samples(i,j);
		}
	}
	// Define transpose matrices for both samples and sorted utilities
	// (they are necessary to perform the calculation of gradients)
	Eigen::MatrixXf trSortedUtility(m_numOffspring,1);
	Eigen::MatrixXf trInd(m_numOffspring,1);
	for (int j = 0; j < m_numOffspring; j++)
	{
		trSortedUtility(j,0) = utilityRank[j];
	}
	// Compute the update of the individual
	trInd = m_indLrate * expCovMat * m_samples * trSortedUtility;
	for (int i = 0; i < m_numGenes; i++)
	{
		deltaInd->setGene(i, trInd(i,0));
	}
	// Compute the update of the covariance matrix
	Eigen::MatrixXf G = sortedUtilityPerSamples * trSamples - utilitySum * eye;
	dCovMat = m_covMatLrate * G;
}
	
void XnesAlgo::update(Genotype* dInd, const Eigen::MatrixXf dCovMat)
{
	GenotypeFloat* deltaInd = dynamic_cast<GenotypeFloat*>(dInd);
	GenotypeFloat* individual = dynamic_cast<GenotypeFloat*>(m_individual);
	// Update of the individual
	for (int i = 0; i < m_numGenes; i++)
	{
		float newVal = individual->getGene(i) + deltaInd->getGene(i);
		individual->setGene(i, newVal);
	}
	// Update of the covariance matrix
	m_covMat += dCovMat;
}

} // end namespace farsa
