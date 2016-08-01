/********************************************************************************
 *  WorldSim -- library for robot simulations                                   *
 *  Copyright (C) 2008-2013 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "physlider.h"
#include "mathutils.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhySlider::PhySlider(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& axis, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();

	// Creating the DOFs
	createDOFs(PhyDOFInitParamsList() << PhyDOFInitParams(axis, wVector(), true));
	PhyDOFShared& dof = d->dofs[0];

	// Calculating the localMatrixParent using the Gramm-Schmidt prcedure
	d->localMatrixParent = wMatrix::grammSchmidt(axis);

	// Calculating the local matrix respect to child object
	if (m_parent != NULL) {
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

PhySlider::~PhySlider()
{
	// Nothing to do here
}

wVector PhySlider::centre() const
{
	// Calculating global matrix if necessary
	if (parent() != NULL) {
		return (m_shared->localMatrixParent * parent()->matrix()).w_pos;
	} else {
		return m_shared->globalMatrixParent.w_pos;
	}
}

wVector PhySlider::getForceOnJoint() const
{
	return m_shared->forceOnJoint;
}

void PhySlider::updateJointInfo()
{
	Shared* const d = m_shared.getModifiableShared();

	//--- calculate the global matrices
	//--- if parent doesn't exist, then globalMatrixParent and localMatrixParent coincide and
	//--- there is no need to re-assign it because it was already done in constructor
	if (parent() != NULL) {
		d->globalMatrixParent = m_shared->localMatrixParent * parent()->matrix();
	}
	d->globalMatrixChild = m_shared->localMatrixChild * child()->matrix();

	if (!d->globalMatrixParent.sanityCheck()) {
		qWarning("Sanity Check Failed on globalMatrixParent");
	}
	if (!d->globalMatrixChild.sanityCheck()) {
		qWarning("Sanity Check Failed on globalMatrixChild");
	}

	PhyDOF* const dof = m_dofs[0];
	PhyDOFShared& dofShared = d->dofs[0];

	dofShared.axis = m_shared->globalMatrixParent.z_ax;
	dofShared.xAxis = m_shared->globalMatrixParent.x_ax;
	dofShared.yAxis = m_shared->globalMatrixParent.y_ax;
	dofShared.centre = m_shared->globalMatrixParent.w_pos;

	real pos = (m_shared->globalMatrixChild.w_pos - m_shared->globalMatrixParent.w_pos) % m_shared->globalMatrixParent.z_ax;

	real vel;
	// Velocity is computed in two different ways depending on whether the joint is enabled or not:
	// if it is, we use Newton functions to get the linear velocities (for a better result), otherwise
	// we revert to computing the difference with the previous position divided by the timestep
	if (m_shared->enabled) {
		wVector velocityParent, velocityChild;
#ifdef WORLDSIM_USE_NEWTON
		if (parent() != NULL) {
			NewtonBodyGetVelocity(m_priv->parent, &velocityParent[0]);
		} else {
			velocityParent = wVector(0, 0, 0);
		}
		NewtonBodyGetVelocity(m_priv->child, &velocityChild[0]);
#endif
		real velP = velocityParent % m_shared->globalMatrixChild.z_ax;
		real velC = velocityChild %m_shared-> globalMatrixChild.z_ax;
		vel = velC - velP;
	} else {
		vel = (pos - dofShared.position) / (world()->timeStep());
	}
	dof->setPosition(pos);
	dof->setVelocity(vel);
}

void PhySlider::createPrivateJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	m_priv->joint = NewtonConstraintCreateUserJoint(m_worldPriv->world, 6, PhyJointPrivate::userBilateralHandler, 0, m_priv->child, m_priv->parent);
	NewtonJointSetUserData(m_priv->joint, this);
#endif
}

void PhySlider::updateJoint(real timestep)
{
	// If the joint is disabled, we don't do anything here
	if (!m_shared->enabled) {
		return;
	}

	// Updating the global matrices and the dof
	updateJointInfo();

	PhyDOF* const dof = m_dofs[0];

	const real pos = dof->position();
	const real vel = dof->velocity();

#ifdef WORLDSIM_USE_NEWTON

	wVector childPos = m_shared->globalMatrixChild.w_pos;
	//--- Restrict the movement on the centre of the joint along two orthonormal direction
	NewtonUserJointAddLinearRow(m_priv->joint, &childPos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixParent.x_ax[0]);
	NewtonUserJointSetRowStiffness(m_priv->joint, 1.0f);
	NewtonUserJointAddLinearRow(m_priv->joint, &childPos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixParent.y_ax[0]);
	NewtonUserJointSetRowStiffness(m_priv->joint, 1.0f);

	//--- In order to constraint the rotation about X and Y axis of the joint
	//--- we use LinearRow (that are stronger) getting a point far from objects along
	//--- the Z axis. Doing this if the two object rotates about X or Y axes then
	//--- the difference between qChild and qParent augments and then Newton Engine will apply
	//--- a corresponding force (that applyied outside the centre of the object will become
	//--- a torque) that will blocks any rotation about X and Y axis
	const real len = 5000.0;
	wVector qChild(m_shared->globalMatrixChild.w_pos + m_shared->globalMatrixChild.z_ax.scale(len));
	wVector qParent(m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.z_ax.scale(len));
	NewtonUserJointAddLinearRow(m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.x_ax[0]);
	NewtonUserJointSetRowStiffness(m_priv->joint, 1.0);
	NewtonUserJointAddLinearRow(m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.y_ax[0]);
	NewtonUserJointSetRowStiffness(m_priv->joint, 1.0);

	qChild = m_shared->globalMatrixChild.w_pos + m_shared->globalMatrixChild.x_ax.scale(len);
	qParent = m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.x_ax.scale(len);
	NewtonUserJointAddLinearRow(m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixParent.y_ax[0]);
	NewtonUserJointSetRowStiffness(m_priv->joint, 1.0);

	// check limits
	if(dof->isLimited()) {
		real lolimit;
		real hilimit;
		dof->limits(lolimit, hilimit);
		if(pos < lolimit) {
			wVector limitPos = m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.z_ax.scale(lolimit);
			NewtonUserJointAddLinearRow(m_priv->joint, &childPos[0], &limitPos[0], &m_shared->globalMatrixChild.z_ax[0]);
			NewtonUserJointSetRowStiffness(m_priv->joint, 1.0f);
			return;
		} else if (pos > hilimit) {
			wVector limitPos = m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.z_ax.scale(hilimit);
			NewtonUserJointAddLinearRow(m_priv->joint, &childPos[0], &limitPos[0], &m_shared->globalMatrixChild.z_ax[0]);
			NewtonUserJointSetRowStiffness(m_priv->joint, 1.0f);
			return;
		}
	}

	//--- if it reach this point means that the joint if far from limits
	//--- and we check for type of motion and we'll apply the corresponding entity
	real force, accel, mass;
	switch(dof->motion()) {
	case PhyDOFShared::Force:
		//--- force will be used to set-up max and min torque newton will use to
		//--- resolve the constraint and accel to the double of appliedForce
		//--- this means that Newton will apply exactly forse amount of torque around
		//--- the axis

		// ======================= VECCHIA VERSIONE ========================
// 		force = dof->appliedForce();
// 		if (dof->maxForce() > 0.0) {
// 			if (force > dof->maxForce()) {
// 				force = dof->maxForce();
// 			} else if (force < -dof->maxForce()) {
// 				force = -dof->maxForce();
// 			}
// 		}
// 		NewtonUserJointAddLinearRow( priv->joint, &globalMatrixChild.w_pos[0], &globalMatrixParent.w_pos[0], &globalMatrixChild.z_ax[0] );
// 		mass = child()->mass();
// 		accel = force / mass;
// 		NewtonUserJointSetRowAcceleration( priv->joint, accel );
// 		printf("--------- JOINT accel: %f, force: %f, mass: %f, maxForce: %f\n", accel, force, mass, dof->maxForce());

		// ======================= NUOVA VERSIONE ========================
		force = fabs(dof->appliedForce());
		if ((dof->maxForce() > 0.0) && (force > dof->maxForce())) {
			force = dof->maxForce();
		}
		accel = 2.0 * dof->appliedForce();
// 		NewtonUserJointAddLinearRow( priv->joint, &globalMatrixChild.w_pos[0], &globalMatrixParent.w_pos[0], &globalMatrixChild.z_ax[0] );
		NewtonUserJointAddLinearRow(m_priv->joint, &(m_shared->globalMatrixChild.w_pos - m_shared->globalMatrixChild.z_ax)[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixChild.z_ax[0]);
// 		printf("--------- pivot0: %f %f %f, pivot1: %f %f %f, asse: %f %f %f\n", globalMatrixChild.w_pos.x, globalMatrixChild.w_pos.y, globalMatrixChild.w_pos.z,
// 		       globalMatrixParent.w_pos.x, globalMatrixParent.w_pos.y, globalMatrixParent.w_pos.z,
// 			globalMatrixChild.z_ax.x, globalMatrixChild.z_ax.y, globalMatrixChild.z_ax.z);
		NewtonUserJointSetRowMinimumFriction(m_priv->joint, -force);
		NewtonUserJointSetRowMaximumFriction(m_priv->joint, +force);
		NewtonUserJointSetRowAcceleration(m_priv->joint, accel);
		NewtonUserJointSetRowStiffness(m_priv->joint, dof->stiffness());

// 		printf("--------- JOINT accel: %f, force: %f, maxForce: %f, stiffness: %f\n", accel, force, dof->maxForce(), dof->stiffness());

		break;
	case PhyDOFShared::Velocity:
// 		//--- I'm not sure that this implementation is correct
// 		accel = 0.8*( dof->desiredVelocity() - vel ) / ( timestep );
// 		wishangle = (dof->desiredVelocity()*timestep);
// 		NewtonUserJointAddAngularRow( priv->joint, wishangle, &globalMatrixChild.z_ax[0] );
// 		NewtonUserJointSetRowAcceleration( priv->joint, accel );
// 		NewtonUserJointSetRowStiffness( priv->joint, dof->stiffness() );
// 		if ( dof->maxForce() > 0.0 ) {
// 			NewtonUserJointSetRowMinimumFriction( priv->joint, -dof->maxForce() );
// 			NewtonUserJointSetRowMaximumFriction( priv->joint, +dof->maxForce() );
// 		}
		break;
	case PhyDOFShared::Position:
// 		wishangle = dof->desiredPosition() - angle;
// 		wishvel = wishangle / timestep;
// 		if ( fabs(wishvel) > dof->maxVelocity() ) {
// 			sign = (wishvel<0) ? -1.0 : +1.0;
// 			wishvel = sign*dof->maxVelocity();
// 		}
// 		accel = 0.8*( wishvel - vel ) / ( timestep );
// 		NewtonUserJointAddAngularRow( priv->joint, wishangle, &globalMatrixChild.z_ax[0] );
// 		NewtonUserJointSetRowAcceleration( priv->joint, accel );
// 		NewtonUserJointSetRowStiffness( priv->joint, dof->stiffness() );
// 		if ( dof->maxForce() > 0.0 ) {
// 			NewtonUserJointSetRowMinimumFriction( priv->joint, -dof->maxForce() );
// 			NewtonUserJointSetRowMaximumFriction( priv->joint, +dof->maxForce() );
// 		}
// 		//NewtonUserJointAddAngularRow( priv->joint, wishangle, &globalMatrixChild.z_ax[0] );
// 		//NewtonUserJointSetRowSpringDamperAcceleration( priv->joint, 30000.0, 2000 );
// 		//NewtonUserJointSetRowStiffness( priv->joint, dof->stiffness() );
		break;
	case PhyDOFShared::Off:
// 	default:
		break;
	}

	//--- Retrive forces applied to the joint by the constraints
#warning THIS IS NOT UPDATE IF THE JOINT REACHES THE LIMITS
	m_shared->forceOnJoint.x = NewtonUserJointGetRowForce(m_priv->joint, 0);
	m_shared->forceOnJoint.y = NewtonUserJointGetRowForce(m_priv->joint, 1);
	m_shared->forceOnJoint.z = NewtonUserJointGetRowForce(m_priv->joint, 2);
#endif
}

RenderPhySlider::RenderPhySlider(const PhySlider* entity)
	: RenderPhyJoint(entity)
{
}

RenderPhySlider::~RenderPhySlider()
{
	// Nothing to do here
}

void RenderPhySlider::render(const PhySliderShared* sharedData, GLContextAndData* contextAndData)
{
	// Drawing the joint
	const wVector end1 = (sharedData->localMatrixParent.inverse() * sharedData->globalMatrixParent).w_pos;
	const wVector end2 = (sharedData->localMatrixChild.inverse() * sharedData->globalMatrixChild).w_pos;
	drawJoint(sharedData, contextAndData, sharedData->globalMatrixParent.w_pos, end1, end2);
}

} // end namespace salsa
