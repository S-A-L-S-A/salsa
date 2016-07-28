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
#include <QTemporaryFile>
#include "private/inifilesupport.h"
#include "configurationmanager.h"

// NOTES AND TODOS
//
//

using namespace salsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class IniFileLoaderSaver_Test : public QObject
{
	Q_OBJECT

private slots:
	void escapeSimpleString()
	{
		const QString original = "simple string";

		QCOMPARE(IniFileStringMangler(original).escape(), original);
	}

	void escapeStringWithLeadingSpaces()
	{
		const QString original = "  string with leading spaces";

		QCOMPARE(IniFileStringMangler(original).escape(), QString("\\  string with leading spaces"));
	}

	void escapeStringWithSlashes()
	{
		const QString original = "\\  string wi\\th some slash\\es";

		QCOMPARE(IniFileStringMangler(original).escape(), QString("\\\\  string wi\\\\th some slash\\\\es"));
	}

	void escapeStringWithNewlines()
	{
		const QString original = " string with\nnewlines\rcarr\\iage returns\n\r and both";

		QCOMPARE(IniFileStringMangler(original).escape(), QString("\\ string with\\nnewlines\\rcarr\\\\iage returns\\n\\r and both"));
	}

	void escapeStringWithEqual()
	{
		const QString original = " s=tring with\nnewlines\rcarr\\iage ret= urns\n\r and both";

		QCOMPARE(IniFileStringMangler(original).escape(), QString("\\ s\\=tring with\\nnewlines\\rcarr\\\\iage ret\\= urns\\n\\r and both"));
	}

	void escapeStringWithSharpAndSemicolon()
	{
		const QString original = " s=tring with\nnew;lines\rcarr\\iage ret= urns\n\r and bo#th";

		QCOMPARE(IniFileStringMangler(original).escape(), QString("\\ s\\=tring with\\nnew\\;lines\\rcarr\\\\iage ret\\= urns\\n\\r and bo\\#th"));
	}

	void escapeStringWithSquareBrackets()
	{
		const QString original = " s=t[ring with\nnew;line]s\rcarr\\iage ret= urns\n\r and bo#th";

		QCOMPARE(IniFileStringMangler(original).escape(), QString("\\ s\\=t\\[ring with\\nnew\\;line\\]s\\rcarr\\\\iage ret\\= urns\\n\\r and bo\\#th"));
	}

	void unescapeSimpleString()
	{
		checkUnescapeOfEscape("simple string");
	}

	void unescapeStringWithLeadingSpaces()
	{
		checkUnescapeOfEscape("  string with leading spaces");
	}

	void unescapeStringWithSlashes()
	{
		checkUnescapeOfEscape("\\  string wi\\th some slash\\es");
	}

	void unescapeStringWithSlashesAndLeadingSpace()
	{
		checkUnescapeOfEscape("  string wi\\th some slash\\es");
	}

	void unescapeStringWithNewlines()
	{
		checkUnescapeOfEscape(" string with\nnewlines\rcarr\\iage returns\n\r and both");
	}

	void unescapeStringWithEqual()
	{
		checkUnescapeOfEscape(" s=tring with\nnewlines\rcarr\\iage ret= urns\n\r and both");
	}

	void unescapeStringWithSharpAndSemicolon()
	{
		checkUnescapeOfEscape(" s=tring with\nnew;lines\rcarr\\iage ret= urns\n\r and bo#th");
	}

	void unescapeStringWithSquareBrackets()
	{
		checkUnescapeOfEscape(" s=t[ring with\nnew;line]s\rcarr\\iage ret= urns\n\r and bo#th");
	}

	void checkIniFileSaveAndLoad()
	{
		ConfigurationManager original;

		original.createGroup("one/two");
		original.createGroup("one/another");
		original.createGroup("one/str\nang\\e");
		original.createGroup("one/ less strang\\e\n ?  ");
		original.createGroup("#notAComment");

		original.createParameter("one", "param", "value1");
		// There can be a parameter with the same name of a group
		original.createParameter("one", "two", " otherValue with leading and trailing spaces  ");
		original.createParameter("one/two", "  parameter  ", "Timmy\nAnd a newline");
		original.createParameter("one/two", "du\nmmy", "  Jimm\\y is\n compli\\cated");
		original.createParameter("one/another", " param\n", "valueSetDirectly");
		original.createParameter("one/str\nang\\e", "p1\n\r", "value ");
		original.createParameter("one/ less strang\\e\n ?  ", "\n\n name", "joe");
		original.createParameter("one/ less strang\\e\n ?  ", "\n\n surname", "");
		original.createParameter("one/ less strang\\e\n ?  ", "address", QString());
		original.createParameter("#notAComment", "#not a comment", "#neither this one");
		original.createParameter("#notAComment", ";not a comment2", ";neither this one");

		QTemporaryFile tmpFile("XXXXXX.ini");
		QVERIFY(tmpFile.open());

		QVERIFY(original.saveParameters(tmpFile.fileName()));

		ConfigurationManager loaded;
		QVERIFY(loaded.loadParameters(tmpFile.fileName()));

		checkSameValue(original, loaded, "one/param");
		checkSameValue(original, loaded, "one/two");
		checkSameValue(original, loaded, "one/two/  parameter  ");
		checkSameValue(original, loaded, "one/two/du\nmmy");
		checkSameValue(original, loaded, "one/another/ param\n");
		checkSameValue(original, loaded, "one/str\nang\\e/p1\n\r");
		checkSameValue(original, loaded, "one/ less strang\\e\n ?  /\n\n name");
		checkSameValue(original, loaded, "one/ less strang\\e\n ?  /\n\n surname");
		checkSameValue(original, loaded, "one/ less strang\\e\n ?  /address");
		checkSameValue(original, loaded, "#notAComment/#not a comment");
		checkSameValue(original, loaded, "#notAComment/;not a comment2");
	}

private:
	void checkUnescapeOfEscape(QString str)
	{
		const QString escaped = IniFileStringMangler(str).escape();

		QCOMPARE(IniFileStringMangler(escaped).unescape(), str);
	}

	void checkSameValue(const ConfigurationManager& first, const ConfigurationManager& second, const QString& paramPath)
	{
		QCOMPARE(first.getValue(paramPath), second.getValue(paramPath));
	}
};

QTEST_MAIN(IniFileLoaderSaver_Test)
#include "inifilesupport_test.moc"
