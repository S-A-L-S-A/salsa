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

#include "evoalgotestcurrent.h"
#include "configurationhelper.h"

namespace farsa {

EvoAlgoTestCurrent::EvoAlgoTestCurrent()
	: EvoAlgoTest()
{
}

void EvoAlgoTestCurrent::configure(ConfigurationParameters& params, QString prefix)
{
	EvoAlgoTest::configure(params, prefix);
}

void EvoAlgoTestCurrent::describe( QString type )
{
	EvoAlgoTest::describe(type);
	Descriptor d = addTypeDescription( type, "Current test" );
}

void EvoAlgoTestCurrent::runTest()
{
	Logger::info("Start test");
	getGAEvaluator()->evaluate();
	Logger::info("End test");
}

} //end namespace farsa
