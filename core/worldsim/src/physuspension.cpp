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

#include "physuspension.h"
#include "mathutils.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhySuspension::PhySuspension(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child,  const wVector& axis, const wVector& centre, const wVector& xAxis, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();

	// Creating the DOFs
	createDOFs(PhyDOFInitParamsList() << PhyDOFInitParams(axis, centre, false));
	PhyDOFShared& dof = d->dofs[0];

	// Calculating the localMatrixParent supposing that axis and x_axis are ortogonals
	d->localMatrixParent.x_ax = xAxis;
	d->localMatrixParent.y_ax = axis * xAxis;
	d->localMatrixParent.z_ax = axis;
	d->localMatrixParent.w_pos = centre;
	d->localMatrixParent.sanitifize();

	// Calculating the local matrix respect to child object
	if (m_parent != nullptr) {
		d->globalMatrixParent = m_shared->localMatrixParent * m_parent->matrix();
	} else {
		d->globalMatrixParent = m_shared->localMatrixParent;
	}
	d->localMatrixChild = m_shared->globalMatrixParent * m_child->matrix().inverse();

	dof.axis = d->globalMatrixParent.z_ax;
	dof.xAxis = d->globalMatrixParent.x_ax;
	dof.yAxis = d->globalMatrixParent.y_ax;
	dof.centre = d->globalMatrixParent.w_pos;

	if (!d->localMatrixParent.sanityCheck() ){
		qWarning("Sanity Check Failed on localMatrixParent");
	}
	if (!d->localMatrixChild.sanityCheck()) {
		qWarning("Sanity Check Failed on localMatrixChild");
	}
}

PhySuspension::~PhySuspension()
{
	// Nothing to do here
}

wVector PhySuspension::centre() const
{
	// Calculating global matrix if necessary
	if (parent() != nullptr) {
		return (m_shared->localMatrixParent * parent()->matrix()).w_pos;
	} else {
		return m_shared->globalMatrixParent.w_pos;
	}
}

wVector PhySuspension::getForceOnJoint() const
{
	return m_shared->forceOnJoint;
}

void PhySuspension::updateJointInfo()
{
	Shared* const d = m_shared.getModifiableShared();

	//--- calculate the global matrices
	//--- if parent doesn't exist, then globalMatrixParent and localMatrixParent coincide and
	//--- there is no need to re-assign it because it was already done in constructor
	if ( parent() != nullptr ) {
		d->globalMatrixParent = m_shared->localMatrixParent * parent()->matrix();
	}
	d->globalMatrixChild = m_shared->localMatrixChild * child()->matrix();

	if ( !d->globalMatrixParent.sanityCheck() ) {
		qWarning( "Sanity Check Failed on globalMatrixParent" );
	}
	if ( !d->globalMatrixChild.sanityCheck() ) {
		qWarning( "Sanity Check Failed on globalMatrixChild" );
	}

	PhyDOF* const dof = m_dofs[0];
	PhyDOFShared& dofShared = d->dofs[0];

	dofShared.axis = m_shared->globalMatrixParent.z_ax;
	dofShared.xAxis = m_shared->globalMatrixParent.x_ax;
	dofShared.yAxis = m_shared->globalMatrixParent.y_ax;
	dofShared.centre = m_shared->globalMatrixParent.w_pos;

	//--- calculate the rotation assuming the local X axis of joint frame as 0
	//--- and following the right-hand convention
	real sinAngle = (m_shared->globalMatrixParent.x_ax * m_shared->globalMatrixChild.x_ax) % m_shared->globalMatrixChild.z_ax;
	real cosAngle = m_shared->globalMatrixChild.x_ax % m_shared->globalMatrixParent.x_ax;
	real angle = atan2( sinAngle, cosAngle );
	real vel;
	// Velocity is computed in two different ways depending on whether the joint is enabled or not:
	// if it is, we use Newton functions to get the angular velocities (for a better result), otherwise
	// we revert to computing the difference with the previous position divided by the timestep
	if (m_shared->enabled) {
		//--- the velocity is calculated projecting the angular velocity of objects on the main axis (z_ax)
		//    This code is not general, becuase it is not appliable in the case of parent==nullptr
		//    and also return different results respect to the kinematic way
		//    Then [Gianluca] comment it and replaced with the same code used in the kinematic mode
		wVector omegaParent, omegaChild;
#ifdef WORLDSIM_USE_NEWTON
		if ( parent() ) {
			NewtonBodyGetOmega( m_priv->parent, &omegaParent[0] );
		} else {
			omegaParent = wVector(0, 0, 0);
		}
		NewtonBodyGetOmega( m_priv->child, &omegaChild[0] );
#endif
		real velP = omegaParent % m_shared->globalMatrixChild.z_ax;
		real velC = omegaChild % m_shared->globalMatrixChild.z_ax;
		vel = velC - velP;
		//vel = (angle - dof->position()) / (world()->timeStep());
	} else {
		vel = (angle - dofShared.position) / (world()->timeStep());
	}
	dof->setPosition( angle );
	dof->setVelocity( vel );
}

