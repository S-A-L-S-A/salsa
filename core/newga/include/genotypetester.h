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

#ifndef GENOTYPETESTER_H
#define GENOTYPETESTER_H

#include "newgaconfig.h"
#include "genotype.h"
#include "evonet.h"
#include "robotexperiment.h"
#include <QList>
#include <cstdlib>

namespace salsa {

/**
 * \brief The interface for classed that test genotypes
 *
 * The typical usage for subclasses is to take genotypes, tranform them into
 * something that can be fed to an experiment (e.g. a neural network) and pass
 * the generated objects to an ExperimentInput subclass. Some of the subclasses
 * also provide methods to get the requested genotype length
 */
class SALSA_NEWGA_TEMPLATE GenotypeTester : public ParameterSettableInConstructor
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
	GenotypeTester(ConfigurationParameters& params, QString prefix)
		: ParameterSettableInConstructor(params, prefix)
	{
	}
	/**
	 * \brief Destructor
	 *
	 * This is here just to declare it virtual
	 */
	virtual ~GenotypeTester()
	{
	}

	virtual void save(ConfigurationParameters& /*params*/, QString /*prefix*/)
	{
		Logger::error("NOT IMPLEMENTED (GenotypeTester::save)");
		abort();
	}
};

/**
 * \brief The interface for genotype testers that test a single genotype
 *
 * This class has also a method that returns the requested length of a genotype,
 * requestedGenotypeLength()
 */
class SALSA_NEWGA_TEMPLATE SingleGenotypeTester : public GenotypeTester
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
	SingleGenotypeTester(ConfigurationParameters& params, QString prefix)
		: GenotypeTester(params, prefix)
	{
	}
	/**
	 * \brief Destructor
	 */
	virtual ~SingleGenotypeTester()
	{
	}

	/**
	 * \brief Sets the genotype to test
	 *
	 * \param g the genotype to test
	 */
	virtual void setGenotype(Genotype* g) = 0;

	/**
	 * \brief Returns the requested size of the genotype
	 *
	 * \return the requested size of the genotype
	 */
	virtual int requestedGenotypeLength() const = 0;
};

/**
 * \brief The interface for genotype testers that test multiple genotypes at
 *        once
 *
 * This class has also a method that returns the requested length of a genotype,
 * requestedGenotypeLength(). The assumption is that all genotypes have the same
 * length
 */
class SALSA_NEWGA_TEMPLATE MultipleGenotypesTester : public GenotypeTester
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
	MultipleGenotypesTester(ConfigurationParameters& params, QString prefix)
		: GenotypeTester(params, prefix)
	{
	}
	/**
	 * \brief Destructor
	 */
	virtual ~MultipleGenotypesTester()
	{
	}

	/**
	 * \brief Sets the list of genotypes to test
	 *
	 * \param g the list of genotypes to test
	 */
	virtual void setGenotypes(QList<Genotype*> g) = 0;

	/**
	 * \brief Returns the requested size of the genotype
	 *
	 * All genotypes have the same length
	 * \return the requested size of the genotype
	 */
	virtual int requestedGenotypeLength() const = 0;
};

class SALSA_NEWGA_API SingleGenotypeFloatToEvonet : public SingleGenotypeTester
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
	SingleGenotypeFloatToEvonet(ConfigurationParameters& params, QString prefix);
	/**
	 * \brief Destructor
	 */
	~SingleGenotypeFloatToEvonet();

	static void describe(QString type);

	/**
	 * \brief Sets the genotype to test
	 *
	 * \param g the genotype to test
	 */
	void setGenotype(Genotype* g);

	/**
	 * \brief Returns the requested size of the genotype
	 *
	 * \return the requested size of the genotype
	 */
	int requestedGenotypeLength() const;
private:
	//! The evaluator
	RobotExperiment* m_gae;
	//! The network
	Evonet* m_evonet;
};

class SALSA_NEWGA_API SingleGenotypeIntToEvonet : public SingleGenotypeTester
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
	SingleGenotypeIntToEvonet(ConfigurationParameters& params, QString prefix);
	/**
	 * \brief Destructor
	 */
	~SingleGenotypeIntToEvonet();

	static void describe(QString type);

	/**
	 * \brief Sets the genotype to test
	 *
	 * \param g the genotype to test
	 */
	void setGenotype(Genotype* g);

	/**
	 * \brief Returns the requested size of the genotype
	 *
	 * \return the requested size of the genotype
	 */
	int requestedGenotypeLength() const;
private:
	//! The evaluator
	RobotExperiment* m_gae;
	//! The network
	Evonet* m_evonet;
};

} // end namespace salsa

#endif
