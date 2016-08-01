/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
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

#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include "utilitiesconfig.h"

namespace salsa {

class RandomGenerator;
class RandomGeneratorPrivate;

/*! Global Random Generator
 *  \warning this may be not-thread safe
 * \ingroup utilities_rng
 */
extern SALSA_UTIL_API RandomGenerator* globalRNG;

/**
 * \brief Random number generator
 *
 * The class to generate random numbers
 *
 * \ingroup utilities_rng
 */
class SALSA_UTIL_API RandomGenerator
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param seed the seed of the random number generator
	 */
	RandomGenerator(unsigned int seed = 0);

	/**
	 * \brief Destructor
	 */
	~RandomGenerator();

	/**
	 * \brief Sets the seed of this random number generator
	 *
	 * \param seed the new seed
	 */
	void setSeed(unsigned int seed);

	/**
	 * \brief Returns the seed of this random number generator
	 *
	 * \return the seed of this random number generator
	 */
	unsigned int seed() const;

	/**
	 * \brief Returns a random boolean value
	 *
	 * \param trueProbability the probability that the returned value is
	 *                        true
	 * \return a random boolean value
	 */
	bool getBool(double trueProbability);

	/**
	 * \brief Returns a random integer
	 *
	 * The value is taken from a flat distribution with the given range
	 * (bounds are included)
	 * \param min the lower bound of the range
	 * \param max the upper bound of the range
	 * \return a random integer in the given range
	 */
	int getInt(int min, int max);

	/**
	 * \brief Returns a random double
	 *
	 * The value is taken from a flat distribution with the given range
	 * (bounds are included)
	 * \param min the lower bound of the range
	 * \param max the upper bound of the range
	 * \return a random double in the given range
	 */
	double getDouble(double min, double max);

	/**
	 * \brief Returns a random double taken from a gaussian distribution
	 *
	 * \param var the variance of the gaussian distribution
	 * \param mean the centre of the gaussian distribution
	 * \return a random double taken from a gaussian distribution
	 */
	double getGaussian(double var, double mean = 0.0);

private:
	RandomGeneratorPrivate* const m_priv;
	unsigned int m_seed;
};

} // end namespace salsa

#endif
