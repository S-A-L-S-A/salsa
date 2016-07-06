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

#include "phycylinder.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "farsaglutils.h"
#include "worldsimutils.h"
#include <QtAlgorithms>

namespace farsa {

void PhyCylinder::calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning CHECK IF THIS WORKS CORRECTLY
#endif
	const wMatrix mtr = sharedData->collisionShapeOffset * tm;

	// To compute the AABB of the cylinder we first compute the AABBs of the two bases and then merge them. The u and v
	// vectors of the two bases are parallel to the y and z axes of the local frame of reference
	const wVector uRadius = mtr.y_ax.scale(sharedData->radius);
	const wVector vRadius = mtr.z_ax.scale(sharedData->radius);
	const wVector centerBaseVec = mtr.x_ax.scale(sharedData->height / 2.0);
	const wVector b1u = uRadius + centerBaseVec;
	const wVector b1v = vRadius + centerBaseVec;
	// Here we use minPoint and maxPoint directly because they will also be used in the merge as the output variables
	computeEllipseAABB(b1u, b1v, minPoint, maxPoint);

	const wVector b2u = uRadius - centerBaseVec;
	const wVector b2v = vRadius - centerBaseVec;
	wVector b2MinPoint;
	wVector b2MaxPoint;
	computeEllipseAABB(b2u, b2v, b2MinPoint, b2MaxPoint);

	// Now merging the two AABBs
	mergeAABBs(minPoint, maxPoint, b2MinPoint, b2MaxPoint);
}

wVector PhyCylinder::calculateMBBSize(const Shared* sharedData)
{
	return wVector(sharedData->height, sharedData->radius * 2.0, sharedData->radius * 2.0);
}

PhyCylinder::PhyCylinder(World* world, SharedDataWrapper<Shared> shared, real radius, real height, QString name, const wMatrix& tm)
	: PhyObject(world, shared, name, tm)
	, m_shared(shared)
	, m_uniformColor()
{
	Shared* const d = m_shared.getModifiableShared();
	d->radius = radius;
	d->height = height;

	// Initializing the list we return when there is a uniform color
	m_uniformColor.append(PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), m_shared->color));

	// Triggering an update so that all renderers will update their representation the first time
	d->colorChangeTrigger.triggerUpdate();
}

PhyCylinder::~PhyCylinder()
{
	// Nothing to do here
}

void PhyCylinder::setUpperBaseColor(QColor color)
{
	Shared* const d = m_shared.getModifiableShared();

	d->upperBaseColor = color;

	if (m_shared->color.isValid()) {
		d->lowerBaseColor = m_shared->color;
		d->segmentsColor.append(PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), m_shared->color));
		d->color = QColor();
	}

	d->colorChangeTrigger.triggerUpdate();
}

void PhyCylinder::setLowerBaseColor(QColor color)
{
	Shared* const d = m_shared.getModifiableShared();

	d->lowerBaseColor = color;

	if (m_shared->color.isValid()) {
		d->upperBaseColor = m_shared->color;
		d->segmentsColor.append(PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), m_shared->color));
		d->color = QColor();
	}

	d->colorChangeTrigger.triggerUpdate();
}

namespace {
	// Utility class used in the function PhyCylinder::setSegmentsColor(). This is
	// used basically to sort section of various color by ascending interval start
	class SimpleIntervalAndColor {
	public:
		SimpleIntervalAndColor() :
			interval(),
			color()
		{
		}

		SimpleIntervalAndColor(const SimpleInterval& i, QColor c) :
			interval(i),
			color(c)
		{
		}

		bool operator<(const SimpleIntervalAndColor& other) const
		{
			return interval < other.interval;
		}

		SimpleInterval interval;
		QColor color;
	};
}

