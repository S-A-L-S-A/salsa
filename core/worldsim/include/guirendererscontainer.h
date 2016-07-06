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

#ifndef GUIRENDERERSCONTAINER_H
#define GUIRENDERERSCONTAINER_H

#include "worldsimconfig.h"
#include "world.h"
#include <QObject>
#include "qglviewer/qglviewer.h"
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include <QColor>
#include <QMutex>
#include <QTimer>
#include "rendererscontainer.h"
#include "glcontextanddata.h"
#include "dataexchange.h"
#include "updatetrigger.h"

namespace farsa {

/**
 * \brief The class with rendering data sent from simulation thread to GUI
 *        thread
 */
class FARSA_WSIM_API RenderingDataToGUI
{
public:
	/**
	 * \brief Destructor
	 *
	 * This frees memory for all AbstractRenderingProxy in the set
	 */
	~RenderingDataToGUI();

	/**
	 * \brief The set of renderers
	 *
	 * This should be used only if an update has been triggered in
	 * renderersChanged
	 */
	QSet<RenderWEntity*> renderers;

	/**
	 * \brief The map with all textures
	 *
	 * This should be used only if an update has been triggered in
	 * texturesChanged
	 */
	QMap<QString, QImage> textures;

	/**
	 * \brief Graphical information about the world
	 *
	 * This should be used only if an update has been triggered in
	 * worldGraphicalInfoChanged
	 */
	WorldGraphicalInfo worldGraphicalInfo;

	/**
	 * \brief The list with all rendering proxies
	 *
	 * There is one list for every RenderingDataToGUI. It is the data
	 * structure that must be read at every timestep
	 */
	QList<AbstractRenderingProxy*> renderingProxies;

	/**
	 * \brief The map of contacts
	 */
	ContactMap contacts;

	/**
	 * \brief The timestep length
	 */
	real timeStep;

	/**
	 * \brief The elapsed time
	 */
	real elapsedTime;

	/**
	 * \brief A change is triggered every time a renderer is added or
	 *        deleted
	 */
	UpdateTriggerLong renderersChanged;

	/**
	 * \brief A change is triggered every time a texture is added or deleted
	 */
	UpdateTriggerLong texturesChanged;

	/**
	 * \brief A change is triggered every time the world graphical
	 *        information change
	 */
	UpdateTriggerLong worldGraphicalInfoChanged;

	/**
	 * \brief The checker for changes in the list of AbstractRenderingProxy
	 *
	 * This is used to update the set of AbstractRenderingProxy here, as
	 * every RenderingDataToGUI object has its own list
	 */
	UpdateCheckerLong renderingProxiesChecker;
};

/**
 * \brief The class with rendering data sent from GUI thread to simulation
 *        thread
 */
class FARSA_WSIM_TEMPLATE RenderingDataFromGUI
{
public:
	/**
	 * \brief The set of renderers to delete
	 *
	 * The simulation thread tells the GUI when the set of renderers changes
	 * and the GUI in turn tells us when it is safe to delete the renderers
	 */
	QSet<RenderWEntity*> renderersToDelete;
};

/**
 * \brief The type of the data uploader/downloader on the simulation side
 */
typedef DataUploaderDownloader<RenderingDataToGUI, RenderingDataFromGUI> WorldDataUploadeDownloaderSimSide;

/**
 * \brief The type of the data uploader/downloader on the GUI side
 */
typedef DataUploaderDownloader<RenderingDataFromGUI, RenderingDataToGUI> WorldDataUploadeDownloaderGUISide;

/**
 * \brief The RendererContainer which forwards rendering to the GUI
 *
 * This class is used to communicate rendering data to the GUI thread. The
 * object in the GUI thread which receives data is then responsible to dispatch
 * it to the widgets doing the actual rendering. Renderers are deleted here
 * after "permission" from the GUI (if it exists)
 */
class FARSA_WSIM_API GUIRenderersContainer : public AbstractRendererContainer, public WorldDataUploadeDownloaderSimSide
{
protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world we live in
	 * \param otherEnd the uplaoder/downloader living in the GUI thread.
	 *                 This can be NULL (in this case you must create this
	 *                 object before the GUI one and pass this to the
	 *                 constructor of the GUI one)
	 */
	GUIRenderersContainer(World* world, WorldDataUploadeDownloaderGUISide* otherEnd = NULL);

