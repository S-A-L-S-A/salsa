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

#include "configurationnode.h"
#include <QtTest/QtTest>
#include <algorithm>
#include <memory>
#include <QRegExp>
#include <QSet>

// NOTES AND TODOS
//
//

using namespace salsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ConfigurationNode_Test : public QObject
{
	Q_OBJECT

	typedef std::unique_ptr<ConfigurationNode> ConfigurationNodeUniquePtr;
	typedef QSet<QString> StringSet;
	typedef QSet<ConfigurationNode*> ConfigurationNodesSet;

	struct TreeAndNodes
	{
		// Indentation reflects the hierarchy of nodes
		ConfigurationNodeUniquePtr root;
			ConfigurationNode* one;
			ConfigurationNode* two;
				ConfigurationNode* grandchild;
					ConfigurationNode* downOneLevel;
					ConfigurationNode* downAgain;
						ConfigurationNode* andAgain;
	};

	void generateTree(TreeAndNodes& t)
	{
		// Indentation reflects the hierarchy of nodes
		t.root.reset(new ConfigurationNode("root"));
			t.one = t.root->addNode("one");
			t.two = t.root->addNode("two");
				t.grandchild = t.two->addNode("grandchild");
					t.downOneLevel = t.grandchild->addNode("downOneLevel");
					t.downAgain = t.grandchild->addNode("downAgain");
						t.andAgain = t.downAgain->addNode("andAgain");
	}

	// Returns a pointer to a component given a number. This does not create
	// a real component, it simply casts the number to a pointer. It can be
	// used for calls taking a pointer to a Component object because we know
	// that ConfigurationNode function only store pointers and never use
	// them.
	Component* componentDummyPointerFromNumber(int n)
	{
		return reinterpret_cast<Component*>(n);
	}

private slots:
	void expectedGroupSeparatorAndParentGroup()
	{
		QCOMPARE(GroupSeparator, "/");
		QCOMPARE(ParentGroup, "..");
	}

	void createNodeValidName()
	{
		ConfigurationNodeUniquePtr nodeValidName(new ConfigurationNode("group"));
		ConfigurationNodeUniquePtr nodeValidDotName(new ConfigurationNode("thisIs..Valid"));

		QCOMPARE(nodeValidName->getParent(), (ConfigurationNode*) NULL);
		QCOMPARE(nodeValidName->getName(), QString("group"));
		QCOMPARE(nodeValidDotName->getParent(), (ConfigurationNode*) NULL);
		QCOMPARE(nodeValidDotName->getName(), QString("thisIs..Valid"));
	}

	void createNodeInvalidName()
	{
		QVERIFY_EXCEPTION_THROWN(new ConfigurationNode(""), InvalidGroupNameException);
		QVERIFY_EXCEPTION_THROWN(new ConfigurationNode("group/child"), InvalidGroupNameException);
		QVERIFY_EXCEPTION_THROWN(new ConfigurationNode(".."), InvalidGroupNameException);
	}

	void addNode()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("rootGroup"));
		ConfigurationNode* child = root->addNode("child");

		QCOMPARE(child->getParent(), root.get());
		QVERIFY_EXCEPTION_THROWN(root->addNode(".."), InvalidGroupNameException);
		QVERIFY_EXCEPTION_THROWN(root->addNode("child"), AlreadyExistingGroupNameException);
	}

	void addNodeOrReturnExisting()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("rootGroup"));
		ConfigurationNode* child = root->addNodeOrReturnExisting("child");

		QCOMPARE(child->getParent(), root.get());
		QVERIFY_EXCEPTION_THROWN(root->addNodeOrReturnExisting(".."), InvalidGroupNameException);
		QCOMPARE(root->addNodeOrReturnExisting("child"), child);
	}

	void checkChildExists()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("rootGroup"));
		root->addNodeOrReturnExisting("child");

		QCOMPARE(root->hasChild("child"), true);
		QCOMPARE(root->hasChild("anotherChild"), false);
	}

	void childrenList()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("rootGroup"));
		const StringSet childrenNames = StringSet() << "child" << "anotherChild" << "Timmy" << "Tommy" << "Jimmy";
		ConfigurationNodesSet children;
		const QRegExp re(".*hild");
		const StringSet filteredChildrenNames = StringSet() << "child" << "anotherChild";

		foreach (QString c, childrenNames) {
			children.insert(root->addNode(c));
		}

		QCOMPARE(ConfigurationNodesSet::fromList(root->getChildrenList()), children);
		QCOMPARE(StringSet::fromList(root->getChildrenNamesList()), childrenNames);
		QCOMPARE(StringSet::fromList(root->getFilteredChildrenNamesList(re)), filteredChildrenNames);
	}

	void ancestorsAndFullName()
	{
		TreeAndNodes t;
		generateTree(t);

		const QList<const ConfigurationNode*> ancestors = QList<const ConfigurationNode*>() << t.root.get() << t.two << t.grandchild << t.downAgain << t.andAgain;
		const QStringList ancestorsNames = QStringList() << "root" << "two" << "grandchild" << "downAgain" << "andAgain";
		const QString fullName = "root/two/grandchild/downAgain/andAgain";

		QCOMPARE(t.andAgain->getAncestors(), ancestors);
		QCOMPARE(t.andAgain->getAncestorsNames(), ancestorsNames);
		QCOMPARE(t.andAgain->getFullName(), fullName);
	}

	void deleteChild()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("root"));
		root->addNode("one");
		root->addNode("two");
		root->addNode("three");

		QCOMPARE(root->hasChild("two"), true);
		root->deleteChild("two");
		QCOMPARE(root->hasChild("two"), false);

		QVERIFY_EXCEPTION_THROWN(root->deleteChild("dummy"), NonExistentGroupNameException);
	}

	void renameChild()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("root"));
		root->addNode("one");
		root->addNode("two");
		root->addNode("three");

		QCOMPARE(root->hasChild("two"), true);
		QCOMPARE(root->hasChild("twoRenamed"), false);
		root->renameChild("two", "twoRenamed");
		QCOMPARE(root->hasChild("two"), false);
		QCOMPARE(root->hasChild("twoRenamed"), true);

		QVERIFY_EXCEPTION_THROWN(root->renameChild("one", "a/b"), InvalidGroupNameException);
		QVERIFY_EXCEPTION_THROWN(root->renameChild("one", "three"), AlreadyExistingGroupNameException);
		QVERIFY_EXCEPTION_THROWN(root->renameChild("dummy", "tummy"), NonExistentGroupNameException);
	}

	void getNode()
	{
		TreeAndNodes t;
		generateTree(t);

		QCOMPARE(t.root->getNode("one"), t.one);
		QCOMPARE(t.root->getNode("two/grandchild/downOneLevel"), t.downOneLevel);
		QCOMPARE(t.grandchild->getNode("downAgain/andAgain"), t.andAgain);
		QCOMPARE(t.one->getNode("/"), t.one);
		QCOMPARE(t.two->getNode(""), t.two);
		QCOMPARE(t.grandchild->getNode("downAgain/.."), t.grandchild);
		QCOMPARE(t.grandchild->getNode("../.."), t.root.get());
		QVERIFY_EXCEPTION_THROWN(t.root->getNode("dummy"), NonExistentGroupNameException);
		QVERIFY_EXCEPTION_THROWN(t.root->getNode("one/dummy"), NonExistentGroupNameException);
	}

	void isPathValid()
	{
		TreeAndNodes t;
		generateTree(t);

		QCOMPARE(t.root->isPathValid("two/grandchild/downOneLevel"), true);
		QCOMPARE(t.root->isPathValid("two/downOneLevel"), false);
		QCOMPARE(t.one->isPathValid("/"), true);
		QCOMPARE(t.andAgain->isPathValid(""), true);
		QCOMPARE(t.downOneLevel->isPathValid("../../grandchild"), true);
		QCOMPARE(t.downOneLevel->isPathValid("../dummy"), false);

	}

	void setComponentForNode()
	{
		TreeAndNodes t;
		generateTree(t);

		// Dummy pointers to componenents
		int componentsId = 1;
		Component* rootComponent = componentDummyPointerFromNumber(componentsId++);
		Component* twoComponent = componentDummyPointerFromNumber(componentsId++);
		Component* downOneLevelComponent = componentDummyPointerFromNumber(componentsId++);
		Component* andAgainComponent = componentDummyPointerFromNumber(componentsId++);

		t.root->setComponentForNode("", rootComponent, ComponentCreatedAndConfigured);
		t.root->setComponentForNode("two", twoComponent, CreatingComponent);
		t.root->setComponentForNode("two/grandchild/downOneLevel", downOneLevelComponent, ComponentCreatedNotConfigured);
		t.grandchild->setComponentForNode("downAgain/andAgain", andAgainComponent, ConfiguringComponent);

		QCOMPARE(t.root->getComponentForNode("").component, rootComponent);
		QCOMPARE(t.root->getComponentForNode("").status, ComponentCreatedAndConfigured);
		QCOMPARE(t.two->getComponentForNode("").component, twoComponent);
		QCOMPARE(t.two->getComponentForNode("").status, CreatingComponent);
		QCOMPARE(t.downOneLevel->getComponentForNode("").component, downOneLevelComponent);
		QCOMPARE(t.downOneLevel->getComponentForNode("").status, ComponentCreatedNotConfigured);
		QCOMPARE(t.root->getComponentForNode("two/grandchild/downAgain/andAgain").component, andAgainComponent);
		QCOMPARE(t.root->getComponentForNode("two/grandchild/downAgain/andAgain").status, ConfiguringComponent);

		t.root->setComponentForNode("two", NULL, ConfiguringComponent);
		QCOMPARE(t.two->getComponentForNode("").component, twoComponent);
		QCOMPARE(t.two->getComponentForNode("").status, ConfiguringComponent);

		Component* andAgainOtherComponent = componentDummyPointerFromNumber(componentsId++);
		t.grandchild->setComponentForNode("downAgain/andAgain", andAgainOtherComponent, CreatingComponent);
		QCOMPARE(t.andAgain->getComponentForNode("").component, andAgainOtherComponent);
		QCOMPARE(t.andAgain->getComponentForNode("").status, CreatingComponent);

		t.two->setComponentForNode("", NULL, ComponentNotCreated);
		QCOMPARE(t.two->getComponentForNode("").component, (Component*) NULL);
		QCOMPARE(t.two->getComponentForNode("").status, ComponentNotCreated);

		Component* downOneLevelOtherComponent = componentDummyPointerFromNumber(componentsId++);
		t.downOneLevel->setComponentForNode("", downOneLevelOtherComponent, ComponentNotCreated);
		QCOMPARE(t.downOneLevel->getComponentForNode("").component, (Component*) NULL);
		QCOMPARE(t.downOneLevel->getComponentForNode("").status, ComponentNotCreated);

		t.root->resetComponent();
		QCOMPARE(t.root->getComponentForNode("").component, (Component*) NULL);
		QCOMPARE(t.root->getComponentForNode("").status, ComponentNotCreated);
	}

	void addParameter()
	{
		ConfigurationNodeUniquePtr node(new ConfigurationNode("root"));

		node->addParameter("p1");
		node->addParameter("p2..p3");

		QCOMPARE(node->parameterExists("p1"), true);
		QCOMPARE(node->parameterExists("p2..p3"), true);
		QCOMPARE(node->parameterExists("param"), false);
		QVERIFY_EXCEPTION_THROWN(node->addParameter("p1"), AlreadyExistingParameterException);
		QVERIFY_EXCEPTION_THROWN(node->addParameter(""), InvalidParameterNameException);
		QVERIFY_EXCEPTION_THROWN(node->addParameter("aaa/bbb"), InvalidParameterNameException);
		QVERIFY_EXCEPTION_THROWN(node->addParameter(".."), InvalidParameterNameException);
	}

	void deleteParameter()
	{
		ConfigurationNodeUniquePtr node(new ConfigurationNode("root"));

		node->addParameter("p1");
		node->addParameter("param");

		QCOMPARE(node->parameterExists("p1"), true);
		QCOMPARE(node->parameterExists("param"), true);

		node->deleteParameter("p1");

		QCOMPARE(node->parameterExists("p1"), false);
		QCOMPARE(node->parameterExists("param"), true);

		QVERIFY_EXCEPTION_THROWN(node->deleteParameter("p1"), NonExistentParameterException);
		QVERIFY_EXCEPTION_THROWN(node->deleteParameter("whatIsThis"), NonExistentParameterException);
	}

	void parametersList()
	{
		ConfigurationNodeUniquePtr root(new ConfigurationNode("root"));
		const StringSet parameters = StringSet() << "parameter" << "anotherParam" << "One" << "Two" << "Three";
		const QRegExp re(".*aram.*");
		const StringSet filteredParameters = StringSet() << "parameter" << "anotherParam";

		foreach (QString p, parameters) {
			root->addParameter(p);
		}

		QCOMPARE(StringSet::fromList(root->getParametersList()), parameters);
		QCOMPARE(StringSet::fromList(root->getFilteredParametersList(re)), filteredParameters);
	}

	void setParameterValue()
	{
		TreeAndNodes t;
		generateTree(t);

		// The tree and the parameters
		// root (param = r)
		//   +-> one (param = 1)
		//   +-> two (p1 = uno)
		//        +-> grandchild (p1 = nonUno, p2 = ciao)
		//                +-> downOneLevel
		//                +-> downAgain
		//                        +-> andAgain

		t.root->addParameter("param");
		t.one->addParameter("param");
		t.two->addParameter("p1");
		t.grandchild->addParameter("p1");
		t.grandchild->addParameter("p2");

		t.root->setValue("param", "r");
		t.root->setValue("one/param", "1");
		t.root->setValue("two/p1", "uno");
		t.root->setValue("two/grandchild/p1", "nonUno");
		t.root->setValue("two/grandchild/p2", "ciao");

		QCOMPARE(t.root->getValue("param"), QString("r"));
		QCOMPARE(t.root->getValue("one/param"), QString("1"));
		QCOMPARE(t.root->getValue("two/p1"), QString("uno"));
		QCOMPARE(t.root->getValue("two/grandchild/p1"), QString("nonUno"));
		QCOMPARE(t.root->getValue("two/grandchild/p2"), QString("ciao"));

		QCOMPARE(t.root->getValueAlsoMatchParents("param"), QString("r"));
		QCOMPARE(t.root->getValueAlsoMatchParents("one/param"), QString("1"));
		QCOMPARE(t.root->getValueAlsoMatchParents("two/p1"), QString("uno"));
		QCOMPARE(t.root->getValueAlsoMatchParents("two/grandchild/p1"), QString("nonUno"));
		QCOMPARE(t.root->getValueAlsoMatchParents("two/grandchild/p2"), QString("ciao"));
		QCOMPARE(t.root->getValueAlsoMatchParents("two/grandchild/downOneLevel/p1"), QString("nonUno"));
		QCOMPARE(t.root->getValueAlsoMatchParents("two/grandchild/downAgain/andAgain/p2"), QString("ciao"));
		QCOMPARE(t.root->getValueAlsoMatchParents("two/grandchild/downOneLevel/param"), QString("r"));

		QVERIFY_EXCEPTION_THROWN(t.root->getValue("two/dummy/grandchild/downOneLevel/p1"), NonExistentGroupNameException);
		QVERIFY_EXCEPTION_THROWN(t.root->getValue("two/grandchild/downOneLevel/p3"), NonExistentParameterException);
		QVERIFY_EXCEPTION_THROWN(t.root->getValueAlsoMatchParents("one/dummy/p1"), NonExistentGroupNameException);
		QVERIFY_EXCEPTION_THROWN(t.root->getValueAlsoMatchParents("one/p4"), NonExistentParameterException);
	}

	void compareNodes()
	{
		const int numTrees = 4;
		TreeAndNodes t[numTrees];

		for (int i = 0; i < numTrees; ++i) {
			generateTree(t[i]);

			t[i].root->addParameter("param");
			t[i].one->addParameter("param");
			t[i].two->addParameter("p1");
			t[i].grandchild->addParameter("p1");
			t[i].grandchild->addParameter("p2");

			t[i].root->setValue("param", "r");
			t[i].root->setValue("one/param", "1");
			t[i].root->setValue("two/p1", "uno");
			t[i].root->setValue("two/grandchild/p1", "nonUno");
			t[i].root->setValue("two/grandchild/p2", "ciao");
		}

		t[2].root->setValue("two/grandchild/p2", "bye");
		t[3].grandchild->deleteChild("downOneLevel");

		QVERIFY(*(t[0].root) == *(t[1].root));
		QVERIFY(*(t[0].root) != *(t[2].root));
		QVERIFY(*(t[0].root) != *(t[3].root));
		QVERIFY(*(t[0].one) == *(t[2].one));
	}

	void duplicateNode()
	{
		TreeAndNodes t;
		generateTree(t);

		ConfigurationNode* newNode = t.root->duplicateNode("two/grandchild", "newNode", "one");

		QVERIFY(t.one->hasChild("newNode"));
		QVERIFY(*(t.grandchild) == *newNode);
	}

	void createDeepCopy()
	{
		TreeAndNodes t;
		generateTree(t);

		ConfigurationNodeUniquePtr newTree1(t.root->createDeepCopy());
		ConfigurationNodeUniquePtr newTree2(new ConfigurationNode("rootNode"));
		ConfigurationNode* newTree2Ptr = t.root->createDeepCopy(newTree2.get());

		QVERIFY(*(t.root) == *newTree1);
		QVERIFY(newTree2.get() == newTree2Ptr);
		QVERIFY(*(t.root) == *newTree2);
	}

	void depthLevelOfNodes()
	{
		TreeAndNodes t;
		generateTree(t);

		QCOMPARE(t.root->getDepthLevel(), 0);
		QCOMPARE(t.one->getDepthLevel(), 1);
		QCOMPARE(t.two->getDepthLevel(), 1);
		QCOMPARE(t.grandchild->getDepthLevel(), 2);
		QCOMPARE(t.downOneLevel->getDepthLevel(), 3);
		QCOMPARE(t.downAgain->getDepthLevel(), 3);
		QCOMPARE(t.andAgain->getDepthLevel(), 4);
	}

	void depthLevelOfNodesAfterDeepCopy()
	{
		TreeAndNodes t;
		generateTree(t);

		ConfigurationNodeUniquePtr newTree(t.grandchild->createDeepCopy());

		QCOMPARE(newTree->getNode("")->getDepthLevel(), 0);
		QCOMPARE(newTree->getNode("downOneLevel")->getDepthLevel(), 1);
		QCOMPARE(newTree->getNode("downAgain")->getDepthLevel(), 1);
		QCOMPARE(newTree->getNode("downAgain/andAgain")->getDepthLevel(), 2);
	}

	void getLowestCommonAncestorInDifferentTrees()
	{
		TreeAndNodes t1;
		generateTree(t1);

		TreeAndNodes t2;
		generateTree(t2);

		QVERIFY_EXCEPTION_THROWN(t1.two->getLowestCommonAncestor(t2.downAgain), NoCommonAncestorException);

	}

	void getLowestCommonAncestor()
	{
		TreeAndNodes t;
		generateTree(t);

		// Adding some more nodes, to obtain the following structure:
		// ConfigurationNodeAutoPtr root;
		// 	ConfigurationNode* one;
		// 		ConfigurationNode* subOne;
		// 			ConfigurationNode* deepOne;
		// 	ConfigurationNode* two;
		// 		ConfigurationNode* grandchild;
		// 			ConfigurationNode* downOneLevel;
		// 			ConfigurationNode* downAgain;
		// 				ConfigurationNode* andAgain;
		ConfigurationNode* subOne = t.one->addNode("subone");
		ConfigurationNode* deepOne = subOne->addNode("deepOne");

		QCOMPARE(t.andAgain->getLowestCommonAncestor(t.grandchild), t.grandchild);
		QCOMPARE(t.two->getLowestCommonAncestor(t.two), t.two);
		QCOMPARE(t.downAgain->getLowestCommonAncestor(deepOne), t.root.get());
		QCOMPARE(t.andAgain->getLowestCommonAncestor(t.downOneLevel), t.grandchild);
	}

	void getNodesDistance()
	{
		TreeAndNodes t;
		generateTree(t);

		// Adding some more nodes, see above
		ConfigurationNode* subOne = t.one->addNode("subone");
		ConfigurationNode* deepOne = subOne->addNode("deepOne");

		QCOMPARE(t.andAgain->getDistance(t.grandchild), 2);
		QCOMPARE(t.two->getDistance(t.two), 0);
		QCOMPARE(t.downAgain->getDistance(deepOne), 6);
		QCOMPARE(t.andAgain->getDistance(t.downOneLevel), 3);
	}
};

QTEST_MAIN(ConfigurationNode_Test)
#include "configurationnode_test.moc"
