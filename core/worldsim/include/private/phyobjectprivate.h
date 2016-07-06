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

#ifndef PHYOBJECT_PRIVATE_H
#define PHYOBJECT_PRIVATE_H

#include <QDebug>

#include "private/worldprivate.h"

#ifdef WORLDSIM_USE_NEWTON
#include "Newton.h"

namespace farsa {

class FARSA_WSIM_INTERNAL PhyObjectPrivate {
public:
	PhyObjectPrivate()
		: body(NULL)
		, collision(NULL)
	{
	}

	NewtonBody* body;
	NewtonCollision* collision;

	static void setTransformHandler(const NewtonBody* body, const dFloat* matrix, int) {
		PhyObject* obj = (PhyObject*)NewtonBodyGetUserData( body );
		wMatrix m;
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI SI POTREBBE USARE memcpy PER VELOCIZZARE LA COPIA, MA CONTROLLARE CHE TUTTO FUNZIONI (E CHE CI SIA UN EFFETTIVO MIGLIORAMENTO DI PRESTAZIONI)!!!
#endif
		real* ptr = &(m.x_ax[0]);
		for( int i=0; i<16; i++ ) {
			ptr[i] = matrix[i];
		}
		//--- direct access to protected member tm, because
		//    calling of setMatrix will call NetwonBodySetMatrix resulting in spurios matrix settings
		PhyObject::Shared* d = obj->m_shared.getModifiableShared();
		d->tm = m;
		//obj->setMatrix( m );
	}

	static void applyForceAndTorqueHandler(const NewtonBody* body, dFloat timestep, int) {
		UNUSED_PARAM( timestep );
		PhyObject* obj = (PhyObject*)NewtonBodyGetUserData( body );
		PhyObject::Shared* d = obj->m_shared.getModifiableShared();

		// apply the gravity force
		wVector mi = obj->massInertiaVec();
		real gforce = obj->world()->materials().gravitationalAcceleration( obj->material() );
		wVector force( 0.0f, 0.0f, mi[0]*gforce );
		// apply the force accumulated
		force += d->forceAcc;
		NewtonWorldCriticalSectionLock( obj->m_worldPriv->world );
		NewtonBodySetForce( body, &force[0] );
		// apply the torque accumulated
		NewtonBodySetTorque( body, &(d->torqueAcc[0]) );
		NewtonWorldCriticalSectionUnlock( obj->m_worldPriv->world );
		// reset accumulators
		d->forceAcc = wVector(0,0,0);
		d->torqueAcc= wVector(0,0,0);
	}
};

} // use namespace farsa

#endif


#endif
