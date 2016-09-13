/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
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

#ifndef RENDER2DGUI_H
#define RENDER2DGUI_H

#include <QLabel>
#include "renderer2d.h"
#include "experimentsconfig.h"

namespace salsa {

/**
 * \brief The widget rendering an image from a Renderer2D
 *
 * This can be associated to a Renderer2D object to obtain an image to render.
 * This also communicates its own current size an visibility to the Renderer2D
 */
class SALSA_EXPERIMENTS_API Render2DGui : public QLabel, public DataUploaderDownloader<Rendering2DDataFromGui, Rendering2DDataToGui>
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param parent the parent widget
	 * \param f the window flags
	 */
	Render2DGui(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	/**
	 * \brief Destructor
	 */
	virtual ~Render2DGui();

private:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void timerEvent(QTimerEvent *event);
	void sendInfo();

	int m_timerID;
};

} //end namespace salsa

#endif

