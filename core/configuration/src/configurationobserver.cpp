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

#include "configurationobserver.h"
#include "configurationmanager.h"
#include "component.h"

namespace salsa {

ConfigurationObserver::ConfigurationObserver()
	: m_manager(NULL)
{
}

ConfigurationObserver::~ConfigurationObserver()
{
	stopObserving();
}

void ConfigurationObserver::observe(ConfigurationManager* manager)
{
	if (m_manager != manager) {
		stopObserving();
	}

	if (manager != NULL) {
		m_manager = manager;

		m_manager->addObserver(this);
	}
}

void ConfigurationObserver::stopObserving()
{
	if (m_manager != NULL) {
		m_manager->removeObserver(this);
		m_manager = NULL;
	}
}

void ConfigurationObserver::observedDestroyed()
{
	m_manager = NULL;
}

} // end namespace salsa
