/*
 * Implementation of the algorithm that combines xNES and back-propagation (.h)
 */
 
class multiLearningAlgo : public xnes, public : salsa::Component
{

public:
	/**
	 * \brief Constructor
	 */
	multiLearningAlgo();
	
	/**
	 * \brief Destructor
	 */
	~multiLearningAlgo();
	
	/**
	 * \brief Set the seed
	 */
	virtual void setSeed(unsigned int seed);
	
	/**
	 * \brief Entry point of the algorithm
	 */
	virtual void run();
	
private:
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
	 * \brief Initialise the genotypes
	 */
	void initialiseGenotypes();
	
	/**
	 * \brief Evaluate the genotypes
	 */
	void evaluateGenotypes();
	
	/**
	 * \brief Compute the back-propagation contribution
	 */
	void computeBackPropagationContribution();
	
	//! The vector of possible initial genotypes
	QVector<Genotype> m_initialGenotypes;
	int m_initialGenotypesSize;
	int m_bestGenIdx;
}
