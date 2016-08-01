/*******************************************************************************
 * SALSA New Genetic Algorithm Library                                        *
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

#ifndef ROBOTEXPERIMENTUI_H
#define ROBOTEXPERIMENTUI_H

#include "robotexperiment.h"
#include "parametersettableui.h"
#include "resourcesuser.h"
#include "renderworldwrapperwidget.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QLabel>
#include <QList>
#include <QString>
#include <QWidget>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QObject>

namespace salsa {

class SALSA_NEWGA_API RobotExperimentUI : public QObject, public ParameterSettableUI {
	Q_OBJECT
public:
	/*! \brief Construct the GUI manager for Evonet
	 *  \warning This only works with Evonet
	 *
	 * \param e the experiment to which this is associates
	 */
	RobotExperimentUI(RobotExperiment* e);
	/*! Destructor */
	~RobotExperimentUI();
	/*! Return the list of all viewers for the Component with corresponding
	 *  informations for fill the menu "Views" of Total99
	 *  \param parent is the parent widget for all viewers created by ParameterSettableUI
	 *  \param flags are the necessary flags to specify when constructing the viewers
	 */
	QList<ParameterSettableUIViewer> getViewers( QWidget* parent, Qt::WindowFlags flags );
private slots:
	void updateRenderWorld();
private:
	ParameterSettableUIViewer renderWorldView( QWidget* parent, Qt::WindowFlags flags );

	RenderWorldWrapperWidget *renderworld;
	//! the experiment
	RobotExperiment* exp;
};

} //end namespace salsa

#endif
