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

#include "singleir.h"
#include "mathutils.h"
#include "farsaglutils.h"
#include "world.h"

namespace farsa {

namespace {
	// The side of the cube representing the sensor
	const float sensorCubeSide = 0.005f;
}

SingleIR::SingleIR(World* world, SharedDataWrapper<Shared> shared, const InitParams& params, QString name, const wMatrix& tm)
	: OwnerFollower(world, shared, params.displacement, name, tm)
	, m_shared(shared)
{
	// Setting the owner
	setOwner(params.owner);

	set(params.minDist, params.maxDist, params.aperture, params.numRays);

	// We also use our own color and texture
	setUseColorTextureOfOwner(false);
	setTexture("");
	setColor(Qt::black);
}

SingleIR::~SingleIR()
{
	// Nothing to do here
}

void SingleIR::update()
{
	// Checking if the sensor is valid
	if (!isValid()) {
		return;
	}

	// Resetting the current RayCastHit object
	Shared* const d = m_shared.getModifiableShared();
	d->rayCastHit.object = NULL;
	d->rayCastHit.distance = 1.0;

	// Getting the owner as a WObject
	WObject* own = ownerWObject();

	// If we are attached to a phyobject, we ignore it in collisions
	QSet<PhyObject*> ignoredObjs;
	if (own != NULL) {
		PhyObject* phyObj = dynamic_cast<PhyObject*>(own);
		if (phyObj != NULL) {
			ignoredObjs.insert(phyObj);
		}
	}

	// The minimum distance (distances range from 0.0 to 1.0)
	double minDist = 2.0;
	for (unsigned int i = 0; i < m_shared->numRays; i++) {
		// Computing the start and end point of the ray in the global frame of reference
		const wVector start = own->matrix().transformVector(m_shared->startingRayPoints[i]);
		const wVector end = own->matrix().transformVector(m_shared->endingRayPoints[i]);

		// Casting the ray
		const RayCastHitVector v = world()->worldRayCast(start, end, true, ignoredObjs);

		// Taking the lowest distance: this sensor only reports the distance of the closest
		// object
		if ((v.size() != 0) && (v[0].distance < minDist)) {
			minDist = v[0].distance;
			d->rayCastHit = v[0];
		}
	}
}

void SingleIR::set(double minDist, double maxDist, double aperture, unsigned int numRays)
{
	Shared* const d = m_shared.getModifiableShared();
	d->minDist = minDist;
	d->maxDist = maxDist;
	d->aperture = aperture;
	d->numRays = numRays;

	d->rayCastHit.object = NULL;
	d->rayCastHit.distance = 1.0;

	if (m_shared->numRays != 0) {
		d->startingRayPoints.resize(m_shared->numRays);
		d->endingRayPoints.resize(m_shared->numRays);
	} else {
		d->startingRayPoints.clear();
		d->endingRayPoints.clear();
	}

	computeRayPoints();
}


void SingleIR::setGraphicalProperties(bool drawSensor, bool drawRay, bool drawRealRay)
{
	Shared* const d = m_shared.getModifiableShared();
	d->drawSensor = drawSensor;
	d->drawRay = drawRay;
	d->drawRealRay = drawRealRay;
}

void SingleIR::computeRayPoints()
{
	// Checking the sensor is valid
	if (!isValid()) {
		return;
	}

	// First of all we need to compute the starting angle of the rays and the angular distance
	// between them. If there is only one ray it is at angle 0.0 (i.e. along the local Z axis).
	// The angles computed here are in radiants.
	const double startAngle = (m_shared->numRays == 1) ? 0.0 : -(toRad(m_shared->aperture) / 2.0);
	const double angularIncrement = (m_shared->numRays == 1) ? 0.0 : (toRad(m_shared->aperture) / double(m_shared->numRays - 1));

	// Now computing the angles in the object fame of reference
	Shared* const d = m_shared.getModifiableShared();
	for (unsigned int i = 0; i < m_shared->numRays; i++) {
		const double curAngle = startAngle + i * angularIncrement;

		// To get the ray in the sensor frame of reference we have to rotate the Z axis
		// around the Y axis. Then we compute the starting and ending point
		const wVector localRay = wVector::Z().rotateAround(wVector::Y(), curAngle);
		const wVector localStart = localRay.scale(m_shared->minDist);
		const wVector localEnd = localRay.scale(m_shared->maxDist);

		// Finally we can compute the start and end in the object frame of reference
		d->startingRayPoints[i] = m_shared->displacement.transformVector(localStart);
		d->endingRayPoints[i] = m_shared->displacement.transformVector(localEnd);
	}
}

RenderSingleIR::RenderSingleIR(const SingleIR* entity)
	: RenderOwnerFollower(entity)
{
}

RenderSingleIR::~RenderSingleIR()
{
	// Nothing to do here
}

void RenderSingleIR::render(const SingleIRShared* sharedData, GLContextAndData* contextAndData)
{
	if (!sharedData->drawSensor) {
		return;
	}

	// Pushing the transformation matrices separately because the box needs both of them
	// while rays doesn't need the offset (they are already in the solid frame of reference)
	// Bringing the coordinate system on the sensor
	glPushMatrix();
	GLMultMatrix(&(sharedData->tm[0][0]));

	glPushMatrix();
	GLMultMatrix(&(sharedData->displacement[0][0]));

	setupColorTexture(sharedData, contextAndData);

	// First drawing the cube representing the sensor. The cube will just be drawn as
	// six quads for the sake of simplicity. For each face, we specify the quad normal
	// (for lighting), then specify the quad's 4 vertices. The top part of the front
	// face is drawn in green to understand if the sensor is mounted upside-down
	glBegin(GL_QUADS);
	const float hside = sensorCubeSide / 2.0;

	// front (top part)
	glColor3f(0.0, 1.0, 0.0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-hside,    0.0,  hside);
	glVertex3f( hside,    0.0,  hside);
	glVertex3f( hside,  hside,  hside);
	glVertex3f(-hside,  hside,  hside);

	// front (bottom part)
	glColor3f(0.0, 0.0, 0.0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-hside, -hside,  hside);
	glVertex3f( hside, -hside,  hside);
	glVertex3f( hside,    0.0,  hside);
	glVertex3f(-hside,    0.0,  hside);

	// back
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f( hside, -hside, -hside);
	glVertex3f(-hside, -hside, -hside);
	glVertex3f(-hside,  hside, -hside);
	glVertex3f( hside,  hside, -hside);

	// top
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-hside,  hside,  hside);
	glVertex3f( hside,  hside,  hside);
	glVertex3f( hside,  hside, -hside);
	glVertex3f(-hside,  hside, -hside);

