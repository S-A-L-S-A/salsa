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

#include "evonet.h"
#include "logger.h"
#include "configurationhelper.h"
#include "evonetui.h"
#include "mathutils.h"
#include "evonetiterator.h"
#include <QFileInfo>
#include <cstring>
#include <limits>
#include <algorithm>

#include <Eigen/Dense>

#ifndef SALSA_MAC
	#include <malloc.h>  // DEBUG: to be sobstituted with new
#endif

// All the suff below is to avoid warnings on Windows about the use of unsafe
// functions. This should be only a temporary workaround, the solution is stop
// using C string and file functions...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

// MAXN and MAXSTOREDACTIVATIONS are declared in .h and their value is set there, but that is not a
// definition. This means that if you try to get the address of MAXN or MAXSTOREDACTIVATIONS you get
// a linker error (this also happends if you pass one of them to a function that takes a reference
// or const reference). However, we cannot initialize them here because these are used to define
// arrays, so their value must be present in the .h
// We must however not define them on Visual Studio 2008...
#if !defined(_MSC_VER) || _MSC_VER > 1600
const int Evonet::MAXSTOREDACTIVATIONS;
const int Evonet::MAXN;
#endif
const float Evonet::DEFAULT_VALUE = -99.0f;

Evonet::Evonet(ConfigurationManager& params)
	: Controller(params)
	, neuronsMonitorUploader(20, DataUploader<ActivationsToGui>::IncreaseQueueSize /*BlockUploader*/) // we can be ahead of GUI by at most 20 steps, then we are blocked
	, m_evonetUI(NULL)
	, m_evonetIterator(NULL)
	, m_inputCurIndex(0)
	, m_outputCurIndex(0)
{
	wrange = 5.0; // weight range
	grange = 5.0; // gain range
	brange = 5.0; // bias range
	neuronlesions = 0;
	freep = new float[1000];
	phep = NULL;
	muts = NULL;
	geneMaxValue = 255;
	pheloaded = false;
	selectedp= (float **) malloc(100 * sizeof(float **));
	for (int i = 0; i < MAXN; i++) {
		neuronl[i][0] = '\0';
		neurondisplay[i] = 1;
		neuronrange[i][0] = 0.0;
		neuronrange[i][1] = 1.0;
		neurondcolor[i] = Qt::black;
		neuronlesion[i] = false;
		neuronlesionVal[i] = 0.0;
	}
	net_nblocks = 0;

	nextStoredActivation = 0;
	firstStoredActivation = 0;
	updatescounter = 0;

	updateNeuronMonitor = false;
}

void Evonet::setNetworkName(const QString& name)
{
	networkName = name;
}

const QString& Evonet::getNetworkName() const
{
	return networkName;
}

