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

#include "libradialfunctions.h"
#include "configurationhelper.h"

namespace farsa {

GaussFunction::GaussFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	centre(0.0),
	max(1.0),
	variancev(1.0) {
	msqrvar = -( variancev*variancev );
}

void GaussFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	// --- out <- max * exp( (centre-inputs)^2 / -(variance^2) )
	for( int i=0; i<inputs.size(); i++ ) {
		double diffsqr = (centre-inputs[i])*(centre-inputs[i]);
		outputs[i] = max * exp( diffsqr / msqrvar );
	}
}

bool GaussFunction::derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const {
    // --- d <- ( 2.0*(centre-x) / variance^2 ) * y
	for( int i=0; i<x.size(); i++ ) {
		d[i] = ( 2.0*(centre-x[i]) / (variancev*variancev) )*y[i];
	}
	return true;
}

void GaussFunction::configure() {
	centre = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"centre", 0.0);
	variancev = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"variance", 1.0);
	// Also recomputing minus squared variance
	msqrvar = -( variancev*variancev );
	max = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"max", 1.0);
}

void GaussFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "GaussFunction", this);
	params.createParameter(prefix, "centre", QString::number(centre));
	params.createParameter(prefix, "variance", QString::number(variancev));
	params.createParameter(prefix, "max", QString::number(max));
}

void GaussFunction::describe( QString type ) {
	Descriptor d = addTypeDescription(type, "A Gaussian Function");
	d.describeReal("centre").def(0.0).help("The center of the gaussian function, where the output value is maximum");
	d.describeReal("variance").def(1.0).help("The variance of the gaussian function");
	d.describeReal("max").def(1.0).help("The maximum output value of the gaussian function");
}

void GaussFunction::setCoefficients( double max, double variance, double centre ) {
	this->max = max;
	this->variancev = variance;
	this->centre = centre;
	msqrvar = -( variancev*variancev );
}

double GaussFunction::getVariance() {
	return variancev;
}

double GaussFunction::getCentre() {
	return centre;
}

double GaussFunction::getMax() {
	return max;
}

}
