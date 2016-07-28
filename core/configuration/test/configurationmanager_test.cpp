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
#include "testutils.h"
#include <QtTest/QtTest>

// NOTES AND TODOS
//
//

using namespace salsa;

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

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test. Most of the tests are multithreaded, so we
 * declare child of class Operation (template parameter is ConfigurationManager)
 * and then use doMultiThreadedTest in test slots
 */
class ConfigurationManager_Test : public QObject
{
	Q_OBJECT

	typedef QSet<QString> StringSet;

	// Performs a multithreaded test. Creates a ConfigurationManager
	// instance and starts parallel tests. Also checks that the
	// executeParallelRunners function returns true. The parameters can be
	// used to override the default number of threads, test per threads and
	// the timeout
	template <class OperationType>
	void doMultiThreadedTest(int nt = numParallelTests, int tt = testsPerThread, int t = maxTimeoutInMilliseconds)
	{
		ConfigurationManager manager;
		// This will fail if tests take too long
		QVERIFY(executeParallelRunners<OperationType>(manager, nt, tt, t));
	}

	class SingleOperationWithConfigurationManagerAndPrefix : public SingleOperation<ConfigurationManager>
	{
	public:
		virtual void runSingle(ConfigurationManager& object, int index)
		{
			const QString prefix = QString::number(index) + "/";
			runSingleWithPrefix(object, prefix);
		}

		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix) = 0;
	};

	class createGroupOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one///two//three");
			manager.createSubGroup(prefix + "one/two", "fourInARow");

			QVERIFY(manager.groupExists(prefix + "one"));
			QVERIFY(manager.groupExists(prefix + "one/two"));
			QVERIFY(manager.groupExists(prefix + "one/two/three"));
			QVERIFY(manager.groupExists(prefix + "one/two/fourInARow"));
			QVERIFY_EXCEPTION_THROWN(manager.createGroup(".."), InvalidGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createGroup(""), InvalidGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createGroup("/"), InvalidGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createGroup("A/B/../C"), InvalidGroupNameException);
		}
	};

	class deleteGroupOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one///two//three");

			QVERIFY(manager.groupExists(prefix + "one/two/three"));

			manager.deleteGroup(prefix + "one/two");

			QVERIFY(manager.groupExists(prefix + "one"));
			QVERIFY(!manager.groupExists(prefix + "one/two"));
			QVERIFY(!manager.groupExists(prefix + "one/two/three"));

			QVERIFY_EXCEPTION_THROWN(manager.deleteGroup("A/B"), NonExistentGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.deleteGroup(""), NonExistentGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.deleteGroup(".."), NonExistentGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.deleteGroup("/"), NonExistentGroupNameException);
		}
	};

	class renameGroupOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one///two//three");

			QVERIFY(manager.groupExists(prefix + "one/two/three"));

			manager.renameGroup(prefix + "one/two", "dummy");

			QVERIFY(manager.groupExists(prefix + "one"));
			QVERIFY(manager.groupExists(prefix + "one/dummy"));
			QVERIFY(manager.groupExists(prefix + "one/dummy/three"));
			QVERIFY(!manager.groupExists(prefix + "one/two"));

			QVERIFY_EXCEPTION_THROWN(manager.renameGroup("one/ball", "round"), NonExistentGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.renameGroup(prefix + "one/dummy", "aaa/bbb"), InvalidGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.renameGroup(prefix + "one", ".."), InvalidGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.renameGroup(prefix + "one", "/"), InvalidGroupNameException);
		}
	};

	class getGroupsListOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			const StringSet groups = StringSet() << "one" << "two" << "timmy" << "tommy" << "jimmy" << "group:1" << "group:3" << "group:1ttt";
			const QRegExp re(".*immy");
			const StringSet filteredGroups = StringSet() << "timmy" << "jimmy";
			const QString groupPrefix = "group:";
			const StringSet prefixGroups = StringSet() << "group:1" << "group:3" << "group:1ttt";

			foreach(QString s, groups) {
				manager.createGroup(prefix + "root/" + s);
			}

			QCOMPARE(StringSet::fromList(manager.getGroupsList(prefix + "root")), groups);
			QCOMPARE(StringSet::fromList(manager.getGroupsWithPrefixList(prefix + "root", groupPrefix)), prefixGroups);
			QCOMPARE(StringSet::fromList(manager.getFilteredGroupsList(prefix + "root", re)), filteredGroups);
			QVERIFY_EXCEPTION_THROWN(manager.getGroupsList(prefix + "root/nonExisting"), NonExistentGroupNameException);
		}
	};

	class createParameterOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one///two");
			manager.createGroup(prefix + "one///another");

			manager.createParameter(prefix + "one", "param");
			// There can be a parameter with the same name of a group
			manager.createParameter(prefix + "one", "two");
			manager.createParameter(prefix + "one/two", "parameter");
			manager.createParameter(prefix + "one/two", "dummy");
			manager.createParameter(prefix + "one/another", "param");

			QVERIFY(manager.parameterExists(prefix + "one/param"));
			QVERIFY(manager.parameterExists(prefix + "one/two"));
			QVERIFY(manager.groupExists(prefix + "one/two"));
			QVERIFY(manager.parameterExists(prefix + "one/two/parameter"));
			QVERIFY(manager.parameterExists(prefix + "one/two/dummy"));
			QVERIFY(manager.parameterExists(prefix + "one/another/param"));
			QVERIFY_EXCEPTION_THROWN(manager.createParameter(prefix + "one", ".."), InvalidParameterNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createParameter(prefix + "one", ""), InvalidParameterNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createParameter(prefix + "one/two", "/"), InvalidParameterNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createParameter(prefix + "one/another", "A/B/../C"), InvalidParameterNameException);
			QVERIFY_EXCEPTION_THROWN(manager.createParameter(prefix + "dummy", "A"), NonExistentGroupNameException);
		}
	};

	class deleteParameterOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one/two");

			manager.createParameter(prefix + "one", "param");
			manager.createParameter(prefix + "one", "two");

			QVERIFY(manager.parameterExists(prefix + "one/param"));
			QVERIFY(manager.parameterExists(prefix + "one/two"));

			manager.deleteParameter(prefix + "one", "param");

			QVERIFY(!manager.parameterExists(prefix + "one/param"));
			QVERIFY(manager.parameterExists(prefix + "one/two"));

			manager.deleteParameter(prefix + "one", "two");

			QVERIFY(!manager.parameterExists(prefix + "one/param"));
			QVERIFY(!manager.parameterExists(prefix + "one/two"));
			QVERIFY(manager.groupExists(prefix + "one/two"));

			QVERIFY_EXCEPTION_THROWN(manager.deleteParameter("A", "B"), NonExistentGroupNameException);
			QVERIFY_EXCEPTION_THROWN(manager.deleteParameter(prefix + "one", "param"), NonExistentParameterException);
		}
	};

	class setAndGetValueOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one///two");
			manager.createGroup(prefix + "one///another");

			manager.createParameter(prefix + "one", "param");
			// There can be a parameter with the same name of a group
			manager.createParameter(prefix + "one", "two");
			manager.createParameter(prefix + "one/two", "parameter");
			manager.createParameter(prefix + "one/two", "dummy");

			manager.createParameter(prefix + "one/another", "param", "valueSetDirectly");

			manager.setValue(prefix + "one/param", "value1");
			manager.setValue(prefix + "one/two", "otherValue");
			manager.setValue(prefix + "one/two/parameter", "Timmy");
			manager.setValue(prefix + "one/two/dummy", "Jimmy");

			QCOMPARE(manager.getValue(prefix + "one/param"), QString("value1"));
			QCOMPARE(manager.getValue(prefix + "one/two"), QString("otherValue"));
			QCOMPARE(manager.getValue(prefix + "one/two/parameter"), QString("Timmy"));
			QCOMPARE(manager.getValue(prefix + "one/two/dummy"), QString("Jimmy"));
			QCOMPARE(manager.getValue(prefix + "one/another/param"), QString("valueSetDirectly"));
			QVERIFY_EXCEPTION_THROWN(manager.setValue(prefix + "one/non-exiting", "non-exiting"), NonExistentParameterException);
			QVERIFY_EXCEPTION_THROWN(manager.setValue(prefix + "dummy/param", "value"), NonExistentGroupNameException);
		}
	};

	class getValueAlsoMatchParentsOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one/two/three");
			manager.createGroup(prefix + "one/another");

			manager.createParameter(prefix + "one", "param", "valueInOne");
			manager.createParameter(prefix + "one/two", "param", "valueInTwo");
			manager.createParameter(prefix + "one/two/three", "dummy", "dummy");
			manager.createParameter(prefix + "one/another/", "param", "valueInAnother");
			manager.createParameter(prefix + "one/another/", "anotherParameter", "value");

			QCOMPARE(manager.getValueAlsoMatchParents(prefix + "one/two/three/param"), QString("valueInTwo"));
			QVERIFY_EXCEPTION_THROWN(manager.getValueAlsoMatchParents(prefix + "one/two/three/anotherParameter"), NonExistentParameterException);
		}
	};

	class getParametersListOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			const StringSet parameters = StringSet() << "one" << "two" << "timmy" << "tommy" << "jimmy" << "group:1" << "group:3" << "group:1ttt";
			const QRegExp re(".*immy");
			const StringSet filteredParameters = StringSet() << "timmy" << "jimmy";
			const QString parameterPrefix = "group:";
			const StringSet prefixParameters = StringSet() << "group:1" << "group:3" << "group:1ttt";

			manager.createGroup(prefix + "root");
			foreach(QString s, parameters) {
				manager.createParameter(prefix + "root/", s);
			}

			QCOMPARE(StringSet::fromList(manager.getParametersList(prefix + "root")), parameters);
			QCOMPARE(StringSet::fromList(manager.getParametersWithPrefixList(prefix + "root", parameterPrefix)), prefixParameters);
			QCOMPARE(StringSet::fromList(manager.getFilteredParametersList(prefix + "root", re)), filteredParameters);
			QVERIFY_EXCEPTION_THROWN(manager.getParametersList(prefix + "root/nonExisting"), NonExistentGroupNameException);
		}
	};

	class copyGroupOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "base/GROUP_A/GROUP_B");
			manager.createParameter(prefix + "base", "dummy", "joe");
			manager.createParameter(prefix + "base/GROUP_A", "param1", "value1");
			manager.createParameter(prefix + "base/GROUP_A/GROUP_B", "param2", "value2");

			manager.copyGroup(prefix + "base/GROUP_A", prefix + "anotherBase/GROUP_C");

			QVERIFY(manager.groupExists(prefix + "base"));
			QCOMPARE(manager.getValue(prefix + "base/dummy"), QString("joe"));
			QVERIFY(manager.groupExists(prefix + "base/GROUP_A"));
			QCOMPARE(manager.getValue(prefix + "base/GROUP_A/param1"), QString("value1"));
			QVERIFY(manager.groupExists(prefix + "base/GROUP_A/GROUP_B"));
			QCOMPARE(manager.getValue(prefix + "base/GROUP_A/GROUP_B/param2"), QString("value2"));

			QVERIFY(manager.groupExists(prefix + "anotherBase"));
			QVERIFY(manager.groupExists(prefix + "anotherBase/GROUP_C"));
			QCOMPARE(manager.getValue(prefix + "anotherBase/GROUP_C/param1"), QString("value1"));
			QVERIFY(manager.groupExists(prefix + "anotherBase/GROUP_C/GROUP_B"));
			QCOMPARE(manager.getValue(prefix + "anotherBase/GROUP_C/GROUP_B/param2"), QString("value2"));

			QVERIFY_EXCEPTION_THROWN(manager.copyGroup(prefix + "base/GROUP_A", prefix + "anotherBase"), AlreadyExistingGroupNameException);
		}
	};

	class createDeepCopyOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one/two");
			manager.createGroup(prefix + "one/another");

			manager.createParameter(prefix + "one/two", "parameter");
			manager.createParameter(prefix + "one/two", "dummy");
			manager.createParameter(prefix + "one/another", "param");

			std::auto_ptr<ConfigurationManager> copyAllocated(manager.createDeepCopy());

			QVERIFY(copyAllocated->groupExists(prefix + "one"));
			QVERIFY(copyAllocated->groupExists(prefix + "one/two"));
			QVERIFY(copyAllocated->parameterExists(prefix + "one/two/parameter"));
			QVERIFY(copyAllocated->parameterExists(prefix + "one/two/dummy"));
			QVERIFY(copyAllocated->groupExists(prefix + "one/another"));
			QVERIFY(copyAllocated->parameterExists(prefix + "one/another/param"));

			ConfigurationManager copyLocalVar;
			ConfigurationManager* copyLocalVarReturned = manager.createDeepCopy(&copyLocalVar);

			QCOMPARE(copyLocalVarReturned, &copyLocalVar);
			QVERIFY(copyLocalVar.groupExists(prefix + "one"));
			QVERIFY(copyLocalVar.groupExists(prefix + "one/two"));
			QVERIFY(copyLocalVar.parameterExists(prefix + "one/two/parameter"));
			QVERIFY(copyLocalVar.parameterExists(prefix + "one/two/dummy"));
			QVERIFY(copyLocalVar.groupExists(prefix + "one/another"));
			QVERIFY(copyLocalVar.parameterExists(prefix + "one/another/param"));

			manager.createGroup(prefix + "private");

			QVERIFY(!copyAllocated->groupExists(prefix + "private"));
			QVERIFY(!copyLocalVar.groupExists(prefix + "private"));

			copyAllocated->createParameter(prefix + "one", "paramOfOneCopy");

			QVERIFY(!manager.parameterExists(prefix + "one/paramOfOneCopy"));
			QVERIFY(!copyLocalVar.parameterExists(prefix + "one/paramOfOneCopy"));

			copyLocalVar.createParameter(prefix + "one/another", "another");

			QVERIFY(!manager.parameterExists(prefix + "one/another/another"));
			QVERIFY(!copyAllocated->parameterExists(prefix + "one/another/another"));
		}
	};

	class copyOperation : public SingleOperationWithConfigurationManagerAndPrefix
	{
	public:
		virtual void runSingleWithPrefix(ConfigurationManager& manager, QString prefix)
		{
			manager.createGroup(prefix + "one/two");
			manager.createGroup(prefix + "one/another");

			manager.createParameter(prefix + "one/two", "parameter");
			manager.createParameter(prefix + "one/two", "dummy");
			manager.createParameter(prefix + "one/another", "param");

			ConfigurationManager constructorCopy(manager);
			ConfigurationManager operatorCopy;
			operatorCopy = manager;

			QVERIFY(constructorCopy.groupExists(prefix + "one"));
			QVERIFY(constructorCopy.groupExists(prefix + "one/two"));
			QVERIFY(constructorCopy.parameterExists(prefix + "one/two/parameter"));
			QVERIFY(constructorCopy.parameterExists(prefix + "one/two/dummy"));
			QVERIFY(constructorCopy.groupExists(prefix + "one/another"));
			QVERIFY(constructorCopy.parameterExists(prefix + "one/another/param"));

			QVERIFY(operatorCopy.groupExists(prefix + "one"));
			QVERIFY(operatorCopy.groupExists(prefix + "one/two"));
			QVERIFY(operatorCopy.parameterExists(prefix + "one/two/parameter"));
			QVERIFY(operatorCopy.parameterExists(prefix + "one/two/dummy"));
			QVERIFY(operatorCopy.groupExists(prefix + "one/another"));
			QVERIFY(operatorCopy.parameterExists(prefix + "one/another/param"));

			manager.createGroup(prefix + "private");

			QVERIFY(constructorCopy.groupExists(prefix + "private"));
			QVERIFY(operatorCopy.groupExists(prefix + "private"));

			constructorCopy.createParameter(prefix + "one", "paramOfOneCopy");

			QVERIFY(manager.parameterExists(prefix + "one/paramOfOneCopy"));
			QVERIFY(operatorCopy.parameterExists(prefix + "one/paramOfOneCopy"));

			operatorCopy.createParameter(prefix + "one/another", "another");

			QVERIFY(manager.parameterExists(prefix + "one/another/another"));
			QVERIFY(constructorCopy.parameterExists(prefix + "one/another/another"));
		}
	};

