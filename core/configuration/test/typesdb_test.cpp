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
//

using namespace farsa;
using namespace TypesDBTestHierarchy;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class TypesDB_Test : public QObject
{
	Q_OBJECT

private slots:
	void registerInstantiableComponentConfiguringInConstructor()
	{
		TypesDB::instance().registerType<InstantiableComponentConfigInConstructor>("InstantiableComponentConfigInConstructor", QStringList() << "Component");

		QVERIFY(TypesDB::instance().isTypeRegistered("InstantiableComponentConfigInConstructor"));
		QVERIFY(TypesDB::instance().typeInfo("InstantiableComponentConfigInConstructor").canBeCreated);
		QVERIFY(TypesDB::instance().typeInfo("InstantiableComponentConfigInConstructor").configuresInConstructor);
	}

	void registerInstantiableComponentConfiguringInFunction()
	{
		TypesDB::instance().registerType<InstantiableComponentConfigInFunction>("InstantiableComponentConfigInFunction", QStringList() << "Component");

		QVERIFY(TypesDB::instance().isTypeRegistered("InstantiableComponentConfigInFunction"));
		QVERIFY(TypesDB::instance().typeInfo("InstantiableComponentConfigInFunction").canBeCreated);
		QVERIFY(!TypesDB::instance().typeInfo("InstantiableComponentConfigInFunction").configuresInConstructor);
	}

	void registerAbstractComponent()
	{
		TypesDB::instance().registerType<AbstractComponent>("AbstractComponent", QStringList() << "Component");

		QVERIFY(TypesDB::instance().isTypeRegistered("AbstractComponent"));
		QVERIFY(!TypesDB::instance().typeInfo("AbstractComponent").canBeCreated);
	}

	void registerComponentWithCustomCreator()
	{
		TypesDB::instance().registerType<ComponentWithCustomCreator, ComponentCustomCreator>("ComponentWithCustomCreator", QStringList() << "Component");

		QVERIFY(TypesDB::instance().isTypeRegistered("ComponentWithCustomCreator"));
		QVERIFY(TypesDB::instance().typeInfo("ComponentWithCustomCreator").canBeCreated);
		QVERIFY(TypesDB::instance().typeInfo("ComponentWithCustomCreator").configuresInConstructor);
	}

	void getComponentSubclasses()
	{
		TypesDB::instance().registerType<SampleHierarchyRoot>("SampleHierarchyRoot", QStringList() << "Component");
		TypesDB::instance().registerType<SampleHierarchyChildOne>("SampleHierarchyChildOne", QStringList() << "SampleHierarchyRoot");
		TypesDB::instance().registerType<SampleHierarchyGrandChild>("SampleHierarchyGrandChild", QStringList() << "SampleHierarchyChildOne");
		TypesDB::instance().registerType<SampleHierarchyChildTwo>("SampleHierarchyChildTwo", QStringList() << "SampleHierarchyRoot");
		TypesDB::instance().registerType<SampleHierarchyAbstractChild>("SampleHierarchyAbstractChild", QStringList() << "SampleHierarchyRoot");

		const QSet<QString> allChildren = QSet<QString>() << "SampleHierarchyChildOne" << "SampleHierarchyGrandChild" << "SampleHierarchyChildTwo" << "SampleHierarchyAbstractChild";
		const QSet<QString> allDirectChildren = QSet<QString>() << "SampleHierarchyChildOne" << "SampleHierarchyChildTwo" << "SampleHierarchyAbstractChild";
		const QSet<QString> allConcrecteChildren = QSet<QString>() << "SampleHierarchyChildOne" << "SampleHierarchyGrandChild" << "SampleHierarchyChildTwo";

		QCOMPARE(QSet<QString>::fromList(TypesDB::instance().getAllTypeSubclasses("SampleHierarchyRoot")), allChildren);
		QCOMPARE(QSet<QString>::fromList(TypesDB::instance().getAllTypeSubclasses("SampleHierarchyRoot", 1)), allDirectChildren);
		QCOMPARE(QSet<QString>::fromList(TypesDB::instance().getAllTypeSubclasses("SampleHierarchyRoot", -1, true)), allConcrecteChildren);
	}
};

QTEST_MAIN(TypesDB_Test)
#include "typesdb_test.moc"
