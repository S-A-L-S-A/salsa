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

#include "randomgenerator.h"
#include <cmath>
#include <cstdlib>

#ifdef FARSA_USE_GSL
	#include "gsl/gsl_rng.h"
	#include "gsl/gsl_randist.h"
#endif

namespace farsa {

class FARSA_UTIL_INTERNAL RandomGeneratorPrivate
{
public:
#ifdef FARSA_USE_GSL
	gsl_rng* const rng;

	RandomGeneratorPrivate()
		: rng(gsl_rng_alloc(gsl_rng_taus2))
	{
	}

	~RandomGeneratorPrivate()
	{
		gsl_rng_free( rng );
	}

#else
	RandomGeneratorPrivate()
		: spare(0.0)
		, isSpareReady(false)
	{
	}

	double spare;
	bool isSpareReady;
#endif
};

RandomGenerator::RandomGenerator(unsigned int seed)
	: m_priv(new RandomGeneratorPrivate())
	, m_seed(0)
{
	setSeed(seed);
}

RandomGenerator::~RandomGenerator()
{
	delete m_priv;
}

void RandomGenerator::setSeed(unsigned int seed)
{
	m_seed = seed;
#ifdef FARSA_USE_GSL
	gsl_rng_set(m_priv->rng, seed);
#else
	srand(seed);
#endif
}

unsigned int RandomGenerator::seed() const
{
	return m_seed;
}

bool RandomGenerator::getBool(double trueProbability)
{
#ifdef FARSA_USE_GSL
	return gsl_rng_uniform(m_priv->rng) < trueProbability;
#else
	return (double(rand()) / double(RAND_MAX)) < trueProbability;
#endif
}

int RandomGenerator::getInt(int min, int max)
{
#ifdef FARSA_USE_GSL
	return gsl_rng_uniform_int(m_priv->rng, std::abs(max - min) + 1) + min;
#else
	return min + (rand() % (max - min));
#endif
}

double RandomGenerator::getDouble(double min, double max)
{
#ifdef FARSA_USE_GSL
	// FIXME: this implementation never returns max
	return gsl_ran_flat(m_priv->rng, min, max);
#else
	return min + (double(rand()) / double(RAND_MAX)) * (max - min);
#endif
}

double RandomGenerator::getGaussian(double var, double mean)
{
#ifdef FARSA_USE_GSL
	return gsl_ran_gaussian(m_priv->rng, var) + mean;
#else
	// Using Marsaglia polar method to convert uniform random numbers to gaussian. Implementation
	// adapted from http://en.wikipedia.org/wiki/Marsaglia_polar_method#Implementation

	const double stdDev = sqrt(var);
	if (m_priv->isSpareReady) {
		m_priv->isSpareReady = false;
		return m_priv->spare * stdDev + mean;
	} else {
		double u, v, s;
		do {
			u = (double(rand()) / double(RAND_MAX)) * 2.0 - 1.0;
			v = (double(rand()) / double(RAND_MAX)) * 2.0 - 1.0;
			s = u * u + v * v;
		} while (s >= 1.0 || s == 0.0);

		const double mul = sqrt(-2.0 * log(s) / s);
		m_priv->spare = v * mul;
		m_priv->isSpareReady = true;
		return mean + stdDev * u * mul;
	}
#endif
}

} // end namespace farsa
