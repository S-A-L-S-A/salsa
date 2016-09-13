/*
 * My own idea about the structure of SALSA (GAs/Evolutionary Algorithms, evaluator, mapping gen2phe, experiment, controller and other stuff)
 */
 
// Experiment (world, arena and so on)

// Evolutionary algorithm / GA

class EvoAlgo
{
	public:
		// Constructor
		EvoAlgo();
		// Destructor
		~EvoAlgo();
		// Method for setting the seed
		virtual void setSeed();
		// Entry point to run the evolution
		virtual void run();
}

// Evaluator

class Evaluator
{
	public:
		// Constructor
		Evaluator();
		// Destructor
		~Evaluator();
		// Method for the evaluation of the population
		virtual void evaluate();
}

// Mapping gen2phe (Evolutionary Algorithm side)

class GenotypeToTest
{
	public:
		// Constructor
		GenotypeToTest();
		// Destructor
		~GenotypeToTest();
		// Method for setting the genotype to test
		virtual void setGenotype(Genotype* gen); // To be extended to two or more genotypes (in this latter case a vector should be used)
}

// Mapping gen2phe (evaluator side)

class PhenotypeGenerator
{
	public:
		// Constructor
		PhenotypeGenerator();
		// Destructor
		~PhenotypeGenerator();
		// Method for getting the phenotype
		virtual NeuralNetwork getNeuralNetwork();
}

// Genotype
class Genotype
{
	public:
		// Constructor
		Genotype();
		// Destructor
		~Genotype();
}

// Neural network
class NeuralNetwork
{
	public:
		// Constructor
		NeuralNetwork();
		// Destructor
		~NeuralNetwork();
}

// Notice that I have not included all sub-classes that inherit from these interfaces, but they must exist.
// I have also avoided to write all pieces of the Genotype and NeuralNetwork classes.

// Personal considerations on the basis on my own knowledge of the NES (and of the SteadyState)

class SteadyState : public EvoAlgo, public salsa::component
{
	public:
		SteadyState()
			: EvoAlgo()
			, m_genotypeToTest(nullptr)
			, m_genotypes()
			, m_evaluator(nullptr)
		{
			int numGenes = m_genotypeToTest->requestedGenotypeLength();
			... // Create genotype with the correct length and all the rest (reproduction, mutation, ...)
		}

		virtual ~SteadyState()
		{
			... // Delete stuff
		}

		virtual void setSeed(unsigned int seed)
		{
			rng->setSeed(seed);
		}

		virtual void run()
		{
			// Initialise population
			initPopulation();
			for (int i = 0; i < numGenerations; i++) {
				for (int g = 0; g < numGenotypes; g++) {
					m_genotypeToTest->setGenotype(m_genotypes[g]);
					m_evaluator->evaluate();
					int genotypeFitness = m_evaluator->getFitness();
					... // set fitness for the genotype
				}

				... // select, reproduce, mutate
			}
		}
		
		void initPopulation();

	private:
		GenotypeToTest* m_genotypeToTest;
		QVector<Genotype*> m_genotypes;
		Evaluator* m_evaluator;
}

class xNES : public EvoAlgo, public salsa::component
{
	public:
		xNES()
			: EvoAlgo()
			, m_genotypeToTest(nullptr)
			, m_offspring(),
			, m_individual(nullptr)
			, m_evaluator(nullptr)
		{
			int numGenes = m_genotypeToTest->requestedGenotypeLength();
			Eigen::MatrixXf covMatr(numGenes,numGenes);
			... // Create genotype with the correct length
			... // Create offspring with the correct length
		}
		
		virtual ~xNES()
		{
			... // Delete stuff
		}

		virtual void setSeed(unsigned int seed)
		{
			rng->setSeed(seed);
		}

		virtual void run()
		{
			// Compute NES parameters (number of offspring, learning rates, utility values, matrix exponential and so on)
			computeParameters();
			for (int i = 0; i < numGenerations; i++) {
				// Extract offspring
				extractOffspring();
				QVector<float> offspringFitness(m_offspring.size());
				for (int g = 0; g < m_offspring.size(); g++)
				{
					m_genotypeToTest->setGenotype(m_offspring[g]);
					m_evaluator->evaluate();
					offspringFitness[g]	= m_evaluator->getFitness();
				}
				// Sort fitnesses
				sortFitness();
				// Compute the utility ranking
				utilityRanking();
				// Compute components to update the individual
				computeUpdateComponents();
				// Update the individual
				updateGenotype();
				// Update the covariance matrix
				updateCovMatrix();
			}
		}
		
		void updateGenotype()
		{
			... // Update the individual (individual += dInd)
		}
		
		void updateCovMatrix()
		{
			... // Update the covariance matrix (covMatr += dCovMatr)
		}
		
		void extractOffspring()
		{
			for (int j = 0; j < offspringSize; j++) // The offspring population size is a constant value that depends on <numGenes>
			{
				Genotype* offspring = new Genotype[numGenes];
				for (int i = 0; i < numGenes; i++)
				{
					offspring[i] = rng->getGaussian(0.0,1.0); // Use a gaussian distribution with mean 0.0 and standard deviation 1.0 (or settable with a parameter)
				}
				m_offspring[j] = m_individual[j] + expCovMatr * offspring; // Now I am not taking into account the dimensions of arrays and matrices
			}
		}
		
		void sortFitness()
		{
			... // Sorting algorithm
		}
		
		void utilityRanking()
		{
			... // Simply put the utility values in the right order (on the basis on the fitness values)
		}
		
		void computeParameters()
		{
			... // Compute the NES parameters
		}
		
		void computeUpdateComponents()
		{
			... // Compute the update components (both for the individual and for the covariance matrix)
		}

	private:
		GenotypeToTest* m_genotypeToTest;
		QVector<Genotype*> m_offspring;
		Genotype* m_individual;
		Evaluator* m_evaluator;
}