	/**
	 * \brief Destructor
	 *
	 * This does not delete renderers, do it in subclass (renderers could be
	 * living in another thread...)
	 */
	virtual ~GUIRenderersContainer();

public:
	/**
	 * \brief Called when the world state is updated
	 *
	 * When this is called, the scene should be rendered
	 */
	virtual void update();

private:
	/**
	 * \brief Implement in subclases to handle the case of a new entity
	 *        added
	 *
	 * This is called after the renderer has been created and added to the
	 * maps
	 * \param renderer the renderer just created
	 */
	virtual void rendererAdded(RenderWEntity* renderer);

	/**
	 * \brief Implement in subclases to handle the case of an entity being
	 *        removed
	 *
	 * This is called before the renderer is deleted. The renderer should
	 * be deleted here, as it is not deleted in the calling function (it
	 * could be living in another thread...)
	 * \param renderer the renderer to be removed
	 */
	virtual void rendererToBeDeleted(RenderWEntity* renderer);

	/**
	 * \brief Implement in subclases to handle the case of a new texture
	 *        added
	 *
	 * This is called after the texture has been added to the map
	 * \param name the name of the texture that has just been added
	 */
	virtual void textureAdded(const QString& name);

	/**
	 * \brief Implement in subclases to handle the case of a texture being
	 *        removed
	 *
	 * This is called before the texture is removed from the map
	 * \param name the name of the texture that is going to be deleted
	 */
	virtual void textureToBeDeleted(const QString& name);

	/**
	 * \brief Implement in subclasses to handle the case of world graphical
	 *        information changes
	 *
	 * \param info the updated graphical information about the world
	 */
	virtual void worldGraphicalInfoChanged(const WorldGraphicalInfo& info);

	/**
	 * \brief Sends data to the GUI
	 */
	void sendData();

	/**
	 * \brief Checks if we can delete some renderers
	 *
	 * This gets the list of renderers to delete from the GUI if we are
	 * associated
	 */
	void deleteRenderers();

	/**
	 * \brief A change is triggered every time a renderer is added or
	 *        deleted
	 */
	UpdateTriggerLong m_renderersChanged;

	/**
	 * \brief A change is triggered every time a texture is added or deleted
	 */
	UpdateTriggerLong m_texturesChanged;

	/**
	 * \brief A change is triggered every time the world graphical
	 *        information change
	 */
	UpdateTriggerLong m_worldGraphicalInfoChanged;

	/**
	 * \brief A change is triggered every time a renderer is added or
	 *        deleted (to update the list of rendering proxies)
	 *
	 * We need both this and m_renderersChanged because this one is needed
	 * to sync data in RenderingDataToGUI objects, while m_renderersChanged
	 * is needed to sync data with the GUI. In fact the GUI could have
	 * already been updated but the current RenderingDataToGUI object (i.e.
	 * the one we are going to upload) may be not synced because it is an
	 * old one
	 */
	UpdateTriggerLong m_renderingProxiesChanged;

	/**
	 * \brief The set of renderers
	 *
	 * We need this to quickly compute what changed in objects to upload
	 */
	QSet<RenderWEntity*> m_renderers;

	/**
	 * \brief The set of renderers to delete
	 *
	 * Here we hold all renderers to delete, but only actually delete those
	 * the GUI tells us to delete
	 */
	QSet<RenderWEntity*> m_renderersToDelete;

	/**
	 * \brief Graphical information about the world
	 *
	 * This contains information to send with the next update
	 */
	WorldGraphicalInfo m_worldGraphicalInfo;

