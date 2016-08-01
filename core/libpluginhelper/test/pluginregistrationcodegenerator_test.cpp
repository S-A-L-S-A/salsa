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
#include "pluginhelper.h"
#include "utilitiesexceptions.h"

// NOTES AND TODOS
//

#warning PERMETTERE DI SPECIFCARE IL CREATOR DEI COMPONENTI NEI PLUGIN: AGGIUNGERE UN TYPEDEF A COMPONENT (AD ESEMPIO Component::Creator) CHE SE PRESENTE È IL TIPO DEL CREATORE E/O UNA MACRO CON LA QUALE SPECIFICARE IL CREATOR E UNA CON LA QUALE SPECIFICARE UNA FUNZIONE CHE RITORNA IL CREATOR (PER LE REGISTRAZIONI CON UNA ISTANZA DEL CREATOR INVECE CHE SEMPLICEMENTE CON IL TIPO DEL CREATOR)

using namespace salsa;
using namespace salsa::pluginHelper;

////////////////////////////////////////////////////////////////////////////////

const char* noClassToRegister_0 = "\
class SALSA_PLUGIN_API Dummy1\n\
{\n\
};\n\
";

const char* noClassToRegister_1 = "\
class SALSA_PLUGIN_API Dummy2\n\
{\n\
};\n\
";

const char* noClassToRegisterButDeps_h = "\
#ifndef PLUGIN_MAIN_H\n\
#define PLUGIN_MAIN_H\n\
\n\
#include <QObject>\n\
#include \"salsaplugin.h\"\n\
\n\
class DummyPlugin : public QObject, public SalsaPlugin\n\
{\n\
	Q_OBJECT\n\
	SALSA_PLUGIN_METADATA(IID SalsaPlugin_IID)\n\
	Q_INTERFACES(SalsaPlugin)\n\
\n\
public:\n\
	virtual void registerTypes();\n\
	virtual QStringList getDependencies();\n\
};\n\
\n\
#endif\n\
\n\
";

const char* noClassToRegisterButDeps_cpp = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
#include \"header1.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList() << \"dep1\" << \"dep2\";\n\
}\n\
\n\
";

////////////////////////////////////////////////////////////////////////////////

const char* noClassToRegisterButPrePostFnc_0 = "\
class SALSA_PLUGIN_API Dummy1\n\
{\n\
};\n\
SALSA_PRE_REGISTRATION_FUNCTION(pre00)\n\
SALSA_PRE_REGISTRATION_FUNCTION(pre01)\n\
SALSA_POST_REGISTRATION_FUNCTION(post00)\n\
";

const char* noClassToRegisterButPrePostFnc_1 = "\
class SALSA_PLUGIN_API Dummy2\n\
{\n\
};\n\
SALSA_PRE_REGISTRATION_FUNCTION(pre10)\n\
SALSA_POST_REGISTRATION_FUNCTION(post10)\n\
SALSA_POST_REGISTRATION_FUNCTION(post11)\n\
";

const char* noClassToRegisterButPrePostFnc_h = "\
#ifndef PLUGIN_MAIN_H\n\
#define PLUGIN_MAIN_H\n\
\n\
#include <QObject>\n\
#include \"salsaplugin.h\"\n\
\n\
class DummyPlugin : public QObject, public SalsaPlugin\n\
{\n\
	Q_OBJECT\n\
	SALSA_PLUGIN_METADATA(IID SalsaPlugin_IID)\n\
	Q_INTERFACES(SalsaPlugin)\n\
\n\
public:\n\
	virtual void registerTypes();\n\
	virtual QStringList getDependencies();\n\
};\n\
\n\
#endif\n\
\n\
";

const char* noClassToRegisterButPrePostFnc_cpp = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
#include \"header1.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
	pre00();\n\
	pre01();\n\
	pre10();\n\
	post00();\n\
	post10();\n\
	post11();\n\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList() << \"dep1\" << \"dep2\";\n\
}\n\
\n\
";

////////////////////////////////////////////////////////////////////////////////

const char* singleClassToRegister = "\
class SALSA_PLUGIN_API Dummy\n\
{\n\
SALSA_REGISTER_CLASS\n\
};\n\
";

const char* singleClassToRegister_h = "\
#ifndef PLUGIN_MAIN_H\n\
#define PLUGIN_MAIN_H\n\
\n\
#include <QObject>\n\
#include \"salsaplugin.h\"\n\
\n\
class DummyPlugin : public QObject, public SalsaPlugin\n\
{\n\
	Q_OBJECT\n\
	SALSA_PLUGIN_METADATA(IID SalsaPlugin_IID)\n\
	Q_INTERFACES(SalsaPlugin)\n\
\n\
public:\n\
	virtual void registerTypes();\n\
	virtual QStringList getDependencies();\n\
};\n\
\n\
#endif\n\
\n\
";

