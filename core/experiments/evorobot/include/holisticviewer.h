// /********************************************************************************
//  *  FARSA Experiments Library                                                   *
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
// #ifndef HOLISTICVIEWER_H
// #define HOLISTICVIEWER_H
//
// #include <QWidget>
// #include <QGridLayout>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QComboBox>
// #include <QLabel>
// #include <QPaintEvent>
// #include <QVector>
// #include "evonet.h"
// #include "evodataviewer.h"
// #include "evorobotexperiment.h"
//
// namespace farsa {
//
// class FARSA_EXPERIMENTS_API HolisticViewer : public QWidget
// {
//     Q_OBJECT
// private:
// 	QVector<QLabel*> labels;
// 	Evonet* net;
//     QGridLayout* grid;
// 	QVBoxLayout* mainLayout;
// 	QHBoxLayout* neuronChoice;
// 	QComboBox *neuronX, *neuronY;
// 	int getNeuronAct(int);
// protected:
// 	void paintEvent(QPaintEvent* qpe);
// public:
// 	HolisticViewer(Evonet*, QWidget* parent=0, Qt::WindowFlags flags=0);
// 	void updateGrid();
// 	void updatePlot();
// 	void setNet(Evonet* n);
// };
//
// } //end namespace farsa
//
// #endif