void Evonet::configure()
{
	Controller::configure();

	int nSensors = computeNumSensorsFromControllerInputsList();
	int nMotors = computeNumMotorsFromControllerOutputsList();

	int nHiddens = ConfigurationHelper::getInt( configurationManager(), confPath()+"nHiddens");
	QString netFile = ConfigurationHelper::getString( configurationManager(), confPath()+"netFile");

	wrange = ConfigurationHelper::getReal(configurationManager(), confPath() + "weightRange"); // the range of synaptic weights
	grange = ConfigurationHelper::getReal(configurationManager(), confPath() + "gainRange"); // the range of gains
	brange = ConfigurationHelper::getReal(configurationManager(), confPath() + "biasRange"); // the range of biases

	updateMonitor = true; //by default it is always updated

	if ( netFile.isEmpty() ) {
		// generate a neural network from parameters
		ninputs  = nSensors;
		nhiddens = nHiddens;
		noutputs = nMotors;
		nneurons = ninputs + nhiddens + noutputs;
		if (this->nneurons > MAXN) {
			ConfigurationHelper::throwUserConfigError(confPath() + "(nSensors + nHiddens + nMotors)", QString::number(nneurons), "Too many neurons: increase MAXN to support more than " + QString::number(MAXN) + " neurons");
		}
		int inputNeuronType = 0;
		QString str = ConfigurationHelper::getEnum( configurationManager(), confPath() + "inputNeuronType");
		if ( str == QString("no_delta") ) {
			inputNeuronType = 0;
		} else if ( str == QString("with_delta") ) {
			inputNeuronType = 1;
		} else {
			ConfigurationHelper::throwUserConfigError(confPath() + "inputNeuronType", str, "Wrong value (use \"no_delta\" or \"with_delta\"");
		}
		int hiddenNeuronType = 0;
		str = ConfigurationHelper::getEnum( configurationManager(), confPath() + "hiddenNeuronType");
		if ( str == QString("logistic") ) {
			hiddenNeuronType = 0;
		} else if ( str == QString("logistic+delta") ) {
			hiddenNeuronType = 1;
		} else if ( str == QString("binary") ) {
			hiddenNeuronType = 2;
		} else if ( str == QString("logistic_0.2") ) {
			hiddenNeuronType = 3;
		} else {
			ConfigurationHelper::throwUserConfigError(confPath() + "hiddenNeuronType", str, "Wrong value (use \"logistic\", \"logistic+delta\", \"binary\" or \"logistic_0.2\"");
		}
		int outputNeuronType = 0;
		str = ConfigurationHelper::getEnum( configurationManager(), confPath() + "outputNeuronType");
		if ( str == QString("no_delta") ) {
			outputNeuronType = 0;
		} else if ( str == QString("with_delta") ) {
			outputNeuronType = 1;
		} else {
			ConfigurationHelper::throwUserConfigError(confPath() + "outputNeuronType", str, "Wrong value (use \"no_delta\" or \"with_delta\"");
		}
		bool recurrentHiddens = ConfigurationHelper::getBool( configurationManager(), confPath() + "recurrentHiddens");
		bool inputOutputConnections = ConfigurationHelper::getBool( configurationManager(), confPath() + "inputOutputConnections");
		bool recurrentOutputs = ConfigurationHelper::getBool( configurationManager(), confPath() + "recurrentOutputs");
		bool biasOnHidden = ConfigurationHelper::getBool( configurationManager(), confPath() + "biasOnHiddenNeurons");
		bool biasOnOutput = ConfigurationHelper::getBool( configurationManager(), confPath() + "biasOnOutputNeurons");
		create_net_block( inputNeuronType, hiddenNeuronType, outputNeuronType, recurrentHiddens, inputOutputConnections, recurrentOutputs, biasOnHidden, biasOnOutput );
	} else {
		// load the neural network from file. If the file doesn't exists, throwing an exception
		if (load_net_blocks(netFile.toLatin1().data(), 0) == 0) {
			ConfigurationHelper::throwUserConfigError(confPath() + "netFile", netFile, "Could not open the specified network configuration file");
		}
		// Checking sensors and motors are sufficient
		if (ninputs != nSensors) {
			ConfigurationHelper::throwUserConfigError(confPath() + "netFile", netFile, QString("The net file has the wrong number of inputs (expected %1, got %2)").arg(nSensors).arg(ninputs));
		} else if (noutputs != nMotors) {
			ConfigurationHelper::throwUserConfigError(confPath() + "netFile", netFile, QString("The net file has the wrong number of outputs (expected %1, got %2)").arg(nMotors).arg(noutputs));
		}
	}

	computeParameters();
	// --- reallocate data on the basis of number of parameters
	delete[] freep;
	freep=new float[nparameters+1000];  // we allocate more space to handle network variations introduced by the user
	for(int r=0;r<nparameters;r++)
		freep[r]=0.0f;

	delete[] phep;
	phep=new float[nparameters+1000];   // we allocate more space to handle network variations introduced by the user
	for(int r=0;r<nparameters;r++)
		phep[r]=DEFAULT_VALUE; // default value correspond to dont' care

	delete[] muts;
	muts=new float[nparameters+1000];   // we allocate more space to handle network variations introduced by the user
	for(int r=0;r<nparameters;r++)
		muts[r]=DEFAULT_VALUE; // default value correspond to dont' care

	if ( !netFile.isEmpty() ) {
		QFileInfo fileNet( netFile );
		QString filePhe = fileNet.baseName() + ".phe";
		load_net_blocks(filePhe.toLatin1().data(), 1);
	}

	//resetting net
	resetNet();

	printBlocks();

	// we create the labels of the hidden neurons
	for(int i = 0; i < nhiddens; i++) {
		sprintf(neuronl[ninputs+i], "h%d", i);
		neuronrange[ninputs+i][0] = 0.0;
		neuronrange[ninputs+i][1] = 1.0;
		neurondcolor[ninputs+i] = QColor(125,125,125);
	}
}

void Evonet::postConfigureInitialization()
{
	Controller::postConfigureInitialization();
}

void Evonet::describe(RegisteredComponentDescriptor& d)
{
	Controller::describe(d);

	d.help("Neural Network imported from Evorobot");

	d.describeInt( "nHiddens" ).limits( 0, MAXN ).def(0).help( "The number of hidden neurons" );
	d.describeString( "netFile" ).def("").help( "The file .net where is defined the architecture to load. WARNING: when this parameter is specified any other parameters will be ignored" );
	d.describeReal( "weightRange" ).def(5.0f).limits(1,+Infinity).help( "The synpatic weight of the neural network can only assume values in [-weightRange, +weightRange]" );
	d.describeReal( "gainRange" ).def(5.0f).limits(0,+Infinity).help( "The gain of a neuron will can only assume values in [0, +gainRange]" );
	d.describeReal( "biasRange" ).def(5.0f).limits(0,+Infinity).help( "The bias of a neuron will can only assume values in [-biasRange, +biasRange]" );
	d.describeEnum( "inputNeuronType" ).def("no_delta").values( QStringList() << "no_delta" << "with_delta" ).help( "The type of input neurons when the network is auto generated");
	d.describeEnum( "hiddenNeuronType" ).def("logistic").values( QStringList() << "logistic" << "logistic+delta" << "binary" << "logistic_0.2" ).help( "The type of hidden neurons when the network is auto generated");
	d.describeEnum( "outputNeuronType" ).def("no_delta").values( QStringList() << "no_delta" << "with_delta" ).help( "The type of output neurons when the network is auto generated");
	d.describeBool( "recurrentHiddens" ).def(false).help( "when true generated a network with recurrent hidden neurons");
	d.describeBool( "inputOutputConnections" ).def(false).help( "when true generated a network with input-output connections in addition to input-hidden-output connections");
	d.describeBool( "recurrentOutputs" ).def(false).help( "when true generated a network with recurrent output neurons");
	d.describeBool( "biasOnHiddenNeurons" ).def(false).help( "when true generate a network with hidden neurons with a bias");
	d.describeBool( "biasOnOutputNeurons" ).def(false).help( "when true generate a network with output neurons with a bias");
	d.describeBool( "showTeachingInput" ).def(false).help( "Whether the teaching input has to be shown in the UI");
}

