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

#include "guirendererscontainer.h"

namespace salsa {

RenderingDataToGUI::~RenderingDataToGUI()
{
	// Freeing all AbstractRenderingProxy
	foreach (AbstractRenderingProxy* p, renderingProxies) {
		delete p;
	}
	renderingProxies.clear();
}

GUIRenderersContainer::GUIRenderersContainer(World* world, WorldDataUploadeDownloaderGUISide* otherEnd)
	: AbstractRendererContainer(world)
	, WorldDataUploadeDownloaderSimSide(1, SignalUploader, NoNotification)
	, m_renderersChanged()
	, m_texturesChanged()
	, m_worldGraphicalInfoChanged()
	, m_renderingProxiesChanged()
	, m_renderers()
	, m_renderersToDelete()
	, m_worldGraphicalInfo()
{
	if (otherEnd != NULL) {
		GlobalUploaderDownloader::attach(this, otherEnd);
	}
}

GUIRenderersContainer::~GUIRenderersContainer()
{
	// Deleting all renderers we can delete
	deleteRenderers();
}

void GUIRenderersContainer::update()
{
	// Sending data to the GUI
	sendData();

	// Checking if we can delete some renderers
	deleteRenderers();
}

void GUIRenderersContainer::rendererAdded(RenderWEntity* renderer)
{
	// Adding to our set of renderers
	m_renderers.insert(renderer);

	m_renderersChanged.triggerUpdate();
	m_renderingProxiesChanged.triggerUpdate();
}

void GUIRenderersContainer::rendererToBeDeleted(RenderWEntity* renderer)
{
	// Removing from our set of renderers
	m_renderers.remove(renderer);

	// Adding the renderer to the set of renderers to delete
	m_renderersToDelete.insert(renderer);

	m_renderersChanged.triggerUpdate();
	m_renderingProxiesChanged.triggerUpdate();
}

void GUIRenderersContainer::textureAdded(const QString&)
{
	// We always copy the whole list if there is a change, so we don't need to keep track of which texture has been added

	m_texturesChanged.triggerUpdate();
}

void GUIRenderersContainer::textureToBeDeleted(const QString&)
{
	// We always copy the whole list if there is a change, so we don't need to keep track of which texture has been deleted

	m_texturesChanged.triggerUpdate();
}

void GUIRenderersContainer::worldGraphicalInfoChanged(const WorldGraphicalInfo& info)
{
	// Copying the updated information and triggering an update
	m_worldGraphicalInfo = info;

	m_worldGraphicalInfoChanged.triggerUpdate();
}

void GUIRenderersContainer::sendData()
{
	// If we are not associated, doing nothing
	if (!downloaderPresent()) {
		return;
	}

	// Getting a datum to upload
	DatumToUpload d(*this);

	// If the queue is full skipping this update
	if (!d) {
		return;
	}

	// Checking if we have to update the list of rendering proxies for this RenderingDataToGUI object. We
	// check which proxies to delete and which to create by confronting the set of renderers (we have not
	// updated it in the datum to upload, yet)
	if (d->renderingProxiesChecker.updateNeeded(m_renderingProxiesChanged)) {
		QSet<RenderWEntity*> renderersAdded = m_renderers - d->renderers;
		QSet<RenderWEntity*> renderersDeleted = d->renderers - m_renderers;

		// Deleting all proxies whose renderer has been deleted
		QList<AbstractRenderingProxy*>::iterator it = d->renderingProxies.begin();
		while (it != d->renderingProxies.end()) {
			if (renderersDeleted.contains((*it)->renderer())) {
				delete *it;
				it = d->renderingProxies.erase(it);
			} else {
				++it;
			}
		}

		// Adding a proxy for each new renderer
		foreach (RenderWEntity* r, renderersAdded) {
			d->renderingProxies.append(creatorsForRenderers()[r]->generateRenderingProxyWithCopy(r));
		}

		// Now we also update the list of renderers in the datum to upload
		d->renderers = m_renderers;
	}

	// Syncing the update trigger of the renderers (we have already synced the list)
	d->renderersChanged = m_renderersChanged;

	// Checking if we have to send a new list of textures and syncing the update trigger
	if (m_texturesChanged.recentUpdateTriggered()) {
		d->textures = textures();
	}
	d->texturesChanged = m_texturesChanged;

	// Checking if we have to send new information about the world
	if (m_worldGraphicalInfoChanged.recentUpdateTriggered()) {
		d->worldGraphicalInfo = m_worldGraphicalInfo;
	}
	d->worldGraphicalInfoChanged = m_worldGraphicalInfoChanged;

	// Also copying the contact map
	d->contacts = world()->contacts();

	// Now copying data
	foreach (AbstractRenderingProxy* p, d->renderingProxies) {
		p->copyDataFromWEntity();
	}
}

void GUIRenderersContainer::deleteRenderers()
{
	// If we are not associated, deleting all renderers to delete
	if (!uploaderPresent()) {
		foreach (RenderWEntity* r, m_renderersToDelete) {
			delete r;
		}
		m_renderersToDelete.clear();
	} else {
		// Trying to get the list of renderers to delete from the GUI
		const RenderingDataFromGUI* d;

		while ((d = downloadDatum()) != NULL) {
#ifdef SALSA_DEBUG
			// Safety check: the renderers the GUI tells us to delete should also be in our set of renderers to delete
			if (!m_renderersToDelete.contains(d->renderersToDelete)) {
				abort();
			}
#endif

			// Deleting all renderers we can delete
			foreach (RenderWEntity* r, d->renderersToDelete) {
				delete r;

				m_renderersToDelete.remove(r);
			}
		}
	}
}

GUIRenderersContainerDataExchange::GUIRenderersContainerDataExchange(int updateDelay, WorldDataUploadeDownloaderSimSide* otherEnd, QObject* parent)
	: QObject(parent)
	, WorldDataUploadeDownloaderGUISide(1, SignalUploader, NoNotification)
	, m_updateDelay(updateDelay)
	, m_updateTimer()
	, m_renderers()
	, m_textures()
	, m_worldGraphicalInfo()
	, m_renderersToDelete()
	, m_data(NULL)
	, m_dummyContacts()
	, m_renderersChanged()
	, m_texturesChanged()
	, m_worldGraphicalInfoChanged()
{
	if (otherEnd != NULL) {
		GlobalUploaderDownloader::attach(this, otherEnd);
	}

	m_updateTimer.setInterval(m_updateDelay);
	m_updateTimer.setSingleShot(false);

	// Connecting timer to the update slot
	connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(checkUpdates()));

