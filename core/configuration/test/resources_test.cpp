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
#include "testhierarchy.h"
#include <QtTest/QtTest>
#include <memory>

// NOTES AND TODOS
//
// Implement and test ResourceHolder (think if it is easy to do or not: if it
// 	isn't, leave for later and modify the description of Component removing
// 	references to this class)
// Would it be possible to make the resource mechanism const-aware? At the
// 	moment most of the resource-related functions are non-const...
// Move all resource notification code to ResourceChangeNotifee from Component

using namespace salsa;
using namespace ResourcesTestHierarchy;

/**
 * \brief The number of parallel tests
 */
const int numParallelTests = 50;

/**
 * \brief How many tests each thread should do
 */
const int testsPerThread = 10;

/**
 * \brief The maximum timeout to wait for threads
 */
const int maxTimeoutInMilliseconds = 5000;

// Configuration files used in the test are declared here in ini format

const char* standaloneConfigurationFile = "\
[root]\n\
type = Standalone\n\
";

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
[one/two/three/subThree]\n\
type = SubThree\n\
\n\
[one/another]\n\
type = Another\n\
\n\
[one/another/three]\n\
type = Three\n\
\n\
[one/another/three/subThree]\n\
type = SubThree\n\
\n\
[one/another/subAnother]\n\
type = SubAnother\n\
";

const char* hierarchyWithResourcesInConfigurationPhaseConfigurationFile = "\
[one]\n\
type = RCOne\n\
\n\
[one/two]\n\
type = RCTwo\n\
\n\
[one/two/three]\n\
type = RCThree\n\
\n\
[one/two/three/subThree]\n\
type = RCSubThree\n\
\n\
[one/another]\n\
type = RCAnother\n\
\n\
[one/another/three]\n\
type = RCThree\n\
\n\
[one/another/three/subThree]\n\
type = RCSubThree\n\
\n\
[one/another/subAnother]\n\
type = RCSubAnother\n\
";

