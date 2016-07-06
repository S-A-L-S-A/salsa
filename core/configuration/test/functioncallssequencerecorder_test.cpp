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
#include <QSet>
#include "testutils.h"

// NOTES AND TODOS
//
//

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class FunctionCallsSequenceRecorder_Test : public QObject
{
	Q_OBJECT

	static void a()
	{
		FunctionCall call("a");
	}

	static void f()
	{
		FunctionCall call("f");

		a();
	}

	static void f2()
	{
		FunctionCall call("f2");
	}

	static void g()
	{
		FunctionCall call("g");

		f();

		f2();
	}

	static void h()
	{
		FunctionCall call("h");

		g();

		f();
	}

private slots:
	void simpleFunctionCalls()
	{
		FunctionCallsSequenceRecorder expectedSequence;
		expectedSequence.callBegin("g");
			expectedSequence.callBegin("f");
				expectedSequence.callBegin("a");
				expectedSequence.callEnd();
			expectedSequence.callEnd();
			expectedSequence.callBegin("f2");
			expectedSequence.callEnd();
		expectedSequence.callEnd();

		QCOMPARE(expectedSequence.calls().name, QString());
		QCOMPARE(expectedSequence.calls().subcalls.size(), 1);
		QCOMPARE(expectedSequence.calls().subcalls[0].name, QString("g"));
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls.size(), 2);
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls[0].name, QString("f"));
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls[0].subcalls.size(), 1);
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls[0].subcalls[0].name, QString("a"));
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls[0].subcalls[0].subcalls.size(), 0);
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls[1].name, QString("f2"));
		QCOMPARE(expectedSequence.calls().subcalls[0].subcalls[1].subcalls.size(), 0);

		FunctionCallsSequenceRecorder actualCalls;
		g();

		QCOMPARE(expectedSequence, actualCalls);
	}

	void functionsMap()
	{
		QMap<QString, int> expectedMap;
		expectedMap["a"] = 2;
		expectedMap["f"] = 2;
		expectedMap["f2"] = 1;
		expectedMap["g"] = 1;
		expectedMap["h"] = 1;

		FunctionCallsSequenceRecorder actualCalls;
		h();

		QCOMPARE(expectedMap, actualCalls.functionsMap());
	}
};

QTEST_MAIN(FunctionCallsSequenceRecorder_Test)
#include "functioncallssequencerecorder_test.moc"