ComponentUI* Evonet::getUIManager() {
	m_evonetUI = new EvonetUI( this, &neuronsMonitorUploader );
	return m_evonetUI;
}

void Evonet::update()
{
	updateNet();
}

void Evonet::resetControllerStatus()
{
	resetNet();
}

void Evonet::create_net_block( int inputNeuronType, int hiddenNeuronType, int outputNeuronType, bool recurrentHiddens, bool inputOutputConnections, bool recurrentOutputs, bool biasOnHidden, bool biasOnOutput )
{
	int n;
	int i;
	int startx;
	int dx;

	// setting the neuron types
	for(i = 0; i < this->ninputs; i++) {
		this->neurontype[i]= inputNeuronType;
		neuronbias[i] = 0;
	}
	for(i = this->ninputs; i < (this->nneurons - this->noutputs); i++) {
		this->neurontype[i]= hiddenNeuronType;
		neuronbias[i] = (biasOnHidden) ? 1 : 0;
	}
	for(i = (this->nneurons - this->noutputs); i < this->nneurons; i++) {
		this->neurontype[i]= outputNeuronType;
		neuronbias[i] = (biasOnOutput) ? 1 : 0;
	}

	// gain
	for(i=0; i < this->nneurons; i++) {
		this->neurongain[i]= 0;
	}

	this->net_nblocks = 0;
	// input update block
	this->net_block[this->net_nblocks][0] = 1;
	this->net_block[this->net_nblocks][1] = 0;
	this->net_block[this->net_nblocks][2] = this->ninputs;
	this->net_block[this->net_nblocks][3] = 0;
	this->net_block[this->net_nblocks][4] = 0;
	this->net_block[this->net_nblocks][5] = 0;
	this->net_nblocks++;

	// input-hidden connections
	if (this->nhiddens > 0) {
		this->net_block[this->net_nblocks][0] = 0;
		this->net_block[this->net_nblocks][1] = this->ninputs;
		this->net_block[this->net_nblocks][2] = this->nhiddens;
		this->net_block[this->net_nblocks][3] = 0;
		this->net_block[this->net_nblocks][4] = this->ninputs;
		this->net_block[this->net_nblocks][5] = 0;
		this->net_nblocks++;
	}

	// hidden-hidden connections
	if (recurrentHiddens) {
		this->net_block[this->net_nblocks][0] = 0;
		this->net_block[this->net_nblocks][1] = this->ninputs;
		this->net_block[this->net_nblocks][2] = this->nhiddens;
		this->net_block[this->net_nblocks][3] = this->ninputs;
		this->net_block[this->net_nblocks][4] = this->nhiddens;
		this->net_block[this->net_nblocks][5] = 0;
		this->net_nblocks++;
	}

	// hidden update block
	if (this->nhiddens > 0) {
		this->net_block[this->net_nblocks][0] = 1;
		this->net_block[this->net_nblocks][1] = this->ninputs;
		this->net_block[this->net_nblocks][2] = this->nhiddens;
		this->net_block[this->net_nblocks][3] = 0;
		this->net_block[this->net_nblocks][4] = 0;
		this->net_block[this->net_nblocks][5] = 0;
		this->net_nblocks++;
	}

	// input-output connections
	if (this->nhiddens == 0 || inputOutputConnections) {
		this->net_block[this->net_nblocks][0] = 0;
		this->net_block[this->net_nblocks][1] = this->ninputs + this->nhiddens;
		this->net_block[this->net_nblocks][2] = this->noutputs;
		this->net_block[this->net_nblocks][3] = 0;
		this->net_block[this->net_nblocks][4] = this->ninputs;
		this->net_block[this->net_nblocks][5] = 0;
		this->net_nblocks++;
	}

	// hidden-output connections
	if (this->nhiddens > 0) {
		this->net_block[net_nblocks][0] = 0;
		this->net_block[net_nblocks][1] = this->ninputs + this->nhiddens;
		this->net_block[net_nblocks][2] = this->noutputs;
		this->net_block[net_nblocks][3] = this->ninputs;
		this->net_block[net_nblocks][4] = this->nhiddens;
		this->net_block[this->net_nblocks][5] = 0;
		this->net_nblocks++;
	}

	// output-output connections
	if (recurrentOutputs) {
		this->net_block[this->net_nblocks][0] = 0;
		this->net_block[this->net_nblocks][1] = this->ninputs + this->nhiddens;
		this->net_block[this->net_nblocks][2] = this->noutputs;
		this->net_block[this->net_nblocks][3] = this->ninputs + this->nhiddens;
		this->net_block[this->net_nblocks][4] = this->noutputs;
		this->net_block[this->net_nblocks][5] = 0;
		this->net_nblocks++;
	}

	// output update block
	this->net_block[this->net_nblocks][0] = 1;
	this->net_block[this->net_nblocks][1] = this->ninputs + this->nhiddens;
	this->net_block[this->net_nblocks][2] = this->noutputs;
	this->net_block[this->net_nblocks][3] = 0;
	this->net_block[this->net_nblocks][4] = 0;
	this->net_block[this->net_nblocks][5] = 0;
	this->net_nblocks++;

	// cartesian xy coordinate for sensory neurons for display (y=400)
	n = 0;
	dx = 30;//25
	if (this->ninputs > this->noutputs) {
		startx = 50;
	} else {
		startx = ((this->noutputs - this->ninputs) / 2) * dx + 50;
	}
	for(i = 0; i < this->ninputs; i++, n++) {
		this->neuronxy[n][0] = (i * dx) + startx;
		this->neuronxy[n][1] = 400;
	}

	// cartesian xy coordinate for internal neurons for display (y=225)
	startx = this->ninputs * dx;
	for(i=0; i < (this->nneurons - (this->ninputs + this->noutputs)); i++, n++) {
		this->neuronxy[n][0] = startx + (i * dx);
		this->neuronxy[n][1] = 225;
	}

	// cartesian xy coordinate for motor neurons for display (y=50)
	if (this->ninputs > this->noutputs) {
		startx = ((this->ninputs - this->noutputs) / 2) * dx + 50;
	} else {
		startx = 50;
	}
	for(i=0; i < this->noutputs; i++, n++) {
		this->neuronxy[n][0] = startx + (i * dx);
		this->neuronxy[n][1] = 50;
	}

	// set neurons whose activation should be displayed
	for(i=0; i < this->nneurons; i++) {
		this->neurondisplay[i] = 1;
	}

	// calculate the height and width necessary to display all created neurons (drawnymax, drawnxmax)
	drawnymax = 400 + 30;
	for(i = 0, drawnxmax = 0; i < nneurons; i++) {
		if (neuronxy[i][0] > drawnxmax) {
			drawnxmax = neuronxy[i][0];
		}
	}
	drawnxmax += 60;

	// compute the number of parameters
	computeParameters();

	//i = this->ninputs;
	//if (this->ninputs > i)
	//  i = this->noutputs;
	//if ((this->nneurons - this->noutputs) > i)
	//  i = (this->nneurons - this->noutputs);
	//drawnxmax = (i * dx) + dx + 30;
}

