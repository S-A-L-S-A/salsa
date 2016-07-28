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
#include "testutils.h"
#include "testhierarchy.h"
#include <QtTest/QtTest>

// NOTES AND TODOS
//
// Check that all components that are not stored in auto_ptr in their parents
// 	are destroyed when exceptions are thrown. They probably aren't (see
// 	comment in getComponentFromGroup)

using namespace salsa;
using namespace ComponentInstantiationTestHierarchy;

// Configuration files used in the test are declared here in ini format

const char* invalidConfigurationFile = "\
[dummy]\n\
parameter = test\n\
";

const char* standaloneTypeConfigurationFile = "\
[group]\n\
type = Standalone\n\
param = 10\n\
";

const char* standaloneWithConfigureTypeConfigurationFile = "\
[group]\n\
type = StandaloneWithConfigure\n\
param = 100\n\
";

const char* hierarchyConfigurationFile = "\
[one]\n\
type = One\n\
p1 = 0\n\
p2 = 3\n\
\n\
[one/two:4]\n\
type = Two\n\
p_1 = 0_0\n\
\n\
[one/two:4/three]\n\
type = Three\n\
pithree = pigreco\n\
pithreeAA = pi^e\n\
\n\
[one/two:12]\n\
type = Two\n\
p_1 = 0_1\n\
\n\
[one/two:12/three]\n\
type = Three\n\
pithree = pi/2\n\
pithreeAA = pi*e\n\
\n\
[one/two:aa]\n\
type = Two\n\
p_1 = 0_2\n\
\n\
[one/two:aa/three]\n\
type = Three\n\
pithree = pi/4\n\
pithreeAA = pi/y\n\
\n\
[one/another]\n\
type = Another\n\
p_a = -1\n\
\n\
[one/another/three]\n\
type = Three\n\
pithree = pi/a\n\
pithreeAA = pi/e\n\
";

const char* correctCircularDependencyConfigurationFile = "\
[Root]\n\
type = CorrectCircularDependencyRoot\n\
\n\
[A]\n\
type = CorrectCircularDependencyA\n\
\n\
[B]\n\
type = CorrectCircularDependencyB\n\
";

const char* constructorCircularDependencyConfigurationFile = "\
[Root]\n\
type = ConstructorCircularDependencyRoot\n\
\n\
[A]\n\
type = ConstructorCircularDependencyA\n\
\n\
[B]\n\
type = ConstructorCircularDependencyB\n\
";

const char* wrongCircularDependencyConfigurationFile = "\
[Root]\n\
type = WrongCircularDependencyRoot\n\
\n\
[A]\n\
type = WrongCircularDependencyA\n\
\n\
[B]\n\
type = WrongCircularDependencyB\n\
";

const char* hierarchyForDestructionConfigurationFile = "\
[one]\n\
type = DestructionOne\n\
p1 = 0\n\
p2 = 3\n\
\n\
[one/two:4]\n\
type = DestructionTwo\n\
p_1 = 0_0\n\
\n\
[one/two:4/three]\n\
type = DestructionThree\n\
pithree = pigreco\n\
pithreeAA = pi^e\n\
\n\
[one/two:12]\n\
type = DestructionTwo\n\
p_1 = 0_1\n\
\n\
[one/two:12/three]\n\
type = DestructionThree\n\
pithree = pi/2\n\
pithreeAA = pi*e\n\
\n\
[one/two:aa]\n\
type = DestructionTwo\n\
p_1 = 0_2\n\
\n\
[one/two:aa/three]\n\
type = DestructionThree\n\
pithree = pi/4\n\
pithreeAA = pi/y\n\
\n\
[one/another]\n\
type = DestructionAnother\n\
p_a = -1\n\
\n\
[one/another/three]\n\
type = DestructionThree\n\
pithree = pi/a\n\
pithreeAA = pi/e\n\
";

const char* componentFromParameterConfigurationFile = "\
[root]\n\
mycomponent = /group\n\
\n\
[group]\n\
type = Standalone\n\
param = 10\n\
";

