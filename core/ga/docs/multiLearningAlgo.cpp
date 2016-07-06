/*
 * Implementation of the algorithm that combines xNES and back-propagation (.cpp)
 */
 
multiLearningAlgo::multiLearningAlgo()
	: multiLearningAlgo()
	, m_initialGenotypes()
	, m_initialGenotypesSize(0)
	, m_bestGenIdx(0)
{
	int m_numGenes = m_genotypeToTest->requestedGenotypeLength();
}
multiLearningAlgo::~multiLearningAlgo()
{
}

void multiLearningAlgo::setSeed(unsigned int seed)
{
	m_rng->setSeed(seed);
}
	
void multiLearningAlgo::run()
{
	// Local variables
	Eigen::MatrixXf expCovMat(m_numGenes,m_numGenes);
	
	// Compute the NES parameters
	computeNesParameters();
	// Initialise the individual
	initialiseIndividual();
	// Initialise the covariance matrix
	for (int i = 0; i < m_numGenes; i++)
	{
		for (int j = 0; j < m_numGenes; j++)
		{
			m_covMat(i,j) = 0.0;
		}
	}
	for (int i = 0; i < numGenerations; i++) {
		// Compute the exponential of the covariance matrix
		Eigen::MatrixExponential<Eigen::MatrixXf>(m_covMat).compute(expCovMat);
		// Extract the offspring
		extractOffspring(expCovMat);
		// Prepare a vector for the fitnesses
		QVector<float> offspringFitness(m_numOffspring);
		int* offspringIdx = new int[m_numOffspring];
		// Evaluate all the offspring
		for (int g = 0; g < m_numOffspring; g++)
		{
			// Set the genotype to be tested
			m_genotypeToTest->setGenotype(m_offspring[g]);
			// Evaluate the genotype
			m_evaluator->evaluate();
			// Get the fitness of the current genotype
			offspringFitness[g]	= m_evaluator->getFitness();
		}
		// Sort the fitnesses
		sortFitness(offspringFitness, offspringIdx);
		// Compute the utility ranking
		QVector<float> utilityRank = computeUtilityRanking(offspringIdx);
		// Compute the updates
		computeUpdates(dInd, dCovMat, utilityRank, expCovMat);
		// Update both the individual and the covariance matrix
		update(*dInd, *dCovMat);
	}
}

void multiLearningAlgo::computeNesParameters()
{
	// Define the number of offspring
	const float nGenes = (const float)m_numGenes;
	float numOff = 4.0 + 3.0 * floor(log(nGenes)) + 1.0;
	m_numOffspring = (int)numOff;
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
	for (int i = 0; i < numOff; i++)
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
	for (int i = 0; i < numOff; i++)
	{
		if (utilitySum != 0.0) // It is a useless check, only for safety purposes
		{
			m_utility[i] /= utilitySum;
		}
	}
}

void multiLearningAlgo::initialiseIndividual()
{
	// Initialise each gene
	for (int i = 0; i < m_numGenes; i++)
	{
		m_individual[i] = m_initialGenotypes[m_bestGenIdx][i];
	}
}

void multiLearningAlgo::extractOffspring(Eigen::MatrixXf expCovMat)
{
	Eigen::MatrixXf dSamples(m_numGens, m_numOffspring);
	for (int j = 0; j < m_numOffspring; j++)
	{
		Genotype* offspring = new Genotype[m_numGenes];
		for (int i = 0; i < m_numGenes; i++)
		{
			offspring[i] = m_rng->getGaussian(0.5,0.0); // Use a gaussian distribution with mean 0.0 and standard deviation 0.5
		}
		// Store the samples (they are needed to compute the updates)
		m_samples(i,j) = offspring[i];
	}
	// Compute the back-propagation contribution
	computeBackPropagationContribution();
	dSamples = expCovMat * m_samples;
	// Now extract the offspring
	for (int j = 0; j < m_numOffspring; j++)
	{
		for (int i = 0; i < m_numGenes; i++)
		{
			m_offspring[j][i] = m_individual[i] + dSamples(i,j);
		}
	}
}
	
void multiLearningAlgo::sortFitness(QVector<float> fitness, int* offspringIdx)
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
		if (countExamined == 1)
		{
			if (!examined[m_numOffspring - 1])
			{
				indMaxf = m_numOffspring - 1;
			}
		}
		// Store the index
		offspringIdx[startIndex] = indMaxf;
		examined[indMaxf] = true;
		countExamined++;
		startIndex++;
	}
}

void multiLearningAlgo::initialiseGenotypes()
{
	for (int i = 0; i < m_initialGenotypesSize; i++)
	{
		m_initialGenotypes[i] = m_rng->getGaussian(1.0,0.0); // Use a gaussian distribution with mean 0.0 and standard deviation 1.0
	}
}

void multiLearningAlgo::evaluateGenotypes()
{
	QVector<float> fitness(m_initialGenotypesSize);
	float bestFitness;
	int bestFitIdx;
	for (int i = 0; i < m_initialGenotypesSize; i++)
	{
		// Set the genotype to be tested
		m_genotypeToTest->setGenotype(m_initialGenotypes[i]);
		// Evaluate the genotype
		m_evaluator->evaluate();
		// Get the fitness of the current genotype
		fitness[i]	= m_evaluator->getFitness();
	}
	// Now find the best genotype
	bestFitness = fitness[0];
	bestFitIdx = 0;
	for (int i = 1; i < m_initialGenotypesSize; i++)
	{
		if (fitness[i] > bestFitness)
		{
			bestFitness = fitness[i];
			bestFitIdx = i;
		}
	}
	// Store the best genotype index
	m_bestGenIdx = bestFitIdx;
}

void multiLearningAlgo::computeBackPropagationContribution()
{
	float* backProp = new float[m_numGenes];
	// Set the genotype to be tested
	m_genotypeToTest->setGenotype(m_individual);
	// Evaluate the genotype
	m_evaluator->evaluate();
	// Get the back-propagation vector
	backProp = m_evaluator->getBackPropArray();
	// Set the last offspring based on the back-propagation contribution
	for (int i = 0; i < m_numGenes; i++)
	{
		m_samples(i,m_numOffspring - 1) = backProp;
	}
}
