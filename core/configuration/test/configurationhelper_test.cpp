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
#include <QPair>
#include <QTextStream>
#include <algorithm>
#include "configurationhelper.h"
#include "configurationmanager.h"
#include "componentdescriptors.h"

// NOTES AND TODOS
//
// ConfigurationHelper functions to get parameters as bool, int or others,
// 	should also check flags: if a parameter is mandatory it must exists
// 	(exception thrown otherwise), if it is a list it must be get as a list.
// 	The only flag that we can ignore here is ParamAllowMultiple (or at least
// 	it is not clear how to handle it... perhaps add a function to return all
// 	parameters/groups in case the ParamAllowMultiple flag is set?)
// Re-add functions to get lists directly from parameters (for the moment we
// 	only have functions to encode/decode lists to/from string

using namespace salsa;

// The class with descriptions
class TestComponentWithDescriptions : public Component
{
public:
	TestComponentWithDescriptions(ConfigurationManager& params)
		: Component(params)
	{
	}

	static void describe(RegisteredComponentDescriptor& d)
	{
		d.describeString("strParam").def("ummo");
		d.describeString("strName").def("Jimmy");
		d.describeInt("strI").def(10);

		d.describeInt("intParam").def(13).limits(-20, 20);
		d.describeInt("intName").def(MinInteger);
		d.describeInt("intAnInt").def(100).limits(0, MaxInteger);
		d.describeInt("intWrongRange").def(0).limits(0, MaxInteger);
		d.describeString("intS").def("dummy");

		d.describeReal("realParam").def(13.5).limits(-20.0, 20.0);
		d.describeReal("realName").def(-Infinity);
		d.describeReal("realWrongRange").def(0.0).limits(0.0, Infinity);
		d.describeInt("realI").def(10);

		d.describeBool("boolParam").def(true);
		d.describeBool("boolFlag").def(false);
		d.describeBool("boolAnotherFlag").def(false);
		d.describeInt("boolI").def(10);

		d.describeEnum("enumParam").def("ummo").values(QStringList() << "ummo" << "dummo" << "bummo");
		d.describeEnum("enumName").def("Jimmy").values(QStringList() << "Jimmy" << "Tommy" << "Timmy");
		d.describeEnum("enumStrangeDefault").def("def").values(QStringList() << "one" << "two" << "three");
		d.describeEnum("enumInvalid").def("A").values(QStringList() << "A" << "B" << "C");
		d.describeInt("enumI").def(10);

		SubgroupDescriptor& s = d.describeSubgroup("SUBG");
		s.describeBool("subBool").def(true);
	}
};

