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

#include "phyuniversal.h"
#include "private/phyjointprivate.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhyUniversal::PhyUniversal(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& firstAxis, const wVector& secondAxis, const wVector& centre, QString name)
	: PhyJoint(world, shared, parent, child, name)
	, m_shared(shared)
{
	Shared* const d = m_shared.getModifiableShared();

	// Creating the DOFs
	createDOFs(PhyDOFInitParamsList() << PhyDOFInitParams(firstAxis, centre, false) << PhyDOFInitParams(secondAxis, centre, false));
	PhyDOFShared& firstDof = d->dofs[0];
	PhyDOFShared& secondDof = d->dofs[1];

	// Calculating the localMatrixParent supposing that firstAxis and secondAxis are ortogonals
	d->localMatrixParent.x_ax = secondAxis;
	d->localMatrixParent.y_ax = firstAxis * secondAxis;
	d->localMatrixParent.z_ax = firstAxis;
	d->localMatrixParent.w_pos = centre;
	d->localMatrixParent.sanitifize();

	// Calculating the local matrix respect to child object
	if (m_parent != nullptr) {
		d->globalMatrixParent = m_shared->localMatrixParent * m_parent->matrix();
	} else {
		d->globalMatrixParent = m_shared->localMatrixParent;
	}
	d->localMatrixChild = m_shared->globalMatrixParent * m_child->matrix().inverse();

	firstDof.axis = d->globalMatrixParent.z_ax;
	firstDof.xAxis = d->globalMatrixParent.x_ax;
	firstDof.yAxis = d->globalMatrixParent.y_ax;
	firstDof.centre = d->globalMatrixParent.w_pos;

	d->globalMatrixChild = m_shared->localMatrixChild * m_child->matrix();
	secondDof.axis = d->globalMatrixParent.x_ax;
	secondDof.xAxis = d->globalMatrixParent.z_ax;
	secondDof.yAxis = d->globalMatrixParent.y_ax;
	secondDof.centre = d->globalMatrixParent.w_pos;

	if (!d->localMatrixParent.sanityCheck() ){
		qWarning("Sanity Check Failed on localMatrixParent");
	}
	if (!d->localMatrixChild.sanityCheck()) {
		qWarning("Sanity Check Failed on localMatrixChild");
	}
}

PhyUniversal::~PhyUniversal()
{
	// Nothing to do here
}

wVector PhyUniversal::centre() const
{
	// Calculating global matrix if necessary
	if (parent() != nullptr) {
		return (m_shared->localMatrixParent * parent()->matrix()).w_pos;
	} else {
		return m_shared->globalMatrixParent.w_pos;
	}
}

wVector PhyUniversal::getForceOnJoint() const
{
	return m_shared->forceOnJoint;
}

void PhyUniversal::updateJointInfo()
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
		qWarning( "Sanity Check Failed" );
	}
	if ( !d->globalMatrixChild.sanityCheck() ) {
		qWarning( "Sanity Check Failed" );
	}

	PhyDOF* const firstDof = m_dofs[0];
	PhyDOFShared& firstDofShared = d->dofs[0];
	PhyDOF* const secondDof = m_dofs[1];
	PhyDOFShared& secondDofShared = d->dofs[1];

	firstDofShared.axis = m_shared->globalMatrixParent.z_ax;
	firstDofShared.xAxis = m_shared->globalMatrixParent.x_ax;
	firstDofShared.yAxis = m_shared->globalMatrixParent.y_ax;
	firstDofShared.centre = m_shared->globalMatrixParent.w_pos;

	//--- calculate the rotation assuming the local X axis of joint frame as 0
	//--- and following the right-hand convention
	real sinAngle = (m_shared->globalMatrixParent.x_ax * m_shared->globalMatrixChild.x_ax) % m_shared->globalMatrixChild.z_ax;
	real cosAngle = m_shared->globalMatrixChild.x_ax % m_shared->globalMatrixParent.x_ax;
	real angle = atan2( sinAngle, cosAngle );
	real vel = (angle - firstDofShared.position) / world()->timeStep();
	firstDof->setPosition( angle );
	firstDof->setVelocity( vel );

	//--- the secondDOF results rotate of angle respect to its local X axis
	wMatrix tmp = wMatrix::identity();
	tmp.x_ax = m_shared->globalMatrixParent.z_ax;
	tmp.y_ax = m_shared->globalMatrixParent.y_ax;
	tmp.z_ax = m_shared->globalMatrixParent.x_ax;
	tmp = tmp * wMatrix( wQuaternion( tmp.x_ax, angle ), wVector(0,0,0) );
	tmp.sanitifize();
	secondDofShared.axis = tmp.z_ax;
	secondDofShared.xAxis = tmp.x_ax;
	secondDofShared.yAxis = tmp.y_ax;
	secondDofShared.centre = m_shared->globalMatrixParent.w_pos;

	//--- calculate the rotation assuming the local X axis of joint frame as 0
	//--- and following the right-hand convention
	//----------- Not sure check the second Axis rotation reference
	sinAngle = (m_shared->globalMatrixParent.z_ax * m_shared->globalMatrixChild.z_ax) % m_shared->globalMatrixChild.x_ax;
	cosAngle = m_shared->globalMatrixChild.z_ax % m_shared->globalMatrixParent.z_ax;
	angle = atan2( sinAngle, cosAngle );
	vel = (angle - secondDofShared.position) / world()->timeStep();
	secondDof->setPosition( angle );
	secondDof->setVelocity( vel );

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning HERE (PhyUniversal) I HAVE ADDED MISSING STUFFS TAKING THEM FROM updateJoint, CHECK THEY WORK OK
#endif
}

