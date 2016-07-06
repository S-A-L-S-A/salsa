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

#ifndef WORLD_PRIVATE_H
#define WORLD_PRIVATE_H

#ifdef WORLDSIM_USE_NEWTON
#include "world.h"
#include "Newton.h"
#include <QHash>

namespace farsa {

class FARSA_WSIM_INTERNAL WorldPrivate {
public:
	NewtonWorld* world;
	QHash<QString, int> matIDs;

	static void processCollisionHandler( const NewtonJoint* contactJointList, dFloat timestep, int threadIndex ) {
		UNUSED_PARAM( timestep );
		UNUSED_PARAM( threadIndex );
		void* contact = NewtonContactJointGetFirstContact( contactJointList );
		NewtonMaterial* material = NewtonContactGetMaterial( contact );
		World* world = (World*)( NewtonMaterialGetMaterialPairUserData( material ) );
		NewtonWorldCriticalSectionLock( world->m_priv->world );
		for( ; contact; contact = NewtonContactJointGetNextContact( contactJointList, contact ) ) {
			NewtonMaterial* material = NewtonContactGetMaterial( contact );
			//--- check if the collision is disabled
			PhyObject* obj1 = (PhyObject*)(NewtonBodyGetUserData( NewtonJointGetBody0( contactJointList ) ) );
			PhyObject* obj2 = (PhyObject*)(NewtonBodyGetUserData( NewtonJointGetBody1( contactJointList ) ) );
			if ( world->m_nobjs.contains( qMakePair( obj1, obj2 ) ) ) {
				//--- the collision between objects was disabled
				NewtonContactJointRemoveContact( contactJointList, contact );
				continue;
			}
			//--- fill-up the rest of the data in pendent contact information
			wVector globalPos;
			wVector normal;
			wVector normalForce;
			// The second argument to the next call is NULL because it only influences the way the normal is computed
			// and here we discard the normal (see Newton source code, Newton.cpp from line 2013 on)
			NewtonMaterialGetContactPositionAndNormal( material, NULL, &globalPos[0], &normal[0] );
			Contact first, second;
			first.worldPos = globalPos;
			first.pos = obj1->matrix().untransformVector( globalPos );
			first.collide = obj2;
			second.worldPos = globalPos;
			second.pos = obj2->matrix().untransformVector( globalPos );
			second.collide = obj1;

			NewtonMaterialGetContactForce(material, NewtonJointGetBody0(contactJointList), &normalForce[0]);
			first.force = normalForce;
			NewtonMaterialGetContactForce(material, NewtonJointGetBody1(contactJointList), &normalForce[0]);
			second.force = normalForce;

			world->m_cmap[obj1].append( first );
			world->m_cmap[obj2].append( second );
		}
		NewtonWorldCriticalSectionUnlock( world->m_priv->world );
	}

	// An instance of this structure is passed to the NewtonWorldRayCast function
	// as user data (which will be then passed to our callback)
	struct WorldRayCastCallbackUserData {
		// Constructor
		WorldRayCastCallbackUserData(wVector s, wVector e, bool o, const QSet<PhyObject*>& i) :
			rayStart(s),
			rayEnd(e),
			onlyClosest(o),
			vector(),
			ignoredObjs(i)
		{
		}

		// The start point of the ray
		const wVector rayStart;

		// The end point of the ray
		const wVector rayEnd;

		// If true only the closest hit is returned
		const bool onlyClosest;

		// The vector with objects hit by the ray
		RayCastHitVector vector;

		// The set of objects to ignore
		const QSet<PhyObject*>& ignoredObjs;
	};

	// The NewtonWorldRayFilterCallback used by the NewtonWorldRayCast function
	static dFloat worldRayFilterCallback( const NewtonBody* body, const dFloat* hitNormal, int /*collisionID*/, void* userData, dFloat intersectParam )
	{
		// Casting userData to WorldRayCastCallbackUserData
		WorldRayCastCallbackUserData* data = (WorldRayCastCallbackUserData*) userData;

		// The object with information about this collision
		RayCastHit h;
		h.object = (PhyObject*) NewtonBodyGetUserData(body);

		// Before continuing to fill the object with information about the collision we have to check whether
		// the object is in the set of objects to ignore. If so, returning 1 to continue to check for collisions
		if (data->ignoredObjs.contains(h.object)) {
			return 1;
		}

		h.distance = intersectParam;
		h.position = data->rayStart + (data->rayEnd - data->rayStart).scale(intersectParam);
		h.normal.x = hitNormal[0];
		h.normal.y = hitNormal[1];
		h.normal.z = hitNormal[2];

		// If onlyClosest is true, we have to stop searching for collisions further that this one, so we
		// return intersectParam (Newton will not look for intersection points further than the
		// value returned by this function). If all intersections are requested, we return 1. Moreover, if
		// only the closest object is requested, we only insert the nearest collision into the vector
		if (data->onlyClosest) {
			if (data->vector.size() == 0) {
				data->vector.append(h);
			} else {
				// Checking if this collision is nearer than the one already in the vector
				if (h.distance < data->vector[0].distance) {
					data->vector[0] = h;
				}
			}

			return intersectParam;
		} else {
			// Adding the object with information about this collision to the vector of collisions
			data->vector.append(h);

			return 1;
		}
	}
};

} // end namespace farsa

#endif

#endif
