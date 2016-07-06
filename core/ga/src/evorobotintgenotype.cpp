/********************************************************************************
 *  FARSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#include "evorobotintgenotype.h"

namespace farsa {

EvorobotIntGenotype::EvorobotIntGenotype()
	: m_genotype()
	, m_fitness(0.0)
{
}

EvorobotIntGenotype::EvorobotIntGenotype(unsigned int length)
	: m_genotype(length)
	, m_fitness(0.0)
{
}

EvorobotIntGenotype::~EvorobotIntGenotype()
{
}

unsigned int EvorobotIntGenotype::length() const
{
	return m_genotype.size();
}

unsigned char& EvorobotIntGenotype::operator[](unsigned int i)
{
	return m_genotype[i];
}

unsigned char EvorobotIntGenotype::operator[](unsigned int i) const
{
	return m_genotype[i];
}

void EvorobotIntGenotype::resize(unsigned int s)
{
	m_genotype.resize(s);
}

void EvorobotIntGenotype::write(QTextStream& stream) const
{
	stream << "DYNAMICAL NN\n";

	for (int i = 0; i < m_genotype.size(); ++i) {
		stream << m_genotype[i] << "\n";
	}

	stream << "END\n";
}

bool EvorobotIntGenotype::read(QTextStream& stream)
{
	QString s;
	QVector<unsigned char> readGeotype;

	stream >> s;
	if (s != "DYNAMICAL") {
		return false;
	}
	stream >> s;
	if (s != "NN") {
		return false;
	}

	bool isInt;
	do {
		stream >> s;
		const unsigned int v = s.toUInt(&isInt);
		if (isInt) {
			if (v > 255) {
				return false;
			}
			readGeotype.append((unsigned char) v);
		}
	} while (isInt && (!stream.atEnd()));

	if (s != "END") {
		return false;
	}

	m_genotype = readGeotype;

	return true;
}

void EvorobotIntGenotype::setFitness(double f)
{
	m_fitness = f;
}

double EvorobotIntGenotype::getFitness() const
{
	return m_fitness;
}

bool EvorobotIntGenotype::operator<(const EvorobotIntGenotype& other) const
{
	return m_fitness < other.m_fitness;
}

} // end namespace farsa

