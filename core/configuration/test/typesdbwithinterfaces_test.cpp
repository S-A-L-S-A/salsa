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
#include "typesdb.h"
#include "testhierarchy.h"

// NOTES AND TODOS
//
// Do more tests...

using namespace farsa;
using namespace TypesDBWithInterfacesTestHierarchy;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class TypesDBWithInterfaces_Test : public QObject
{
	Q_OBJECT

private slots:
	void conversionToComponentTemplate()
	{
		QVERIFY(__Factory_internal::isComponent<farsa::Component>::value);
		QVERIFY(!__Factory_internal::isComponent<InterfaceDummy>::value);
	}

	void registerInterface()
	{
		TypesDB::instance().registerType<InterfaceDummy>("InterfaceDummy", QStringList());

		QVERIFY(TypesDB::instance().isTypeRegistered("InterfaceDummy"));
		QVERIFY(!TypesDB::instance().typeInfo("InterfaceDummy").canBeCreated);
		QVERIFY(TypesDB::instance().typeInfo("InterfaceDummy").isInterface);
	}

	void registerComponentWithMultipleInterfaces()
	{
		TypesDB::instance().registerType<InterfaceA>("InterfaceA", QStringList());
		TypesDB::instance().registerType<InterfaceB>("InterfaceB", QStringList());
		TypesDB::instance().registerType<MultipleInterfacesComponent>("MultipleInterfacesComponent", QStringList() << "Component" << "InterfaceA" << "InterfaceB");

		QVERIFY(TypesDB::instance().isTypeRegistered("InterfaceA"));
		QVERIFY(TypesDB::instance().isTypeRegistered("InterfaceB"));
		QVERIFY(TypesDB::instance().isTypeRegistered("MultipleInterfacesComponent"));

		const QStringList childA = TypesDB::instance().getAllTypeSubclasses("InterfaceA");
		const QStringList childB = TypesDB::instance().getAllTypeSubclasses("InterfaceB");

		QCOMPARE(childA, QStringList() << "MultipleInterfacesComponent");
		QCOMPARE(childB, QStringList() << "MultipleInterfacesComponent");
	}

#warning DO MORE TESTS!!!
};

QTEST_MAIN(TypesDBWithInterfaces_Test)
#include "typesdbwithinterfaces_test.moc"
