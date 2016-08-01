/********************************************************************************
 *  SALSA GeneticAlgo UI                                                        *
 *  Copyright (C) 2012-2013                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#include "core/geneticalgoui.h"
#include "core/geneticalgo.h"
#include "logger.h"
#include "datastreamswidget.h"
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QMetaObject>

namespace salsa {

GeneticAlgoUI::GeneticAlgoUI( GeneticAlgo* component )
	: QObject(), ParameterSettableUI() {
	ga = component;
	ga->addObserver( this );
}

GeneticAlgoUI::~GeneticAlgoUI()
{
	// Nothing to do
	// --- All objects are destroyed in others parts because none of them are owend by this object
}

QList<ParameterSettableUIViewer> GeneticAlgoUI::getViewers( QWidget* parent, Qt::WindowFlags flags ) {
	QList<ParameterSettableUIViewer> viewsList;
	viewsList.append( fitnessView( parent, flags ) );
	return viewsList;
}

void GeneticAlgoUI::onEndGeneration() {
	QVector<double> bests = ga->bestFits().last();
	QVector<double> averages = ga->averageFits().last();
	
	if ( fitnessPlot->getNumPlots()<bests.size() ) {
		QMetaObject::invokeMethod(this, "fitPlotsToGenotype", Qt::BlockingQueuedConnection);
	}

	QVector<float> values;
	values.resize(2);
	for( int i=0; i<bests.size(); i++ ) {
		values[0] = bests[i];
		values[1] = averages[i];
		fitnessPlot->appendDatas( i, values );
	}
}

ParameterSettableUIViewer GeneticAlgoUI::fitnessView( QWidget* parent, Qt::WindowFlags flags ) {
	QWidget* widget = new QWidget( parent, flags );
	QGridLayout* lay = new QGridLayout( widget );

	QScrollArea* scroll = new QScrollArea( widget );
	lay->addWidget( scroll, 0, 0 );
	// creating the plot
	fitnessPlot = new DataStreamsWidget( scroll );

	scroll->setWidget( fitnessPlot );
	scroll->setWidgetResizable( true );
	return ParameterSettableUIViewer( widget, "Fitness Components over Generations" );
}

void GeneticAlgoUI::fitPlotsToGenotype() {
	QVector<double> bests = ga->bestFits().last();
	// adding datastream to plot if needed
	for( int i=fitnessPlot->getNumPlots(); i<bests.size(); i++ ) {
		int id = fitnessPlot->addPlot( QString("Component %1").arg(i) );
		DataStreamPlot* plot = fitnessPlot->getPlot(id);
		plot->addDataStream( "Max Fitness" );
		plot->addDataStream( "Average Fitness" );
		plot->setDataStreamColor( 0, Qt::red );
		plot->setDataStreamColor( 1, Qt::blue );
	}
}

} // end namespace salsa
