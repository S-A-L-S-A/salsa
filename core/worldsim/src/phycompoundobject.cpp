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

#include "phycompoundobject.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "worldsimutils.h"
#include "worldsimexceptions.h"
#include "farsaglutils.h"
#include <QString>
#include <cstdlib>

namespace farsa {

PhyCompoundComponentsList::PhyCompoundComponentsList(World* world)
	: OwnershipChangesListener()
	, m_world(world)
	, m_shared(new SharedStuffs)
{
}

PhyCompoundComponentsList::~PhyCompoundComponentsList()
{
	// Deleting all objects still in the list of components. If we have been
	// used to build a PhyCompound object, the list is empty
	foreach (PhyCompoundComponentAndAbstractComponent c, m_shared->components) {
		m_world->deleteEntity(c.object);
		delete c.abstractComponent;
	}
}

void PhyCompoundComponentsList::ownerChanged(WEntity*, WEntity*)
{
	if (m_shared->creatingPhyCompound) {
		return;
	}

	abort();
}

void PhyCompoundComponentsList::entityDestroyed(WEntity*)
{
	if (m_shared->creatingPhyCompound) {
		return;
	}

	abort();
}

void PhyCompoundComponentsList::listenerChanged(WEntity*, OwnershipChangesListener*)
{
	if (m_shared->creatingPhyCompound) {
		return;
	}

	abort();
}

PhyCompoundObjectShared::PhyCompoundObjectShared()
	: PhyObjectShared()
	, m_components()
	, m_destroySharedDataHolders(true)
	, m_phyCompoundObject(NULL)
{
}

PhyCompoundObjectShared::PhyCompoundObjectShared(const PhyCompoundObjectShared& other)
	: PhyObjectShared(other)
	, m_components()
	, m_destroySharedDataHolders(true)
	, m_phyCompoundObject(other.m_phyCompoundObject)
{
	// Creating a copy of components and data
	foreach(AbstractComponentAndData c, other.m_components) {
		AbstractComponentAndData myC;
		myC.component = c.component;
		myC.sharedData = myC.component->generateSharedData();

		// Incrementing reference counter
		myC.component->referenceCounter.ref();

		m_components.append(myC);
	}

	// Copying data
	for (int i = 0; i < m_components.size(); ++i) {
		m_components[i].component->copyData(other.m_components[i].sharedData->data(), m_components[i].sharedData->data());
	}
}

PhyCompoundObjectShared& PhyCompoundObjectShared::operator=(const PhyCompoundObjectShared& other)
{
	if (this == &other) {
		return *this;
	}

	if (m_phyCompoundObject == NULL) {
		m_phyCompoundObject = other.m_phyCompoundObject;

		// We have to create shared data of components
		foreach(AbstractComponentAndData c, other.m_components) {
			AbstractComponentAndData myC;
			myC.component = c.component;
			myC.component->referenceCounter.ref(); // Incrementing reference counter
			myC.sharedData = myC.component->generateSharedData();

			m_components.append(myC);
		}
	}

	if (m_phyCompoundObject == other.m_phyCompoundObject) {
		// We can copy shared data
		for (int i = 0; i < m_components.size(); ++i) {
			m_components[i].component->copyData(other.m_components[i].sharedData->data(), m_components[i].sharedData->data());
		}
	} else {
		// We cannot copy if we are associated with different compound objects
		abort();
	}

	return *this;
}

PhyCompoundObjectShared::~PhyCompoundObjectShared()
{
	foreach(AbstractComponentAndData c, m_components) {
		if (!c.component->referenceCounter.deref()) {
			delete c.component;
		}

		if (m_destroySharedDataHolders) {
			delete c.sharedData;
		}
	}
}

void PhyCompoundObject::calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
{
	// Computing the AABBs of all components and merging them
	const QList<PhyCompoundObjectShared::AbstractComponentAndData>& components = sharedData->components();

#ifdef FARSA_DEBUG
	// Safety check, just to be sure
	if (Q_UNLIKELY(components.isEmpty())) {
		abort();
	}
#endif

	components[0].component->calculateAABB(components[0].sharedData->data(), minPoint, maxPoint, tm);
	wVector minP, maxP;
	for(int i = 1; i < components.size(); ++i) {
		components[i].component->calculateAABB(components[i].sharedData->data(), minPoint, maxPoint, tm);
		mergeAABBs(minPoint, maxPoint, minP, maxP);
	}
}

wVector PhyCompoundObject::calculateMBBSize(const Shared* sharedData)
{
	// This is only approximate, we compute the AABB when tm is the identity and use that to compute dimensions
	wVector minP, maxP;
	calculateAABB(sharedData, minP, maxP, wMatrix::identity());

	return maxP - minP;
}

PhyCompoundObject::PhyCompoundObject(World* world, SharedDataWrapper<Shared> shared, PhyCompoundComponentsList& objs, QString name, const wMatrix& tm)
	: PhyObject(world, shared, name, tm)
	, OwnershipChangesListener()
	, m_shared(shared)
	, m_components(objs.m_shared->components)
	, m_skipComponentOwnershipChecks(false)
{
	if (m_components.isEmpty()) {
		throw PhyCompoundComponentsListEmptyException();
	}

	// First of all clearing the list of the PhyCompoundComponentsList (we have already copied it) and setting
	// ourself as the owner and listener for changes of components owners. Moreover we set the transformation
	// matrix of the component to the identity, it will not be changed anymore because we destroy newton stuffs
	// when we create the compound object
	objs.m_shared->creatingPhyCompound = true;
	objs.m_shared->components.clear();
	foreach (PhyCompoundComponentAndAbstractComponent c, m_components) {
		// Setting owner before changing the OwnertshipChangesListener because otherwise an exception would be thrown
		c.object->setOwner(this, true);
		c.object->registerOwnertshipChangesListener(this);

		// Setting the transformation matrix
		c.object->m_shared.getModifiableShared()->tm = wMatrix::identity();
	}

	// Now filling the Shared structure. In this instance we do not delete the shared data holders because World will do it
	// when components are destroyed
	Shared* const d = m_shared.getModifiableShared();
	d->m_destroySharedDataHolders = false;
	d->m_phyCompoundObject = this;
	foreach (PhyCompoundComponentAndAbstractComponent c, m_components) {
		Shared::AbstractComponentAndData sd;
		sd.component = c.abstractComponent;
		sd.sharedData = c.sharedDataHolder;

		// Incrementing the reference counter
		sd.component->referenceCounter.ref();

		d->m_components.append(sd);
	}
	objs.m_shared->creatingPhyCompound = false;
}

PhyCompoundObject::~PhyCompoundObject()
{
	// We do not have to delete anything here: components are destroyed because we are the owner, the
	// AbstractPhyObjectComponent is destroyed by PhyCompoundObjectShared destructor and the shared data holder
	// and the renderer creator are destroyed by world when the component is destroyed

	// This is to avoid throwing exceptions when components are destroyed
	m_skipComponentOwnershipChecks = true;
	foreach (PhyCompoundComponentAndAbstractComponent c, m_components) {
		// Setting a NULL listener: we cannot be the listener anymore because we will be already destroyed when
		// components are destroyed
		c.object->registerOwnertshipChangesListener(NULL);
	}
}

void PhyCompoundObject::createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset)
{
#ifdef WORLDSIM_USE_NEWTON
	// Newton does not support a collision shape offset for compound collisions. If we got anything different from NULL
	// in collisionShapeOffset, printing a warning
	if (collisionShapeOffset != NULL) {
		qWarning("Cannot set a collision shape offset for PhyCompoundObjects in Newton");
	}

	const int dim = m_components.size();
	NewtonCollision** temp = new NewtonCollision*[dim];
	real mass = 0.0;
	for(int i = 0; i < dim; i++) {
		temp[i] = m_components[i].object->m_priv->collision;
		mass += m_components[i].object->mass();
	}
	NewtonCollision* c = NewtonCreateCompoundCollision(m_worldPriv->world, dim, temp, 1);
	m_priv->collision = c;
	for(int i = 0; i < dim; i++) {
		NewtonReleaseCollision(m_worldPriv->world, m_components[i].object->m_priv->collision);
		m_components[i].object->m_priv->collision = NULL;

		// We also delete the private object of components, they are not used anymore
		delete m_components[i].object->m_priv;
		m_components[i].object->m_priv = NULL;
	}
	delete []temp;
	if (!onlyCollisionShape) {
		wMatrix initialTransformationMatrix = wMatrix::identity(); // The transformation matrix is set in other places
		m_priv->body = NewtonCreateBody(m_worldPriv->world, c, &initialTransformationMatrix[0][0]);
		NewtonBodySetAutoSleep(m_priv->body, 0);
		setMass(mass);
		NewtonBodySetUserData(m_priv->body, this);
		NewtonBodySetLinearDamping(m_priv->body, 0.0);
		wVector zero = wVector(0,0,0,0);
		NewtonBodySetAngularDamping(m_priv->body, &zero[0]);
		NewtonBodySetAutoSleep(m_priv->body, 0);
		NewtonBodySetFreezeState(m_priv->body, 0);
		// Sets the signal-wrappers callback
		NewtonBodySetTransformCallback(m_priv->body, (PhyObjectPrivate::setTransformHandler));
		NewtonBodySetForceAndTorqueCallback(m_priv->body, (PhyObjectPrivate::applyForceAndTorqueHandler));
	}
#endif
	changedMatrix();
}

