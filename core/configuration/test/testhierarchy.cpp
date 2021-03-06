/***************************************************************************
 *  SALSA Configuration Library                                            *
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

#include "testhierarchy.h"
#include "configurationhelper.h"
#include "typesdb.h"

namespace TypesDBTestHierarchy {
	InstantiableComponentConfigInConstructor::InstantiableComponentConfigInConstructor(salsa::ConfigurationManager& params)
		: Component(params)
	{
	}

	InstantiableComponentConfigInFunction::InstantiableComponentConfigInFunction(salsa::ConfigurationManager& params)
		: Component(params)
	{
	}

	AbstractComponent::AbstractComponent(salsa::ConfigurationManager& params)
		: Component(params)
	{
	}

	ComponentWithCustomCreator::ComponentWithCustomCreator(salsa::ConfigurationManager& params, int)
		: Component(params)
	{
	}

	ComponentWithCustomCreator* ComponentCustomCreator::instantiate(salsa::ConfigurationManager& settings, QString, salsa::ResourceAccessor*)
	{
		return new ComponentWithCustomCreator(settings, 17);
	}

	SampleHierarchyRoot::SampleHierarchyRoot(salsa::ConfigurationManager& params)
		: Component(params)
	{
	}

	SampleHierarchyChildOne::SampleHierarchyChildOne(salsa::ConfigurationManager& params)
		: SampleHierarchyRoot(params)
	{
	}

	SampleHierarchyGrandChild::SampleHierarchyGrandChild(salsa::ConfigurationManager& params)
		: SampleHierarchyChildOne(params)
	{
	}

	SampleHierarchyChildTwo::SampleHierarchyChildTwo(salsa::ConfigurationManager& params)
		: SampleHierarchyRoot(params)
	{
	}

	SampleHierarchyAbstractChild::SampleHierarchyAbstractChild(salsa::ConfigurationManager& params)
		: SampleHierarchyRoot(params)
	{
	}
}

namespace TypesDBWithInterfacesTestHierarchy {
	MultipleInterfacesComponent::MultipleInterfacesComponent(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, InterfaceA()
		, InterfaceB()
	{
	}
}

#define FC(name) FunctionCall call(name)

namespace ComponentInstantiationTestHierarchy {
	void registerAllComponents()
	{
		using namespace salsa;

		TypesDB::instance().registerType<Standalone>("Standalone", QStringList() << "Component");
		TypesDB::instance().registerType<StandaloneWithConfigure>("StandaloneWithConfigure", QStringList() << "Component");

		TypesDB::instance().registerType<One>("One", QStringList() << "Component");
		TypesDB::instance().registerType<Two, TwoCreator>("Two", QStringList() << "Component");
		TypesDB::instance().registerType<AbstractThree>("AbstractThree", QStringList() << "Component");
		TypesDB::instance().registerType<Three>("Three", QStringList() << "AbstractThree");
		TypesDB::instance().registerType<AbstractAnother>("AbstractAnother", QStringList() << "Component");
		TypesDB::instance().registerType<Another>("Another", QStringList() << "AbstractAnother");

		TypesDB::instance().registerType<CorrectCircularDependencyRoot>("CorrectCircularDependencyRoot", QStringList() << "Component");
		TypesDB::instance().registerType<CorrectCircularDependencyA>("CorrectCircularDependencyA", QStringList() << "Component");
		TypesDB::instance().registerType<CorrectCircularDependencyB>("CorrectCircularDependencyB", QStringList() << "Component");

		TypesDB::instance().registerType<ConstructorCircularDependencyRoot>("ConstructorCircularDependencyRoot", QStringList() << "Component");
		TypesDB::instance().registerType<ConstructorCircularDependencyA>("ConstructorCircularDependencyA", QStringList() << "Component");
		TypesDB::instance().registerType<ConstructorCircularDependencyB>("ConstructorCircularDependencyB", QStringList() << "Component");

		TypesDB::instance().registerType<WrongCircularDependencyRoot>("WrongCircularDependencyRoot", QStringList() << "Component");
		TypesDB::instance().registerType<WrongCircularDependencyA>("WrongCircularDependencyA", QStringList() << "Component");
		TypesDB::instance().registerType<WrongCircularDependencyB>("WrongCircularDependencyB", QStringList() << "Component");

		TypesDB::instance().registerType<DestructionOne>("DestructionOne", QStringList() << "Component");
		TypesDB::instance().registerType<DestructionTwo, DestructionTwoCreator>("DestructionTwo", QStringList() << "Component");
		TypesDB::instance().registerType<DestructionAbstractThree>("DestructionAbstractThree", QStringList() << "Component");
		TypesDB::instance().registerType<DestructionThree>("DestructionThree", QStringList() << "DestructionAbstractThree");
		TypesDB::instance().registerType<DestructionAbstractAnother>("DestructionAbstractAnother", QStringList() << "Component");
		TypesDB::instance().registerType<DestructionAnother>("DestructionAnother", QStringList() << "DestructionAbstractAnother");
	}

	void throwExceptionIfThrowParameterExists(const salsa::Component* component)
	{
		const QString p = component->confPath() + "throwException";
		if (component->configurationManager().parameterExists(p)) {
			salsa::ConfigurationHelper::throwUserConfigError(p, "", "We were forced to throw an exception");
		}
	}

	void throwExceptionIfThrowInPostConfigureParameterExists(const salsa::Component* component)
	{
		const QString p = component->confPath() + "throwExceptionInPostConfigure";
		if (component->configurationManager().parameterExists(p)) {
			salsa::ConfigurationHelper::throwUserConfigError(p, "", "We were forced to throw an exception");
		}
	}

	Standalone::Standalone(salsa::ConfigurationManager& params)
		: Component(params)
		, m_param(0)
	{
		FC("Standalone::Standalone");

		m_param = configurationManager().getValue(confPath() + "param").toInt();
		throwExceptionIfThrowParameterExists(this);
	}

	Standalone::~Standalone()
	{
		FC("Standalone::~Standalone");
	}

	void Standalone::configure()
	{
		FC("Standalone::configure");
	}

	void Standalone::postConfigureInitialization()
	{
		FC("Standalone::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	StandaloneWithConfigure::StandaloneWithConfigure(salsa::ConfigurationManager& params)
		: Component(params)
		, m_param(0)
	{
		FC("StandaloneWithConfigure::StandaloneWithConfigure");
	}

	StandaloneWithConfigure::~StandaloneWithConfigure()
	{
		FC("StandaloneWithConfigure::~StandaloneWithConfigure");
	}

	void StandaloneWithConfigure::configure()
	{
		FC("StandaloneWithConfigure::configure");

		m_param = configurationManager().getValue(confPath() + "param").toInt();
		throwExceptionIfThrowParameterExists(this);
	}

	void StandaloneWithConfigure::postConfigureInitialization()
	{
		FC("StandaloneWithConfigure::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	One::One(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_p1()
		, m_p2()
		, m_another()
		, m_two0()
		, m_two1()
		, m_two2()
	{
		FC("One::One");

		m_p1 = params.getValue(confPath() + "p1");
		m_p2 = params.getValue(confPath() + "p2");

		m_another.reset(params.getComponentFromGroup<AbstractAnother>(confPath() + "another"));
		m_two0.reset(params.getComponentFromGroup<Two>(confPath() + "two:4"));
		m_two1.reset(params.getComponentFromGroup<Two>(confPath() + "two:12"));
		m_two2.reset(params.getComponentFromGroup<Two>(confPath() + "two:aa"));

		throwExceptionIfThrowParameterExists(this);
	}

	One::~One()
	{
		FC("One::~One");
	}

	void One::configure()
	{
		FC("One::configure");

		Component::configure();
	}

	void One::postConfigureInitialization()
	{
		FC("One::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	AbstractAnother* One::another()
	{
		return m_another.get();
	}

	Two* One::two0()
	{
		return m_two0.get();
	}

	Two* One::two1()
	{
		return m_two1.get();
	}

	Two* One::two2()
	{
		return m_two2.get();
	}

	Two::Two(salsa::ConfigurationManager& params, float f)
		: salsa::Component(params)
		, m_f(f)
		, m_p1()
		, m_three()
	{
		FC("Two::Two");

		m_p1 = params.getValue(confPath() + "p_1");
		m_three.reset(params.getComponentFromGroup<AbstractThree>(confPath() + "three"));

		throwExceptionIfThrowParameterExists(this);
	}

	Two::~Two()
	{
		FC("Two::~Two");
	}

	void Two::configure()
	{
		FC("Two::configure");

		Component::configure();
	}

	void Two::postConfigureInitialization()
	{
		FC("Two::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	AbstractThree* Two::three()
	{
		return m_three.get();
	}

	Two* TwoCreator::instantiate(salsa::ConfigurationManager& settings, QString, salsa::ResourceAccessor*)
	{
		return new Two(settings, 13.15);
	}

	AbstractThree::AbstractThree(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_pithree()
	{
		FC("AbstractThree::AbstractThree");
	}

	AbstractThree::~AbstractThree()
	{
		FC("AbstractThree::~AbstractThree");
	}

	void AbstractThree::configure()
	{
		FC("AbstractThree::configure");

		Component::configure();

		m_pithree = configurationManager().getValue(confPath() + "pithree");

		throwExceptionIfThrowParameterExists(this);
	}

	void AbstractThree::postConfigureInitialization()
	{
		FC("AbstractThree::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	Three::Three(salsa::ConfigurationManager& params)
		: AbstractThree(params)
		, m_pithreeAA()
	{
		FC("Three::Three");
	}

	Three::~Three()
	{
		FC("Three::~Three");
	}

	void Three::configure()
	{
		FC("Three::configure");

		AbstractThree::configure();

		m_pithreeAA = configurationManager().getValue(confPath() + "pithreeAA");

		throwExceptionIfThrowParameterExists(this);
	}

	void Three::postConfigureInitialization()
	{
		FC("Three::postConfigureInitialization");

		AbstractThree::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	void Three::f()
	{
		FC("Three::f");
	}

	AbstractAnother::AbstractAnother(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("AbstractAnother::AbstractAnother");

		throwExceptionIfThrowParameterExists(this);
	}

	AbstractAnother::~AbstractAnother()
	{
		FC("AbstractAnother::~AbstractAnother");
	}

	void AbstractAnother::configure()
	{
		FC("AbstractAnother::configure");

		Component::configure();
	}

	void AbstractAnother::postConfigureInitialization()
	{
		FC("AbstractAnother::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	Another::Another(salsa::ConfigurationManager& params)
		: AbstractAnother(params)
		, m_pa()
		, m_three()
	{
		FC("Another::Another");

		m_pa = params.getValue(confPath() + "p_a");

		m_three.reset(configurationManager().getComponentFromGroup<Three>(confPath() + "three"));

		throwExceptionIfThrowParameterExists(this);
	}

	Another::~Another()
	{
		FC("Another::~Another");
	}

	void Another::configure()
	{
		FC("Another::configure");

		AbstractAnother::configure();
	}

	void Another::postConfigureInitialization()
	{
		FC("Another::postConfigureInitialization");

		AbstractAnother::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	void Another::g()
	{
		FC("Another::g");
	}

	AbstractThree* Another::three()
	{
		return m_three.get();
	}

	CorrectCircularDependencyRoot::CorrectCircularDependencyRoot(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_a()
	{
		FC("CorrectCircularDependencyRoot::CorrectCircularDependencyRoot");
	}

	CorrectCircularDependencyRoot::~CorrectCircularDependencyRoot()
	{
		FC("CorrectCircularDependencyRoot::~CorrectCircularDependencyRoot");
	}

	void CorrectCircularDependencyRoot::configure()
	{
		FC("CorrectCircularDependencyRoot::configure");

		m_a.reset(configurationManager().getComponentFromGroup<CorrectCircularDependencyA>("A"));

		throwExceptionIfThrowParameterExists(this);
	}

	void CorrectCircularDependencyRoot::postConfigureInitialization()
	{
		FC("CorrectCircularDependencyRoot::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	CorrectCircularDependencyA::CorrectCircularDependencyA(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_b()
	{
		FC("CorrectCircularDependencyA::CorrectCircularDependencyA");
	}

	CorrectCircularDependencyA::~CorrectCircularDependencyA()
	{
		FC("CorrectCircularDependencyA::~CorrectCircularDependencyA");
	}

	void CorrectCircularDependencyA::configure()
	{
		FC("CorrectCircularDependencyA::configure");

		m_b.reset(configurationManager().getComponentFromGroup<CorrectCircularDependencyB>("B", false));

		throwExceptionIfThrowParameterExists(this);
	}

	void CorrectCircularDependencyA::postConfigureInitialization()
	{
		FC("CorrectCircularDependencyA::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	CorrectCircularDependencyB::CorrectCircularDependencyB(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_a(nullptr)
	{
		FC("CorrectCircularDependencyB::CorrectCircularDependencyB");
	}

	CorrectCircularDependencyB::~CorrectCircularDependencyB()
	{
		FC("CorrectCircularDependencyB::~CorrectCircularDependencyB");
	}

	void CorrectCircularDependencyB::configure()
	{
		FC("CorrectCircularDependencyB::configure");

		m_a = configurationManager().getComponentFromGroup<CorrectCircularDependencyA>("A", false);

		throwExceptionIfThrowParameterExists(this);
	}

	void CorrectCircularDependencyB::postConfigureInitialization()
	{
		FC("CorrectCircularDependencyB::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	ConstructorCircularDependencyRoot::ConstructorCircularDependencyRoot(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_a()
	{
		FC("ConstructorCircularDependencyRoot::ConstructorCircularDependencyRoot");
	}

	ConstructorCircularDependencyRoot::~ConstructorCircularDependencyRoot()
	{
		FC("ConstructorCircularDependencyRoot::~ConstructorCircularDependencyRoot");
	}

	void ConstructorCircularDependencyRoot::configure()
	{
		FC("ConstructorCircularDependencyRoot::configure");

		m_a.reset(configurationManager().getComponentFromGroup<ConstructorCircularDependencyA>("A"));

		throwExceptionIfThrowParameterExists(this);
	}

	void ConstructorCircularDependencyRoot::postConfigureInitialization()
	{
		FC("ConstructorCircularDependencyRoot::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	ConstructorCircularDependencyA::ConstructorCircularDependencyA(salsa::ConfigurationManager& params)
		: Component(params)
		, m_b()
	{
		FC("ConstructorCircularDependencyA::ConstructorCircularDependencyA");

		m_b.reset(configurationManager().getComponentFromGroup<ConstructorCircularDependencyB>("B"));
	}

	ConstructorCircularDependencyA::~ConstructorCircularDependencyA()
	{
		FC("ConstructorCircularDependencyA::~ConstructorCircularDependencyA");
	}

	void ConstructorCircularDependencyA::postConfigureInitialization()
	{
		FC("ConstructorCircularDependencyA::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	ConstructorCircularDependencyB::ConstructorCircularDependencyB(salsa::ConfigurationManager& params)
		: Component(params)
		, m_a(nullptr)
	{
		FC("ConstructorCircularDependencyB::ConstructorCircularDependencyB");

		m_a = configurationManager().getComponentFromGroup<ConstructorCircularDependencyA>("A", false);

		throwExceptionIfThrowParameterExists(this);
	}

	ConstructorCircularDependencyB::~ConstructorCircularDependencyB()
	{
		FC("ConstructorCircularDependencyB::~ConstructorCircularDependencyB");
	}

	void ConstructorCircularDependencyB::postConfigureInitialization()
	{
		FC("ConstructorCircularDependencyB::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	WrongCircularDependencyRoot::WrongCircularDependencyRoot(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_a()
	{
		FC("WrongCircularDependencyRoot::WrongCircularDependencyRoot");
	}

	WrongCircularDependencyRoot::~WrongCircularDependencyRoot()
	{
		FC("WrongCircularDependencyRoot::~WrongCircularDependencyRoot");
	}

	void WrongCircularDependencyRoot::configure()
	{
		FC("WrongCircularDependencyRoot::configure");

		m_a.reset(configurationManager().getComponentFromGroup<WrongCircularDependencyA>("A"));

		throwExceptionIfThrowParameterExists(this);
	}

	void WrongCircularDependencyRoot::postConfigureInitialization()
	{
		FC("WrongCircularDependencyRoot::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	WrongCircularDependencyA::WrongCircularDependencyA(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_b()
	{
		FC("WrongCircularDependencyA::WrongCircularDependencyA");
	}

	WrongCircularDependencyA::~WrongCircularDependencyA()
	{
		FC("WrongCircularDependencyA::~WrongCircularDependencyA");
	}

	void WrongCircularDependencyA::configure()
	{
		FC("WrongCircularDependencyA::configure");

		m_b.reset(configurationManager().getComponentFromGroup<WrongCircularDependencyB>("B"));

		throwExceptionIfThrowParameterExists(this);
	}

	void WrongCircularDependencyA::postConfigureInitialization()
	{
		FC("WrongCircularDependencyA::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	WrongCircularDependencyB::WrongCircularDependencyB(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_a(nullptr)
	{
		FC("WrongCircularDependencyB::WrongCircularDependencyB");
	}

	WrongCircularDependencyB::~WrongCircularDependencyB()
	{
		FC("WrongCircularDependencyB::~WrongCircularDependencyB");
	}

	void WrongCircularDependencyB::configure()
	{
		FC("WrongCircularDependencyB::configure");

		m_a = configurationManager().getComponentFromGroup<WrongCircularDependencyA>("A");

		throwExceptionIfThrowParameterExists(this);
	}

	void WrongCircularDependencyB::postConfigureInitialization()
	{
		FC("WrongCircularDependencyB::postConfigureInitialization");

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	DestructionOne::DestructionOne(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_p1()
		, m_p2()
		, m_another(nullptr)
		, m_two0(nullptr)
		, m_two1(nullptr)
		, m_two2(nullptr)
	{
		FC("DestructionOne::DestructionOne");

		m_p1 = params.getValue(confPath() + "p1");
		m_p2 = params.getValue(confPath() + "p2");

		m_another = params.getComponentFromGroup<DestructionAbstractAnother>(confPath() + "another");
		m_two0 = params.getComponentFromGroup<DestructionTwo>(confPath() + "two:4");
		m_two1 = params.getComponentFromGroup<DestructionTwo>(confPath() + "two:12");
		m_two2 = params.getComponentFromGroup<DestructionTwo>(confPath() + "two:aa");

		throwExceptionIfThrowParameterExists(this);
	}

	DestructionOne::~DestructionOne()
	{
		FC("DestructionOne::~DestructionOne");
	}

	void DestructionOne::configure()
	{
		FC("DestructionOne::configure");

		Component::configure();
	}

	void DestructionOne::postConfigureInitialization()
	{
		FC("DestructionOne::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	DestructionTwo::DestructionTwo(salsa::ConfigurationManager& params, float f)
		: salsa::Component(params)
		, m_f(f)
		, m_p1()
		, m_three(nullptr)
	{
		FC("DestructionTwo::DestructionTwo");

		m_p1 = params.getValue(confPath() + "p_1");
		m_three = params.getComponentFromGroup<DestructionAbstractThree>(confPath() + "three");

		throwExceptionIfThrowParameterExists(this);
	}

	DestructionTwo::~DestructionTwo()
	{
		FC("DestructionTwo::~DestructionTwo");
	}

	void DestructionTwo::configure()
	{
		FC("DestructionTwo::configure");

		Component::configure();
	}

	void DestructionTwo::postConfigureInitialization()
	{
		FC("DestructionTwo::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	DestructionTwo* DestructionTwoCreator::instantiate(salsa::ConfigurationManager& settings, QString, salsa::ResourceAccessor*)
	{
		return new DestructionTwo(settings, 13.15);
	}

	DestructionAbstractThree::DestructionAbstractThree(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_pithree()
	{
		FC("DestructionAbstractThree::DestructionAbstractThree");
	}

	DestructionAbstractThree::~DestructionAbstractThree()
	{
		FC("DestructionAbstractThree::~DestructionAbstractThree");
	}

	void DestructionAbstractThree::configure()
	{
		FC("DestructionAbstractThree::configure");

		Component::configure();

		m_pithree = configurationManager().getValue(confPath() + "pithree");

		throwExceptionIfThrowParameterExists(this);
	}

	void DestructionAbstractThree::postConfigureInitialization()
	{
		FC("DestructionAbstractThree::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	DestructionThree::DestructionThree(salsa::ConfigurationManager& params)
		: DestructionAbstractThree(params)
		, m_pithreeAA()
	{
		FC("DestructionThree::DestructionThree");
	}

	DestructionThree::~DestructionThree()
	{
		FC("DestructionThree::~DestructionThree");
	}

	void DestructionThree::configure()
	{
		FC("DestructionThree::configure");

		DestructionAbstractThree::configure();

		m_pithreeAA = configurationManager().getValue(confPath() + "pithreeAA");

		throwExceptionIfThrowParameterExists(this);
	}

	void DestructionThree::postConfigureInitialization()
	{
		FC("DestructionThree::postConfigureInitialization");

		DestructionAbstractThree::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	void DestructionThree::f()
	{
		FC("DestructionThree::f");
	}

	DestructionAbstractAnother::DestructionAbstractAnother(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("DestructionAbstractAnother::DestructionAbstractAnother");

		throwExceptionIfThrowParameterExists(this);
	}

	DestructionAbstractAnother::~DestructionAbstractAnother()
	{
		FC("DestructionAbstractAnother::~DestructionAbstractAnother");
	}

	void DestructionAbstractAnother::configure()
	{
		FC("DestructionAbstractAnother::configure");

		Component::configure();
	}

	void DestructionAbstractAnother::postConfigureInitialization()
	{
		FC("DestructionAbstractAnother::postConfigureInitialization");

		Component::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	DestructionAnother::DestructionAnother(salsa::ConfigurationManager& params)
		: DestructionAbstractAnother(params)
		, m_pa()
		, m_three(nullptr)
	{
		FC("DestructionAnother::DestructionAnother");

		m_pa = params.getValue(confPath() + "p_a");

		m_three = configurationManager().getComponentFromGroup<DestructionThree>(confPath() + "three");

		throwExceptionIfThrowParameterExists(this);
	}

	DestructionAnother::~DestructionAnother()
	{
		FC("DestructionAnother::~DestructionAnother");
	}

	void DestructionAnother::configure()
	{
		FC("DestructionAnother::configure");

		DestructionAbstractAnother::configure();
	}

	void DestructionAnother::postConfigureInitialization()
	{
		FC("DestructionAnother::postConfigureInitialization");

		DestructionAbstractAnother::postConfigureInitialization();

		throwExceptionIfThrowInPostConfigureParameterExists(this);
	}

	void DestructionAnother::g()
	{
		FC("DestructionAnother::g");
	}
}

namespace ConfigurationObserverTestHierarchy {
	void registerAllComponents()
	{
		using namespace salsa;

		TypesDB::instance().registerType<One>("One", QStringList() << "Component");
		TypesDB::instance().registerType<Two>("Two", QStringList() << "Component");
		TypesDB::instance().registerType<Three>("Three", QStringList() << "Component");
		TypesDB::instance().registerType<Another>("Another", QStringList() << "Component");
	}

	One::One(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_another(nullptr)
		, m_two(nullptr)
	{
		m_another = params.getComponentFromGroup<Another>(confPath() + "another");
		m_two = params.getComponentFromGroup<Two>(confPath() + "two");
	}

	Another* One::another()
	{
		return m_another;
	}

	Two* One::two()
	{
		return m_two;
	}

	Two::Two(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_three(nullptr)
	{
		m_three = params.getComponentFromGroup<Three>(confPath() + "three");
	}

	Three* Two::three()
	{
		return m_three;
	}

	Three::Three(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
	}

	void Three::configure()
	{
		Component::configure();
	}

	Another::Another(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_three(nullptr)
	{
		m_three = configurationManager().getComponentFromGroup<Three>(confPath() + "three");
	}

	Three* Another::three()
	{
		return m_three;
	}
}

namespace ResourcesTestHierarchy {
	namespace {
		void useGetResourceIfNameRecognized(salsa::ResourceChangeNotifee* notifee, QString name, salsa::ResourceChangeType changeType)
		{
			if (changeType == salsa::ResourceDeleted) {
				return;
			}

			if (name.startsWith("ni")) {
				notifee->getResource<int>();
			} else if (name.startsWith("nf")) {
				notifee->getResource<float>();
			} else if (name.startsWith("nd")) {
				notifee->getResource<double>();
			} else if (name.startsWith("nb")) {
				notifee->getResource<bool>();
			} else if (name.startsWith("nr")) {
				notifee->getResource<salsa::Resource>();
			} else if (name.startsWith("nc")) {
				notifee->getResource<salsa::Component>();
			} else if (name.startsWith("nq")) {
				notifee->getResource<QObject>();
			}

		}
	}

	void registerAllComponents()
	{
		using namespace salsa;

		TypesDB::instance().registerType<Standalone>("Standalone", QStringList() << "Component");
		TypesDB::instance().registerType<One>("One", QStringList() << "Component");
		TypesDB::instance().registerType<Two>("Two", QStringList() << "Component");
		TypesDB::instance().registerType<Three>("Three", QStringList() << "Component");
		TypesDB::instance().registerType<SubThree>("SubThree", QStringList() << "Component");
		TypesDB::instance().registerType<Another>("Another", QStringList() << "Component");
		TypesDB::instance().registerType<SubAnother>("SubAnother", QStringList() << "Component");
		TypesDB::instance().registerType<RCOne>("RCOne", QStringList() << "Component");
		TypesDB::instance().registerType<RCTwo>("RCTwo", QStringList() << "Component");
		TypesDB::instance().registerType<RCThree>("RCThree", QStringList() << "Component");
		TypesDB::instance().registerType<RCSubThree>("RCSubThree", QStringList() << "Component");
		TypesDB::instance().registerType<RCAnother>("RCAnother", QStringList() << "Component");
		TypesDB::instance().registerType<RCSubAnother>("RCSubAnother", QStringList() << "Component");
		TypesDB::instance().registerType<RCCFirst>("RCCFirst", QStringList() << "Component");
		TypesDB::instance().registerType<RCCSecond, RCCSecondCreator>("RCCSecond", QStringList() << "Component");
	}

	Standalone::Standalone(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
	}

	QString nameForNotifyResourceChangeWithParameters(QString className, QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		QString funcName = className + "::resourceChanged(" + name + ", " + owner->confPath() + ", ";
		switch (changeType) {
			case salsa::ResourceCreated:
				funcName += "ResourceCreated";
				break;
			case salsa::ResourceModified:
				funcName += "ResourceModified";
				break;
			case salsa::ResourceDeclaredAsNull:
				funcName += "ResourceDeclaredAsNull";
				break;
			case salsa::ResourceDeleted:
				funcName += "ResourceDeleted";
				break;
		}
		funcName += ")";

		return funcName;
	}

	One::One(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("One::One");

		m_another = configurationManager().getComponentFromGroup<Another>(confPath() + "another");
		m_two = configurationManager().getComponentFromGroup<Two>(confPath() + "two");
	}

	Another* One::another()
	{
		return m_another;
	}

	Two* One::two()
	{
		return m_two;
	}

	void One::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("One", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	Two::Two(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("Two::Two");

		m_three = configurationManager().getComponentFromGroup<Three>(confPath() + "three");
	}

	Three* Two::three()
	{
		return m_three;
	}

	void Two::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("Two", name, owner, changeType));


		useGetResourceIfNameRecognized(this, name, changeType);
	}

	Three::Three(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("Three::Three");

		m_subThree = configurationManager().getComponentFromGroup<SubThree>(confPath() + "subThree");
	}

	SubThree* Three::subThree()
	{
		return m_subThree;
	}

	void Three::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("Three", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	SubThree::SubThree(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("SubThree::SubThree");
	}

	void SubThree::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("SubThree", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	Another::Another(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("Another::Another");

		m_three = configurationManager().getComponentFromGroup<Three>(confPath() + "three");
		m_subAnother = configurationManager().getComponentFromGroup<SubAnother>(confPath() + "subAnother");
	}

	Three* Another::three()
	{
		return m_three;
	}

	SubAnother* Another::subAnother()
	{
		return m_subAnother;
	}

	void Another::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("Another", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	SubAnother::SubAnother(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("SubAnother::SubAnother");
	}

	void SubAnother::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("SubAnother", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCOne::RCOne(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("RCOne::RCOne");

		addNotifiedResource("bTwo");

		m_another = configurationManager().getComponentFromGroup<RCAnother>(confPath() + "another");
		m_two = configurationManager().getComponentFromGroup<RCTwo>(confPath() + "two");
	}

	RCAnother* RCOne::another()
	{
		return m_another;
	}

	RCTwo* RCOne::two()
	{
		return m_two;
	}

	void RCOne::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("RCOne", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCTwo::RCTwo(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_bTwo(false)
	{
		FC("RCTwo::RCTwo");

		declareResource("bTwo", &m_bTwo);

		m_three = configurationManager().getComponentFromGroup<RCThree>(confPath() + "three");
	}

	RCThree* RCTwo::three()
	{
		return m_three;
	}

	void RCTwo::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("RCTwo", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCThree::RCThree(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_ambiguous(17)
	{
		FC("RCThree::RCThree");

		declareResource("ambiguous", &m_ambiguous);

		m_subThree = configurationManager().getComponentFromGroup<RCSubThree>(confPath() + "subThree");
	}

	RCSubThree* RCThree::subThree()
	{
		return m_subThree;
	}

	void RCThree::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("RCThree", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCSubThree::RCSubThree(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_sub(13.0)
	{
		FC("RCSubThree::RCSubThree");

		declareResource("sub", &m_sub);
	}

	void RCSubThree::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("RCSubThree", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCAnother::RCAnother(salsa::ConfigurationManager& params)
		: salsa::Component(params)
	{
		FC("RCAnother::RCAnother");

		if (params.parameterExists(confPath() + "throwException")) {
			addNotifiedResource("ambiguous");
		}

		m_three = configurationManager().getComponentFromGroup<RCThree>(confPath() + "three");
		m_subAnother = configurationManager().getComponentFromGroup<RCSubAnother>(confPath() + "subAnother");
	}

	RCThree* RCAnother::three()
	{
		return m_three;
	}

	RCSubAnother* RCAnother::subAnother()
	{
		return m_subAnother;
	}

	void RCAnother::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("RCAnother", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCSubAnother::RCSubAnother(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_ambiguous(1)
	{
		FC("RCSubAnother::RCSubAnother");

		declareResource("ambiguous", &m_ambiguous);

		getResource<Component>("one");
	}

	void RCSubAnother::resourceChanged(QString name, salsa::Component* owner, salsa::ResourceChangeType changeType)
	{
		FC(nameForNotifyResourceChangeWithParameters("RCSubAnother", name, owner, changeType));

		useGetResourceIfNameRecognized(this, name, changeType);
	}

	RCCFirst::RCCFirst(salsa::ConfigurationManager& params)
		: salsa::Component(params)
		, m_resource(17)
		, m_second(nullptr)
	{
		declareResource("resource", &m_resource);

		m_second = configurationManager().getComponentFromGroup<RCCSecond>(confPath() + "Second");
	}

	RCCSecond* RCCFirst::second()
	{
		return m_second;
	}

	RCCSecond::RCCSecond(salsa::ConfigurationManager& params, int i)
		: salsa::Component(params)
		, m_i(i)
	{
	}

	int RCCSecond::i() const
	{
		return m_i;
	}

	salsa::Component* RCCSecondCreator::instantiate(salsa::ConfigurationManager& settings, QString, salsa::ResourceAccessor* accessorForResources)
	{
		int* i = accessorForResources->getResource<int>("resource");

		return new RCCSecond(settings, *i);
	}
}