void PhyUniversal::createPrivateJoint()
{
#ifdef WORLDSIM_USE_NEWTON
	m_priv->joint = NewtonConstraintCreateUserJoint( m_worldPriv->world, 6, PhyJointPrivate::userBilateralHandler, 0, m_priv->child, m_priv->parent );
	NewtonJointSetUserData( m_priv->joint, this );
#endif
}

void PhyUniversal::updateJoint(real)
{
	// If the joint is disabled, we don't do anything here
	if (!m_shared->enabled) {
		return;
	}

	// Updating the global matrices and the dof
	updateJointInfo();

	PhyDOF* const firstDof = m_dofs[0];
	PhyDOF* const secondDof = m_dofs[1];

#ifdef WORLDSIM_USE_NEWTON
	//--- get the pin fixed to the first body
	const wVector& dir0 = m_shared->globalMatrixChild.z_ax;
	//--- get the pin fixed to the second body
	const wVector& dir1 = m_shared->globalMatrixParent.x_ax;

	//--- construct an orthogonal coordinate system with these two vectors
	wVector dir2 = ( dir0 * dir1 );
	dir2.normalize();
	wVector dir3( dir2 * dir0 );
	dir3.normalize();

	const wVector& p0 = m_shared->globalMatrixChild.w_pos;
	const wVector& p1 = m_shared->globalMatrixParent.w_pos;

	real len = 100.0;
	wVector q0( p0 + dir3.scale(len) );
	wVector q1( p1 + dir1.scale(len) );

	//--- Restrict the movement on the centre of the joint along all tree orthonormal direction
	NewtonUserJointAddLinearRow( m_priv->joint, &p0[0], &p1[0], &dir0[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &p0[0], &p1[0], &dir1[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &p0[0], &p1[0], &dir2[0] );
	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	//NewtonUserJointAddAngularRow( m_priv->joint, 0.0, &dir2[0] );
	//NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );
	NewtonUserJointAddLinearRow( m_priv->joint, &q0[0], &q1[0], &dir0[0] );
 	NewtonUserJointSetRowStiffness( m_priv->joint, 1.0 );

	//--- force will be used to set-up max and min torque newton will use to
	//--- resolve the constraint and accel to the double of appliedForce
	//--- this means that Newton will apply exactly forse amount of torque around
	//--- the axis
	real force = fabs( firstDof->appliedForce() );
	real accel = 2.0*firstDof->appliedForce();

	real angle = firstDof->position();
	if ( firstDof->isLimited() ) {
		real lolimit;
		real hilimit;
		firstDof->limits( lolimit, hilimit );
		if ( angle < lolimit ) {
			real relAngle = lolimit - angle; // - lolimit;
			NewtonUserJointAddAngularRow( m_priv->joint, relAngle, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowStiffness( m_priv->joint, firstDof->stiffness() );
			//--- this allow the joint to move back freely
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, 0.0 );
		} else if ( angle > hilimit ) {
			real relAngle = hilimit - angle; // - hilimit;
			NewtonUserJointAddAngularRow( m_priv->joint, relAngle, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowStiffness( m_priv->joint, firstDof->stiffness() );
			//--- this allow the joint to move forth freely
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, 0.0 );
		} else {
			NewtonUserJointAddAngularRow( m_priv->joint, 0.0, &m_shared->globalMatrixChild.z_ax[0] );
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, -force );
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, +force );
			NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
			NewtonUserJointSetRowStiffness( m_priv->joint, firstDof->stiffness() );
		}
	} else {
		NewtonUserJointAddAngularRow( m_priv->joint, 0.0, &m_shared->globalMatrixChild.z_ax[0] );
		NewtonUserJointSetRowMinimumFriction( m_priv->joint, -force );
		NewtonUserJointSetRowMaximumFriction( m_priv->joint, +force );
		NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
		NewtonUserJointSetRowStiffness( m_priv->joint, firstDof->stiffness() );
	}

	//--- force will be used to set-up max and min torque newton will use to
	//--- resolve the constraint and accel to the double of appliedForce
	//--- this means that Newton will apply exactly forse amount of torque around
	//--- the axis
	force = fabs( secondDof->appliedForce() );
	accel = 2.0*secondDof->appliedForce();

	angle = secondDof->position();
	if ( secondDof->isLimited() ) {
		real lolimit;
		real hilimit;
		secondDof->limits( lolimit, hilimit );
		if ( angle < lolimit ) {
			real relAngle = lolimit - angle; // - lolimit;
			NewtonUserJointAddAngularRow( m_priv->joint, relAngle, &m_shared->globalMatrixChild.x_ax[0] );
			NewtonUserJointSetRowStiffness( m_priv->joint, secondDof->stiffness() );
			//--- this allow the joint to move back freely
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, 0.0 );
		} else if ( angle > hilimit ) {
			real relAngle = hilimit - angle; // - hilimit;
			NewtonUserJointAddAngularRow( m_priv->joint, relAngle, &m_shared->globalMatrixChild.x_ax[0] );
			NewtonUserJointSetRowStiffness( m_priv->joint, secondDof->stiffness() );
			//--- this allow the joint to move forth freely
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, 0.0 );
		} else {
			NewtonUserJointAddAngularRow( m_priv->joint, 0.0, &m_shared->globalMatrixChild.x_ax[0] );
			NewtonUserJointSetRowMinimumFriction( m_priv->joint, -force );
			NewtonUserJointSetRowMaximumFriction( m_priv->joint, +force );
			NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
			NewtonUserJointSetRowStiffness( m_priv->joint, secondDof->stiffness() );
		}
	} else {
		NewtonUserJointAddAngularRow( m_priv->joint, 0.0, &m_shared->globalMatrixChild.x_ax[0] );
		NewtonUserJointSetRowMinimumFriction( m_priv->joint, -force );
		NewtonUserJointSetRowMaximumFriction( m_priv->joint, +force );
		NewtonUserJointSetRowAcceleration( m_priv->joint, accel );
		NewtonUserJointSetRowStiffness( m_priv->joint, secondDof->stiffness() );
	}

	//--- Retrive forces applied to the joint by the constraints
	Shared* const d = m_shared.getModifiableShared();
	d->forceOnJoint.x = NewtonUserJointGetRowForce (m_priv->joint, 0);
	d->forceOnJoint.y = NewtonUserJointGetRowForce (m_priv->joint, 1);
	d->forceOnJoint.z = NewtonUserJointGetRowForce (m_priv->joint, 2);
#endif
}

RenderPhyUniversal::RenderPhyUniversal(const PhyUniversal* entity)
	: RenderPhyJoint(entity)
{
}

RenderPhyUniversal::~RenderPhyUniversal()
{
	// Nothing to do here
}

void RenderPhyUniversal::render(const PhyUniversalShared* sharedData, GLContextAndData* contextAndData)
{
	// Drawing the joint
	const wVector end1 = (sharedData->localMatrixParent.inverse() * sharedData->globalMatrixParent).w_pos;
	const wVector end2 = (sharedData->localMatrixChild.inverse() * sharedData->globalMatrixChild).w_pos;
	drawJoint(sharedData, contextAndData, sharedData->globalMatrixParent.w_pos, end1, end2);
}

} // end namespace salsa
