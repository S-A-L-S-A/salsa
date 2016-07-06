/***************************************************************************
 *  FARSA Configuration Library                                            *
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
#include "evorobotintgenotype.h"

// NOTES AND TODOS
//

using namespace farsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class EvorobotIntGenotype_Test : public QObject
{
	Q_OBJECT

private slots:
	void emptyGenotype()
	{
		EvorobotIntGenotype g(0);

		QCOMPARE(g.length(), 0u);
	}

	void nonEmptyGenotype()
	{
		EvorobotIntGenotype g(5);

		QCOMPARE(g.length(), 5u);
	}

	void writeAndReadGenes()
	{
		EvorobotIntGenotype g(3);

		g[0] = 15;
		g[1] = 3;
		g[2] = 157;

		QCOMPARE(g[0], (unsigned char) 15);
		QCOMPARE(g[1], (unsigned char) 3);
		QCOMPARE(g[2], (unsigned char) 157);
	}

	void resizeGenotype()
	{
		EvorobotIntGenotype g;

		g.resize(7);

		QCOMPARE(g.length(), 7u);
	}

	void writeGenotype()
	{
		EvorobotIntGenotype g(5);

		g[0] = 65;
		g[1] = 0;
		g[2] = 178;
		g[3] = 234;
		g[4] = 11;

		QString output;
		QTextStream stream(&output, QIODevice::WriteOnly);

		g.write(stream);

		QCOMPARE(output, QString("DYNAMICAL NN\n65\n0\n178\n234\n11\nEND\n"));
	}

	void readInvalidGenotype()
	{
		EvorobotIntGenotype g(3);

		g[0] = 15;
		g[1] = 3;
		g[2] = 157;

		QString intput = "invalid";
		QTextStream stream(&intput, QIODevice::ReadOnly);

		QVERIFY(!g.read(stream));
		QCOMPARE(g.length(), 3u);
		QCOMPARE(g[0], (unsigned char) 15);
		QCOMPARE(g[1], (unsigned char) 3);
		QCOMPARE(g[2], (unsigned char) 157);
	}

	void readGenotype()
	{
		EvorobotIntGenotype g;

		QString intput = "DYNAMICAL NN\n65\n0\n178\n234\n11\nEND\nmarker";
		QTextStream stream(&intput, QIODevice::ReadOnly);

		QVERIFY(g.read(stream));
		QCOMPARE(g.length(), 5u);
		QCOMPARE(g[0], (unsigned char) 65);
		QCOMPARE(g[1], (unsigned char) 0);
		QCOMPARE(g[2], (unsigned char) 178);
		QCOMPARE(g[3], (unsigned char) 234);
		QCOMPARE(g[4], (unsigned char) 11);

		// Checking the stream is in the correct status
		QString s;
		stream >> s;
		QCOMPARE(s, QString("marker"));
	}

	void getFitness()
	{
		EvorobotIntGenotype g;

		QCOMPARE(g.getFitness(), 0.0);
	}

	void setFitness()
	{
		EvorobotIntGenotype g;

		g.setFitness(10.5);

		QCOMPARE(g.getFitness(), 10.5);
	}

	void compareGenotypes()
	{
		EvorobotIntGenotype g0;
		EvorobotIntGenotype g1;
		EvorobotIntGenotype g2;

		g0.setFitness(10.5);
		g1.setFitness(-7.7);
		g2.setFitness(143.2);

		QVERIFY(!(g0 < g1));
		QVERIFY(g0 < g2);
		QVERIFY(g1 < g0);
		QVERIFY(g1 < g2);
		QVERIFY(!(g2 < g0));
		QVERIFY(!(g2 < g1));
	}
};

QTEST_MAIN(EvorobotIntGenotype_Test)
#include "evorobotintgenotype_test.moc"
