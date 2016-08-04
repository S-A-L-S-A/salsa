/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2005-2014 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef GACONFIG_H
#define GACONFIG_H

#include "salsaversion.h"

// SALSA_GA_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef SALSA_GA_BUILDING_DLL
		#define SALSA_GA_API __declspec(dllexport)
		#define SALSA_GA_TEMPLATE __declspec(dllexport)
	#else
		#define SALSA_GA_API __declspec(dllimport)
		#define SALSA_GA_TEMPLATE
	#endif
	#define SALSA_GA_INTERNAL
#else
	#define SALSA_GA_API
	#define SALSA_GA_TEMPLATE
	#define SALSA_GA_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

namespace salsa {

extern bool SALSA_GA_API initGALib();
static const bool GALibInitializer = initGALib();

} // end namespace salsa

#include <QtDebug>

#endif
