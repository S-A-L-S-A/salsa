// Tentative structure of the new GA

class Genotype; // Or perhaps a template parameter?

////////////////////////////////////////////////////////////////////////////////
// GA LIBRARY - Interface for the genetic algorithm
////////////////////////////////////////////////////////////////////////////////

class EvoAlgo
{
public:
	virtual ~EvoAlgo();

	virtual void setSeed(unsigned int seed) = 0;
	virtual void runEvolution() = 0;

	// parameters: genotype preparer and ga evaluator. here we only keep base pointers but subclasses should use their owns (to force a specific subtype)
};

////////////////////////////////////////////////////////////////////////////////
// GA LIBRARY - Interface for the genotypes-preparer classes
////////////////////////////////////////////////////////////////////////////////

// Empty, just used to force ga to use a GenotyPreparer
class GenotypesPreparer
{
public:
	virtual ~GenotypesPreparer();
};

class SingleFixedLengthGenotypePreparer : public GenotypesPreparer
{
public:
	virtual ~SingleGenotypePreparer();

	virtual void setGenotype(Genotype* g) = 0;
	virtual int requestedGenotypeLength() const = 0;
};

class TwoGenotypesPreparer : public GenotypesPreparer
{
public:
	virtual ~TwoGenotypesPreparer();

	virtual void setGenotypes(Genotype* g1, Genotype* g2) = 0;
};

class MultipleGenotypesPreparer : public GenotypesPreparer
{
public:
	virtual ~MultipleGenotypesPreparer();

	virtual void setGenotypes(QVector<Genotype*> g) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// GA LIBRARY - Interface for ga evaluators
////////////////////////////////////////////////////////////////////////////////

class GAEvaluator
{
public:
	virtual ~GAEvaluator();

	virtual void evaluate() = 0;
};

class UniqueFitnessGAEvaluator : public GAEvaluator
{
public:
	virtual ~UniqueFitnessGAEvaluator();

	virtual real getFitness() = 0;
};

class FitnessVectorGAEvaluator : public GAEvaluator
{
public:
	virtual ~FitnessVectorGAEvaluator();

	virtual QVector<real> getFitnessVector() = 0;
}

class FitnessForMultipleGenotypesGAEvaluator : public GAEvaluator
{
public:
	virtual ~FitnessForMultipleGenotypesGAEvaluator();

	virtual real getFitness(int genotypeIndex) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// EXPERIMENT LIBRARY - Interface for the experiment inputs
////////////////////////////////////////////////////////////////////////////////

class ExperimentInput
{
public:
	virtual ~ExperimentInput();
};

class OneNeuralNetworkExperimentInput : public ExperimentInput
{
public:
	virtual ~OneNeuralNetworkExperimentInput();

	virtual void setNeuralNetwork(NeuralNetwork n) = 0;
};

class TwoNeuralNetworksExperimentInput : public ExperimentInput
{
public:
	virtual ~TwoNeuralNetworksExperimentInput();

	virtual void setFirstNeuralNetwork(NeuralNetwork n) = 0;
	virtual void setSecondNeuralNetwork(NeuralNetwork n) = 0;
};

class MultipleNeuralNetworksExperimentInput : public ExperimentInput
{
public:
	virtual ~MultipleNeuralNetworksExperimentInput();

	virtual void setNumNeuralNetworks(int n) = 0;
	virtual void setNeuralNetwork(int i, NeuralNetwork) = 0;
};

class RobotBodyAndBrainExperimentInput : public ExperimentInput
{
public:
	virtual ~RobotBodyAndBrainExperimentInput();

	virtual void setRobotBody(Robot r) = 0;
	virtual void setRobotBrain(Controller c) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// EXPERIMENT LIBRARY - Other classes
////////////////////////////////////////////////////////////////////////////////

// Has all that's needed for a robotic experiment (e.g. setup arena, world,
// sensors, motors, ...)
class RoboticExperiment;

////////////////////////////////////////////////////////////////////////////////
// Example for simple generational GA
////////////////////////////////////////////////////////////////////////////////

class SimpleGenerationalGA : public EvoAlgo, public farsa::Component
{
public:
	SimpleGenerationalGA(ConfigurationManager& params, QString prefix)
		: EvoAlgo()
		, farsa::Component(params, prefix)
		, m_genotypeToTest(NULL)
		, m_fitnessEvaluator(NULL)
	{
		// Configuring in the constructor, the configure function will not be called

		m_genotypeToTest = params.getObjectFromParameter<SingleGenotypeToTest>(prefix + "genotypeToTest");
		m_fitnessEvaluator = params.getObjectFromParameter<UniqueFitnessEvaluator>(prefix + "evaluator");

		OR, USING JONATA APPROACH:

		QString genotypeToTestGroup = params.getValue(prefix + "genotypeToTest");
		m_genotypeToTest = params.getObjectFromParameter<SingleGenotypeToTest>(genotypeToTestGroup);
		m_fitnessEvaluator = params.getObjectFromParameter<UniqueFitnessEvaluator>(genotypeToTestGroup + ConfigurationManager::groupSeparator() + "evaluator");

		int numGenes = m_genotypeToTest->requestedGenotypeLength();
		... // Create genotype with the correct length and all the rest (reproduction, mutation, ...)
	}