int Evonet::load_net_blocks(const char *filename, int mode)
{

	FILE  *fp;
	int   b;
	int   n;
	int   i;
	float *ph;
	float *mu;
	float *p;
	int   np;
	const int bufferSize = 128;
	char  cbuffer[bufferSize];

	if ((fp = fopen(filename,"r")) != NULL)
	{
		fscanf(fp,"ARCHITECTURE\n");
		fscanf(fp,"nneurons %d\n", &nneurons);
		fscanf(fp,"nsensors %d\n", &ninputs);
		fscanf(fp,"nmotors %d\n", &noutputs);
		if (nneurons > MAXN)
			Logger::error( "Evonet - increase MAXN to support more than "+QString::number(MAXN)+" neurons" );
		nhiddens = nneurons - (ninputs + noutputs);
		fscanf(fp,"nblocks %d\n", &net_nblocks);
		for (b=0; b < net_nblocks; b++)
		{
			fscanf(fp,"%d %d %d %d %d %d", &net_block[b][0],&net_block[b][1],&net_block[b][2],&net_block[b][3],&net_block[b][4], &net_block[b][5]);
			if (net_block[b][0] == 0)
				fscanf(fp," // connections block\n");
			if (net_block[b][0] == 1)
				fscanf(fp," // block to be updated\n");
			if (net_block[b][0] == 2)
				fscanf(fp," // gain block\n");
			if (net_block[b][0] == 3)
				fscanf(fp," // modulated gain block\n");
		}

		fscanf(fp,"neurons bias, delta, gain, xy position, display\n");
		drawnxmax = 0;
		drawnymax = 0;
		for(n=0; n < nneurons; n++)
		{
			fscanf(fp,"%d %d %d %d %d %d\n", &neuronbias[n], &neurontype[n], &neurongain[n], &neuronxy[n][0], &neuronxy[n][1], &neurondisplay[n]);
			if(drawnxmax < neuronxy[n][0])
				drawnxmax = neuronxy[n][0];
			if(drawnymax < neuronxy[n][1])
				drawnymax = neuronxy[n][1];
		}
		drawnxmax += 30;
		drawnymax += 30;

		if (mode == 1)
		{
			fscanf(fp,"FREE PARAMETERS %d\n", &np);
			if (nparameters != np) {
				Logger::error(QString("ERROR: parameters defined are %1 while %2 contains %3 parameters").arg(nparameters).arg(filename).arg(np));
			}
			i = 0;
			ph = phep;
			mu = muts;
			p = freep;

			while (fgets(cbuffer,bufferSize,fp) != NULL && i < np)
			{
				//read values from line
				QString line = cbuffer;
				QStringList lineContent = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

				bool floatOnSecondPlace = false;
				lineContent[1].toFloat(&floatOnSecondPlace);

				if(lineContent.contains("*") || floatOnSecondPlace)
					readNewPheLine(lineContent, ph, mu);
				else
					readOldPheLine(lineContent, ph, mu);

				*p = *ph;

				i++;
				mu++;
				ph++;
				p++;
			}
			pheloaded = true;
		}
		fclose(fp);

		Logger::info( "Evonet - loaded file " + QString(filename) );
		return(1);
	}
	else
	{
		Logger::warning( "Evonet - File " + QString(filename) + " not found" );
		return(0);
	}
}

