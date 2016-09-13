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

#include "phyellipsoid.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "salsaglutils.h"
#include <cmath>

namespace salsa {

void PhyEllipsoid::calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning CHECK IF THIS WORKS CORRECTLY
#endif
	const wMatrix mtr = sharedData->collisionShapeOffset * tm;

	// To compute the bounding box of an ellipsoid we follow instructions at
	// http://www.loria.fr/~shornus/ellipsoid-bbox.html (the demonstration of
	// how to affine-transform a normal vector is in
	// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.178.4810&rep=rep1&type=pdf )
	// Documentation is also in the salsa repository under miscDocumentation/Bounding Box Ellipsoid

	// The matrix transforming a unit sphere into the ellipsoid
	const wMatrix ellipsoidTransf(wVector(sharedData->radiusX, 0.0, 0.0, 0.0),
	                              wVector(0.0, sharedData->radiusY, 0.0, 0.0),
	                              wVector(0.0, 0.0, sharedData->radiusZ, 0.0),
	                              wVector(0.0, 0.0, 0.0, 1.0));

	// The matrix transforming the unit sphere into the ellipsoid in the current position and rotation
	const wMatrix M = ellipsoidTransf * mtr; // CHECK IF THIS OR THE CONTRARY

	// See documentation on why these are half the dimensions of the bounding box
	const real hX = sqrt(M.x_ax[0] * M.x_ax[0] + M.y_ax[0] * M.y_ax[0] + M.z_ax[0] * M.z_ax[0]);
	const real hY = sqrt(M.x_ax[1] * M.x_ax[1] + M.y_ax[1] * M.y_ax[1] + M.z_ax[1] * M.z_ax[1]);
	const real hZ = sqrt(M.x_ax[2] * M.x_ax[2] + M.y_ax[2] * M.y_ax[2] + M.z_ax[2] * M.z_ax[2]);
	const wVector h(hX, hY, hZ);

	minPoint = tm.w_pos - h;
	maxPoint = tm.w_pos + h;
}

wVector PhyEllipsoid::calculateMBBSize(const Shared* sharedData)
{
	return wVector(2.0 * sharedData->radiusX, 2.0 * sharedData->radiusY, 2.0 * sharedData->radiusZ);
}

PhyEllipsoid::PhyEllipsoid(World* world, SharedDataWrapper<Shared> shared, real radiusX, real radiusY, real radiusZ, QString name, const wMatrix& tm)
	: PhyObject(world, shared, name, tm)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();
	d->radiusX = radiusX;
	d->radiusY = radiusY;
	d->radiusZ = radiusZ;
}

PhyEllipsoid::~PhyEllipsoid()
{
	// Nothing to do here
}

void PhyEllipsoid::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonCollision* c = NewtonCreateSphere( m_worldPriv->world, m_shared->radiusX, m_shared->radiusY, m_shared->radiusZ, 1, (collisionShapeOffset == nullptr) ? nullptr : &(*collisionShapeOffset)[0][0] );
	m_priv->collision = c;
	if (collisionShapeOffset != nullptr) {
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

RenderPhyEllipsoid::RenderPhyEllipsoid(const PhyEllipsoid* entity)
	: RenderPhyObject(entity)
{
}

RenderPhyEllipsoid::~RenderPhyEllipsoid()
{
	// Nothing to do here
}

void RenderPhyEllipsoid::render(const PhyEllipsoidShared* sharedData, GLContextAndData* contextAndData)
{
	glPushMatrix();
	GLUquadricObj *pObj;
	setupColorTexture(sharedData, contextAndData);
	wMatrix mat = collisionShapeMatrix(sharedData);
	mat.x_ax = mat.x_ax.scale( sharedData->radiusX );
	mat.y_ax = mat.y_ax.scale( sharedData->radiusY );
	mat.z_ax = mat.z_ax.scale( sharedData->radiusZ );
	GLMultMatrix(&mat[0][0]);

	// Get a new Quadric off the stack
	pObj = gluNewQuadric();
	// Get a new Quadric off the stack
	gluQuadricTexture(pObj, true);
	gluSphere(pObj, 1, 20, 20);
	gluDeleteQuadric(pObj);

	glPopMatrix();
}

} // end namespace salsa
