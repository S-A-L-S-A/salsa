/***************************************************************************
 *  FARSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
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

#ifndef TEST_HIERARCHY_H
#define TEST_HIERARCHY_H

#include "configurationmanager.h"
#include "component.h"
#include "testutils.h"
#include <memory>

/**
 * \brief A namespace with classes used in typesdb class registration tests
 */
namespace TypesDBTestHierarchy {
	class InstantiableComponentConfigInConstructor : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		InstantiableComponentConfigInConstructor(farsa::ConfigurationManager& params);
	};

	class InstantiableComponentConfigInFunction : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		InstantiableComponentConfigInFunction(farsa::ConfigurationManager& params);
	};

	class AbstractComponent : public farsa::Component
	{
	public:
		AbstractComponent(farsa::ConfigurationManager& params);

		virtual void f() = 0;
	};

	class ComponentWithCustomCreator : public farsa::Component
	{
	public:
		ComponentWithCustomCreator(farsa::ConfigurationManager& params, int i);
	};

	class ComponentCustomCreator : public farsa::ComponentCreator
	{
	private:
		ComponentWithCustomCreator* instantiate(farsa::ConfigurationManager& settings, QString prefix, farsa::ResourceAccessor* accessorForResources);
	};

	class SampleHierarchyRoot : public farsa::Component
	{
	public:
		SampleHierarchyRoot(farsa::ConfigurationManager& params);
	};

	class SampleHierarchyChildOne : public SampleHierarchyRoot
	{
	public:
		SampleHierarchyChildOne(farsa::ConfigurationManager& params);
	};

	class SampleHierarchyGrandChild : public SampleHierarchyChildOne
	{
	public:
		SampleHierarchyGrandChild(farsa::ConfigurationManager& params);
	};

	class SampleHierarchyChildTwo : public SampleHierarchyRoot
	{
	public:
		SampleHierarchyChildTwo(farsa::ConfigurationManager& params);
	};

	class SampleHierarchyAbstractChild : public SampleHierarchyRoot
	{
	public:
		SampleHierarchyAbstractChild(farsa::ConfigurationManager& params);

		virtual void g() = 0;
	};
}

/**
 * \brief A namespace used when testing TypesDB with multiple interfaces
 */
namespace TypesDBWithInterfacesTestHierarchy {
	class InterfaceDummy {};

	class InterfaceA {};

	class InterfaceB {};

	class MultipleInterfacesComponent : public farsa::Component, public InterfaceA, public InterfaceB
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		MultipleInterfacesComponent(farsa::ConfigurationManager& params);
	};
}

/**
 * \brief A namespace used in component instantiation tests
 *
 * All function calls use FunctionCall to register the sequence of calls, except
 * the configuresInConstructor() function and functions of creators. If a group
 * contains a parameter named "throwException" the corresponding component will
 * throw an exception of type UserDefinedCheckFailureException, whatever the
 * parameter value is, when reading parameters. If a group contains a parameter
 * named "throwExceptionInPostConfigure", the corresponding component will throw
 * an exception of type UserDefinedCheckFailureException, whatever the parameter
 * value is, in the postConfigureInitialization() function.
 */
namespace ComponentInstantiationTestHierarchy {
	/**
	 * \brief Registers all components declared here to TypesDB
	 */
	void registerAllComponents();

