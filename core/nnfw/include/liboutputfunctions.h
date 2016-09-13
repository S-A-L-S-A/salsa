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

#ifndef LIBOUTPUTFUNCTIONS_H
#define LIBOUTPUTFUNCTIONS_H

#include "nnfwconfig.h"
#include "outputfunction.h"

namespace salsa {

/*! \brief IdentityFunction
 *
 * IdentityFunction copyies the inputs to the outputs
 */
class SALSA_NNFW_API IdentityFunction : public OutputFunction {
public:
	/*! Construct */
	IdentityFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~IdentityFunction() { /* Nothing to do */ };
	/*! Implement the identity function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return always 1 (an explain of why will be coming soon) */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
};

/*! \brief ScaleFunction
 *
 * ScaleFunction scales the inputs vector to the outputs
 */
class SALSA_NNFW_API ScaleFunction : public OutputFunction {
public:
	/*! Construct */
	ScaleFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~ScaleFunction() { /* Nothing to do */ };
	/*! Implement the identity function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return always the rate (an explain of why will be coming soon) */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! \brief Set the rate coefficient of the ScaleFunction */
	void setRate( double rate );
	/*! return the rate coefficient */
	double getRate();
private:
	/*! the rate coefficient */
	double rate;
};

/*! \brief GainFunction
 *
 * GainFunction add a constan value to inputs
 */
class SALSA_NNFW_API GainFunction : public OutputFunction {
public:
	/*! Construct */
	GainFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~GainFunction() { /* Nothing to do */ };
	/*! Implement the Gain function */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return always 1 (an explain of why will be coming soon) */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! Set the gain of this function */
	void setGain( double gain );
	/*! return the gain of this function */
	double getGain();
private:
	double gainv;
};

/*! \brief Sigmoid Function
 *
 * Details..
 */
class SALSA_NNFW_API SigmoidFunction : public OutputFunction {
public:
	/*! Construct a sigmoid updater with parameter l */
	SigmoidFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~SigmoidFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return the approximation commonly used in backpropagation learning: y*(1-y) */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! Set the lambda (slope) of this function */
	void setLambda( double lambda );
	/*! return the lambda (slope) of this function */
	double getLambda();
private:
	/*! lambda is the slope of the curve */
	double lambda;
};

/*! \brief Fake Sigmoid Function !! Is a linear approximation of sigmoid function
 *
 */
class SALSA_NNFW_API FakeSigmoidFunction : public OutputFunction {
public:
	/*! Construct a sigmoid updater with parameter l */
	FakeSigmoidFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~FakeSigmoidFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return the approximation commonly used in backpropagation learning: x(1-x) */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! Set the lambda (slope) of this function */
	void setLambda( double lambda );
	/*! return the lambda (slope) of this function */
	double getLambda();
private:
	/*! lambda is the slope of the curve */
	double lambda;
};

/*! \brief ScaledSigmoid Function
 *
 * lambda is the slope of the curve<br>
 * min is the y value when x -> -infinite <br>
 * max is the y value when x -> +infinite <br>
 * (max-min) is the y value when x == 0
 *
 */
class SALSA_NNFW_API ScaledSigmoidFunction : public OutputFunction {
public:
	/*! Construct a scaled sigmoid updater with parameter l */
	ScaledSigmoidFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~ScaledSigmoidFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return the approximation commonly used in backpropagation learning: x(1-x) */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! set the coefficients for this function
	 * \param lambda is the slope of the curve
	 * \param min is the y value when x -> -infinite
	 * \param max is the y value when x -> +infinite
	 */
	void setCoefficients( double lambda, double min, double max );
	/*! return the lambda coefficient */
	double getLambda();
	/*! return the min coefficient */
	double getMin();
	/*! return the max coefficient */
	double getMax();
private:
	double lambda;
	/*! min is the y value when x -> -infinite */
	double min;
	/*! max is the y value when x -> +infinite */
	double max;
};

/*! \brief Ramp Function
 *
 * <pre>
 *  maxY            /---------------
 *                 /
 *                /
 *               /
 *  minY -------/
 *           minX   maxX
 * </pre>
 *
 */
class SALSA_NNFW_API RampFunction : public OutputFunction {
public:
	/*! Default constructor*/
	RampFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~RampFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return the m coefficient if x is in [minX, maxX] and x(1-x) otherwise */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! set the coefficients for this function
	 * <pre>
	 *  maxY            /---------------
	 *                 /
	 *                /
	 *               /
	 *  minY -------/
	 *           minX   maxX
	 * </pre>
	 */
	void setCoefficients( double min_x, double max_x, double min_y, double max_y );
	/*! return the minX coefficient */
	double getMinX();
	/*! return the maxX coefficient */
	double getMaxX();
	/*! return the minY coefficient */
	double getMinY();
	/*! return the maxY coefficient */
	double getMaxY();
private:
	/*! minX */
	double min_x;
	/*! maxX */
	double max_x;
	/*! minY */
	double min_y;
	/*! maxY */
	double max_y;
};

/*! \brief Linear equation Function
 *  Implements a linear equation y = m*x + b
 *
 */
class SALSA_NNFW_API LinearFunction : public OutputFunction {
public:
	/*! Default constructor */
	LinearFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~LinearFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! return the m coefficient */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! set the coefficient of this function: y = m*x + b */
	void setCoefficients( double m, double b );
	/*! return the m coefficient */
	double getM();
	/*! return the b coefficient */
	double getB();
private:
	/*! m */
	double m;
	/*! b */
	double b;
};

/*! \brief Step Function
 *
 * Further Details coming soon ;-)
 *
 */
class SALSA_NNFW_API StepFunction : public OutputFunction {
public:
	/*! Construct a step updater */
	StepFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~StepFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! Using the derivate of the sigmoid function!!!  */
	virtual bool derivate( const DoubleVector& x, const DoubleVector& y, DoubleVector& d ) const;
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! set the coefficients of this function */
	void setCoefficients( double min, double max, double threshold );
	/*! return the min coefficient */
	double getMin();
	/*! return the max coefficient */
	double getMax();
	/*! return the threshold */
	double getThreshold();
private:
	/*! Minimum value  */
	double min;
	/*! Maximum value */
	double max;
	/*! Threshold */
	double threshold;
};

/*! \brief LeakyIntegrator Function !!
 *
 */
class SALSA_NNFW_API LeakyIntegratorFunction : public OutputFunction {
public:
	/*! Default constructor */
	LeakyIntegratorFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~LeakyIntegratorFunction() { /* Nothing to do */ };
	/*! Zeroing the status */
	void zeroingStatus();
	/*! Implement the updating method<br>
	 * it computes: y(t) <- delta * y(t-1) + (1.0-delta) * inputs
	 */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! resize itself to fit the size of Cluster */
	virtual void clusterSetted();
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! set the deltas */
	void setDeltas( DoubleVector delta );
	/*! return a modifiable reference of delta vector */
	DoubleVector& getDeltas();
private:
	/*! delta is the leak rate of the function */
	DoubleVector delta;
	/*! previous outputs */
	DoubleVector outprev;
};

/*! \brief LogLike Function !!
 *
 *  It compute the following equation:
 *  \f[
 *     y = \frac{x}{ 1+Ax+B }
 *  \f]
 *
 */
class SALSA_NNFW_API LogLikeFunction : public OutputFunction {
public:
	/*! Construct a LogLike with deltas specified */
	LogLikeFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~LogLikeFunction() { /* Nothing to do */ };
	/*! Implement the updating method */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! set the coefficient of this function */
	void setCoefficients( double A, double B );
	/*! return the A coefficient */
	double getA();
	/*! return the B coefficient */
	double getB();
private:
	/*! A coefficient */
	double A;
	/*! B coefficient */
	double B;
};

/*! \brief Composite Function !!
 *
 * \note Component OutputFunction objects are destroyed by the CompositeFunction
 *       object to which they belong.
 */
class SALSA_NNFW_API CompositeFunction : public OutputFunction {
public:
	/*! Default constructor */
	CompositeFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~CompositeFunction();
	/*! Set the first function of CompositeFunction */
	bool setFirstFunction( OutputFunction *f );
	/*! Return the first function of CompositeFunction */
	OutputFunction* getFirstFunction();
	/*! Set the second function of CompositeFunction */
	bool setSecondFunction( OutputFunction *g );
	/*! Return the second function of CompositeFunction */
	OutputFunction* getSecondFunction();
	/*! Implement the updating method <br>
	 * it computes: y <- second( first( input, mid ), outputs ) <br>
	 * where mid is a private vector that traces the outputs of first function
	 */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! recursive call setCluster on first and second function setted */
	virtual void clusterSetted();
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
private:
	//! first function
	std::unique_ptr<OutputFunction> first;
	//! second function
	std::unique_ptr<OutputFunction> second;
	//! intermediate result (for speedup calculation)
	DoubleVector mid;
	//! Copy constructor and copy operator (here to prevent usage)
	CompositeFunction(const CompositeFunction&);
	CompositeFunction& operator=(const CompositeFunction&);
};

/*! \brief Linear Combination of Two Function !!
 *
 * \note Component OutputFunction objects are destroyed by the
 *       LinearComboFunction object to which they belong.
 *
 */
class SALSA_NNFW_API LinearComboFunction : public OutputFunction {
public:
	/*! Standard constructor */
	LinearComboFunction( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~LinearComboFunction();
	/*! Set the first function of LinearComboFunction */
	bool setFirstFunction( OutputFunction *f );
	/*! Return the first function of LinearComboFunction */
	OutputFunction* getFirstFunction();
	/*! Set the first weight of LinearComboFunction */
	bool setFirstWeight( double v );
	/*! Return the first weight of LinearComboFunction */
	double getFirstWeight();
	/*! Set the second function of CompositeFunction */
	bool setSecondFunction( OutputFunction *g );
	/*! Return the second function of CompositeFunction */
	OutputFunction* getSecondFunction();
	/*! Set the second weight of LinearComboFunction */
	bool setSecondWeight( double v );
	/*! Return the second weight of LinearComboFunction */
	double getSecondWeight();
	/*! Implement the updating method <br>
	 * it computes: y <- w1*first(input,output) + w2*second(input,outputs)
	 */
	virtual void apply( DoubleVector& inputs, DoubleVector& outputs );
	/*! recursive call setCluster on first and second function setted */
	virtual void clusterSetted();
	/*! \brief Configures the object using a ConfigurationParameters object */
	virtual void configure();
	/*! \brief Save the actual status of parameters into the ConfigurationParameters object passed */
	virtual void save();
	/*! \brief Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
private:
	//--- functions
	std::unique_ptr<OutputFunction> first;
	std::unique_ptr<OutputFunction> second;
	//--- temporary result
	DoubleVector mid;
	//--- weights
	double w1, w2;
	// Copy constructor and copy operator (here to prevent usage)
	LinearComboFunction(const LinearComboFunction&);
	LinearComboFunction& operator=(const LinearComboFunction&);
};

}

#endif
