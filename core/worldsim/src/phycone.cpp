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

#include "phycone.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "farsaglutils.h"
#include "worldsimutils.h"

namespace farsa {

void PhyCone::calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning CHECK IF THIS WORKS CORRECTLY
#endif
	const wMatrix mtr = sharedData->collisionShapeOffset * tm;

	// To compute the AABB of the cone we first compute the AABB of the base and then merge it with the tip point
	// (which can be seen as a degenerated AABB). The u and v vectors of the base are equal to the y and z axes
	// of the local frame of reference
	const wVector u = mtr.y_ax.scale(sharedData->radius);
	const wVector v = mtr.z_ax.scale(sharedData->radius);
	// Here we use minPoint and maxPoint directly because they will also be used in the merge as the output variables
	computeEllipseAABB(u, v, minPoint, maxPoint);

	const wVector tipPoint = mtr.x_ax.scale(sharedData->height);

	// Now merging the two AABBs
	mergeAABBs(minPoint, maxPoint, tipPoint, tipPoint);
}

wVector PhyCone::calculateMBBSize(const Shared* sharedData)
{
	return wVector(sharedData->height, sharedData->radius * 2.0, sharedData->radius * 2.0);
}

PhyCone::PhyCone(World* world, SharedDataWrapper<Shared> shared, real radius, real height, QString name, const wMatrix& tm)
	: PhyObject(world, shared, name, tm)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();
	d->radius = radius;
	if (height < (2.0 * radius)) {
		d->height = 2.0 * radius;
	} else {
		d->height = height;
	}
}

PhyCone::~PhyCone()
{
	// Nothing to do here
}

void PhyCone::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonCollision* c = NewtonCreateCone( m_worldPriv->world, m_shared->radius, m_shared->height, 1, (collisionShapeOffset == NULL) ? NULL : &(*collisionShapeOffset)[0][0] );
	m_priv->collision = c;
	if (collisionShapeOffset != NULL) {
		m_shared.getModifiableShared()->collisionShapeOffset = *collisionShapeOffset;
	}
	if (!onlyCollisionShape) {
		wMatrix initialTransformationMatrix = wMatrix::identity(); // The transformation matrix is set in other places
		m_priv->body = NewtonCreateBody( m_worldPriv->world, c, &initialTransformationMatrix[0][0] );
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

RenderPhyCone::RenderPhyCone(const PhyCone* entity)
	: RenderPhyObject(entity)
{
}

RenderPhyCone::~RenderPhyCone()
{
	// Nothing to do here
}

void RenderPhyCone::render(const PhyConeShared* sharedData, GLContextAndData* contextAndData)
{
	glPushMatrix();
	GLUquadricObj *pObj;
	setupColorTexture(sharedData, contextAndData);

	// opengl cylinder are aligned alogn the z axis, we want it along the x axis,
	// we create a rotation matrix to do the alignment
	wMatrix matrix = wMatrix::yaw( PI_GRECO * 0.5 );
	matrix.w_pos = matrix.rotateVector( wVector(0.0, 0.0, -sharedData->height*0.5) );
	matrix = matrix * collisionShapeMatrix(sharedData);
	GLMultMatrix(&matrix[0][0]);

	// Get a new Quadric off the stack
	pObj = gluNewQuadric();
	gluQuadricTexture(pObj, true);
	gluCylinder(pObj, sharedData->radius, 0, sharedData->height, 20, 2);

	// render the caps
	gluQuadricOrientation(pObj, GLU_INSIDE);
	gluDisk(pObj, 0.0f, sharedData->radius, 20, 1);

	gluDeleteQuadric(pObj);

	glPopMatrix();
}

} // end namespace farsa
