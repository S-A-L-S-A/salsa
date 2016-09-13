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

#include "phyjoint.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "salsaglutils.h"

#ifdef SALSA_DEBUG
	#include <QDebug>
#endif

namespace salsa {

PhyJoint::PhyJoint(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, QString name)
	: WEntity(world, shared, name)
	, m_shared(shared)
	, m_dofs()
	, m_parent(parent)
	, m_child(child)
	, m_priv(new PhyJointPrivate())
	, m_parentPriv((parent == nullptr) ? nullptr : parent->m_priv)
	, m_childPriv(child->m_priv)
	, m_worldPriv(world->m_priv.get())
{
	m_priv->parent = (m_parent == nullptr) ? nullptr : m_parentPriv->body;
	m_priv->child = m_childPriv->body;

	// Computing the radius and length values used to draw the joint
	wVector minPoint;
	wVector maxPoint;

	wVector dimension = m_child->calculateMBBSize();
	real cube = (dimension[0] + dimension[1] + dimension[2]) / 3.0;

	if (m_parent != nullptr) {
		dimension = m_parent->calculateMBBSize();
		const real cube2 = (dimension[0] + dimension[1] + dimension[2]) / 3.0;
		cube = min(cube2, cube);
	}

	Shared* const d = m_shared.getModifiableShared();
	d->length = cube * 0.70;
	d->radius = m_shared->length * 0.25;
}

PhyJoint::~PhyJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonDestroyJoint(m_worldPriv->world, m_priv->joint);
#endif

	delete m_priv;
	foreach(PhyDOF* dof, m_dofs) {
		delete dof;
	}
}

void PhyJoint::enable(bool b)
{
	m_shared.getModifiableShared()->enabled = b;
}

void PhyJoint::createDOFs(const PhyDOFInitParamsList& initParams)
{
	Shared* const d = m_shared.getModifiableShared();

	// First of all creating structures with shared data of DOFs
	d->dofs.resize(initParams.size());

	// Now creating DOFs
	m_dofs.resize(initParams.size());
	for (int i = 0; i < initParams.size(); ++i) {
		m_dofs[i] = new PhyDOF(this, &(d->dofs[i]), initParams[i].axis, initParams[i].centre, initParams[i].translate);
	}
}

void PhyJoint::postCreatePrivateJoint()
{
}

RenderPhyJoint::RenderPhyJoint(const PhyJoint* entity)
	: RenderWEntity(entity)
{
}

RenderPhyJoint::~RenderPhyJoint()
{
	// Nothing to do here
}

void RenderPhyJoint::drawJoint(const PhyJointShared* sharedData, GLContextAndData* contextAndData, const wVector& center, const wVector& end1, const wVector& end2)
{
	if (!contextAndData->drawJoints()) {
		return;
	}

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable( GL_LIGHTING );

	const real dist1 = wVector::distance(center, end1);
	const real dist2 = wVector::distance(center, end2);
	const real radius = (dist1 + dist2) * 0.04;

	// Drawing the joint as a couple of cylinders
	GLUtils::drawCylinder(center, end1, radius * 0.6);
	GLUtils::drawCylinder(center, end2, radius * 0.6);

	// Now drawing all DOFs
	for (int k = 0; k < sharedData->dofs.size(); ++k) {
		const PhyDOFShared& dof = sharedData->dofs[k];
		const salsa::real len = sharedData->length;
		const salsa::real rad = sharedData->radius;

		//--- FIX ME: this method handles only rotational joints :-(
		if (dof.isTranslate) {
			return;
		}

		wMatrix mat;
		mat.x_ax = dof.xAxis;
		mat.y_ax = dof.yAxis;
		mat.z_ax = dof.axis;
		mat.w_pos = dof.centre;
		mat.sanitifize();

		mat.w_pos = dof.centre - dof.axis.scale(len / 2.0);
		GLUtils::drawCylinder(mat,len, rad, QColor(Qt::cyan));
		if (dof.limitsOn) {
			//--- draw indication about limits
			const real ang = dof.hiLimit - dof.loLimit;
			mat.w_pos = wVector(0.0, 0.0, 0.0);
			mat = mat * wMatrix(wQuaternion(dof.axis, dof.loLimit), wVector(0.0, 0.0, 0.0));
			mat.w_pos = dof.centre + dof.axis.scale(len / 2.0);
			GLUtils::drawTorus(rad, rad * 0.6, mat, ang);
		} else {
			mat.w_pos = dof.centre + dof.axis.scale(len / 2.0);
			GLUtils::drawTorus(rad, rad * 0.6, mat);
		}

		//--- draw indication about current position
		mat.x_ax = dof.xAxis;
		mat.y_ax = dof.yAxis;
		mat.z_ax = dof.axis;
		mat.w_pos = wVector(0.0, 0.0, 0.0);
		mat.sanitifize();
		mat = mat * wMatrix(wQuaternion(dof.axis, dof.position - 0.05), wVector(0.0, 0.0, 0.0));
		mat.w_pos = dof.centre + dof.axis.scale(len / 2.0);
		GLUtils::drawTorus(rad, rad * 0.55f, mat, 0.1f, Qt::green);

		// Drawing axes
		GLUtils::drawArrow(dof.centre, dof.centre + dof.xAxis.scale(len * 1.2), rad * 0.4, 12, QColor(Qt::magenta));
		GLUtils::drawArrow(dof.centre, dof.centre + dof.yAxis.scale(len * 1.2), rad * 0.5, 12, QColor(Qt::yellow));
	}

	glPopAttrib();
}

} // end namespace salsa
