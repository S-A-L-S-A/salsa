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

#include "biasedcluster.h"
#include "liboutputfunctions.h"
#include "randomgenerator.h"
#include "configurationhelper.h"

using namespace Eigen;

namespace salsa {

BiasedCluster::BiasedCluster( ConfigurationManager& params, QString prefix, Component* parent ) :
	Cluster( params, prefix, parent ),
	biasesdata(VectorXd::Zero(numNeurons())),
	tempdata(VectorXd::Zero(numNeurons())) {
	//--- create the delegate for biases
	setDelegateFor<BiasedCluster, &BiasedCluster::biases>( "biases" );

	QString vectorSizeErrorTmpl( "The number of elements of the %1 vector in configuration file (%2) is different from the number of neurons (%3)");
	// biases is a vector, that is a list of space-separated values
	QVector<double> vect = ConfigurationHelper::getVector( params, prefix + "biases" );
	if ( !vect.isEmpty() && vect.size() != (int)numNeurons() ) {
		ConfigurationHelper::throwUserConfigError("biases", QString::number(vect.size()),
												  vectorSizeErrorTmpl.arg( "biases" ).arg( vect.size() ).arg( numNeurons() ) );
	}
	biasesdata = Map<VectorXd>(vect.data(), numNeurons());
	markAsConfigured();
}

BiasedCluster::~BiasedCluster() {
}

void BiasedCluster::update() {
	tempdata = inputs()-biasesdata;
	outFunction()->apply( tempdata, outputs() );
	setNeedReset( true );
}

void BiasedCluster::setBias( unsigned int neuron, double bias ) {
	biasesdata[neuron] = bias;
}

void BiasedCluster::setAllBiases( double bias ) {
	biasesdata.setConstant( bias );
}

void BiasedCluster::setBiases( const DoubleVector& bias ) {
	biasesdata = bias;
}

double BiasedCluster::getBias( unsigned int neuron ) {
	return biasesdata[neuron];
}

void BiasedCluster::randomize( double min, double max ) {
	for ( unsigned int i = 0; i < numNeurons(); i++ ) {
		biasesdata[i] = globalRNG->getDouble( min, max );
	}
}

void BiasedCluster::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	Cluster::save();
	params.startObjectParameters(prefix, "BiasedCluster", this);
	// First creating a string list, then transforming to a single string
	QStringList list;
	for (int i = 0; i < biasesdata.size(); i++) {
		list.push_back(QString::number(biasesdata[i]));
	}
	params.createParameter(prefix, "biases", list.join(" "));
}

void BiasedCluster::describe( QString type ) {
	Cluster::describe( type );
	Descriptor d = addTypeDescription( type, "A Cluster where neurons have also a bias value", "The bias values are subtracted from the input values before the calculation of the output" );
	d.describeReal( "biases" ).props( ParamIsList ).help( "The vector of bias values. It must contains numNeurons elements" );
}

}
