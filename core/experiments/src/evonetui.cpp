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

#include "evonetui.h"
#include "displaycontroller.h"
#include "evodataviewer.h"
#include "holisticviewer.h"
#include "total99resources.h"
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QEvent>
#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>

// All the suff below is to avoid warnings on Windows about the use of unsafe
// functions. This should be only a temporary workaround, the solution is stop
// using C string and file functions...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

EvonetUI::EvonetUI(Evonet* net, DataUploader<ActivationsToGui>* neuronsMonitorUploader)
	: QObject()
	, ComponentUI()
	, edv(nullptr)
// 	, hlv(nullptr)
// 	, networkDialog(nullptr)
	, evonet(net)
	, neuronsMonitorDownloader( DataDownloader<ActivationsToGui>::NoNotification )
	, nNeurons(0)
{
	// Associating uploaders and downloaders
	GlobalUploaderDownloader::attach(neuronsMonitorUploader, &neuronsMonitorDownloader);

	// Global connections
	// !! DO NOT CONNECT TO THE evonetUpdated SIGNAL to update the network becuase that signals may be
	//    emitted so fast that the GUI will freeze !!
	//connect( evonet, SIGNAL(evonetUpdated()), this, SLOT(onEvonetUpdated()), Qt::QueuedConnection );
	QTimer* timer = new QTimer(this);
	timer->setInterval( 40 );
	timer->setSingleShot( false );
	timer->start();
	connect( timer, SIGNAL(timeout()), this, SLOT(onEvonetUpdated()) );
}

EvonetUI::~EvonetUI()
{
	// Nothing to do
	// --- All objects are destroyed in others parts because none of them are owend by this object
}

QList<ComponentUIViewer> EvonetUI::getViewers( QWidget* parent, Qt::WindowFlags flags ) {
	QList<ComponentUIViewer> viewsList;
// 	viewsList.append( networkView( parent, flags ) );
	viewsList.append( neuroMonitorView( parent, flags ) );
// 	viewsList.append( holisticView( parent, flags ) );
	return viewsList;
}

void EvonetUI::changeNet(Evonet* n)
{
	GlobalUploaderDownloader::detach(&neuronsMonitorDownloader);
	GlobalUploaderDownloader::attach(n->getNeuronsMonitorUploader(), &neuronsMonitorDownloader);

// 	networkDialog->setNet(n);
// 	hlv->setNet(n);

	evonet = n;
}

// ComponentUIViewer EvonetUI::networkView( QWidget* parent, Qt::WindowFlags flags )
// {
// 	networkDialog = new NetworkDialog(evonet,parent,flags);
// 	networkDialog->pseudo_activate_net();
// 	networkDialog->setWindowTitle( "Neural Network Editor" );
// 	return ComponentUIViewer( networkDialog, "Nervous System" );
// }

ComponentUIViewer EvonetUI::neuroMonitorView( QWidget* parent, Qt::WindowFlags flags )
{
	int addInfo = 0; // (evonet->showTeachingInput() ? evonet->getNoOutputs() + 1 : 0);
	nNeurons = evonet->getNoNeurons();
	edv = new EvoDataViewer( evonet->getNoNeurons() + addInfo, 1000, 0, parent, flags );

	//setting chunk properties
	int i;
	bool dn;
	for (i = 0; i < evonet->getNoNeurons(); i++) {
		if (evonet->neurondisplay[i] == 1) {
			dn = true;
		} else {
			dn = false;
		}
		if ( evonet->neurondcolor[i].isValid() ) {
			edv->setChunkProperties(i, evonet->neuronrange[i][0], evonet->neuronrange[i][1], evonet->neuronl[i], evonet->neurondcolor[i], dn);
		} else {
			// if the color is not valid, will use the color red
			edv->setChunkProperties(i, evonet->neuronrange[i][0], evonet->neuronrange[i][1], evonet->neuronl[i], QColor(255,0,0), dn);
		}
	}
	// Setting chunk properties for teaching input and backpropagation error (if the teaching input must be shown)
	dn = true;

	//
	edv->setWindowTitle( "Neurons Monitor" );
	edv->setGeometry(50, 50, 600, 600);
	return ComponentUIViewer( edv, "Neurons Monitor" );
}

// ComponentUIViewer EvonetUI::holisticView( QWidget* parent, Qt::WindowFlags flags )
// {
// 	hlv = new HolisticViewer(evonet, parent, flags);
// 	hlv->resize(300, 300);
// 	hlv->setWindowTitle("Holistic Viewer");
// 	return ParameterSettableUIViewer( hlv, "Holistic View" );
// }

void EvonetUI::onEvonetUpdated() {
	if ( edv ) {
		QElapsedTimer timer;
		timer.start();
		while(true) {
			// This call returns nullptr if no new activation is available
			const ActivationsToGui* d = neuronsMonitorDownloader.downloadDatum();

			if (d == nullptr) {
				break;
			}

			if (d->activations) {
				for (int ch = 0; ch < d->data.size(); ch++) {
					edv->setChunkValue(ch, d->data[ch]);
				}

				// Also setting the current step
				edv->setCurrentStep(d->updatesCounter);
			} else {
				// Setting teaching inputs
				for (int ch = 0; ch < (d->data.size() - 1); ch++) {
					edv->setChunkValue(nNeurons + ch, d->data[ch]);
				}
				// The error is the last value in d->data
				edv->setChunkValue(nNeurons + d->data.size() - 1, d->data.last());


				// Also setting the current step
				edv->setCurrentStep(d->updatesCounter);
			}

			if (d->updateLabelAndColors) {
				for (int i = 0; i < d->neuronl.size(); ++i) {
					if (d->neurondcolor[i].isValid()) {
						edv->setChunkColor(i, d->neurondcolor[i]);
					} else {
						edv->setChunkColor(i, QColor(255,0,0));
					}
					edv->setChunkLabel(i, d->neuronl[i]);
				}
			}

			// break the cycle if it takes too long to get data
			if ( timer.elapsed() > 20 ) {
				break;
			}
		}
		edv->update();
	}

	// updating the Newtork Dialog
// 	if ( networkDialog ) {
// 		networkDialog->update();
// 	}
	// updating holistic viewer
// 	if ( hlv ) {
// 		hlv->updateGrid();
// 		hlv->updatePlot();
// 		hlv->update();
// 	}
}

} //end namespace salsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
