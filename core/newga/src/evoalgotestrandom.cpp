/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#include "evoalgotestrandom.h"
#include "configurationhelper.h"

namespace farsa {

EvoAlgoTestRandom::EvoAlgoTestRandom()
	: EvoAlgoTest()
{
}

void EvoAlgoTestRandom::configure(ConfigurationParameters& params, QString prefix)
{
	EvoAlgoTest::configure(params, prefix);
}

void EvoAlgoTestRandom::describe( QString type )
{
	EvoAlgoTest::describe(type);
	Descriptor d = addTypeDescription( type, "Random test" );
}

void EvoAlgoTestRandom::runTest()
{
	Logger::info("Start test");
	Genotype* g = getPrototype()->clone();
	GenotypeFloat* gf;
	GenotypeInt* gi;
	if ((gf = dynamic_cast<GenotypeFloat*>(g)) != NULL)
	{
		const float minVal = -5.0;
		const float maxVal = 5.0;
		gf->initRandom(minVal, maxVal);
	}
	else if ((gi = dynamic_cast<GenotypeInt*>(g)) != NULL)
	{
		const int minVal = 0;
		const int maxVal = 256;
		gi->initRandom(minVal, maxVal);
	}
	else
	{
		farsa::Logger::error("Invalid genotype type");
		return;
	}
	getGenotypeTester()->setGenotype(g);
	getGAEvaluator()->evaluate();
	Logger::info("End test");
}

} //end namespace farsa
