/********************************************************************************
 *  SALSA Experimentes Library                                                  *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#include "evoalgo.h"
#include "genotypetophenotype.h"
#include "gaevaluator.h"
#include "configurationmanager.h"
#include "configurationhelper.h"

namespace salsa {

#warning VALUTARE SE GenotypesPreparer E GAEvaluator DEVONO ESSERE COMPONENTI O NO E SE QUESTA CLASSE DEVE DISTRUGGERLI O NO. PIÙ IN GENERALE LE DUE COSE SONO LEGATE: FORSE SI PUÒ DIRE CHE SI DISTRUGGONO SOLO I Component E CHE CHI PRENDE UN OGGETTO COME ISTANZA DI UNA SOTTOCLASSE DI Component (INVECE CHE COME ISTANZA DI UNA INTERFACCIA) DEVE DISTRUGGERLO

EvoAlgo::EvoAlgo(ConfigurationManager& params)
	: Component(params)
	, FlowControlled()
	, m_rng()
	, m_genotypeToPhenotype()
	, m_gaEvaluator()
{
}

EvoAlgo::~EvoAlgo()
{
}

void EvoAlgo::configure()
{
	setSeed(ConfigurationHelper::getInt(configurationManager(), confPath() + "seed"));
	m_genotypeToPhenotype.reset(configurationManager().getComponentFromParameter<GenotypeToPhenotype>(confPath() + "genotypeToPhenotype", false));
	m_gaEvaluator.reset(configurationManager().getComponentFromParameter<GAEvaluator>(confPath() + "gaEvaluator", false));
}

void EvoAlgo::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("The base class of genetic Algorithms" );

	d.describeInt("seed").def(1).limits(0, MaxInteger).help("The seed of the random number generator");
	d.describeComponent("genotypeToPhenotype").props(ParamIsMandatory).componentType("GenotypeToPhenotype").help("The component preparing genotypes for evaluation", "Set this to the group of the component that prepares the genotypes for evaluation (i.e. that performs the genotype-to-phenotype mapping).");
	d.describeComponent("gaEvaluator").props(ParamIsMandatory).componentType("GAEvaluator").help("The component evaluating genotypes", "Set this to the group of the component that evaluates the genotypes.");
}

void EvoAlgo::setSeed(unsigned int seed)
{
	const unsigned int oldSeed = seed;
	m_rng.setSeed(seed);
	seedChanged(oldSeed);
}

unsigned int EvoAlgo::seed() const
{
	return m_rng.seed();
}

bool EvoAlgo::resumeEvolution()
{
	return false;
}

void EvoAlgo::seedChanged(unsigned int)
{
}

} // end namespace salsa
