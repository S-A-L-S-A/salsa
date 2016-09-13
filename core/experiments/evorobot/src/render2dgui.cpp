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

#include "render2dgui.h"
#include "logger.h"
#include <QPixmap>

namespace salsa {

Render2DGui::Render2DGui(QWidget *parent, Qt::WindowFlags f)
	: QLabel(parent, f)
	, DataUploaderDownloader<Rendering2DDataFromGui, Rendering2DDataToGui>(1, OverrideOlder, this)
	, m_timerID(0)
{
	setScaledContents(true);
	setWindowTitle("2D Renderer");
	resize(100, 100);
	// This is needed to be able to grow as well as shrink the window
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	m_timerID = startTimer(40);
	if (m_timerID == 0) {
		Logger::warning("Render2DGui: cannot start timer");
	}
}

Render2DGui::~Render2DGui()
{
	killTimer(m_timerID);
}

void Render2DGui::resizeEvent(QResizeEvent *event)
{
	QLabel::resizeEvent(event);

	sendInfo();
}

void Render2DGui::showEvent(QShowEvent *event)
{
	QLabel::showEvent(event);

	sendInfo();
}

void Render2DGui::sendInfo()
{
	DatumToUpload d(*this);

	d->width = width();
	d->height = height();
	d->visible = isVisible();
}

void Render2DGui::timerEvent(QTimerEvent *)
{
	const Rendering2DDataToGui* d = downloadDatum();
	if (d == nullptr) {
		return;
	}

	setPixmap(QPixmap::fromImage(d->image));
}

} // end namespace salsa