	class Standalone : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}
	public:
		Standalone(farsa::ConfigurationManager& params);
		~Standalone();

		// This should not be called
		void configure();
		void postConfigureInitialization();

		int param() const
		{
			return m_param;
		}

	private:
		int m_param;
	};

	class StandaloneWithConfigure : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}
	public:
		StandaloneWithConfigure(farsa::ConfigurationManager& params);
		~StandaloneWithConfigure();

		void configure();
		void postConfigureInitialization();

		int param() const
		{
			return m_param;
		}

	private:
		int m_param;
	};

	class One;
	class Two;
	class AbstractThree;
	class Three;
	class AbstractAnother;
	class Another;

	class One : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		One(farsa::ConfigurationManager& params);
		~One();

		void configure();
		void postConfigureInitialization();

		const QString& p1() const
		{
			return m_p1;
		}
		const QString& p2() const
		{
			return m_p2;
		}
		AbstractAnother* another();
		Two* two0();
		Two* two1();
		Two* two2();
	private:
		QString m_p1;
		QString m_p2;
		std::auto_ptr<AbstractAnother> m_another;
		std::auto_ptr<Two> m_two0;
		std::auto_ptr<Two> m_two1;
		std::auto_ptr<Two> m_two2;
	};

	class Two : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		Two(farsa::ConfigurationManager& params, float f);
		~Two();

		void configure();
		void postConfigureInitialization();

		float f() const
		{
			return m_f;
		}
		const QString& p1() const
		{
			return m_p1;
		}
		AbstractThree* three();
	private:
		const float m_f;
		QString m_p1;
		std::auto_ptr<AbstractThree> m_three;
	};

	class TwoCreator : public farsa::ComponentCreator
	{
	private:
		Two* instantiate(farsa::ConfigurationManager& settings, QString prefix, farsa::ResourceAccessor* accessorForResources);
	};

	class AbstractThree : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		AbstractThree(farsa::ConfigurationManager& params);
		~AbstractThree();

		void configure();
		void postConfigureInitialization();

		virtual void f() = 0;

		const QString& pithree() const
		{
			return m_pithree;
		}

	private:
		QString m_pithree;
	};

	class Three : public AbstractThree
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		Three(farsa::ConfigurationManager& params);
		~Three();

		void configure();
		void postConfigureInitialization();

		void f();

		const QString& pithreeAA() const
		{
			return m_pithreeAA;
		}

	private:
		QString m_pithreeAA;
	};

	class AbstractAnother : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		AbstractAnother(farsa::ConfigurationManager& params);
		~AbstractAnother();

		void configure();
		void postConfigureInitialization();

		virtual void g() = 0;
		virtual AbstractThree* three() = 0;
	};

	class Another : public AbstractAnother
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		Another(farsa::ConfigurationManager& params);
		~Another();

		void configure();
		void postConfigureInitialization();

		void g();

		const QString& pa() const
		{
			return m_pa;
		}
		AbstractThree* three();

	private:
		QString m_pa;
		std::auto_ptr<AbstractThree> m_three;
	};

	class CorrectCircularDependencyRoot;
	class CorrectCircularDependencyA;
	class CorrectCircularDependencyB;

	class CorrectCircularDependencyRoot : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		CorrectCircularDependencyRoot(farsa::ConfigurationManager& params);
		~CorrectCircularDependencyRoot();

		void configure();
		void postConfigureInitialization();

	private:
		std::auto_ptr<CorrectCircularDependencyA> m_a;
	};

	class CorrectCircularDependencyA : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		CorrectCircularDependencyA(farsa::ConfigurationManager& params);
		~CorrectCircularDependencyA();

		void configure();
		void postConfigureInitialization();

	private:
		std::auto_ptr<CorrectCircularDependencyB> m_b;
	};

	class CorrectCircularDependencyB : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		CorrectCircularDependencyB(farsa::ConfigurationManager& params);
		~CorrectCircularDependencyB();

		void configure();
		void postConfigureInitialization();

	private:
		// Not an auto_ptr to prevent double-free
		CorrectCircularDependencyA* m_a;
	};

	class ConstructorCircularDependencyRoot;
	class ConstructorCircularDependencyA;
	class ConstructorCircularDependencyB;

	class ConstructorCircularDependencyRoot : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		ConstructorCircularDependencyRoot(farsa::ConfigurationManager& params);
		~ConstructorCircularDependencyRoot();

		void configure();
		void postConfigureInitialization();

	private:
		std::auto_ptr<ConstructorCircularDependencyA> m_a;
	};

	class ConstructorCircularDependencyA : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		ConstructorCircularDependencyA(farsa::ConfigurationManager& params);
		~ConstructorCircularDependencyA();

		void postConfigureInitialization();

	private:
		std::auto_ptr<ConstructorCircularDependencyB> m_b;
	};

	class ConstructorCircularDependencyB : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		ConstructorCircularDependencyB(farsa::ConfigurationManager& params);
		~ConstructorCircularDependencyB();

		void postConfigureInitialization();

	private:
		// Not an auto_ptr to prevent double-free
		ConstructorCircularDependencyA* m_a;
	};

	class WrongCircularDependencyRoot;
	class WrongCircularDependencyA;
	class WrongCircularDependencyB;

	class WrongCircularDependencyRoot : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		WrongCircularDependencyRoot(farsa::ConfigurationManager& params);
		~WrongCircularDependencyRoot();

		void configure();
		void postConfigureInitialization();

	private:
		std::auto_ptr<WrongCircularDependencyA> m_a;
	};

	class WrongCircularDependencyA : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		WrongCircularDependencyA(farsa::ConfigurationManager& params);
		~WrongCircularDependencyA();

		void configure();
		void postConfigureInitialization();

	private:
		std::auto_ptr<WrongCircularDependencyB> m_b;
	};

	class WrongCircularDependencyB : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		WrongCircularDependencyB(farsa::ConfigurationManager& params);
		~WrongCircularDependencyB();

		void configure();
		void postConfigureInitialization();

	private:
		// Not an auto_ptr to prevent double-free
		WrongCircularDependencyA* m_a;
	};

	class DestructionOne;
	class DestructionTwo;
	class DestructionAbstractThree;
	class DestructionThree;
	class DestructionAbstractAnother;
	class DestructionAnother;

	class DestructionOne : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		DestructionOne(farsa::ConfigurationManager& params);
		~DestructionOne();

		void configure();
		void postConfigureInitialization();

		const QString& p1() const
		{
			return m_p1;
		}
		const QString& p2() const
		{
			return m_p2;
		}
	private:
		QString m_p1;
		QString m_p2;
		DestructionAbstractAnother* m_another;
		DestructionTwo* m_two0;
		DestructionTwo* m_two1;
		DestructionTwo* m_two2;
	};

	class DestructionTwo : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		DestructionTwo(farsa::ConfigurationManager& params, float f);
		~DestructionTwo();

		void configure();
		void postConfigureInitialization();

		float f() const
		{
			return m_f;
		}
		const QString& p1() const
		{
			return m_p1;
		}
	private:
		const float m_f;
		QString m_p1;
		DestructionAbstractThree* m_three;
	};

	class DestructionTwoCreator : public farsa::ComponentCreator
	{
	private:
		DestructionTwo* instantiate(farsa::ConfigurationManager& settings, QString prefix, farsa::ResourceAccessor* accessorForResources);
	};

	class DestructionAbstractThree : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		DestructionAbstractThree(farsa::ConfigurationManager& params);
		~DestructionAbstractThree();

		void configure();
		void postConfigureInitialization();

		virtual void f() = 0;

		const QString& pithree() const
		{
			return m_pithree;
		}

	private:
		QString m_pithree;
	};

	class DestructionThree : public DestructionAbstractThree
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		DestructionThree(farsa::ConfigurationManager& params);
		~DestructionThree();

		void configure();
		void postConfigureInitialization();

		void f();

		const QString& pithreeAA() const
		{
			return m_pithreeAA;
		}

	private:
		QString m_pithreeAA;
	};

	class DestructionAbstractAnother : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		DestructionAbstractAnother(farsa::ConfigurationManager& params);
		~DestructionAbstractAnother();

		void configure();
		void postConfigureInitialization();

		virtual void g() = 0;
	};

	class DestructionAnother : public DestructionAbstractAnother
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		DestructionAnother(farsa::ConfigurationManager& params);
		~DestructionAnother();

		void configure();
		void postConfigureInitialization();

		void g();

		const QString& pa() const
		{
			return m_pa;
		}

	private:
		QString m_pa;
		DestructionAbstractThree* m_three;
	};
}

