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

#include "configurationmanager.h"
#include "component.h"
#include "typesdb.h"
#include "componentdescriptors.h"
#include "testutils.h"
#include "testhierarchy.h"
#include <QtTest/QtTest>

// NOTES AND TODOS
//

using namespace farsa;

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
		d.help("short help", "not very long help string");

		// String
		d.describeString("str").def("dummy").props(ParamIsMandatory | ParamAllowMultiple).help("short help", "long help");
		d.describeString("strBare").help("sh");

		// Int
		d.describeInt("i").def(10).props(ParamIsMandatory).help("short help", "long help");
		d.describeInt("i2").props(ParamIsList).limits(MinInteger, 15);
		d.describeInt("i3").def(100).limits(0, MaxInteger);

		// Real
		d.describeReal("r").props(ParamIsMandatory | ParamIsList).limits(0.0, Infinity).help("help", "help me a lot");
		d.describeReal("real").def(-313.17).props(ParamIsList).limits(-2.0, 2.0).help("h");

		// Bool
		d.describeBool("bool").props(ParamIsList).help("oh", "oh oh oh");
		d.describeBool("10").def(true).props(ParamAllowMultiple);

		// Enum
		d.describeEnum("enum").def("value3").props(ParamIsMandatory).values(QStringList() << "value1" << "value2" << "value3");
		d.describeEnum("param").values(QStringList() << "timmy" << "tommy" << "jimmy" << "red" << "green" << "blue").help("little");

		// Component
		d.describeComponent("subc").props(ParamIsMandatory).componentType("Strange").help("jingle");
		d.describeComponent("owned").componentType("Another").help("all the way", "horse open sleight");

		// Subgroup
		SubgroupDescriptor& s1 = d.describeSubgroup("subg").props(ParamIsMandatory | ParamIsList).help("sub help");
		s1.describeString("subp").def("dummy").help("missing");
		d.describeSubgroup("subcomp").componentType("AComponent").help("subcomponent help string", "verbose one");
	}
};

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ComponentDescriptionWithDescriptors_Test : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase()
	{
		// Registering the class with descriptions
		TypesDB::instance().registerType<TestComponentWithDescriptions>("TestComponentWithDescriptions", QStringList() << "Component");
	}

	void checkRegistrationSuccessful()
	{
		QVERIFY(TypesDB::instance().isTypeRegistered("TestComponentWithDescriptions"));
		QVERIFY(TypesDB::instance().isTypeRegisteredAndComponent("TestComponentWithDescriptions"));
	}

	void describeType()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QCOMPARE(d.type(), QString("TestComponentWithDescriptions"));
		QCOMPARE(d.shortHelp(), QString("short help"));
		QCOMPARE(d.longHelp(), QString("not very long help string"));
	}

	void describeString()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("str"));
		QCOMPARE(d.descriptorType("str"), StringDescriptorType);

		const StringDescriptor& strD = d.stringDescriptor("str");

		QCOMPARE(strD.type(), StringDescriptorType);
		QCOMPARE(strD.shortHelp(), QString("short help"));
		QCOMPARE(strD.longHelp(), QString("long help"));
		QVERIFY(strD.hasDefault());
		QCOMPARE(strD.def(), QString("dummy"));
		QCOMPARE(strD.props(), (ParamIsMandatory | ParamAllowMultiple));

		QVERIFY(d.hasParameterOrSubgroup("strBare"));
		QCOMPARE(d.descriptorType("strBare"), StringDescriptorType);

		const StringDescriptor& strBareD = d.stringDescriptor("strBare");

		QCOMPARE(strBareD.type(), StringDescriptorType);
		QCOMPARE(strBareD.shortHelp(), QString("sh"));
		QCOMPARE(strBareD.longHelp(), QString("sh"));
		QVERIFY(!strBareD.hasDefault());
		QVERIFY_EXCEPTION_THROWN(strBareD.def(), ParameterNotRegisteredOrWithoutDefaultException);
		QCOMPARE(strBareD.props(), ParamDefault);
	}

	void describeInt()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("i"));
		QCOMPARE(d.descriptorType("i"), IntDescriptorType);

		const IntDescriptor& iD = d.intDescriptor("i");

		QCOMPARE(iD.type(), IntDescriptorType);
		QCOMPARE(iD.shortHelp(), QString("short help"));
		QCOMPARE(iD.longHelp(), QString("long help"));
		QVERIFY(iD.hasDefault());
		QCOMPARE(iD.def(), 10);
		QCOMPARE(iD.props(), ParamIsMandatory);
		QCOMPARE(iD.lowerBound(), MinInteger);
		QCOMPARE(iD.upperBound(), MaxInteger);

		QVERIFY(d.hasParameterOrSubgroup("i2"));
		QCOMPARE(d.descriptorType("i2"), IntDescriptorType);

		const IntDescriptor& i2D = d.intDescriptor("i2");

		QCOMPARE(i2D.type(), IntDescriptorType);
		QCOMPARE(i2D.shortHelp(), QString());
		QCOMPARE(i2D.longHelp(), QString());
		QVERIFY(!i2D.hasDefault());
		QCOMPARE(i2D.props(), ParamIsList);
		QCOMPARE(i2D.lowerBound(), MinInteger);
		QCOMPARE(i2D.upperBound(), 15);

		QVERIFY(d.hasParameterOrSubgroup("i3"));
		QCOMPARE(d.descriptorType("i3"), IntDescriptorType);

		const IntDescriptor& i3D = d.intDescriptor("i3");

		QCOMPARE(i3D.type(), IntDescriptorType);
		QCOMPARE(i3D.shortHelp(), QString());
		QCOMPARE(i3D.longHelp(), QString());
		QVERIFY(i3D.hasDefault());
		QCOMPARE(i3D.def(), 100);
		QCOMPARE(i3D.props(), ParamDefault);
		QCOMPARE(i3D.lowerBound(), 0);
		QCOMPARE(i3D.upperBound(), MaxInteger);
	}

	void describeReal()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("r"));
		QCOMPARE(d.descriptorType("r"), RealDescriptorType);

		const RealDescriptor& rD = d.realDescriptor("r");

		QCOMPARE(rD.type(), RealDescriptorType);
		QCOMPARE(rD.shortHelp(), QString("help"));
		QCOMPARE(rD.longHelp(), QString("help me a lot"));
		QVERIFY(!rD.hasDefault());
		QCOMPARE(rD.props(), (ParamIsMandatory | ParamIsList));
		QCOMPARE(rD.lowerBound(), 0.0);
		// Cannot use QCOMPARE because it doesn't seem to work with Infinity
		QVERIFY((rD.upperBound() == +Infinity));

		QVERIFY(d.hasParameterOrSubgroup("real"));
		QCOMPARE(d.descriptorType("real"), RealDescriptorType);

		const RealDescriptor& realD = d.realDescriptor("real");

		QCOMPARE(realD.type(), RealDescriptorType);
		QCOMPARE(realD.shortHelp(), QString("h"));
		QCOMPARE(realD.longHelp(), QString("h"));
		QVERIFY(realD.hasDefault());
		QCOMPARE(realD.def(), -313.17);
		QCOMPARE(realD.props(), ParamIsList);
		QCOMPARE(realD.lowerBound(), -2.0);
		QCOMPARE(realD.upperBound(), 2.0);
	}

	void describeBool()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("bool"));
		QCOMPARE(d.descriptorType("bool"), BoolDescriptorType);

		const BoolDescriptor& boolD = d.boolDescriptor("bool");

		QCOMPARE(boolD.type(), BoolDescriptorType);
		QCOMPARE(boolD.shortHelp(), QString("oh"));
		QCOMPARE(boolD.longHelp(), QString("oh oh oh"));
		QVERIFY(!boolD.hasDefault());
		QCOMPARE(boolD.props(), ParamIsList);

		QVERIFY(d.hasParameterOrSubgroup("10"));
		QCOMPARE(d.descriptorType("10"), BoolDescriptorType);

		const BoolDescriptor& ioD = d.boolDescriptor("10");

		QCOMPARE(ioD.type(), BoolDescriptorType);
		QCOMPARE(ioD.shortHelp(), QString());
		QCOMPARE(ioD.longHelp(), QString());
		QVERIFY(ioD.hasDefault());
		QCOMPARE(ioD.def(), true);
		QCOMPARE(ioD.props(), ParamAllowMultiple);
	}

	void describeEnum()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("enum"));
		QCOMPARE(d.descriptorType("enum"), EnumDescriptorType);

		const EnumDescriptor& enumD = d.enumDescriptor("enum");

		QCOMPARE(enumD.type(), EnumDescriptorType);
		QCOMPARE(enumD.shortHelp(), QString());
		QCOMPARE(enumD.longHelp(), QString());
		QVERIFY(enumD.hasDefault());
		QCOMPARE(enumD.def(), QString("value3"));
		QCOMPARE(enumD.props(), ParamIsMandatory);
		QCOMPARE(enumD.values(), QStringList() << "value1" << "value2" << "value3");

		QVERIFY(d.hasParameterOrSubgroup("param"));
		QCOMPARE(d.descriptorType("param"), EnumDescriptorType);

		const EnumDescriptor& paramD = d.enumDescriptor("param");

		QCOMPARE(paramD.type(), EnumDescriptorType);
		QCOMPARE(paramD.shortHelp(), QString("little"));
		QCOMPARE(paramD.longHelp(), QString("little"));
		QVERIFY(!paramD.hasDefault());
		QCOMPARE(paramD.props(), ParamDefault);
		QCOMPARE(paramD.values(), QStringList() << "timmy" << "tommy" << "jimmy" << "red" << "green" << "blue");
	}

	void describeComponent()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("subc"));
		QCOMPARE(d.descriptorType("subc"), ComponentDescriptorType);

		const ComponentDescriptor& subcD = d.componentDescriptor("subc");

		QCOMPARE(subcD.type(), ComponentDescriptorType);
		QCOMPARE(subcD.shortHelp(), QString("jingle"));
		QCOMPARE(subcD.longHelp(), QString("jingle"));
		QCOMPARE(subcD.props(), ParamIsMandatory);
		QCOMPARE(subcD.componentType(), QString("Strange"));

		QVERIFY(d.hasParameterOrSubgroup("owned"));
		QCOMPARE(d.descriptorType("owned"), ComponentDescriptorType);

		const ComponentDescriptor& ownedD = d.componentDescriptor("owned");

		QCOMPARE(ownedD.type(), ComponentDescriptorType);
		QCOMPARE(ownedD.shortHelp(), QString("all the way"));
		QCOMPARE(ownedD.longHelp(), QString("horse open sleight"));
		QCOMPARE(ownedD.props(), ParamDefault);
		QCOMPARE(ownedD.componentType(), QString("Another"));
	}

	void describeSubgroup()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY(d.hasParameterOrSubgroup("subg"));
		QCOMPARE(d.descriptorType("subg"), SubgroupDescriptorType);

		const SubgroupDescriptor& subgD = d.subgroupDescriptor("subg");

		QCOMPARE(subgD.type(), SubgroupDescriptorType);
		QCOMPARE(subgD.shortHelp(), QString("sub help"));
		QCOMPARE(subgD.longHelp(), QString("sub help"));
		QCOMPARE(subgD.props(), (ParamIsMandatory | ParamIsList));
		QCOMPARE(subgD.componentType(), QString());
		QCOMPARE(subgD.stringDescriptor("subp").type(), StringDescriptorType);

		QVERIFY(d.hasParameterOrSubgroup("subcomp"));
		QCOMPARE(d.descriptorType("subcomp"), SubgroupDescriptorType);

		const SubgroupDescriptor& subcompD = d.subgroupDescriptor("subcomp");

		QCOMPARE(subcompD.type(), SubgroupDescriptorType);
		QCOMPARE(subcompD.shortHelp(), QString("subcomponent help string"));
		QCOMPARE(subcompD.longHelp(), QString("verbose one"));
		QCOMPARE(subcompD.props(), ParamDefault);
		QCOMPARE(subcompD.componentType(), QString("AComponent"));
	}

	void checkListOfParametersAndSubgroups()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		const QSet<QString> params = QSet<QString>::fromList(d.parameters());
		const QSet<QString> expectedParams = QSet<QString>() << "str" << "strBare" << "i" << "i2" << "i3" << "r" << "real" << "bool" << "10" << "enum" << "param" << "subc" << "owned";

		QCOMPARE(expectedParams, params);

		const QSet<QString> subgroups = QSet<QString>::fromList(d.subgroups());
		const QSet<QString> expectedSubgroups = QSet<QString>() << "subg" << "subcomp";

		QCOMPARE(expectedSubgroups, subgroups);

		const SubgroupDescriptor& subgD = d.subgroupDescriptor("subg");

		const QSet<QString> subgParams = QSet<QString>::fromList(subgD.parameters());
		const QSet<QString> subgExpectedParams = QSet<QString>() << "subp";

		QCOMPARE(subgExpectedParams, subgParams);
	}

	void checkExceptionsForInvalidTypes()
	{
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor("TestComponentWithDescriptions");

		QVERIFY_EXCEPTION_THROWN(d.intDescriptor("str"), WrongParameterTypeException);
		QVERIFY_EXCEPTION_THROWN(d.stringDescriptor("i"), WrongParameterTypeException);
	}
};

QTEST_MAIN(ComponentDescriptionWithDescriptors_Test)
#include "componentdescriptionwithdescriptors_test.moc"
