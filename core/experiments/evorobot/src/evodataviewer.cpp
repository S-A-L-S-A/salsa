/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#include "evodataviewer.h"

#include <QPainter>
#include <QPen>
#include <QPaintEvent>
#include <QApplication>
#include <QScrollArea>
#include <QDebug>
#include <QStringList>
#include <QtGui>
#include <QtAlgorithms>
#include "logger.h"

namespace farsa {

//! EvodaViewer constructor
EvoDataViewer::EvoDataViewer(int nchunks, int chunksize, int style, QWidget* parent, Qt::WindowFlags flags) :
	QWidget(parent,flags)
{
	//to do: put this stuff within a method
	this->style=style;
	panning_width=4;
	label_width=100;
	this->setMaximumWidth(chunksize+label_width);
	this->setMinimumWidth(label_width*2);
	this->setMinimumHeight(10*nchunks);
	this->nviewChange=false;
	vchunks=new int[nchunks];

	this->setAttribute(Qt::WA_OpaquePaintEvent, true);

	qpixmap = new QPixmap(width(), height());

	//initialiazing datachunks
	this->nchunks = nchunks;
	this->chunksize = chunksize;
	this->nvchunks=nchunks;

	 dataChunks = new DataChunk*[nchunks];
	for(int i=0;i<nchunks;i++) {
		dataChunks[i] = new DataChunk(QString("chunk"),QColor(255,0,0),chunksize, true);
	}

	stepChunk=new DataChunk(QString("step"),QColor(255,0,0),chunksize, false);

	//this->setChunkLabel(0,"Input");

	this->listVisibleChunks();

	qtimer=new QTimer(this);
	qtimer->setInterval(4000);
	QObject::connect(qtimer,SIGNAL(timeout()),this,SLOT(pickUnvisible()));

	elw=NULL;


	pickY = 0;
	pickX = 0;
	pickValueVisible = false;

}
EvoDataViewer::~EvoDataViewer()
{
	for(int i=0;i<nchunks;i++) {
		delete dataChunks[i];
	}
	delete[] dataChunks;

	delete stepChunk;

	delete qpixmap;
	delete elw;
}

//! clean and draws only the grid
void EvoDataViewer::evoDataPaint()
{
	//draws background and labels and axes
	QPainter painter(qpixmap);
	QPen pen(Qt::black, 1);
	QPen pen2(Qt::lightGray);
	QPen pen3(Qt::darkGray);
	painter.setPen(pen);
	int hh;

	painter.fillRect(0,0,width(),height(),Qt::white);

	for(int i=0;i<nvchunks+1;i++)
	{

	painter.setPen(pen);
		if (i<nvchunks)
		{
		painter.drawText(4,(int)(vertical_step*(i+1)-vertical_step/2.0),dataChunks[vchunks[i]]->getLabel());
		painter.drawText(label_width-40,(int)(vertical_step*(i+1))-4,QString::number(dataChunks[vchunks[i]]->getRangeMin()));
		painter.drawText(label_width-40,(int)(vertical_step*(i+1))-4-(int)(vertical_step-20),QString::number(dataChunks[vchunks[i]]->getRangeMax()));
		}
	//drawing range

	painter.drawLine(0,(int)(vertical_step*i),width(),(int)(vertical_step*i));

	painter.setPen(pen3);
	painter.drawLine(0,(int)(vertical_step*i+1),width(),(int)(vertical_step*i+1));

	painter.setPen(pen2);
	painter.drawLine(0,(int)(vertical_step*i+2),width(),(int)(vertical_step*i+2));
	//	painter.drawLine(50,50,400,400);
	//painter.drawText(4,(int)(vertical_step*i),dataChunks[i]->

	}

	hh=(int)(vertical_step*(nvchunks));
	painter.setPen(pen);

	painter.drawLine(label_width-1,0,label_width-1,hh);
	painter.setPen(pen2);

	painter.drawLine(label_width,0,label_width,hh);
	painter.drawLine(label_width-2,0,label_width-2,hh);
}

//! update values drawing only a line for each chunk so to speed up the drawing time of the data
void EvoDataViewer::updateGraphic(int ch)
{
	if (nviewChange) {
		reset();
		nviewChange = false;
	}

	if (!dataChunks[ch]->isVisible()) {
		// If the chunk is not visible, we have nothing to do
		return;
	}

	QPainter painter(qpixmap);
	const QPen pen(Qt::red);
	const QPen penw(Qt::white);
	const QPen leadpen(Qt::black);

	// Computing the index of the chunk excluding invisible ones
	int i = 0;
	for (int ii = 0; ii < ch; ii++) {
		if (dataChunks[ii]->isVisible()) {
			i++;
		}
	}

	int actualindex = -1;
	int predindex;

	if(dataChunks[i]->getIndex() > -1) {
		actualindex=(int)(dataChunks[vchunks[i]]->getIndex()*dataChunks[vchunks[i]]->getDPRatio());
		predindex=(int)((dataChunks[vchunks[i]]->getIndex()-1)*dataChunks[vchunks[i]]->getDPRatio());
		painter.setPen(pen);

		QPen personalpen(dataChunks[vchunks[i]]->getColor());
		painter.setPen(personalpen);

		///drawing stuff
		int zeropoint=(int)((vertical_step-panning_width-1)*dataChunks[vchunks[i]]->getZeroValue());

		if (style == 0) {
			painter.drawLine(actualindex+label_width,(int)(vertical_step*(i+1)-1)-zeropoint,actualindex+label_width,(int)(vertical_step*(i+1)-1)-zeropoint-(int)((dataChunks[vchunks[i]]->getValueToDraw()-dataChunks[vchunks[i]]->getZeroValue())*(vertical_step-panning_width)));
		} else if (style == 1) {
			painter.drawLine(actualindex+label_width,(int)(vertical_step*(i+1)-1)-zeropoint-(int)((dataChunks[vchunks[i]]->getValueToDraw()-dataChunks[vchunks[i]]->getZeroValue())*(vertical_step-panning_width)),predindex+label_width,(int)(vertical_step*(i+1)-1)-zeropoint-(int)((dataChunks[vchunks[i]]->getValue(dataChunks[vchunks[i]]->getIndex()-1)-dataChunks[vchunks[i]]->getZeroValue())*(vertical_step-panning_width)));
		}

	}

	//white line
	painter.setPen(penw);
	painter.drawLine(actualindex+label_width+1,(int)(vertical_step*(i+1)-1),actualindex+label_width+1,(int)(vertical_step*(i+1)-1)-(int)(vertical_step-panning_width));

	//lead line
	painter.setPen(leadpen);
	painter.drawLine(actualindex+label_width+2,(int)(vertical_step*(i+1)-1),actualindex+label_width+2,(int)(vertical_step*(i+1)-1)-(int)(vertical_step-panning_width));
}

void EvoDataViewer::resizeEvent(QResizeEvent *evt)
{

	evt->accept();
	if (qpixmap!=NULL) delete qpixmap;
	qpixmap=new QPixmap(width(), height());
	reset();
}

void EvoDataViewer::paintEvent(QPaintEvent *evt)
{
	evt->accept();
	int offx,offy;
	offx=0;
	offy=0;
	if (pickY<20) offy=20-pickY;
	if ((width()-pickX)<50) offx=-60;

	QPainter painter(this);
	painter.drawPixmap(0, 0, width(), height(), *qpixmap);
	if(pickValueVisible)
	{
		QPen bpen(Qt::black);
		painter.drawText(pickX+offx,pickY+offy,QString("Val : ")+QString::number(pickValue));
		painter.drawText(pickX+offx,pickY+10+offy,QString("Step: ")+QString::number(pickStep));
	}
}

//! set the chunk label
void EvoDataViewer::setChunkLabel(int ch, const QString &label)
{
	dataChunks[ch]->setLabel(label);
}

//! set chunk value by chunck id
void EvoDataViewer::setChunkValue(int ch, double value)
{
	if ((ch >= 0) && (ch < this->nchunks)) {
		dataChunks[ch]->setData(value);
		updateGraphic(ch);
	}
}

//! set chunk value by chunk label
bool EvoDataViewer::setChunkValue(const QString& name, double value)
{

	for(int i=0;i<nchunks;i++)
	{
		if(name == dataChunks[i]->getLabel())
		{
			setChunkValue(i, value);
			return true;
		}
	}
	return false;
}


//! set the range of the incoming data
void EvoDataViewer::setChunkRange(int ch, double mn, double mx)
{
	dataChunks[ch]->setRange(mn, mx);
}

void EvoDataViewer::mousePressEvent(QMouseEvent* evt)
{
	if(evt->button()==Qt::LeftButton) {
		const int xc = evt->x();
		const int yc = evt->y();
		const int nochunk = (float(yc) / vertical_step);
		if (nochunk >= nvchunks) {
			return;
		}

		const int chunkIndex = xc - label_width;
		const int dataIndex = (int)((float)chunkIndex/dataChunks[vchunks[nochunk]]->getDPRatio());
		if ((dataIndex < 0) || (dataIndex >= chunksize)) {
			return;
		}
		float val = dataChunks[vchunks[nochunk]]->getValue(dataIndex);
		val = dataChunks[vchunks[nochunk]]->linearMap(val,0.0, 1.0,dataChunks[vchunks[nochunk]]->getRangeMin(), dataChunks[vchunks[nochunk]]->getRangeMax());

		pickX = xc;
		pickY = yc;
		pickValue = val;
		pickStep = (int)stepChunk->getValue((int)((float)chunkIndex/dataChunks[vchunks[nochunk]]->getDPRatio()));
		pickValueVisible = true;
		qtimer->start();

		update();
	}

	if(evt->button()==Qt::RightButton) {
		if(elw==NULL) {
			elw = new EvoListViewer (this->dataChunks,this->nchunks, &this->nviewChange);
		} else {
			elw->restoreSelected();
			elw->setVisible(true);
		}
	}
}

//! cleans the data about value and step on the screen
void EvoDataViewer::pickUnvisible()
{
	pickValueVisible=false;
	qtimer->stop();
	update();
}

/*! it stores the current step for each value. It is useful for retriving later the value and the step of a single point
	by clicking on the graph
*/
void EvoDataViewer::setCurrentStep(int step)
{
	stepChunk->setDataRaw(step);

}
//! set the drawing style 0: filled lines, 1: simple line
void EvoDataViewer::setStyle(int style)
{
	switch (style)
	{
	case 0:
		style=0;
		break;
	case 1:
		style=1;
		break;
	default:
		style=1;
		break;

	}

}
int EvoDataViewer::visibleChunks()
{
	int vchunks=0;
	for(int i=0;i<nchunks;i++)
		if(dataChunks[i]->isVisible())
			vchunks++;
	nvchunks=vchunks;
	return vchunks;
}

//! Creates a list of the visible chunks
void EvoDataViewer::listVisibleChunks()
{
	int vi=0;
	for(int i=0;i<this->nchunks;i++)
	{
		if (dataChunks[i]->isVisible())
		{
			this->vchunks[vi]=i;
			vi++;
		}

	}
	this->nvchunks=vi;
	vertical_step=height()/(double)nvchunks;
}

void EvoDataViewer::setChunkColor(int ch, QColor color)
{
	dataChunks[ch]->setColor(color);
}
//! Set all the chunk properties in one step
void EvoDataViewer::setChunkProperties(int ch, double rangeMin, double rangeMax, const QString &label, QColor color, bool visible)
{
	dataChunks[ch]->setRange(rangeMin, rangeMax);
	dataChunks[ch]->setLabel(label);
	dataChunks[ch]->setColor(color);
	dataChunks[ch]->setVisible(visible);

}
//! Cleans the screen and resets chunk indexes in order to display new data
void EvoDataViewer::reset()
{
	this->listVisibleChunks();
	vertical_step=height()/(double)nvchunks;
	//
	for(int i=0;i<nchunks;i++)
	{
		//dataChunks[i]->setDSize(width()-label_width);
		dataChunks[i]->setDPRatio((double)(width()-label_width)/(double)chunksize);
	}

	evoDataPaint();
}
// DataChunk implementation -------------------------------------------------------------------------------------------------------

DataChunk::DataChunk(const QString&  lab, const QColor& col, int s, bool vis)
	: color(col)
	, label(lab)
	, size(s)
	, dpratio(1)
	, visible(vis)
	, index(-1)
	, min(0)
	, max(1)
	, maxValue(-9999.00)
	, style(0)
	, data(NULL)
	, zeroValue(0.0)
{
	data= new double[size];
	this->index=-1;
	for(int i=0;i<size;i++) {
		data[i]=0;
	}
}

DataChunk::~DataChunk()
{
	delete[] data;
}

void DataChunk::setColor(QColor color)
{
	this->color=color;

}

void DataChunk::setData(double value)
{
	index++;

	data[index]=linearMap(value, min, max);
	checkMaxValue(data[index]);
	//index++;
	if(index>size-2) index=-1;

}
void DataChunk::setDataRaw(double value)
{
		index++;
	data[index]=value;
	checkMaxValue(value);

	//index++;
	if(index>size-2) index=-1;
}
void DataChunk::setDataRaw(int ind, double value)
{
	if(ind>-1 && ind<size)
	data[ind]=value;
	checkMaxValue(value);
}

void DataChunk::setLabel(const QString& label)
{
	this->label=label;
}

void DataChunk::setRange(double min, double max)
{
	this->min=min;
	this->max=max;
	zeroValue=linearMap(0.0,min,max);
}

void DataChunk::setStyle(int style)
{
	this->style = style;

}
QString DataChunk::getLabel() const
{

	return this->label; // + "=" + QString::number(data[index]);
}


double DataChunk::getValueToDraw()
{
	return data[index];
}

int DataChunk::getIndex()
{
	return index;

}
double DataChunk::linearMap(double x, double rmin, double rmax, double outMin, double outMax)
{
	//Reusing here Gianluca Masssera's code.
	double m = ( outMax-outMin )/( rmax-rmin );
    double q = outMin - m*rmin;
    double ret = m*x+q;
    if (ret < outMin) return outMin;
    if (ret > outMax) return outMax;
    return ret;


}

double DataChunk::getRangeMin()
{
	return min;
}


double DataChunk::getRangeMax()
{
	return max;
}

double DataChunk::getZeroValue()
{
	return zeroValue;
}

void DataChunk::setDPRatio(double val)
{
	dpratio=val;
	this->index=-1;
}

double DataChunk::getDPRatio()
{
	return dpratio;
}

double DataChunk::getValue(int ind)
{
	return data[ind];
}

bool DataChunk::isVisible()
{
	return visible;
}

void DataChunk::setVisible(bool vis)
{
	visible=vis;
}
QColor& DataChunk::getColor()
{
	return color;
}
double DataChunk::getMaxValue()
{
	return maxValue;
}

namespace __DataChunk_loadRawData_helpers {
	// This has been copied from Factory::orderByNumberAfterColon. Perhaps we could put this function into an utility
	// library, instead of copying it around...
	bool orderByNumberAfterColon(const QString& s1, const QString& s2)
	{
		// If a string doesn't contain any colon, it always follows the other string; this way strings without colons are always
		// at the end when sorting
		QStringList list = s1.split(':', QString::SkipEmptyParts);
		if (list.size() < 2) {
			return false;
		}
		const double ns1 = list[1].toDouble();
		list = s2.split(':', QString::SkipEmptyParts);
		if (list.size() < 2) {
			return true;
		}
		const double ns2 = list[1].toDouble();

		return (ns1 < ns2);
	}
}
void DataChunk::checkMaxValue(double val)
{
	if(val>maxValue) maxValue=val;
}
bool DataChunk::loadRawData(const QString &filename, int column)
{
	// Tomassino: this is really ugly, but I have no better (and quick to implement) idea for the moment
	if (filename.endsWith(".fit", Qt::CaseInsensitive)) {
		index = 0;
		QFile file(filename);
		QString line;
		if(file.open(QIODevice::ReadOnly)) {
			QTextStream in(&file);
			line = in.readLine();

			while(!line.isNull()) {
				//process line
				QStringList list;
				list = line.split(" ");
				if (column<0 && column>=list.size()) {
					Logger::error(QString("column number %1 does not exist in the loaded file.").arg(column));
					return false;
				}

				QString value=(QString)list.at(column);
				setDataRaw(index, value.toDouble());
				index++;

				line=in.readLine();
			}
			file.close();

			return true;
		}
	} else {
		return false;

// 		// Trying to load using ConfigurationParameters
// 		farsa::ConfigurationParameters params(true);
//
// 		if (!params.loadParameters(filename)) {
// 			return false;
// 		}
//
// 		// Getting the parameter name corresponding to the given column
// 		QString paramName;
// 		switch (column) {
// 			case 0:
// 				paramName = "bestFitness";
// 				break;
// 			case 1:
// 				paramName = "averageFitness";
// 				break;
// 			case 2:
// 				paramName = "worstFitness";
// 				break;
// 			default:
// 				paramName = "Unknown";
// 				break;
// 		}
//
// 		// Here we look for all the groups named GENOTYPES:<number> and read some parameters from each
// 		QStringList genotypes = params.getGroupsWithPrefixList("/", "GENOTYPES:");
//
// 		qSort(genotypes.begin(), genotypes.end(), __DataChunk_loadRawData_helpers::orderByNumberAfterColon);
// 		index = 0;
// 		for (int i = 0; i < genotypes.size(); i++) {
// 			setDataRaw(index, params.getValue(genotypes[i] + farsa::ConfigurationParameters::GroupSeparator() + paramName).toDouble());
// 			index++;
// 		}
//
// 		return true;
	}

	//use the return value to give feedback instead of standard output
	return false;
}



// implementing EvoListViewer -----------------------------------------------------------------------------------------------------

EvoListViewer::EvoListViewer(DataChunk **dataChunks, int n, bool *nviewChange ,  QWidget *parent) :
	QWidget(parent)
{
	this->dataChunks=dataChunks;
	this->nchunks=n;
	this->nvchange=nviewChange;
	QListWidgetItem *listItem;


	layout=new QGridLayout();
	listwidget = new QListWidget();
	deselectAll = new QPushButton("deselect all");
	QPushButton *bcancel = new QPushButton("cancel");
	QPushButton *bok = new QPushButton("ok");
	for(int i=0;i<n;i++)
	{
	//listwidget->insertItem(i,dataChunks[i]->getLabel());
		listwidget->insertItem(i, new QListWidgetItem());

		listItem=listwidget->item(i);
		listItem->setText(dataChunks[i]->getLabel());
		Qt::ItemFlags mflags;
		mflags=  Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
		listItem->setFlags(mflags);
		listItem->setCheckState(Qt::Checked);
	}

	//listwidget.insertItem(3,"terzo");


	layout->addWidget(deselectAll, 0,0);
	layout->addWidget(listwidget,1,0);
	layout->addWidget(bcancel,2,0);
	layout->addWidget(bok,3,0);



	this->setLayout(layout);
	this->setWindowTitle("Chunks List");

	this->setGeometry(50,50,250,300);
	this->setVisible(true);

	//QObject::connect(qtimer,SIGNAL(timeout()),this,SLOT(pickUnvisible()));
	QObject::connect(bok,SIGNAL(clicked()),this,SLOT(okSelected()));
	QObject::connect(bcancel,SIGNAL(clicked()),this,SLOT(restoreSelected()));
	QObject::connect(deselectAll, SIGNAL(clicked()),this,SLOT(allOnOrAllOff()));

	//setting up the gui





}

void EvoListViewer::okSelected()
{
	for(int i=0;i<nchunks;i++)
	{
		if(this->listwidget->item(i)->checkState()== Qt::Checked)
			dataChunks[i]->setVisible(true);
		else
			dataChunks[i]->setVisible(false);
	}
	this->setVisible(false);
	*nvchange=true;
}

void EvoListViewer::restoreSelected()
{
	//reset choices
	this->setVisible(false);
	for(int i=0;i<nchunks;i++)
	{
		if(dataChunks[i]->isVisible())
			this->listwidget->item(i)->setCheckState(Qt::Checked);
		else
			this->listwidget->item(i)->setCheckState(Qt::Unchecked);


	}
}

void EvoListViewer::allOnOrAllOff()
{
	bool someoneIsChecked = false;
	for(int i=0;i<nchunks;i++)	//check if there are checked items
	{
		if(listwidget->item(i)->checkState() == Qt::Checked)
			someoneIsChecked = true;
	}

	bool select;
	if(someoneIsChecked)	//some item(s) is checked
	{
		select = false;
		deselectAll->setText("select all");
	}
	else		//there are no checked items
	{
		select = true;
		deselectAll->setText("deselect all");
	}

	for(int i=0;i<nchunks;i++)
	{
		if(select)
			this->listwidget->item(i)->setCheckState(Qt::Checked);
		else
			this->listwidget->item(i)->setCheckState(Qt::Unchecked);
	}
}

/*
void EvoListViewer::resizeEvent(QResizeEvent *evt)
{
	//layout->
}
*/

EvoListViewer::~EvoListViewer()
{
	delete listwidget;
	delete layout;


}

//___________________ FitViewer

void FitViewer::setValues(int gen, double min, double average, double max)
{

	fitVal[gen][0]=min;
	fitVal[gen][1]=average;
	fitVal[gen][2]=max;
	currentGen=gen;

	//checking minimum and maximum
	//if (min<vmin) vmin=min;
	//if (max>vmax) vmax=max;
	checkGraphRange(min);
	checkGraphRange(average);
	checkGraphRange(max);



}
int  FitViewer::checkGraphRange(double val)
{
	int ret=0;
	if (val<vmin)
	{
		vmin=val;
		ret++;
	}
	if (val>vmax)
	{
		vmax=val;
		ret++;
	}
	return ret;
}

void FitViewer::checkChunkRange(int chunk)
{
	//for(int i=0;i<nchunks;i++)
		for(int c=0;c<=currentGen;c++)
			checkGraphRange(dataChunks[chunk]->getValue(c));
}

FitViewer::FitViewer( QWidget* parent, Qt::WindowFlags flags ) : QWidget( parent, flags )
{

	padding=50;
	reset();

	sortedIndex = NULL;
	dataChunks = NULL;
}

FitViewer::FitViewer(int nchunks, int chunksize, QWidget* parent, Qt::WindowFlags flags) : QWidget( parent, flags )
{

	padding=100;
	reset();
	this->nchunks=nchunks;
	this->chunksize=chunksize;
	currentGen=0;
	sortedIndex=new int[nchunks];
	dataChunks = new DataChunk*[nchunks];
	for(int i=0;i<nchunks;i++) {
		dataChunks[i] = new DataChunk(QString("chunk"),QColor(255,0,0),chunksize, true);
		sortedIndex[i]=i;
	}
}

FitViewer::~FitViewer()
{
	delete[] sortedIndex;
	if (dataChunks != NULL) {
		for (int i = 0; i < nchunks; i++) {
			delete dataChunks[i];
		}
		delete[] dataChunks;
	}
}

void FitViewer::paintEvent(QPaintEvent* /*evt*/)
{
	QPainter painter(this);
	QPen blackPen(Qt::black);
	QPen bluePen(Qt::blue);
	QPen greenPen(Qt::green);
	QPen redPen(Qt::red);
	int xt,yt;

	painter.fillRect(0,0,width(),height(),Qt::white);
	painter.setPen(blackPen);
	painter.setRenderHint(QPainter::Antialiasing, false);
	xstep=(double)(width()-2*padding)/(double)(currentGen);
	wyaxes=height()-2*padding;

	painter.drawRect(padding,padding,width()-2*padding,height()-2*padding);
	if (vmin>0) zeroy=height()-padding;
	else
	{
	zeroy=((-1*vmin)/(vmax-vmin))*wyaxes;
	}

	painter.drawLine(padding,height()-padding-zeroy,width()-padding,height()-padding-zeroy);
	painter.drawText(padding/2.0,padding,QString::number(vmax));
	painter.drawText(padding/2.0,height()-padding,QString::number(vmin));
	painter.drawText(padding/2.0,height()-padding-zeroy,QString::number(0));

	//drawing zero axes
	painter.drawText(width()-padding,height()-padding-zeroy,QString::number(currentGen));

	//graph title
	painter.drawText(width()/2.0-padding,padding/2.0,gtitle);

	//drawing xlabel
	painter.drawText(width()/2.0-padding,height()-padding/2.0,xlabel);

	//drawing ylabel
	xt=padding/4.0;
	yt=height()/2.0;
	painter.save();
	painter.translate(xt,yt);
	painter.rotate(-90.0);
	painter.drawText(0,0,ylabel);
	painter.restore();


	/*
	painter.setPen(bluePen);
	painter.drawText(width()/2.0,padding/4.0*3,"Minimum");
	painter.setPen(greenPen);
	painter.drawText(width()/2.0,padding/4.0*2,"Average");
	painter.setPen(redPen);
	painter.drawText(width()/2.0,padding/4.0*1,"Maximum");
	*/

	painter.setRenderHint(QPainter::Antialiasing, true);
	for (int dc=0;dc<nchunks;dc++)
	{
	int c=sortedIndex[dc];
	painter.setPen(dataChunks[c]->getColor());
	painter.drawText(width()-padding+2,padding+dc*20,dataChunks[c]->getLabel());

	for (int i=1;i<currentGen+1;i++)
	{
		/*
	painter.setPen(bluePen);
	painter.drawLine(padding+xstep*(i-1), height()-padding-getYnormValue(fitVal[i-1][0]),padding+xstep*i,height()-padding-getYnormValue(fitVal[i][0]));
	painter.setPen(greenPen);
	painter.drawLine(padding+xstep*(i-1), height()-padding-getYnormValue(fitVal[i-1][1]),padding+xstep*i,height()-padding-getYnormValue(fitVal[i][1]));
	painter.setPen(redPen);
	painter.drawLine(padding+xstep*(i-1), height()-padding-getYnormValue(fitVal[i-1][2]),padding+xstep*i,height()-padding-getYnormValue(fitVal[i][2]));
	*/

	checkGraphRange(dataChunks[c]->getValue(i-1));
	painter.drawLine(padding+xstep*(i-1), height()-padding-getYnormValue(dataChunks[c]->getValue(i-1)),padding+xstep*i,height()-padding-getYnormValue(dataChunks[c]->getValue(i)));


	}
	}

}

double FitViewer::getYnormValue(double val)
{
	if(vmin>0)
	{
	return (val/vmax)*wyaxes;
	}
	else
	{

	return ((val-vmin)/(vmax-vmin))*wyaxes;

	}
}

void FitViewer::setChunkLabel(int ch, const QString &label)
{

	dataChunks[ch]->setLabel(label);
}
void FitViewer::setChunkProperties(int ch, const QString &label, QColor color, bool visible)
{
	dataChunks[ch]->setLabel(label);
	dataChunks[ch]->setColor(color);
	dataChunks[ch]->setVisible(visible);
}

void FitViewer::setChunkValue(int ch,int ind, double value)
{
	if (ch>= 0 && ch < this->nchunks)
	{
	dataChunks[ch]->setDataRaw(ind,value);
	checkGraphRange(value);
	}
}

bool FitViewer::setChunkValue(const QString &name,int ind, double value)
{
		for(int i=0;i<nchunks;i++)
	{
		if(name == dataChunks[i]->getLabel())
		{
			setChunkValue(i,ind, value);
			checkGraphRange(value);
			return true;
		}
	}
	return false;

}
void FitViewer::diplayUntilStep(int st)
{
	currentGen=st;
}

void FitViewer::setLabels(const QString &title, const QString &xlabel, const QString &ylabel)
{
	this->gtitle=title;
	this->xlabel=xlabel;
	this->ylabel=ylabel;
}

void FitViewer::reset()
{
	vmin=-0.0;
	vmax=0.0;
	padding=90;
	setMinimumSize(padding*4,padding*4);
	currentGen=0;
}

bool FitViewer::loadRawData(int nchunk, const QString &filename, int column)
{
	bool res;
	res=dataChunks[nchunk]->loadRawData(filename,column);
	if(res)
	{
		//currentGen=dataChunks[nchunk]->getIndex()-1;
		if((dataChunks[nchunk]->getIndex()-1)>currentGen) currentGen=dataChunks[nchunk]->getIndex()-1;
	}
	checkChunkRange(nchunk);
	return res;
}

int FitViewer::getCurrentGeneration()
{
	return currentGen;
}

void FitViewer::sortchunks()
{
	bool swap=true;
	for(int i=0;i<nchunks;i++)
	{
		sortedIndex[i]=i;
	}

	while (swap)
	{
		swap=false;
		int s;
		for(int i=0;i<nchunks-1;i++)
		{
			if (dataChunks[sortedIndex[i]]->getMaxValue()<dataChunks[sortedIndex[i+1]]->getMaxValue())
			{
				s=sortedIndex[i];
				sortedIndex[i]=sortedIndex[i+1];
				sortedIndex[i+1]=s;
				swap=true;
			}
		}
	}
}

} //end namespace farsa
