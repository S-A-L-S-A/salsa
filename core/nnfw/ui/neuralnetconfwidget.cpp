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

#include "neuralnetconfwidget.h"
#include "typesdb.h"
#include <QLabel>
#include <QGridLayout>
#include <QGraphicsView>
#include <QTextStream>
#include <QFile>
#include "qgv/QGVScene.h"
#include <QMap>

namespace salsa {

NeuralNetConfWidget::NeuralNetConfWidget(ConfigurationManager& params, QString prefix, QWidget* parent, Qt::WindowFlags f) :
	ConfigurationWidget( params, prefix, parent, f )
{
	QGridLayout* mainLay = new QGridLayout( this );
	
	view = new QGraphicsView( this );
	view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	mainLay->addWidget( view, 0, 0 );

	scene = new QGVScene("NNFW Viewer", this);
	view->setScene( scene );
	
	createQGVNetwork();
}

NeuralNetConfWidget::~NeuralNetConfWidget() {
	/* nothing to do */
}

void NeuralNetConfWidget::createQGVNetwork() {
	// get all objects in the net: Clusters and Linkers
	QStringList allObjects;
	foreach( QString sub, m_params.getGroupsList( m_prefix ) ) {
		QString path = m_prefix+sub;
		if ( path.startsWith( ConfigurationManager::GroupSeparator() ) ) {
			path.remove( 0, 1 );
		}
		if ( path.endsWith( ConfigurationManager::GroupSeparator() ) ) {
			path.chop( 1 );
		}
		// append the full path
		allObjects << path;
	}
	//--- clustersList and linkersList are supposed to be full path
	QString str = m_params.getValue(m_prefix + "clustersList");
	if (!str.isEmpty()) {
		allObjects << str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	}
	str = m_params.getValue(m_prefix + "linkersList");
	if (!str.isEmpty()) {
		allObjects << str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	}
	// get only the input clusters
	QStringList inputs;
	str = m_params.getValue(m_prefix + "inputClusters");
	if (!str.isEmpty()) {
		inputs << str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	}
	// get only the output clusters
	QStringList outputs;
	str = m_params.getValue(m_prefix + "outputClusters");
	if (!str.isEmpty()) {
		outputs << str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	}

	// set up the graph and create the subgroups
	//Configure scene attributes
	scene->setGraphAttribute("rankdir", "BT");
	scene->setNodeAttribute("shape", "box");
	scene->setNodeAttribute("style", "filled");
	scene->setNodeAttribute("fillcolor", "white");
	scene->setEdgeAttribute("minlen", "2" );
	QGVSubGraph *sensors = scene->addSubGraph("Sensors");
	sensors->setAttribute("rank", "same");
	QGVSubGraph *hiddens = scene->addSubGraph("Hiddens");
	hiddens->setAttribute("rank", "same");
	QGVSubGraph *motors = scene->addSubGraph("Motors");
	motors->setAttribute("rank", "same");

	// parse and create the graph
	QStringList allClusterTypes = TypesDB::getInstance().getAllComponentSubclasses( "Cluster" );
	QStringList allLinkerTypes = TypesDB::getInstance().getAllComponentSubclasses( "Linker" );
	// first pass we add all clusters
	QMap<QString, QGVNode*> clusterNodes;
	foreach( QString sub, allObjects ) {
		// avoid duplicates
		if ( clusterNodes.contains(sub) ) continue;
		QString type = m_params.getValue("/"+sub+"/type");
		if ( !allClusterTypes.contains(type) ) continue;
		// add the node to one of the subgraphs
		if ( inputs.contains(sub) ) {
			clusterNodes[sub] = sensors->addNode( sub );
		} else if ( outputs.contains(sub) ) {
			clusterNodes[sub] = motors->addNode( sub );
		} else {
			clusterNodes[sub] = hiddens->addNode( sub );
		}
	}
	// second pass we add the linkers
	foreach( QString sub, allObjects ) {
		QString type = m_params.getValue("/"+sub+"/type");
		if ( !allLinkerTypes.contains(type) ) continue;
		QString from = m_params.getValue("/"+sub+"/from");
		QString to = m_params.getValue("/"+sub+"/to");
		scene->addEdge( clusterNodes[from], clusterNodes[to] );
	}
	// Layout scene
	scene->applyLayout();
	// Resize the view
	view->setFixedSize( scene->sceneRect().width()+20, scene->sceneRect().height()+20 );
}

} // end namespace salsa
