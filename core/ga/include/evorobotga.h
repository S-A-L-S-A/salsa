// /********************************************************************************
//  *  FARSA Experiments Library                                                   *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #ifndef EVOROBOTGA_H
// #define EVOROBOTGA_H
//
// #include "configurationmanager.h"
// #include "component.h"
// #include "gaconfig.h"
// #include "evorobotgastatistics.h"
// #include "evorobotintgenotype.h"
// #include <QString>
//
// namespace farsa {
//
// /**
//  * \brief Genetic algorithm from evorobot (generational implementation)
//  *
//  * This class still contains parts from the original algorithm...
//  */
// class FARSA_GA_API EvorobotGA : public EvoAlgo
// {
// public:
// 	static bool configuresInConstructor()
// 	{
// 		return false;
// 	}
//
// public:
// 	/**
// 	 * \brief Constructor
// 	 */
// 	EvorobotGA(ConfigurationManager& params);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	~EvorobotGA();
//
// 	/**
// 	 * \brief Configures the object
// 	 */
// 	virtual void configure();
//
// 	/**
// 	 * \brief Adds the descriptions of all parameters and subgroups
// 	 * \param d the RegisteredComponentDescriptor to use to describe
// 	 *          parameters and subgroups. Only use inside this function, do
// 	 *          not store any reference because the object si destroyed by
// 	 *          the caller
// 	 */
// 	static void describe(RegisteredComponentDescriptor& d);
//
// 	/**
// 	 * \brief This function is called after all linked objects have been
// 	 *        configured
// 	 */
// 	virtual void postConfigureInitialization();
//
// 	/**
// 	 * \brief Returns an instance of the ComponentUI class that handles the
// 	 *        viewers of this component
// 	 *
// 	 * \return a new-allocated instance of the class handling the viewers of
// 	 *         this component
// 	 */
// 	virtual ComponentUI* getUIManager();
//
// 	/**
// 	 * \brief The function running the evolutionary process
// 	 */
// 	virtual void runEvolution();
//
// 	/**
// 	 * \brief The function to resume an interrupted evolution
// 	 *
// 	 * \return true if an evolution process has been resumed, false
// 	 *         otherwise
// 	 */
// 	virtual bool resumeEvolution();
//
// private:
// 	virtual void seedChanged(unsigned int oldSeed);
//
// 	QVector<EvorobotIntGenotype> m_population;
// 	EvorobotGAStatistics m_statistics;
//
//
//
//
//
//
//
//
//
// 	void saveStatistics(int popSize, int rp);
//
//     /*! Main function of the Genetic Algorithm (Generational, Truncation Selection Version)
//      * The size of the population is set equal to nreproducing*noffspring
//      * Each individual is allowed to generate a variable number of offspring (noffspring parameter)
//      * Every generation the best "nreproducing" individuals are allowed to reproduce
//      * At reproduction the entire population is replaced by offspring unless the elitism parameter is greated than 0
//      * When elitism is used, each individual is allowed to produce one or more offspring without mutations (i.e. identical copies)
//      */
//     void evolveGenerational();
//
// 	void saveBestFitness();
//
//     /*! Identify the best individuals and copy their genotypes into the bestgenome Matrix
//      *  It also save the best genotypes in the Bx.gen files
//      */
//     void  reproduce();
//     /*! Identify the best individuals and copy their genotypes into the bestgenome Matrix
//      *  It also save the best genotypes in the Bx.gen files
//      *  Assume that the best individuals have the lowest fitness (minimization)
//      *  This function is not used and it is to be removed
//      */
//     void  mreproduce();
//     /*! Copy the genome of one individual to the bestgenome matrix in a specific position
//      *
//      *  \param fromgenome The id of the individual genome to be copies
//      *  \param tobestgenome The position in which the genome should be copied
//      */
//     void putGenome(int fromgenome, int tobestgenome);
//     /*! Copy a genome stored in the bestgenome matrix into the genome of the current population
//      *  Apply mutation during the copy
//      *
//      *  \param frombestgenome The id of the genome in the best genome matrix
//      *  \param togenome The id of the individual that should receive the genome
//      *  \param mut A 0/1 flag that specify whether mutation should be applied or not
//      */
//     void getGenome(int frombestgenome, int togenome, int mut);
//     /*! Mutate one gene
//      *  Since genes are store as integer, this function first trasform them into 8 bits
//      *  and then mutate (i.e. flip) each bit with a certain probability
//      *
//      *  \param w The integer storing the gene value
//      *  \param mut The mutation probability (i.e. the probability that each bit of the gene is flipped)
//      */
//     int mutate(int w, double mut);
//     /*! Randomize the genome of the population
//      *
//      */
//     void randomizePop();
//     /*! Overwrite the genome of all individuals with a manually specify vector of parameters (loaded from a .phe file)
//      *  This enable to manually fix some of the parameters
//      *  The Evonet::DEFAULT_VALUE value is used to indicate the parameters are not fixed and that are replaced with
//      *  randomly generated value
//      *
//      *  \param int* Pointer to the vector of parameter to be used to overwrite genes
//      */
//     void setInitialPopulation(int*);
//     /*! Set the mutation rate of each parameter
//      *
//      *  \param float* Pointer to the vector of mutation rate to be used
//      */
//     void setMutations(float*);
//     /*! Save the genome of one individual in a text file
//      *
//      *  \param *fp The pointer to .gen file
//      *  \param ind The id of the individual
//      */
//     void saveagenotype(FILE *fp, int ind);
//     /*! Load the genome of one individual in a text file
//      *
//      *  \param *fp The pointer to .gen file
//      *  \param ind The id of the individual
//      */
//     void loadgenotype(FILE *fp, int ind);
//     /*! Save the genome of the current population in a G?S?.gen file
//      *
//      */
//     void saveallg();
//
//     /*! Save the composed genome of the current population in a G?S?.composed.gen file
//      *
//      */
//     void saveallgComposed(QVector< QVector<int> > composedGen);
//
//     /*! Load the genome of the population from a file
//      *
//      *  \param gen The generation number used to create the GxS?.gen file name. If gen=0 the file name should be passed explicitly
//      *  \param ind The filename from which the genome should be load. When gen>0 this parameter is ignored
//      */
//     int loadallg(int gen, const char *filew);
//
//
//     /*! Load the teams of the population from a file
//      *
//      *  \param gen The generation number used to create the GxS?.composed.gen file name. If gen=0 the file name should be passed explicitly
//      *  \param ind The filename from which the teams should be load. When gen>0 this parameter is ignored
//      */
//     void loadallTeams(int gen, const char *filew, QVector< QVector<int> > &teams);
//     /*! Calculate the average, minimal and maximal fitness of the current population
//      *  Assume that the fitness is always positive
//      */
//     void computeFStat();
//     /*! Calculate the average, minimal and maximal fitness of the current population
//      * Assume that the fitness can also be negative
//      */
//     void computeFStat2();
//     /*! Save the average, minimal and maximal fitness by appending a line to the statS%d.fit file
//      *
//      */
//     void saveFStat();
//     /*! return the last value of min, max and average fitness
//      *  \param min in this parameter will be returned the minimum fitness
//      *  \param max in this parameter will be returned the maximum fitness
//      *  \param average in this parameter will be returned the average fitness
//      */
//     void getLastFStat( double &min, double &max, double &average );
//     /*! Load the fitness statistics from a .fit file
//      *  Return the number of loaded individuals (0 if the file does not exists)
//      *
//      *  \param *filename The filename
//      */
//     int loadStatistics(char *filename);
//     /*! Return a pointer to the genome of one individual of the current population
//      *
//      *  \param ind The id of the individual
//      */
//     int* getGenes(int ind);
//     /*! Return a pointer to the genome of one individual stored in the bestgenome matrix
//      *
//      *  \param ind The id of the individual
//      */
//     int* getBestGenes(int ind);
//     /*! Reset the generation counter to 0
//      */
//     void resetGenerationCounter();
//     /*! Copy the genotype of an individual into the genotype of a second individual
//      *  and introduce mutations
//      *  This function is used by the steadystate genetic algorithm to store the child in the genome matrix
//      *  at position population+1 and eventually to use the genome of the child to overwrite the genome
//      *  of the worst individual or discarded
//      *
//      *  \param from The id of the individual genome to reproduce
//      *  \param to The id of the location where the reproduced individual is stored
//      *  \param mut A binary flag that specify whether mutation should be introduced of not
//      */
//     void copyGenes(int from, int to, int mut);
//     /*! Append the genome of the best individual of the current population to the B%dS%d.gen file
//      */
//     void saveBestInd();
//
//     /*! Append the best team of the current population to the B%dS%d.composed.gen file
//      *  \param teams Vector with the selected teams
//      *  \param fitness Vector with the fitness of each team
//      */
//     void saveBestTeam(QVector< QVector<int> > teams, QVector<double> fitness);
//
//     /**
//      * \brief Evolves all replications of the ga process
//      */
//     virtual void evolveAllReplicas();
//
//     /**
//      * \brief Returns the current generation
//      *
//      * \return the current generation
//      */
//     virtual unsigned int getCurrentGeneration();
//
//     /**
//      * \brief Returns the starting seed
//      *
//      * \return the starting seed
//      */
//     virtual unsigned int getStartingSeed();
//
//     /**
//      * \brief Returns the current seed
//      *
//      * \return the current seed
//      */
//     virtual unsigned int getCurrentSeed();
//
//     /**
//      * \brief Returns the number of generations to do
//      *
//      * \return the number of generations
//      */
//     virtual unsigned int getNumOfGenerations();
//
//     /**
//      * \brief Returns the current mutation rate
//      * \note this can differ from generation to generation in the case of steadyState evolution
//      * \return the current mutation rate
//      */
//     virtual double getCurrentMutationRate();
//
//     /**
//      * \brief Set the current mutation rate to use
//      * \param mutation_rate the new mutation rate to use
//      */
//     virtual void setCurrentMutationRate( double mutation_rate );
//
//     /**
//      * \brief Loads genotypes from the given file
//      *
//      * \param filename the name of the file to load
//      * \return the number of individuals that have been loaded, 0 in case of
//      *         errors
//      */
//     virtual unsigned int loadGenotypes(QString filename);
//
//     /**
//      * \brief Returns number of loaded genotypes
//      *
//      * \return the number of individuals that have been loaded
//      */
//     virtual unsigned int numLoadedGenotypes() const;
//
//     /**
//      * \brief Returns the array of genes for the given individual
//      *
//      * \param ind the individual for whom genes are requested
//      * \return the array of genes for the given individual
//      */
//     virtual int* getGenesForIndividual(unsigned int id);
//
//     /**
//      * \brief Returns the name of the file with statistics (fitness) for the
//      *        given seed
//      *
//      * \param seed the seed from which the filename should be returned
//      * \return the name of the file with statistics (fitness) for the given
//      *         seed
//      */
//     virtual QString statisticsFilename(unsigned int seed);
//
//     /**
//      * \brief Returns the name of the file with the best genomes for the
//      *        given seed
//      *
//      * \param seed the seed from which the filename should be returned
//      * \return the name of the file with the best genomes for the given
//      *         seed
//      */
//     virtual QString bestsFilename(unsigned int seed);
//
//     /**
//      * \brief Returns the template name (regular expression) for "best genomes" files
//      */
//     virtual QString bestsFilename();
//
//     /**
//      * \brief Returns the name of the file containing the given generation for the
//      *        given seed
//      *
//      * \param seed the seed from which the filename should be returned
//      * \param generation the generation of interest
//      * \return the name of the file with genomes for the given
//      *         seed and generation
//      */
//     virtual QString generationFilename(unsigned int generation, unsigned int seed);
//
//     /**
//      * \brief Returns the template name (regular expression) for "generation" files
//      */
//     virtual QString generationFilename();
//
//     /**
//      * \brief Returns the name of the file with statistics (retentions) for the
//      *        given seed
//      *
//      * \param seed the seed from which the filename should be returned
//      * \return the name of the file with statistics (retentions) for the given
//      *         seed
//      */
//     virtual QString retentionsFilename(unsigned int seed);
//
//     //! population size
//     int popSize;
//     //! genome lenght (number of free parameters)
//     int glen;
//     //! if the Evoga will use elitism or not
//     bool elitism;
//     //! number of individuals allowed to reproduce
//     int nreproducing;
//     //! number of sons
//     int noffspring;
//
//     /**
//      * \brief A class modelling a population of genomes
//      * \internal
//      */
//     class Population {
//     public:
//         Population() :
//             m_pop(),
//             m_genomelength(1)
//         {
//         }
//
//         ~Population()
//         {
//             clear();
//         }
//
//         void setGenomeLength(int genomelength)
//         {
//             if (genomelength < 1) {
//                 genomelength = 1;
//             }
//             m_genomelength = genomelength;
//
//             clear();
//         }
//
//         int getGenomeLength() const
//         {
//             return m_genomelength;
//         }
//
//         void resize(int newSize)
//         {
//             if (newSize < 0) {
//                 newSize = 0;
//             }
//
//             // We have to be careful clearing or allocating memory
//             if (newSize < m_pop.size()) {
//                 for (int i = (m_pop.size() - 1); i >= newSize; i--) {
//                     delete[] m_pop[i];
//                 }
//                 m_pop.resize(newSize);
//             } else if (newSize > m_pop.size()) {
//                 const int oldSize = m_pop.size();
//                 m_pop.resize(newSize);
//                 for (int i = oldSize; i < newSize; i++) {
//                     m_pop[i] = new int[m_genomelength];
//                 }
//             }
//         }
//
//         int addOne()
//         {
//             m_pop.append(new int[m_genomelength]);
//
//             // Returning the index of the new genome
//             return (m_pop.size() - 1);
//         }
//
//         void clear()
//         {
//             resize(0);
//         }
//
//         int size() const
//         {
//             return m_pop.size();
//         }
//
//         int* operator[](int i)
//         {
// #ifdef FARSA_DEBUG
//             if (i > m_pop.size()) {
//                 abort();
//             }
// #endif
//             return m_pop[i];
//         }
//
//         const int* operator[](int i) const
//         {
// #ifdef FARSA_DEBUG
//             if (i > m_pop.size()) {
//                 abort();
//             }
// #endif
//             return m_pop[i];
//         }
//
//     private:
//         QVector<int *> m_pop;
//         int m_genomelength;
//     };
//     //! The matrix that contain the genome of the corrent population
//     Population genome;
//     //! The matrix that contains a copy of the genome of the best individuals
//     Population bestgenome;
//     //! The number of individual genome loaded from a .gen file into the genome matrix (-1 when none has been loaded)
//     int loadedIndividuals;
//     //! A pointer to a vector used to store the sum of the fitness obtained during different evaluations of an unmodified individual
//     double *tfitness;
//     //! A pointer to a vector used to store the total number of trials in which an unchanged individuals have been evaluated
//     double *ntfitness;
//     //! A matrix that store the statistics (average, min, and max fitness) for all generations
//     double **statfit;
//     //! The number of generations specified with the ngenerations parameter
//     int nogenerations;
//     //! The number of replications specified with the corresponding parameter
//     int nreplications;
//     //! The number used to initialized the seed of the first replication (successive replication use incremented numbers)
//     int seed;
//     //! The seed of the current generation (used for example to generate the name of the G?S?.gen files)
//     int currentSeed;
//     //! The mutation rate can vary from 0 to 1 continuosly to express probability in the range [0, 100%]
//     double mutation;
//     /*! The amount of reduction of the mutation generations during the first generations
//      *  until it reaches the specified mutation rate
//      */
//     double mutationdecay;
//     //! The initial mutation rate setted when steadyState algorithm is used
//     double initial_mutation;
//     //! vector of parameter-specific mutations
//     float *mutations;
//     //! current generation
//     int cgen;
//     //! save best savebest parents by default equals 1
//     int savebest;
//     //! fitness statistics (minimum, maximum, and average fitness)
//     double fmin, fmax,faverage;
// 	//! fitness best
// 	double fbest;
// 	//! fitness best generation (maximum)
// 	int fbestgen;
//     //! The current cycle
//     int ccycle;
//     //! How often (i.e. how many generations) we want to same the population genome in a .gen file
//     int savePopulationEachNGenerations;
//
// 	//! How often (in terms of number of generations) the best phenotype must be saved
// 	int pheGen;
// };
//
// } // end namespace farsa
//
// #endif