private slots:
	void expectedGroupSeparatorAndParentGroup()
	{
		QCOMPARE(GroupSeparator, "/");
		QCOMPARE(ParentGroup, "..");
	}

	void createGroup()
	{
		// This used to fail because locks in ConfigurationManager were not taken correctly:
		// all functions had QMutexLocker(&(m_shared->mutex)); in which the object is created
		// and immediately destroyed, instead of QMutexLocker locker(&(m_shared->mutex)); in
		// which the object is destroyed when it goes out of scope. This comment is here
		// to remind which is the correct way to create a QMutexLocker
		doMultiThreadedTest<createGroupOperation>();
	}

	void deleteGroup()
	{
		doMultiThreadedTest<deleteGroupOperation>();
	}

	void renameGroup()
	{
		doMultiThreadedTest<renameGroupOperation>();
	}

	void getGroupsList()
	{
		doMultiThreadedTest<getGroupsListOperation>();
	}

	void clearAll()
	{
		ConfigurationManager manager;
		const StringSet groups = StringSet() << "one" << "two" << "timmy" << "tommy" << "jimmy" << "group:1" << "group:3" << "group:1ttt";

		foreach(QString s, groups) {
			manager.createGroup("root/" + s);
		}

		QCOMPARE(manager.getGroupsList("root").size(), groups.size());
		QCOMPARE(manager.getGroupsList("").size(), 1);

		manager.clearAll();

		QVERIFY(manager.getGroupsList("").isEmpty());
	}

	void createParameter()
	{
		doMultiThreadedTest<createParameterOperation>();
	}

	void deleteParameter()
	{
		doMultiThreadedTest<deleteParameterOperation>();
	}

	void setAndGetValue()
	{
		doMultiThreadedTest<setAndGetValueOperation>();
	}

	void getValueAlsoMatchParents()
	{
		doMultiThreadedTest<getValueAlsoMatchParentsOperation>();
	}

	void getParametersList()
	{
		doMultiThreadedTest<getParametersListOperation>();
	}

	void copyGroup()
	{
		doMultiThreadedTest<copyGroupOperation>();
	}

	void createDeepCopy()
	{
		doMultiThreadedTest<createDeepCopyOperation>(10, 5);
	}

	void copy()
	{
		doMultiThreadedTest<copyOperation>();
	}
};

QTEST_MAIN(ConfigurationManager_Test)
#include "configurationmanager_test.moc"
