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

#include "liboutputfunctions.h"
#include "cluster.h"
#include "configurationhelper.h"
#include <QStringList>
#include <QRegExp>
#include <cmath>

using namespace Eigen;

namespace salsa {

IdentityFunction::IdentityFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent) {
	markAsConfigured();
}

void IdentityFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	outputs = inputs;
}

bool IdentityFunction::derivate( const DoubleVector&, const DoubleVector&, DoubleVector& derivates ) const {
    derivates.setConstant( 1.0 );
	return true;
}

void IdentityFunction::save()
{
	// Just telling our type to ConfigurationParameters
	configurationManager().startObjectParameters(prefixPath(), "IdentityFunction", this);
}

void IdentityFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "An Identity Function" );
}

ScaleFunction::ScaleFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	rate(1.0f) {
}

void ScaleFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	outputs = inputs * rate;
}

bool ScaleFunction::derivate( const DoubleVector&, const DoubleVector&, DoubleVector& derivates ) const {
    derivates.setConstant( rate );
	return true;
}

void ScaleFunction::configure() {
	rate = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"rate", 1.0 );
	markAsConfigured();
}

void ScaleFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "ScaleFunction", this);
	params.createParameter(prefix, "rate", QString::number(rate));
}

void ScaleFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Scale Function" );
	d.describeReal("rate").def(1.0).help("The coefficient of the scaling");
}

void ScaleFunction::setRate( double rate ) {
	this->rate = rate;
}

double ScaleFunction::getRate() {
	return rate;
}

GainFunction::GainFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	gainv(1.0) {
}

void GainFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = inputs[i] + gainv;
	}
}

bool GainFunction::derivate( const DoubleVector&, const DoubleVector&, DoubleVector& derivates ) const {
    derivates.setConstant( 1.0 );
	return true;
}

void GainFunction::configure() {
	gainv = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"gain", 1.0 );
	markAsConfigured();
}

void GainFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "GainFunction", this);
	params.createParameter(prefix, "gain", QString::number(gainv));
}

void GainFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Gain Function" );
	d.describeReal("gain").def(1.0).help("The coefficient of the gain");
}

void GainFunction::setGain( double gain ) {
	this->gainv = gain;
}

double GainFunction::getGain() {
	return gainv;
}

SigmoidFunction::SigmoidFunction( ConfigurationManager& params, QString prefix, Component* parent ) :
	OutputFunction(params, prefix, parent),
	lambda(1.0) {
}

void SigmoidFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	// ____________1_________________
	//   exp( -lamba*inputs ) + 1
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = 1.0/(exp(-lambda*inputs[i])+1.0);
	}
}

bool SigmoidFunction::derivate( const DoubleVector&, const DoubleVector& outputs, DoubleVector& derivates ) const {
	// derivates <- lambda * out * (1.0-out)
	for( int i=0; i<outputs.size(); i++ ) {
		derivates[i] = lambda * outputs[i] * (1.0-outputs[i]);
	}
	return true;
}

void SigmoidFunction::configure() {
	lambda = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"lambda", 1.0 );
	markAsConfigured();
}

void SigmoidFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "SigmoidFunction", this);
	params.createParameter(prefix, "lambda", QString::number(lambda));
}

void SigmoidFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Sigmoid Function" );
	d.describeReal("lambda").def(1.0).help("The lambda coefficient of the sigmoid function");
}

void SigmoidFunction::setLambda( double lambda ) {
	this->lambda = lambda;
}

double SigmoidFunction::getLambda() {
	return lambda;
}

FakeSigmoidFunction::FakeSigmoidFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params, prefix,parent),
	lambda(1.0) {
}

void FakeSigmoidFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	unsigned int size = inputs.size();
	double x;
	double x0 = 6. + 2./3.;
	double zero = 0.5;
	for ( unsigned int i = 0; i<size; i++ ) {
		x = inputs[i];
		x *= lambda;
		x -= (.5 - zero) / (.075 + zero);
		if ( x <= -x0 ) {
			outputs[i] = 0.0;
		} else {
			if ( x < x0 ) {
				outputs[i] = .5 + .575 * x / ( 1.0 + fabs(x) );
			} else {
				outputs[i] = 1.0;
			}
		}
	}
}

