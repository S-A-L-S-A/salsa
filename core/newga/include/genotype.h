/*******************************************************************************
 * FARSA New Genetic Algorithm Library                                        *
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

#ifndef GENOTYPE_H
#define GENOTYPE_H

#include "newgaconfig.h"
#include "parametersettable.h"
#include "randomgenerator.h"
#include "logger.h"
#include <QTextStream>
#include <cstdlib>

namespace farsa {

/**
 * \brief The genotype
 *
 * COMMENT HERE
 */
class FARSA_NEWGA_TEMPLATE Genotype : public ParameterSettableInConstructor
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
	Genotype(ConfigurationParameters& params, QString prefix)
		: ParameterSettableInConstructor(params, prefix)
	{
	}
	/**
	 * \brief Destructor
	 */
	virtual ~Genotype()
	{
	}

	virtual Genotype* clone() const = 0;

	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters
	 *        object passed
	 *
	 * This is not implemented, a call to this function will cause an abort
	 * \param params the configuration parameters object on which save actual
	 *               parameters
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters.
	 */
	virtual void save(ConfigurationParameters& /*params*/, QString /*prefix*/)
	{
		Logger::error("NOT IMPLEMENTED (EvoAlgoTestIndividual::save)");
		abort();
	}

	/**
	 * \brief Load the genotype
	 * \param in the input stream
	 */
	virtual bool loadGen(QTextStream& in) = 0;
	/**
	 * \brief Save the genotype
	 * \param out the output stream
	 */
	virtual bool saveGen(QTextStream& out) = 0;
	/**
	 * \brief Set the fitness of the genotype
	 * \param f the fitness
	 */
	virtual void setFitness(const float f) = 0;
	/**
	 * \brief Return the fitness of the genotype
	 */
	virtual float getFitness() const = 0;
	/**
	 * \brief Return the length of the genotype
	 */
	virtual int getLength() const = 0;
	/**
	 * \brief Set the length of the genotype
	 */
	virtual void setLength(const int len) = 0;
protected:
	Genotype() :
		ParameterSettableInConstructor()
	{
	}
};

/**
 * \brief The floating-point genotype
 *
 * COMMENT HERE
 */
class FARSA_NEWGA_API GenotypeFloat : public Genotype
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
    GenotypeFloat(ConfigurationParameters& params, QString prefix);
    /**
	 * \brief Destructor
	 */
    ~GenotypeFloat();

	GenotypeFloat* clone() const;

	static void describe(QString type);
	/**
	 * \brief Load the genotype
	 * \param in the input stream
	 */
	bool loadGen(QTextStream& in);
	/**
	 * \brief Save the genotype
	 * \param out the output stream
	 */
	bool saveGen(QTextStream& out);
	/**
	 * \brief Initialise the genotype with random values
	 * \param min the lowest value an element of the genotype could have
	 * \param max the highest value an element of the genotype could have
	 */
	void initRandom(const float min, const float max);
	/**
	 * \brief Initialise the genotype with values drawn from a Gaussian
	 *        distribution with mean <mean> and standard deviation <stdDev>
	 * \param mean the mean of the Gaussian distribution
	 * \param stdDev the standard deviation of the Gaussian distribution
	 */
	void initGaussian(const float mean, const float stdDev);
	/**
	 * \brief Set the fitness of the genotype
	 * \param f the fitness
	 */
	void setFitness(const float f)
	{
		m_fitness = f;
	}
	/**
	 * \brief Return the fitness of the genotype
	 */
	float getFitness() const
	{
		return m_fitness;
	}
	/**
	 * \brief Return the length of the genotype
	 */
	int getLength() const
	{
		return m_genLength;
	}
	/**
	 * \brief Set the length of the genotype
	 */
	void setLength(const int len)
	{
		m_gen.resize(len);
		m_genLength = len;
	}
	/**
	 * \brief Set the i-th element of the genotype
	 * \param i the index of the element
	 * \param val the new value of the element
	 */
	void setGene(const int i, const float val)
	{
		m_gen[i] = val;
	}
	/**
	 * \brief Get the i-th element of the genotype
	 * \param i the index of the element
	 */
	float getGene(const int i) const
	{
		return m_gen[i];
	}
	/**
	 * \brief Return the seed
	 */
	int getSeed() const
	{
		return m_seed;
	}
protected:
	GenotypeFloat() :
		Genotype()
	{
	}
private:
	//! The genotype
	QVector<float> m_gen;
	//! The length of the genotype
	int m_genLength;
	//! The fitness of the genotype
	float m_fitness;
	//! The seed to be used to initialise the random number generator
	int m_seed;
};

/**
 * \brief The integer-point genotype
 *
 * COMMENT HERE
 */
class FARSA_NEWGA_API GenotypeInt : public Genotype
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
    GenotypeInt(ConfigurationParameters& params, QString prefix);
    /**
	 * \brief Destructor
	 */
    ~GenotypeInt();

	GenotypeInt* clone() const;

	static void describe(QString type);
	/**
	 * \brief Load the genotype
	 * \param in the input stream
	 */
	bool loadGen(QTextStream& in);
	/**
	 * \brief Save the genotype
	 * \param out the output stream
	 */
	bool saveGen(QTextStream& out);
	/**
	 * \brief Initialise the genotype with random values
	 * \param min the lowest value an element of the genotype could have
	 * \param max the highest value an element of the genotype could have
	 */
	void initRandom(const int min, const int max);
	/**
	 * \brief Set the fitness of the genotype
	 * \param f the fitness
	 */
	void setFitness(const float f)
	{
		m_fitness = f;
	}
	/**
	 * \brief Return the fitness of the genotype
	 */
	float getFitness() const
	{
		return m_fitness;
	}
	/**
	 * \brief Return the length of the genotype
	 */
	int getLength() const
	{
		return m_genLength;
	}
	/**
	 * \brief Set the length of the genotype
	 */
	void setLength(const int len)
	{
		m_gen.resize(len);
		m_genLength = len;
	}
	/**
	 * \brief Set the i-th element of the genotype
	 * \param i the index of the element
	 * \param val the new value of the element
	 */
	void setGene(const int i, const int val)
	{
		m_gen[i] = val;
	}
	/**
	 * \brief Get the i-th element of the genotype
	 * \param i the index of the element
	 */
	int getGene(const int i) const
	{
		return m_gen[i];
	}
	/**
	 * \brief Return the seed
	 */
	int getSeed() const
	{
		return m_seed;
	}
protected:
	GenotypeInt() :
		Genotype()
	{
	}
private:
	//! The genotype
	QVector<int> m_gen;
	//! The length of the genotype
	int m_genLength;
	//! The fitness of the genotype
	float m_fitness;
	//! The seed to be used to initialise the random number generator
	int m_seed;
};

} // end namespace farsa

#endif
