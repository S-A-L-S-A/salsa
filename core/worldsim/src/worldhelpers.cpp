/********************************************************************************
 *  SALSA                                                                       *
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

#include "worldhelpers.h"
#include "world.h"
#include "phyjoint.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "motorcontrollers.h"
#include "logger.h"
#include <QPair>

namespace salsa {

MaterialDB::MaterialDB( World* w ) : mats(), pmap() {
	this->worldv = w;
}

void MaterialDB::createInitialMaterials()
{
	createMaterial( "nonCollidable" );
	createMaterial( "default" );
}

bool MaterialDB::createMaterial( QString name ) {
	if ( mats.contains( name ) ) {
		return false;
	}
#ifdef WORLDSIM_USE_NEWTON
	NewtonWorld* ngdWorld = worldv->m_priv->world;
	int newm;
	if ( name == "default" ) {
		newm = NewtonMaterialGetDefaultGroupID( ngdWorld );
	} else {
		newm = NewtonMaterialCreateGroupID( ngdWorld );
	}
	worldv->m_priv->matIDs[name] = newm;
	NewtonMaterialSetCollisionCallback( ngdWorld, newm, newm, (void*)worldv, NULL, (WorldPrivate::processCollisionHandler) );
	// --- setting callbacks
	foreach( QString k, mats.values() ) {
		int kid = worldv->m_priv->matIDs[k];
		NewtonMaterialSetCollisionCallback( ngdWorld, newm, kid, (void*)worldv, NULL, (WorldPrivate::processCollisionHandler) );
	}
	// --- setting nonCollidable material
	NewtonMaterialSetDefaultCollidable( ngdWorld, worldv->m_priv->matIDs["nonCollidable"], newm, 0 );
#endif
	mats.insert( name );
	return true;
}

void MaterialDB::setFrictions( QString mat1, QString mat2, real st, real kn ) {
	if ( !mats.contains( mat1 ) ) return;
	if ( !mats.contains( mat2 ) ) return;
	QString pkey = createKey( mat1, mat2 );
	pmap[pkey].staticFriction = st;
	pmap[pkey].dynamicFriction = kn;
#ifdef WORLDSIM_USE_NEWTON
	int id1 = worldv->m_priv->matIDs[mat1];
	int id2 = worldv->m_priv->matIDs[mat2];
	NewtonMaterialSetDefaultFriction( worldv->m_priv->world, id1, id2, st, kn );
#endif
	return;
}

void MaterialDB::setElasticity( QString mat1, QString mat2, real el ) {
	if ( !mats.contains( mat1 ) ) return;
	if ( !mats.contains( mat2 ) ) return;
	QString pkey = createKey( mat1, mat2 );
	pmap[pkey].elasticity = el;
#ifdef WORLDSIM_USE_NEWTON
	int id1 = worldv->m_priv->matIDs[mat1];
	int id2 = worldv->m_priv->matIDs[mat2];
	NewtonMaterialSetDefaultElasticity( worldv->m_priv->world, id1, id2, el );
#endif
	return;
}

void MaterialDB::setSoftness( QString mat1, QString mat2, real sf ) {
	if ( !mats.contains( mat1 ) ) return;
	if ( !mats.contains( mat2 ) ) return;
	QString pkey = createKey( mat1, mat2 );
	pmap[pkey].softness = sf;
#ifdef WORLDSIM_USE_NEWTON
	int id1 = worldv->m_priv->matIDs[mat1];
	int id2 = worldv->m_priv->matIDs[mat2];
	NewtonMaterialSetDefaultSoftness( worldv->m_priv->world, id1, id2, sf );
#endif
	return;
}

void MaterialDB::setGravitationalAcceleration( QString mat, real force ) {
	gravities[mat] = force;
}

real MaterialDB::gravitationalAcceleration( QString mat ) {
	if ( gravities.contains( mat ) ) {
		return gravities[mat];
	} else {
		return worldv->gravitationalAcceleration();
	}
}


void MaterialDB::enableCollision( QString mat1, QString mat2, bool enable ) {
	if ( !mats.contains( mat1 ) ) return;
	if ( !mats.contains( mat2 ) ) return;
	QString pkey = createKey( mat1, mat2 );
	pmap[pkey].collisions = enable;
#ifdef WORLDSIM_USE_NEWTON
	int id1 = worldv->m_priv->matIDs[mat1];
	int id2 = worldv->m_priv->matIDs[mat2];
	NewtonMaterialSetDefaultCollidable( worldv->m_priv->world, id1, id2, enable );
#endif
}

void MaterialDB::setProperties( QString mat1, QString mat2, real fs, real fk, real el, real sf, bool en ) {
	setFrictions( mat1, mat2, fs, fk );
	setElasticity( mat1, mat2, el );
	setSoftness( mat1, mat2, sf );
	enableCollision( mat1, mat2, en );
}

QString MaterialDB::createKey( QString mat1, QString mat2 ) {
	if ( mat1 < mat2 ) {
		return mat1 + ":" + mat2;
	} else {
		return mat2 + ":" + mat1;
	}
}

} // end namespace salsa
