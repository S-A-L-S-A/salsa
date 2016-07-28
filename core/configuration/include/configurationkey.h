/***************************************************************************
 *  SALSA Configuration Library                                            *
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

#ifndef CONFIGURATION_KEY_H
#define CONFIGURATION_KEY_H

#include "configurationconfig.h"
#include <QString>

namespace salsa {

/**
 * \brief The class modelling the name of a group or property
 *
 * We use this instead of QString because this has the < and == operator
 * overloaded to respect ordering rules described in the documentation
 * of ConfigurationManager. This is only used internally, the public APIs use
 * QString directly
 *
 * \internal
 */
class ConfigurationKey : public QString
{
public:
	/**
	 * \brief Constructor
	 *
	 * This sets the name to the empty string
	 */
	ConfigurationKey();

	/**
	 * \brief Constructor
	 *
	 * \param name the name of this key
	 */
	ConfigurationKey(const QString& name);

	/**
	 * \brief Copy constructor
	 *
	 * \param other the object to copy
	 */
	ConfigurationKey(const ConfigurationKey& other);

	/**
	 * \brief Copy operator
	 *
	 * \param other the object to copy
	 */
	ConfigurationKey& operator=(const ConfigurationKey& other);

	/**
	 * \brief Equality operator
	 *
	 * \param other the object to which we are compared
	 * \return true if this and other are the same
	 */
	bool operator==(const ConfigurationKey& other) const;

	/**
	 * \brief Disequality operator
	 *
	 * \param other the object to which we are compared
	 * \return true if this and other are different
	 */
	bool operator!=(const ConfigurationKey& other) const;

	/**
	 * \brief Less-than operator
	 *
	 * \param other the object to which we are compared
	 * \return true if this is less than other
	 */
	bool operator<(const ConfigurationKey& other) const;
};

}

#endif
