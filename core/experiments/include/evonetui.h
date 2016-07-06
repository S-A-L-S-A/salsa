/********************************************************************************
 *  FARSA Experiments Library                                                   *
 *  Copyright (C) 2007-2013                                                     *
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

#ifndef EVONETUI_H
#define EVONETUI_H

#include "evonet.h"
#include "componentui.h"

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

namespace farsa {

class EvoDataViewer;
class NetworkDialog;
class HolisticViewer;

class FARSA_EXPERIMENTS_API EvonetUI : public QObject, public ComponentUI
{
	Q_OBJECT

public:
	/*! \brief Construct the GUI manager for Evonet
	 *  \warning This only works with Evonet
	 *
	 * \param evonet the evonet to which this is associates
	 * \param neuronsMonitorUploader the uploader for data to send to the neurons monitor
	 */
	EvonetUI( Evonet* net, DataUploader<ActivationsToGui>* neuronsMonitorUploader);

	/*!
	 * \brief Destructor
	 */
	~EvonetUI();

	/*! Return the list of all viewers for the Component with corresponding
	 *  informations for fill the menu "Views" of Total99
	 *  \param parent is the parent widget for all viewers created by ParameterSettableUI
	 *  \param flags are the necessary flags to specify when constructing the viewers
	 */
	QList<ComponentUIViewer> getViewers( QWidget* parent, Qt::WindowFlags flags );

	/**
	 * \brief Changes the net to the given one
	 *
	 * \param n the new net
	 */
	void changeNet(Evonet* n);

private:
// 	ComponentUIViewer networkView( QWidget* parent, Qt::WindowFlags flags );
	ComponentUIViewer neuroMonitorView( QWidget* parent, Qt::WindowFlags flags );
// 	ComponentUIViewer holisticView( QWidget* parent, Qt::WindowFlags flags );

	//! neural controller monitor class
	EvoDataViewer* edv;
	//! additional neural dynamics viewer
// 	HolisticViewer* hlv;
	// The widget for display/manipulate the neural network
// 	NetworkDialog *networkDialog;
	//! the neural network that this ui manages
	Evonet* evonet;
	//! The downloader for data to send to the neurons monitor
	DataDownloader<ActivationsToGui> neuronsMonitorDownloader;
	//! The total number of neurons (necessary to access the correct chunk in edv)
	int nNeurons;

private slots:
	/*! this update the graphics when the Evonet has been updated
	 */
	void onEvonetUpdated();
};

} //end namespace farsa

#endif