const char* componentFromParameterRelativeConfigurationFile = "\
[root]\n\
mycomponent = ../group\n\
\n\
[group]\n\
type = Standalone\n\
param = 10\n\
";

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ComponentCreation_Test : public QObject
{
	Q_OBJECT

private slots:
	// Before performing any test, we need to register all components
	void initTestCase()
	{
		registerAllComponents();
	}

	void expectedGroupSeparatorAndParentGroup()
	{
		QCOMPARE(GroupSeparator, "/");
		QCOMPARE(ParentGroup, "..");
	}

	void invalidCalls()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(invalidConfigurationFile);

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("group"), NonExistentGroupNameException);
		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("dummy"), NonExistentParameterException);
	}

	void createStandaloneComponent()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneTypeConfigurationFile);

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("Standalone::Standalone");
		expected.callEnd();
		expected.callBegin("Standalone::postConfigureInitialization");
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		std::auto_ptr<Component> c(manager.getComponentFromGroup<Component>("group"));
		QVERIFY(c.get() != NULL);
		QCOMPARE(c->typeName(), QString("Standalone"));
		QCOMPARE(c->confPath(), QString("group/"));
		QCOMPARE(expected, actual);
	}

	void createStandaloneComponentThatThrows()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneTypeConfigurationFile);
		manager.createParameter("group", "throwException");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("Standalone::Standalone");
		expected.callEnd();
		// Destructor is not called because there was an exception before the constructor had ended

		FunctionCallsSequenceRecorder actual;

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("group"), UserDefinedCheckFailureException);
		QCOMPARE(expected, actual);
	}

	void createStandaloneWithConfigureComponent()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneWithConfigureTypeConfigurationFile);

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("StandaloneWithConfigure::StandaloneWithConfigure");
		expected.callEnd();
		expected.callBegin("StandaloneWithConfigure::configure");
		expected.callEnd();
		expected.callBegin("StandaloneWithConfigure::postConfigureInitialization");
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		std::auto_ptr<Component> c(manager.getComponentFromGroup<Component>("group"));
		QVERIFY(c.get() != NULL);
		QCOMPARE(c->typeName(), QString("StandaloneWithConfigure"));
		QCOMPARE(c->confPath(), QString("group/"));
		QCOMPARE(expected, actual);
	}

	void createStandaloneWithConfigureComponentThatThrows()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(standaloneWithConfigureTypeConfigurationFile);
		manager.createParameter("group", "throwException");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("StandaloneWithConfigure::StandaloneWithConfigure");
		expected.callEnd();
		expected.callBegin("StandaloneWithConfigure::configure");
		expected.callEnd();
		expected.callBegin("StandaloneWithConfigure::~StandaloneWithConfigure");
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("group"), UserDefinedCheckFailureException);
		QCOMPARE(expected, actual);
	}

	void createHierarchy()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("One::One");
			expected.callBegin("AbstractAnother::AbstractAnother");
			expected.callEnd();
			expected.callBegin("Another::Another");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Another::postConfigureInitialization");
			expected.callBegin("AbstractAnother::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Two::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Two::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Two::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("One::postConfigureInitialization");
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		std::auto_ptr<Component> c(manager.getComponentFromGroup<Component>("one"));
		QVERIFY(c.get() != NULL);
		QCOMPARE(expected, actual);
	}

	void createHierarchyThatThrows()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		manager.createParameter("one/two:12", "throwException");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("One::One");
			expected.callBegin("AbstractAnother::AbstractAnother");
			expected.callEnd();
			expected.callBegin("Another::Another");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Three::~Three");
			expected.callEnd();
			expected.callBegin("AbstractThree::~AbstractThree");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Two::~Two");
		expected.callEnd();
		expected.callBegin("Three::~Three");
		expected.callEnd();
		expected.callBegin("AbstractThree::~AbstractThree");
		expected.callEnd();
		expected.callBegin("Another::~Another");
		expected.callEnd();
		expected.callBegin("Three::~Three");
		expected.callEnd();
		expected.callBegin("AbstractThree::~AbstractThree");
		expected.callEnd();
		expected.callBegin("AbstractAnother::~AbstractAnother");
		expected.callEnd();
		// Destructor of One is not called because the exception was
		// thrown during the One constructor

		FunctionCallsSequenceRecorder actual;

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("one"), UserDefinedCheckFailureException);
		QCOMPARE(expected, actual);
	}

	void createHierarchyThatThrowsInPostConfigure()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);
		manager.createParameter("one/two:12", "throwExceptionInPostConfigure");

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("One::One");
			expected.callBegin("AbstractAnother::AbstractAnother");
			expected.callEnd();
			expected.callBegin("Another::Another");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("Two::Two");
				expected.callBegin("AbstractThree::AbstractThree");
				expected.callEnd();
				expected.callBegin("Three::Three");
				expected.callEnd();
				expected.callBegin("Three::configure");
					expected.callBegin("AbstractThree::configure");
					expected.callEnd();
				expected.callEnd();
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Another::postConfigureInitialization");
			expected.callBegin("AbstractAnother::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Two::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("Three::postConfigureInitialization");
			expected.callBegin("AbstractThree::postConfigureInitialization");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("Two::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("One::~One");
		expected.callEnd();
		expected.callBegin("Two::~Two");
		expected.callEnd();
		expected.callBegin("Three::~Three");
		expected.callEnd();
		expected.callBegin("AbstractThree::~AbstractThree");
		expected.callEnd();
		expected.callBegin("Two::~Two");
		expected.callEnd();
		expected.callBegin("Three::~Three");
		expected.callEnd();
		expected.callBegin("AbstractThree::~AbstractThree");
		expected.callEnd();
		expected.callBegin("Two::~Two");
		expected.callEnd();
		expected.callBegin("Three::~Three");
		expected.callEnd();
		expected.callBegin("AbstractThree::~AbstractThree");
		expected.callEnd();
		expected.callBegin("Another::~Another");
		expected.callEnd();
		expected.callBegin("Three::~Three");
		expected.callEnd();
		expected.callBegin("AbstractThree::~AbstractThree");
		expected.callEnd();
		expected.callBegin("AbstractAnother::~AbstractAnother");
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("one"), UserDefinedCheckFailureException);
		QCOMPARE(expected, actual);
	}

	void createCircularlyDependentComponents()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(correctCircularDependencyConfigurationFile);

		FunctionCallsSequenceRecorder expected;
		expected.callBegin("CorrectCircularDependencyRoot::CorrectCircularDependencyRoot");
		expected.callEnd();
		expected.callBegin("CorrectCircularDependencyRoot::configure");
			expected.callBegin("CorrectCircularDependencyA::CorrectCircularDependencyA");
			expected.callEnd();
			expected.callBegin("CorrectCircularDependencyA::configure");
				expected.callBegin("CorrectCircularDependencyB::CorrectCircularDependencyB");
				expected.callEnd();
			expected.callEnd();
			expected.callBegin("CorrectCircularDependencyB::configure");
			expected.callEnd();
		expected.callEnd();
		expected.callBegin("CorrectCircularDependencyA::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("CorrectCircularDependencyB::postConfigureInitialization");
		expected.callEnd();
		expected.callBegin("CorrectCircularDependencyRoot::postConfigureInitialization");
		expected.callEnd();

		FunctionCallsSequenceRecorder actual;

		std::auto_ptr<Component> c(manager.getComponentFromGroup<Component>("Root"));
		QVERIFY(c.get() != NULL);
		QCOMPARE(expected, actual);
	}

	void createConstructorCircularlyDependentComponents()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(constructorCircularDependencyConfigurationFile);

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("Root"), CyclicDependencyException);
	}

	void createWrongCircularlyDependentComponents()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(wrongCircularDependencyConfigurationFile);

		QVERIFY_EXCEPTION_THROWN(manager.getComponentFromGroup<Component>("Root"), CyclicDependencyException);
	}

	void checkComponentsDestruction()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyForDestructionConfigurationFile);

		Component* c = manager.getComponentFromGroup<Component>("one");
		QVERIFY(c != NULL);

		QMap<QString, int> expectedDestructionMap;
		expectedDestructionMap["DestructionOne::~DestructionOne"] = 1;
		expectedDestructionMap["DestructionAnother::~DestructionAnother"] = 1;
		expectedDestructionMap["DestructionThree::~DestructionThree"] = 4;
		expectedDestructionMap["DestructionAbstractThree::~DestructionAbstractThree"] = 4;
		expectedDestructionMap["DestructionAbstractAnother::~DestructionAbstractAnother"] = 1;
		expectedDestructionMap["DestructionTwo::~DestructionTwo"] = 3;

		FunctionCallsSequenceRecorder actualDestruction;

		manager.destroyAllComponents();

		QCOMPARE(expectedDestructionMap, actualDestruction.functionsMap());
	}

	void componentFromParameter()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(componentFromParameterConfigurationFile);

		Standalone* c = manager.getComponentFromParameter<Standalone>("root/mycomponent");

		QVERIFY(c != NULL);
		manager.destroyAllComponents();
	}

	void componentFromParameterRelative()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(componentFromParameterRelativeConfigurationFile);

		Standalone* c = manager.getComponentFromParameter<Standalone>("root/mycomponent");

		QVERIFY(c != NULL);
		manager.destroyAllComponents();
	}

	void verifyGetComponentFromGroupAlwaysReturnsTheSameComponenets()
	{
		ConfigurationManager manager = fillTemporaryConfigurationFileAndLoadParameters(hierarchyConfigurationFile);

		std::auto_ptr<One> c(manager.getComponentFromGroup<One>("one"));
		QVERIFY(c.get() != NULL);
		QCOMPARE(c.get(), manager.getComponentFromGroup<Component>("one"));
		QCOMPARE(c->two0(), manager.getComponentFromGroup<Component>("one/two:4"));
		QCOMPARE(c->two0()->three(), manager.getComponentFromGroup<Component>("one/two:4/three"));
		QCOMPARE(c->two1(), manager.getComponentFromGroup<Component>("one/two:12"));
		QCOMPARE(c->two1()->three(), manager.getComponentFromGroup<Component>("one/two:12/three"));
		QCOMPARE(c->two2(), manager.getComponentFromGroup<Component>("one/two:aa"));
		QCOMPARE(c->two2()->three(), manager.getComponentFromGroup<Component>("one/two:aa/three"));
		QCOMPARE(c->another(), manager.getComponentFromGroup<Component>("one/another"));
		QCOMPARE(c->another()->three(), manager.getComponentFromGroup<Component>("one/another/three"));
	}
};

QTEST_MAIN(ComponentCreation_Test)
#include "componentcreation_test.moc"
