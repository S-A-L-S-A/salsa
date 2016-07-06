/*******************************************************************************
 * FARSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#include "evoalgocomponent.h"
#include "configurationhelper.h"
#include "renderworldwrapperwidget.h"

namespace farsa {

EvoAlgoComponent::EvoAlgoComponent() :
	BaseExperiment(),
	m_batchRunning(false),
	m_ga(NULL),
	m_replications(1),
	m_tests()
{
	DECLARE_STEPPABLE_THREAD_OPERATION(EvoAlgoComponent, evolve)
}

EvoAlgoComponent::~EvoAlgoComponent()
{
}

void EvoAlgoComponent::configure(ConfigurationParameters& params, QString prefix)
{
	BaseExperiment::configure(params, prefix);
	m_batchRunning = ConfigurationHelper::getBool(params, "__INTERNAL__/BatchRunning", m_batchRunning);
	m_ga = params.getObjectFromGroup<EvoAlgo>(prefix + QString("GA"));
	m_replications = ConfigurationHelper::getInt(params, prefix + QString("replications"), 1);
	m_ga->setFlowController(flowController());
	foreach( QString group, params.getGroupsWithPrefixList(prefix, QString( "Test" )) ) {
		m_tests.append( params.getObjectFromGroup<NewAbstractTest>( prefix + group ) );
		m_tests.last()->setFlowController(flowController());
		addOperation(m_tests.last()->name(), &EvoAlgoComponent::runTest, (int)(m_tests.size() - 1), true, true);
		//DECLARE_STEPPABLE_THREAD_OPERATION_ONE_PARAMETER(EvoAlgoComponent, runTest, m_tests.size() - 1)
	}
}

void EvoAlgoComponent::save(ConfigurationParameters& params, QString prefix)
{
	Logger::error("NOT IMPLEMENTED (EvoAlgoComponent::save)");
	abort();
}

void EvoAlgoComponent::describe(QString type)
{
	BaseExperiment::describe(type);
	Descriptor d = addTypeDescription(type, "Evolutionary algorithm component");
	d.describeSubgroup("GA").type("EvoAlgo").props(IsMandatory).help("Object containing the evolutionary algorithm");
	d.describeInt("numReplications").limits(1, INT_MAX).def(1).help("Number of the relpications of the evolutionary process");
	d.describeSubgroup("Test").type("NewAbstractTest").props(AllowMultiple).help("The test");
}

void EvoAlgoComponent::postConfigureInitialization()
{
	BaseExperiment::postConfigureInitialization();
}

void EvoAlgoComponent::evolve()
{
	for (int i = 0; i < m_replications; i++)
	{
		m_ga->setSeed(1 + m_ga->seed());
		m_ga->runEvolution();
		pauseFlow();
		if (stopFlow())
		{
			return;
		}
	}
}

void EvoAlgoComponent::runTest(int idTest)
{
	if (idTest < 0 || idTest >= m_tests.size())
	{
		Logger::warning("Invalid test id "+QString::number(idTest));
		return;
	}
	m_tests[idTest]->runTest();
}

} // end namespace farsa