/**
 * \brief A namespace used in configuration observers tests
 */
namespace ConfigurationObserverTestHierarchy {
	/**
	 * \brief Registers all components declared here to TypesDB
	 */
	void registerAllComponents();

	class One;
	class Two;
	class Three;
	class Another;

	class One : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		One(farsa::ConfigurationManager& params);

		Another* another();
		Two* two();
	private:
		Another* m_another;
		Two* m_two;
	};

	class Two : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		Two(farsa::ConfigurationManager& params);

		Three* three();
	private:
		Three* m_three;
	};

	class Three : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return false;
		}

	public:
		Three(farsa::ConfigurationManager& params);

		void configure();
	};

	class Another : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}

	public:
		Another(farsa::ConfigurationManager& params);

		Three* three();

	private:
		Three* m_three;
	};
}

/**
 * \brief A namespace used in resources tests
 */
namespace ResourcesTestHierarchy {
	/**
	 * \brief Registers all components declared here to TypesDB
	 */
	void registerAllComponents();

	// Empty class, for basic tests on resources
	class Standalone : public farsa::Component
	{
	public:
		static bool configuresInConstructor()
		{
			return true;
		}
	public:
		Standalone(farsa::ConfigurationManager& params);
	};

	QString nameForNotifyResourceChangeWithParameters(QString className, QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	class One;
	class Two;
	class Three;
	class SubThree;
	class Another;
	class SubAnother;

	class One : public farsa::Component
	{
	public:
		One(farsa::ConfigurationManager& params);

		Another* another();
		Two* two();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		Another* m_another;
		Two* m_two;
	};

	class Two : public farsa::Component
	{
	public:
		Two(farsa::ConfigurationManager& params);

		Three* three();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		Three* m_three;
	};

	class Three : public farsa::Component
	{
	public:
		Three(farsa::ConfigurationManager& params);

		SubThree* subThree();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		SubThree* m_subThree;
	};

	class SubThree : public farsa::Component
	{
	public:
		SubThree(farsa::ConfigurationManager& params);

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);
	};