void Evonet::readOldPheLine(QStringList line, float* par, float* mut)
{
	*par = line[0].toFloat();

	if(*par != DEFAULT_VALUE) { //no mutations
		*mut = 0;
	}
}

void Evonet::readNewPheLine(QStringList line, float* par, float* mut)
{
	if(line[0] == "*") {
		*par = DEFAULT_VALUE; //start at random
	} else {
		//error handling

/*        char *tmp = line[0].toLatin1().data();
        printf("read : %s\n",line[0].toLatin1().data());
        printf("tmp : %s\n",tmp);

        for (int i = 0; i<line[0].length(); i++) {
            if (tmp[i]==',') {
                tmp[i] = '.';

            }
        }
*/
//        *par = strtof(tmp, NULL);



//        sscanf(tmp, "%f", par);
		*par = line[0].toFloat();
	}


	if(line[1] == "*") {
		*mut = DEFAULT_VALUE;
	} else {
		*mut = line[1].toFloat();
	}
}

/*
 * It save the architecture and also the parameters (when mode =1)
 */
void Evonet::save_net_blocks(const char *filename, int mode)
{
	FILE *fp;
	int b;
	int n;
	int i;
	int t;

	char* default_string = "*\t\t";
	char **p = new char*[freeParameters()];
	char **mu = new char*[freeParameters()];
	for(int h=0; h<freeParameters(); h++) {
		mu[h] = new char[50];
		p[h] = new char[50];

		if(muts[h] == DEFAULT_VALUE) {
			mu[h] = default_string;
		} else {
			sprintf(mu[h], "%f", muts[h]);
		}

		if(freep[h] == DEFAULT_VALUE) {
			p[h] = default_string;
		} else {
			sprintf(p[h], "%f", freep[h]);
		}
	}

	if ((fp = fopen(filename,"w")) != NULL) {
		fprintf(fp,"ARCHITECTURE\n");
		fprintf(fp,"nneurons %d\n", nneurons);
		fprintf(fp,"nsensors %d\n", ninputs);
		fprintf(fp,"nmotors %d\n", noutputs);
		fprintf(fp,"nblocks %d\n", net_nblocks);
		for (b = 0; b < net_nblocks; b++) {
			fprintf(fp,"%d %d %d %d %d %d", net_block[b][0],net_block[b][1],net_block[b][2],net_block[b][3],net_block[b][4],net_block[b][5]);
			if (net_block[b][0] == 0) {
				fprintf(fp," // connections block\n");
			} else if (net_block[b][0] == 1) {
				fprintf(fp," // block to be updated\n");
			} else if (net_block[b][0] == 2) {
				fprintf(fp," // gain block\n");
			} else if (net_block[b][0] == 3) {
				fprintf(fp," // modulated gain block\n");
			}
		}
		fprintf(fp,"neurons bias, delta, gain, xy position, display\n");
		for(n = 0; n < nneurons; n++) {
			fprintf(fp,"%d %d %d %d %d %d\n", neuronbias[n], neurontype[n], neurongain[n], neuronxy[n][0], neuronxy[n][1], neurondisplay[n]);
		}

		computeParameters();
		if (mode == 1) {
			fprintf(fp,"FREE PARAMETERS %d\n", nparameters);
			for(i = 0; i < nneurons; i++) {
				if (neurongain[i] == 1) {
					fprintf(fp,"%s \t %s \tgain %s\n",*p, *mu, neuronl[i]);
					p++;
					mu++;
				}
			}
			for(i=0; i<nneurons; i++) {
				if (neuronbias[i] == 1) {
					fprintf(fp,"%s \t %s \tbias %s\n",*p, *mu, neuronl[i]);
					p++;
					mu++;
				}
			}
			for (b=0; b < net_nblocks; b++) {
				if (net_block[b][0] == 0) {
					for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++) {
						for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++) {
							fprintf(fp,"%s \t %s \tweight %s from %s\n",*p, *mu, neuronl[t], neuronl[i]);
							p++;
							mu++;
						}
					}
				} else if (net_block[b][0] == 1) {
					for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++) {
						if (neurontype[t] == 1) {
							float timeC = 0;
							if(*p != default_string) {
								timeC = atof(*p);
								timeC = fabs(timeC)/wrange;  //(timeC + wrange)/(wrange*2);
							}

							fprintf(fp,"%s \t %s \ttimeconstant %s (%f)\n", *p, *mu, neuronl[t], timeC);
							p++;
							mu++;
						}
					}
				}
			}
		}
		fprintf(fp,"END\n");

		Logger::info( "Evonet - controller saved on file " + QString(filename) );
	} else {
		Logger::error( "Evonet - unable to create the file " + QString(filename) );
	}
	fclose(fp);
}

