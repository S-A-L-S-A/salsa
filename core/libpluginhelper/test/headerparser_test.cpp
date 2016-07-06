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
#include "pluginhelper.h"
#include "utilitiesexceptions.h"

// NOTES AND TODOS
//

#warning PERMETTERE DI SPECIFCARE IL CREATOR DEI COMPONENTI NEI PLUGIN: AGGIUNGERE UN TYPEDEF A COMPONENT (AD ESEMPIO Component::Creator) CHE SE PRESENTE È IL TIPO DEL CREATORE E/O UNA MACRO CON LA QUALE SPECIFICARE IL CREATOR E UNA CON LA QUALE SPECIFICARE UNA FUNZIONE CHE RITORNA IL CREATOR (PER LE REGISTRAZIONI CON UNA ISTANZA DEL CREATOR INVECE CHE SEMPLICEMENTE CON IL TIPO DEL CREATOR)

#warning TOGLIERE I NAMESPACES DALLE CLASSI PARENT QUANDO SI REGISTRA VEDI COMMENTO SOTTO
// ALTRIMENTI RISCHIAMO DI AVERE PROBLEMI (AD ESEMPIO NEL TYPESDB C'È Component, NON
// farsa::Component, MA SE UNA CLASSE EREDITA DA farsa::Component AL MOMENTO SI USEREBBE
// COME PARENT farsa::Component CHE NON VIENE RICONOSCIUTO)

using namespace farsa;
using namespace farsa::pluginHelper;

const char* h0 = "\n\
class FARSA_PLUGIN_API C0 {};\n\
";

const char* h1 = "\n\
class FARSA_PLUGIN_API C1 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
";

const char* h2 = "\n\
class FARSA_PLUGIN_API C1 : public Component\n\
{\n\
	// FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
";

const char* h3 = "\n\
class C1\n\
{\n\
	// FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
\n\
class FARSA_PLUGIN_API C2 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C2(ConfigurationManager& params);\n\
};\n\
";

const char* h4 = "\n\
class FARSA_PLUGIN_API C1\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
\n\
class FARSA_PLUGIN_API C2 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C2(ConfigurationManager& params);\n\
};\n\
";

const char* h5 = "\n\
//class FARSA_PLUGIN_API C1\n\
//{\n\
//	FARSA_REGISTER_CLASS\n\
//public:\n\
//	C1(ConfigurationManager& params);\n\
//};\n\
\n\
class FARSA_PLUGIN_API C2 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C2(ConfigurationManager& params);\n\
};\n\
";

const char* h6 = "\n\
class FARSA_PLUGIN_API C1\n\
{\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
\n\
class FARSA_PLUGIN_API C2 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C2(ConfigurationManager& params);\n\
};\n\
";

const char* h7 = "\n\
class C1\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
\n\
class FARSA_PLUGIN_API C2 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C2(ConfigurationManager& params);\n\
};\n\
";

const char* h8 = "\n\
class FARSA_PLUGIN_API C1\n\
{\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
\n\
class C2 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C2(ConfigurationManager& params);\n\
};\n\
";

const char* h9 = "\n\
class FARSA_PLUGIN_API C1\n\
{}\n\
FARSA_REGISTER_CLASS\n\
";

const char* h10 = "\n\
class FARSA_PLUGIN_API C1;\n\
FARSA_REGISTER_CLASS\n\
";

const char* h11 = "\n\
namespace N1 {\n\
class FARSA_PLUGIN_API C1 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
}\n\
";

const char* h12 = "\n\
namespace N1 {\n\
namespace N2 {\n\
class FARSA_PLUGIN_API C1 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
}\n\
}\n\
";

const char* h13 = "\n\
namespace N1 {\n\
\n\
namespace No {\n\
}\n\
\n\
namespace N2 {\n\
class FARSA_PLUGIN_API C1 : public Component\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1(ConfigurationManager& params);\n\
};\n\
}\n\
}\n\
";

