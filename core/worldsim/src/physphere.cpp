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

#include "physphere.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "salsaglutils.h"

namespace salsa {

void PhySphere::calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
	const wVector rds(sharedData->radius, sharedData->radius, sharedData->radius);
	minPoint = tm.w_pos + sharedData->collisionShapeOffset.w_pos - rds;
	maxPoint = tm.w_pos + sharedData->collisionShapeOffset.w_pos + rds;
}

wVector PhySphere::calculateMBBSize(const Shared* sharedData)
{
	return wVector(2.0 * sharedData->radius, 2.0 * sharedData->radius, 2.0 * sharedData->radius);
}

PhySphere::PhySphere(World* world, SharedDataWrapper<Shared> shared, real radius, QString name, const wMatrix& tm)
	: PhyObject(world, shared, name, tm)
	, m_shared(shared)
{
	m_shared.getModifiableShared()->radius = radius;
}

PhySphere::~PhySphere()
{
	// Nothing to do here
}

void PhySphere::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonCollision* c = NewtonCreateSphere( m_worldPriv->world, m_shared->radius, m_shared->radius, m_shared->radius, 1, (collisionShapeOffset == nullptr) ? nullptr : &(*collisionShapeOffset)[0][0] );
	m_priv->collision = c;
	if (collisionShapeOffset != nullptr) {
		m_shared.getModifiableShared()->collisionShapeOffset = *collisionShapeOffset;
	}
	if (!onlyCollisionShape) {
		wMatrix initialTransformationMatrix = wMatrix::identity(); // The transformation matrix is set in other places
		m_priv->body = NewtonCreateBody( m_worldPriv->world, c, &initialTransformationMatrix[0][0] );
		//NewtonReleaseCollision( worldpriv->world, c );
		NewtonBodySetAutoSleep( m_priv->body, 0 );
		setMass( 1 );
		NewtonBodySetUserData( m_priv->body, this );
		NewtonBodySetLinearDamping( m_priv->body, 0.0 );
		wVector zero = wVector(0,0,0,0);
		NewtonBodySetAngularDamping( m_priv->body, &zero[0] );
		NewtonBodySetAutoSleep( m_priv->body, 0 );
		NewtonBodySetFreezeState( m_priv->body, 0 );
		// Sets the signal-wrappers callback
		NewtonBodySetTransformCallback( m_priv->body, (PhyObjectPrivate::setTransformHandler) );
		NewtonBodySetForceAndTorqueCallback( m_priv->body, (PhyObjectPrivate::applyForceAndTorqueHandler) );
	}
#endif
	changedMatrix();
}

RenderPhySphere::RenderPhySphere(const PhySphere* entity)
	: RenderPhyObject(entity)
{
}

RenderPhySphere::~RenderPhySphere()
{
	// Nothing do do here
}

void RenderPhySphere::render(const PhySphereShared* sharedData, GLContextAndData* contextAndData)
{
	glPushMatrix();
	GLUquadricObj *pObj;
	setupColorTexture(sharedData, contextAndData);
	wMatrix mtr = collisionShapeMatrix(sharedData);
	GLMultMatrix(&(mtr[0][0]));

	// Get a new Quadric off the stack
	pObj = gluNewQuadric();
	// Get a new Quadric off the stack
	gluQuadricTexture(pObj, true);
	gluSphere(pObj, sharedData->radius, 20, 20);
	gluDeleteQuadric(pObj);

	glPopMatrix();
}

} // end namespace salsa
