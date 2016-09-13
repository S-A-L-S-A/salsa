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

#ifndef CLUSTER_H
#define CLUSTER_H

/*! \file
 *  \brief This file contains the declarations of the Cluster class
 */

#include "nnfwconfig.h"
#include "updatable.h"
#include "outputfunction.h"
#include <exception>
#include <memory>

namespace salsa {

/*! \brief Define the common interface among Clusters
 *
 *    The Cluster class define the common interface amog Cluster. The subclasses may extends this interface
 *    for specific purpose (ex. SimpleCluster), but the BaseNeuralNet, the Linker and other classes depends
 *    only by this class. This abstraction allow to isolate the specific implementation of various classes
 *
 *    The Cluster class represent an abstract group of neurons. There isn't a neuron class. The Cluster
 *    class represent a group of neurons as two arrays: inputs and outputs. The inputs array represent the
 *    inputs of the neurons 'contained' in the cluster, and the outputs of this neurons are calculated by
 *    appling the function provided by OutputFunction.<br>
 *    The number of neuron returned by size() method is also the dimension of inputs and outputs arrays<br>
 *    You can sets one subclasses of OutputFunction by setUpdater methods. If you don't specify an index when
 *    set a OutputFunction then this OutputFunction will be used to update the output of all neurons. Otherwise,
 *    you can specifiy different OutputFunction for different neuron.
 *    \code
 * // create a SimpleCluster, a specialized subclass of Cluster
 * SimpleCluster* simple = new SimpleCluster( 10 ); // this cluster contains 10 neurons
 * // set the SigmoidUpdater for all neurons
 * simple->setUpdater( new SigmoidUpdater( 1.0 ) );
 *    \endcode
 *
 *    <b>For whose want to implement a subclass of Cluster: </b>
 *    This class allocate the memory for inputs, outputs and updaters array. So, a subclass have to implements only
 *    the update method.
 *    The getInputs and getOutputs returns a valid array of internal data, and not simply a copy
 *    of the internal data. Look at the following code:
 *    \code
 * RealVec& in = cluster->inputs();
 * in[2] = 3.0;   // This statement will be changes the inputs of third neuron.
 * // the above statement is equivalent with the following
 * cluster->setInput( 2, 3.0 );
 *    \endcode
 *    The reasons behind this kind of behaviour its the efficiency!! When another class must do heavy calculation
 *    on all inputs of a Cluster (as MatrixLinker do), then its more efficient that it takes the array returned
 *    by inputs (or outputs) and works over them.
 *
 */
class SALSA_NNFW_API Cluster : public Updatable {
public:
	/*! Constructor */
	Cluster( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	virtual ~Cluster();
	/*! Return the number of neurons (the length of input and output arrays) */
	unsigned int numNeurons() const {
		return numneurons;
	};
	/*! Return true if inputs needs a reset */
	bool needReset() {
		return needRst;
	};
	/*! Enable/Disable accumulation mode<br>
	 *  If accumulation is enabled (true) then linkers attached to this Cluster will never resetInput and accumulates data,
	 *  otherwise the inputs will be resetted at each step of neural network
	*/
	void setAccumulate( bool mode ) {
		accOff = !mode;
	};
	/*! return true if the Cluster will accumulates inputs */
	bool isAccumulate() const {
		return !accOff;
	};
	/*! Randomize the parameters of the Cluster<br>
	 *  The parameters randomized by this method will be specified by sub-classes
	 */
	virtual void randomize( double min, double max ) = 0;
	/*! Set the input of neuron
	 * Details...
	 */
	void setInput( unsigned int neuron, double value );
	/*! Set the inputs from the vector given */
	void setInputs( const DoubleVector& inputs );
	/*! Set all the inputs with the same value
	 * Details...
	 */
	void setAllInputs( double value );
	/*! Reset the inputs of this cluster; the inputs will be set to zero.
	 * Details...
	 */
	void resetInputs();
	/*! Get the input of neuron
	 */
	double getInput( unsigned int neuron ) const;
	/*! Get the array of inputs */
	DoubleVector& inputs() {
		return inputdata;
	};
	/*! Get the array of inputs */
	const DoubleVector& inputs() const {
		return inputdata;
	};
	/*! Force the output of the neuron at value specified */
	void setOutput( unsigned int neuron, double value );
	/*! Set the outputs from the vector given */
	void setOutputs( const DoubleVector& outputs );
	/*! Get the output of neuron */
	double getOutput( unsigned int neuron ) const;
	/*! Get the array of outputs */
	DoubleVector& outputs() {
		return outputdata;
	};
	/*! Get the array of outputs */
	const DoubleVector& outputs() const {
		return outputdata;
	};
	/*! Set the output function for all neurons contained<br>
	 *  This method create an internal copy of the OutputFunction passed <br>
	 *  \warning This function delete the previous updater class registered !!! <br>
	 */
	void setOutFunction( OutputFunction* up );
	/*! Get the Output function */
	OutputFunction* outFunction() const {
		return updater.get();
	};
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
	/*! Delegate type for retrieving a vector by name (pointer to function)
	 *  It works in this way:
	 *  \code
	 *  getStateVectorFuncPtr delegate = aCluster->getDelegateFor( "biases" );
	 *  DoubleVector& biasesVector = (*delegate)( aCluster );
	 *  \endcode
	 *  In the first row, getDelegateFor return a pointer to the function for retrieving the
	 *  vector of biases.
	 *  At the second row the delegate is used.
	 *
	 *  Look at the code of Linker and DotLinker for a full example on how to use it.
	 */
	typedef DoubleVector& (*getStateVectorFuncPtr)( Cluster* );
	/*! Return the pointer to function for retrieving the DoubleVector representing the state requested
	 *  \param stateVector is the name of the DoubleVector requested (i.e. "biases" in the case of the biases
	 *   of a BiasedCluster)
	 *  \note the class Cluster defines "inputs" and "outputs" that correspond to method inputs() and outputs()
	 *  \warning It will raise an exception if the state requested does not exists
	 */
	getStateVectorFuncPtr getDelegateFor( QString stateVector ) {
		if ( stateDelegates.contains( stateVector ) ) {
			return stateDelegates[stateVector];
		}
		throw ClusterStateVectorNotPresent( (QString("The state vector named ") + stateVector + " is not part of this Cluster").toLatin1().data() );
	};
protected:
	/*! Configure a delegate for a specifing state vector; who implements subclasses of Cluster
	 *  has to specify a name and the method used to retrieve any state vector that needs to be
	 *  public (i.e. used by Linkers for accessing and modifing it.
	 *
	 *  The usage is the following: suppose the case of BiasedCluster that has the biases vector and
	 *  the metod biases() that returns the reference to the DoubleVector containing the biases.
	 *  In this case, in the constructor there is the following statement:
	 *  \code
	 *    setDelegateFor<BiasedCluster, &BiasedCluster::biases>( "biases" )
	 *  \endcode
	 *  where the first parameter of the template if the class with the new state vector, the second
	 *  parameter is the method for getting the vector specified with the pointer-to-member syntax, and
	 *  the name of the state vector (used for referencing it by name)
	 */
	template <class T, DoubleVector& (T::*TMethod)()>
	void setDelegateFor( QString vectorName ) {
		stateDelegates[vectorName] = &staticDelegateMethod<T, TMethod>;
	}
	/*! Set the state of 'needReset'<br>
	 *  Used by subclasses into update implementation
	 */
	void setNeedReset( bool b ) {
		needRst = accOff && b;
	};
private:
	/*! Number of neurons */
	unsigned int numneurons;
	/*! Input of neurons */
	DoubleVector inputdata;
	/*! Output of neurons */
	DoubleVector outputdata;
	/*! OutputFunction Object */
	std::unique_ptr<OutputFunction> updater;
	/*! True if the inputs needs a reset */
	bool needRst;
	/*! In Accumulated mode the needRst is always false, and then linkers attached to this will never resetInputs
	 *  --- Warns for developers --- accOff == true means NO-ACCUMULATION
	 */
	bool accOff;

	/*! Mapping from vector's name => pointer to function for retrieving it */
	QMap<QString, getStateVectorFuncPtr> stateDelegates;
	/*! Template for creating wrapper for accessing the method used for retrieving the state vector */
	template <class T, DoubleVector& (T::*TMethod)()>
	static DoubleVector& staticDelegateMethod( Cluster* cluster_ptr ) {
		T* p = static_cast<T*>(cluster_ptr);
		//--- call the delegate method using pointer-to-member syntax
		return (p->*TMethod)();
	}
};

}

#endif