bool FakeSigmoidFunction::derivate( const DoubleVector&, const DoubleVector& outputs, DoubleVector& derivates ) const {
	// derivates <- lambda * out * (1.0-out)
	for( int i=0; i<outputs.size(); i++ ) {
		derivates[i] = lambda * outputs[i] * (1.0-outputs[i]);
	}
	return true;
}

void FakeSigmoidFunction::configure() {
	lambda = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"lambda", 1.0 );
	markAsConfigured();
}

void FakeSigmoidFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "FakeSigmoidFunction", this);
	params.createParameter(prefix, "lambda", QString::number(lambda));
}

void FakeSigmoidFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Sigmoid Function computed used a linear approximation" );
	d.describeReal("lambda").def(1.0).help("The lambda coefficient of the sigmoid function");
}

void FakeSigmoidFunction::setLambda( double lambda ) {
	this->lambda = lambda;
}

double FakeSigmoidFunction::getLambda() {
	return lambda;
}

ScaledSigmoidFunction::ScaledSigmoidFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	lambda(1.0),
	min(-1.0),
	max(+1.0) {
}

void ScaledSigmoidFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	//--- compute the sigmoid
	// ____________1_________________
	//   exp( -lamba*inputs ) + 1
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = 1.0/(exp(-lambda*inputs[i])+1.0);
		//--- and scale it
		outputs[i] = (max-min)*outputs[i]+min;
	}
}

bool ScaledSigmoidFunction::derivate( const DoubleVector&, const DoubleVector& outputs, DoubleVector& derivates ) const {
	// derivates <- lambda * out * (1.0-out)
	for( int i=0; i<outputs.size(); i++ ) {
		derivates[i] = lambda * outputs[i] * (1.0-outputs[i]);
	}
	return true;
}

void ScaledSigmoidFunction::configure() {
	lambda = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"lambda", 1.0 );
	min = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"min", -1.0 );
	max = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"max", 1.0 );
	markAsConfigured();
}

void ScaledSigmoidFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "ScaledSigmoidFunction", this);
	params.createParameter(prefix, "lambda", QString::number(lambda));
	params.createParameter(prefix, "min", QString::number(min));
	params.createParameter(prefix, "max", QString::number(max));
}

void ScaledSigmoidFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Sigmoid Function that can be scaled in an arbitrary [min,max] range" );
	d.describeReal("lambda").def(1.0).help("The lambda coefficient of the sigmoid function");
	d.describeReal("min").def(-1.0).help("It is the y value when x -> -infinite");
	d.describeReal("max").def(+1.0).help("It is the y value when x -> -infinite");
}

void ScaledSigmoidFunction::setCoefficients( double lambda, double min, double max ) {
	this->lambda = lambda;
	this->min = min;
	this->max = max;
}

double ScaledSigmoidFunction::getLambda() {
	return lambda;
}

double ScaledSigmoidFunction::getMin() {
	return min;
}

double ScaledSigmoidFunction::getMax() {
	return max;
}

RampFunction::RampFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	min_x(-1.0),
	max_x(+1.0),
	min_y(-1.0),
	max_y(+1.0) {
}

void RampFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	unsigned int size = inputs.size();
	double m = ( max_y-min_y )/( max_x-min_x );
	double q = min_y - m*min_x;
	for ( unsigned int i = 0; i<size; i++ ) {
		double ret = m*(inputs[i]) + q;
		if (ret < min_y) {
			outputs[i] = min_y;
		} else if (ret > max_y) {
			outputs[i] = max_y;
		} else {
			outputs[i] = ret;
		}
	}
}

bool RampFunction::derivate( const DoubleVector& inputs, const DoubleVector&, DoubleVector& derivates ) const {
	for( int i=0; i<inputs.size(); i++ ) {
		if ( inputs[i] >= min_x && inputs[i] <= max_x ) {
			derivates[i] = ( max_y-min_y )/( max_x-min_x );
		} else {
			double y;
			y = 1.0/( 1.0 + std::exp( -inputs[i] ) );
			derivates[i] = y * ( 1.0 - y );
		}
	}
	return true;
}

