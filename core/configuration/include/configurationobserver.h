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

#ifndef CONFIGURATION_OBSERVER_H
#define CONFIGURATION_OBSERVER_H

#include "configurationconfig.h"
#include "configurationmanager.h"
#include <QString>

namespace salsa {

class Component;

/**
 * \brief This is the interface to use for classes that want to keep track of
 *        operations done by ConfigurationManager
 *
 * Functions in this class are called when particular events happend in a
 * ConfigurationManager object. Each observer can only be attached to one
 * ConfigurationManager object at a time
 * \ingroup configuration_factory
 */
class SALSA_CONF_API ConfigurationObserver
{
public:
	/**
	 * \brief Constructor
	 */
	ConfigurationObserver();

	/**
	 * \brief Destructor
	 */
	virtual ~ConfigurationObserver();

	/**
	 * \brief Adds self to the list of observer of the given configuration
	 *        manger object
	 *
	 * If this observer was associated to another ConfigurationManager, the
	 * previous association is removed
	 * \param manager the configuration manger to observe. If nullptr, stops
	 *                observing
	 */
	void observe(ConfigurationManager* manager);

	/**
	 * \brief Removes self from the list of observers
	 */
	void stopObserving();

	/**
	 * \brief Returns the observed ConfigurationManager instance
	 *
	 * \return the observed ConfigurationManager instance
	 */
	const ConfigurationManager* observed() const
	{
		return m_manager;
	}

	/**
	 * \brief The function called when a component is created
	 *
	 * \param component the component that has been created
	 */
	virtual void onComponentCreation(Component* component) = 0;

	/**
	 * \brief The function called when a component has been destroyed
	 *
	 * This function is called by the destructor of the component, so by the
	 * time this is called, the component should not be used anymore
	 * \param component the component in that is being destroyed
	 */
	virtual void onComponentDestruction(Component* component) = 0;

private:
	// This is called by the configuration manager instance when it is
	// destroyed, so that we can remove the association
	void observedDestroyed();

	// The ConfigurationManager instance we observe
	ConfigurationManager* m_manager;

	// ConfigurationManager::SharedData is fried to call observedDestroyed()
	friend class ConfigurationManager::SharedData;
};

}

#endif
