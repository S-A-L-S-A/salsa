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

#ifndef NEURALNET_CONFIGURATION_WIDGET_H
#define NEURALNET_CONFIGURATION_WIDGET_H

#include "nnfwconfig.h"
#include "configurationwidget.h"

class QGraphicsView;
class QGVScene;

namespace farsa {

/**
 * \brief the graphical widget for configuring a neural network
 *
 */
class NeuralNetConfWidget : public ConfigurationWidget
{
	Q_OBJECT
public:
	/**
	 * \brief Constructor
	 *
	 * \param params the ConfigurationParameters instance with the
	 *               parameters we have to edit
	 * \param prefix the group whose parameters we edit
	 * \param parent the parent widget
	 * \param f the window flags for this widget
	 */
	NeuralNetConfWidget(farsa::ConfigurationManager& params, QString prefix, QWidget* parent = NULL, Qt::WindowFlags f = 0);

	/**
	 * \brief Destructor
	 */
	virtual ~NeuralNetConfWidget();

	/**
	 * \brief Returns trues because it configure the whole neural network
	 */
	virtual bool configuringSubgroups() const { return true; };

signals:
	/**
	 * \brief The signal emitted when parameters are changed
	 */
	void parametersChanged();
private slots:
	/*! read the actual configuration of the NNFW and convert it into
	 *  a QGV representation in order to render it graphically */
	void createQGVNetwork();
private:
	/*! the graphics view where the neural network is rendered */
	QGraphicsView* view;
	/*! the scene containing the neural network graphics items */
	QGVScene* scene;
};

} // end namespace farsa

#endif