void PhyCylinder::setSegmentsColor(QColor base, const QList<PhyCylinderSegmentColor>& segmentsColor)
{
	Shared* const d = m_shared.getModifiableShared();

	if (segmentsColor.isEmpty()) {
		d->segmentsColor.clear();
		d->segmentsColor << PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), base);

		d->colorChangeTrigger.triggerUpdate();

		return;
	}

	// The part of the cylinder not in segments of the list (which at the end will have the base color)
	Intervals remainingArc(SimpleInterval(-PI_GRECO, PI_GRECO));

	// Adding the first interval. We use a temporary list because the final one will have a different structure
	QList<PhyCylinderSegmentColor> tmpSegments;
	const Intervals intervalToAdd = segmentsColor[0].intervals & remainingArc;
	remainingArc -= segmentsColor[0].intervals;
	tmpSegments << PhyCylinderSegmentColor(intervalToAdd, segmentsColor[0].color);
	for (QList<PhyCylinderSegmentColor>::const_iterator it = segmentsColor.begin(); it != segmentsColor.end(); it++) {
		const Intervals intervalToAdd = it->intervals & remainingArc;
		remainingArc -= it->intervals;
		tmpSegments << PhyCylinderSegmentColor(intervalToAdd, it->color);
	}
	// If the remainingArc is not empty, adding the final PhyCylinderSegmentColor
	if (!remainingArc.isEmpty()) {
		tmpSegments << PhyCylinderSegmentColor(remainingArc, base);
	}

	// Now we fill m_shared->segmentsColor so that it respects the format described in segmentsColor(). First we create
	// a vector of SimpleIntervalAndColor, then sort it and finally use it to populate m_shared->segmentsColor. This procedure
	// works because tmpSegments is made up of  intervals that cover the whole circle and without any overlapping
	// Creating a vector of SimpleIntervalAndColor
	QList<SimpleIntervalAndColor> simpleIntervals;
	for (QList<PhyCylinderSegmentColor>::const_iterator segmentIt = tmpSegments.constBegin(); segmentIt != tmpSegments.constEnd(); ++segmentIt) {
		for (Intervals::const_iterator intervalIt = segmentIt->intervals.constBegin(); intervalIt != segmentIt->intervals.constEnd(); ++intervalIt) {
			simpleIntervals << SimpleIntervalAndColor(*intervalIt, segmentIt->color);
		}
	}
	// Sorting the vector we just generated
	qSort(simpleIntervals);
	// Now filling m_shared->segmentsColor
	d->segmentsColor.clear();
	for (QList<SimpleIntervalAndColor>::const_iterator it = simpleIntervals.begin(); it != simpleIntervals.end(); ++it) {
		d->segmentsColor << PhyCylinderSegmentColor(it->interval, it->color);
	}

	if (m_shared->color.isValid()) {
		d->upperBaseColor = m_shared->color;
		d->lowerBaseColor = m_shared->color;
		d->color = QColor();
	}

	d->colorChangeTrigger.triggerUpdate();
}

void PhyCylinder::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonCollision* c = NewtonCreateCylinder( m_worldPriv->world, m_shared->radius, m_shared->height, 1, (collisionShapeOffset == NULL) ? NULL : &(*collisionShapeOffset)[0][0] );
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

RenderPhyCylinder::RenderPhyCylinder(const PhyCylinder* entity)
	: RenderPhyObject(entity)
	, m_colorChangeChecker()
	, m_oldColor()
	, m_cylinderVertexes()
	, m_cylinderNormals()
	, m_cylinderColors()
	, m_cylinderTextureCoords()
	, m_upperBaseVertexes()
	, m_upperBaseNormals()
	, m_upperBaseColors()
	, m_upperBaseTextureCoords()
	, m_upperBaseSegmentsLength()
	, m_lowerBaseVertexes()
	, m_lowerBaseNormals()
	, m_lowerBaseColors()
	, m_lowerBaseTextureCoords()
	, m_lowerBaseSegmentsLength()
{
}

RenderPhyCylinder::~RenderPhyCylinder()
{
	// Nothing to do here
}

