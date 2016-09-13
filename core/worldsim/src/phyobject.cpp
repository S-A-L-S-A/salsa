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

#include "phyobject.h"
#include "world.h"
#include "salsaglutils.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"

namespace salsa {

PhyObject::PhyObject(World* world, SharedDataWrapper<Shared> shared, QString name, const wMatrix& tm)
	: WObject(world, shared, name, tm)
	, m_shared(shared)
	, m_priv(new PhyObjectPrivate())
	, m_worldPriv(world->m_priv.get())
{
}

PhyObject::~PhyObject()
{
#ifdef WORLDSIM_USE_NEWTON
	if (m_priv != nullptr) {
		if (m_priv->body != nullptr) {
			NewtonDestroyBody(m_worldPriv->world, m_priv->body);
		}
		if (m_priv->collision != nullptr) {
			NewtonReleaseCollision(m_worldPriv->world, m_priv->collision);
		}
	}
#endif

	delete m_priv;
}

void PhyObject::setKinematic(bool b, bool c)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	Shared* const d = m_shared.getModifiableShared();

	//if (m_shared->isKinematic == b) return;
	d->isKinematic = b;
#ifdef WORLDSIM_USE_NEWTON
	if (b) {
		// Unsets the signal-wrappers callback and collision shape
		NewtonBodySetTransformCallback(m_priv->body, 0);
		NewtonBodySetForceAndTorqueCallback(m_priv->body, 0);
		//NewtonBodySetFreezeState( priv->body, 1 ); <== this will block also any body jointed to this one
// 		//--- Queste tre righe mettono un NullCollision all'oggetto fisico,
// 		//    e quindi tutte le collisioni vengono totalmente ignorarte da NGD
// 		//--- Un alternativa e' commentare queste righe, ed impostare la massa a 0
// 		//    in questo modo, l'oggetto risultera' statico per NGD, e generera' collisioni
// 		//    con gli altri oggetti. Cosi' facendo anche muovendo l'oggetto in modo
// 		//    cinematico potra' cmq collidere e spostare oggetti dinamici.
// 		//    Pero', cmq, gli passera' attraverso come se fossero trasparenti, quindi
// 		//    movimenti veloci risulteranno 'sbagliati' dal punto di vista fisico
// 		NewtonCollision* nullC = NewtonCreateNull( worldpriv->world );
// 		NewtonBodySetCollision( priv->body, nullC );
// 		NewtonReleaseCollision( worldpriv->world, nullC );
// 		//--- soluzione alternativa: NewtonBodySetMassMatrix( priv->body, 0, 1, 1, 1 );
		d->isKinematicCollidable = c;
		if (m_shared->isKinematicCollidable) {
			// Here we set mass to 0. This way the object will be static for NGD and will
			// collide with other solids, influencing them without being influenced.
			NewtonBodySetMassMatrix(m_priv->body, 0, 1, 1, 1);
		} else {
			// The following lines set a NullCollision for the physical object, so that
			// all collisions are completely ignored by Newton Game Dynamics
			NewtonCollision* nullC = NewtonCreateNull(m_worldPriv->world);
			NewtonBodySetCollision(m_priv->body, nullC);
			NewtonReleaseCollision(m_worldPriv->world, nullC);
		}
	} else {
		// Sets the signal-wrappers callback and collision shape
		NewtonBodySetTransformCallback(m_priv->body, (PhyObjectPrivate::setTransformHandler));
		NewtonBodySetForceAndTorqueCallback(m_priv->body, (PhyObjectPrivate::applyForceAndTorqueHandler));
		NewtonBodySetFreezeState(m_priv->body, 0);
		NewtonBodySetCollision(m_priv->body, m_priv->collision);
		NewtonBodySetMatrix(m_priv->body, &(m_shared->tm[0][0]));
		// Restoring mass and inertia
		if (m_shared->isStatic) {
			NewtonBodySetMassMatrix(m_priv->body, 0.0, 1.0, 1.0, 1.0);
		} else {
			NewtonBodySetMassMatrix(m_priv->body, m_shared->objInertiaVec[0], m_shared->objInertiaVec[1], m_shared->objInertiaVec[2], m_shared->objInertiaVec[3]);
		}
	}
#endif
}

void PhyObject::setStatic(bool b)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	if (m_shared->isStatic == b) return;

	m_shared.getModifiableShared()->isStatic = b;
#ifdef WORLDSIM_USE_NEWTON
	if (b) {
		NewtonBodySetMassMatrix(m_priv->body, 0.0, 1.0, 1.0, 1.0 );
	} else {
		NewtonBodySetMassMatrix(m_priv->body, m_shared->objInertiaVec[0], m_shared->objInertiaVec[1], m_shared->objInertiaVec[2], m_shared->objInertiaVec[3]);
	}
#endif
}

