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

#include "genotype.h"
#include "configurationhelper.h"
#include "logger.h"
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <cstdlib>

namespace farsa {

GenotypeFloat::GenotypeFloat(ConfigurationParameters& params, QString prefix)
	: Genotype(params, prefix)
	, m_gen()
	, m_genLength(1)
	, m_fitness(0.0f)
	, m_seed(0)
{
	m_gen.resize(m_genLength);
	m_seed = ConfigurationHelper::getInt(params, prefix + QString("seed"), 0);
	globalRNG->setSeed(m_seed);
}

GenotypeFloat::~GenotypeFloat()
{
}

void GenotypeFloat::describe(QString type)
{
	Descriptor d = addTypeDescription(type, "Floating-point genotype");
	d.describeInt("genLength").limits(1, INT_MAX).def(1).help("Number of genes of the genotype");
	d.describeReal("fitness").def(0.0).help("The fitness of the genotype");
	d.describeObject("randomgenerator").type("RandomGenerator").props(IsMandatory).help("The random number generator");
}

GenotypeFloat* GenotypeFloat::clone() const
{
	GenotypeFloat* g = new GenotypeFloat();
	g->m_gen = m_gen;
	g->m_genLength = m_genLength;
	g->m_fitness = m_fitness;
	g->m_seed = m_seed;
	return g;
}

bool GenotypeFloat::loadGen(QTextStream& in)
{
	QString str;
	bool ok;
	//! Read the length of the genotype
	in >> str;
	int genLen = str.toInt(&ok);
	if (!ok || (in.status() != QTextStream::Ok))
	{
		return false;
	}
	else
	{
		m_genLength = genLen;
	}
	//! Read the gene values
	for (int i = 0; i < m_genLength; i++)
	{
		in >> str;
		float elem = str.toFloat(&ok);
		if (!ok || (in.status() != QTextStream::Ok))
		{
			return false;
		}
		else
		{
			m_gen[i] = elem;
		}
	}
	//! Read the fitness of the genotype
	in >> str;
	float f = str.toFloat(&ok);
	if (!ok || (in.status() != QTextStream::Ok))
	{
		return false;
	}
	else
	{
		m_fitness = f;
	}
	return true;
}

bool GenotypeFloat::saveGen(QTextStream& out)
{
	QString str;
	//! Write the length of the genotype
	str = QString::number(m_genLength);
	out << str;
	//! Write the gene values
	for (int i = 0; i < m_genLength; i++)
	{
		str = QString::number(m_gen[i]);
		out << " " << str;
	}
	//! Write the fitness of the genotype
	str = QString::number(m_fitness);
	out << " " << str;
	out << endl;
	return true;
}

void GenotypeFloat::initRandom(const float min, const float max)
{
	for (int i = 0; i < m_genLength; i++)
	{
		m_gen[i] = globalRNG->getDouble(min, max);
	}
}

void GenotypeFloat::initGaussian(const float mean, const float stdDev)
{
	for (int i = 0; i < m_genLength; i++)
	{
		m_gen[i] = globalRNG->getGaussian(stdDev, mean);
	}
}

GenotypeInt::GenotypeInt(ConfigurationParameters& params, QString prefix)
	: Genotype(params, prefix)
	, m_gen()
	, m_genLength(1)
	, m_fitness(0.0f)
	, m_seed(0)
{
	m_gen.resize(m_genLength);
	m_seed = ConfigurationHelper::getInt(params, prefix + QString("seed"), 0);
	globalRNG->setSeed(m_seed);
}

GenotypeInt::~GenotypeInt()
{
}

void GenotypeInt::describe(QString type)
{
	Descriptor d = addTypeDescription(type, "Integer genotype");
	d.describeInt("genLength").limits(1, INT_MAX).def(1).help("Number of genes of the genotype");
	d.describeReal("fitness").limits(0.0, 100.0).def(0.0).help("The fitness of the genotype");
	d.describeObject("randomgenerator").type("RandomGenerator").props(IsMandatory).help("The random number generator");
}

GenotypeInt* GenotypeInt::clone() const
{
	GenotypeInt* g = new GenotypeInt();
	g->m_gen = m_gen;
	g->m_genLength = m_genLength;
	g->m_fitness = m_fitness;
	g->m_seed = m_seed;
	return g;
}

bool GenotypeInt::loadGen(QTextStream& in)
{
	QString str;
	bool ok;
	//! Read the length of the genotype
	in >> str;
	int genLen = str.toInt(&ok);
	if (!ok || (in.status() != QTextStream::Ok))
	{
		return false;
	}
	else
	{
		m_genLength = genLen;
	}
	//! Read the gene values
	for (int i = 0; i < m_genLength; i++)
	{
		in >> str;
		int elem = str.toInt(&ok);
		if (!ok || (in.status() != QTextStream::Ok))
		{
			return false;
		}
		else
		{
			m_gen[i] = elem;
		}
	}
	//! Read the fitness of the genotype
	in >> str;
	float f = str.toFloat(&ok);
	if (!ok || (in.status() != QTextStream::Ok))
	{
		return false;
	}
	else
	{
		m_fitness = f;
	}
	return true;
}

bool GenotypeInt::saveGen(QTextStream& out)
{
	QString str;
	//! Write the length of the genotype
	str = QString::number(m_genLength);
	out << str;
	//! Write the gene values
	for (int i = 0; i < m_genLength; i++)
	{
		str = QString::number(m_gen[i]);
		out << " " << str;
	}
	//! Write the fitness of the genotype
	str = QString::number(m_fitness);
	out << " " << str;
	out << endl;
	return true;
}

void GenotypeInt::initRandom(const int min, const int max)
{
	for (int i = 0; i < m_genLength; i++)
	{
		m_gen[i] = globalRNG->getInt(min, max);
	}
}

} // end namespace farsa
