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

#ifndef EVODATAVIEWER_H
#define EVODATAVIEWER_H

#include "experimentsconfig.h"
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QFile>
#include <QTextStream>

namespace salsa {


//DataChunk class
class SALSA_EXPERIMENTS_API DataChunk
{




private:
	QColor  color;
	QString label;
	int size; //actual size of the array
	double dpratio;// dat pixel ratio data size/pixels
	bool visible;
	int index;
	double min, max;//range in case we need a normalization
	double maxValue;


	int style; //different visualization style 0: full line, 1: edge
	double* data; //to be sized on size value;
	double zeroValue;
	void checkMaxValue(double val);


public:
	DataChunk(const QString& label, const QColor& color, int size, bool visible);
	~DataChunk();
	void setColor(QColor color);
	void setData(double value);
	void setDataRaw(double value); //not ranged
	void setDataRaw(int ind, double value); //not ranged


	void setRange(double min, double max);
	void setLabel(const QString& label);
	void setStyle(int style);
	void setVisible(bool vis);
	bool isVisible();
	QString getLabel() const;
	QColor& getColor();

	double getValueToDraw();
	double getValue(int ind);
	int getIndex();
	double linearMap( double x, double rmin, double rmax,
                double outMin = 0, double outMax = 1 );


	double getRangeMin();
	double getRangeMax();
	double getMaxValue();
	double getZeroValue();
	void setDPRatio(double val);
	double getDPRatio();
	//loads space separated columns from a txt file
	bool loadRawData(const QString& filename, int column);




};
//listwindow triggered by right mouse click ---------------------------------------------------------------------------------------
class SALSA_EXPERIMENTS_API EvoListViewer : public QWidget
{
	Q_OBJECT
public:
	EvoListViewer(DataChunk** dataChunks, int n, bool *nviewChange , QWidget *parent = 0);
	~EvoListViewer();
	private slots:
		void okSelected();
	public slots:
		void restoreSelected();
		void allOnOrAllOff();


private:
	int nchunks;
	DataChunk** dataChunks;
	QListWidget *listwidget;
	QGridLayout *layout;
	QPushButton* deselectAll;
	bool *nvchange;
	//EvoDataViewr *edv;
	//void resizeEvent(QResizeEvent* evt);



};


// EvodataViewer ----------------------------------------------------------------------------------------------------------------

class SALSA_EXPERIMENTS_API EvoDataViewer : public QWidget
{
	Q_OBJECT
public:
	//EvoDataViewer();
	EvoDataViewer(int nchunks, int chunksize, int style=0, QWidget* parent=0, Qt::WindowFlags flags=0);
	~EvoDataViewer();

	void setChunkLabel(int ch, const QString& label);
	void setChunkValue(int ch, double value);              //setting value by id
	bool setChunkValue(const QString& name, double value); //setting value by chunk name
	void setChunkColor(int ch, QColor color);
	void setCurrentStep(int step);
	void setStyle(int style);
	void setChunkRange(int ch, double mn, double mx);
	void setChunkProperties(int ch, double rangeMin, double rangeMax, const QString& label, QColor color, bool visible);
	int visibleChunks();
	void listVisibleChunks();
	void reset();
private slots:
	void pickUnvisible();
protected:
	void paintEvent(QPaintEvent* evt);
	void resizeEvent(QResizeEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
private:
	void updateGraphic(int ch);//draws only latest data on the pixmap. ch is the chunk to update
	QPixmap* qpixmap;
	int dheight;
	int dwidth;
	int nchunks;
	int nvchunks; //# of visible chunks
	int chunksize;
	DataChunk** dataChunks;
	DataChunk*  stepChunk; //here we set the current step/cycle
	int vertical_step;
	int label_width;	//no pixels of the label column
	int panning_width;  //width between label column and data column in which we draw 3 pixels large line
	int style;
	double pickValue;
	int    pickStep;
	bool   pickValueVisible;
	int pickX, pickY;
	QTimer *qtimer;
	EvoListViewer *elw;

	int *vchunks; //visible chunks
public:
	bool nviewChange;
private:
	void evoDataPaint();


};

// Fitviewer
class SALSA_EXPERIMENTS_API FitViewer : public QWidget
{

private:
	DataChunk** dataChunks;
	int nchunks;
	int chunksize;

	double fitVal[4000][3];
	int currentGen;
	int padding;
	double zerox, zeroy;
	double vmin;
	double vmax;
	double wyaxes;
	double xstep;
	int*   sortedIndex;
	QString gtitle;
	QString xlabel;
	QString ylabel;

public:
	void setValues(int gen, double min, double average, double max);
	FitViewer( QWidget* parent=0, Qt::WindowFlags flags=0 );
	FitViewer(int nchunks, int chunkSize, QWidget* parent=0, Qt::WindowFlags flags=0);
	~FitViewer();
private:
	double getYnormValue(double val);
	int checkGraphRange(double val);

protected:
void paintEvent(QPaintEvent* evt);
public:
	void setChunkLabel(int ch, const QString &label);
	void setChunkProperties(int ch, const QString &label, QColor color, bool visible);
	void setChunkValue(int ch,int ind, double value);              //setting value by id
	bool setChunkValue(const QString& name,int ind, double value); //setting value by chunk name
	void diplayUntilStep(int st);
	void setLabels(const QString &title, const QString &xlabel, const QString &ylabel);
	bool loadRawData(int nchunk, const QString &filename, int column);
	void checkChunkRange(int chunk);
	void reset();
	int  getCurrentGeneration();
	void sortchunks();


};

} //end namespace salsa

#endif
