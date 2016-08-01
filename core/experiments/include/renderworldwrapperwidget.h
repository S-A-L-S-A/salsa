/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2014                                                     *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#ifndef RENDERWORLDWRAPPERWIDGET_H
#define RENDERWORLDWRAPPERWIDGET_H

#include "experimentsconfig.h"
#include "wmatrix.h"
#include "renderworld.h"
#include "guirendererscontainer.h"
#include <QWidget>
#include <QTimer>

class QVBoxLayout;

namespace salsa {

class RenderWorld;

/**
 * \brief The wrapper widget for RenderWorld
 *
 * This widget contains RenderWorld. Renderworld has to be wrapped inside a
 * QWidget, because it is not possible to set the flags as required by Total99
 * directly on the RenderWorld object. If you try to do this, it will crash on
 * some windows machines :-S
 */
class SALSA_EXPERIMENTS_API RenderWorldWrapperWidget : public QWidget
{
	Q_OBJECT
public:
	/**
	 * \brief Constructor
	 *
	 * \param otherEnd the simulation end of the data uploader/downloader
	 * \param parent the parent widget
	 * \param flags window flags
	 */
	RenderWorldWrapperWidget(WorldDataUploadeDownloaderSimSide* otherEnd, QWidget* parent = NULL, Qt::WindowFlags flags = 0);

	/**
	 * \brief Destructor
	 */
	virtual ~RenderWorldWrapperWidget();

protected:
	/**
	 * \brief Brings the camera in front of the robot
	 */
	void lookAtRobot();

	/**
	 * \brief The object receiving data from the simulation thread
	 */
	GUIRenderersContainerDataExchange* const m_dataExchanger;

	/**
	 * \brief The object actually rendering the world
	 */
	RenderWorld* const m_renderWorld;

	/**
	 * \brief The layout for this widget
	 */
	QVBoxLayout* const m_layout;

	/**
	 * \brief True if we have to set camera to look at the robot
	 */
	bool m_setCameraToLookAtRobot;

	/**
	 * \brief The transformation matrix of the iCub
	 *
	 * This is used to have the camera look at the robot if needed
	 */
	wMatrix m_robotTm;
};

} //end namespace salsa

#endif

