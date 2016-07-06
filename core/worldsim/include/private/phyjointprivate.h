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

#ifndef PHYJOINT_PRIVATE_H
#define PHYJOINT_PRIVATE_H

#ifdef WORLDSIM_USE_NEWTON
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "Newton.h"

namespace farsa {

class FARSA_WSIM_INTERNAL PhyJointPrivate {
public:
	NewtonJoint* joint;
	NewtonBody* parent;
	NewtonBody* child;

	/*! \param userJoint is the NewtonJoint to control
	 *  \param timestep is the timestep of which the joint should be advanced
	 *  \param int is the thread identifier, used when multi-thread is on
	 */
	static void userBilateralHandler(const NewtonJoint* userJoint, dFloat timestep, int) {
		PhyJoint* joint = (PhyJoint*)(NewtonJointGetUserData(userJoint));
		NewtonWorldCriticalSectionLock( joint->m_worldPriv->world );
		joint->updateJoint( timestep );
		NewtonWorldCriticalSectionUnlock( joint->m_worldPriv->world );
	};
	static void userBilateralGetInfoHandler(const NewtonJoint* userJoint, NewtonJointRecord* info) {
		UNUSED_PARAM( userJoint );
		UNUSED_PARAM( info );
		/* this is only to retrieve internal information about the joint, useful to save/restore joints */
		//--- not used at the moment
	};
};

} // end namespace farsa

#endif

#endif
