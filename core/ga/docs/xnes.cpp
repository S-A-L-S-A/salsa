/*
 * Concrete example for the xNES algorithm (.cpp)
 */
 
xnes::xnes()
	: EvoAlgo()
	, m_genotypeToTest(nullptr)
	, m_offspring()
	, m_individual(nullptr)
	, m_covMat()
	, m_evaluator(nullptr)
	, m_rng(nullptr)
{
	int m_numGenes = m_genotypeToTest->requestedGenotypeLength();
}
xnes::~xnes()
{
}

void xnes::setSeed(unsigned int seed)
{
	m_rng->setSeed(seed);
}
	
void xnes::run()
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

void xnes::computeNesParameters()
{
	// Define the number of offspring
	const float nGenes = (const float)m_numGenes;
	float numOff = 4.0 + 3.0 * floor(log(nGenes));
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

void xnes::initialiseIndividual()
{
	// Now simply initialise each gene
	for (int i = 0; i < m_numGenes; i++)
	{
		m_individual[i] = m_rng->getGaussian(1.0,0.0); // Use a gaussian distribution with mean 0.0 and standard deviation 1.0
	}
}

void xnes::extractOffspring(Eigen::MatrixXf expCovMat)
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
	
void xnes::sortFitness(QVector<float> fitness, int* offspringIdx)
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
	
QVector<float> xnes::computeUtilityRanking(int* offspringIdx)
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
	
void xnes::computeUpdates(Genotype* dInd, Eigen::MatrixXf* dCovMat, QVector<float> utilityRank, Eigen::MatrixXf expCovMat)
{
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
	for (i = 0; i < m_numGenes; i++)
	{
		dInd[i] = trInd(i,0);
	}
	// Compute the update of the covariance matrix
	Eigen::MatrixXf G = sortedUtilityPerSamples * trSamples - utilitySum * eye;
	dCovMat = m_covMatLrate * G;
}
	
void xnes::update(Genotype dInd, Eigen::MatrixXf dCovMat)
{
	// Update of the individual
	for (int i = 0; i < m_numGenes; i++)
	{
		m_individual[i] += dInd[i];
	}
	// Update of the covariance matrix
	m_covMat += dCovMat;
}
