/********************************************************************************
 *  FARSA                                                                       *
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

#ifndef WORLDSIMCONFIG_H
#define WORLDSIMCONFIG_H

// FARSA_WSIM_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#ifndef NOMINMAX
		//--- the min and max macros defined by window will conflicts with functions defined in YARP_math
		#define NOMINMAX
	#endif
	#ifdef FARSA_WSIM_BUILDING_DLL
		#define FARSA_WSIM_API __declspec(dllexport)
		#define FARSA_WSIM_TEMPLATE __declspec(dllexport)
	#else
		#define FARSA_WSIM_API __declspec(dllimport)
		#define FARSA_WSIM_TEMPLATE
	#endif
	#define FARSA_WSIM_INTERNAL
#else
	#define FARSA_WSIM_API
	#define FARSA_WSIM_TEMPLATE
	#define FARSA_WSIM_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

extern bool FARSA_WSIM_API initWorldSimLib();
static const bool worldSimInitializer = initWorldSimLib();

//--- read it as: x.yy.zz where
//---  x  is major version
//---  yy is minor version
//---  zz is patch version
//--- Example: version 2.4.6 is 20406
#define FARSA_WSIM_VERSION 0x020000

#endif
