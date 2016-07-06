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

#ifndef EVOROBOTGASTATISTICS_H
#define EVOROBOTGASTATISTICS_H

#include "gaconfig.h"
#include <QVector>
#include <QTextStream>

namespace farsa {

/**
 * \brief A class storing statistics of an evolution
 *
 * This stores for each generation the worst, mean and best fitness. This is
 * compatible with evorobot fit files
 */
class FARSA_GA_API EvorobotGAStatistics
{
public:
	/**
	 * \brief The structure with statistics for one generation
	 */
	struct GenerationStatistics
	{
		/**
		 * \brief The maximum fitness of the generation
		 */
		double maxFit;

		/**
		 * \brief The average fitness of the generation
		 */
		double avgFit;

		/**
		 * \brief The minimum fitness of the generation
		 */
		double minFit;
	};

public:
	/**
	 * \brief Constructor
	 *
	 * The seed is set to 0
	 */
	EvorobotGAStatistics();

	/**
	 * \brief Constructor
	 *
	 * \param seed the seed to which these statistics are referred
	 */
	EvorobotGAStatistics(unsigned int seed);

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	EvorobotGAStatistics(const EvorobotGAStatistics& other);

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 * \return a reference to this
	 */
	EvorobotGAStatistics& operator=(const EvorobotGAStatistics& other);

	/**
	 * \brief Destructor
	 */
	~EvorobotGAStatistics();

	/**
	 * \brief Sets the seed associated to this object
	 *
	 * \param s the seed associated to this object
	 */
	void setSeed(unsigned int s);

	/**
	 * \brief Returns the seed associated to this object
	 *
	 * \return the seed associated to this object
	 */
	unsigned int seed() const;

	/**
	 * \brief Returns the number of generations for which statistics are
	 *        stored here
	 *
	 * \return the number of generations for which statistics are stored
	 *         here
	 */
	unsigned int numGenerations() const;

	/**
	 * \brief Adds statistics for a new generation
	 *
	 * \param maxFit the maximum fitness of the generation
	 * \param avgFit the average fitness of the generation
	 * \param minFit the minimum fitness of the generation
	 */
	void append(double maxFit, double avgFit, double minFit);

	/**
	 * \brief Returns the statistics for generation g
	 *
	 * \param g the generation for which statistics should be returned
	 * \return statistics for generation g
	 */
	const GenerationStatistics& operator[](unsigned int g) const;

	/**
	 * \brief Removes all statistics
	 */
	void clear();

	/**
	 * \brief Writes statistics to a stream
	 *
	 * \param stream the stream to which the statistics should be written
	 */
	void write(QTextStream& stream) const;

	/**
	 * \brief Writes statistics to a file
	 *
	 * The file is named statS\<seed\>.fit. Any file with the same name in
	 * the path will be overwritten
	 * \param dir the directory where to save the file
	 * \return false in case of error
	 */
	bool write(const QString& dir) const;

	/**
	 * \brief Reads statistics from the stream
	 *
	 * The vector of statistics is resized to contain the stream. If the
	 * stream is invalid this function returns false and the current
	 * statists are not modified
	 * \return false in case of errors, true otherwise
	 */
	bool read(QTextStream& stream);

	/**
	 * \brief Reads statistics from a file
	 *
	 * The file must be named statS\<seed\>.fit. If the file cannot be
	 * loaded the data currently available in this object is preserved
	 * \param dir the directory where to save the file
	 * \return false in case of error
	 */
	bool read(const QString& dir);

	/**
	 * \brief Returns the full path of the stat file
	 *
	 * If dir is a relative path, the returned path is still relative
	 * \param dir the directory where the stat file is present
	 * \return the full path of the stat file
	 */
	QString statFilename(const QString& dir) const;

private:
	unsigned int m_seed;
	QVector<GenerationStatistics> m_statistics;
};

} // end namespace farsa

#endif

