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

#include "libcompetitivefunctions.h"
#include "configurationhelper.h"

namespace farsa {

WinnerTakeAllFunction::WinnerTakeAllFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	valuev(1.0) {
}

bool WinnerTakeAllFunction::setValue( double v ) {
	valuev = v;
	return true;
}

double WinnerTakeAllFunction::value() {
	return valuev;
}

void WinnerTakeAllFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	outputs.setZero();
	int index = 0;
	inputs.maxCoeff(&index);
	outputs[ index ] = valuev;
}

void WinnerTakeAllFunction::configure() {
	valuev = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"value", 1.0);
	markAsConfigured();
}

void WinnerTakeAllFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "WinnerTakeAllFunction", this);
	params.createParameter(prefix, "value", QString::number(valuev));
}

void WinnerTakeAllFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Winner Take All Output function" );
	d.describeReal( "value" ).def(1.0).limits(1, +Infinity).help("The output value assumed by the winner (the most activated neuron); all other neurons will have 0.0 as output value");
}

}