const char* hierarchyWithGetResourceInComponentCreator = "\
[First]\n\
type = RCCFirst\n\
\n\
[First/Second]\n\
type = RCCSecond\n\
";

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class Resources_Test : public QObject
{
	Q_OBJECT

	// This is to be sure to destroy all components
	class ManagerRAII
	{
	public:
		ManagerRAII(ConfigurationManager& manager)
			: m_manager(manager)
		{
		}

		~ManagerRAII()
		{
			m_manager.destroyAllComponents();
		}
	private:
		ConfigurationManager& m_manager;
	};

	// An operation for a parallel test
	class ParallelResourcesAccessWithSameNameInDifferentSubtreesOperation : public SingleOperation<ConfigurationManager>
	{
	public:
		virtual void runSingle(ConfigurationManager& object, int index)
		{
			const QString groupName = "one/two:" + QString::number(index / numIndexesPerRunner());
			Two* myTwo = object.getComponentFromGroup<Two>(groupName);

			const double initialDValue = 17.13 * index;
			double d = initialDValue;
			bool b = true;

			myTwo->three()->declareResource("d", &d);
			myTwo->three()->subThree()->declareResource("b", &b);
			QCOMPARE(myTwo->getResource<double>("d"), &d);
			QCOMPARE(*(myTwo->getResource<double>("d")), initialDValue);
			QCOMPARE(myTwo->getResource<bool>("b"), &b);
			const double modifiedDValue = 100.0 + index;
			*(myTwo->getResource<double>("d")) = modifiedDValue;
			QCOMPARE(*(myTwo->three()->getResource<double>("d")), modifiedDValue);
		}
	};

private slots:
	// Before performing any test, we need to register all components
	void initTestCase()
	{
		registerAllComponents();

		m_rResource = NULL;
		m_cResource = NULL;
		m_qResource = NULL;

		// Creating the dummy objects to use as resources
		class DummyResource : public Resource
		{
		};
		m_rResource = new DummyResource;

		// Using the Standalone component. The component survives the destruction of the
		// ConfigurationManager because it internally has its own ConfigurationManager
		ConfigurationManager m;
		m.createGroup("dummy");
		m.createParameter("dummy", "type", "Standalone");
		m_cResource = m.getComponentFromGroup<Component>("dummy");

		m_qResource = new QObject(NULL);
	}

	void noResources()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));

		QCOMPARE(s->resourcesCount("dummy"), 0);
		QCOMPARE(s->resourcesCount<int>("dummy"), 0);
		QCOMPARE(s->resourcesCount<Component>("dummy"), 0);
	}

	void declareResourceAndCount()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));

		int i = 0;
		float f = 0.0f;
		double d = 0.0;
		bool b = true;

		s->declareResource("i", &i);
		s->declareResource("f", &f);
		s->declareResource("d", &d);
		s->declareResource("b", &b);
		s->declareResource("r", m_rResource);
		s->declareResource("c", m_cResource);
		s->declareResource("q", m_qResource);

		QCOMPARE(s->resourcesCount("i"), 1);
		QCOMPARE(s->resourcesCount<int>("i"), 1);
		QCOMPARE(s->resourcesCount<Component>("i"), 0);
		QCOMPARE(s->resourcesCount("f"), 1);
		QCOMPARE(s->resourcesCount<float>("f"), 1);
		QCOMPARE(s->resourcesCount<double>("f"), 0);
		QCOMPARE(s->resourcesCount("d"), 1);
		QCOMPARE(s->resourcesCount<double>("d"), 1);
		QCOMPARE(s->resourcesCount<bool>("d"), 0);
		QCOMPARE(s->resourcesCount("b"), 1);
		QCOMPARE(s->resourcesCount<bool>("b"), 1);
		QCOMPARE(s->resourcesCount<Resource>("b"), 0);
		QCOMPARE(s->resourcesCount("r"), 1);
		QCOMPARE(s->resourcesCount<Resource>("r"), 1);
		QCOMPARE(s->resourcesCount<int>("r"), 0);
		QCOMPARE(s->resourcesCount("c"), 1);
		QCOMPARE(s->resourcesCount<Component>("c"), 1);
		QCOMPARE(s->resourcesCount<QObject>("c"), 0);
		QCOMPARE(s->resourcesCount("q"), 1);
		QCOMPARE(s->resourcesCount<QObject>("q"), 1);
		QCOMPARE(s->resourcesCount<float>("q"), 0);
	}

	void getResourceSimpleScenario()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));

		int i = 0;
		float f = 0.0f;
		double d = 0.0;
		bool b = true;

		s->declareResource("i", &i);
		s->declareResource("f", &f);
		s->declareResource("d", &d);
		s->declareResource("b", &b);
		s->declareResource("r", m_rResource);
		s->declareResource("c", m_cResource);
		s->declareResource("q", m_qResource);

		QCOMPARE(s->getResource<int>("i"), &i);
		QCOMPARE(s->getResource<float>("f"), &f);
		QCOMPARE(s->getResource<double>("d"), &d);
		QCOMPARE(s->getResource<bool>("b"), &b);
		QCOMPARE(s->getResource<Resource>("r"), m_rResource);
		QCOMPARE(s->getResource<Component>("c"), m_cResource);
		QCOMPARE(s->getResource<QObject>("q"), m_qResource);
		QVERIFY_EXCEPTION_THROWN(s->getResource<QObject>("dummy"), ResourceNotDeclaredException);
		QVERIFY_EXCEPTION_THROWN(s->getResource<QObject>("i"), ResourceNotDeclaredException);
	}

	void getResourceDifferentVersions()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);
		ConfigurationManager manager2 = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));
		std::auto_ptr<Component> s2(manager2.getComponentFromGroup<Component>("root"));

		int i = 0;

		s->declareResource("i", &i);

		QCOMPARE(s->getResource<int>("i"), &i);

		QList<Component::ResourceAndOwner<int> > resAndOwnerList = s->getAllCandidateResources<int>("i");
		QCOMPARE(resAndOwnerList.size(), 1);
		QCOMPARE(resAndOwnerList[0].resource, &i);
		QCOMPARE(resAndOwnerList[0].owner, s.get());
		QVERIFY_EXCEPTION_THROWN(s->getAllCandidateResources<int>("dummy"), ResourceNotDeclaredException);

		QCOMPARE(s->getResource<int>("i", s.get()), &i);
		QVERIFY_EXCEPTION_THROWN(s->getResource<int>("i", s2.get()), ResourceNotDeclaredException);

		resAndOwnerList = s->getAllResources<int>("i");
		QCOMPARE(resAndOwnerList.size(), 1);
		QCOMPARE(resAndOwnerList[0].resource, &i);
		QCOMPARE(resAndOwnerList[0].owner, s.get());
		resAndOwnerList = s->getAllResources<int>("dummy");
		QCOMPARE(resAndOwnerList.size(), 0);

		QList<Component*> owners = s->getResourcesOwners("i");
		QCOMPARE(owners.size(), 1);
		QCOMPARE(owners[0], s.get());
		owners = s->getResourcesOwners("dummy");
		QCOMPARE(owners.size(), 0);

		owners = s->getResourcesOwners<int>("i");
		QCOMPARE(owners.size(), 1);
		QCOMPARE(owners[0], s.get());
		owners = s->getResourcesOwners<float>("i");
		QCOMPARE(owners.size(), 0);
	}

	void declareResourceAndCheckExistance()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));

		int i = 0;
		float f = 0.0f;
		double d = 0.0;
		bool b = true;

		s->declareResource("i", &i);
		s->declareResource("f", &f);
		s->declareResource("d", &d);
		s->declareResource("b", &b);
		s->declareResource("r", m_rResource);
		s->declareResource("c", m_cResource);
		s->declareResource("q", m_qResource);

		QVERIFY(s->resourceExists("i"));
		QVERIFY(s->resourceExists<int>("i"));
		QVERIFY(s->resourceExists("i", s.get()));
		QVERIFY(s->resourceExists<int>("i", s.get()));
		QVERIFY(s->resourceExists("f"));
		QVERIFY(s->resourceExists<float>("f"));
		QVERIFY(s->resourceExists("f", s.get()));
		QVERIFY(s->resourceExists<float>("f", s.get()));
		QVERIFY(s->resourceExists("d"));
		QVERIFY(s->resourceExists<double>("d"));
		QVERIFY(s->resourceExists("d", s.get()));
		QVERIFY(s->resourceExists<double>("d", s.get()));
		QVERIFY(s->resourceExists("b"));
		QVERIFY(s->resourceExists<bool>("b"));
		QVERIFY(s->resourceExists("b", s.get()));
		QVERIFY(s->resourceExists<bool>("b", s.get()));
		QVERIFY(s->resourceExists("r"));
		QVERIFY(s->resourceExists<Resource>("r"));
		QVERIFY(s->resourceExists("r", s.get()));
		QVERIFY(s->resourceExists<Resource>("r", s.get()));
		QVERIFY(s->resourceExists("c"));
		QVERIFY(s->resourceExists<Component>("c"));
		QVERIFY(s->resourceExists("c", s.get()));
		QVERIFY(s->resourceExists<Component>("c", s.get()));
		QVERIFY(s->resourceExists("q"));
		QVERIFY(s->resourceExists<QObject>("q"));
		QVERIFY(s->resourceExists("q", s.get()));
		QVERIFY(s->resourceExists<QObject>("q", s.get()));

		QVERIFY(!s->resourceExists("dummy"));
		QVERIFY(!s->resourceExists<double>("i"));
		QVERIFY(!s->resourceExists("dummy", s.get()));
		QVERIFY(!s->resourceExists<float>("i", s.get()));
	}

	void declareResourceAsNull()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));

		int i = 0;

		s->declareResource("i", &i);

		s->declareResourceAsNull("i");

		QVERIFY(s->getResource<int>("i") == NULL);
		QVERIFY(s->getResource<double>("i") == NULL);
		QVERIFY(s->getResource<Resource>("i") == NULL);
		QVERIFY(s->resourceExists("i", s.get()));
	}

	void deleteResource()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneConfigurationFile);

		std::auto_ptr<Component> s(manager.getComponentFromGroup<Component>("root"));

		int i = 0;

		s->declareResource("i", &i);

		s->deleteResource("i");

		QVERIFY(!s->resourceExists("i"));
		QVERIFY_EXCEPTION_THROWN(s->getResource<int>("i"), ResourceNotDeclaredException);
	}

	void checkResourcesWithComponentHierarchy()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		// The hierarchy of components is the following one:
		// one
		// 	another
		// 		three
		// 			subtree
		// 		subAnother
		// 	two
		// 		three
		// 			subThree

		// Resources
		int i0 = 17;
		int i1 = 13;
		float i2 = 5.10;

		one->another()->three()->declareResource("i", &i0);
		QCOMPARE(one->another()->getResource<int>("i"), &i0);
		one->two()->three()->declareResource("i", &i1);
		QCOMPARE(one->another()->getResource<int>("i"), &i0);
		QCOMPARE(one->another()->subAnother()->getResource<int>("i"), &i0);
		QCOMPARE(one->two()->getResource<int>("i"), &i1);
		QVERIFY_EXCEPTION_THROWN(one->getResource<int>("i"), ResourceCannotResolveAmbiguityException);
		QCOMPARE(one->getResource<int>("i", one->another()->three()), &i0);
		QVERIFY_EXCEPTION_THROWN(one->getResource<int>("i", one->another()), ResourceNotDeclaredException);
		QList<Component*> owners = one->getResourcesOwners("i");
		QCOMPARE(owners.size(), 2);
		QVERIFY(owners.contains(one->another()->three()));
		QVERIFY(owners.contains(one->two()->three()));
		one->another()->subAnother()->declareResource("i", &i2);
		QCOMPARE(one->getResource<float>("i"), &i2);
		owners = one->two()->three()->subThree()->getResourcesOwners("i");
		QCOMPARE(owners.size(), 3);
		QVERIFY(owners.contains(one->another()->three()));
		QVERIFY(owners.contains(one->another()->subAnother()));
		QVERIFY(owners.contains(one->two()->three()));
	}

	void parallelResourcesAccessWithSameNameInDifferentSubtrees()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		// Duplicating the "two" subgroup, so that each thread has its own
		for (int i = 0; i < numParallelTests; ++i) {
			manager.copyGroup("one/two", "one/two:" + QString::number(i));
		}

		// Starting parallel tests
		QVERIFY(executeParallelRunners<ParallelResourcesAccessWithSameNameInDifferentSubtreesOperation>(manager, numParallelTests, testsPerThread, maxTimeoutInMilliseconds));
	}

	void componentsDeclaredAsTheirOwnResources()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		QCOMPARE(one->another()->getResource<Component>("one"), one);
		QCOMPARE(one->getResource<Component>("another"), one->another());
		QCOMPARE(one->another()->getResource<Component>("three"), one->another()->three());
		QCOMPARE(one->another()->getResource<Component>("subThree"), one->another()->three()->subThree());
		QCOMPARE(one->getResource<Component>("subAnother"), one->another()->subAnother());
		QCOMPARE(one->another()->three()->getResource<Component>("two"), one->two());
		QCOMPARE(one->two()->getResource<Component>("three"), one->two()->three());
		QCOMPARE(one->two()->getResource<Component>("subThree"), one->two()->three()->subThree());
	}

	void simpleSelfNotificationDuringLifetime()
	{
		// Here a component asks notifications about his own resources. This works but there
		// is a small "problem" during destruction (see below)
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one, ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one, ResourceModified));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one, ResourceDeleted));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni2", one, ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni2", one, ResourceDeclaredAsNull));
		expected.callEnd();
		// The following call does not happend because we delete one, so its destructor deletes the resource
		// but by that time the class One has already been delete and its vtable removed, so the resourceChanged
		// of ResourceChangeNotifee is called, not the one of class One. This however should not create real
		// problems because a component is missing the notification about the destruction of a resource he owns when
		// he is destroyed
		// expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni2", one, ResourceDeleted));
		// expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		int i0 = 10;
		int i1 = 17;

		one->declareResource("ni", &i0);
		one->addNotifiedResource("ni");
		one->declareResource("ni", &i1);
		one->deleteResource("ni");

		QVERIFY_EXCEPTION_THROWN(one->addNotifiedResource("ni2"), ResourceNotDeclaredException);

		one->declareResource("ni2", &i0);
		one->addNotifiedResource("ni2");
		one->declareResourceAsNull("ni2");
		delete one;

		QCOMPARE(expected, actual);
	}

	void simpleNotificationDuringLifetime()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one->two(), ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one->two(), ResourceModified));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one->two(), ResourceDeleted));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni2", one->two(), ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni2", one->two(), ResourceDeclaredAsNull));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni2", one->two(), ResourceDeleted));
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		int i0 = 10;
		int i1 = 17;

		one->two()->declareResource("ni", &i0);
		one->addNotifiedResource("ni");
		one->two()->declareResource("ni", &i1);
		one->two()->deleteResource("ni");

		QVERIFY_EXCEPTION_THROWN(one->addNotifiedResource("ni2"), ResourceNotDeclaredException);

		one->two()->declareResource("ni2", &i0);
		one->addNotifiedResource("ni2");
		one->two()->declareResourceAsNull("ni2");
		delete one->two();

		QCOMPARE(expected, actual);
	}

	void notificationRegistrationBeforeResourceDeclarationDuringLifetime()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Another", "ni", one->two()->three(), ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Another", "ni", one->two()->three(), ResourceDeclaredAsNull));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Another", "ni", one->two()->three(), ResourceModified));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Another", "ni", one->two()->three(), ResourceDeleted));
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		int i = 10;

		one->another()->addNotifiedResource("ni", one->two()->three());

		// We should have received no notification at this moment
		QCOMPARE(actual.calls().subcalls.size(), 0);

		one->two()->three()->declareResource("ni", &i);
		one->two()->three()->declareResourceAsNull("ni");
		one->two()->three()->declareResource("ni", &i);
		one->two()->three()->deleteResource("ni");

		QCOMPARE(expected, actual);
	}

	void automaticNotificationRemovalOnComponentDestruction()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		one->another()->addNotifiedResource("ni", one->two()->three());

		int i = 13;

		one->two()->three()->declareResource("ni", &i);

		delete one->another();

		// This will crash if resources are not automatically deleted when
		// the component is destroyed
		one->two()->three()->declareResourceAsNull("ni");
	}

	void manualRemovalOfNotificationDuringLifetime()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		ManagerRAII managerRAII(manager);

		One* one = manager.getComponentFromGroup<One>("one");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one->another(), ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one->another(), ResourceDeclaredAsNull));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("One", "ni", one->another(), ResourceDeleted));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Two", "nf", one->another()->subAnother(), ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Two", "nf", one->another()->subAnother(), ResourceDeclaredAsNull));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("Two", "nf", one->another()->subAnother(), ResourceDeleted));
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		int i = 10;
		float f = 13.17;

		one->addNotifiedResource("ni", one->another());
		one->another()->declareResource("ni", &i);
		one->another()->declareResourceAsNull("ni");
		one->removeNotifiedResource("ni");
		one->another()->declareResource("ni", &i);

		one->two()->addNotifiedResource("nf", one->another()->subAnother());
		one->another()->subAnother()->declareResource("nf", &f);
		one->another()->subAnother()->declareResourceAsNull("nf");
		one->two()->removeNotifiedResource("nf");
		one->another()->subAnother()->declareResource("nf", &f);

		delete one->another()->subAnother();
		delete one->another();

		QCOMPARE(expected, actual);
	}

	void notificationRegistrationDuringConfigurationPhase()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyWithResourcesInConfigurationPhaseConfigurationFile);
		ManagerRAII managerRAII(manager);

		FunctionCallsSequenceRecorder actual;

		RCOne* one = manager.getComponentFromGroup<RCOne>("one");

		// Modifying one resource to test that notifications are actually registered
		one->two()->declareResourceAsNull("bTwo");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("RCOne::RCOne");
			expected.callBegin("RCAnother::RCAnother");
				expected.callBegin("RCThree::RCThree");
					expected.callBegin("RCSubThree::RCSubThree");
					expected.callEnd();
				expected.callEnd();
				expected.callBegin("RCSubAnother::RCSubAnother");
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("RCTwo::RCTwo");
				expected.callBegin("RCThree::RCThree");
					expected.callBegin("RCSubThree::RCSubThree");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("RCOne", "bTwo", one->two(), ResourceCreated));
		expected.callEnd();
		expected.callBegin(nameForNotifyResourceChangeWithParameters("RCOne", "bTwo", one->two(), ResourceDeclaredAsNull));
		expected.callEnd();

		QCOMPARE(expected, actual);
	}

	void notificationRegistrationDuringConfigurationPhaseWithException()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyWithResourcesInConfigurationPhaseConfigurationFile);
		ManagerRAII managerRAII(manager);

		manager.createParameter("one/another", "throwException");

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<RCOne>("one"), ResourceCannotResolveAmbiguityException);
	}

	void getResourceInComponentCreator()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyWithGetResourceInComponentCreator);
		ManagerRAII managerRAII(manager);

		RCCFirst* first = manager.getComponentFromGroup<RCCFirst>("First");

		QCOMPARE(first->second()->i(), 17);
	}

	void cleanupTestCase()
	{
		delete m_rResource;
		delete m_cResource;
		delete m_qResource;
	}

private:
	// Objects for resources test
	Resource* m_rResource;
	Component* m_cResource;
	QObject* m_qResource;
};

QTEST_MAIN(Resources_Test)
#include "resources_test.moc"
