/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "cluster.h"
#include "liboutputfunctions.h"
#include "configurationhelper.h"
#include <exception>

using namespace Eigen;

namespace farsa {

Cluster::Cluster( ConfigurationManager& params, QString prefix, Component* parent ) :
	Updatable(params, prefix, parent),
	numneurons(ConfigurationHelper::getInt( params, prefix + "numNeurons", 1 )),
	inputdata(VectorXd::Zero(numneurons)),
	outputdata(VectorXd::Zero(numneurons)),
	stateDelegates()
{
	//--- create the delegates for inputs and outputs vector
	setDelegateFor<Cluster, &Cluster::inputs>( "inputs" );
	setDelegateFor<Cluster, &Cluster::outputs>( "outputs" );

	accOff = !ConfigurationHelper::getBool( params, prefix + "accumulate" );

	QString vectorSizeErrorTmpl( "The number of elements of the %1 vector in configuration file (%2) is different from the number of neurons (%3)");
	// inputs is a vector, that is a list of space-separated values
	QVector<double> vect = ConfigurationHelper::getVector( params, prefix + "inputs" );
	if ( !vect.isEmpty() && vect.size() != (int)numneurons ) {
		ConfigurationHelper::throwUserConfigError("inputs", QString::number(vect.size()),
												  vectorSizeErrorTmpl.arg( "inputs" ).arg( vect.size() ).arg( numneurons ) );
	}
	inputdata = Eigen::Map<VectorXd>(vect.data(), numneurons);

	// outputs is a vector, that is a list of space-separated values
	vect = ConfigurationHelper::getVector( params, prefix + "outputs" );
	if ( !vect.isEmpty() && vect.size() != (int)numneurons ) {
		ConfigurationHelper::throwUserConfigError("outputs", QString::number(vect.size()),
												  vectorSizeErrorTmpl.arg( "outputs" ).arg( vect.size() ).arg( numneurons ) );
	}
	outputdata = Eigen::Map<VectorXd>(vect.data(), numneurons);

	// The group for the outputFunction is "prefix/OutFunction"
	setOutFunction(params.getObjectFromGroup<OutputFunction>( prefix + "OutFunction", true ));
	setNeedReset( false );
	markAsConfigured();
}

Cluster::~Cluster() {
	// No need to delete anything else, we use auto_ptr
}

void Cluster::setOutFunction( OutputFunction *up ) {
	updater.reset(up);
	updater->setCluster( this );
}

void Cluster::setInput( unsigned int neuron, double value ) {
	inputdata[neuron] = value;
}

void Cluster::setInputs( const DoubleVector& inputs ) {
	inputdata = inputs;
}

void Cluster::setAllInputs( double value ) {
	inputdata.setConstant( value );
	setNeedReset( false );
}

void Cluster::resetInputs() {
	inputdata.setZero();
	setNeedReset( false );
}

double Cluster::getInput( unsigned int neuron ) const {
	return inputdata[neuron];
}

void Cluster::setOutput( unsigned int neuron, double value ) {
	outputdata[neuron] = value;
}

void Cluster::setOutputs( const DoubleVector& outputs ) {
	outputdata = outputs;
}

double Cluster::getOutput( unsigned int neuron ) const {
	return outputdata[neuron];
}

void Cluster::save()
{
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	Updatable::save();
	params.startObjectParameters(prefix, "Cluster", this);
	params.createParameter(prefix, "numNeurons", QString::number(numneurons));
	params.createParameter(prefix, "accumulate", (isAccumulate() ? "True" : "False"));
	// First creating a string list, then transforming to a single string
	QStringList list;
	for (int i = 0; i < inputdata.size(); i++) {
		list.push_back(QString::number(inputdata[i]));
	}
	params.createParameter(prefix, "inputs", list.join(" "));
	// Doing the same with outputdata
	list.clear();
	for (int i = 0; i < outputdata.size(); i++) {
		list.push_back(QString::number(outputdata[i]));
	}
	params.createParameter(prefix, "outputs", list.join(" "));
	// and finally the outfunction will be saved in the group "prefix/OutFunction"
	updater->save();
}

void Cluster::describe( QString type ) {
	Updatable::describe( type );
	Descriptor d = addTypeDescription( type, "A Cluster of neurons" );
	d.describeInt( "numNeurons" ).limits( 1, MaxInteger ).props( ParamIsMandatory ).help( "The number of neurons contained by the Cluster" );
	d.describeBool( "accumulate" ).def( false ).help( "If true new inputs will be added to the previous values" );
	d.describeReal( "inputs" ).props( ParamIsList ).help( "The input values of the neurons" );
	d.describeReal( "outputs" ).props( ParamIsList ).help( "The output values of the neurons" );
	d.describeSubgroup( "OutFunction" ).type( "OutputFunction" ).props( ParamIsMandatory ).help( "The output function used to calculate the output values" );
}

}
