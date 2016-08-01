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

#include "component.h"
#include "evoalgo.h"
#include "gaconfig.h"
#include "gaevaluator.h"
#include "genotypetophenotype.h"
#include "logger.h"
#include "typesdb.h"

namespace salsa {

bool SALSA_GA_API initGaLib()
{
	static bool calledOnce = false;

	if (calledOnce) {
		return true;
	}
	calledOnce = true;

	// Registering types
	TypesDB::instance().registerType<EvoAlgo>("EvoAlgo", QStringList() << "Component");
	TypesDB::instance().registerType<GAEvaluator>("GAEvaluator", QStringList());
	TypesDB::instance().registerType<GenotypeToPhenotype>("GenotypeToPhenotype", QStringList());

	Logger::info("Library Ga Initialized");
	return true;
}

} // end namespace salsa
