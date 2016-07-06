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

#include "libperiodicfunctions.h"
#include "configurationhelper.h"
#include "mathutils.h"
#include <cmath>

namespace farsa {

PeriodicFunction::PeriodicFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params, prefix, parent),
	phase(0.0),
	span(1.0),
	amplitude(1.0) {
}

void PeriodicFunction::configure() {
	phase = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"phase", 0.0 );
	span = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"span", 1.0 );
	amplitude = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"amplitude", 1.0 );
	markAsConfigured();
}

void PeriodicFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	// Here we call startObjectParameters even if this is an abstract class
	// because it creates the group
	params.startObjectParameters(prefix, typeName(), this);
	params.createParameter(prefix, "phase", QString::number(phase));
	params.createParameter(prefix, "span", QString::number(span));
	params.createParameter(prefix, "amplitude", QString::number(amplitude));
}

void PeriodicFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A PeriodicFunction" );
	d.describeReal("phase").def(0.0).help("The phase expressed in radians of the periodic function");
	d.describeReal("span").def(1.0).help("The extension on 'x' axis of the period of the function");
	d.describeReal("amplitude").def(1.0).help("The maximum amplitude on 'y' axis of the periodic function");
}

void PeriodicFunction::setCoefficients( double phase, double span, double amplitude ) {
	this->phase = phase;
	this->span = span;
	this->amplitude = amplitude;
}

double PeriodicFunction::getPhase() {
	return phase;
}

double PeriodicFunction::getSpan() {
	return span;
}

double PeriodicFunction::getAmplitude() {
	return amplitude;
}

SawtoothFunction::SawtoothFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: PeriodicFunction(params,prefix,parent) {
}

void SawtoothFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	// --- out <- 2.0*( (x-c)/a-floor((x-c)/a+0.5) )
	for( int i=0; i<(int)inputs.size(); i++ ) {
		outputs[i] = amplitude*( (inputs[i]-phase)/span-floor((inputs[i]-phase)/span+0.5) );
	}
}

TriangleFunction::TriangleFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: PeriodicFunction(params,prefix,parent) {
}

void TriangleFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	// --- out <- 2.0*( (x-c)/a-floor((x-c)/a+0.5) )
	for( int i=0; i<inputs.size(); i++ ) {
		double sawtooth = (inputs[i]-phase)/span-floor((inputs[i]-phase)/span+0.5);
		outputs[i] = amplitude*( 1.0 - fabs( sawtooth ) );
	}
}

SinFunction::SinFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: PeriodicFunction(params,prefix,parent) {
}

double SinFunction::getFrequency() {
	return 2.0*PI_GRECO/span;
}

void SinFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	for( int i=0; i<(int)inputs.size(); i++ ) {
		outputs[i] = amplitude*sin(2.0*PI_GRECO*(inputs[i]/span)-PI_GRECO*phase);
	}
}

PseudoGaussFunction::PseudoGaussFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: PeriodicFunction(params,prefix,parent) {
}

void PseudoGaussFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = 0.5*amplitude*( sin( 2.0*PI_GRECO*((inputs[i]-phase)/span+0.25) ) + 1.0 );
	}
}

}

