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

#include "neuralnetui.h"
#include "neuralnet.h"
#include "logger.h"
#include "cluster.h"
#include "linker.h"
#include "qgv/QGVScene.h"
#include "datastreamswidget.h"
#include <QLabel>
#include <QGridLayout>
#include <QBoxLayout>
#include <QGraphicsView>
#include <QScrollArea>

namespace farsa {

NeuralNetUI::NeuralNetUI( NeuralNet* component )
	: QObject(), ComponentUI() {
	net = component;
	net->addObserver( this );
}

NeuralNetUI::~NeuralNetUI()
{
	// Nothing to do
	// --- All objects are destroyed in others parts because none of them are owend by this object
}

QList<ComponentUIViewer> NeuralNetUI::getViewers( QWidget* parent, Qt::WindowFlags flags ) {
	QList<ComponentUIViewer> viewsList;
	viewsList.append( topologyView( parent, flags ) );
	viewsList.append( activationsView( parent, flags ) );
	return viewsList;
}

void NeuralNetUI::onStepDone() {
	foreach( Cluster* cl, net->clusters() ) {
		DataStreamsWidget* plot = plotsMap[cl];
		QVector<float> values;
		values.resize(2);
		for( int i=0; i<cl->numNeurons(); i++ ) {
			values[0] = cl->getInput(i);
			values[1] = cl->getOutput(i);
			plot->appendDatas( i, values );
		}
	}
}

ComponentUIViewer NeuralNetUI::topologyView( QWidget* parent, Qt::WindowFlags flags ) {
	QWidget* widget = new QWidget( parent, flags );
	QGridLayout* lay = new QGridLayout( widget );

	view = new QGraphicsView( widget );
	view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	lay->addWidget( view, 0, 0 );

	scene = new QGVScene("NNFW Viewer", widget);
	view->setScene( scene );
	
	createQGVNetwork();

	return ComponentUIViewer( widget, "Neural Network Topology" );
}

ComponentUIViewer NeuralNetUI::activationsView( QWidget* parent, Qt::WindowFlags flags ) {
	QWidget* widget = new QWidget( parent, flags );
	QGridLayout* lay = new QGridLayout( widget );

	QScrollArea* scroll = new QScrollArea( widget );
	lay->addWidget( scroll, 0, 0 );
	QWidget* plots = new QWidget( scroll );
	QVBoxLayout* plotLay = new QVBoxLayout( plots );
	scroll->setWidget( plots );
	scroll->setWidgetResizable( true );
	// creating the plots
	plotsMap.clear();
	foreach( Cluster* cl, net->clusters() ) {
		DataStreamsWidget* plot = new DataStreamsWidget( plots );
		plotLay->addWidget( plot );
		plotsMap[cl] = plot;
		for( int i=0; i<cl->numNeurons(); i++ ) {
			int id = plot->addPlot( QString( "%1@%2" ).arg(cl->name()).arg(i) );
			DataStreamPlot* nplot = plot->getPlot(id);
			nplot->addDataStream( "net input" );
			nplot->addDataStream( "output" );
			nplot->setDataStreamColor( 0, Qt::red );
			nplot->setDataStreamColor( 1, Qt::blue );
			nplot->enableAxis( QwtPlot::xBottom, false );
		}
	}

	return ComponentUIViewer( widget, "Neural Network Activations" );
}

void NeuralNetUI::createQGVNetwork() {
	// set up the graph and create the subgroups
	// Configure scene attributes
	scene->setGraphAttribute("rankdir", "BT");
	scene->setNodeAttribute("shape", "box");
	scene->setNodeAttribute("style", "filled");
	scene->setNodeAttribute("fillcolor", "white");
	scene->setEdgeAttribute("minlen", "2" );
	QGVSubGraph *sensors = scene->addSubGraph("Sensors");
	sensors->setAttribute("rank", "same");
	QGVSubGraph *hiddens = scene->addSubGraph("Hiddens");
	//hiddens->setAttribute("rank", "same");
	QGVSubGraph *motors = scene->addSubGraph("Motors");
	motors->setAttribute("rank", "same");

	// parse and create the graph
	// first pass we add all clusters
	QMap<Cluster*, QGVNode*> clusterNodes;
	foreach( Cluster* cl, net->inputClusters() ) {
		clusterNodes[cl] = sensors->addNode( cl->name() );
	}
	foreach( Cluster* cl, net->outputClusters() ) {
		clusterNodes[cl] = motors->addNode( cl->name() );
	}
	foreach( Cluster* cl, net->hiddenClusters() ) {
		clusterNodes[cl] = hiddens->addNode( cl->name() );
	}
	// second pass we add the linkers
	foreach( Linker* lnk, net->linkers() ) {
		scene->addEdge( clusterNodes[lnk->from()], clusterNodes[lnk->to()] );
	}
	// Layout scene
	scene->applyLayout();
	// Resize the view
	view->setFixedSize( scene->sceneRect().width()+20, scene->sceneRect().height()+20 );
}

} // end namespace farsa
