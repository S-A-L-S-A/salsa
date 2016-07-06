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
#include <QString>
#include <QStringList>
#include "commandlineparser.h"
#include "utilitiesexceptions.h"

// NOTES AND TODOS
//

using namespace farsa;
using namespace farsa::pluginHelper;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class CommandLineParser_Test : public QObject
{
	Q_OBJECT

private slots:
	void checkParsing()
	{
		const QStringList args = QStringList() << "exeName" << "pName" << "dDir" << "hdDir" << "gSrc" << "dFile" << "hdr1" << "hdr2" << "hdr3";

		CommandLineParser parser(args);

		QVERIFY(!parser.printHelp());
		QCOMPARE(parser.pluginName(), QString("pName"));
		QCOMPARE(parser.destDir(), QString("dDir"));
		QCOMPARE(parser.headersDestDir(), QString("hdDir"));
		QCOMPARE(parser.genSource(), QString("gSrc"));
		QCOMPARE(parser.dependenciesFile(), QString("dFile"));
		const QStringList expectedHeaders = QStringList() << "hdr1" << "hdr2" << "hdr3";
		QCOMPARE(parser.headers(), expectedHeaders);
	}

	void checkHelp()
	{
		const QStringList args = QStringList() << "exeName" << "--help";

		CommandLineParser parser(args);

		QVERIFY(parser.printHelp());
	}

	void checkExceptionEmptyCommandline()
	{
		const QStringList args;

		QVERIFY_EXCEPTION_THROWN(CommandLineParser parser(args), RuntimeUserException);
	}

	void checkExceptionTooFewParameters()
	{
		const QStringList args = QStringList() << "exeName" << "pName" << "dDir" << "hdDir" << "gSrc";

		QVERIFY_EXCEPTION_THROWN(CommandLineParser parser(args), RuntimeUserException);
	}
};

QTEST_MAIN(CommandLineParser_Test)
#include "commandlineparser_test.moc"
