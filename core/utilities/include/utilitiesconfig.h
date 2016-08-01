/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2008 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef UTILITIESCONFIG_H
#define UTILITIESCONFIG_H

// SALSA_UTIL_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef SALSA_UTIL_BUILDING_DLL
		#define SALSA_UTIL_API __declspec(dllexport)
		#define SALSA_UTIL_TEMPLATE __declspec(dllexport)
	#else
		#define SALSA_UTIL_API __declspec(dllimport)
		#define SALSA_UTIL_TEMPLATE
	#endif
	#define SALSA_UTIL_INTERNAL
#else
	#define SALSA_UTIL_API
	#define SALSA_UTIL_TEMPLATE
	#define SALSA_UTIL_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

//--- read it as: x.yy.zz where
//---  x  is major version
//---  yy is minor version
//---  zz is patch version
//--- Example: version 2.4.6 is 20406
#define SALSA_UTIL_VERSION 0x020000

namespace salsa {

extern bool SALSA_UTIL_API initUtilitiesLib();
static const bool utilitiesLibInitializer = initUtilitiesLib();

} // end namespace salsa

/*! Useful for avoid warning when you don't use parameters */
#define UNUSED_PARAM( a ) ( (void) a );

#endif