void RampFunction::configure() {
	min_x = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"minX", -1.0 );
	max_x = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"maxX", +1.0 );
	min_y = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"minY", -1.0 );
	max_y = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"maxY", +1.0 );
	markAsConfigured();
}

void RampFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "RampFunction", this);
	params.createParameter(prefix, "minX", QString::number(min_x));
	params.createParameter(prefix, "maxX", QString::number(max_x));
	params.createParameter(prefix, "minY", QString::number(min_y));
	params.createParameter(prefix, "maxY", QString::number(max_y));
}

void RampFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Ramp Function" );
	d.describeReal("minX").def(-1.0).help("It is the minimum X value allowed");
	d.describeReal("maxX").def(+1.0).help("It is the maximum X value allowed");
	d.describeReal("minY").def(-1.0).help("It is the minimum Y value allowed");
	d.describeReal("maxY").def(+1.0).help("It is the maximum Y value allowed");
}

void RampFunction::setCoefficients( double min_x, double max_x, double min_y, double max_y ) {
	this->min_x = min_x;
	this->max_x = max_x;
	this->min_y = min_y;
	this->max_y = max_y;
}

double RampFunction::getMinX() {
	return min_x;
}

double RampFunction::getMaxX() {
	return max_x;
}

double RampFunction::getMinY() {
	return min_y;
}

double RampFunction::getMaxY() {
	return max_y;
}

LinearFunction::LinearFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	m(1.0),
	b(0.0) {
}

void LinearFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = m*inputs[i]+b;
	}
}

bool LinearFunction::derivate( const DoubleVector& , const DoubleVector&, DoubleVector& derivates ) const {
	derivates.setConstant( m );
	return true;
}

void LinearFunction::configure() {
	m = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"m", 1.0 );
	b = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"b", 0.0 );
	markAsConfigured();
}

void LinearFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "LinearFunction", this);
	params.createParameter(prefix, "m", QString::number(m));
	params.createParameter(prefix, "b", QString::number(b));
}

void LinearFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Linear Function y=m*x+b" );
	d.describeReal("m").def(-1.0).help("The m coefficient of linear equation y=m*x+b");
	d.describeReal("b").def(+1.0).help("The b coefficient of linear equation y=m*x+b");
}

void LinearFunction::setCoefficients( double m, double b ) {
	this->m = m;
	this->b = b;
}

double LinearFunction::getM() {
	return m;
}

double LinearFunction::getB() {
	return b;
}

StepFunction::StepFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	min(0.0),
	max(1.0),
	threshold(0.0) {
}

void StepFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	for( int i = 0; i<inputs.size(); i++ ) {
		( inputs[i] > threshold ) ? outputs[i] = max : outputs[i] = min;
	}
}

bool StepFunction::derivate( const DoubleVector& inputs, const DoubleVector&, DoubleVector& derivates ) const {
	//--- return the same as if it is a sigmoid with lambda = 1
	for( int i=0; i<inputs.size(); i++ ) {
		double y;
		y = 1.0/( 1.0 + std::exp( -inputs[i] ) );
		derivates[i] = y * ( 1.0 - y );
	}
	return true;
}

void StepFunction::configure() {
	min = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"min", 0.0 );
	max = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"max", 1.0 );
	threshold = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"threshold", 0.0 );
	markAsConfigured();
}

void StepFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "StepFunction", this);
	params.createParameter(prefix, "min", QString::number(min));
	params.createParameter(prefix, "max", QString::number(max));
	params.createParameter(prefix, "threshold", QString::number(threshold));
}

void StepFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Step Function" );
	d.describeReal("min").def(0.0).help("The value returned if the input is below threshold");
	d.describeReal("max").def(1.0).help("The value returned if the input is above threshold");
	d.describeReal("threshold").def(0.0).help("The threshold value used for checking if would return min or max on output");
}

void StepFunction::setCoefficients( double min, double max, double threshold ) {
	this->min = min;
	this->max = max;
	this->threshold = threshold;
}

double StepFunction::getMin() {
	return min;
}

double StepFunction::getMax() {
	return max;
}

double StepFunction::getThreshold() {
	return threshold;
}

