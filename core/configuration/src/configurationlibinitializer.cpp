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

#include "configurationmanager.h"
#include "private/inifilesupport.h"
#include "private/xmlfilesupport.h"
#include "configurationconfig.h"
#include "typesdb.h"

namespace farsa {

bool FARSA_CONF_API initConfigurationLib()
{
	static bool alreadyCalled = false;
	if (alreadyCalled) {
		return true;
	}

	ConfigurationManager::registerFileFormat("ini", new IniFileLoaderSaver, "ini");
	ConfigurationManager::registerFileFormat("XML", new XMLFileLoaderSaver, "xml");

	alreadyCalled = true;

	return true;
}

const char* GroupSeparator = "/";
const char* ParentGroup = "..";

} // end namespace farsa
