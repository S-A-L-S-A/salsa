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

#include "world.h"
#include "phyobject.h"
#include "phyjoint.h"
#include "private/phyobjectprivate.h"
#include "private/worldprivate.h"
#include "motorcontrollers.h"
#include "logger.h"
#include <QPair>

namespace salsa {

World::World(QString worldname)
	: m_name(worldname)
	, m_time(0.0f)
	, m_timestep(0.0150f)
	, m_g(-9.8f)
	, m_minP(-100.0f, -100.0f, -100.0f)
	, m_maxP(100.0f, 100.0f, 100.0f)
	, m_entities()
	, m_entitiesMap()
	, m_shadowEntities()
	, m_shadowEntitiesMap()
	, m_renderersContainers()
	, m_mapObjJoints()
	, m_creatingSomething(false)
	, m_cmap()
	, m_nobjs()
	, m_mats()
	, m_isInitialized(false)
	, m_priv()
	, m_textures()
{
	createWorld();
}

World::~World()
{
	destroyWorld();
}

QString World::name() const
{
	return m_name;
}

void World::reset()
{
	destroyWorld();
	createWorld();
}

real World::elapsedTime() const
{
	return m_time;
}

void World::resetElapsedTime()
{
	m_time = 0.0;
}

void World::setTimeStep(real timestep)
{
	m_timestep = timestep;
}

void World::setMinimumFrameRate(unsigned int frames)
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonSetMinimumFrameRate(m_priv->world, frames);
#endif
}

real World::timeStep() const
{
	return m_timestep;
}

void World::setGravitationalAcceleration(real g)
{
	m_g = g;
}

real World::gravitationalAcceleration() const
{
	return m_g;
}

QList<WEntity*> World::entities() const
{
	return m_entitiesMap.keys();
}

WEntity* World::getEntity(const QString& name)
{
	foreach (const WEntityAndBuddies& e, m_entities) {
		if (e.entity->name() == name) {
			return e.entity;
		}
	}

	return nullptr;
}

const QHash<PhyObject*, QList<PhyJoint*> >& World::mapObjectsToJoints() const
{
	return m_mapObjJoints;
}

void World::disableCollisions(PhyObject* obj1, PhyObject* obj2)
{
	m_nobjs.insert(qMakePair(obj1, obj2));
	m_nobjs.insert(qMakePair(obj2, obj1));
}

void World::enableCollisions(PhyObject* obj1, PhyObject* obj2)
{
	m_nobjs.remove(qMakePair(obj1, obj2));
	m_nobjs.remove(qMakePair(obj2, obj1));
}

const ContactMap& World::contacts() const
{
	return m_cmap;
}

bool World::closestPoints(PhyObject* objA, PhyObject* objB, wVector& pointA, wVector& pointB)
{
#ifdef WORLDSIM_USE_NEWTON
	wVector normal;
	wMatrix t1 = objA->matrix();
	wMatrix t2 = objB->matrix();
	int ret = NewtonCollisionClosestPoint(m_priv->world,
	                                      objA->m_priv->collision, &t1[0][0],
	                                      objB->m_priv->collision, &t2[0][0],
	                                      &pointA[0], &pointB[0], &normal[0], 0);
	if (ret == 0) {
		pointA = wVector(0,0,0);
		pointB = wVector(0,0,0);
		return false;
	}
	return true;
#endif
}

bool World::checkContacts(PhyObject* obj1, PhyObject* obj2, int maxContacts, QVector<wVector>* contacts, QVector<wVector>* normals, QVector<real>* penetra)
{
#ifdef WORLDSIM_USE_NEWTON
	// Allocating the vector we use to get data from NewtonCollisionCollide
	real *const tmp_data = new real[2 * 3 * maxContacts + maxContacts];
	real *const tmp_contacts = &tmp_data[0 * 3 * maxContacts];
	real *const tmp_normals = &tmp_data[1 * 3 * maxContacts];
	real *const tmp_penetra = &tmp_data[2 * 3 * maxContacts];

	// Computing contacts
	wMatrix t1 = obj1->matrix();
	wMatrix t2 = obj2->matrix();
	const int numContacts = NewtonCollisionCollide(m_priv->world, maxContacts, obj1->m_priv->collision, &t1[0][0], obj2->m_priv->collision, &t2[0][0], tmp_contacts, tmp_normals, tmp_penetra, 0);

	// Now copying contacts information into user vectors
	if (contacts != nullptr) {
		contacts->resize(numContacts);
		for (int i = 0; i < numContacts; i++) {
			(*contacts)[i].x = tmp_contacts[0 + i * 3];
			(*contacts)[i].y = tmp_contacts[1 + i * 3];
			(*contacts)[i].z = tmp_contacts[2 + i * 3];
		}
	}
	if (normals != nullptr) {
		normals->resize(numContacts);
		for (int i = 0; i < numContacts; i++) {
			(*normals)[i].x = tmp_normals[0 + i * 3];
			(*normals)[i].y = tmp_normals[1 + i * 3];
			(*normals)[i].z = tmp_normals[2 + i * 3];
		}
	}
	if (penetra != nullptr) {
		penetra->resize(numContacts);
		for (int i = 0; i < numContacts; i++) {
			(*penetra)[i] = tmp_penetra[i];
		}
	}

	delete[] tmp_data;
	return (numContacts != 0);
#endif
}

