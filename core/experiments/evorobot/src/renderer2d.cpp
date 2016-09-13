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

#include "renderer2d.h"

namespace salsa {

Renderer2D::Renderer2D()
	: DataUploaderDownloader<Rendering2DDataToGui, Rendering2DDataFromGui>(1, OverrideOlder, NoNotification)
	, m_curWidth(0)
	, m_curHeight(0)
	, m_curVisibility(false)
{
}

Renderer2D::~Renderer2D()
{
}

int Renderer2D::render2Dwidth()
{
	updateGuiInfo();

	return m_curWidth;
}

int Renderer2D::render2Dheight()
{
	updateGuiInfo();

	return m_curHeight;
}

bool Renderer2D::render2Dvisible()
{
	updateGuiInfo();

	return m_curVisibility;
}

void Renderer2D::render2D(QImage image)
{
	DatumToUpload d(*this);

	d->image = image;
}

void Renderer2D::updateGuiInfo()
{
	while (true) {
		const Rendering2DDataFromGui* d = downloadDatum();

		if (d == nullptr) {
			break;
		}

		m_curWidth = d->width;
		m_curHeight = d->height;
		m_curVisibility = d->visible;
	}
}

} // end namespace salsa

