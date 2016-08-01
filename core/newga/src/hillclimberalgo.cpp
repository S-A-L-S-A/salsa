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

#include "hillclimberalgo.h"
#include "evodataviewer.h"
#include "configurationhelper.h"
#include "evodataviewer.h"
#include <QTextStream>
#include <QFile>

namespace salsa {

HillClimberAlgo::HillClimberAlgo(ConfigurationParameters& params, QString prefix)
	: EvoAlgo(params, prefix)
	, m_numGenes(1)
	, m_individual(NULL)
	, m_prototype(NULL)
	, m_gt(NULL)
	, m_gae(NULL)
	, m_numGenerations(1)
	, m_mutRate(0.1)
	, m_mutDecay(0.01)
	, m_initMutRate(0.5)
	, m_saveFitnessAllIndividuals(false)
	, m_rngSeed(0)
{
	// Get parameters' values from the configuration file (.ini), if some of these parameters
	// are not in the file, the default values will be used
	m_gt = params.getObjectFromParameter<SingleGenotypeIntToEvonet>(prefix + QString("genotypeTester"));
	m_gae = params.getObjectFromParameter<RobotExperiment>(prefix + QString("gaEvaluator"));
	m_numGenerations = ConfigurationHelper::getInt(params, prefix + QString("numGenerations"), 1000);
	m_mutRate = ConfigurationHelper::getDouble(params, prefix + QString("mutRate"), 0.1);
	// If the mutation rate exceeds 1.0, it must be converted in percentage
	if (m_mutRate >= 1.0)
	{
		m_mutRate /= 100.0;
	}
	m_mutDecay = ConfigurationHelper::getDouble(params, prefix + "mutDecay", 0.01);
	m_initMutRate = ConfigurationHelper::getDouble(params, prefix + "initMutRate", 0.5);
	m_saveFitnessAllIndividuals = ConfigurationHelper::getBool(params, prefix + QString("saveFitnessAllIndividuals"), false);
	m_numGenes = m_gt->requestedGenotypeLength();
	m_prototype = params.getObjectFromGroup<Genotype>( prefix + "Genotype" );
	GenotypeInt* g = dynamic_cast<GenotypeInt*>(m_prototype);
	g->setLength(m_numGenes);
	m_individual = m_prototype->clone();
	m_offspring = m_prototype->clone();
	// Initialise the seed
	GenotypeInt* g0 = dynamic_cast<GenotypeInt*>(m_individual);
	m_rngSeed = g0->getSeed();
}

HillClimberAlgo::~HillClimberAlgo()
{
	// Delete all objects (i.e., pointers) in order to free memory.
	delete m_gt;
	delete m_gae;
	delete m_individual;
	delete m_offspring;
}

void HillClimberAlgo::describe(QString type)
{
	Descriptor d = addTypeDescription(type, "Hill-Climber Algorithm");
	d.describeInt("numGenerations").limits(1, INT_MAX).def(1000).help("Number of the generations of the evolutionary process");
	d.describeReal("mutRate").def(0.1).help("Mutation rate");
	d.describeReal("mutDecay").def(0.01).help("Mutation rate decay");
	d.describeReal("initMutRate").def(0.5).help("Initial mutation rate");
	d.describeObject("gaEvaluator").type("RobotExperiment").props(IsMandatory).help("Object that calculate the fitness");
	d.describeObject("genotypeTester").type("SingleGenotypeIntToEvonet").props(IsMandatory).help("Object that sets the genotype to be tested");
	d.describeSubgroup("Genotype").type("GenotypeInt").props(IsMandatory).help("Object containing the individual under evolution");
}

void HillClimberAlgo::runEvolution()
{
	int startGeneration = 0;
	char statfile[64];
	char inFilename[128];
	char outFilename[128];
	char bestOutFilename[128];
	int bestGeneration = -1;
	float bestFitness = -1.0;
	// Define the final mutation rate (i.e., the lower bound for the mutation rate)
	const double finalMutRate = m_mutRate;
	// Initialise the mutation rate
	m_mutRate = m_initMutRate; // initially mutation (default 50%)
	// Set the seed
	setSeed(m_rngSeed);
	// Initialise the individual
	randomiseIndividual();
	// Check whether to recover a previous evolution
	sprintf(statfile, "S%d.fit", seed());
	// Check if the file exists
	DataChunk statTest(QString("stattest"), Qt::blue, 2000, false);
	if (statTest.loadRawData(QString(statfile), 0))
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
			// Check the consistency of the loaded genotype (i.e., the number of genes)
			if (m_individual->getLength() != m_numGenes)
			{
				Logger::error("Wrong genotype length!");
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
		// Define a fitness array with size = 2 (i.e., the first location is
		// reserved for the parent, while the second one is for the offspring)
		QVector<float> fit(2);
		m_gae->setIndividualCounter();
		m_gae->initGeneration(gn);
		// Set the genotype to be tested (in this case the individual)
		m_gt->setGenotype(m_individual);
		// Evaluate the individual
		m_gae->evaluate();
		// Get the fitness
		fit[0] = m_gae->getFitness();
		// Set the fitness of the individual
		m_individual->setFitness(fit[0]);
		// Use subclasses for modifying and/or getting elements (i.e., genes)
		GenotypeInt* ind = dynamic_cast<GenotypeInt*>(m_individual);
		GenotypeInt* off = dynamic_cast<GenotypeInt*>(m_offspring);
		// Now generate an offspring and evaluate it
		for (int g = 0; g < m_numGenes; g++)
		{
			int val = ind->getGene(g);
			// Mutate the value
			int newVal = mutate(val, m_mutRate);
			off->setGene(g, newVal);
		}
		m_gt->setGenotype(m_offspring);
		// Evaluate the offspring
		m_gae->evaluate();
		// Get the fitness
		fit[1] = m_gae->getFitness();
		// Flow control
		pauseFlow();
		if (stopFlow())
		{
			return;
		}
		m_gae->resetIndividualCounter();
		// Select the best individual
		if (fit[1] > fit[0])
		{
			// The offspring is better than its parent, thus replace it
			GenotypeInt* ind = dynamic_cast<GenotypeInt*>(m_individual);
			GenotypeInt* off = dynamic_cast<GenotypeInt*>(m_offspring);
			for (int g = 0; g < m_numGenes; g++)
			{
				int val = off->getGene(g);
				ind->setGene(g, val);
			}
			// Update also the fitness
			ind->setFitness(fit[1]);
		}

		// Flow control
		pauseFlow();
		if (stopFlow())
		{
			break;
		}

		// Save fitness statistics
		char fitStatFilename[64];
		sprintf(fitStatFilename, "S%d.fit", seed());
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
			// (in the case of the Hill-Climber max = min = avg!!!)
			QVector<float> fitArray(3);
			// Max
			fitArray[0] = fit[0];
			// Average
			fitArray[1] = fit[0];
			// Min
			fitArray[2] = fit[0];
			saveFitStats(fitArray, fitStat);
			fitStatData.close();
		}

		// Save all fitness statistics if <saveFitnessAllIndividuals> flag is set to true
		if (m_saveFitnessAllIndividuals)
		{
			char allFitStatFilename[64];
			sprintf(allFitStatFilename, "S%dall.fit", seed());
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
				saveFitStats(fit, allFitStat);
				allFitStatData.close();
			}
		}