bool World::smartCheckContacts(PhyObject* obj1, PhyObject* obj2, int maxContacts, QVector<wVector>* contacts)
{
	if (obj1->getKinematic() || obj2->getKinematic() || (obj1->getStatic() && obj2->getStatic())) {
		return checkContacts(obj1, obj2, maxContacts, contacts, nullptr, nullptr);
	} else {
		if (!m_cmap.contains(obj1)) {
			return false;
		}

		// Taking the vector of contacts
		const ContactVec& c = m_cmap[obj1];
		bool collision = false;
		if (contacts != nullptr) {
			contacts->clear();
		}

		// Now extracting all collisions between obj1 and obj2
		for (int i = 0; i < c.size(); i++) {
			if (c[i].collide == obj2) {
				collision = true;
				if ((contacts != nullptr) && (contacts->size() < maxContacts)) {
					// Adding contact point
					contacts->append(c[i].worldPos);
				}
			}
		}

		return collision;
	}
}

real World::collisionRayCast(PhyObject* obj, wVector start, wVector end, wVector* normal)
{
#ifdef WORLDSIM_USE_NEWTON
	// The vector storing the normal to the contact point and the attribute (unused)
	dFloat n[3];
	int attribute;
	wVector localStart = obj->matrix().inverse().transformVector(start);
	wVector localEnd = obj->matrix().inverse().transformVector(end);

	// Computing the contact
	const real contact = NewtonCollisionRayCast(obj->m_priv->collision, &localStart[0], &localEnd[0], n, &attribute);

	if (normal != nullptr) {
		(*normal)[0] = n[0];
		(*normal)[1] = n[1];
		(*normal)[2] = n[2];
	}

	return contact;
#else
	return 2.0;
#endif
}

RayCastHitVector World::worldRayCast(wVector start, wVector end, bool onlyClosest, const QSet<PhyObject*>& ignoredObjs)
{
#ifdef WORLDSIM_USE_NEWTON
	WorldPrivate::WorldRayCastCallbackUserData data(start, end, onlyClosest, ignoredObjs);

	// Casting the ray
	NewtonWorldRayCast(m_priv->world, &start[0], &end[0], WorldPrivate::worldRayFilterCallback, &data, nullptr);

	return data.vector;
#else
	return rayCastHitVector();
#endif
}


MaterialDB& World::materials()
{
	return *m_mats;
}

const MaterialDB& World::materials() const
{
	return *m_mats;
}

void World::setSolverModel(QString model)
{
#ifdef WORLDSIM_USE_NEWTON
	if ("exact" == model) {
		NewtonSetSolverModel(m_priv->world, 0);
	} else if ("linear" == model) {
		NewtonSetSolverModel(m_priv->world, 1);
	}
#endif
}

void World::setFrictionModel(QString model)
{
#ifdef WORLDSIM_USE_NEWTON
	if ("exact" == model) {
		NewtonSetFrictionModel(m_priv->world, 0);
	} else if ("linear" == model) {
		NewtonSetFrictionModel(m_priv->world, 1);
	}
#endif
}

void World::setMultiThread(int numThreads)
{
#ifdef WORLDSIM_USE_NEWTON
	numThreads = max(1, numThreads);
	NewtonSetThreadsCount(m_priv->world, numThreads);
#endif
}

void World::setSize(const wVector& minPoint, const wVector& maxPoint)
{
	m_minP = minPoint;
	m_maxP = maxPoint;
#ifdef WORLDSIM_USE_NEWTON
	NewtonSetWorldSize(m_priv->world, &m_minP[0], &m_maxP[0]);
#endif

	// Sending the new dimension to all renderers containers
	WorldGraphicalInfo info;
	info.minP = m_minP;
	info.maxP = m_maxP;
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		c->setWorldGraphicalInfo(info);
	}
}

