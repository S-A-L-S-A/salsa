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

#include "phyfixed.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhyFixed::PhyFixed(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, QString name)
	:PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();

	// There is no DOF to create, here

	// Calculating the localMatrixParent
	d->localMatrixParent.x_ax = wVector::X();
	d->localMatrixParent.y_ax = wVector::Y();
	d->localMatrixParent.z_ax = wVector::Z();
	d->localMatrixParent.w_pos = wVector(0, 0, 0);
	d->localMatrixParent.sanitifize();

	// Calculating the local matrix respect to child object
	if (m_parent != NULL) {
		d->globalMatrixParent = m_shared->localMatrixParent * parent->matrix();
	} else {
		d->globalMatrixParent = m_shared->localMatrixParent;
	}
	d->localMatrixChild = m_shared->globalMatrixParent * m_child->matrix().inverse();

	if (!d->localMatrixParent.sanityCheck()) {
		qWarning("Sanity Check Failed");
	}
	if (!d->localMatrixChild.sanityCheck()) {
		qWarning("Sanity Check Failed");
	}
}

PhyFixed::~PhyFixed()
{
	// Nothing to do here
}

wVector PhyFixed::centre() const
{
	// Calculating global matrix if necessary
	if (parent() != NULL) {
		return (m_shared->localMatrixParent * parent()->matrix()).w_pos;
	} else {
		return m_shared->globalMatrixParent.w_pos;
	}
}

wVector PhyFixed::getForceOnJoint() const
{
	return m_shared->forceOnJoint;
}

void PhyFixed::updateJointInfo()
{
	Shared* const d = m_shared.getModifiableShared();

	//--- calculate the global matrices
	//--- if parent doesn't exist, then globalMatrixParent and localMatrixParent coincide and
	//--- there is no need to re-assign it because it was already done in constructor
	if (parent()) {
		d->globalMatrixParent = m_shared->localMatrixParent * parent()->matrix();
	}
	d->globalMatrixChild = m_shared->localMatrixChild * child()->matrix();

	if (!d->globalMatrixParent.sanityCheck()) {
		qWarning("Sanity Check Failed");
	}
	if (!d->globalMatrixChild.sanityCheck()) {
		qWarning("Sanity Check Failed");
	}
}

void PhyFixed::createPrivateJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	m_priv->joint = NewtonConstraintCreateUserJoint( m_worldPriv->world, 6, PhyJointPrivate::userBilateralHandler, 0, m_priv->child, m_priv->parent );
	NewtonJointSetUserData( m_priv->joint, this );
#endif
}

void PhyFixed::updateJoint(real timestep)
{
	// If the joint is disabled, we don't do anything here
	if (!m_shared->enabled) {
		return;
	}

	// Updating the global matrices and the dof
	updateJointInfo();

#ifdef WORLDSIM_USE_NEWTON
	UNUSED_PARAM( timestep );
	//--- Restrict the movement on the centre of the joint along all tree orthonormal direction
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixParent.x_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0f );
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixParent.y_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0f );
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixParent.z_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0f );

	//--- In order to constraint the rotation about X and Y axis of the joint
	//--- we use LinearRow (that are stronger) getting a point far from objects along
	//--- the Z axis. Doing this if the two object rotates about X or Y axes then
	//--- the difference between qChild and qParent augments and then Newton Engine will apply
	//--- a corresponding force (that applyied outside the centre of the object will become
	//--- a torque) that will blocks any rotation about X and Y axis
	real len = 5000.0;
	wVector qChild( m_shared->globalMatrixChild.w_pos + m_shared->globalMatrixChild.z_ax.scale(len) );
	wVector qParent( m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.z_ax.scale(len) );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.x_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.y_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );

	NewtonUserJointAddAngularRow( m_priv->joint, 0.0f, &m_shared->globalMatrixParent.z_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0f );

	//--- In order to do the same with third axis (Z), I need others point along different axis
/*	qChild = wVector( m_shared->globalMatrixChild.w_pos + m_shared->globalMatrixChild.y_ax.scale(len) );
	qParent = wVector( m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.y_ax.scale(len) );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.z_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );*/

	//--- Retrive forces applied to the joint by the constraints
	Shared* const d = m_shared.getModifiableShared();
	d->forceOnJoint.x = NewtonUserJointGetRowForce (m_priv->joint, 0);
	d->forceOnJoint.y = NewtonUserJointGetRowForce (m_priv->joint, 1);
	d->forceOnJoint.z = NewtonUserJointGetRowForce (m_priv->joint, 2);

#endif
}

RenderPhyFixed::RenderPhyFixed(const PhyFixed* entity)
	: RenderPhyJoint(entity)
{
}

RenderPhyFixed::~RenderPhyFixed()
{
	// Nothing to do here
}

void RenderPhyFixed::render(const PhyFixedShared* sharedData, GLContextAndData* contextAndData)
{
	// Drawing the joint
	const wVector end1 = (sharedData->localMatrixParent.inverse() * sharedData->globalMatrixParent).w_pos;
	const wVector end2 = (sharedData->localMatrixChild.inverse() * sharedData->globalMatrixChild).w_pos;
	drawJoint(sharedData, contextAndData, sharedData->globalMatrixParent.w_pos, end1, end2);
}

} // end namespace salsa