ConfigurationManager& configurationManager()
{
	static ConfigurationManager m;

	if (!m.groupExists("Typed")) {
		m.createGroup("Typed");
		m.createParameter("Typed", "type", "TestComponentWithDescriptions");

		m.createParameter("Typed", "strParam", "dummy value");
		m.createParameter("Typed", "strParam2", "not registered");

		m.createParameter("Typed", "intParam", "11");
		m.createParameter("Typed", "intAnInt", "500");
		m.createParameter("Typed", "intWrongRange", "-2");

		m.createParameter("Typed", "realParam", "11.5");
		m.createParameter("Typed", "realWrongRange", "-2.0");

		m.createParameter("Typed", "boolParam", "false");
		m.createParameter("Typed", "boolFlag", "invalid value");
		m.createParameter("Typed", "boolParam2", "1");

		m.createParameter("Typed", "enumParam", "dummo");
		m.createParameter("Typed", "enumParam2", "not registered");
		m.createParameter("Typed", "enumInvalid", "D");

		m.createGroup("Typed/SUBG");
		m.createParameter("Typed/SUBG", "subBool", "false");

		m.createGroup("NoType");

		m.createParameter("NoType", "strAParameter", "and its value");
		m.createParameter("NoType", "intAParameter", "17");
		m.createParameter("NoType", "realAParameter", "17.56");
		m.createParameter("NoType", "realAnotherParameter", "+Inf");
		m.createParameter("NoType", "boolAParameter", "T");
		m.createParameter("NoType", "enumAParameter", "and its value");
	}

	return m;
};

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ConfigurationHelper_Test : public QObject
{
	Q_OBJECT

	typedef QPair<QString, QString> StrPair;

private slots:
	void initTestCase()
	{
		// Registering the class with descriptions
		TypesDB::instance().registerType<TestComponentWithDescriptions>("TestComponentWithDescriptions", QStringList() << "Component");
	}

	void expectedGroupSeparatorAndParentGroup()
	{
		QCOMPARE(GroupSeparator, "/");
		QCOMPARE(ParentGroup, "..");
	}

	void equalKeys()
	{
		QCOMPARE(ConfigurationHelper::configKeysEqual("pippo", "Pluto"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:1", "gruppo:2"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:3", "gruppo:13"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppoD:1", "gruppo:2"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:1a", "gruppo:2"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:pippo", "gruppo:pluto"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:1", "gruppone"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppone", "Gruppone"), false);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:1", "gruppo:1"), true);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppo:1", "gruppo:01"), true);
		QCOMPARE(ConfigurationHelper::configKeysEqual("gruppone", "gruppone"), true);
	}

	void keysOrdering()
	{
		QCOMPARE(ConfigurationHelper::configKeysLessThan("pippo", "Pluto"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:1", "gruppo:2"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:3", "gruppo:13"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppoD:1", "gruppo:2"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:1a", "gruppo:2"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:pippo", "gruppo:pluto"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:1", "gruppone"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:1", "gruppo:1"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:1", "gruppo:01"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppone", "gruppone"), false);

		// These tests are the same of the ones above with the first and second key exchanged
		QCOMPARE(ConfigurationHelper::configKeysLessThan("Pluto", "pippo"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:2", "gruppo:1"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:13", "gruppo:3"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:2", "gruppoD:1"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:2", "gruppo:1a"), true);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:pluto", "gruppo:pippo"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppone", "gruppo:1"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:1", "gruppo:1"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppo:01", "gruppo:1"), false);
		QCOMPARE(ConfigurationHelper::configKeysLessThan("gruppone", "gruppone"), false);
	}

	void keysSorting()
	{
		QStringList allKeys = QStringList() << "pippo" << "Pluto" << "gruppo:3" << "gruppo:13" << "gruppo:01" << "gruppo:1" << "gruppoD:1" << "gruppo:1a" << "gruppo:2" << "gruppo:pippo" << "gruppo:pluto" << "gruppone";

		const QString sortedKeys = "Pluto gruppo:01 gruppo:1 gruppo:2 gruppo:3 gruppo:13 gruppo:1a gruppo:pippo gruppo:pluto gruppoD:1 gruppone pippo";

		std::sort(allKeys.begin(), allKeys.end(), ConfigurationHelper::configKeysLessThan);

		QCOMPARE(allKeys.join(" "), sortedKeys);
	}

	void getDescriptor()
	{
		const ConfigurationManager& m = configurationManager();

		const AbstractDescriptor& d0 = ConfigurationHelper::getDescriptorForParameter(m, "Typed/strParam");

		QCOMPARE(d0.type(), StringDescriptorType);
		const StringDescriptor& sd0 = dynamic_cast<const StringDescriptor&>(d0);
		QCOMPARE(sd0.def(), QString("ummo"));

		const AbstractDescriptor& d1 = ConfigurationHelper::getDescriptorForParameter(m, "Typed/realParam");

		QCOMPARE(d1.type(), RealDescriptorType);
		const RealDescriptor& rd1 = dynamic_cast<const RealDescriptor&>(d1);
		QCOMPARE(rd1.def(), 13.5);
		QCOMPARE(rd1.lowerBound(), -20.0);
		QCOMPARE(rd1.upperBound(), 20.0);

		const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(m, "Typed/SUBG");

		QVERIFY(dc.hasParameterOrSubgroup("subBool"));
		QVERIFY((dynamic_cast<const SubgroupDescriptor*>(&dc) != nullptr));

		const AbstractDescriptor& d2 = ConfigurationHelper::getDescriptorForParameter(m, "Typed/SUBG/subBool");

		QCOMPARE(d2.type(), BoolDescriptorType);
		const BoolDescriptor& bd2 = dynamic_cast<const BoolDescriptor&>(d2);
		QCOMPARE(bd2.def(), true);

		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getDescriptorForParameter(m, "NoType/intAParameter");, ParameterOrSubgroupNotDescribedException);
	}

	void getInt()
	{
		const ConfigurationManager& m = configurationManager();

		QCOMPARE(ConfigurationHelper::getInt(m, "Typed/intParam"), 11);
		QCOMPARE(ConfigurationHelper::getInt(m, "Typed/intName"), MinInteger);
		QCOMPARE(ConfigurationHelper::getInt(m, "Typed/intAnInt"), 500);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getInt(m, "Typed/intWrongRange"), ParameterValueOutOfRangeException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getInt(m, "Typed/intS"), WrongParameterTypeException);
		QCOMPARE(ConfigurationHelper::getInt(m, "Typed/intParam2", 65), 65);
		QCOMPARE(ConfigurationHelper::getInt(m, "NoType/intAParameter", 0), 17);
	}

	void getReal()
	{
		const ConfigurationManager& m = configurationManager();

		QCOMPARE(ConfigurationHelper::getReal(m, "Typed/realParam"), QString("11.5").toDouble());
		QVERIFY(ConfigurationHelper::getReal(m, "Typed/realName") == -Infinity);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getReal(m, "Typed/realWrongRange"), ParameterValueOutOfRangeException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getReal(m, "Typed/realI"), WrongParameterTypeException);
		QCOMPARE(ConfigurationHelper::getReal(m, "Typed/realParam2", 65.43), 65.43);
		QCOMPARE(ConfigurationHelper::getReal(m, "NoType/realAParameter", 0.0), QString("17.56").toDouble());
		QVERIFY(ConfigurationHelper::getReal(m, "NoType/realAnotherParameter", 0.0) == +Infinity);
	}

	void getBool()
	{
		const ConfigurationManager& m = configurationManager();

		QCOMPARE(ConfigurationHelper::getBool(m, "Typed/boolParam"), false);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getBool(m, "Typed/boolFlag"), CannotConvertParameterValueToTypeException);
		QCOMPARE(ConfigurationHelper::getBool(m, "Typed/boolAnotherFlag"), false);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getBool(m, "Typed/boolParam2"), ParameterOrSubgroupNotDescribedException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getBool(m, "Typed/boolI"), WrongParameterTypeException);
		QCOMPARE(ConfigurationHelper::getBool(m, "Typed/boolParam2", false), true);
		QCOMPARE(ConfigurationHelper::getBool(m, "NoType/boolAParameter", false), true);
		QCOMPARE(ConfigurationHelper::getBool(m, "NoType/boolAnotherParameter", true), true);
	}

	void getString()
	{
		const ConfigurationManager& m = configurationManager();

		QCOMPARE(ConfigurationHelper::getString(m, "Typed/strParam"), QString("dummy value"));
		QCOMPARE(ConfigurationHelper::getString(m, "Typed/strName"), QString("Jimmy"));
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getString(m, "Typed/strParam2"), ParameterOrSubgroupNotDescribedException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getString(m, "Typed/strI"), WrongParameterTypeException);
		QCOMPARE(ConfigurationHelper::getString(m, "Typed/strParam2", "myDefault"), QString("not registered"));
		QCOMPARE(ConfigurationHelper::getString(m, "NoType/strAParameter", "def1"), QString("and its value"));
		QCOMPARE(ConfigurationHelper::getString(m, "NoType/strAnotherParameter", "def2"), QString("def2"));
	}

	void getEnum()
	{
		const ConfigurationManager& m = configurationManager();

		QCOMPARE(ConfigurationHelper::getEnum(m, "Typed/enumParam"), QString("dummo"));
		QCOMPARE(ConfigurationHelper::getEnum(m, "Typed/enumName"), QString("Jimmy"));
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getEnum(m, "Typed/enumParam2"), ParameterOrSubgroupNotDescribedException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getEnum(m, "Typed/enumI"), WrongParameterTypeException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getEnum(m, "Typed/enumStrangeDefault"), InvalidEnumParameterDefaultException);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::getEnum(m, "Typed/enumInvalid"), InvalidEnumValueException);
		QCOMPARE(ConfigurationHelper::getEnum(m, "Typed/enumParam2", "myDefault"), QString("not registered"));
		QCOMPARE(ConfigurationHelper::getEnum(m, "NoType/enumAParameter", "def1"), QString("and its value"));
		QCOMPARE(ConfigurationHelper::getEnum(m, "NoType/enumAnotherParameter", "def2"), QString("def2"));
	}

	void encodeDecodeListOfStrings()
	{
		QStringList list = QStringList() << "one " << "two/three" << "four in a row";

		const QString encodedList = ConfigurationHelper::encodeListOfStrings(list);

		QCOMPARE(encodedList, QString("one /two\\/three/four in a row"));
		QCOMPARE(ConfigurationHelper::decodeListOfStrings(encodedList), list);
		QCOMPARE(ConfigurationHelper::decodeListOfStrings(""), QStringList());
		QCOMPARE(ConfigurationHelper::decodeListOfStrings(ConfigurationHelper::encodeListOfStrings(QStringList())), QStringList());
	}

	void encodeDecodeListOfBools()
	{
		QVector<bool> list;
		list.resize(10);
		list[0] = true;
		list[1] = true;
		list[2] = false;
		list[3] = false;
		list[4] = true;
		list[5] = false;
		list[6] = true;
		list[7] = true;
		list[8] = false;
		list[9] = false;

		const QString encodedList = ConfigurationHelper::encodeListOfBools(list);

		QCOMPARE(ConfigurationHelper::decodeListOfBools(encodedList), list);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::decodeListOfBools("invalid"), ListDecodeFailedException);
	}

	void encodeDecodeListOfReals()
	{
		QVector<double> list;
		list.resize(10);
		list[0] = 0.0;
		list[1] = -1.3;
		list[2] = 17.8;
		list[3] = +Infinity;
		list[4] = -0.000002;
		list[5] = 4.45;
		list[6] = -Infinity;
		list[7] = 6.4;
		list[8] = -43.5;
		list[9] = 775.3;

		const QString encodedList = ConfigurationHelper::encodeListOfReals(list);

		QCOMPARE(ConfigurationHelper::decodeListOfReals(encodedList), list);
		QVERIFY_EXCEPTION_THROWN(ConfigurationHelper::decodeListOfBools("1.0 dummy -4.3"), ListDecodeFailedException);
	}

	void encodeDecodeListOfInts()
	{
		QVector<int> list;
		list.resize(10);
		list[0] = 3;
		list[1] = -13;
		list[2] = 17;
		list[3] = MinInteger;
		list[4] = 876;
		list[5] = 9987;
		list[6] = -2144;
		list[7] = MaxInteger;
		list[8] = -74;
		list[9] = -1;

		const QString encodedList = ConfigurationHelper::encodeListOfInts(list);

		QCOMPARE(ConfigurationHelper::decodeListOfInts(encodedList), list);
	}
};

QTEST_MAIN(ConfigurationHelper_Test)
#include "configurationhelper_test.moc"