LeakyIntegratorFunction::LeakyIntegratorFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	delta(),
	outprev() {
}

void LeakyIntegratorFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	//--- y <- delta*y(t-1) + (1.0-delta)*inputs
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = delta[i]*outprev[i] + (1.0-delta[i])*inputs[i];
	}
	outprev = outputs;
}

void LeakyIntegratorFunction::zeroingStatus() {
	outprev.setZero();
}

void LeakyIntegratorFunction::clusterSetted() {
	if ( clusterv->numNeurons() != delta.size() ) {
		delta.resize( clusterv->numNeurons() );
		outprev.resize( clusterv->numNeurons() );
		outprev.setZero();
	}
}

void LeakyIntegratorFunction::configure() {
	// Delta is a vector, that is a list of space-separated values
	QVector<double> vect = ConfigurationHelper::getVector( configurationManager(), prefixPath() + "delta" );
	delta = Eigen::Map<VectorXd>(vect.data(), vect.size());

	// Also reloading outprev (resizing it to match delta length)
	vect = ConfigurationHelper::getVector( configurationManager(), prefixPath() + "outprev" );
	outprev = Eigen::Map<VectorXd>(vect.data(), vect.size());
	outprev.resize(delta.size());
	markAsConfigured();
}

void LeakyIntegratorFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "LeakyIntegratorFunction", this);

	// First creating a string list, then transforming to a single string
	QStringList list;
	for (int i = 0; i < delta.size(); i++) {
		list.push_back(QString::number(delta[i]));
	}
	params.createParameter(prefix, "delta", list.join(" "));

	// Saving in the same way also outprev
	list.clear();
	for (int i = 0; i < delta.size(); i++) {
		list.push_back(QString::number(outprev[i]));
	}
	params.createParameter(prefix, "outprev", list.join(" "));
}

void LeakyIntegratorFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Leaky Integrator Function" );
	d.describeReal("delta").def(0.0).props( ParamIsList ).help("The list of delta coefficients");
	d.describeReal("outprev").def(0.0).props( ParamIsList ).help("The previous values used in the calculation");
}

void LeakyIntegratorFunction::setDeltas( DoubleVector delta ) {
	this->delta = delta;
}

DoubleVector& LeakyIntegratorFunction::getDeltas() {
	return delta;
}

LogLikeFunction::LogLikeFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	A(1.0),
	B(5.0) {
}

void LogLikeFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	//--- y <- x / ( 1+A*x+b )
	for( int i=0; i<inputs.size(); i++ ) {
		outputs[i] = inputs[i]/(1.0+A*inputs[i]+B);
	}
}

void LogLikeFunction::configure() {
	A = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"A", 1.0 );
	B = ConfigurationHelper::getDouble(configurationManager(), prefixPath()+"B", 5.0 );
	markAsConfigured();
}

void LogLikeFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "LogLikeFunction", this);
	params.createParameter(prefix, "A", QString::number(A));
	params.createParameter(prefix, "B", QString::number(B));
}

void LogLikeFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A LogLike Function" );
	d.describeReal("A").def(1.0).help("The A coefficient of the function");
	d.describeReal("B").def(5.0).help("The B coefficient of the function");
}

void LogLikeFunction::setCoefficients( double A, double B ) {
	this->A = A;
	this->B = B;
}

double LogLikeFunction::getA() {
	return A;
}

double LogLikeFunction::getB() {
	return B;
}

CompositeFunction::CompositeFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	first(),
	second(),
	mid() {
}

CompositeFunction::~CompositeFunction() {
	// unique_ptr will release memory for us
}

void CompositeFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	first->apply( inputs, mid );
	second->apply( mid, outputs );
}

bool CompositeFunction::setFirstFunction( OutputFunction *f ) {
	first.reset(f);
	first->setCluster( clusterv );
	return true;
}

OutputFunction* CompositeFunction::getFirstFunction() {
	return first.get();
}

bool CompositeFunction::setSecondFunction( OutputFunction *g ) {
	second.reset(g);
	second->setCluster( clusterv );
	return true;
}

OutputFunction* CompositeFunction::getSecondFunction() {
	return second.get();
}

