/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
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

// FARSA_UTIL_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef FARSA_UTIL_BUILDING_DLL
		#define FARSA_UTIL_API __declspec(dllexport)
		#define FARSA_UTIL_TEMPLATE __declspec(dllexport)
	#else
		#define FARSA_UTIL_API __declspec(dllimport)
		#define FARSA_UTIL_TEMPLATE
	#endif
	#define FARSA_UTIL_INTERNAL
#else
	#define FARSA_UTIL_API
	#define FARSA_UTIL_TEMPLATE
	#define FARSA_UTIL_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

//--- read it as: x.yy.zz where
//---  x  is major version
//---  yy is minor version
//---  zz is patch version
//--- Example: version 2.4.6 is 20406
#define FARSA_UTIL_VERSION 0x020000

namespace farsa {

extern bool FARSA_UTIL_API initUtilitiesLib();
static const bool utilitiesLibInitializer = initUtilitiesLib();

} // end namespace farsa

/*! Useful for avoid warning when you don't use parameters */
#define UNUSED_PARAM( a ) ( (void) a );

#endif
