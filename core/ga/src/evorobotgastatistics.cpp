/********************************************************************************
 *  SALSA Experiments Library                                                   *
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

#include "evorobotgastatistics.h"
#include <QFile>
#include <QTextStream>

namespace salsa {

EvorobotGAStatistics::EvorobotGAStatistics()
	: m_seed(0)
	, m_statistics()
{
}

EvorobotGAStatistics::EvorobotGAStatistics(unsigned int seed)
	: m_seed(seed)
	, m_statistics()
{
}

EvorobotGAStatistics::EvorobotGAStatistics(const EvorobotGAStatistics& other)
	: m_seed(other.m_seed)
	, m_statistics(other.m_statistics)
{
}

EvorobotGAStatistics& EvorobotGAStatistics::operator=(const EvorobotGAStatistics& other)
{
	if (&other == this) {
		return *this;
	}

	m_seed = other.m_seed;
	m_statistics = other.m_statistics;

	return *this;
}

EvorobotGAStatistics::~EvorobotGAStatistics()
{
}

void EvorobotGAStatistics::setSeed(unsigned int s)
{
	m_seed = s;
}

unsigned int EvorobotGAStatistics::seed() const
{
	return m_seed;
}

unsigned int EvorobotGAStatistics::numGenerations() const
{
	return m_statistics.size();
}

void EvorobotGAStatistics::append(double maxFit, double avgFit, double minFit)
{
	GenerationStatistics s;
	s.maxFit = maxFit;
	s.avgFit = avgFit;
	s.minFit = minFit;

	m_statistics.append(s);
}

const EvorobotGAStatistics::GenerationStatistics& EvorobotGAStatistics::operator[](unsigned int g) const
{
	return m_statistics.at(g);
}

void EvorobotGAStatistics::clear()
{
	m_statistics.clear();
}

void EvorobotGAStatistics::write(QTextStream& stream) const
{
	for (int i = 0; i < m_statistics.size(); ++i) {
		const GenerationStatistics& s = m_statistics[i];

		stream << s.maxFit << " " << s.avgFit << " " << s.minFit << "\n";
	}
}

bool EvorobotGAStatistics::write(const QString& dir) const
{
	const QString filename = statFilename(dir);

	// Opening file for writing
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) {
		return false;
	}

	// Creating a text stream and calling the other write function
	QTextStream s(&f);
	write(s);

	return true;
}

bool EvorobotGAStatistics::read(QTextStream& stream)
{
	QVector<GenerationStatistics> newStats;

	while (!stream.atEnd()) {
		const QString row = stream.readLine();
		const QStringList lineElements = row.split(" ", QString::SkipEmptyParts);

		if (lineElements.size() != 3) {
			return false;
		}

		bool ok;
		GenerationStatistics s;
		s.maxFit = lineElements[0].toDouble(&ok);
		if (!ok) {
			return false;
		}
		s.avgFit = lineElements[1].toDouble(&ok);
		if (!ok) {
			return false;
		}
		s.minFit = lineElements[2].toDouble(&ok);
		if (!ok) {
			return false;
		}

		newStats.append(s);
	}

	m_statistics = newStats;

	return true;
}

bool EvorobotGAStatistics::read(const QString& dir)
{
	const QString filename = statFilename(dir);

	// Opening file for reading
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) {
		return false;
	}

	// Creating a text stream and calling the other read function
	QTextStream s(&f);
	return read(s);
}

QString EvorobotGAStatistics::statFilename(const QString& dir) const
{
	const QString separator = (dir.isEmpty() || dir.endsWith("/")) ? "" : "/";
	return dir + separator + "statS" + QString::number(m_seed) + ".fit";
}

} // end namespace salsa