void RenderPhyCylinder::render(const PhyCylinderShared* sharedData, GLContextAndData* contextAndData)
{
	// Regenerating the representation of the cylinder if we have to
	updateGraphicalRepresentation(sharedData);

	glPushMatrix();
	setupColorTexture(sharedData, contextAndData);
	wMatrix mtr = collisionShapeMatrix(sharedData);
	GLMultMatrix(&(mtr[0][0]));

	// First drawing the cylinder. We need to enable the vertex arrays, set the
	// starting point for vertex, normals and colors and then actually draw triangles
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glNormalPointer(GL_FLOAT, 0, m_cylinderNormals.data());
	glColorPointer(3, GL_FLOAT, 0, m_cylinderColors.data());
	glVertexPointer(3, GL_FLOAT, 0, m_cylinderVertexes.data());
	glTexCoordPointer(2, GL_FLOAT, 0, m_cylinderTextureCoords.data());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_cylinderVertexes.size() / 3);

	// Now drawing the upper base
	glNormalPointer(GL_FLOAT, 0, m_upperBaseNormals.data());
	glColorPointer(3, GL_FLOAT, 0, m_upperBaseColors.data());
	glVertexPointer(3, GL_FLOAT, 0, m_upperBaseVertexes.data());
	glTexCoordPointer(2, GL_FLOAT, 0, m_upperBaseTextureCoords.data());

	unsigned int startIndex = 0;
	foreach (unsigned int curLength, m_upperBaseSegmentsLength) {
		glDrawArrays(GL_TRIANGLE_FAN, startIndex, curLength);
		startIndex += curLength;
	}

	// Finally drawing the lower base
	glNormalPointer(GL_FLOAT, 0, m_lowerBaseNormals.data());
	glColorPointer(3, GL_FLOAT, 0, m_lowerBaseColors.data());
	glVertexPointer(3, GL_FLOAT, 0, m_lowerBaseVertexes.data());
	glTexCoordPointer(2, GL_FLOAT, 0, m_lowerBaseTextureCoords.data());

	startIndex = 0;
	foreach (unsigned int curLength, m_lowerBaseSegmentsLength) {
		glDrawArrays(GL_TRIANGLE_FAN, startIndex, curLength);
		startIndex += curLength;
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();
}