	// bottom
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-hside, -hside, -hside);
	glVertex3f( hside, -hside, -hside);
	glVertex3f( hside, -hside,  hside);
	glVertex3f(-hside, -hside,  hside);

	// right
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-hside, -hside, -hside);
	glVertex3f(-hside, -hside,  hside);
	glVertex3f(-hside,  hside,  hside);
	glVertex3f(-hside,  hside, -hside);

	// left
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f( hside, -hside,  hside);
	glVertex3f( hside, -hside, -hside);
	glVertex3f( hside,  hside, -hside);
	glVertex3f( hside,  hside,  hside);

	glEnd();

	// Popping only one matrix because ray need the other one
	glPopMatrix();

	// Now drawing the ray if we have to
	if (sharedData->drawRay) {
		// Disabling lighting here (we want pure red lines no matter from where we look at them)
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);

		glLineWidth(2.5);
		glColor3f(1.0, 0.0, 0.0);

		glBegin(GL_LINES);
		for (int i = 0; i < sharedData->startingRayPoints.size(); i++) {
			const wVector& s = sharedData->startingRayPoints[i];
			const wVector& e = sharedData->endingRayPoints[i];
			if (sharedData->drawRealRay) {
				glVertex3f(s.x, s.y, s.z);
				glVertex3f(e.x, e.y, e.z);
			} else {
				const wVector d = (e - s).normalize().scale(sensorCubeSide * 5.0);
				glVertex3f(sharedData->displacement.w_pos.x, sharedData->displacement.w_pos.y, sharedData->displacement.w_pos.z);
				glVertex3f(sharedData->displacement.w_pos.x + d.x, sharedData->displacement.w_pos.y + d.y, sharedData->displacement.w_pos.z + d.z);
			}
		}
		glEnd();

		// Restoring lighting status
		glPopAttrib();
	}

	glPopMatrix();
}

} // end namespace farsa
