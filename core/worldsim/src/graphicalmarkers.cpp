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

#include "graphicalmarkers.h"
#include "farsaglutils.h"

namespace farsa {

CircularGraphicalMarker::CircularGraphicalMarker(World* world, SharedDataWrapper<Shared> shared, real radius, const wMatrix& disp, QString name, const wMatrix& tm)
	: OwnerFollower(world, shared, disp, name, tm)
	, m_shared(shared)
{
	m_shared.getModifiableShared()->radius = radius;
}

CircularGraphicalMarker::~CircularGraphicalMarker()
{
	// Nothing to do here
}

RenderCircularGraphicalMarker::RenderCircularGraphicalMarker(const CircularGraphicalMarker* entity)
	: RenderOwnerFollower(entity)
{
}

RenderCircularGraphicalMarker::~RenderCircularGraphicalMarker()
{
	// Nothing to do here
}

void RenderCircularGraphicalMarker::render(const CircularGraphicalMarkerShared* sharedData, GLContextAndData* contextAndData)
{
	// Computing the final matrix and pushing it
	const wMatrix tm = finalMatrix(sharedData);
	setupColorTexture(sharedData, contextAndData);
	glPushMatrix();
	GLMultMatrix(&tm[0][0]);

	// Drawing the disk. We disable lightining
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(sharedData->color.redF(), sharedData->color.greenF(), sharedData->color.blueF());

	// Actually drawing the disk
	GLUquadricObj *pObj = gluNewQuadric();
	gluDisk(pObj, 0.0f, sharedData->radius, 20, 1);
	gluDeleteQuadric(pObj);

	// Restoring lighting status
	glPopAttrib();

	glPopMatrix();
}

PlanarArrowGraphicalMarker::PlanarArrowGraphicalMarker(World* world, SharedDataWrapper<Shared> shared, real arrowLength, real arrowTailWidth, real arrowHeadWidth, real arrowTailPortion, const wMatrix& disp, QString name, const wMatrix& tm)
	: OwnerFollower(world, shared, disp, name, tm)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();
	d->arrowLength = arrowLength;
	d->arrowTailWidth = arrowTailWidth;
	d->arrowHeadWidth = arrowHeadWidth;
	d->arrowTailPortion = max(min(arrowTailPortion, 1.0), 0.0);
}

PlanarArrowGraphicalMarker::~PlanarArrowGraphicalMarker()
{
	// Nothing to do here
}

RenderPlanarArrowGraphicalMarker::RenderPlanarArrowGraphicalMarker(const PlanarArrowGraphicalMarker* entity)
	: RenderOwnerFollower(entity)
{
}

RenderPlanarArrowGraphicalMarker::~RenderPlanarArrowGraphicalMarker()
{
	// Nothing to do here
}

void RenderPlanarArrowGraphicalMarker::render(const PlanarArrowGraphicalMarkerShared* sharedData, GLContextAndData* contextAndData)
{
	// Computing the final matrix and pushing it
	const wMatrix tm = finalMatrix(sharedData);
	setupColorTexture(sharedData, contextAndData);
	glPushMatrix();
	GLMultMatrix(&tm[0][0]);

	// Drawing the arrow. We disable lightining
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(sharedData->color.redF(), sharedData->color.greenF(), sharedData->color.blueF());

	// First drawing the tail
	const real tailLength = sharedData->arrowLength * sharedData->arrowTailPortion;
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, -sharedData->arrowTailWidth / 2.0, 0.0);
	glVertex3f(0.0, sharedData->arrowTailWidth / 2.0, 0.0);
	glVertex3f(tailLength, sharedData->arrowTailWidth / 2.0, 0.0);
	glVertex3f(tailLength, -sharedData->arrowTailWidth / 2.0, 0.0);
	glEnd();

	// Now drawing the tip
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(tailLength, -sharedData->arrowHeadWidth / 2.0, 0.0);
	glVertex3f(tailLength, sharedData->arrowHeadWidth / 2.0, 0.0);
	glVertex3f(sharedData->arrowLength, 0.0, 0.0);
	glEnd();

	// Restoring lighting status
	glPopAttrib();

	glPopMatrix();
}

}
