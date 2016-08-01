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

#include "renderworldwrapperwidget.h"
#include "renderworld.h"
#include "logger.h"
#include <QVBoxLayout>

using namespace qglviewer;

// All the suff below is to avoid warnings on Windows about the use of unsafe
// functions. This should be only a temporary workaround, the solution is stop
// using C string and file functions...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

#warning THE HARDCODED 40 BELOW SHOULD BE CHECKED
RenderWorldWrapperWidget::RenderWorldWrapperWidget(WorldDataUploadeDownloaderSimSide* otherEnd, QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, m_dataExchanger(new GUIRenderersContainerDataExchange(40, otherEnd, this))
	, m_renderWorld(new RenderWorld(m_dataExchanger, this))
	, m_layout(new QVBoxLayout(this))
	, m_setCameraToLookAtRobot(true)
	, m_robotTm(wMatrix::identity())
{
	// Setting some window property and creating the layout that will contain renderworld
	setWindowTitle("3D World");
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->addWidget(m_renderWorld);
	const QString stateFileName = ".evolver.xml";
	m_renderWorld->setStateFileName(stateFileName);
}

RenderWorldWrapperWidget::~RenderWorldWrapperWidget()
{
}

void RenderWorldWrapperWidget::lookAtRobot()
{
	wVector cameraDefaultPosition = m_robotTm.transformVector(wVector(-0.8f, 0.0f, +0.6f));
	m_renderWorld->camera()->setPosition(Vec(cameraDefaultPosition[0], cameraDefaultPosition[1], cameraDefaultPosition[2]));
	m_renderWorld->camera()->setUpVector(Vec(0.0f, 0.0f, 1.0f));
	m_renderWorld->camera()->lookAt(Vec(m_robotTm.w_pos[0], m_robotTm.w_pos[1], m_robotTm.w_pos[2]));
}

} //end namespace salsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
