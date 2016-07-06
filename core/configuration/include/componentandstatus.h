/***************************************************************************
 *  FARSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Gianluca Massera <emmegian@yahoo.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#ifndef COMPONENT_AND_STATUS_H
#define COMPONENT_AND_STATUS_H

#include "configurationconfig.h"

namespace farsa {

class Component;

/**
 * \brief The enumeration with possible states of an object associated to a
 *        group created using the factory
 */
enum ComponentCreationStatus {
	ComponentNotCreated, /**< The group has no object currently associated */
	CreatingComponent, /**< The object is about to be created */
	ComponentCreatedNotConfigured, /**< The object has been created but has not
	                                 been configured */
	ConfiguringComponent, /**< The object has been created and is about to be
	                        configured */
	ComponentCreatedAndConfigured /**< The object has been created and
	                                configured */
};

/**
 * \brief The structure with the object for a node and its status
 */
struct FARSA_CONF_TEMPLATE ComponentAndStatus
{
	/**
	 * \brief Constructor
	 */
	ComponentAndStatus() :
		component(NULL),
		status(ComponentNotCreated)
	{
	}

	/**
	 * \brief Copy constructor
	 */
	ComponentAndStatus(const ComponentAndStatus& other) :
		component(other.component),
		status(other.status)
	{
	}

	/**
	 * \brief Copy operator
	 */
	ComponentAndStatus& operator=(const ComponentAndStatus& other)
	{
		if (this == &other) {
			return *this;
		}

		component = other.component;
		status = other.status;

		return *this;
	}

	/**
	 * \brief The component
	 */
	Component *component;

	/**
	 * \brief The status of the component
	 */
	ComponentCreationStatus status;
};

} // end namespace farsa

#endif
