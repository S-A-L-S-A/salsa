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

#include "phyballandsocket.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhyBallAndSocket::PhyBallAndSocket(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child,  const wVector& centre, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();

	// This joint has not dof: we constrain the center to stay fixed but do not allow actuating any other axis

	// Calculating the localMatrixParent
	d->localMatrixParent.x_ax = wVector::X();
	d->localMatrixParent.y_ax = wVector::Y();
	d->localMatrixParent.z_ax = wVector::Z();
	d->localMatrixParent.w_pos = centre;
	d->localMatrixParent.sanitifize();

	// Calculating the local matrix respect to child object
	if (m_parent != nullptr) {
		d->globalMatrixParent = m_shared->localMatrixParent * m_parent->matrix();
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

PhyBallAndSocket::~PhyBallAndSocket()
{
	// Nothing to do here
}

wVector PhyBallAndSocket::centre() const
{
	// Calculating global matrix if necessary
	if (m_parent != nullptr) {
		return (m_shared->localMatrixParent * parent()->matrix()).w_pos;
	} else {
		return m_shared->globalMatrixParent.w_pos;
	}
}

wVector PhyBallAndSocket::getForceOnJoint() const
{
	return m_shared->forceOnJoint;
}

void PhyBallAndSocket::updateJointInfo()
{
	Shared* const d = m_shared.getModifiableShared();

	//--- calculate the global matrices
	//--- if parent doesn't exist, then globalMatrixParent and localMatrixParent coincide and
	//--- there is no need to re-assign it because it was already done in constructor
	if ( parent() ) {
		d->globalMatrixParent = m_shared->localMatrixParent * m_parent->matrix();
	}
	d->globalMatrixChild = m_shared->localMatrixChild * m_child->matrix();

	if ( !d->globalMatrixParent.sanityCheck() ) {
		qWarning( "Sanity Check Failed" );
	}
	if ( !d->globalMatrixChild.sanityCheck() ) {
		qWarning( "Sanity Check Failed" );
	}
}

void PhyBallAndSocket::createPrivateJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	m_priv->joint = NewtonConstraintCreateUserJoint(m_worldPriv->world, 6, PhyJointPrivate::userBilateralHandler, 0, m_priv->child, m_priv->parent);
	NewtonJointSetUserData(m_priv->joint, this);
#endif
}

void PhyBallAndSocket::updateJoint(real timestep)
{
	// If the joint is disabled, we don't do anything here
	if (!m_shared->enabled) {
		return;
	}

	// Updating the global matrices
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

	//--- Retrive forces applied to the joint by the constraints
	Shared* const d = m_shared.getModifiableShared();
	d->forceOnJoint.x = NewtonUserJointGetRowForce (m_priv->joint, 0);
	d->forceOnJoint.y = NewtonUserJointGetRowForce (m_priv->joint, 1);
	d->forceOnJoint.z = NewtonUserJointGetRowForce (m_priv->joint, 2);
#endif
}

RenderPhyBallAndSocket::RenderPhyBallAndSocket(const PhyBallAndSocket* entity)
	: RenderPhyJoint(entity)
{
}

RenderPhyBallAndSocket::~RenderPhyBallAndSocket()
{
	// Nothing to do here
}

void RenderPhyBallAndSocket::render(const PhyBallAndSocketShared* sharedData, GLContextAndData* contextAndData)
{
	// Drawing the joint
	const wVector end1 = (sharedData->localMatrixParent.inverse() * sharedData->globalMatrixParent).w_pos;
	const wVector end2 = (sharedData->localMatrixChild.inverse() * sharedData->globalMatrixChild).w_pos;
	drawJoint(sharedData, contextAndData, sharedData->globalMatrixParent.w_pos, end1, end2);
}

} // end namespace salsa