void World::size(wVector &minPoint, wVector &maxPoint)
{
	minPoint = m_minP;
	maxPoint = m_maxP;
}

void World::cleanUpMemory()
{
#ifdef WORLDSIM_USE_NEWTON
	NewtonInvalidateCache(m_priv->world);
	m_cmap.clear();
#endif
}

void World::initialize()
{
	if (m_isInitialized) {
		return;
	}

	m_time = 0.0;
	m_isInitialized = true;
}

void World::advance()
{
	if (!m_isInitialized) {
		initialize();
	}

	// Call preUpdate() on all entities
	for (QLinkedList<WEntityAndBuddies>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
		it->entity->preUpdate();
	}

	// Simulate physic. Before doing the actual simulation step, clearing the map of contacts
	// (we do it here so that calls to preUpdate() can access the old map of contacts)
	m_cmap.clear();
#ifdef WORLDSIM_USE_NEWTON
	NewtonUpdate(m_priv->world, m_timestep);
#endif

	// Call postUpdate() on all entities
	for (QLinkedList<WEntityAndBuddies>::iterator it = m_entities.begin(); it != m_entities.end(); ++it) {
		it->entity->postUpdate();
	}

	m_time += m_timestep;

	// Updating all renderers containers
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		c->update();
	}
}

void World::deleteEntity(WEntity* entity)
{
	if (entity == nullptr) {
		return;
	}

	bool isShadowEntity;

	if (m_entitiesMap.contains(entity)) {
		isShadowEntity = false;
	} else if (m_shadowEntitiesMap.contains(entity)) {
		isShadowEntity = true;
	} else {
#ifdef SALSA_DEBUG
		qDebug( "Trying to delete an entity not in the map" );
#endif
		return;
	}

	// First of all getting all entity buddies
	QLinkedList<WEntityAndBuddies>::iterator eit = isShadowEntity ? m_shadowEntitiesMap[entity] : m_entitiesMap[entity];

	// Now performing all actions that are type-dependent
	PhyObject* const phyObject = dynamic_cast<PhyObject*>(entity);
	if (phyObject != nullptr) {
		// Destroying joints. We have to do this because Newton destroys them anyway
		if (m_mapObjJoints.contains(phyObject)) {
			// Getting the list of joints for the object
			QList<PhyJoint*> jointsForObj = m_mapObjJoints[phyObject];

			// Removing the object from the map from objects to joints
			m_mapObjJoints.remove(phyObject);

			// Calling deleteEntity() on all joints
			foreach (PhyJoint* j, jointsForObj) {
				deleteEntity(j);
			}
		}

		// Removing contacts for the object
		if (m_cmap.contains(phyObject)) {
			const ContactVec &cvec = m_cmap[phyObject];
			foreach (Contact c, cvec) {
				ContactVec &otherCVec = m_cmap[c.collide];
				ContactVec::iterator it = otherCVec.begin();
				while (it != otherCVec.end()) {
					if (it->collide == phyObject) {
						it = otherCVec.erase(it);
					} else {
						++it;
					}
				}
				if (otherCVec.isEmpty()) {
					// Removing collisions if the list is empty
					m_cmap.remove(c.collide);
				}
			}
			m_cmap.remove(phyObject);
		}

		// Also removing the object from the m_nobjs set
		QSet<NObj>::iterator it = m_nobjs.begin();
		while (it != m_nobjs.end()) {
			if ((it->first == phyObject) || (it->second == phyObject)) {
				it = m_nobjs.erase(it);
			} else {
				++it;
			}
		}
	} else {
		PhyJoint* const phyJoint = dynamic_cast<PhyJoint*>(entity);
		if (phyJoint != nullptr) {
			if (m_mapObjJoints.contains(phyJoint->child())) {
				m_mapObjJoints[phyJoint->child()].removeAll(phyJoint);
				if (m_mapObjJoints[phyJoint->child()].isEmpty()) {
					m_mapObjJoints.remove(phyJoint->child());
				}
			}
			if ((phyJoint->parent() != nullptr) && (m_mapObjJoints.contains(phyJoint->parent()))) {
				m_mapObjJoints[phyJoint->parent()].removeAll(phyJoint);
				if (m_mapObjJoints[phyJoint->parent()].isEmpty()) {
					m_mapObjJoints.remove(phyJoint->parent());
				}
			}
		}
	}

	// Notifying the renderers containers that an entity is being removed
	if (!isShadowEntity) {
		foreach (AbstractRendererContainer* c, m_renderersContainers) {
			c->deletedRenderer(eit->rendererCreator);
		}
	}

	// Removing the entity from the map
	if (isShadowEntity) {
		m_shadowEntitiesMap.remove(entity);
	} else {
		m_entitiesMap.remove(entity);
	}

	WEntityAndBuddies entityToDelete = *eit;

	// Removing the entity from the list
	if (isShadowEntity) {
		m_shadowEntities.erase(eit);
	} else {
		m_entities.erase(eit);
	}

	// Deleting objects related to the entity to destroy
	delete entityToDelete.rendererCreator;
	delete entityToDelete.sharedDataHolder;

	// Finally deleting the entity
	delete entity;
}