const char* singleClassToRegister_cpp = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
\tsalsa::TypesDB::instance().registerType<Dummy>(\"Dummy\", QStringList());\n\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList();\n\
}\n\
\n\
";

////////////////////////////////////////////////////////////////////////////////

const char* singleClassToRegisterWithParents = "\
class SALSA_PLUGIN_API Dummy : public Parent1, public Parent2, privare NoReg\n\
{\n\
SALSA_REGISTER_CLASS\n\
};\n\
";

const char* singleClassToRegisterWithParents_h = "\
#ifndef PLUGIN_MAIN_H\n\
#define PLUGIN_MAIN_H\n\
\n\
#include <QObject>\n\
#include \"salsaplugin.h\"\n\
\n\
class DummyPlugin : public QObject, public SalsaPlugin\n\
{\n\
	Q_OBJECT\n\
	SALSA_PLUGIN_METADATA(IID SalsaPlugin_IID)\n\
	Q_INTERFACES(SalsaPlugin)\n\
\n\
public:\n\
	virtual void registerTypes();\n\
	virtual QStringList getDependencies();\n\
};\n\
\n\
#endif\n\
\n\
";

const char* singleClassToRegisterWithParents_cpp0 = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
\tsalsa::TypesDB::instance().registerType<Dummy>(\"Dummy\", QStringList() << \"Parent1\" << \"Parent2\");\n\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList();\n\
}\n\
\n\
";

const char* singleClassToRegisterWithParents_cpp1 = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
\tsalsa::TypesDB::instance().registerType<Dummy>(\"Dummy\", QStringList() << \"Parent2\" << \"Parent1\");\n\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList();\n\
}\n\
\n\
";

////////////////////////////////////////////////////////////////////////////////

const char* classesToRegisterWithDependencies_0 = "\
class SALSA_PLUGIN_API Dummy : public Parent1, public Parent2, privare NoReg\n\
{\n\
SALSA_REGISTER_CLASS\n\
};\n\
";

const char* classesToRegisterWithDependencies_1 = "\
class SALSA_PLUGIN_API Parent1 : public Timmy\n\
{\n\
SALSA_REGISTER_CLASS\n\
};\n\
";

const char* classesToRegisterWithDependencies_h = "\
#ifndef PLUGIN_MAIN_H\n\
#define PLUGIN_MAIN_H\n\
\n\
#include <QObject>\n\
#include \"salsaplugin.h\"\n\
\n\
class DummyPlugin : public QObject, public SalsaPlugin\n\
{\n\
	Q_OBJECT\n\
	SALSA_PLUGIN_METADATA(IID SalsaPlugin_IID)\n\
	Q_INTERFACES(SalsaPlugin)\n\
\n\
public:\n\
	virtual void registerTypes();\n\
	virtual QStringList getDependencies();\n\
};\n\
\n\
#endif\n\
\n\
";

// Either this or the following one are correct
const char* classesToRegisterWithDependencies_cpp0 = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
#include \"header1.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
\tsalsa::TypesDB::instance().registerType<Parent1>(\"Parent1\", QStringList() << \"Timmy\");\n\
\tsalsa::TypesDB::instance().registerType<Dummy>(\"Dummy\", QStringList() << \"Parent1\" << \"Parent2\");\n\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList();\n\
}\n\
\n\
";

const char* classesToRegisterWithDependencies_cpp1 = "\
#include <QtPlugin>\n\
#include \"plugin_main.h\"\n\
#include \"typesdb.h\"\n\
#include \"header0.h\"\n\
#include \"header1.h\"\n\
\n\
void DummyPlugin::registerTypes()\n\
{\n\
\tsalsa::TypesDB::instance().registerType<Parent1>(\"Parent1\", QStringList() << \"Timmy\");\n\
\tsalsa::TypesDB::instance().registerType<Dummy>(\"Dummy\", QStringList() << \"Parent2\" << \"Parent1\");\n\
}\n\
\n\
QStringList DummyPlugin::getDependencies()\n\
{\n\
	return QStringList();\n\
}\n\
\n\
";

////////////////////////////////////////////////////////////////////////////////

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class PluginRegistrationCodeGenerator_Test : public QObject
{
	Q_OBJECT

private:
	typedef QList<PluginRegistrationCodeGenerator::FilenameAndParsedHeader> FPH;

private slots:
	void checkEmptyRegistrationCodeWhenEmptyHeadersAndNoDependencies()
	{
		PluginRegistrationCodeGenerator p("plugin_main", "Dummy", FPH(), QStringList());

		QVERIFY(p.registrationHeader().isEmpty());
		QVERIFY(p.registrationSource().isEmpty());
	}

	void checkEmptyRegistrationCodeWhenNoClassToRegisterAndNoDependencies()
	{
		const FPH l = generateListOfHeaders(QStringList() << noClassToRegister_0 << noClassToRegister_1);
		PluginRegistrationCodeGenerator p("plugin_main", "Dummy", l, QStringList());

		QVERIFY(p.registrationHeader().isEmpty());
		QVERIFY(p.registrationSource().isEmpty());
	}

	void checkRegistrationCodeWhenNoClassToRegisterButDependencies()
	{
		const FPH l = generateListOfHeaders(QStringList() << noClassToRegister_0 << noClassToRegister_1);
		const QStringList deps = QStringList() << "dep1" << "dep2";
		PluginRegistrationCodeGenerator p("plugin_main", "Dummy", l, deps);

		QCOMPARE(p.registrationHeader(), QString(noClassToRegisterButDeps_h));
		QCOMPARE(p.registrationSource(), QString(noClassToRegisterButDeps_cpp));
	}

	void checkRegistrationCodeWhenNoClassToRegisterButPrePostFunctions()
	{
		const FPH l = generateListOfHeaders(QStringList() << noClassToRegisterButPrePostFnc_0 << noClassToRegisterButPrePostFnc_1);
		const QStringList deps = QStringList() << "dep1" << "dep2";
		PluginRegistrationCodeGenerator p("plugin_main", "Dummy", l, deps);

		QCOMPARE(p.registrationHeader(), QString(noClassToRegisterButPrePostFnc_h));
		QCOMPARE(p.registrationSource(), QString(noClassToRegisterButPrePostFnc_cpp));
	}

	void checkRegistrationCodeWhenSingleClassToRegister()
	{
		const FPH l = generateListOfHeaders(QStringList() << singleClassToRegisterWithParents);
		PluginRegistrationCodeGenerator p("plugin_main", "Dummy", l, QStringList());

		QCOMPARE(p.registrationHeader(), QString(singleClassToRegisterWithParents_h));
		// singleClassToRegisterWithParents_cpp0 and singleClassToRegisterWithParents_cpp1 are both
		// fine, what changes is the order of the parents but that doesn't matter
		if ((p.registrationSource() != QString(singleClassToRegisterWithParents_cpp0)) &&
		    (p.registrationSource() != QString(singleClassToRegisterWithParents_cpp1))) {
			qDebug() << "WRONG REGISTRATION SOURCE:" << p.registrationSource();
			QVERIFY(false);
		}
	}

	void checkRegistrationCodeWhenClassesWithDependencies()
	{
		const FPH l = generateListOfHeaders(QStringList() << classesToRegisterWithDependencies_0 << classesToRegisterWithDependencies_1);
		PluginRegistrationCodeGenerator p("plugin_main", "Dummy", l, QStringList());

		QCOMPARE(p.registrationHeader(), QString(classesToRegisterWithDependencies_h));
		// classesToRegisterWithDependencies_cpp0 and classesToRegisterWithDependencies_cpp1 are both
		// fine, what changes is the order of the parents but that doesn't matter
		if ((p.registrationSource() != QString(classesToRegisterWithDependencies_cpp0)) &&
		    (p.registrationSource() != QString(classesToRegisterWithDependencies_cpp1))) {
			qDebug() << "WRONG REGISTRATION SOURCE:" << p.registrationSource();
			QVERIFY(false);
		}
	}

private:
	QList<PluginRegistrationCodeGenerator::FilenameAndParsedHeader> generateListOfHeaders(QStringList headers) const
	{
		QList<PluginRegistrationCodeGenerator::FilenameAndParsedHeader> l;

		for (int i = 0; i < headers.size(); ++i) {
			const QString headerFilename = "header" + QString::number(i) + ".h";
			l.append(PluginRegistrationCodeGenerator::FilenameAndParsedHeader(headerFilename, HeaderParser(headers[i])));
		}

		return l;
	}
};

QTEST_MAIN(PluginRegistrationCodeGenerator_Test)
#include "pluginregistrationcodegenerator_test.moc"
