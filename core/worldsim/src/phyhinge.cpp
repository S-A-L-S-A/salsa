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

#include "phyhinge.h"
#include "mathutils.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhyHinge::PhyHinge(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& axis, const wVector& centre, real startAngle, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	construct(axis, centre, startAngle);
}

PhyHinge::PhyHinge(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& axis, const wVector& centre, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	construct(axis, centre, 0.0);
}

PhyHinge::PhyHinge(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& axis, real startAngle, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	construct(axis, wVector(0.0, 0.0, 0.0), startAngle);
}

PhyHinge::PhyHinge(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& axis, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	construct(axis, wVector(0.0, 0.0, 0.0), 0);
}

PhyHinge::PhyHinge(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& axis, const wVector& centre, const wVector& x_axis, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();

	// Creating the DOFs
	createDOFs(PhyDOFInitParamsList() << PhyDOFInitParams(axis, centre, false));
	PhyDOFShared& dof = d->dofs[0];

	// Calculating the localMatrixParent supposing that axis and x_axis are ortogonal
	d->localMatrixParent.x_ax = x_axis;
	d->localMatrixParent.y_ax = axis * x_axis;
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

PhyHinge::~PhyHinge()
{
	// Nothing to do here
}

void PhyHinge::construct(const wVector& axis, const wVector& centre, real startAngle)
{
	Shared* const d = m_shared.getModifiableShared();

	// Creating the DOFs
	createDOFs(PhyDOFInitParamsList() << PhyDOFInitParams(axis, centre, false));
	PhyDOFShared& dof = d->dofs[0];

	// Calculating the localMatrixParent using the Gramm-Schmidt prcedure
	d->localMatrixParent = wMatrix::grammSchmidt(axis);

	// Calculating the axis that connects the centre of joint with centre of child object
	wVector start = centre;
	wVector end = m_child->matrix().w_pos;
	if (m_parent != nullptr) {
		start -= m_parent->matrix().w_pos;
	}
	wVector newx = (start - end).normalize();
	if (isnan(newx[0]) || isnan(newx[1]) || isnan(newx[2])) {
		// This happens when it is not possible to calculate the axis above, in this case nothing has to be done
	} else {
		// Rotating the matrix in order to align X axis calculated from grammSchmidt procedure to the axis connecting
		// centres. In this way, angle zero correspond to position at which child object was connected by this joint
		const real sinAngle = (newx * d->localMatrixParent.x_ax) % d->localMatrixParent.z_ax;
		const real cosAngle = newx % d->localMatrixParent.x_ax;
		const real angle = atan2(sinAngle, cosAngle);
		const wQuaternion ql(axis, angle);
		d->localMatrixParent = m_shared->localMatrixParent * wMatrix(ql, wVector(0.0, 0.0, 0.0));
	}
	d->localMatrixParent.w_pos = centre;
	d->localMatrixParent.sanitifize();

	// Calculating the local matrix respect to child object
	if (m_parent != nullptr) {
		d->globalMatrixParent = m_shared->localMatrixParent * m_parent->matrix();
	} else {
		d->globalMatrixParent = m_shared->localMatrixParent;
	}
	d->localMatrixChild = m_shared->globalMatrixParent * m_child->matrix().inverse();

	d->localMatrixParent.w_pos = wVector(0.0, 0.0, 0.0);
	// This will rotate the localMatrixParent in order to align the axis with the requested startAngle
	wQuaternion q(axis, startAngle);
	d->localMatrixParent = m_shared->localMatrixParent * wMatrix(q, wVector(0.0, 0.0, 0.0));
	d->localMatrixParent.w_pos = centre;
	if (m_parent != nullptr) {
		d->globalMatrixParent = m_shared->localMatrixParent * m_parent->matrix();
	} else {
		d->globalMatrixParent = m_shared->localMatrixParent;
	}

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

wVector PhyHinge::centre() const
{
	// Calculating global matrix if necessary
	if (parent() != nullptr) {
		return (m_shared->localMatrixParent * parent()->matrix()).w_pos;
	} else {
		return m_shared->globalMatrixParent.w_pos;
	}
}

wVector PhyHinge::getForceOnJoint() const
{
	return m_shared->forceOnJoint;
}

void PhyHinge::updateJointInfo()
{
	Shared* const d = m_shared.getModifiableShared();

	//--- calculate the global matrices
	//--- if parent doesn't exist, then globalMatrixParent and localMatrixParent coincide and
	//--- there is no need to re-assign it because it was already done in constructor
	if (parent() != nullptr) {
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

	//--- calculate the rotation assuming the local X axis of joint frame as 0
	//--- and following the right-hand convention
	real sinAngle = (m_shared->globalMatrixParent.x_ax * m_shared->globalMatrixChild.x_ax) % m_shared->globalMatrixChild.z_ax;
	real cosAngle = m_shared->globalMatrixChild.x_ax % m_shared->globalMatrixParent.x_ax;
	real angle = atan2(sinAngle, cosAngle);
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
		if (parent()) {
			NewtonBodyGetOmega(m_priv->parent, &omegaParent[0]);
		} else {
			omegaParent = wVector(0, 0, 0);
		}
		NewtonBodyGetOmega(m_priv->child, &omegaChild[0]);
#endif
		real velP = omegaParent % m_shared->globalMatrixChild.z_ax;
		real velC = omegaChild % m_shared->globalMatrixChild.z_ax;
		vel = velC - velP;
		//vel = (angle - dof->position()) / (world()->timeStep());
	} else {
		vel = (angle - dofShared.position) / (world()->timeStep());
	}
	dof->setPosition(angle);
	dof->setVelocity(vel);
}

void PhyHinge::createPrivateJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	m_priv->joint = NewtonConstraintCreateUserJoint( m_worldPriv->world, 6, PhyJointPrivate::userBilateralHandler, 0, m_priv->child, m_priv->parent );
	NewtonJointSetUserData( m_priv->joint, this );
#endif
}

void PhyHinge::updateJoint(real timestep)
{
	// If the joint is disabled, we don't do anything here
	if (!m_shared->enabled) {
		return;
	}

	// Updating the global matrices and the dof
	updateJointInfo();

	PhyDOF* const dof = m_dofs[0];

	const real angle = dof->position();
	const real vel = dof->velocity();

#ifdef WORLDSIM_USE_NEWTON
	//--- Restrict the movement on the centre of the joint along all tree orthonormal direction
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixChild.x_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixChild.y_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &m_shared->globalMatrixChild.w_pos[0], &m_shared->globalMatrixParent.w_pos[0], &m_shared->globalMatrixChild.z_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );

	//--- In order to constraint the rotation about X and Y axis of the joint
	//--- we use LinearRow (that are stronger) getting a point far from objects along
	//--- the Z axis. Doing this if the two object rotates about X or Y axes then
	//--- the difference between qChild and qParent augments and then Newton Engine will apply
	//--- a corresponding force (that applyied outside the centre of the object will become
	//--- a torque) that will blocks any rotation about X and Y axis
	const real len = 50000.0;
	wVector qChild( m_shared->globalMatrixChild.w_pos + m_shared->globalMatrixChild.z_ax.scale(len) );
	wVector qParent( m_shared->globalMatrixParent.w_pos + m_shared->globalMatrixParent.z_ax.scale(len) );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixChild.x_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &qChild[0], &qParent[0], &m_shared->globalMatrixChild.y_ax[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );

	bool atLimit = false;
	if ( dof->isLimited() ) {
		real lolimit;
		real hilimit;
		dof->limits( lolimit, hilimit );
		if ( angle < lolimit ) {
			real relAngle = lolimit - angle; // - lolimit;
			//--- the command is opposite to the movement required to go away from the limit
			//    so, it will be overrided for going away from the limit
			NewtonUserJointAddAngularRow( m_priv->joint, relAngle, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
			NewtonUserJointSetRowSpringDamperAcceleration( m_priv->joint, 50000.0, 100 );
			//--- this allow the joint to move back freely
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, 0.0 );
			atLimit = true;
		} else if ( angle > hilimit ) {
			real relAngle = hilimit - angle; // - hilimit;
			//--- the command is opposite to the movement required to go away from the limit
			//    so, it will be overrided for going away from the limit
			NewtonUserJointAddAngularRow( m_priv->joint, relAngle, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
			NewtonUserJointSetRowSpringDamperAcceleration( m_priv->joint, 50000.0, 100 );
			//--- this allow the joint to move forth freely
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, 0.0 );
			atLimit = true;
		}
	}

	//--- if it reach this point means that the joint if far from limits
	//--- and we check for type of motion and we'll apply the corresponding entity
	if (!atLimit) {
	real force, accel, wishangle, wishvel, sign;
		switch( dof->motion() ) {
		case PhyDOFShared::Force:
			//--- force will be used to set-up max and min torque newton will use to
			//--- resolve the constraint and accel to the double of appliedForce
			//--- this means that Newton will apply exactly forse amount of torque around
			//--- the axis
			force = fabs( dof->appliedForce() );
			if ( ( dof->maxForce() > 0.0 ) && ( force > dof->maxForce() ) ) {
				force = dof->maxForce();
			}
			accel = 2.0*dof->appliedForce();
			NewtonUserJointAddAngularRow( m_priv->joint, 0.0, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, -force );
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, +force );
			NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
			NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
			break;
		case PhyDOFShared::Velocity:
			//--- I'm not sure that this implementation is correct
			accel = 0.8*( dof->desiredVelocity() - vel ) / ( timestep );
			wishangle = (dof->desiredVelocity()*timestep);
			NewtonUserJointAddAngularRow( m_priv->joint, wishangle, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
			NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
			if ( dof->maxForce() > 0.0 ) {
				NewtonUserJointSetRowMinimumFriction( m_priv->joint, -dof->maxForce() );
				NewtonUserJointSetRowMaximumFriction( m_priv->joint, +dof->maxForce() );
			}
			break;
		case PhyDOFShared::Position:
			wishangle = dof->desiredPosition() - angle;
			wishvel = wishangle / timestep;
			if ( fabs(wishvel) > dof->maxVelocity() ) {
				sign = (wishvel<0) ? -1.0 : +1.0;
				wishvel = sign*dof->maxVelocity();
			}
			accel = 0.8*( wishvel - vel ) / ( timestep );
			NewtonUserJointAddAngularRow( m_priv->joint, wishangle, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
			NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
			if ( dof->maxForce() > 0.0 ) {
				NewtonUserJointSetRowMinimumFriction( m_priv->joint, -dof->maxForce() );
				NewtonUserJointSetRowMaximumFriction( m_priv->joint, +dof->maxForce() );
			}
			//NewtonUserJointAddAngularRow( m_priv->joint, wishangle, &globalMatrixChild.z_ax[0] );
			//NewtonUserJointSetRowSpringDamperAcceleration( m_priv->joint, 30000.0, 2000 );
			//NewtonUserJointSetRowStiffness( m_priv->joint, dof->stiffness() );
			break;
		case PhyDOFShared::Off:
		default:
			break;
		}
	}

	//--- Retrive forces applied to the joint by the constraints
	m_shared->forceOnJoint.x = NewtonUserJointGetRowForce (m_priv->joint, 0);
	m_shared->forceOnJoint.y = NewtonUserJointGetRowForce (m_priv->joint, 1);
	m_shared->forceOnJoint.z = NewtonUserJointGetRowForce (m_priv->joint, 2);
#endif
}

RenderPhyHinge::RenderPhyHinge(const PhyHinge* entity)
	: RenderPhyJoint(entity)
{
}

RenderPhyHinge::~RenderPhyHinge()
{
	// Nothing to do here
}

void RenderPhyHinge::render(const PhyHingeShared* sharedData, GLContextAndData* contextAndData)
{
	// Drawing the joint
	const wVector end1 = (sharedData->localMatrixParent.inverse() * sharedData->globalMatrixParent).w_pos;
	const wVector end2 = (sharedData->localMatrixChild.inverse() * sharedData->globalMatrixChild).w_pos;
	drawJoint(sharedData, contextAndData, sharedData->globalMatrixParent.w_pos, end1, end2);
}

} // end namespace salsa
