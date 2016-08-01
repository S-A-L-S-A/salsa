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

#ifndef NEWGACONFIG_H
#define NEWGACONFIG_H

/*! \file
 *  \brief This file contains the common type defitions used on the whole framework
 *
 *  Details...
 *
 *
 */

// SALSA_NEWGA_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef SALSA_NEWGA_BUILDING_DLL
		#define SALSA_NEWGA_API __declspec(dllexport)
		#define SALSA_NEWGA_TEMPLATE __declspec(dllexport)
	#else
		#define SALSA_NEWGA_API __declspec(dllimport)
		#define SALSA_NEWGA_TEMPLATE
	#endif
	#define SALSA_NEWGA_INTERNAL
#else
	#define SALSA_NEWGA_API
	#define SALSA_NEWGA_TEMPLATE
	#define SALSA_NEWGA_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

namespace salsa {

extern bool SALSA_NEWGA_API initNewGALib();
static const bool NewGALibInitializer = initNewGALib();

} // end namespace salsa

//--- read it as: x.yy.zz where
//---  x  is major version
//---  yy is minor version
//---  zz is patch version
//--- Example: version 2.4.6 is 20406
//--- Actual Version: 0.1.0
#define SALSA_NEWGA_VERSION 10405

#include <QtDebug>

#endif
