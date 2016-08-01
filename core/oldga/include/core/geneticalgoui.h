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

#ifndef GENETICALGOUI_H
#define GENETICALGOUI_H

#include "parametersettableui.h"
#include "geneticalgo.h"

namespace salsa {

class DataStreamsWidget;

class GeneticAlgoUI : public QObject, public salsa::GeneticAlgoObserver, public salsa::ParameterSettableUI {
	Q_OBJECT
public:
	/*! \brief Construct the GUI manager for GeneticAlgo
	 *  \warning This only works with GeneticAlgo hierarchy
	 */
	GeneticAlgoUI( GeneticAlgo* component );
	/*! Destructor */
	~GeneticAlgoUI();
	/*! Return the list of all viewers for the Component with corresponding
	 *  informations for fill the menu "Views" of Total99
	 *  \param parent is the parent widget for all viewers created by ParameterSettableUI
	 *  \param flags are the necessary flags to specify when constructing the viewers
	 */
	QList<salsa::ParameterSettableUIViewer> getViewers( QWidget* parent, Qt::WindowFlags flags );
	/*! called after a step of the neural network */
	void onEndGeneration();
private slots:
	/*! update the plots to fits the number of components of the genotype */
	void fitPlotsToGenotype();
private:
	salsa::ParameterSettableUIViewer fitnessView( QWidget* parent, Qt::WindowFlags flags );

	GeneticAlgo* ga;
	DataStreamsWidget* fitnessPlot;
};

} // end namespace salsa

#endif
