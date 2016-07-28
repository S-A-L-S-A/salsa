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
#include "configurationobserver.h"
#include "typesdb.h"
#include "testutils.h"
#include "testhierarchy.h"
#include <QtTest/QtTest>

// NOTES AND TODOS
//
//

using namespace salsa;
using namespace ConfigurationObserverTestHierarchy;

// Configuration files used in the test are declared here in ini format

const char* hierarchyConfigurationFile = "\
[one]\n\
type = One\n\
\n\
[one/two]\n\
type = Two\n\
\n\
[one/two/three]\n\
type = Three\n\
\n\
[one/another]\n\
type = Another\n\
\n\
[one/another/three]\n\
type = Three\n\
";

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ConfigurationObserver_Test : public QObject
{
	Q_OBJECT

	static QString nameForComponentPointer(Component* component)
	{
		return QString::number(reinterpret_cast<unsigned long>(component));
	}

	class TestObserver : public salsa::ConfigurationObserver
	{
	public:
		virtual void onComponentCreation(Component* component)
		{
			FunctionCall call("TestObserver::onComponentCreation " + nameForComponentPointer(component));
		}

		virtual void onComponentDestruction(Component* component)
		{
			FunctionCall call("TestObserver::onComponentDestruction " + nameForComponentPointer(component));
		}
	};

private slots:
	// Before performing any test, we need to register all components
	void initTestCase()
	{
		registerAllComponents();
	}

	void notificationOfCreation()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);

		TestObserver observer;
		observer.observe(&manager);

		FunctionCallsSequenceRecorder actual;

		One* one = manager.getComponentFromGroup<One>("one");
		QVERIFY(one != NULL);

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("TestObserver::onComponentCreation " + nameForComponentPointer(one->another()->three()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentCreation " + nameForComponentPointer(one->another()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentCreation " + nameForComponentPointer(one->two()->three()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentCreation " + nameForComponentPointer(one->two()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentCreation " + nameForComponentPointer(one));
		expected.callEnd();

		QCOMPARE(expected, actual);
	}

	void notificationOfDestruction()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);

		TestObserver observer;
		observer.observe(&manager);

		One* one = manager.getComponentFromGroup<One>("one");
		QVERIFY(one != NULL);

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("TestObserver::onComponentDestruction " + nameForComponentPointer(one));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentDestruction " + nameForComponentPointer(one->another()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentDestruction " + nameForComponentPointer(one->another()->three()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentDestruction " + nameForComponentPointer(one->two()));
		expected.callEnd();
		expected.callBegin("TestObserver::onComponentDestruction " + nameForComponentPointer(one->two()->three()));
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;
		manager.destroyAllComponents();

		QCOMPARE(expected, actual);
	}

	void detachObserversWhenConfigurationManagerDestroyed()
	{
		TestObserver observer;

		{
			ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);

			observer.observe(&manager);

			One* one = manager.getComponentFromGroup<One>("one");
			QVERIFY(one != NULL);
			manager.destroyAllComponents();
		}

		QVERIFY(observer.observed() == NULL);
	}
};

QTEST_MAIN(ConfigurationObserver_Test)
#include "configurationobserver_test.moc"
