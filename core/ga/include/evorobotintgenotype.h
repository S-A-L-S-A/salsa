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

#ifndef EVOROBOTINTGENOTYPE_H
#define EVOROBOTINTGENOTYPE_H

#include "gaconfig.h"
#include <QVector>
#include <QTextStream>

namespace farsa {

/**
 * \brief A genotype storing genes as small integers
 *
 * This is compatible with evorobot genotype
 */
class FARSA_GA_API EvorobotIntGenotype
{
public:
	/**
	 * \brief Constructor
	 *
	 * The genotype has zero length
	 */
	EvorobotIntGenotype();

	/**
	 * \brief Constructor
	 *
	 * \param length the length of the genotype
	 */
	EvorobotIntGenotype(unsigned int length);

	/**
	 * \brief Destructor
	 */
	~EvorobotIntGenotype();

	/**
	 * \brief Returns the length of the genotype
	 *
	 * \return the length of the genotype
	 */
	unsigned int length() const;

	/**
	 * \brief Returns the i-th gene
	 *
	 * \param i the gnee to return
	 * \return the i-th gene
	 */
	unsigned char& operator[](unsigned int i);

	/**
	 * \brief Returns the i-th gene (const version)
	 *
	 * \param i the gnee to return
	 * \return the i-th gene
	 */
	unsigned char operator[](unsigned int i) const;

	/**
	 * \brief Resizes the genotype
	 *
	 * \param s the new size of the genotype
	 */
	void resize(unsigned int s);

	/**
	 * \brief Writes this genotype to a stream
	 *
	 * \param stream the stream to which the genotype should be written
	 */
	void write(QTextStream& stream) const;

	/**
	 * \brief Reads the genotype from the stream
	 *
	 * The genotype is resized to contain the stream. If the stream is
	 * invalid this function returns false and the current genotype is not
	 * modified
	 * \return false in case of errors, true otherwise
	 */
	bool read(QTextStream& stream);

	/**
	 * \brief Sets the fitness value associated to the genotype
	 *
	 * \param f the fitness value associated to the genotype
	 */
	void setFitness(double f);

	/**
	 * \brief Returns the fitness associated to the genotype
	 *
	 * \return the fitness associated to the genotype
	 */
	double getFitness() const;

	/**
	 * \brief Compares two genotypes using their fitness
	 *
	 * \param other the other genotype to compare
	 * \return true if the fitness of this is less than the fitness of
	 *         other, false otherwise
	 */
	bool operator<(const EvorobotIntGenotype& other) const;

private:
	QVector<unsigned char> m_genotype;
	double m_fitness;

};

} // end namespace farsa

#endif