void RenderPhyCylinder::updateGraphicalRepresentation(const PhyCylinderShared* sharedData)
{
	// Checking if an update is really needed
	if ((!m_colorChangeChecker.updateNeeded(sharedData->colorChangeTrigger)) && (sharedData->color == m_oldColor)) {
		return;
	}

	// Updating color
	m_oldColor = sharedData->color;

	// Some helper constants
	const int numDivs = 20;
	const float anglePerDiv = (2 * M_PI) / float(numDivs);
	const float lowerBaseX = -sharedData->height / 2.0;
	const float upperBaseX = +sharedData->height / 2.0;

	// Resetting all lists
	m_cylinderVertexes.clear();
	m_cylinderNormals.clear();
	m_cylinderColors.clear();
	m_cylinderTextureCoords.clear();
	m_upperBaseVertexes.clear();
	m_upperBaseNormals.clear();
	m_upperBaseColors.clear();
	m_upperBaseTextureCoords.clear();
	m_upperBaseSegmentsLength.clear();
	m_lowerBaseVertexes.clear();
	m_lowerBaseNormals.clear();
	m_lowerBaseColors.clear();
	m_lowerBaseTextureCoords.clear();
	m_lowerBaseSegmentsLength.clear();

	// Getting the base color of the cylinder
	const QColor cylinderColor = sharedData->color;

	// Getting the list of colors of the cylinder. If the cylinder has only one color, we generate a list to be
	// able to use the code below anyway. We use a temporary list because before computing all the points we
	// generate a list which contains exactly the angular segments that will be drawn, splitting the bigger ones
	QList<PhyCylinderSegmentColor> tmpCylinderColors;
	if (cylinderColor.isValid()){
		// Single color for the whole cylinder
		tmpCylinderColors.append(PhyCylinderSegmentColor(SimpleInterval(-PI_GRECO, PI_GRECO), cylinderColor));
	} else {
		tmpCylinderColors = sharedData->segmentsColor;
	}
	// Now modifying the list to be the actual segments to draw
	QList<PhyCylinderSegmentColor> cylinderColors;
	for (int i = 0; i < tmpCylinderColors.size(); ++i) {
		if (tmpCylinderColors[i].intervals.length() < anglePerDiv) {
			cylinderColors << tmpCylinderColors[i];
		} else {
			// Computing in how many parts we have to split the current segment and the length of each segment
			const int numDivisions = int(ceil(tmpCylinderColors[i].intervals.length() / anglePerDiv));
			if (numDivisions == 0) {
				continue;
			}
			const real divisionsLength = tmpCylinderColors[i].intervals.length() / real(numDivisions);

			// If we have only one interval (it should never happend, however better safe than sorry) adding the original interval
			if (numDivisions == 1) {
				cylinderColors << tmpCylinderColors[i];
			} else {
				// Adding the first segment
				const real firstIntervalStart = tmpCylinderColors[i].intervals.begin()->start;
				cylinderColors << PhyCylinderSegmentColor(SimpleInterval(firstIntervalStart, firstIntervalStart + divisionsLength), tmpCylinderColors[i].color);
				for (int j = 1; j < numDivisions - 1; j++) {
					const real curIntervalStart = cylinderColors.last().intervals.begin()->end;
					cylinderColors << PhyCylinderSegmentColor(SimpleInterval(curIntervalStart, curIntervalStart + divisionsLength), tmpCylinderColors[i].color);
				}
				// Just to avoid numerical problems, setting the end of the last interval to the end of the original interval
				const real lastIntervalStart = cylinderColors.last().intervals.begin()->end;
				const real lastIntervalEnd = tmpCylinderColors[i].intervals.begin()->end;
				cylinderColors << PhyCylinderSegmentColor(SimpleInterval(lastIntervalStart, lastIntervalEnd), tmpCylinderColors[i].color);
			}
		}
	}

	// Adding the first two points (one on the lower base, the other on the upper base), then adding the others in the cycle.
	{
		const QColor color = cylinderColors[0].color;
		const float normY = cos(-PI_GRECO);
		const float normZ = sin(-PI_GRECO);
		const float y = sharedData->radius * normY;
		const float z = sharedData->radius * normZ;
		m_cylinderVertexes.append(lowerBaseX);
		m_cylinderVertexes.append(y);
		m_cylinderVertexes.append(z);
		m_cylinderNormals.append(0.0);
		m_cylinderNormals.append(normY);
		m_cylinderNormals.append(normZ);
		m_cylinderColors.append(color.redF());
		m_cylinderColors.append(color.greenF());
		m_cylinderColors.append(color.blueF());
		m_cylinderTextureCoords.append(0.0);
		m_cylinderTextureCoords.append(0.0);
		m_cylinderVertexes.append(upperBaseX);
		m_cylinderVertexes.append(y);
		m_cylinderVertexes.append(z);
		m_cylinderNormals.append(0.0);
		m_cylinderNormals.append(normY);
		m_cylinderNormals.append(normZ);
		m_cylinderColors.append(color.redF());
		m_cylinderColors.append(color.greenF());
		m_cylinderColors.append(color.blueF());
		m_cylinderTextureCoords.append(1.0);
		m_cylinderTextureCoords.append(0.0);
	}
	// We have to save the old color to decide when to switch
	QColor prevColor = cylinderColors[0].color;
	for (int i = 0; i < cylinderColors.size(); ++i) {
		const QColor color = cylinderColors[i].color;

		if (prevColor != color) {
			// Here we have to add the start of the interval (which is equal to the end of the previous interval).
			// We add two points, one on the lower base, the other on the upper base
			const real prevAngle = cylinderColors[i].intervals.begin()->start;
			const float normY = cos(prevAngle);
			const float normZ = sin(prevAngle);
			const float y = sharedData->radius * normY;
			const float z = sharedData->radius * normZ;

			m_cylinderVertexes.append(lowerBaseX);
			m_cylinderVertexes.append(y);
			m_cylinderVertexes.append(z);
			m_cylinderNormals.append(0.0);
			m_cylinderNormals.append(normY);
			m_cylinderNormals.append(normZ);
			m_cylinderColors.append(color.redF());
			m_cylinderColors.append(color.greenF());
			m_cylinderColors.append(color.blueF());
			m_cylinderTextureCoords.append(0.0);
			m_cylinderTextureCoords.append((prevAngle + PI_GRECO) / (2.0 * PI_GRECO));
			m_cylinderVertexes.append(upperBaseX);
			m_cylinderVertexes.append(y);
			m_cylinderVertexes.append(z);
			m_cylinderNormals.append(0.0);
			m_cylinderNormals.append(normY);
			m_cylinderNormals.append(normZ);
			m_cylinderColors.append(color.redF());
			m_cylinderColors.append(color.greenF());
			m_cylinderColors.append(color.blueF());
			m_cylinderTextureCoords.append(1.0);
			m_cylinderTextureCoords.append((prevAngle + PI_GRECO) / (2.0 * PI_GRECO));
		}

		const real curAngle = cylinderColors[i].intervals.begin()->end;
		const float normY = cos(curAngle);
		const float normZ = sin(curAngle);
		const float y = sharedData->radius * normY;
		const float z = sharedData->radius * normZ;

		// Adding two points, one on the lower base, the other on the upper base
		m_cylinderVertexes.append(lowerBaseX);
		m_cylinderVertexes.append(y);
		m_cylinderVertexes.append(z);
		m_cylinderNormals.append(0.0);
		m_cylinderNormals.append(normY);
		m_cylinderNormals.append(normZ);
		m_cylinderColors.append(color.redF());
		m_cylinderColors.append(color.greenF());
		m_cylinderColors.append(color.blueF());
		m_cylinderTextureCoords.append(0.0);
		m_cylinderTextureCoords.append((curAngle + PI_GRECO) / (2.0 * PI_GRECO));
		m_cylinderVertexes.append(upperBaseX);
		m_cylinderVertexes.append(y);
		m_cylinderVertexes.append(z);
		m_cylinderNormals.append(0.0);
		m_cylinderNormals.append(normY);
		m_cylinderNormals.append(normZ);
		m_cylinderColors.append(color.redF());
		m_cylinderColors.append(color.greenF());
		m_cylinderColors.append(color.blueF());
		m_cylinderTextureCoords.append(1.0);
		m_cylinderTextureCoords.append((curAngle + PI_GRECO) / (2.0 * PI_GRECO));

		prevColor = cylinderColors[i].color;
	}

	// Now the upper base. First getting the list of colors; as for the cylinder we generate a list of
	// colors so that the code below will work in any case and the generated list has the same angles as the
	// one of the cylinder
	QList<PhyCylinderSegmentColor> upperBaseColor;
	QColor colorForUpperBase;
	if (cylinderColor.isValid()) {
		// Single color for the whole cylinder
		colorForUpperBase = cylinderColor;
	} else if (sharedData->upperBaseColor.isValid()) {
		// Single color for the whole base
		colorForUpperBase = sharedData->upperBaseColor;
	}
	if (colorForUpperBase.isValid()) {
		for (int i = 0; i < cylinderColors.size(); i++) {
			PhyCylinderSegmentColor s = cylinderColors[i];
			s.color = colorForUpperBase;
			upperBaseColor << s;
		}
	} else {
		upperBaseColor = cylinderColors;
	}

	// Now generating vertexes for the upper base. This variable is needed to count the number of points
	// for each color (we have to re-add the center when changing the color)
	unsigned int numVertexesCurSegment = 0;

	// Before entering the cycle, adding the central point (remember that we will draw this list of
	// vertexes as a TRIANGLE_FAN)
	{
		const QColor color = upperBaseColor[0].color;
		m_upperBaseVertexes.append(upperBaseX);
		m_upperBaseVertexes.append(0.0);
		m_upperBaseVertexes.append(0.0);
		m_upperBaseNormals.append(1.0);
		m_upperBaseNormals.append(0.0);
		m_upperBaseNormals.append(0.0);
		m_upperBaseColors.append(color.redF());
		m_upperBaseColors.append(color.greenF());
		m_upperBaseColors.append(color.blueF());
		m_upperBaseTextureCoords.append(0.5);
		m_upperBaseTextureCoords.append(0.5);
		++numVertexesCurSegment;

		const real curAngle = upperBaseColor[0].intervals.begin()->start;
		const float normY = cos(curAngle);
		const float normZ = sin(curAngle);
		const float y = sharedData->radius * normY;
		const float z = sharedData->radius * normZ;

		m_upperBaseVertexes.append(upperBaseX);
		m_upperBaseVertexes.append(y);
		m_upperBaseVertexes.append(z);
		m_upperBaseNormals.append(1.0);
		m_upperBaseNormals.append(0.0);
		m_upperBaseNormals.append(0.0);
		m_upperBaseColors.append(color.redF());
		m_upperBaseColors.append(color.greenF());
		m_upperBaseColors.append(color.blueF());
		m_upperBaseTextureCoords.append((normY + 1.0) / 2.0);
		m_upperBaseTextureCoords.append((normZ + 1.0) / 2.0);
		++numVertexesCurSegment;
	}
	// We have to save the old color to decide when to switch
	prevColor = upperBaseColor[0].color;
	for (int i = 0; i < upperBaseColor.size(); ++i) {
		const QColor color = upperBaseColor[i].color;

		if (prevColor != color) {
			// Saving the number of vertex of the current segment and resetting the counter
			m_upperBaseSegmentsLength.append(numVertexesCurSegment);
			numVertexesCurSegment = 0;

			// Here we have to add again the center of the circle and the start of the
			// interval (which is equal to the end of the previous interval).
			const real prevAngle = upperBaseColor[i].intervals.begin()->start;
			const float normY = cos(prevAngle);
			const float normZ = sin(prevAngle);
			const float y = sharedData->radius * normY;
			const float z = sharedData->radius * normZ;
			m_upperBaseVertexes.append(upperBaseX);
			m_upperBaseVertexes.append(0.0);
			m_upperBaseVertexes.append(0.0);
			m_upperBaseNormals.append(1.0);
			m_upperBaseNormals.append(0.0);
			m_upperBaseNormals.append(0.0);
			m_upperBaseColors.append(color.redF());
			m_upperBaseColors.append(color.greenF());
			m_upperBaseColors.append(color.blueF());
			m_upperBaseTextureCoords.append(0.5);
			m_upperBaseTextureCoords.append(0.5);
			++numVertexesCurSegment;
			m_upperBaseVertexes.append(upperBaseX);
			m_upperBaseVertexes.append(y);
			m_upperBaseVertexes.append(z);
			m_upperBaseNormals.append(1.0);
			m_upperBaseNormals.append(0.0);
			m_upperBaseNormals.append(0.0);
			m_upperBaseColors.append(color.redF());
			m_upperBaseColors.append(color.greenF());
			m_upperBaseColors.append(color.blueF());
			m_upperBaseTextureCoords.append((normY + 1.0) / 2.0);
			m_upperBaseTextureCoords.append((normZ + 1.0) / 2.0);
			++numVertexesCurSegment;
		}

		const real curAngle = upperBaseColor[i].intervals.begin()->end;
		const float normY = cos(curAngle);
		const float normZ = sin(curAngle);
		const float y = sharedData->radius * normY;
		const float z = sharedData->radius * normZ;

		m_upperBaseVertexes.append(upperBaseX);
		m_upperBaseVertexes.append(y);
		m_upperBaseVertexes.append(z);
		m_upperBaseNormals.append(1.0);
		m_upperBaseNormals.append(0.0);
		m_upperBaseNormals.append(0.0);
		m_upperBaseColors.append(color.redF());
		m_upperBaseColors.append(color.greenF());
		m_upperBaseColors.append(color.blueF());
		m_upperBaseTextureCoords.append((normY + 1.0) / 2.0);
		m_upperBaseTextureCoords.append((normZ + 1.0) / 2.0);
		++numVertexesCurSegment;

		prevColor = upperBaseColor[i].color;
	}
	// Adding the length of the last segment
	m_upperBaseSegmentsLength.append(numVertexesCurSegment);

	// Finally the lower base. The code is exactly like the one for the upper base
	QList<PhyCylinderSegmentColor> lowerBaseColor;
	QColor colorForLowerBase;
	if (cylinderColor.isValid()) {
		// Single color for the whole cylinder
		colorForLowerBase = cylinderColor;
	} else if (sharedData->lowerBaseColor.isValid()) {
		// Single color for the whole base
		colorForLowerBase = sharedData->lowerBaseColor;
	}
	if (colorForLowerBase.isValid()) {
		for (int i = 0; i < cylinderColors.size(); i++) {
			PhyCylinderSegmentColor s = cylinderColors[i];
			s.color = colorForLowerBase;
			lowerBaseColor << s;
		}
	} else {
		lowerBaseColor = cylinderColors;
	}

	numVertexesCurSegment = 0;

	{
		const QColor color = lowerBaseColor[0].color;
		m_lowerBaseVertexes.append(lowerBaseX);
		m_lowerBaseVertexes.append(0.0);
		m_lowerBaseVertexes.append(0.0);
		m_lowerBaseNormals.append(-1.0);
		m_lowerBaseNormals.append(0.0);
		m_lowerBaseNormals.append(0.0);
		m_lowerBaseColors.append(color.redF());
		m_lowerBaseColors.append(color.greenF());
		m_lowerBaseColors.append(color.blueF());
		m_lowerBaseTextureCoords.append(0.5);
		m_lowerBaseTextureCoords.append(0.5);
		++numVertexesCurSegment;

		const real curAngle = lowerBaseColor[0].intervals.begin()->start;
		const float normY = cos(curAngle);
		const float normZ = sin(curAngle);
		const float y = sharedData->radius * normY;
		const float z = sharedData->radius * normZ;

		m_lowerBaseVertexes.append(lowerBaseX);
		m_lowerBaseVertexes.append(y);
		m_lowerBaseVertexes.append(z);
		m_lowerBaseNormals.append(-1.0);
		m_lowerBaseNormals.append(0.0);
		m_lowerBaseNormals.append(0.0);
		m_lowerBaseColors.append(color.redF());
		m_lowerBaseColors.append(color.greenF());
		m_lowerBaseColors.append(color.blueF());
		m_lowerBaseTextureCoords.append((normY + 1.0) / 2.0);
		m_lowerBaseTextureCoords.append((normZ + 1.0) / 2.0);
		++numVertexesCurSegment;
	}
	// We have to save the old color to decide when to switch
	prevColor = lowerBaseColor[0].color;
	for (int i = 0; i < lowerBaseColor.size(); ++i) {
		const QColor color = lowerBaseColor[i].color;

		if (prevColor != color) {
			// Saving the number of vertex of the current segment and resetting the counter
			m_lowerBaseSegmentsLength.append(numVertexesCurSegment);
			numVertexesCurSegment = 0;

			// Here we have to add again the center of the circle and the start of the
			// interval (which is equal to the end of the previous interval).
			const real prevAngle = lowerBaseColor[i].intervals.begin()->start;
			const float normY = cos(prevAngle);
			const float normZ = sin(prevAngle);
			const float y = sharedData->radius * normY;
			const float z = sharedData->radius * normZ;
			m_lowerBaseVertexes.append(lowerBaseX);
			m_lowerBaseVertexes.append(0.0);
			m_lowerBaseVertexes.append(0.0);
			m_lowerBaseNormals.append(-1.0);
			m_lowerBaseNormals.append(0.0);
			m_lowerBaseNormals.append(0.0);
			m_lowerBaseColors.append(color.redF());
			m_lowerBaseColors.append(color.greenF());
			m_lowerBaseColors.append(color.blueF());
			m_lowerBaseTextureCoords.append(0.5);
			m_lowerBaseTextureCoords.append(0.5);
			++numVertexesCurSegment;
			m_lowerBaseVertexes.append(lowerBaseX);
			m_lowerBaseVertexes.append(y);
			m_lowerBaseVertexes.append(z);
			m_lowerBaseNormals.append(-1.0);
			m_lowerBaseNormals.append(0.0);
			m_lowerBaseNormals.append(0.0);
			m_lowerBaseColors.append(color.redF());
			m_lowerBaseColors.append(color.greenF());
			m_lowerBaseColors.append(color.blueF());
			m_lowerBaseTextureCoords.append((normY + 1.0) / 2.0);
			m_lowerBaseTextureCoords.append((normZ + 1.0) / 2.0);
			++numVertexesCurSegment;
		}

		const real curAngle = lowerBaseColor[i].intervals.begin()->end;
		const float normY = cos(curAngle);
		const float normZ = sin(curAngle);
		const float y = sharedData->radius * normY;
		const float z = sharedData->radius * normZ;

		m_lowerBaseVertexes.append(lowerBaseX);
		m_lowerBaseVertexes.append(y);
		m_lowerBaseVertexes.append(z);
		m_lowerBaseNormals.append(-1.0);
		m_lowerBaseNormals.append(0.0);
		m_lowerBaseNormals.append(0.0);
		m_lowerBaseColors.append(color.redF());
		m_lowerBaseColors.append(color.greenF());
		m_lowerBaseColors.append(color.blueF());
		m_lowerBaseTextureCoords.append((normY + 1.0) / 2.0);
		m_lowerBaseTextureCoords.append((normZ + 1.0) / 2.0);
		++numVertexesCurSegment;

		prevColor = lowerBaseColor[i].color;
	}
	// Adding the length of the last segment
	m_lowerBaseSegmentsLength.append(numVertexesCurSegment);
}

} // end namespace farsa
