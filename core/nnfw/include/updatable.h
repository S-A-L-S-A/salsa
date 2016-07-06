/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef UPDATABLE_H
#define UPDATABLE_H

#include "nnfwconfig.h"
#include "configurationmanager.h"

namespace farsa {

/*! \brief Updatables objects
 *
 *  The Updatable objects has a name.
 */
class FARSA_NNFW_API Updatable : public Component {
public:
	/*! Constructor */
	Updatable( ConfigurationManager& params, QString prefix, Component* parent = NULL );
	/*! Destructor */
	virtual ~Updatable();
	/*! Update the object */
	virtual void update() = 0;
	/*! Set the name of Updatable */
	void setName( QString newname );
	/*! Return its name */
	QString name() const;
	/**
	 * \brief Save the actual status of parameters into the ConfigurationManager
	 *
	 * This saves the name property, remember to call this in child classes
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
private:
	//! The name of the Updatable
	QString namev;
};

}

#endif
