/*******************************************************************************
 * SALSA New Genetic Algorithm Library                                        *
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

#include "genotypetester.h"
#include "configurationhelper.h"

namespace salsa {

SingleGenotypeFloatToEvonet::SingleGenotypeFloatToEvonet(ConfigurationParameters& params, QString prefix)
	: SingleGenotypeTester(params, prefix)
	, m_gae(nullptr)
	, m_evonet(nullptr)
{
	m_gae = params.getObjectFromParameter<RobotExperiment>(prefix + QString("gaEvaluator"));
	if (params.getValue(prefix + "NET/netFile").isEmpty()) {
		const int nInputs = m_gae->getRequestedSensors();
		const int nOutputs = m_gae->getRequestedMotors();
		params.createParameter(prefix + "NET", "nSensors", QString::number(nInputs));
		params.createParameter(prefix + "NET", "nMotors", QString::number(nOutputs));
	}
	m_evonet = params.getObjectFromGroup<Evonet>(prefix + QString("/NET"));
}

SingleGenotypeFloatToEvonet::~SingleGenotypeFloatToEvonet()
{
	delete m_evonet;
	delete m_gae;
}

void SingleGenotypeFloatToEvonet::describe(QString type)
{
	Descriptor d = addTypeDescription(type, "single genotype float to evonet mapping");
	d.describeObject("gaEvaluator").type("RobotExperiment").props(IsMandatory).help("Object that calculate the fitness");
	d.describeSubgroup("NET").type("Evonet").props(IsMandatory).help("Object containing the neural network");
}

void SingleGenotypeFloatToEvonet::setGenotype(Genotype* g)
{
	const int genSize = m_evonet->freeParameters();
	// Dynamic cast from Genotype to GenotypeFloat
	GenotypeFloat* gf = dynamic_cast<GenotypeFloat*>(g);
	float* gen = new float[genSize];
	for (int i = 0; i < genSize; i++)
	{
		gen[i] = gf->getGene(i);
	}
	m_evonet->setParameters(gen);
	m_gae->setNeuralNetwork(m_evonet);
}

int SingleGenotypeFloatToEvonet::requestedGenotypeLength() const
{
	return m_evonet->freeParameters();
}

SingleGenotypeIntToEvonet::SingleGenotypeIntToEvonet(ConfigurationParameters& params, QString prefix)
	: SingleGenotypeTester(params, prefix)
	, m_gae(nullptr)
	, m_evonet(nullptr)
{
	// HERE WE SHOULD HAVE OneNeuralNetworkExperimentInput INSTEAD OF RobotExperiment, BUT THE FORMER IS NOT A SUBCLASS OF ParameterSettable (IT CAN'T BE BECAUSE IT IS USED WITH MULTIPLE INHERITANCE)!!! 
	m_gae = params.getObjectFromParameter<RobotExperiment>(prefix + QString("gaEvaluator"));
	if (params.getValue(prefix + "NET/netFile").isEmpty()) {
		const int nInputs = m_gae->getRequestedSensors();
		const int nOutputs = m_gae->getRequestedMotors();
		params.createParameter(prefix + "NET", "nSensors", QString::number(nInputs));
		params.createParameter(prefix + "NET", "nMotors", QString::number(nOutputs));
	}
	m_evonet = params.getObjectFromGroup<Evonet>(prefix + QString("/NET"));
}

SingleGenotypeIntToEvonet::~SingleGenotypeIntToEvonet()
{
	delete m_evonet;
	delete m_gae;
}

void SingleGenotypeIntToEvonet::describe(QString type)
{
	Descriptor d = addTypeDescription(type, "single integer genotype to evonet mapping");
	d.describeObject("gaEvaluator").type("RobotExperiment").props(IsMandatory).help("Object that calculate the fitness");
	d.describeSubgroup("NET").type("Evonet").props(IsMandatory).help("Object containing the neural network");
}

void SingleGenotypeIntToEvonet::setGenotype(Genotype* g)
{
	const int genSize = m_evonet->freeParameters();
	// Dynamic cast from Genotype to GenotypeFloat
	GenotypeInt* gf = dynamic_cast<GenotypeInt*>(g);
	int* gen = new int[genSize];
	for (int i = 0; i < genSize; i++)
	{
		gen[i] = gf->getGene(i);
	}
	m_evonet->setParameters(gen);
	m_gae->setNeuralNetwork(m_evonet);
}

int SingleGenotypeIntToEvonet::requestedGenotypeLength() const
{
	return m_evonet->freeParameters();
}

} // end namespace salsa
