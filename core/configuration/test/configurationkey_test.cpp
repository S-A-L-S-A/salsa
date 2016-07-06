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
#include <QPair>
#include <QTextStream>
#include <algorithm>
#include <memory>
#include "configurationkey.h"

// NOTES AND TODOS
//
//

using namespace farsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ConfigurationKey_Test : public QObject
{
	Q_OBJECT

private slots:
	void keyEqualDifferent()
	{
		QVERIFY(ConfigurationKey("pippo") != ConfigurationKey("Pluto"));
		QVERIFY(ConfigurationKey("gruppo:1") != ConfigurationKey("gruppo:2"));
		QVERIFY(ConfigurationKey("gruppo:3") != ConfigurationKey("gruppo:13"));
		QVERIFY(ConfigurationKey("gruppoD:1") != ConfigurationKey("gruppo:2"));
		QVERIFY(ConfigurationKey("gruppo:1a") != ConfigurationKey("gruppo:2"));
		QVERIFY(ConfigurationKey("gruppo:pippo") != ConfigurationKey("gruppo:pluto"));
		QVERIFY(ConfigurationKey("gruppo:1") != ConfigurationKey("gruppone"));
		QVERIFY(ConfigurationKey("gruppone") != ConfigurationKey("Gruppone"));
		QVERIFY(ConfigurationKey("gruppo:1") == ConfigurationKey("gruppo:1"));
		QVERIFY(ConfigurationKey("gruppo:1") == ConfigurationKey("gruppo:01"));
		QVERIFY(ConfigurationKey("gruppone") == ConfigurationKey("gruppone"));
	}

	void keyLessThan()
	{
		QCOMPARE(ConfigurationKey("pippo") < ConfigurationKey("Pluto"), false);
		QCOMPARE(ConfigurationKey("gruppo:1") < ConfigurationKey("gruppo:2"), true);
		QCOMPARE(ConfigurationKey("gruppo:3") < ConfigurationKey("gruppo:13"), true);
		QCOMPARE(ConfigurationKey("gruppoD:1") < ConfigurationKey("gruppo:2"), false);
		QCOMPARE(ConfigurationKey("gruppo:1a") < ConfigurationKey("gruppo:2"), false);
		QCOMPARE(ConfigurationKey("gruppo:pippo") < ConfigurationKey("gruppo:pluto"), true);
		QCOMPARE(ConfigurationKey("gruppo:1") < ConfigurationKey("gruppone"), true);
		QCOMPARE(ConfigurationKey("gruppo:1") < ConfigurationKey("gruppo:1"), false);
		QCOMPARE(ConfigurationKey("gruppo:1") < ConfigurationKey("gruppo:01"), false);
		QCOMPARE(ConfigurationKey("gruppone") < ConfigurationKey("gruppone"), false);

		// These tests are the same of the ones above with the first and second key exchanged
		QCOMPARE(ConfigurationKey("Pluto") < ConfigurationKey("pippo"), true);
		QCOMPARE(ConfigurationKey("gruppo:2") < ConfigurationKey("gruppo:1"), false);
		QCOMPARE(ConfigurationKey("gruppo:13") < ConfigurationKey("gruppo:3"), false);
		QCOMPARE(ConfigurationKey("gruppo:2") < ConfigurationKey("gruppoD:1"), true);
		QCOMPARE(ConfigurationKey("gruppo:2") < ConfigurationKey("gruppo:1a"), true);
		QCOMPARE(ConfigurationKey("gruppo:pluto") < ConfigurationKey("gruppo:pippo"), false);
		QCOMPARE(ConfigurationKey("gruppone") < ConfigurationKey("gruppo:1"), false);
		QCOMPARE(ConfigurationKey("gruppo:1") < ConfigurationKey("gruppo:1"), false);
		QCOMPARE(ConfigurationKey("gruppo:01") < ConfigurationKey("gruppo:1"), false);
		QCOMPARE(ConfigurationKey("gruppone") < ConfigurationKey("gruppone"), false);
	}
};

QTEST_MAIN(ConfigurationKey_Test)
#include "configurationkey_test.moc"
