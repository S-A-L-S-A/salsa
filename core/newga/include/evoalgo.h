/*******************************************************************************
 * SALSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#ifndef EVOALGO_H
#define EVOALGO_H

#include "newgaconfig.h"
#include "parametersettable.h"
#include "logger.h"
#include "flowcontrol.h"
#include <QTextStream>
#include <QVector>
#include <cstdlib>

namespace salsa {

/**
 * \brief The interface for evolutionary algorithms
 *
 * This class inherits from FlowControlled, so subclasses should use the
 * stopFlow and pauseFlow functions to allow their execution flow to be
 * controlled
 */
class SALSA_NEWGA_TEMPLATE EvoAlgo : public ParameterSettableInConstructor, public FlowControlled
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the configuration parameters object with parameters to
	 *               use
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters. This is guaranteed to end with the
	 *               separator character when called by the factory, so you
	 *               don't need to add one
	 */
	EvoAlgo(ConfigurationParameters& params, QString prefix)
		: ParameterSettableInConstructor(params, prefix)
		, FlowControlled()
		, m_seed(1)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~EvoAlgo()
	{
	}

	/**
	 * \brief Saves the actual status of parameters into the
	 *        ConfigurationParameters object passed
	 *
	 * This is not implemented
	 * \param params the configuration parameters object on which save
	 *               actual parameters
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters.
	 */
	virtual void save(ConfigurationParameters& /*params*/, QString /*prefix*/)
	{
		Logger::error("NOT IMPLEMENTED (EvoAlgo::save)");
		abort();
	}

	/**
	 * \brief Sets the random seed to use
	 *
	 * \param seed the random seed to use
	 */
	void setSeed(unsigned int seed)
	{
		unsigned int oldSeed = m_seed;
		m_seed = seed;

		seedChanged(oldSeed);
	}

	/**
	 * \brief Returns the random seed to use
	 *
	 * \return the random seed to use
	 */
	unsigned int seed() const
	{
		return m_seed;
	}

	/**
	 * \brief Runs the evolutionary process
	 */
	virtual void runEvolution() = 0;
	
	/**
	 * \brief Saves the fitness statistics
	 *
	 * It writes the statistics (maximum, average and minimum fitness)
	 * to the file associated to the output stream. Furthermore, it might
	 * save the fitness of all the individuals (if the corresponding parameter
	 * <saveFitnessAllIndividuals> is set to true in the configuration file).
	 *
	 * \param fit the array containing the fitness values
	 * \param out the output stream (i.e., where the fitness values will be saved)
	 */
	virtual void saveFitStats(const QVector<float> fit, QTextStream& out) = 0;

protected:
	/**
	 * \brief Called when the seed is changed, after the value is changed
	 *
	 * To access the new seed, simply call the seed() functions. The default
	 * implementation does nothing
	 * \param oldSeed the old seed
	 */
	virtual void seedChanged(unsigned int oldSeed)
	{
		Q_UNUSED(oldSeed);
	}

private:
	/**
	 * \brief The random seed to use
	 */
	unsigned int m_seed;
};

} // end namespace salsa

#endif