	virtual ~SimpleGenerationalGA()
	{
		...
	}

	static void describe()
	{
		...
	}

	virtual void setSeed(unsigned int seed)
	{
		rng->setSeed(seed);
	}

	virtual void runEvolution()
	{
		for (int i = 0; i < numGenerations; ++i) {
			for (int g = 0; g < numGenotypes; ++g) {
				m_genotypeToTest->setGenotype(m_genotypes[g]);
				m_fitnessEvaluator->evaluate();
				real genotypeFitness = m_fitnessEvaluator->getFitness();
				... // set fitness for the genotype
			}

			... // select, reproduce, mutate
		}
	}

private:
	SingleGenotypeToTest* m_genotypeToTest;
	UniqueFitnessEvaluator* m_fitnessEvaluator;
};

class OneGenomeToOneNeuralNetworkMapping : public SingleGenotypeToTest, public farsa::Component
{
public:
	OneGenomeToOneNeuralNetworkMapping(ConfigurationManager& params, QString prefix)
		: SingleGenotypeToTest()
		, farsa::Component(params, prefix)
		, m_net(NULL)
		, m_nextGenotype(NULL)
		, m_evaluator(NULL)
	{
		// Configuring in the constructor, the configure function will not be called
		m_net = params.getObjectFromGroup<Evonet>(prefix + "NeuralNetwork"); // Empty net, perhaps a prototype of networks to generate
		m_evaluator = params.getObjectFromParameter<OneNeuralNetworkExperimentInput>(prefix + "evaluator");

		... // Read parameters
	}

	virtual ~OneGenomeToOneNeuralNetworkMapping()
	{
		...
	}

	static void describe()
	{
		...
	}

	virtual int requestedGenotypeLength() const
	{
		return m_net->numParameters(); // The total number of weights, biases, deltas, ...
	}

	virtual void setGenotype(Genotype* g)
	{
		// Store genotype
		m_nextGenotype = g;

		net = ... // Generate the neural network

		m_evaluator->setNeuralNetwork(net);
	}

private:
	Evonet* m_net;
	Genotype* m_nextGenotype;
	OneNeuralNetworkExperimentInput* m_evaluator;
};

class SingleRobotEvaluator : public RoboticExperiment, public UniqueFitnessEvaluator, public OneNeuralNetworkExperimentInput, public farsa::Component
{
public:
	SingleRobotEvaluator(ConfigurationManager& params, QString prefix)
		: RoboticExperiment()
		, UniqueFitnessEvaluator()
		, OneNeuralNetworkExperimentInput()
		, farsa::Component(params, prefix)
	{
		... // Read parameters, create agent, ...
	}

	virtual ~SingleRobotEvaluator()
	{
		...
	}

	static void describe()
	{
		...
	}

	virtual void setNeuralNetwork(NeuralNetwork n)
	{
		... // Store neural network
	}

	virtual void evaluate()
	{
		... // Do all trials, steps, ...

		m_fitness = ...;
	}

	virtual real getFitness()
	{
		return m_fitness;
	}
};

// Example configuration file
//
// [GA]
// type = SimpleGenerationalGA
// popSize = 100
// numThreads = 8
// genotypeToTest = MAPPING
// evaluator = EVALUATOR # Not present in Jonata approach (see GA class above)
//
// [GA/Genotype]
// type = ...
//
// [GA/Reproduction]
// type = ...
//
// [GA/Reproduction/Selection]
// type = ...
//
// [GA/Reproduction/Mutation]
// type = ...
//
// #[TEST_FROM_FILE] # This also needs a component to run evaluate()
// #type = ExtractGenotypeFromFile
// #file = genome.file
// #genotypeToTest = MAPPING
//
// [MAPPING]
// type = OneGenomeToOneNeuralNetworkMapping
// evaluator = EVALUATOR
//
// [MAPPING/NeuralNetwork]
// type = Evonet
//
// [EVALUATOR]
// type = SingleRobotEvaluator
// arenaSize = 100, 100
//
// [EVALUATOR/Agent]
// type = EmbodiedAgent
//
// [EVALUATOR/Agent/Sensor:0]
// type = ...
//
// [EVALUATOR/Agent/Motor:0]
// type = ...
