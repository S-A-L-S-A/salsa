/********************************************************************************
 *  SALSA Experiments Library                                                   *
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

#ifndef SALSAPLUGIN_H
#define SALSAPLUGIN_H

#include "experimentsconfig.h"
#include <QObject>
#include <QMap>
#include <QMetaObject>
#include <QString>
#include <QStringList>
#include <QtPlugin>

// The IID of plugins
#define SalsaPlugin_IID "it.cnr.istc.laral.salsa.plugin/2.0"

/**
 * \brief The interface to implement in a SALSA plugin
 *
 * A child of this class is generated automatically in a plugin by the
 * salsapluginhelper binary
 * \ingroup experiments_utils
 */
class SALSA_EXPERIMENTS_TEMPLATE SalsaPlugin
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~SalsaPlugin()
	{
	}

	/**
	 * \brief Register types
	 */
	virtual void registerTypes() = 0;

	/**
	 * \brief Returns the list of plugins on which the current one depends
	 *
	 * \return the list of plugins on which the current one depends
	 */
	virtual QStringList getDependencies()
	{
		return QStringList();
	}
};

Q_DECLARE_INTERFACE(SalsaPlugin, SalsaPlugin_IID);

// Declaring some helper macros, to avoid depending on QT macros directly
#define SALSA_PLUGIN_METADATA(...) Q_PLUGIN_METADATA(__VA_ARGS__)

// More macros for the new structure of plugins
#ifdef WIN32
	#define SALSA_PLUGIN_API __declspec(dllexport)
	#define SALSA_PLUGIN_TEMPLATE __declspec(dllexport)
	#define SALSA_PLUGIN_INTERNAL

	#define SALSA_PLUGIN_API_IMPORT __declspec(dllimport)
	#define SALSA_PLUGIN_TEMPLATE_IMPORT
	#define SALSA_PLUGIN_INTERNAL_IMPORT
#else
	#define SALSA_PLUGIN_API
	#define SALSA_PLUGIN_TEMPLATE
	#define SALSA_PLUGIN_INTERNAL __attribute__ ((visibility ("hidden")))

	#define SALSA_PLUGIN_API_IMPORT
	#define SALSA_PLUGIN_TEMPLATE_IMPORT
	#define SALSA_PLUGIN_INTERNAL_IMPORT __attribute__ ((visibility ("hidden")))
#endif

// The next macros are empty because they are only used by the salsapluginhelper
// preprocessor. SALSA_REGISTER_CLASS must be used at the beginning of classes
// (both components and interfaces) to register. It must be at the beginning of
// the class (like the Q_OBJECT macro). The SALSA_PRE_REGISTRATION_FUNCTION and
// SALSA_POST_REGISTRATION_FUNCTION macros allow to register functions that
// should be called respectively before or after the registration proceduce. The
// SALSA_NR macro can be used to prevent a class from being included among the
// registered parents. For example, if you have:
// 	class MyClass : public P1, public SALSA_NR(P2), private P3
// only P1 will be registered, because P3 is private and P2 is surrounded by the
// SALSA_NR macro
#define SALSA_REGISTER_CLASS
#define SALSA_PRE_REGISTRATION_FUNCTION(...)
#define SALSA_POST_REGISTRATION_FUNCTION(...)
#define SALSA_NR(parent_class) parent_class

#endif