/*
 * standard logistic
 */
float Evonet::logistic(float f)
{
	return((float) (1.0 / (1.0 + exp(0.0 - f))));
}

/*
 * compute the number of free parameters
 */
void Evonet::computeParameters()
{
	int i;
	int t;
	int b;
	int updated[MAXN];
	int ng;
	int nwarnings;

	ng  = 0;
	for(i=0;i < nneurons;i++) {
		updated[i] = 0;
	}
	// gain
	for(i=0;i < nneurons;i++) {
		if (neurongain[i] == 1) {
			ng++;
		}
	}
	// biases
	for(i=0;i < nneurons;i++) {
		if (neuronbias[i] == 1) {
			ng++;
		}
	}
	// timeconstants
	for(i=0;i < nneurons;i++) {
		if (neurontype[i] == 1) {
			ng++;
		}
	}
	// blocks
	for (b=0; b < net_nblocks; b++) {
		// connection block
		if (net_block[b][0] == 0) {
			for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++) {
				for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++) {
					ng++;
				}
			}
		}
	}

	nwarnings = 0;
	for(i=0;i < nneurons;i++) {
		if (updated[i] < 1 && nwarnings == 0) {
			Logger::warning( "Evonet - neuron " + QString::number(i) + " will never be activated according to the current architecture" );
			nwarnings++;
		}
		if (updated[i] > 1 && nwarnings == 0) {
			Logger::warning( "Evonet - neuron " + QString::number(i) + " will be activated more than once according to the current architecture" );
			nwarnings++;
		}
	}
	nparameters=ng; // number of parameters
}

QPair<AbstractControllerInputIterator*, AbstractControllerOutputIterator*> Evonet::createIterators()
{
	m_evonetIterator = new EvonetIterator(this);

	return QPair<AbstractControllerInputIterator*, AbstractControllerOutputIterator*>(m_evonetIterator, m_evonetIterator);
}

int Evonet::getIndexForControllerInput(AbstractControllerInput* input)
{
	const int index = m_evonetIterator->defineBlock(EvonetIterator::InputLayer, m_inputCurIndex, input->size());

	m_inputCurIndex += input->size();

	return index;
}

int Evonet::getIndexForControllerOutput(AbstractControllerOutput* output)
{
	const int index = m_evonetIterator->defineBlock(EvonetIterator::OutputLayer, m_outputCurIndex, output->size());

	m_outputCurIndex += output->size();

	return index;
}

int Evonet::computeNumSensorsFromControllerInputsList() const
{
	int n = 0;
	const ControllerInputsList* const l = controllerInputsList();
	for (int i = 0; i < l->numControllerInputs(); ++i) {
		n += l->getControllerInput(i)->size();
	}
	return n;
}

int Evonet::computeNumMotorsFromControllerOutputsList() const
{
	int n = 0;
	const ControllerOutputsList* const l = controllerOutputsList();
	for (int i = 0; i < l->numControllerOutputs(); ++i) {
		n += l->getControllerOutput(i)->size();
	}
	return n;
}