		// Save the best genotype
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
			GenotypeInt* ind = dynamic_cast<GenotypeInt*>(m_individual);
			ind->saveGen(bestOut);
			bestOutData.close();
		}

		// Save all the genotypes
		sprintf(outFilename, "S%dG%d.gen", seed(), (gn + 1));
		QFile outData(outFilename);
		if (outData.open(QIODevice::WriteOnly))
		{
			QTextStream out(&outData);
			GenotypeInt* ind = dynamic_cast<GenotypeInt*>(m_individual);
			ind->saveGen(out);
			outData.close();
		}
		// Decrease the mutation rate (until it becomes equal to the lower bound)
		if (m_mutRate > finalMutRate)
		{
			m_mutRate -= m_mutDecay;
		} 
		else 
		{
			m_mutRate = finalMutRate;
		}

		// Check the best generation
		if (fit[0] > bestFitness)
		{
			bestFitness = fit[0];
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

void HillClimberAlgo::saveFitStats(QVector<float> fit, QTextStream& out)
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

void HillClimberAlgo::randomiseIndividual()
{
	// Initialise genes in the range [0,256]
	GenotypeInt* g = dynamic_cast<GenotypeInt*>(m_individual);
	g->initRandom(0, 256);
}

int HillClimberAlgo::mutate(int w, double mut)
{
	int b[8];
	int val;
	int i;

	val = w;
	// Represent the weight as a bit array
	for (i = 0; i < 8; i++)
	{
		b[i] = val % 2;
		val = val / 2;
	}
	for (i = 0; i < 8; i++)
	{
		double p = (double)rand() / (double)RAND_MAX;
		if (p < mut)
		{
			// Invert bit (from 0 to 1 or vice-versa)
			b[i] = (b[i] + 1) % 2;
		}
	}
	// Now update the weight
	w = 0;
	w += b[0] * 1;
	w += b[1] * 2;
	w += b[2] * 4;
	w += b[3] * 8;
	w += b[4] * 16;
	w += b[5] * 32;
	w += b[6] * 64;
	w += b[7] * 128;

	return w;
}

} // end namespace salsa
