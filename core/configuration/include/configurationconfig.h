/********************************************************************************
 *  SALSA Configuration Library                                                 *
 *  Copyright (C) 2007-2014                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#ifndef CONFIGURATIONCONFIG_H
#define CONFIGURATIONCONFIG_H

#include "salsaversion.h"

// SALSA_CONF_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef SALSA_CONF_BUILDING_DLL
		#define SALSA_CONF_API __declspec(dllexport)
		#define SALSA_CONF_TEMPLATE __declspec(dllexport)
	#else
		#define SALSA_CONF_API __declspec(dllimport)
		#define SALSA_CONF_TEMPLATE
	#endif
	#define SALSA_CONF_INTERNAL
#else
	#define SALSA_CONF_API
	#define SALSA_CONF_TEMPLATE
	#define SALSA_CONF_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

namespace salsa {

extern bool SALSA_CONF_API initConfigurationLib();
static const bool configurationLibInitializer = initConfigurationLib();

/**
 * \brief The character used to split path in groups
 *
 * \note DON'T change to a character that changes between upper and
 *       lower case
 */
extern const char* GroupSeparator;

/**
 * \brief The sequence used to indicate the parent group
 *
 * \note DON'T change to a character that changes between upper and
 *       lower case
 */
extern const char* ParentGroup;

} // end namespace salsa

#include <QtDebug>

#endif
