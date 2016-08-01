/***************************************************************************
 *  SALSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Gianluca Massera <emmegian@yahoo.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#include <QtTest/QtTest>
#include "gaconfig.h"
#include "evorobotgastatistics.h"

// NOTES AND TODOS
//

using namespace salsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class EvorobotGAStatistics_Test : public QObject
{
	Q_OBJECT

private slots:
	void defaultConstruction()
	{
		EvorobotGAStatistics s;

		QCOMPARE (s.seed(), 0u);
	}

	void constructionWithSeed()
	{
		EvorobotGAStatistics s(17);

		QCOMPARE (s.seed(), 17u);
	}

	void setSeed()
	{
		EvorobotGAStatistics s;

		s.setSeed(17);

		QCOMPARE (s.seed(), 17u);
	}

	void noData()
	{
		EvorobotGAStatistics s;

		QCOMPARE(s.numGenerations(), 0u);
	}

	void appendData()
	{
		EvorobotGAStatistics s;

		s.append(3.4, 2.7, 0.0);
		s.append(7.4, 4.7, -0.1);
		s.append(18.4, 11.7, 4.0);

		QCOMPARE(s.numGenerations(), 3u);
	}

	void getData()
	{
		EvorobotGAStatistics s;

		s.append(3.4, 2.7, 0.0);
		s.append(7.4, 4.7, -0.1);

		QCOMPARE(s.numGenerations(), 2u);
		QCOMPARE(s[0].maxFit, 3.4);
		QCOMPARE(s[0].avgFit, 2.7);
		QCOMPARE(s[0].minFit, 0.0);
		QCOMPARE(s[1].maxFit, 7.4);
		QCOMPARE(s[1].avgFit, 4.7);
		QCOMPARE(s[1].minFit, -0.1);
	}

	void resetData()
	{
		EvorobotGAStatistics s;

		s.append(3.4, 2.7, 0.0);
		s.append(7.4, 4.7, -0.1);

		s.clear();

		QCOMPARE(s.numGenerations(), 0u);
	}

	void writeStatistics()
	{
		EvorobotGAStatistics s;

		s.append(3.4, 2.7, 0.0);
		s.append(7.4, 4.7, -0.1);
		s.append(18.4, 11.7, 4.0);

		QString output;
		QTextStream stream(&output, QIODevice::WriteOnly);

		s.write(stream);

		QCOMPARE(output, QString("3.4 2.7 0\n7.4 4.7 -0.1\n18.4 11.7 4\n"));
	}

	void readInvalidStatistics()
	{
		EvorobotGAStatistics s;

		s.append(3.4, 2.7, 0.0);
		s.append(7.4, 4.7, -0.1);

		QString intput = "invalid";
		QTextStream stream(&intput, QIODevice::ReadOnly);

		QVERIFY(!s.read(stream));
		QCOMPARE(s.numGenerations(), 2u);
		QCOMPARE(s[0].maxFit, 3.4);
		QCOMPARE(s[0].avgFit, 2.7);
		QCOMPARE(s[0].minFit, 0.0);
		QCOMPARE(s[1].maxFit, 7.4);
		QCOMPARE(s[1].avgFit, 4.7);
		QCOMPARE(s[1].minFit, -0.1);
	}

	void readStatistics()
	{
		EvorobotGAStatistics s;

		QString intput = "3.4 2.7 0\n7.4 4.7 -0.1\n18.4 11.7 4\n";
		QTextStream stream(&intput, QIODevice::ReadOnly);

		QVERIFY(s.read(stream));
		QCOMPARE(s.numGenerations(), 3u);
		QCOMPARE(s[0].maxFit, 3.4);
		QCOMPARE(s[0].avgFit, 2.7);
		QCOMPARE(s[0].minFit, 0.0);
		QCOMPARE(s[1].maxFit, 7.4);
		QCOMPARE(s[1].avgFit, 4.7);
		QCOMPARE(s[1].minFit, -0.1);
		QCOMPARE(s[2].maxFit, 18.4);
		QCOMPARE(s[2].avgFit, 11.7);
		QCOMPARE(s[2].minFit, 4.0);
	}

	void statFilename()
	{
		EvorobotGAStatistics s(17);

		QCOMPARE(s.statFilename(""), QString("statS17.fit"));
		QCOMPARE(s.statFilename("/a/path"), QString("/a/path/statS17.fit"));
		QCOMPARE(s.statFilename("another/path/"), QString("another/path/statS17.fit"));
	}
};

QTEST_MAIN(EvorobotGAStatistics_Test)
#include "evorobotgastatistics_test.moc"
