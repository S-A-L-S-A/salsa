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

#ifndef RENDERERSCONTAINER_H
#define RENDERERSCONTAINER_H

#include <QList>
#include <QMap>
#include <QHash>
#include <QString>
#include <QImage>
#include "wentity.h"

namespace salsa {

class World;
class AbstractRenderWEntityCreator;
class WorldGraphicalInfo;

/**
 * \brief The abstract class containing renderers
 *
 * This class contains the renderers for entities. Instances are created using
 * the World::createRenderersContainer() template function and deleted using the
 * World::deleteRenderersContainer() function (this is why constructor and
 * destructor are protected and World is friend). Once created, World will
 * notify whenever a change happends (e.g. new object created, object destroyed,
 * new texture added, world updated...). Children of this class set up
 * everything that's needed to render objects. Instances of this or child
 * classes must live in the same thread as World.
 * \note Renderers are never deleted by this class (neither in the destructor
 *       nor in deletedRenderer()), because they could be living in another
 *       thread. Delete them in subclasses (both in the destructor and in
 *       rendererToBeDeleted())
 */
class SALSA_WSIM_API AbstractRendererContainer
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world we live in
	 */
	AbstractRendererContainer(World* world);

	/**
	 * \brief Destructor
	 *
	 * This does not delete renderers, do it in subclass (renderers could be
	 * living in another thread...)
	 */
	virtual ~AbstractRendererContainer();

public:
	/**
	 * \brief Called to set the complete list of renderers for all entities
	 *
	 * This removes all previous entities renderers
	 * \param rendererCreators the list of renderer creators for all
	 *                         entities
	 */
	void setRenderersList(const QList<AbstractRenderWEntityCreator*> rendererCreators);

	/**
	 * \brief Called when a new entity is added to the world
	 *
	 * \param rendererCreator the renderer creator for the new entity
	 */
	void addRenderer(AbstractRenderWEntityCreator* rendererCreator);

	/**
	 * \brief Called when an entity is removed from the world
	 *
	 * \param rendererCreator the renderer creator for the entity being
	 *                        removed
	 */
	void deletedRenderer(AbstractRenderWEntityCreator* rendererCreator);

	/**
	 * \brief Called when all entities are removed from world
	 */
	void deleteAllRenderers();

	/**
	 * \brief Returns true if there is no renderer
	 *
	 * \return true if there is no renderer
	 */
	bool isEmpty() const;

	/**
	 * \brief Called to set the complete list of textures
	 *
	 * This removes all previous textures
	 * \param textures the list of textures
	 */
	void setTexturesList(const QMap<QString, QImage>& textures);

	/**
	 * \brief Called when a new texture is added
	 *
	 * \param name the name of the texture
	 * \param texture the texture image
	 */
	void addTexture(const QString& name, const QImage& texture);

	/**
	 * \brief Called when a texture is removed
	 *
	 * \param name the name of the texture that is removed
	 */
	void deleteTexture(const QString& name);

	/**
	 * \brief Called when all textures are removed
	 */
	void deleteAllTextures();

	/**
	 * \brief Called when graphical information of world change
	 *
	 * \param info the new graphical information of the world
	 */
	void setWorldGraphicalInfo(const WorldGraphicalInfo& info);

	/**
	 * \brief Called when the world state is updated
	 *
	 * When this is called, the scene should be rendered
	 */
	virtual void update() = 0;

protected:
	/**
	 * \brief Returns a const reference to the map from renderers to the
	 *        corresponding creator
	 *
	 * \return a const reference to the map from renderers to the
	 *         corresponding creator
	 */
	const QHash<RenderWEntity*, AbstractRenderWEntityCreator*>& creatorsForRenderers() const
	{
		return m_creatorsForRenderers;
	}

	/**
	 * \brief Returns a reference to the map from renderers to the
	 *        corresponding creator
	 *
	 * \return a reference to the map from renderers to the corresponding
	 *         creator
	 */
	QHash<RenderWEntity*, AbstractRenderWEntityCreator*>& creatorsForRenderers()
	{
		return m_creatorsForRenderers;
	}

	/**
	 * \brief Returns a const reference to the map from creators to the
	 *        corresponding renderer
	 *
	 * \return a const reference to the map from creators to the
	 *         corresponding renderer
	 */
	const QHash<AbstractRenderWEntityCreator*, RenderWEntity*>& renderersForCreators() const
	{
		return m_rendersForCreators;
	}

	/**
	 * \brief Returns a reference to the map from creators to the
	 *        corresponding renderer
	 *
	 * \return a reference to the map from creators to the corresponding
	 *         renderer
	 */
	QHash<AbstractRenderWEntityCreator*, RenderWEntity*>& renderersForCreators()
	{
		return m_rendersForCreators;
	}