void PhySuspension::createPrivateJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	m_priv->joint = NewtonConstraintCreateUserJoint( m_worldPriv->world, 6, PhyJointPrivate::userBilateralHandler, 0, m_priv->child, m_priv->parent );
	NewtonJointSetUserData( m_priv->joint, this );
#endif
}

void PhySuspension::updateJoint(real timestep)
{
	// If the joint is disabled, we don't do anything here
	if (!m_shared->enabled) {
		return;
	}

	// Updating the global matrices and the dof
	updateJointInfo();

	PhyDOF* const dof = m_dofs[0];

	// const real angle = dof->position();
	const real vel = dof->velocity();

#ifdef WORLDSIM_USE_NEWTON
	//--- Restrict the movement on the centre of the joint along all tree orthonormal direction
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixParent.x_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 0.98f );
	//NewtonUserJointSetRowSpringDamperAcceleration( m_priv->joint, 20, 10 );
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
	const real len = 5000.0;
	wVector qChild( m_shared->globalMatrixChild.w_pos + m_shared->globalMatrixChild.z_ax.scale(len) );
	wVector qParent( m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.z_ax.scale(len) );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.x_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.y_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
// 	NewtonUserJointAddAngularRow( m_priv->joint, 0.0f, &m_shared->globalMatrixParent.x_ax[0] );
// 	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0f );
// 	NewtonUserJointAddAngularRow( m_priv->joint, 0.0f, &m_shared->globalMatrixParent.y_ax[0] );
// 	NewtonUserJointSetRowStiffness( m_priv->joint, 0.98f );

	//--- The only motion supported is by velocity
	if ( dof->motion() == PhyDOFShared::Velocity ) {
		//--- I'm not sure that this implementation is correct
		real accel = 0.8f*( dof->desiredVelocity() - vel ) / ( timestep );
		NewtonUserJointAddAngularRow( m_priv->joint, 0.0f, &m_shared->globalMatrixParent.z_ax[0] );
		NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
		NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
		if ( dof->maxForce() > 0.0 ) {
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, -dof->maxForce() );
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, +dof->maxForce() );
		}
	}

	//--- Retrive forces applied to the joint by the constraints
	Shared* const d = m_shared.getModifiableShared();
	d->forceOnJoint.x = NewtonUserJointGetRowForce (m_priv->joint, 0);
	d->forceOnJoint.y = NewtonUserJointGetRowForce (m_priv->joint, 1);
	d->forceOnJoint.z = NewtonUserJointGetRowForce (m_priv->joint, 2);
#endif
}

RenderPhySuspension::RenderPhySuspension(const PhySuspension* entity)
	: RenderPhyJoint(entity)
{
}

RenderPhySuspension::~RenderPhySuspension()
{
	// Nothing to do here
}

void RenderPhySuspension::render(const PhySuspensionShared* sharedData, GLContextAndData* contextAndData)
{
	// Drawing the joint
	const wVector end1 = (sharedData->localMatrixParent.inverse() * sharedData->globalMatrixParent).w_pos;
	const wVector end2 = (sharedData->localMatrixChild.inverse() * sharedData->globalMatrixChild).w_pos;
	drawJoint(sharedData, contextAndData, sharedData->globalMatrixParent.w_pos, end1, end2);
}

} // end namespace salsa