void Evonet::updateNet()
{
	int i;
	int t;
	int b;
	float *p;
	float delta;
	float netinput[MAXN];
	float gain[MAXN];

	p  = freep;
	//nl  = neuronlesion;

	// gain
	for(i=0;i < nneurons;i++) {
		if (neurongain[i] == 1) {
			gain[i] = (float) (fabs((double) *p) / wrange) * grange;
			p++;
		} else {
			gain[i] = 1.0f;
		}
	}
	// biases

/*    printf("weights: ");
    printWeights();
    printf("\n");
*/
	for(i=0;i < nneurons;i++) {
		if (neuronbias[i] == 1) {
//            printf("netinput[%d]= [%ld] %f/%f*%f = %f*%f = %f",i,p-freep, *p,wrange,brange,((double)*p/wrange),brange,((double)*p/wrange)*brange);
			netinput[i] = ((double)*p/wrange)*brange;
			p++;
		} else {
			netinput[i] = 0.0f;
		}
	}

	// blocks
	for (b=0; b < net_nblocks; b++) {
		// connection block
		if (net_block[b][0] == 0) {
			for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++) {
				for(i=net_block[b][3]; i < net_block[b][3] + net_block[b][4];i++) {
					netinput[t] += act[i] * gain[i] * *p;
//                    printf("netinput[%d] += act[%d] * gain[%d] * %f += %f * %f * %f += %f = %f\n",t,i,i,*p,act[i],gain[i], *p,act[i] * gain[i] * *p, netinput[t] );
					p++;
				}
			}
		}
		// gain block (gain of neuron a-b set equal to gain of neuron a)
		if (net_block[b][0] == 2) {
			for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++) {
				gain[t] = gain[net_block[b][1]];
			}
		}
		// gain block (gain of neuron a-b set equal to act[c])
		if (net_block[b][0] == 3) {
			for(t=net_block[b][1]; t < net_block[b][1] + net_block[b][2];t++) {
				gain[t] = act[net_block[b][3]];
			}
		}
		// update block
		if (net_block[b][0] == 1) {
			for(t=net_block[b][1]; t < (net_block[b][1] + net_block[b][2]); t++) {
				if (t < ninputs) {
					switch(neurontype[t]) {
						case 0: // simple rely units
							act[t] = input[t];
							break;
						case 1: // delta neurons
							delta = (float) (fabs((double) *p) / wrange);
							p++;
							act[t] = (act[t] * delta)  + (input[t] * (1.0f - delta));
							// Check whether activation is within range [0,1]
							if (act[t] < 0.0)
							{
								act[t] = 0.0;
							}
							if (act[t] > 1.0)
							{
								act[t] = 1.0;
							}
						break;
					}
					if(neuronlesions > 0 && neuronlesion[t]) {
						act[t]= (float)neuronlesionVal[t];
					}
				} else {
					switch(neurontype[t]) {
						case 0: // simple logistic
						default:
							act[t] = logistic(netinput[t]);
							delta = 0.0;
							break;
						case 1: // delta neurons
							delta = (float) (fabs((double) *p) / wrange);
							p++;
							act[t] = (act[t] * delta)  + (logistic(netinput[t]) * (1.0f - delta));
							// Check whether activation is within range [0,1]
							if (act[t] < 0.0)
							{
								act[t] = 0.0;
							}
							if (act[t] > 1.0)
							{
								act[t] = 1.0;
							}
							break;
						case 2: // binary neurons
							if (netinput[t] >= 0.0) {
								act[t] = 1.0;
							} else {
								act[t] = 0.0;
							}
							break;
						case 3: // logistic2 neurons
							act[t] = logistic(netinput[t]*0.2f);
							delta = 0.0;
							break;
					}
					if(neuronlesions > 0 && neuronlesion[t]) {
						act[t]= (float)neuronlesionVal[t];
					}
				}
			}
		}
	}

	// Storing the current activations
	memcpy(storedActivations[nextStoredActivation], act, nneurons * sizeof(float));
	nextStoredActivation = (nextStoredActivation + 1) % MAXSTOREDACTIVATIONS;
	if (firstStoredActivation == nextStoredActivation) {
		// We have filled the circular buffer, discarding the oldest activation
		firstStoredActivation = (firstStoredActivation + 1) % MAXSTOREDACTIVATIONS;
	}

	// increment the counter
	updatescounter++;

	// If a downloader is associated with the neuronsMonitorUploader, uploading activations
	if (neuronsMonitorUploader.downloaderPresent() && updateMonitor) {
		// This call can return NULL if GUI is too slow
		DatumToUpload<ActivationsToGui> d(neuronsMonitorUploader);

		d->activations = true;

		// Reserving the correct number of elements, for efficiency reasons
		d->data.reserve(nneurons);
		d->data.resize(nneurons);

		// Copying data
		std::copy(act, &(act[nneurons]), d->data.begin());

		// Adding the current step
		d->updatesCounter = updatescounter;

		if (updateNeuronMonitor) {
			updateNeuronMonitor = false;

			// we also send labels and colors and tell the gui to update them
			d->updateLabelAndColors = true;

			// Copying labels
			d->neuronl.resize(nneurons);
			for (int i = 0; i < nneurons; ++i) {
				d->neuronl[i] = neuronl[i];
			}

			// Copying colors
			d->neurondcolor.resize(nneurons);
			for (int i = 0; i < nneurons; ++i) {
				d->neurondcolor[i] = neurondcolor[i];
			}
		} else {
			d->updateLabelAndColors = false;
		}
	}
}

int Evonet::setInput(int inp, float value)
{
	if (inp>=ninputs || inp<0) {
		return -1;// exceding sensor number
	}
	input[inp]=value;
	return 0;
}