void World::deleteRenderersContainer(AbstractRendererContainer* c)
{
	if (c == nullptr) {
		return;
	}

	// Telling the container to remove all textures and entities
	c->deleteAllRenderers();
	c->deleteAllTextures();

	// Deleting the container
	delete c;
}

void World::addTexture(const QString& name, const QImage& image)
{
	// Adding texture to the map
	m_textures[name] = image;

	// Notifying all renderers containers of the new texture
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		c->addTexture(name, image);
	}
}

void World::deleteTexture(const QString& name)
{
	if (m_textures.contains(name)) {
		// Notifying all renderers containers that the texture has been removed
		foreach (AbstractRendererContainer* c, m_renderersContainers) {
			c->deleteTexture(name);
		}

		// Removing the texture from the map
		m_textures.remove(name);
	}
}

void World::deleteAllTextures()
{
	// Telling all renderers containers that all texture are removed
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		c->deleteAllTextures();
	}

	// Clearing the map of textures
	m_textures.clear();
}

void World::createWorld()
{
	m_time = 0.0f;
	m_creatingSomething = false;
	m_isInitialized = false;

	m_mats.reset(new MaterialDB(this));
	m_priv.reset(new WorldPrivate());

#ifdef WORLDSIM_USE_NEWTON
	m_priv->world = NewtonCreate();
	NewtonInvalidateCache(m_priv->world);
	NewtonSetWorldSize(m_priv->world, &m_minP[0], &m_maxP[0]);
	 // keep at least to 100 for stability
	NewtonSetMinimumFrameRate(m_priv->world, 100);
	// exact model as default
	NewtonSetSolverModel(m_priv->world, 0);
	NewtonSetFrictionModel(m_priv->world, 0);
#endif

	// Registering all pre-defined textures
	m_textures["tile1"].load(":/tiles/16tile10.jpg");
	m_textures["tile2"].load(":/tiles/16tile07.jpg");
	m_textures["white"].load(":/white.jpg");
	m_textures["tile3"].load(":/tiles/16tile11.jpg");
	m_textures["tile4"].load(":/tiles/16tile-B.jpg");
	m_textures["tile5"].load(":/tiles/16tile12.jpg");
	m_textures["tile6"].load(":/tiles/16tile04.jpg");
	m_textures["tile7"].load(":/tiles/tile01.jpg");
	m_textures["tile8"].load(":/tiles/16tile02.jpg");
	m_textures["tile9"].load(":/tiles/16tile05.jpg");
	m_textures["tile10"].load(":/tiles/16tile08.jpg");
	m_textures["icub"].load(":/tiles/16tile11.jpg"); //.load( ":/metal/iron05.jpg" );
	m_textures["icubFace"].load(":/covers/face.jpg");
	m_textures["blueye"].load(":/covers/eyep2_b.jpg");
	m_textures["metal"].load(":/metal/iron05.jpg");
	m_textures["marXbot_12leds"].load(":/covers/marxbot_12leds.jpg");
	//--- The order of the texture is:
	// 0 => TOP
	// 1 => BACK
	// 2 => FRONT
	// 3 => BOTTOM
	// 4 => RIGHT
	// 5 => LEFT
/*	skyb[0].load( ":/skybox/sb_top.jpg" );
	skyb[1].load( ":/skybox/sb_back.jpg" );
	skyb[2].load( ":/skybox/sb_front.jpg" );
	skyb[3].load( ":/skybox/sb_bottom.jpg" );
	skyb[4].load( ":/skybox/sb_right.jpg" );
	skyb[5].load( ":/skybox/sb_left.jpg" );*/
	m_textures["skyb0"].load(":/skybox/sb2_top.jpg");
	m_textures["skyb1"].load(":/skybox/sb2_back.jpg");
	m_textures["skyb2"].load(":/skybox/sb2_front.jpg");
	m_textures["skyb3"].load(":/ground/cobbles01.jpg");
	m_textures["skyb4"].load(":/skybox/sb2_right.jpg");
	m_textures["skyb5"].load(":/skybox/sb2_left.jpg");

	// Creating the initial materials
	m_mats->createInitialMaterials();

	// Notifying all containers of existing textures (at the moment there should be no renderers)
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		notifyRendererContainerOfAllEntitiesAndTextures(c);
	}
}

