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

#include "rendererscontainer.h"
#include "world.h"
#include "worldsimexceptions.h"

namespace salsa {

AbstractRendererContainer::AbstractRendererContainer(World* world)
	: m_world(world)
	, m_creatorsForRenderers()
	, m_rendersForCreators()
	, m_textures()
{
	// Checking that we are being created inside a call to World::createRenderersContainer
	if (!m_world->checkCreatingFromWorldAndResetFlag()) {
		throw RendererContainerCreatedOutsideWorldException();
	}
}

AbstractRendererContainer::~AbstractRendererContainer()
{
	// We do nothing here, call delete renderers in subclasses
}

void AbstractRendererContainer::setRenderersList(const QList<AbstractRenderWEntityCreator*> rendererCreators)
{
	deleteAllRenderers();

	for(int i = 0; i < rendererCreators.size(); ++i) {
		addRenderer(rendererCreators[i]);
	}
}

void AbstractRendererContainer::addRenderer(AbstractRenderWEntityCreator* rendererCreator)
{
	// Creating the renderer and adding it and the creator to the maps
	RenderWEntity* const renderer = rendererCreator->generateRenderer();
	m_creatorsForRenderers[renderer] = rendererCreator;
	m_rendersForCreators[rendererCreator] = renderer;

	// Calling the callback
	rendererAdded(renderer);
}

void AbstractRendererContainer::deletedRenderer(AbstractRenderWEntityCreator* rendererCreator)
{
	// Checking that the creator is in the map. If it is not or the index is wrong, aborting
	RenderWEntity* const renderer = m_rendersForCreators.value(rendererCreator, NULL);
	if ((renderer == NULL) || (m_creatorsForRenderers.value(renderer, NULL) != rendererCreator)){
		abort();
	}

	// Calling callback
	rendererToBeDeleted(renderer);

	// Here we don't delete the renderer as it could be living in another thread. We expect rendererToBeDeleted() to have
	// deleted it
	m_creatorsForRenderers.remove(renderer);
	m_rendersForCreators.remove(rendererCreator);
}

void AbstractRendererContainer::deleteAllRenderers()
{
	while (m_creatorsForRenderers.size() != 0) {
		deletedRenderer(m_creatorsForRenderers.begin().value());
	}
}

bool AbstractRendererContainer::isEmpty() const
{
	return m_creatorsForRenderers.isEmpty();
}

void AbstractRendererContainer::setTexturesList(const QMap<QString, QImage>& textures)
{
	deleteAllTextures();

	// Adding all textures
	for(QMap<QString, QImage>::const_iterator it = textures.constBegin(); it != textures.constEnd(); ++it) {
		addTexture(it.key(), it.value());
	}
}

void AbstractRendererContainer::addTexture(const QString& name, const QImage& texture)
{
	// Adding texture to the map
	m_textures[name] = texture;

	// Calling callback
	textureAdded(name);
}

void AbstractRendererContainer::deleteTexture(const QString& name)
{
	// Calling callback
	textureToBeDeleted(name);

	// Removing texture from the map
	m_textures.remove(name);
}

void AbstractRendererContainer::deleteAllTextures()
{
	// We have to call deleteTexture for all textures
	QMap<QString, QImage>::iterator it = m_textures.begin();
	while (it != m_textures.end()) {
		// Calling callback
		textureToBeDeleted(it.key());

		// Removing texture
		it = m_textures.erase(it);
	}
}

void AbstractRendererContainer::setWorldGraphicalInfo(const WorldGraphicalInfo& info)
{
	// Here we simply call the virtual function
	worldGraphicalInfoChanged(info);
}

} // end namespace salsa