const char* h14 = "\n\
class FARSA_PLUGIN_API C1\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
";

const char* h15 = "\n\
class FARSA_PLUGIN_API C1 : public P1, private P2, public P3\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
";

const char* h16 = "\n\
class FARSA_PLUGIN_API C1 : public P1, private P2, public FARSA_NR(P3)\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
";

const char* h17 = "\n\
class FARSA_PLUGIN_API C1 : public P1, private P2, public P3\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
\n\
class FARSA_PLUGIN_TEMPLATE C2\n\
{\n\
};\n\
\n\
class FARSA_PLUGIN_INTERNAL Int\n\
{\n\
};\n\
";

const char* h17_mangled = "\n\
class FARSA_PLUGIN_API_IMPORT C1 : public P1, private P2, public P3\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
\n\
class FARSA_PLUGIN_TEMPLATE_IMPORT C2\n\
{\n\
};\n\
\n\
class FARSA_PLUGIN_INTERNAL_IMPORT Int\n\
{\n\
};\n\
";

const char* h18 = "\n\
class FARSA_PLUGIN_API C1 : public P1, private P2, public FARSA_NR(P3)\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
\n\
FARSA_PRE_REGISTRATION_FUNCTION(firstFun)\n\
FARSA_PRE_REGISTRATION_FUNCTION(otherFunction)\n\
";