float Evonet::getOutput(int out)
{
	if(out>=noutputs) {
		return -1; //exceeding out numbers
	}
	return act[ninputs+nhiddens+out];
}

float Evonet::getInput(int in)
{
	return this->input[in];
}

float Evonet::getNeuron(int in)
{
	return act[in];
}

void Evonet::resetNet()
{
	int i;
	for (i = 0; i < MAXN; i++) {
		act[i]=0.0;
		netinput[i]=0.0;
		input[i]=0.0;
	}
	updatescounter = 0;
}

void Evonet::injectHidden(int nh, float val)
{
	if(nh<nhiddens) {
		act[this->ninputs+nh] = val;
	}
}

float Evonet::getHidden(int h)
{
	if(h<nhiddens && h>=0) {
		return act[this->ninputs+h];
	} else {
		return -999;
	}
}

int Evonet::freeParameters()
{
	return this->nparameters;
}

float Evonet::getFreeParameter(int i)
{
	return freep[i];
}

bool Evonet::pheFileLoaded()
{
	return pheloaded;
}

/*
 * Copy parameters from genotype
 */
void Evonet::setParameters(const float *dt)
{
	int i;
	float *p;

	p = freep;
	for (i=0; i<freeParameters(); i++, p++) {
		*p = dt[i];
	}
}

void Evonet::setParameters(const int *dt)
{
	int i;
	float *p;

	p = freep;
	for (i=0; i<freeParameters(); i++, p++) {
		*p = wrange - ((float)dt[i]/geneMaxValue)*wrange*2;
	}
}

void Evonet::getMutations(float* GAmut)
{
	//copy mutation vector
	for(int i=0; i<freeParameters(); i++) {
		GAmut[i] = muts[i];
	}
}

void Evonet::copyPheParameters(int* pheGene)
{
	for(int i=0; i<freeParameters(); i++)
	{
		if(phep[i] == DEFAULT_VALUE) {
			pheGene[i] = DEFAULT_VALUE;
		} else {
			pheGene[i] = (int)((wrange - phep[i])*geneMaxValue/(2*wrange));
		}
	}
}

void Evonet::printIO()
{
	QString output;

	output = "In: ";
	for (int in = 0; in < this->ninputs; in++) {
		output += QString("%1 ").arg(this->input[in], 0, 'f', 10);
	}
	output += "Hid: ";
	for (int hi = this->ninputs; hi < (this->nneurons - this->noutputs); hi++) {
		output += QString("%1 ").arg(this->act[hi], 0, 'f', 10);
	}
	output += "Out: ";
	for (int out = 0; out < this->noutputs; out++) {
		output += QString("%1 ").arg(this->act[this->ninputs+this->nhiddens+out], 0, 'f', 10);
	}

	Logger::info(output);

}

int Evonet::getParamBias(int nbias)
{
	int pb=-999; // if remain -999 it means nbias is out of range
	if (nbias<nparambias && nbias>-1) {
		pb=(int) freep[nparambias+nbias];
	}
	return pb;
}

float Evonet::getWrange()
{
	return wrange;
}

float Evonet::getBrange()
{
	return brange;
}

float Evonet::getGrange()
{
	return grange;
}


void Evonet::printBlocks()
{
	Logger::info("Evonet - ninputs " + QString::number(this->ninputs));
	Logger::info("Evonet - nhiddens " + QString::number(this->nhiddens));
	Logger::info("Evonet - noutputs " + QString::number(this->noutputs));
	Logger::info("Evonet - nneurons " + QString::number(this->nneurons));

	for(int i=0;i<this->net_nblocks;i++) {
		Logger::info( QString( "Evonet Block - %1 | %2 - %3 -> %4 - %5 | %6" )
					.arg(net_block[i][0])
					.arg(net_block[i][1])
					.arg(net_block[i][2])
					.arg(net_block[i][3])
					.arg(net_block[i][4])
					.arg(net_block[i][5]));
	}
}

int Evonet::getNoInputs()
{
	return ninputs;
}

int Evonet::getNoHiddens()
{
	return nhiddens;
}

int Evonet::getNoOutputs()
{
	return noutputs;
}

int Evonet::getNoNeurons()
{
	return nneurons;
}

void Evonet::setRanges(double weight, double bias, double gain)
{
	wrange=weight;
	brange=bias;
	grange=gain;
}

float* Evonet::getOldestStoredActivations()
{
	if (firstStoredActivation == nextStoredActivation) {
		return NULL;
	}

	const int ret = firstStoredActivation;
	firstStoredActivation = (firstStoredActivation + 1) % MAXSTOREDACTIVATIONS;
	return storedActivations[ret];
}

int Evonet::updateCounts()
{
	return updatescounter;
}

    void Evonet::activateMonitorUpdate(){
        updateMonitor = true;
    }

    void Evonet::deactivateMonitorUpdate(){
        updateMonitor = false;
    }


} // end namespace salsa


// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
