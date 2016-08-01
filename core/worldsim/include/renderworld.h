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

#ifndef RENDERWORLD_H
#define RENDERWORLD_H

#include "worldsimconfig.h"
#include <QObject>
#include "qglviewer/qglviewer.h"
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QString>
#include <QColor>
#include <QMutex>
#include "guirendererscontainer.h"
#include "glcontextanddata.h"
#include "wvector.h"

namespace salsa {

/**
 * \brief The 3D viewer of World
 */
class SALSA_WSIM_API RenderWorld : public QGLViewer
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param dataProvider the GUIRenderersContainerDataExchange object from
	 *                     which we take data to render
	 * \param parent the parent widget
	 */
	RenderWorld(GUIRenderersContainerDataExchange* dataProvider, QWidget* parent = NULL, Qt::WindowFlags flags = 0);

	/**
	 * \brief Destructor
	 */
	virtual ~RenderWorld();

public slots:
	/**
	 * \brief Sets whether to view object in wireframe mode or not
	 *
	 * \param b if true visualizes objects in wireframe mode
	 */
	void wireframe(bool b);

	/**
	 * \brief Enables or disables the SkyBox
	 *
	 * \param b if true draws the SkyBox
	 */
	void showSkyGround(bool b);

	/**
	 * \brief Shows or hides objects
	 *
	 * \param b if true draws objects
	 */
	void showObjects(bool b);

	/**
	 * \brief Shows or hides joints
	 *
	 * \param b if true draws joints
	 */
	void showJoints(bool b);

	/**
	 * \brief Shows or hides AABBs
	 *
	 * \param b if true draws AABBs
	 */
	void showAABBs(bool b);

	/**
	 * \brief Shows or hides contacts
	 *
	 * \param b if true draws contacts
	 */
	void showContacts(bool b);

	/**
	 * \brief Shows or hides forces
	 *
	 * \param b if true draws forces
	 */
	void showForces(bool b);

	/**
	 * \brief Shows or hides local axes for all objects
	 *
	 * \param b if true draws local axes for all objects
	 */
	void showLocalAxes(bool b);

	/**
	 * \brief Shows or hides labels for all objects
	 *
	 * \param b if true draws labels for all objects
	 */
	void showLabels(bool b);

	/**
	 * \brief Shows or hides the time and step
	 *
	 * \param b if true draws the time and step
	 */
	void showTimeAndStep(bool b);

	/**
	 * \brief Shows the context menu
	 *
	 * \param p thepoint where the context menu should be displayed
	 */
	void contextMenu(const QPoint& p);

private slots:
	/**
	 * \brief The slot called when there are new textures
	 */
	void newTextures();

	/**
	 * \brief The slot called when world graphical information changed
	 */
	void worldGraphicalInfoChanged();

	/**
	 * \brief The slot called when there is an update
	 */
	void updated();

protected:
	/**
	 * \brief Initializes renderWorld
	 */
	virtual void init();

	/**
	 * \brief Draws the scene
	 */
	virtual void draw();

	/**
	 * \brief Handles key pressed events
	 *
	 * \param e the key pressed event
	 */
	virtual void keyPressEvent(QKeyEvent *e);

private:
	/**
	 * \brief The function rendering the sky and ground
	 *
	 * This takes the current context and the textures from m_contextAndData
	 */
	void drawSkyGroundBox();

	/**
	 * \brief The object from which we take data to render
	 */
	GUIRenderersContainerDataExchange* const m_dataProvider;

	/**
	 * \brief The object with the OpenGL context and other data (e.g.
	 *        textures)
	 */
	GLContextAndData m_contextAndData;

	/**
	 * \brief Whether to draw the skybox or not
	 */
	bool m_showskygroundbox;

	/**
	 * \brief Whether to draw contacts or not
	 */
	bool m_showcontacts;

	/**
	 * \brief Whether to show time and step label or not
	 */
	bool m_showtimeandstep;

	/**
	 * \brief The minimum point of the world
	 */
	wVector m_worldMin;

	/**
	 * \brief The maximum point of the world
	 */
	wVector m_worldMax;

#warning FIX BUG WHICH WAS RESOLVED IN SALSA TRUNK BETWEEN VERSIONS 1311 AND 1474 ABOUT LOADING QGLViewer STATUS FROM FILE (bool m_viewerStateRestoredFromFile, bool viewerStateRestoredFromFile() const AND OTHER STUFFS)
};

} // end namespace salsa

#endif
