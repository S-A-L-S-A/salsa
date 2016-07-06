/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2008 Gianluca Massera <emmegian@yahoo.it>                *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

#ifndef MUTATION_H
#define MUTATION_H

#include "gaconfig.h"
#include "parametersettable.h"
#include <QMutex>

namespace farsa {

class Genotype;
class Genome;
class GeneticAlgo;

/*!  \brief Mutation Operator Interface
 *
 *  \par Description
 *    Interface of the Mutation Operator. This allows setting different mutation
 *    rates for different genes (only programmatically) and also automatically
 *    decreasing mutation rates during generations.
 *  \par Warnings
 *    The sign of the variation for the mutation rate is modified so to tend to
 *    the final mutation rate. This means that if the initial mutation rate is
 *    greater than the final mutation rate, variation is made negative, otherwise
 *    it is positive.
 *
 * \ingroup ga_core
 */
class FARSA_GA_API Mutation : public ParameterSettableWithConfigureFunction {
public:
	/*! Constructor */
	Mutation();

	/*! Destructor */
	virtual ~Mutation();

	/*! Mutate the Genotype
	 *  \warning It modifies the Genotype passed
	 */
	virtual void mutate( Genotype* ) = 0;

	/*!
	 * Set the mutation rate, i.e. the probability to apply a mutation.
	 * The mutation rate can either be set for the whole genotype or for parts
	 * of it. If start and end are not given, the rate is set for the whole
	 * genotype. If start is given, then the rate is set from that bit on to
	 * end. If length is also given, the rate is set from the start for length
	 * number of bits. This sets both starting and ending mutation rates
	 * \note The argument start and length are referencing bits in the genotype
	 * 		 and not genes. Use the helper methods geneToBitIndex and
	 * 		 bitToGeneIndex of the Genotype class to get the correct indices.
	 */
	void setMutationRate( double rate, int start = 0, int length = -1);

	/*!
	 * Like the other setMutationRate function, but this allows setting the
	 * initial rate, the final rate and the variation
	 */
	void setMutationRate( double initialRate, double finalRate, double variation, int start = 0, int length = -1);

	/*!
	 * Returns the current mutation rate for the specified bit of the genotype.
	 * This also modifies the current mutation rate if it has to
	 * \note The argument is referencing bits in the genotype and not genes.
	 *       Use the helper methods geneToBitIndex and bitToGeneIndex of the
	 *       Genotype class to get the correct indices.
	 */
	double mutationRate( int bit );

	/*!
	 * Returns the initial mutation rate for the specified bit of the genotype
	 * \note The argument is referencing bits in the genotype and not genes.
	 *       Use the helper methods geneToBitIndex and bitToGeneIndex of the
	 *       Genotype class to get the correct indices.
	 */
	double initialMutationRate( int bit );

	/*!
	 * Returns the final mutation rate for the specified bit of the genotype
	 * \note The argument is referencing bits in the genotype and not genes.
	 *       Use the helper methods geneToBitIndex and bitToGeneIndex of the
	 *       Genotype class to get the correct indices.
	 */
	double finalMutationRate( int bit );

	/*!
	 * Returns the variation of the mutation rate for the specified bit of the genotype
	 * \note The argument is referencing bits in the genotype and not genes.
	 *       Use the helper methods geneToBitIndex and bitToGeneIndex of the
	 *       Genotype class to get the correct indices.
	 */
	double variationMutationRate( int bit );
	
	/*! Set the Genome where Genotypes lives */
	void setGenome( const Genome* g ) {
		genomev = g;
	}

	/*! Return the Genome */
	const Genome* genome() {
		return genomev;
	}

	/*! Set the GeneticAlgo used */
	void setGA( GeneticAlgo* g ) {
		ga = g;
	}

	/*! Return the GeneticAlgo used */
	GeneticAlgo* GA() {
		return ga;
	}

	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with mutation parameters
	 */
	virtual void configure( ConfigurationParameters& params, QString prefix );

	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 *
	 * \param params the configuration parameters object on which save actual parameters
	 * \param prefix the prefix to use to access the object configuration parameters.
	 */
	virtual void save( ConfigurationParameters& params, QString prefix );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );

protected:
	/*! Genome where Genotypes lives */
	const Genome* genomev;

	/*! GeneticAlgo used */
	GeneticAlgo* ga;

private:
	// Here we have all mutation rates stuffs, to force even child classes
	// to use functions, which will keep everything in a consistent state

	/*! Updates the mutation rates to the value for the current generation */
	void updateMutationRates();

	/*! The structure with information about the mutation rate for a genotype */
	struct MutationRate {
		/*! Constructor */
		MutationRate() :
			initial(0.0),
			final(0.0),
			variation(0.0),
			mutaRate(0.0)
		{
		}

		/*! Computes the mutation rate for the given generation */
		void rateForGeneration(unsigned int gen);

		/*! The initial mutation rate */
		double initial;

		/*! The final mutation rate */
		double final;

		/*! The variation for the mutation rate for each generation */
		double variation;

		/*! The current mutation rate */
		double mutaRate;
	};

	/*!
	 * Returns a const reference to the MutationRate object for the given bit
	 * \note The argument is referencing bits in the genotype and not genes.
	 */
	const MutationRate& getMutationRateForBit( int bit ) const;

	/*! The mutation rates for different parts of the genotype */
	QMap<int, MutationRate> mutaRates;

	/*! Mutex for making access to mutaRates thread-safe */
	QMutex mutaRatesMutex;

	/*! The last generation during which mutation rates changed */
	unsigned int lastGenMutaRatesChange;
};

} // end namespace farsa

#endif