	class Another : public farsa::Component
	{
	public:
		Another(farsa::ConfigurationManager& params);

		Three* three();
		SubAnother* subAnother();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		Three* m_three;
		SubAnother* m_subAnother;
	};

	class SubAnother : public farsa::Component
	{
	public:
		SubAnother(farsa::ConfigurationManager& params);

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);
	};

	// RC stands for "Resource notification requested during Configuration
	// phase". These classes check for a "throwException" parameter and do
	// actions that should throw an exception if found (like e.g. trying to
	// get a non-existing resource)
	class RCOne;
	class RCTwo;
	class RCThree;
	class RCSubThree;
	class RCAnother;
	class RCSubAnother;

	class RCOne : public farsa::Component
	{
	public:
		RCOne(farsa::ConfigurationManager& params);

		RCAnother* another();
		RCTwo* two();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		RCAnother* m_another;
		RCTwo* m_two;
	};

	class RCTwo : public farsa::Component
	{
	public:
		RCTwo(farsa::ConfigurationManager& params);

		RCThree* three();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		RCThree* m_three;

		bool m_bTwo;
	};

	class RCThree : public farsa::Component
	{
	public:
		RCThree(farsa::ConfigurationManager& params);

		RCSubThree* subThree();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		RCSubThree* m_subThree;
		int m_ambiguous;
	};

	class RCSubThree : public farsa::Component
	{
	public:
		RCSubThree(farsa::ConfigurationManager& params);

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		float m_sub;
	};

	class RCAnother : public farsa::Component
	{
	public:
		RCAnother(farsa::ConfigurationManager& params);

		RCThree* three();
		RCSubAnother* subAnother();

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		RCThree* m_three;
		RCSubAnother* m_subAnother;
	};

	class RCSubAnother : public farsa::Component
	{
	public:
		RCSubAnother(farsa::ConfigurationManager& params);

	protected:
		void resourceChanged(QString name, farsa::Component* owner, farsa::ResourceChangeType changeType);

	private:
		int m_ambiguous;
	};

	// A small hierarchy to test access to resources in a component creator.
	// CCR stands for access to Resources in Component Creator

	class RCCFirst;
	class RCCSecond;

	class RCCFirst : public farsa::Component
	{
	public:
		RCCFirst(farsa::ConfigurationManager& params);

		RCCSecond* second();

	private:
		int m_resource;
		RCCSecond* m_second;
	};

	class RCCSecond : public farsa::Component
	{
	public:
		RCCSecond(farsa::ConfigurationManager& params, int i);

		int i() const;

	private:
		int m_i;
	};

	class RCCSecondCreator : public farsa::ComponentCreator
	{
	private:
		farsa::Component* instantiate(farsa::ConfigurationManager& settings, QString prefix, farsa::ResourceAccessor* accessorForResources);
	};
}

#endif