void PhyObject::reset()
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}

	Shared* const d = m_shared.getModifiableShared();

	const wVector zero = wVector(0,0,0);
	d->forceAcc = zero;
	d->torqueAcc = zero;
	NewtonBodySetForce(m_priv->body, &(m_shared->forceAcc[0]));
	NewtonBodySetTorque(m_priv->body, &(m_shared->torqueAcc[0]));
	NewtonBodySetOmega(m_priv->body, &zero[0]);
	NewtonBodySetVelocity(m_priv->body, &zero[0]);
	//--- remove any contact involving this object
	NewtonWorld* nworld = NewtonBodyGetWorld(m_priv->body);
	NewtonWorldCriticalSectionLock(nworld);
	for (NewtonJoint* contactList = NewtonBodyGetFirstContactJoint(m_priv->body); contactList;
		 contactList = NewtonBodyGetNextContactJoint(m_priv->body, contactList)) {
		void* nextContact;
		for (void* contact = NewtonContactJointGetFirstContact(contactList); contact; contact = nextContact) {
			//--- before removing contact, It get the nextContact in the list
			nextContact = NewtonContactJointGetNextContact(contactList, contact);
			NewtonContactJointRemoveContact(contactList, contact);
		}
	}
	NewtonWorldCriticalSectionUnlock(nworld);
#endif
}

void PhyObject::addForce(const wVector& force)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	m_shared.getModifiableShared()->forceAcc += force;
}

void PhyObject::setForce(const wVector& force)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	m_shared.getModifiableShared()->forceAcc = force;
}

wVector PhyObject::force()
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return wVector(0.0, 0.0, 0.0);
	}
#endif

	wVector f;
#ifdef WORLDSIM_USE_NEWTON
	NewtonBodyGetForce(m_priv->body, &f[0]);
#endif
	return f;
}

void PhyObject::addTorque(const wVector& torque)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	m_shared.getModifiableShared()->torqueAcc += torque;
}

void PhyObject::setTorque(const wVector& torque)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	m_shared.getModifiableShared()->torqueAcc = torque;
}

wVector PhyObject::torque()
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return wVector(0.0, 0.0, 0.0);
	}
#endif

	wVector t;
#ifdef WORLDSIM_USE_NEWTON
	NewtonBodyGetTorque(m_priv->body, &t[0]);
#endif
	return t;
}

void PhyObject::setMassInertiaVec(const wVector& mi)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	Shared* const d = m_shared.getModifiableShared();
	d->objInertiaVec = mi;
#ifdef WORLDSIM_USE_NEWTON
	// Setting mass and getting the inverse inertia matrix
	NewtonBodySetMassMatrix(m_priv->body, mi[0], mi[1], mi[2], mi[3] );
	NewtonBodyGetInvMass(m_priv->body, &(d->objInvInertiaVec[0]), &(d->objInvInertiaVec[1]), &(d->objInvInertiaVec[2]), &(d->objInvInertiaVec[3]));
#endif
}

const wVector& PhyObject::massInertiaVec() const
{
	return m_shared->objInertiaVec;
}

wVector PhyObject::inertiaVec() const
{
	wVector mi;
	mi[0] = m_shared->objInertiaVec[1];
	mi[1] = m_shared->objInertiaVec[2];
	mi[2] = m_shared->objInertiaVec[3];
	mi[3] = 0.0;
	return mi;
}

const wVector& PhyObject::invMassInertiaVec() const
{
	return m_shared->objInvInertiaVec;
}

wVector PhyObject::invInertiaVec() const
{
	wVector mi;
	mi[0] = m_shared->objInvInertiaVec[1];
	mi[1] = m_shared->objInvInertiaVec[2];
	mi[2] = m_shared->objInvInertiaVec[3];
	mi[3] = 0.0;
	return mi;
}

void PhyObject::setMass(real newmass)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}

	Shared* const d = m_shared.getModifiableShared();

	if ( newmass < 0.0001 ) {
		d->objInertiaVec = wVector(0.0, 1.0, 1.0, 1.0);
		d->objInvInertiaVec = wVector(0.0, 1.0, 1.0, 1.0);
		d->isStatic = true;
		NewtonBodySetMassMatrix( m_priv->body, 0, 1, 1, 1 );
	} else {
		real inertia[3];
		real centre[3] = { 0, 0, 0 };
		NewtonConvexCollisionCalculateInertialMatrix( m_priv->collision, inertia, centre );

		d->objInertiaVec = wVector(newmass, newmass*inertia[0], newmass*inertia[1], newmass*inertia[2]);
		NewtonBodySetMassMatrix( m_priv->body, m_shared->objInertiaVec[0], m_shared->objInertiaVec[1], m_shared->objInertiaVec[2], m_shared->objInertiaVec[3] );
		// Saving the inverse inertia matrix
		NewtonBodyGetInvMass( m_priv->body, &(d->objInvInertiaVec[0]), &(d->objInvInertiaVec[1]), &(d->objInvInertiaVec[2]), &(d->objInvInertiaVec[3]));
		// If the object is static we reset its mass to 0
		if (m_shared->isStatic) {
			NewtonBodySetMassMatrix( m_priv->body, 0, 1, 1, 1 );
		}
	}
