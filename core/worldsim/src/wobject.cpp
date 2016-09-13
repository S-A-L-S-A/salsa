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

#include "wobject.h"
#include "world.h"
#include "salsaglutils.h"
#include <QStack>

namespace salsa {

void WObject::calculateAABB(const Shared*, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
	// Zero size
	minPoint = tm.w_pos;
	maxPoint = tm.w_pos;
}

wVector WObject::calculateMBBSize(const Shared* /*sharedData*/)
{
	// Zero size
	return wVector(0.0, 0.0, 0.0);
}

WObject::WObject(World* world, SharedDataWrapper<Shared> shared, QString name, const wMatrix& tm)
	: WEntity(world, shared, name)
	, m_shared(shared)
{
	m_shared.getModifiableShared()->tm = tm;
}

WObject::~WObject()
{
	// Nothing to do here, shared is destroyed by World
}

const wMatrix& WObject::matrix() const
{
	return m_shared->tm;
}

bool WObject::setMatrix(const wMatrix& newm)
{
	if (!aboutToChangeMatrix()) {
		return false;
	}

	m_shared.getModifiableShared()->tm = newm;

	changedMatrix();

	return true;
}

bool WObject::setPosition(const wVector& newpos)
{
	if (!aboutToChangeMatrix()) {
		return false;
	}

	m_shared.getModifiableShared()->tm.w_pos = newpos;

	changedMatrix();

	return true;
}

bool WObject::setPosition(real x, real y, real z)
{
	if (!aboutToChangeMatrix()) {
		return false;
	}

	m_shared.getModifiableShared()->tm.w_pos = wVector(x, y, z);

	changedMatrix();

	return true;
}

bool WObject::isInvisible() const
{
	return !(m_shared->flags & WObjectShared::Visible);
}

void WObject::setInvisible(bool i)
{
	if (i) {
		m_shared.getModifiableShared()->flags &= ~WObjectShared::Visible;
	} else {
		m_shared.getModifiableShared()->flags |= WObjectShared::Visible;
	}
}

bool WObject::localAxesDrawn() const
{
	return m_shared->flags & WObjectShared::DrawLocalReferenceFrame;
}

void WObject::setLocalAxesDrawn(bool d)
{
	if (d) {
		m_shared.getModifiableShared()->flags |= WObjectShared::DrawLocalReferenceFrame;
	} else {
		m_shared.getModifiableShared()->flags &= ~WObjectShared::DrawLocalReferenceFrame;
	}
}

void WObject::setLabel(QString label)
{
	m_shared.getModifiableShared()->label = label;
}

void WObject::setLabel(QString label, wVector pos)
{
	Shared* const d = m_shared.getModifiableShared();

	d->label = label;
	d->labelPos = pos;
}

void WObject::setLabel(QString label, wVector pos, QColor color)
{
	Shared* const d = m_shared.getModifiableShared();

	d->label = label;
	d->labelPos = pos;
	d->labelColor = color;
}

const QString& WObject::label() const
{
	return m_shared->label;
}

void WObject::setLabelPosition(const wVector& pos)
{
	m_shared.getModifiableShared()->labelPos = pos;
}

const wVector& WObject::labelPosition() const
{
	return m_shared->labelPos;
}

void WObject::setLabelColor(const QColor& color)
{
	m_shared.getModifiableShared()->labelColor = color;
}

const QColor& WObject::labelColor() const
{
	return m_shared->labelColor;
}

bool WObject::labelShown() const
{
	return m_shared->flags & WObjectShared::DrawLabel;
}

void WObject::setLabelShown(bool d)
{
	if (d) {
		m_shared.getModifiableShared()->flags |= WObjectShared::DrawLabel;
	} else {
		m_shared.getModifiableShared()->flags &= ~WObjectShared::DrawLabel;
	}
}

bool WObject::aboutToChangeMatrix()
{
	return true;
}

void WObject::changedMatrix()
{
}

RenderWObject::RenderWObject(const WObject* entity)
	: RenderWEntity(entity)
{
}

RenderWObject::~RenderWObject()
{
	// Nothing to do here
}

void RenderWObject::renderAABB(const WObjectShared* sharedData, GLContextAndData* /*contextAndData*/)
{
	wVector minPoint;
	wVector maxPoint;

	// Computing the AABB and then drawing it (no matrix to push because points are in world coordinates)
	calculateAABB(sharedData, minPoint, maxPoint, sharedData->tm);
	GLUtils::drawWireBox(minPoint, maxPoint);
}

void RenderWObject::drawLabel(const WObjectShared* sharedData, GLContextAndData* contextAndData)
{
	if (sharedData->label.isEmpty()) {
		return;
	}

	glPushMatrix();
	GLMultMatrix(&(sharedData->tm[0][0]));

	// We can only draw text through QGLWidget, so if this fails, we quit
	QGLWidget* const qglwidget = contextAndData->widget();
	if (qglwidget == nullptr) {
		return;
	}

	// Setting color
	glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	const QColor& lcol = sharedData->labelColor;
	glColor4f(lcol.redF(), lcol.greenF(), lcol.blueF(), lcol.alphaF());

	// Drawing text
	const wVector& lpos = sharedData->labelPos;
	qglwidget->renderText(lpos.x, lpos.y, lpos.z, sharedData->label);

	glPopAttrib();

	glPopMatrix();
}

void RenderWObject::drawAxes(const WObjectShared* sharedData, GLContextAndData*)
{
	glPushMatrix();
	GLMultMatrix(&(sharedData->tm[0][0]));

	// Getting the OBB to calculate the dimensions of the axes
	wVector minPoint;
	wVector maxPoint;
	const wVector sizes = calculateMBBSize(sharedData);

	// We take one tenth of the smallest dimension as the radius of axes
	const float radius = min(sizes.x, min(sizes.y, sizes.z)) * 0.1;

	GLUtils::drawArrow(wVector(sizes.x, 0.0, 0.0), wVector(0.0, 0.0, 0.0), radius, 2.0 * radius, 2.0 * radius, Qt::red);
	GLUtils::drawArrow(wVector(0.0, sizes.y, 0.0), wVector(0.0, 0.0, 0.0), radius, 2.0 * radius, 2.0 * radius, Qt::green);
	GLUtils::drawArrow(wVector(0.0, 0.0, sizes.z), wVector(0.0, 0.0, 0.0), radius, 2.0 * radius, 2.0 * radius, Qt::blue);

	glPopMatrix();
}

} // end namespace salsa
