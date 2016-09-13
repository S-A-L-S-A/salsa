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

#include "wentity.h"
#include "world.h"
#include "worldsimexceptions.h"

namespace salsa {

WEntity::WEntity(World* world, SharedDataWrapper<Shared> shared, QString name)
	: m_world(world)
	, m_shared(shared)
	, m_name(name)
	, m_owner(nullptr)
	, m_owned()
	, m_useColorTextureOfOwner(true)
	, m_myTexture(m_shared->texture)
	, m_myColor(m_shared->color)
	, m_ownershipChangesListener(nullptr)
{
	// Checking that we are being created inside a call to World::createEntity
	if (!m_world->checkCreatingFromWorldAndResetFlag()) {
		throw EntityCreatedOutsideWorldException(name.toLatin1().data());
	}
}

WEntity::~WEntity()
{
	// Notifying listener if we have to
	if (m_ownershipChangesListener != nullptr) {
		m_ownershipChangesListener->entityDestroyed(this);
	}

	// If I'm owned by some other object, removing myself from the list
	if (m_owner != nullptr) {
		m_owner->removeFromOwned(this, nullptr, true);
	}

	// Now deleting all owned objects I have to delete
	foreach (Owned o, m_owned) {
		if (o.destroy) {
			m_world->deleteEntity(o.object);
		}
	}

	m_owned.clear();
}

void WEntity::setName(QString name)
{
	m_name = name;
}

#warning ============= RIMUOVERE PARAMETRO DESTROY, QUI, OWNER DEVE SEMPRE DISTRUGGERE OWNED =============
void WEntity::setOwner(WEntity* owner, bool destroy)
{
	// If I was owned by another object, first removing from my old owner
	if ((m_owner != nullptr) && (m_owner != owner)) {
		m_owner->removeFromOwned(this, owner, false);
	}

	WEntity* const oldOwner = m_owner;
	m_owner = owner;

	// Now adding to the new owner
	if (m_owner != nullptr) {
		m_owner->addToOwned(this, destroy);
	}

	// Updating color and texture depending on the value of m_useColorTextureOfOwner
	updateColorTextureFromOwner();

	// Notifying of the change
	if (m_ownershipChangesListener != nullptr) {
		m_ownershipChangesListener->ownerChanged(this, oldOwner);
	}
}

void WEntity::setTexture(QString textureName)
{
	// Storing the texture in my variable
	m_myTexture = textureName;

	// If we don't use the texture of the owner, also updating m_shared and our owned WObjects
	if (!m_useColorTextureOfOwner) {
		m_shared.getModifiableShared()->texture = m_myTexture;

		updateColorTextureOfOwned();
	}
}

const QString& WEntity::texture(bool actual) const
{
	if (actual) {
		return m_shared->texture;
	} else {
		return m_myTexture;
	}
}

void WEntity::setColor(QColor c)
{
	// Storing the color in my variable
	m_myColor = c;

	// If we don't use the color of the owner, also updating m_shared and our owned WObjects
	if (!m_useColorTextureOfOwner) {
		m_shared.getModifiableShared()->color = m_myColor;

		updateColorTextureOfOwned();
	}
}

void WEntity::setAlpha(int alpha)
{
	// Storing the alpha in my variable
	m_myColor.setAlpha(alpha);

	// If we don't use the color of the owner, also updating m_shared and our owned WObjects
	if (!m_useColorTextureOfOwner) {
		m_shared.getModifiableShared()->color = m_myColor;

		updateColorTextureOfOwned();
	}
}

const QColor& WEntity::color(bool actual) const
{
	if (actual) {
		return m_shared->color;
	} else {
		return m_myColor;
	}
}

bool WEntity::useColorTextureOfOwner() const
{
	return m_useColorTextureOfOwner;
}

void WEntity::setUseColorTextureOfOwner(bool b)
{
	if (b != m_useColorTextureOfOwner) {
		m_useColorTextureOfOwner = b;

		if (m_useColorTextureOfOwner) {
			updateColorTextureFromOwner();
		} else {
			Shared* const d = m_shared.getModifiableShared();
			d->texture = m_myTexture;
			d->color = m_myColor;

			updateColorTextureOfOwned();
		}
	}
}

void WEntity::preUpdate()
{
}

void WEntity::postUpdate()
{
}

OwnershipChangesListener* WEntity::registerOwnertshipChangesListener(OwnershipChangesListener* listener)
{
	OwnershipChangesListener* oldListener = m_ownershipChangesListener;

	m_ownershipChangesListener = listener;

	if (oldListener != nullptr) {
		oldListener->listenerChanged(this, m_ownershipChangesListener);
	}

	return oldListener;
}

void WEntity::updateColorTextureOfOwned()
{
	OwnedList ownedStack;

	// Filling the stack with all child WObjects
	ownedStack = owned();
	while (!ownedStack.isEmpty()) {
		// Popping one child off the stack
		WEntity* child = ownedStack.takeLast().object;

		// If child uses color and texture of owner, changing its color and texture and adding its children
		if (child->useColorTextureOfOwner()) {
			// Setting color and texture of child
			Shared* const d = child->m_shared.getModifiableShared();
			d->texture = m_shared->texture;
			d->color = m_shared->color;

			// Adding the children of child
			ownedStack.append(child->owned());
		}
	}
}

void WEntity::updateColorTextureFromOwner()
{
	// Getting the first WEntity owner using its own texture and color
	WEntity* curObj = this;
	WEntity* curOwner = owner();
	while (curObj->useColorTextureOfOwner() && (curOwner != nullptr)) {
		curObj = curOwner;
		curOwner = curOwner->owner();
	}

	Shared* const d = m_shared.getModifiableShared();
	d->texture = curObj->m_shared->texture;
	d->color = curObj->m_shared->color;

	// Also updating owned objects
	updateColorTextureOfOwned();
}


void WEntity::addToOwned(WEntity* obj, bool destroy)
{
	m_owned.append(Owned(obj, destroy));

	// Notifying that we now own a new object
	if (m_ownershipChangesListener != nullptr) {
		m_ownershipChangesListener->ownedAdded(this, obj);
	}
}

void WEntity::removeFromOwned(WEntity* obj, WEntity* newOwner, bool destroyed)
{
	// Calling ownedChangedOwner to notify of the change
	if (m_ownershipChangesListener != nullptr) {
		m_ownershipChangesListener->ownedChangedOwner(this, obj, newOwner, destroyed);
	}

	m_owned.removeAll(Owned(obj));
}

RenderWEntity::RenderWEntity(const WEntity*)
{
}

RenderWEntity::~RenderWEntity()
{
}

void RenderWEntity::render(const WEntityShared*, GLContextAndData*)
{
}

void RenderWEntity::setupColorTexture(const WEntityShared* sharedData, GLContextAndData* contextAndData)
{
	// Set the color
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(sharedData->color.redF(), sharedData->color.greenF(), sharedData->color.blueF(), sharedData->color.alphaF());

	// Adds the texture
	applyTexture(sharedData->texture, contextAndData);
}

void RenderWEntity::applyTexture(const QString& texture, GLContextAndData* contextAndData)
{
	GLuint textureID;

	if (contextAndData->textureID(texture, textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
	}
}

}
