// /********************************************************************************
//  *  SALSA Experiments Library                                                   *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #include "holisticviewer.h"
//
// namespace salsa {
//
// HolisticViewer::HolisticViewer(Evonet* network, QWidget* parent, Qt::WindowFlags flags)
// 	: QWidget(parent, flags),
// 	  labels() {
// 	net = network;
//
// 	mainLayout = new QVBoxLayout(this);
//
// 	grid = new QGridLayout();
// 	grid->setSpacing(0);
// 	mainLayout->addLayout(grid);
//
// 	neuronChoice = new QHBoxLayout();
// 	neuronX = new QComboBox(this);
// 	neuronY = new QComboBox(this);
// 	neuronChoice->addWidget(neuronX);
// 	neuronChoice->addWidget(neuronY);
// 	mainLayout->addLayout(neuronChoice);
//
// }
//
// //return neuron activation scaled from 0 to 255
// int HolisticViewer::getNeuronAct(int n)
// {
// 	double act = net->getNeuron(n);
// 	double min = net->neuronrange[n][0];
// 	double max = net->neuronrange[n][1];
//
// 	act = linearMap(act,min,max,0,255);
//
// 	// Clamping between 0 and 255
// 	int intAct = (int)ceil(act);
// 	intAct = (intAct < 0) ? 0 : ((intAct > 255) ? 255 : intAct);
// 	return intAct;
// }
//
// void HolisticViewer::updateGrid()
// {
// 	int neuronsPerRow = ceil(sqrt((double)net->getNoNeurons()));
//
// 	//first iteration only
// 	if(labels.size() == 0)
// 	{
// 		QFont font;
// 		QString name, color;
//
// 		for(int r=0; r<neuronsPerRow; r++)
// 		{
// 			for(int c=0; c<neuronsPerRow; c++)
// 			{
// 				int index = r*neuronsPerRow+c;
//
// 				if(index < net->getNoNeurons())
// 				{
// 					name = net->neuronl[index];
//
// 					//add items to the combo boxes
// 					neuronX->addItem(name);
// 					neuronY->addItem(name);
//
// 					//insert labels into the grid
// 					color = "yellow";
// 					if(index < net->getNoInputs())
// 						color = "red";
// 					if(index > net->getNoInputs()+net->getNoHiddens())
// 						color = "blue";
//
// 					labels.append( new QLabel(this) );
// 					labels[index]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
// 					labels[index]->setFont(font);
// 					labels[index]->setText("<font color='"+ color + "';>" + name + "</font>");
//
// 					grid->addWidget(labels[index], r, c);
// 				}
// 			}
// 		}
// 	}
//
// 	//every step
//     for(int r=0; r<neuronsPerRow; r++)
//     {
//         for(int c=0; c<neuronsPerRow; c++)
//         {
// 			int index = r*neuronsPerRow+c;
//             if(index < net->getNoNeurons())
//             {
//                 labels[index]->setAutoFillBackground(true);
//                 int act256 = getNeuronAct(index);
//                 labels[index]->setPalette(QPalette(QColor(act256,act256,act256, 255)));
//
// 				int fontSize = (int)(width()+height())/70;
// 				if(fontSize > 30)
// 					fontSize = 30;
// 				QFont font = labels[index]->font();
// 				font.setPointSize(fontSize);
// 				labels[index]->setFont(font);
//             }
//         }
//     }
// }
//
// void HolisticViewer::updatePlot()
// {
// 	//take selected items from combos
// 	//load their buffer
// 	//plot everything
// }
//
// void HolisticViewer::paintEvent(QPaintEvent* /*evt*/)
// {
// 	QPainter painter(this);
//
// 	QPen blackPen(Qt::black);
// 	QPen bluePen(Qt::blue);
// 	QPen greenPen(Qt::green);
// 	QPen redPen(Qt::red);
//
// 	painter.drawRect(0,0,100,100);
//
// 	painter.fillRect(0,0,width(),height(),Qt::white);
// 	painter.setPen(blackPen);
// 	painter.setRenderHint(QPainter::Antialiasing, false);
// }
//
// void HolisticViewer::setNet(Evonet* n)
// {
// 	net = n;
// 	updateGrid();
// }
//
// } //end namespace salsa
