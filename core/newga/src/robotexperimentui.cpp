/*******************************************************************************
 * FARSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#include "robotexperimentui.h"
#include "configurationhelper.h"

namespace farsa {

RobotExperimentUI::RobotExperimentUI( RobotExperiment* e )
	: ParameterSettableUI()
	, renderworld(NULL)
	, exp(e)
{
	QTimer* timer = new QTimer(this);
	timer->setInterval( 40 );
	timer->setSingleShot( false );
	timer->start();
	connect( timer, SIGNAL(timeout()), this, SLOT(updateRenderWorld()) );
}

RobotExperimentUI::~RobotExperimentUI()
{
	// Nothing to do
	// --- All objects are destroyed in others parts because none of them are owend by this object
}

QList<ParameterSettableUIViewer> RobotExperimentUI::getViewers( QWidget* parent, Qt::WindowFlags flags ) {
	QList<ParameterSettableUIViewer> viewsList;
	viewsList.append( renderWorldView( parent, flags ) );
	return viewsList;
}

ParameterSettableUIViewer RobotExperimentUI::renderWorldView( QWidget* parent, Qt::WindowFlags flags )
{
	// Creating the wrapper class for RenderWorld and sharing resources
	renderworld = new RenderWorldWrapperWidget(parent, flags);
	renderworld->shareResourcesWith(exp);

	return ParameterSettableUIViewer(renderworld, "RenderWorld");
}

void RobotExperimentUI::updateRenderWorld()
{
	if (renderworld != NULL)
	{
		renderworld->updateRenderWorld();
	}
}

} //end namespace farsa
