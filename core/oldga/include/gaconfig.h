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

#ifndef GACONFIG_H
#define GACONFIG_H

/*! \file
 *  \brief This file contains the common type defitions used on the whole framework
 *
 *  Details...
 *
 *
 */

// FARSA_GA_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef FARSA_GA_BUILDING_DLL
		#define FARSA_GA_API __declspec(dllexport)
		#define FARSA_GA_TEMPLATE __declspec(dllexport)
	#else
		#define FARSA_GA_API __declspec(dllimport)
		#define FARSA_GA_TEMPLATE
	#endif
	#define FARSA_GA_INTERNAL
#else
	#define FARSA_GA_API
	#define FARSA_GA_TEMPLATE
	#define FARSA_GA_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

namespace farsa {

extern bool FARSA_GA_API initGALib();
static const bool GALibInitializer = initGALib();

} // end namespace farsa

//--- read it as: x.yy.zz where
//---  x  is major version
//---  yy is minor version
//---  zz is patch version
//--- Example: version 2.4.6 is 20406
//--- Actual Version: 0.1.0
#define FARSA_GA_VERSION 10405

#include <QtDebug>

#endif