#endif
}

real PhyObject::mass() const
{
	return m_shared->objInertiaVec[0];
}

void PhyObject::setOmega(const wVector& omega)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}

	NewtonBodySetOmega( m_priv->body, &omega[0] );
#endif
}

wVector PhyObject::omega()
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return wVector(0.0, 0.0, 0.0);
	}
#endif

	wVector t;
#ifdef WORLDSIM_USE_NEWTON
	NewtonBodyGetOmega( m_priv->body, &t[0] );
#endif
	return t;
}

void PhyObject::setVelocity(const wVector& velocity)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}

	NewtonBodySetVelocity( m_priv->body, &velocity[0] );
#endif
}

wVector PhyObject::velocity()
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return wVector(0.0, 0.0, 0.0);
	}
#endif

	wVector t;
#ifdef WORLDSIM_USE_NEWTON
	NewtonBodyGetVelocity( m_priv->body, &t[0] );
#endif
	return t;
}

void PhyObject::addImpulse(const wVector& pointDeltaVeloc, const wVector& pointPosit)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}

	NewtonBodyAddImpulse( m_priv->body, &pointDeltaVeloc[0], &pointPosit[0] );
#endif
}

void PhyObject::setMaterial(QString material)
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}
#endif

	m_shared.getModifiableShared()->material = material;
#ifdef WORLDSIM_USE_NEWTON
	if ( m_worldPriv->matIDs.contains( material ) ) {
		NewtonBodySetMaterialGroupID( m_priv->body, m_worldPriv->matIDs[material] );
	}
#endif
}

QString PhyObject::material() const
{
	return m_shared->material;
}

bool PhyObject::forcesDrawn() const
{
	return m_shared->flags & PhyObjectShared::DrawForces;
}

void PhyObject::setForcesDrawn(bool d)
{
	if (d) {
		m_shared.getModifiableShared()->flags |= PhyObjectShared::DrawForces;
	} else {
		m_shared.getModifiableShared()->flags &= ~PhyObjectShared::DrawForces;
	}
}

bool PhyObject::aboutToChangeMatrix()
{
	return (m_priv != nullptr);
}

void PhyObject::changedMatrix()
{
#ifdef WORLDSIM_USE_NEWTON
	if ((m_priv == nullptr) || (m_priv->body == nullptr)) {
		return;
	}

	//qDebug() << "SYNC POSITION" << tm[3][0] << tm[3][1] << tm[3][2];
	NewtonBodySetMatrix( m_priv->body, &(m_shared->tm[0][0]) );
#endif
}

void PhyObject::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonCollision* c = NewtonCreateNull( m_worldPriv->world );
	m_priv->collision = c;
	if (collisionShapeOffset != nullptr) {
		m_shared.getModifiableShared()->collisionShapeOffset = *collisionShapeOffset;
	}
	if (!onlyCollisionShape) {
		wMatrix initialTransformationMatrix = wMatrix::identity(); // The transformation matrix is set in other places
		m_priv->body = NewtonCreateBody( m_worldPriv->world, c, &initialTransformationMatrix[0][0] );
		//NewtonReleaseCollision( m_worldPriv->world, c );
		NewtonBodySetAutoSleep( m_priv->body, 0 );
		NewtonBodySetMassMatrix( m_priv->body, 1.0, 1.0, 1.0, 1.0 );
		NewtonBodySetUserData( m_priv->body, this );
		// Sets the signal-wrappers callback
		NewtonBodySetTransformCallback( m_priv->body, (PhyObjectPrivate::setTransformHandler) );
		NewtonBodySetForceAndTorqueCallback( m_priv->body, (PhyObjectPrivate::applyForceAndTorqueHandler) );
	}
#endif
	changedMatrix();
}

void PhyObject::postCreatePrivateObject()
{
}

RenderPhyObject::RenderPhyObject(const PhyObject* entity)
	: RenderWObject(entity)
{
}

RenderPhyObject::~RenderPhyObject()
{
	// Nothing to do here
}

void RenderPhyObject::drawForces(const PhyObjectShared* sharedData, GLContextAndData*)
{
	glPushMatrix();
	GLMultMatrix(&(sharedData->tm[0][0]));

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	GLUtils::drawArrow(sharedData->tm.w_pos, sharedData->tm.w_pos + sharedData->forceAcc, 0.1f, 12, QColor(Qt::red));
	GLUtils::drawArrow(sharedData->tm.w_pos, sharedData->tm.w_pos + sharedData->torqueAcc, 0.1f, 12, QColor(Qt::green));
	glPopAttrib();

	glPopMatrix();
}

wMatrix RenderPhyObject::collisionShapeMatrix(const PhyObjectShared* sharedData) const
{
	return sharedData->collisionShapeOffset * sharedData->tm;
}

} // end namespace salsa