	/**
	 * \brief World is friend to be able to instantiate objects of this type
	 */
	friend class World;
};

/**
 * \brief The class that receives data from the GUIRenderersContainer
 *
 * This class lives on the GUI thread and receives rendering data which then
 * dispatches to widgets doing the actual rendering.
 * \note All widgets share the same set of renderers!
 */
class FARSA_WSIM_API GUIRenderersContainerDataExchange : public QObject, public WorldDataUploadeDownloaderGUISide
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param updateDelay each how many milliseconds we must check for
	 *                    updates
	 * \param otherEnd the uplaoder/downloader living in the simulation
	 *                 thread. This can be NULL (in this case you must
	 *                 create this object before the simulation one and pass
	 *                 this to the constructor of the simulation one)
	 * \param parent the parent QObject
	 */
	GUIRenderersContainerDataExchange(int updateDelay, WorldDataUploadeDownloaderSimSide* otherEnd = NULL, QObject* parent = NULL);

	/**
	 * \brief Destructor
	 *
	 * This removes all renderers if we have some and are not associated
	 * with a simulation-thread object
	 */
	virtual ~GUIRenderersContainerDataExchange();

	/**
	 * \brief Performs the rendering
	 *
	 * \param contextAndData the object with the OpenGL context and data
	 */
	void render(GLContextAndData* contextAndData);

	/**
	 * \brief Returns the map of textures
	 *
	 * \return the updated map of textures
	 */
	const QMap<QString, QImage>& textures() const
	{
		return m_textures;
	}

	/**
	 * \brief Returns the map of contacts
	 *
	 * \return the map of contacts
	 */
	const ContactMap& contacts() const;

	/**
	 * \brief Returns the timestep length
	 *
	 * \return the timestep length
	 */
	real timeStep() const;

	/**
	 * \brief Returns the elapsed time
	 *
	 * \return the elapsed time
	 */
	real elapsedTime() const;

	/**
	 * \brief Returns graphical information about the world
	 *
	 * \return graphical information about the world
	 */
	const WorldGraphicalInfo& worldGraphicalInfo() const
	{
		return m_worldGraphicalInfo;
	}

signals:
	/**
	 * \brief The signal emitted when there are new textures
	 */
	void newTextures();

	/**
	 * \brief The signal emitted when world graphical information changed
	 */
	void worldGraphicalInfoChanged();

	/**
	 * \brief The signal emitted when there is an update
	 */
	void updated();

public slots:
	/**
	 * \brief Checks if there is new data from the simulation thread
	 *
	 * This function also sends data to the GUI with renderers that can be
	 * deleted
	 */
	void checkUpdates();

private:
	/**
	 * \brief Each how many milliseconds we must check for updates
	 */
	const int m_updateDelay;

	/**
	 * \brief The timer to check for updates
	 */
	QTimer m_updateTimer;

	/**
	 * \brief The current set of renderers
	 */
	QSet<RenderWEntity*> m_renderers;

	/**
	 * \brief The current map with all textures
	 */
	QMap<QString, QImage> m_textures;

	/**
	 * \brief Updated graphical information about the world
	 */
	WorldGraphicalInfo m_worldGraphicalInfo;

	/**
	 * \brief The set of renderers to delete
	 *
	 * This is needed in case we could not send the set of renderers to
	 * delete to the simulation immediately
	 */
	QSet<RenderWEntity*> m_renderersToDelete;

	/**
	 * \brief The latest downloaded datum
	 *
	 * We use the list of proxies directly from here, as it is updated at
	 * each step
	 */
	const RenderingDataToGUI* m_data;

	/**
	 * \brief A dummy contact map to return in case we do not have data
	 *        ready
	 */
	const ContactMap m_dummyContacts;

	/**
	 * \brief Checks whether a renderer has been added or deleted
	 */
	UpdateCheckerLong m_renderersChanged;

	/**
	 * \brief Checks whether a texture has been added or deleted
	 */
	UpdateCheckerLong m_texturesChanged;

	/**
	 * \brief Checks whether the world graphical have changed
	 */
	UpdateCheckerLong m_worldGraphicalInfoChanged;
};

} // end namespace farsa

#endif
