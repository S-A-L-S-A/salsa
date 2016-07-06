/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#ifndef RENDERER2D_H
#define RENDERER2D_H

#include <QImage>
#include "dataexchange.h"
#include "experimentsconfig.h"

namespace farsa {

/**
 * \brief The structure to send the image to display to the GUI
 */
struct FARSA_EXPERIMENTS_TEMPLATE Rendering2DDataToGui
{
	/**
	 * \brief The image to display
	 */
	QImage image;
};

/**
 * \brief The structure with information the gui sends to the 2D rendered
 */
struct FARSA_EXPERIMENTS_TEMPLATE Rendering2DDataFromGui
{
	/**
	 * \brief The current width of the area displaying the image
	 */
	int width;

	/**
	 * \brief The current height of the area displaying the image
	 */
	int height;

	/**
	 * \brief Whether the gui is visible or not
	 */
	bool visible;
};

/**
 * \brief A class exchanging data with the 2d rendering widget
 *
 * This class can be used to send QImages to display to the Render2DGui. It also
 * has methods to obtain information about the current status of the gui (see
 * the Rendering2DDataFromGui structure). To use this class you have to
 * associate it with the uploader/downloader of the GUI and then you can send
 * the image to render by calling the render2D() function and passing the image
 * to render. To avoid scaling of the image in the GUI you can query the current
 * width and height of the display area using the render2Dwidth() and
 * render2Dheight() functions. You can also check whether the GUI is visible
 * using the render2Dvisible() function. Currently there is no way of being
 * notified when the GUI becomes visible. If the image is not refreshed very
 * often, then, it may be advisable to always send it regardless of whether the
 * GUI is visible or not
 */
class FARSA_EXPERIMENTS_API Renderer2D : public DataUploaderDownloader<Rendering2DDataToGui, Rendering2DDataFromGui>
{
public:
	/**
	 * \brief Constructor
	 */
	Renderer2D();

	/**
	 * \brief Destructor
	 */
	~Renderer2D();

	/**
	 * \brief Returns the current width of the area displaying the image
	 *
	 * \return the current width of the area displaying the image
	 * \note This is not const because here we extract data from the
	 *       downloader
	 */
	int render2Dwidth();

	/**
	 * \brief Returns the current height of the area displaying the image
	 *
	 * \return the current height of the area displaying the image
	 * \note This is not const because here we extract data from the
	 *       downloader
	 */
	int render2Dheight();

	/**
	 * \brief Returns whether the GUI is currently visible or not
	 *
	 * \return whether the GUI is currently visible or not
	 * \note This is not const because here we extract data from the
	 *       downloader
	 */
	bool render2Dvisible();

	/**
	 * \brief Uploads an image to render
	 *
	 * \param image the image to render
	 */
	void render2D(QImage image);

private:
	void updateGuiInfo();

	int m_curWidth;
	int m_curHeight;
	bool m_curVisibility;
};

} //end namespace farsa

#endif