const char* h19 = "\n\
class FARSA_PLUGIN_API C1 : public P1, private P2, public FARSA_NR(P3)\n\
{\n\
	FARSA_REGISTER_CLASS\n\
public:\n\
	C1();\n\
};\n\
\n\
FARSA_PRE_REGISTRATION_FUNCTION(firstFun)\n\
FARSA_PRE_REGISTRATION_FUNCTION(otherFunction)\n\
\n\
FARSA_POST_REGISTRATION_FUNCTION(dummy)\n\
FARSA_POST_REGISTRATION_FUNCTION(dumbFunction)\n\
";

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class HeaderParser_Test : public QObject
{
	Q_OBJECT

private slots:
	void checkClassesToRegisterNotPresent()
	{
		HeaderParser parser(h0);

		QVERIFY(!parser.classesToRegisterPresent());
	}

	void checkClassesToRegisterPresent()
	{
		HeaderParser parser(h1);

		QVERIFY(parser.classesToRegisterPresent());
	}

	void checkClassesToRegisterNotPresentBecauseInComment()
	{
		HeaderParser parser(h2);

		QVERIFY(!parser.classesToRegisterPresent());
	}

	void checkClassesToRegisterPresentOneInComment()
	{
		HeaderParser parser(h3);

		QVERIFY(parser.classesToRegisterPresent());
	}

	void checkNumClassesToRegister()
	{
		HeaderParser p0(h0);
		HeaderParser p1(h1);
		HeaderParser p2(h2);
		HeaderParser p3(h3);
		HeaderParser p4(h4);

		QCOMPARE(p0.numClassesToRegister(), 0);
		QCOMPARE(p1.numClassesToRegister(), 1);
		QCOMPARE(p2.numClassesToRegister(), 0);
		QCOMPARE(p3.numClassesToRegister(), 1);
		QCOMPARE(p4.numClassesToRegister(), 2);
	}

	void checkGetClassNames()
	{
		HeaderParser p1(h1);
		HeaderParser p3(h3);
		HeaderParser p4(h4);

		QCOMPARE(p1.numClassesToRegister(), 1);
		QCOMPARE(p1.className(0), QString("C1"));
		QCOMPARE(p3.numClassesToRegister(), 1);
		QCOMPARE(p3.className(0), QString("C2"));
		QCOMPARE(p4.numClassesToRegister(), 2);
		QCOMPARE(p4.className(0), QString("C1"));
		QCOMPARE(p4.className(1), QString("C2"));
	}

	void checkGetClassNamesWithCommentedClass()
	{
		HeaderParser parser(h5);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.className(0), QString("C2"));
	}

	void checkGetClassNamesWithClassNotToRegister()
	{
		HeaderParser parser(h6);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.className(0), QString("C2"));
	}

	void checkInvalidHeadersAreRecognized()
	{
		QVERIFY_EXCEPTION_THROWN((HeaderParser(h7)), RuntimeUserException);
		QVERIFY_EXCEPTION_THROWN((HeaderParser(h8)), RuntimeUserException);
		QVERIFY_EXCEPTION_THROWN((HeaderParser(h9)), RuntimeUserException);
		QVERIFY_EXCEPTION_THROWN((HeaderParser(h10)), RuntimeUserException);
	}

	void checkClassesWithNoNamespaces()
	{
		HeaderParser parser(h1);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.namespacesForClass(0), QStringList());
	}

	void checkClassesWithOneNamespace()
	{
		HeaderParser parser(h11);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.namespacesForClass(0), QStringList() << "N1");
	}

	void checkClassesWithTwoNamespaces()
	{
		HeaderParser parser(h12);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.namespacesForClass(0), QStringList() << "N1" << "N2");
	}

	void checkClassesWithTwoNamespacesAndOtherNamespaces()
	{
		HeaderParser parser(h13);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.namespacesForClass(0), QStringList() << "N1" << "N2");
	}

	void checkFullyQualifiedClassName()
	{
		HeaderParser p1(h1);
		HeaderParser p11(h11);
		HeaderParser p12(h12);

		QCOMPARE(p1.numClassesToRegister(), 1);
		QCOMPARE(p1.fullyQualifiedClassName(0), QString("C1"));
		QCOMPARE(p11.numClassesToRegister(), 1);
		QCOMPARE(p11.fullyQualifiedClassName(0), QString("N1::C1"));
		QCOMPARE(p12.numClassesToRegister(), 1);
		QCOMPARE(p12.fullyQualifiedClassName(0), QString("N1::N2::C1"));
	}

	void checkClassWithNoParent()
	{
		HeaderParser parser(h14);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.parentsForClass(0), QStringList());
	}

	void checkClassWithParent()
	{
		HeaderParser parser(h1);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.parentsForClass(0), QStringList() << "Component");
	}

	void checkClassWithParents()
	{
		HeaderParser parser(h15);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.parentsForClass(0), QStringList() << "P1" << "P3");
	}

	void checkClassWithParentsToSkip()
	{
		HeaderParser parser(h16);

		QCOMPARE(parser.numClassesToRegister(), 1);
		QCOMPARE(parser.parentsForClass(0), QStringList() << "P1");
	}

	void checkMangledHeader()
	{
		HeaderParser parser(h17);

		QCOMPARE(parser.mangledHeader(), QString(h17_mangled));
	}

	void checkNoPreRegistrationFunctions()
	{
		HeaderParser parser(h16);

		QCOMPARE(parser.preRegistrationFunctions().size(), 0);
	}

	void checkPreRegistrationFunctions()
	{
		HeaderParser parser(h18);

		QCOMPARE(parser.preRegistrationFunctions().size(), 2);
		QCOMPARE(parser.preRegistrationFunctions()[0], QString("firstFun"));
		QCOMPARE(parser.preRegistrationFunctions()[1], QString("otherFunction"));
	}

	void checkNoPostRegistrationFunctions()
	{
		HeaderParser parser(h16);

		QCOMPARE(parser.postRegistrationFunctions().size(), 0);
	}

	void checkPostRegistrationFunctions()
	{
		HeaderParser parser(h19);

		QCOMPARE(parser.postRegistrationFunctions().size(), 2);
		QCOMPARE(parser.postRegistrationFunctions()[0], QString("dummy"));
		QCOMPARE(parser.postRegistrationFunctions()[1], QString("dumbFunction"));
	}
};

QTEST_MAIN(HeaderParser_Test)
#include "headerparser_test.moc"
