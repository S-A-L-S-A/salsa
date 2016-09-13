/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

#include "evoalgotest.h"
#include "configurationhelper.h"

namespace salsa {

NewAbstractTest::NewAbstractTest()
	: ParameterSettableWithConfigureFunction()
	, FlowControlled()
	, m_name()
	, m_tooltip("Abstract Text")
	, m_iconFilename(QString())
{
}

void NewAbstractTest::configure(ConfigurationParameters& params, QString prefix)
{
	m_name = ConfigurationHelper::getString(params, prefix + "name", prefix);
}

void NewAbstractTest::describe( QString type )
{
	Descriptor d = addTypeDescription( type, "Base class of all Tests" );
	d.describeString("name").help("The name of the test");
}

EvoAlgoTest::EvoAlgoTest()
	: NewAbstractTest()
	, m_ga(nullptr)
	, m_prototype(nullptr)
	, m_gt(nullptr)
	, m_gae(nullptr)
{
}

void EvoAlgoTest::configure(ConfigurationParameters& params, QString prefix)
{
	NewAbstractTest::configure(params, prefix);
	QString strGA = ConfigurationHelper::getString(params, prefix + "GA", "Component/GA");
	m_ga = params.getObjectFromGroup<EvoAlgo>(strGA);
	QString strGen = ConfigurationHelper::getString(params, prefix + "Genotype", "Component/GA/Genotype");
	m_prototype = params.getObjectFromGroup<Genotype>(strGen);
	QString strGtester = ConfigurationHelper::getString(params, prefix + "GenotypeTester", "GenotypeToPhenotypeMapping");
	m_gt = params.getObjectFromGroup<SingleGenotypeTester>(strGtester);
	QString strGae = ConfigurationHelper::getString(params, prefix + "GAEvaluator", "Experiment");
	m_gae = params.getObjectFromGroup<RobotExperiment>(strGae);
}

void EvoAlgoTest::describe( QString type )
{
	NewAbstractTest::describe(type);
	Descriptor d = addTypeDescription( type, "Evolutionary algorithm tests" );
	d.describeObject("GA").type("EvoAlgo").help("The genetic algorithm");
	d.describeObject("Genotype").type("Genotype").help("The genotype");
	d.describeObject("GenotypeTester").type("SingleGenotypeTester").help("The mapper");
	d.describeObject("GAEvaluator").type("RobotExperiment").help("The evaluator");
}

} //end namespace salsa