	m_updateTimer.start();
}

GUIRenderersContainerDataExchange::~GUIRenderersContainerDataExchange()
{
	// If we are not associated, deleting all renderers we know of
	if (!downloaderPresent()) {
		foreach (RenderWEntity* r, m_renderersToDelete) {
			delete r;
		}
		m_renderersToDelete.clear();

		if (m_data != NULL) {
			foreach (RenderWEntity* r, m_data->renderers) {
				delete r;
			}
		}
	}
}

void GUIRenderersContainerDataExchange::render(GLContextAndData* contextAndData)
{
	// Calling render() on all proxies (if we have data)
	if (m_data != NULL) {
		foreach (AbstractRenderingProxy* r, m_data->renderingProxies) {
			r->render(contextAndData);
		}
	}
}

const ContactMap& GUIRenderersContainerDataExchange::contacts() const
{
	if (m_data != NULL) {
		return m_data->contacts;
	} else {
		return m_dummyContacts;
	}
}

real GUIRenderersContainerDataExchange::timeStep() const
{
	if (m_data != NULL) {
		return m_data->timeStep;
	} else {
		return 0.0;
	}
}

real GUIRenderersContainerDataExchange::elapsedTime() const
{
	if (m_data != NULL) {
		return m_data->elapsedTime;
	} else {
		return 0.0;
	}
}

void GUIRenderersContainerDataExchange::checkUpdates()
{
	// Trying to get new data
	const RenderingDataToGUI* newData = downloadDatum();

	// Checking if there is something new
	if (newData != NULL) {
		m_data = newData;

		// Checking updates for renderers
		if (m_renderersChanged.updateNeeded(m_data->renderersChanged)) {
			// First of all updating the set of renderers to delete
			m_renderersToDelete.unite(m_renderers - m_data->renderers);

			// Now saving the list of renderers
			m_renderers = m_data->renderers;
		}

		// Now checking if there are updates for textures
		if (m_texturesChanged.updateNeeded(m_data->texturesChanged)) {
			m_textures = m_data->textures;

			emit newTextures();
		}

		// Also checking if graphical information about the world has changed
		if (m_worldGraphicalInfoChanged.updateNeeded(m_data->worldGraphicalInfoChanged)) {
			m_worldGraphicalInfo = m_data->worldGraphicalInfo;

			emit worldGraphicalInfoChanged();
		}

		// We have received new data, emitting the updated() signal
		emit updated();
	}

	// If we have renderers to delete, trying to send the set to the simulation thread
	if (!m_renderersToDelete.isEmpty()) {
		// Getting a datum to upload
		DatumToUpload d(*this);

		// If the queue is full we do not clear m_renderersToDelete so that we can send the list later
		if (!d) {
			return;
		}

		// Sending the set to the simulation thread and clearing our set
		d->renderersToDelete = m_renderersToDelete;
		m_renderersToDelete.clear();
	}
}

} // end namespace salsa