void CompositeFunction::clusterSetted() {
	mid.resize( clusterv->numNeurons() );
	first->setCluster( clusterv );
	second->setCluster( clusterv );
}

void CompositeFunction::configure() {
	// We don't need configured component functions here (and they will be
	// configured after exiting from this function)
	first.reset(configurationManager().getObjectFromGroup<OutputFunction>(prefixPath() + "First"));
	second.reset(configurationManager().getObjectFromGroup<OutputFunction>(prefixPath() + "Second"));
	// We don't need to reload a reference to the cluster as he calls our setCluster
	// function after our creation
	markAsConfigured();
}

void CompositeFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "CompositeFunction", this);
	first->save();
	second->save();
	// We don't need to save the reference to the cluster as he calls our setCluster
	// function after our creation
}

void CompositeFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A Composite Function (made combining two functions)" );
	d.describeSubgroup( "First" ).type( "OutputFunction" ).props( ParamIsMandatory ).help( "The first function applied" );
	d.describeSubgroup( "Second" ).type( "OutputFunction" ).props( ParamIsMandatory ).help( "The second function applied to the result of the first" );
}

LinearComboFunction::LinearComboFunction( ConfigurationManager& params, QString prefix, Component* parent )
	: OutputFunction(params,prefix,parent),
	first(),
	second(),
	mid(),
	w1(1.0),
	w2(1.0) {
}

LinearComboFunction::~LinearComboFunction() {
	// unique_ptr will release memory for us
}

void LinearComboFunction::apply( DoubleVector& inputs, DoubleVector& outputs ) {
	first->apply( inputs, mid );
	mid *= w1;
	second->apply( inputs, outputs );
	outputs *= w2;
	outputs += mid;
}

bool LinearComboFunction::setFirstFunction( OutputFunction *f ) {
	first.reset(f);
	first->setCluster( clusterv );
	return true;
}

OutputFunction* LinearComboFunction::getFirstFunction() {
	return first.get();
}

bool LinearComboFunction::setFirstWeight( double v ) {
	w1 = v;
	return true;
}

double LinearComboFunction::getFirstWeight() {
	return w1;
}

bool LinearComboFunction::setSecondFunction( OutputFunction *g ) {
	second.reset(g);
	second->setCluster( clusterv );
	return true;
}

OutputFunction* LinearComboFunction::getSecondFunction() {
	return second.get();
}

bool LinearComboFunction::setSecondWeight( double v ) {
	w2 = v;
	return true;
}

double LinearComboFunction::getSecondWeight() {
	return w2;
}

void LinearComboFunction::clusterSetted() {
	mid.resize( clusterv->numNeurons() );
	first->setCluster( clusterv );
	second->setCluster( clusterv );
}

void LinearComboFunction::configure() {
	// We don't need configured component functions here (and they will be
	// configured after exiting from this function)
	first.reset(configurationManager().getObjectFromGroup<OutputFunction>(prefixPath() + "First"));
	w1 = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"w1", 1.0 );

	second.reset(configurationManager().getObjectFromGroup<OutputFunction>(prefixPath() + "Second"));
	w2 = ConfigurationHelper::getDouble( configurationManager(), prefixPath()+"w2", 1.0 );
	// We don't need to reload a reference to the cluster as he calls our setCluster
	// function after our creation
	markAsConfigured();
}

void LinearComboFunction::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "LinearComboFunction", this);
	first->save();
	params.createParameter(prefix, "w1", QString::number(w1));
	second->save();
	params.createParameter(prefix, "w2", QString::number(w2));
	// We don't need to save the reference to the cluster as he calls our setCluster
	// function after our creation
}

void LinearComboFunction::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "A weighted linear combination of two functions" );
	d.describeSubgroup( "First" ).type( "OutputFunction" ).props( ParamIsMandatory ).help( "The first function applied to the inputs" );
	d.describeReal("w1").def(1.0).help( "The weight applied to the first function" );
	d.describeSubgroup( "Second" ).type( "OutputFunction" ).props( ParamIsMandatory ).help( "The second function applied to the inputs" );
	d.describeReal("w2").def(1.0).help( "The weight applied to the second function" );
}

}
