/*
 * Concrete example for the xNES algorithm (.h)
 */
 
class xnes : public EvoAlgo, public : farsa::Component
{

public:
	/**
	 * \brief Constructor
	 */
	xnes();
	
	/**
	 * \brief Destructor
	 */
	~xnes();
	
	/**
	 * \brief Set the seed
	 */
	virtual void setSeed(unsigned int seed);
	
	/**
	 * \brief Entry point of the algorithm
	 */
	virtual void run();
	
protected:
	/**
	 * \brief Compute the xNES parameters
	 */
	void computeNesParameters();
	
	/**
	 * \brief Initialise the individual
	 */
	void initialiseIndividual();
	
	/**
	 * \brief Extract the offspring
	 */
	void extractOffspring(Eigen::MatrixXf expCovMat);
	
	/**
	 * \brief Sort fitness
	 */
	void sortFitness(QVector<float> fitness, int* offspringIdx);
	
	/**
	 * \brief Utility ranking
	 */
	QVector<float> computeUtilityRanking(int* offspringIdx);
	
	/**
	 * \brief Compute updates
	 */
	void computeUpdates(Genotype* dInd, Eigen::MatrixXf* dCovMat, QVector<float> utilityRank, Eigen::MatrixXf expCovMat);
	
	/**
	 * \brief Update individual and covariance matrix
	 */
	void update(Genotype dInd, Eigen::MatrixXf dCovMat);
	
	//! The number of genes of the genotype
	int m_numGenes;
	//! The genotype to be tested
	GenotypeToTest* m_genotypeToTest;
	//! The number of offspring
	int m_numOffspring;
	//! The offspring
	QVector<Genotype*> m_offspring;
	//! The individual
	Genotype* m_individual;
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
	//! The evaluator
	Evaluator* m_evaluator;
	//! The random generator
	RandomGenerator* m_rng;
}
