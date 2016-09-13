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

#include "phybox.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "salsaglutils.h"

namespace salsa {

void PhyBox::calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
	const wMatrix mtr = sharedData->collisionShapeOffset * tm;
	const real tdx = fabs(mtr.x_ax[0] * sharedData->sideX) + fabs(mtr.y_ax[0] * sharedData->sideY) + fabs(mtr.z_ax[0] * sharedData->sideZ);
	const real tdy = fabs(mtr.x_ax[1] * sharedData->sideX) + fabs(mtr.y_ax[1] * sharedData->sideY) + fabs(mtr.z_ax[1] * sharedData->sideZ);
	const real tdz = fabs(mtr.x_ax[2] * sharedData->sideX) + fabs(mtr.y_ax[2] * sharedData->sideY) + fabs(mtr.z_ax[2] * sharedData->sideZ);

	const wVector hds(tdx / 2.0, tdy / 2.0, tdz / 2.0);
	minPoint = mtr.w_pos - hds;
	maxPoint = mtr.w_pos + hds;
}

wVector PhyBox::calculateMBBSize(const Shared* sharedData)
{
	return wVector(sharedData->sideX, sharedData->sideY, sharedData->sideZ);
}

PhyBox::PhyBox(World* world, SharedDataWrapper<Shared> shared, real sideX, real sideY, real sideZ, QString name, const wMatrix& tm)
	: PhyObject(world, shared, name, tm)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();
	d->sideX = sideX;
	d->sideY = sideY;
	d->sideZ = sideZ;
}

PhyBox::~PhyBox()
{
	// Nothing to do here
}

void PhyBox::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonCollision* c = NewtonCreateBox( m_worldPriv->world, m_shared->sideX, m_shared->sideY, m_shared->sideZ, 1, (collisionShapeOffset == nullptr) ? nullptr : &(*collisionShapeOffset)[0][0] );
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

RenderPhyBox::RenderPhyBox(const PhyBox* entity)
	: RenderPhyObject(entity)
{
}

RenderPhyBox::~RenderPhyBox()
{
	// Nothing to do here
}

void RenderPhyBox::render(const PhyBoxShared* sharedData, GLContextAndData* contextAndData)
{
	glPushMatrix();
	setupColorTexture(sharedData, contextAndData);
	wMatrix mtr = collisionShapeMatrix(sharedData);
	GLMultMatrix(&(mtr[0][0]));

	// the cube will just be drawn as six quads for the sake of simplicity
	// for each face, we specify the quad's normal (for lighting), then
	// specify the quad's 4 vertices and associated texture coordinates
	glBegin(GL_QUADS);
	float hdx = (sharedData->sideX / 2.0);
	float hdy = (sharedData->sideY / 2.0);
	float hdz = (sharedData->sideZ / 2.0);
	// front
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-hdx, -hdy, hdz);
	glTexCoord2f(1.0, 0.0); glVertex3f( hdx, -hdy, hdz);
	glTexCoord2f(1.0, 1.0); glVertex3f( hdx,  hdy, hdz);
	glTexCoord2f(0.0, 1.0); glVertex3f(-hdx,  hdy, hdz);

	// back
	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f( hdx, -hdy, -hdz);
	glTexCoord2f(1.0, 0.0); glVertex3f(-hdx, -hdy, -hdz);
	glTexCoord2f(1.0, 1.0); glVertex3f(-hdx,  hdy, -hdz);
	glTexCoord2f(0.0, 1.0); glVertex3f( hdx,  hdy, -hdz);

	// top
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-hdx,  hdy,  hdz);
	glTexCoord2f(1.0, 0.0); glVertex3f( hdx,  hdy,  hdz);
	glTexCoord2f(1.0, 1.0); glVertex3f( hdx,  hdy, -hdz);
	glTexCoord2f(0.0, 1.0); glVertex3f(-hdx,  hdy, -hdz);

	// bottom
	glNormal3f(0.0, -1.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-hdx, -hdy, -hdz);
	glTexCoord2f(1.0, 0.0); glVertex3f( hdx, -hdy, -hdz);
	glTexCoord2f(1.0, 1.0); glVertex3f( hdx, -hdy,  hdz);
	glTexCoord2f(0.0, 1.0); glVertex3f(-hdx, -hdy,  hdz);

	// left
	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-hdx, -hdy, -hdz);
	glTexCoord2f(1.0, 0.0); glVertex3f(-hdx, -hdy,  hdz);
	glTexCoord2f(1.0, 1.0); glVertex3f(-hdx,  hdy,  hdz);
	glTexCoord2f(0.0, 1.0); glVertex3f(-hdx,  hdy, -hdz);

	// right
	glNormal3f(1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f( hdx, -hdy,  hdz);
	glTexCoord2f(1.0, 0.0); glVertex3f( hdx, -hdy, -hdz);
	glTexCoord2f(1.0, 1.0); glVertex3f( hdx,  hdy, -hdz);
	glTexCoord2f(0.0, 1.0); glVertex3f( hdx,  hdy,  hdz);

	glEnd();

	glPopMatrix();
}

} // end namespace salsa
