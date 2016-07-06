/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2014 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef NEURALNETUI_H
#define NEURALNETUI_H

#include "componentui.h"
#include "neuralnet.h"
#include <QMap>

class QGraphicsView;
class QGVScene;

namespace farsa {

class DataStreamsWidget;
class Cluster;

class NeuralNetUI : public QObject, public farsa::NeuralNetObserver, public farsa::ComponentUI {
	Q_OBJECT
public:
	/*! \brief Construct the GUI manager for NeuralNet
	 *  \warning This only works with NeuralNet
	 */
	NeuralNetUI( NeuralNet* component );
	/*! Destructor */
	~NeuralNetUI();
	/*! Return the list of all viewers for the Component with corresponding
	 *  informations for fill the menu "Views" of Total99
	 *  \param parent is the parent widget for all viewers created by ParameterSettableUI
	 *  \param flags are the necessary flags to specify when constructing the viewers
	 */
	QList<farsa::ComponentUIViewer> getViewers( QWidget* parent, Qt::WindowFlags flags );
	/*! called after a step of the neural network */
	void onStepDone();
private:
	farsa::ComponentUIViewer topologyView( QWidget* parent, Qt::WindowFlags flags );
	farsa::ComponentUIViewer activationsView( QWidget* parent, Qt::WindowFlags flags );

	/*! convert the net into
	 *  a QGV representation in order to render it graphically */
	void createQGVNetwork();

	NeuralNet* net;
	/*! the graphics view where the neural network is rendered */
	QGraphicsView* view;
	/*! the scene containing the neural network graphics items */
	QGVScene* scene;
	/*! Cluster to plot widget mapping */
	QMap<Cluster*, DataStreamsWidget*> plotsMap;
};

} // end namespace farsa

#endif