void World::destroyWorld()
{
	// First of removing all textures. Renderer containers are not deleted and renderers will be
	// removed as we destroy entities
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		c->deleteAllTextures();
	}
	m_textures.clear();

	// Now deleting entities. Here we have to use a copy of m_entities to avoid removing iterators
	// which are stored in the m_entitiesMap map. elements of m_entities will be removed in
	// calls to deleteEntity (and we check after the cycle whether m_entities is in fact empty)
	QLinkedList<WEntityAndBuddies> entitiesListCopy = m_entities;
	while (!entitiesListCopy.isEmpty()) {
		WEntityAndBuddies e = entitiesListCopy.takeFirst();

		// We don't delete entities that have a parent because the parent has to delete them.
		// However, we remove them from the list
		if (e.entity->owner() == nullptr) {
			deleteEntity(e.entity);
		}
	}

	// Now doing the same for shadow entities
	QLinkedList<WEntityAndBuddies> shadowEntitiesListCopy = m_shadowEntities;
	while (!shadowEntitiesListCopy.isEmpty()) {
		WEntityAndBuddies e = shadowEntitiesListCopy.takeFirst();

		// We don't delete entities that have a parent because the parent has to delete them.
		// However, we remove them from the list
		if (e.entity->owner() == nullptr) {
			deleteEntity(e.entity);
		}
	}

#ifdef SALSA_DEBUG
	// Safety check, m_entities should now be empty
	if (Q_UNLIKELY(!m_entities.isEmpty())) {
		qFatal("List of entities not empty in destroyWorld");
	}
	// Safety check, m_shadowEntities should now be empty
	if (Q_UNLIKELY(!m_shadowEntities.isEmpty())) {
		qFatal("List of shadow entities not empty in destroyWorld");
	}
	// Safety check, there should be no renderer in Renderer containers
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		if (Q_UNLIKELY(!c->isEmpty())) {
			qFatal("Renderers still alive in some renderer containers in destroyWorld")
		}
	}
#endif

#ifdef WORLDSIM_USE_NEWTON
	NewtonDestroy( m_priv->world );
#endif

	m_priv.reset();

	m_mats.reset();

	m_time = 0.0f;
	m_creatingSomething = false;
	m_isInitialized = false;
}

bool World::checkCreatingFromWorldAndResetFlag()
{
	const bool ret = m_creatingSomething;

	m_creatingSomething = false;

	return ret;
}

void World::postCreationActionForType(PhyJoint* entity)
{
	// Calling functions to create the stuffs needed by the physical engine
	entity->createPrivateJoint();
	entity->postCreatePrivateJoint();

	m_mapObjJoints[entity->child()].push_back(entity);
	if (entity->parent() != nullptr) {
		m_mapObjJoints[entity->parent()].push_back(entity);
	}
}

void World::postCreationActionForType(PhyObject* entity, bool onlyCreateCollisionShape, const wMatrix* collisionShapeOffset)
{
	// Calling functions to create the stuffs needed by the physical engine
	entity->createPrivateObject(onlyCreateCollisionShape, collisionShapeOffset);
	entity->postCreatePrivateObject();
}

void World::notifyRendererContainersOfNewEntity(AbstractRenderWEntityCreator* rendererCreator)
{
	foreach (AbstractRendererContainer* c, m_renderersContainers) {
		c->addRenderer(rendererCreator);
	}
}

void World::notifyRendererContainerOfAllEntitiesAndTextures(AbstractRendererContainer* rendererContainer)
{
	// We have to build the list of renderer creators. This is not very efficient but this function will
	// hopefully be called few times during a simulation
	QList<AbstractRenderWEntityCreator*> rendererCreators;

	foreach (WEntityAndBuddies e, m_entities) {
		rendererCreators.append(e.rendererCreator);
	}

	// Setting the list of entities and textures and the graphical information about the world
	rendererContainer->setRenderersList(rendererCreators);
	rendererContainer->setTexturesList(m_textures);

	WorldGraphicalInfo info;
	info.minP = m_minP;
	info.maxP = m_maxP;
	rendererContainer->setWorldGraphicalInfo(info);
}

} // end namespace salsa
