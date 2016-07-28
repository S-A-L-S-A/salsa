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

#include "configurationmanager.h"
#include "component.h"
#include "typesdb.h"
#include "componentdescriptors.h"
#include "testutils.h"
#include "testhierarchy.h"
#include <QtTest/QtTest>

// NOTES AND TODOS
//

using namespace salsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ComponentDescription_Test : public QObject
{
	Q_OBJECT

private slots:
	void describeType()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleType");
		d.help("A simple type", "The long help for a simple type");

		QVERIFY(m.groupExists("simpleType"));
		QCOMPARE(m.getValue("simpleType/shortHelp"), QString("A simple type"));
		QCOMPARE(m.getValue("simpleType/longHelp"), QString("The long help for a simple type"));
	}

	void describeString()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithString");
		d.help("Bla");

		d.describeString("str").def("dummy").props(ParamIsMandatory | ParamAllowMultiple).help("short help", "long help");
		d.describeString("strBare").help("sh");

		QVERIFY(m.groupExists("simpleTypeWithString"));
		QCOMPARE(m.getValue("simpleTypeWithString/shortHelp"), QString("Bla"));
		QCOMPARE(m.getValue("simpleTypeWithString/longHelp"), QString("Bla"));

		QVERIFY(m.groupExists("simpleTypeWithString/Parameters/str"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/type"), QString("string"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/default"), QString("dummy"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/isMandatory"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/allowMultiple"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/shortHelp"), QString("short help"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/str/longHelp"), QString("long help"));

		QVERIFY(m.groupExists("simpleTypeWithString/Parameters/strBare"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/strBare/type"), QString("string"));
		QVERIFY(!m.parameterExists("simpleTypeWithString/Parameters/strBare/default"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/strBare/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/strBare/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/strBare/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/strBare/shortHelp"), QString("sh"));
		QCOMPARE(m.getValue("simpleTypeWithString/Parameters/strBare/longHelp"), QString("sh"));
	}

	void describeInt()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithInt");
		d.help("Bla", "Blu");

		d.describeInt("i").def(10).props(ParamIsMandatory).help("short help", "long help");
		d.describeInt("i2").props(ParamIsList).limits(MinInteger, 15);
		d.describeInt("i3").def(100).limits(0, MaxInteger);

		QVERIFY(m.groupExists("simpleTypeWithInt"));
		QCOMPARE(m.getValue("simpleTypeWithInt/shortHelp"), QString("Bla"));
		QCOMPARE(m.getValue("simpleTypeWithInt/longHelp"), QString("Blu"));

		QVERIFY(m.groupExists("simpleTypeWithInt/Parameters/i"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/type"), QString("int"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/default"), QString("10"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/isMandatory"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/shortHelp"), QString("short help"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i/longHelp"), QString("long help"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i/lowerBound"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i/upperBound"));

		QVERIFY(m.groupExists("simpleTypeWithInt/Parameters/i2"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i2/type"), QString("int"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i2/default"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i2/isList"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i2/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i2/allowMultiple"), QString("false"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i2/shortHelp"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i2/longHelp"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i2/lowerBound"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i2/upperBound"), QString("15"));

		QVERIFY(m.groupExists("simpleTypeWithInt/Parameters/i3"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i3/type"), QString("int"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i3/default"), QString("100"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i3/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i3/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i3/allowMultiple"), QString("false"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i3/shortHelp"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i3/longHelp"));
		QCOMPARE(m.getValue("simpleTypeWithInt/Parameters/i3/lowerBound"), QString("0"));
		QVERIFY(!m.parameterExists("simpleTypeWithInt/Parameters/i3/upperBound"));
	}

	void describeReal()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithReal");
		d.help("Boom");

		d.describeReal("r").props(ParamIsMandatory | ParamIsList).limits(0.0, Infinity).help("help", "help me a lot");
		d.describeReal("real").def(-313.17).props(ParamIsList).limits(-2.0, 2.0).help("h");

		QVERIFY(m.groupExists("simpleTypeWithReal"));
		QCOMPARE(m.getValue("simpleTypeWithReal/shortHelp"), QString("Boom"));
		QCOMPARE(m.getValue("simpleTypeWithReal/longHelp"), QString("Boom"));

		QVERIFY(m.groupExists("simpleTypeWithReal/Parameters/r"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/type"), QString("real"));
		QVERIFY(!m.parameterExists("simpleTypeWithReal/Parameters/r/default"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/isList"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/isMandatory"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/shortHelp"), QString("help"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/longHelp"), QString("help me a lot"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/r/lowerBound"), QString::number(0.0));
		QVERIFY(!m.parameterExists("simpleTypeWithReal/Parameters/r/upperBound"));

		QVERIFY(m.groupExists("simpleTypeWithReal/Parameters/real"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/type"), QString("real"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/default"), QString::number(-313.17));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/isList"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/shortHelp"), QString("h"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/longHelp"), QString("h"));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/lowerBound"), QString::number(-2.0));
		QCOMPARE(m.getValue("simpleTypeWithReal/Parameters/real/upperBound"), QString::number(2.0));
	}

	void describeBool()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithBool");
		d.help("True or False");

		d.describeBool("bool").props(ParamIsList).help("oh", "oh oh oh");
		d.describeBool("10").def(true).props(ParamAllowMultiple);

		QVERIFY(m.groupExists("simpleTypeWithBool"));
		QCOMPARE(m.getValue("simpleTypeWithBool/shortHelp"), QString("True or False"));
		QCOMPARE(m.getValue("simpleTypeWithBool/longHelp"), QString("True or False"));

		QVERIFY(m.groupExists("simpleTypeWithBool/Parameters/bool"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/bool/type"), QString("bool"));
		QVERIFY(!m.parameterExists("simpleTypeWithBool/Parameters/bool/default"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/bool/isList"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/bool/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/bool/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/bool/shortHelp"), QString("oh"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/bool/longHelp"), QString("oh oh oh"));

		QVERIFY(m.groupExists("simpleTypeWithBool/Parameters/10"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/10/type"), QString("bool"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/10/default"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/10/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/10/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithBool/Parameters/10/allowMultiple"), QString("true"));
		QVERIFY(!m.parameterExists("simpleTypeWithBool/Parameters/10/shortHelp"));
		QVERIFY(!m.parameterExists("simpleTypeWithBool/Parameters/10/longHelp"));
	}

	void describeEnum()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithEnum");
		d.help("One of many", "Many many");

		d.describeEnum("enum").def("value3").props(ParamIsMandatory).values(QStringList() << "value1" << "value2" << "value3");
		d.describeEnum("param").values(QStringList() << "timmy" << "tommy" << "jimmy" << "red" << "green" << "blue").help("little");

		QVERIFY(m.groupExists("simpleTypeWithEnum"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/shortHelp"), QString("One of many"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/longHelp"), QString("Many many"));

		QVERIFY(m.groupExists("simpleTypeWithEnum/Parameters/enum"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/type"), QString("enum"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/default"), QString("value3"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/isMandatory"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/enum:0"), QString("value1"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/enum:1"), QString("value2"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/enum/enum:2"), QString("value3"));
		QVERIFY(!m.parameterExists("simpleTypeWithEnum/Parameters/enum/shortHelp"));
		QVERIFY(!m.parameterExists("simpleTypeWithEnum/Parameters/enum/longHelp"));

		QVERIFY(m.groupExists("simpleTypeWithEnum/Parameters/param"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/type"), QString("enum"));
		QVERIFY(!m.parameterExists("simpleTypeWithEnum/Parameters/param/default"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/enum:0"), QString("timmy"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/enum:1"), QString("tommy"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/enum:2"), QString("jimmy"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/enum:3"), QString("red"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/enum:4"), QString("green"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/enum:5"), QString("blue"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/shortHelp"), QString("little"));
		QCOMPARE(m.getValue("simpleTypeWithEnum/Parameters/param/longHelp"), QString("little"));
	}

	void describeComponent()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithComponent");
		d.help("Renamed", "the function");

		d.describeComponent("subc").props(ParamIsMandatory).componentType("Strange").help("jingle");
		d.describeComponent("owned").componentType("Another").help("all the way", "horse open sleight");

		QVERIFY(m.groupExists("simpleTypeWithComponent"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/shortHelp"), QString("Renamed"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/longHelp"), QString("the function"));

		QVERIFY(m.groupExists("simpleTypeWithComponent/Parameters/subc"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/type"), QString("component"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/isMandatory"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/componentType"), QString("Strange"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/shortHelp"), QString("jingle"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/subc/longHelp"), QString("jingle"));

		QVERIFY(m.groupExists("simpleTypeWithComponent/Parameters/owned"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/type"), QString("component"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/componentType"), QString("Another"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/shortHelp"), QString("all the way"));
		QCOMPARE(m.getValue("simpleTypeWithComponent/Parameters/owned/longHelp"), QString("horse open sleight"));
	}

	void describeSubgroup()
	{
		ConfigurationManager m;
		RegisteredComponentDescriptor d(&m, "simpleTypeWithSubgroup");
		d.help("Long test");

		SubgroupDescriptor& s1 = d.describeSubgroup("subg").props(ParamIsMandatory | ParamIsList).help("sub help");
		s1.describeString("subp").def("dummy").help("missing");
		d.describeSubgroup("subcomp").componentType("AComponent").help("subcomponent help string", "verbose one");

		QVERIFY(m.groupExists("simpleTypeWithSubgroup"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/shortHelp"), QString("Long test"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/longHelp"), QString("Long test"));

		QVERIFY(m.groupExists("simpleTypeWithSubgroup/Subgroups/subg"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/isList"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/isMandatory"), QString("true"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/allowMultiple"), QString("false"));
		QVERIFY(!m.parameterExists("simpleTypeWithSubgroup/Subgroups/subg/componentType"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/shortHelp"), QString("sub help"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/longHelp"), QString("sub help"));

		QVERIFY(m.groupExists("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/type"), QString("string"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/default"), QString("dummy"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/shortHelp"), QString("missing"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subg/Parameters/subp/longHelp"), QString("missing"));

		QVERIFY(m.groupExists("simpleTypeWithSubgroup/Subgroups/subcomp"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subcomp/isList"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subcomp/isMandatory"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subcomp/allowMultiple"), QString("false"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subcomp/componentType"), QString("AComponent"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subcomp/shortHelp"), QString("subcomponent help string"));
		QCOMPARE(m.getValue("simpleTypeWithSubgroup/Subgroups/subcomp/longHelp"), QString("verbose one"));
	}
};

QTEST_MAIN(ComponentDescription_Test)
#include "componentdescription_test.moc"
