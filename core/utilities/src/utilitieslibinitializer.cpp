/***************************************************************************
 *   Copyright (C) 2008 by Tomassino Ferrauto                              *
 *   t_ferrauto@yahoo.it                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "utilitiesconfig.h"
#include "randomgenerator.h"
#include "logger.h"

namespace farsa {

/*! inizializzazione di globalRND al caricamento della libreria
 *  \todo it would be possible to automatically load/save the status of globalRNG in order
 *   to continue an interrupted random sequence
 */
#warning THIS SHOULD BE REMOVED, IT IS NOT THREAD-SAFE
FARSA_UTIL_API RandomGenerator* globalRNG;

bool FARSA_UTIL_API initUtilitiesLib()
{
	static bool calledOnce = false;

	if (calledOnce) {
		return true;
	}
	calledOnce = true;

	globalRNG = new RandomGenerator();

	Logger::info( "Library Utilities Initialized" );
	return true;
}

} // end namespace farsa
