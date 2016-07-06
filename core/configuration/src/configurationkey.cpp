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

#include "configurationkey.h"
#include "configurationhelper.h"
#include <memory>

namespace farsa {

ConfigurationKey::ConfigurationKey()
	: QString()
{
}

ConfigurationKey::ConfigurationKey(const QString& name)
	: QString(name)
{
}

ConfigurationKey::ConfigurationKey(const ConfigurationKey& other)
	: QString(other)
{
}

ConfigurationKey& ConfigurationKey::operator=(const ConfigurationKey& other)
{
	if (&other == this) {
		return *this;
	}

	QString::operator=(other);

	return *this;
}

bool ConfigurationKey::operator==(const ConfigurationKey& other) const
{
	return ConfigurationHelper::configKeysEqual(*this, other);
}

bool ConfigurationKey::operator!=(const ConfigurationKey& other) const
{
	return !(*this == other);
}

bool ConfigurationKey::operator<(const ConfigurationKey& other) const
{
	return ConfigurationHelper::configKeysLessThan(*this, other);
}

}