	/**
	 * \brief Returns a const reference to the map of textures and their
	 *        names
	 *
	 * \return a const reference to the map of textures and their names
	 */
	const QMap<QString, QImage>& textures() const
	{
		return m_textures;
	}

	/**
	 * \brief Returns a reference to the map of textures and their names
	 *
	 * \return a reference to the map of textures and their names
	 */
	QMap<QString, QImage>& textures()
	{
		return m_textures;
	}

	/**
	 * \brief Returns a const pointer to world we live in
	 *
	 * \return a const pointer to world we live in
	 */
	const World* world() const
	{
		return m_world;
	}

private:
	/**
	 * \brief Implement in subclases to handle the case of a new entity
	 *        added
	 *
	 * This is called after the renderer has been created and added to the
	 * maps
	 * \param renderer the renderer just created
	 */
	virtual void rendererAdded(RenderWEntity* renderer) = 0;

	/**
	 * \brief Implement in subclases to handle the case of an entity being
	 *        removed
	 *
	 * This is called before the renderer is deleted. The renderer should
	 * be deleted here, as it is not deleted in the calling function (it
	 * could be living in another thread...)
	 * \param renderer the renderer to be removed
	 */
	virtual void rendererToBeDeleted(RenderWEntity* renderer) = 0;

	/**
	 * \brief Implement in subclases to handle the case of a new texture
	 *        added
	 *
	 * This is called after the texture has been added to the map
	 * \param name the name of the texture that has just been added
	 */
	virtual void textureAdded(const QString& name) = 0;

	/**
	 * \brief Implement in subclases to handle the case of a texture being
	 *        removed
	 *
	 * This is called before the texture is removed from the map
	 * \param name the name of the texture that is going to be deleted
	 */
	virtual void textureToBeDeleted(const QString& name) = 0;

	/**
	 * \brief Implement in subclasses to handle the case of world graphical
	 *        information changes
	 *
	 * \param info the updated graphical information about the world
	 */
	virtual void worldGraphicalInfoChanged(const WorldGraphicalInfo& info) = 0;

	/**
	 * \brief The world in which we live
	 */
	World* const m_world;

	/**
	 * \brief The map from renderers to the corresponding creator
	 */
	QHash<RenderWEntity*, AbstractRenderWEntityCreator*> m_creatorsForRenderers;

	/**
	 * \brief The map from creators to the corresponding renderer
	 */
	QHash<AbstractRenderWEntityCreator*, RenderWEntity*> m_rendersForCreators;

	/**
	 * \brief The map with textures and their names
	 */
	QMap<QString, QImage> m_textures;

	/**
	 * \brief World is friend to be able to instantiate objects of this type
	 */
	friend class World;
};

// Ci sono principalmente due sottoclassi: una per la camera che
// fa il rendering quando viene chiamata update(), l'altra per renderworld che
// quando viene chiamata update() fa la copia dei dati e avverte la GUI di fare
// il rendering (solo se la coda è libera)


// class CameraRendererContainer : public AbstractRendererContainer
// {
// 	CameraRendererContainer(...)
// 		: ...
// 	{
// 		initialize m_glContextAndData
// 	}
//
// 	~CameraRendererContainer()
// 	{
// 		deleteObjectsList();
// 	}
//
// 	virtual void update()
// 	{
// 		foreach(AbstractRenderingProxy* p, sharedProviders()) {
// 			p->copyDataFromWObject(); // Questa possiamo evitarcela, tanto sappiamo che non fa niente
// 		}
//
// 		inizializzare renderizzazione
//
// 		foreach(AbstractRenderingProxy* p, sharedProviders()) {
// 			p->render(m_glContextAndData);
// 		}
//
// 		finalizzare renderizzazione
// 	}
//
// private:
// 	virtual void rendererAdded(int index)
// 	{
// 		// Creo l'unico shared provider che mi serve
// 		m_sharedProviders.insert(index, rendererCreators(index)->doGenerateSharedProviderWithoutCopy(renderers(index));
// 	}
//
// 	virtual void rendererToBeDeleted(int index)
// 	{
// 		// Possiamo cancellare tutto, in questa classe l'update è sincrono
// 		delete m_renderers[index];
// 		delete m_sharedProviders[index];
//
// 		m_sharedProviders.removeAt(index);
// 	}
//
// 	implementare anche funzioni per le textures, le textures vanno in m_glContextAndData
//
// 	QList<AbstractRenderingProxy*> m_sharedProviders;
// 	GLContextAndData m_glContextAndData;
// };

} // end namespace salsa

#endif