void PhyCompoundObject::ownerChanged(WEntity*, WEntity*)
{
	if (m_skipComponentOwnershipChecks) {
		return;
	}

	// This is only called when the owner of a component changes (not allowed)
	abort();
}

void PhyCompoundObject::entityDestroyed(WEntity*)
{
	if (m_skipComponentOwnershipChecks) {
		return;
	}

	// This is only called when a component is destroyed (not allowed)
	abort();
}

void PhyCompoundObject::listenerChanged(WEntity*, OwnershipChangesListener*)
{
	if (m_skipComponentOwnershipChecks) {
		return;
	}

	// This is only called when the ownership changes listener of a component changes (not allowed)
	abort();
}

RenderPhyCompoundObject::RenderPhyCompoundObject(const PhyCompoundObject* entity)
	: RenderPhyObject(entity)
	, m_componentsRenderers()
{
	// Creating the renderers and copying the abstract component
	foreach (PhyCompoundComponentAndAbstractComponent c, entity->m_components) {
		ComponentAndRenderer r;
		r.component = c.abstractComponent;
		r.renderer = c.rendererCreator->generateRenderer();

		// Incrementing reference counter
		r.component->referenceCounter.ref();

		m_componentsRenderers.append(r);
	}
}

RenderPhyCompoundObject::~RenderPhyCompoundObject()
{
	foreach (ComponentAndRenderer r, m_componentsRenderers) {
		if (!r.component->referenceCounter.deref()) {
			delete r.component;
		}

		// Deleting renreder
		delete r.renderer;
	}
}

void RenderPhyCompoundObject::render(const PhyCompoundObjectShared* sharedData, GLContextAndData* contextAndData)
{
#ifdef FARSA_DEBUG
	// Safety check, just to be sure
	if (Q_UNLIKELY(m_componentsRenderers.size() != sharedData->components().size())) {
		abort();
	}
#endif

	glPushMatrix();
	setupColorTexture(sharedData, contextAndData);
	wMatrix mtr = collisionShapeMatrix(sharedData);
	GLMultMatrix(&(mtr[0][0]));

	// Rendering all components
	for (int i = 0; i < m_componentsRenderers.size(); ++i) {
		ComponentAndRenderer r = m_componentsRenderers[i];
		PhyCompoundObjectShared::AbstractComponentAndData d = sharedData->components()[i];

#ifdef FARSA_DEBUG
		// Safety check, just to be sure
		if (Q_UNLIKELY(r.component != d.component)) {
			abort();
		}
#endif

		r.component->render(r.renderer, d.sharedData->data(), contextAndData);
	}

	glPopMatrix();
}

} // end namespace farsa
