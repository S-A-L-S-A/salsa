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

#ifndef BACKPROPAGATIONALGO_H
#define BACKPROPAGATIONALGO_H

#include "nnfwconfig.h"
#include "learningalgorithm.h"
#include "biasedcluster.h"
#include "matrixlinker.h"
#include <QMap>
#include <QVector>

namespace salsa {

/*! \brief Back-Propagation Algorithm implementation
 *
 */
class SALSA_NNFW_API BackPropagationAlgo : public LearningAlgorithm {
public:
	/*! Default Constructor */
	BackPropagationAlgo(ConfigurationManager& params, QString prefix, Component* parent = nullptr);

	//! Destructor
	~BackPropagationAlgo();

	/*! Set the order on which the error is backpropagated through the NeuralNet
	 *  \warning Calling this method will also clean any previous data about previous processing and
	 *   re-initialize all datas
	 */
	void setUpdateOrder( const UpdatableList& update_order );
	
	/*! Return the order on which the error is backpropaget through the NeuralNet */
	UpdatableList updateOrder() const {
		return update_order;
	};
	/*! Set the teaching input for Cluster passed
	 *  \param teach_input the DoubleVector teaching input
	 */
	void setTeachingInput( Cluster* output, const DoubleVector& ti );

	virtual void learn();

	/*! Starts a single training step. */
	virtual void learn( Pattern* );

	/*! Calculate the Mean Square Error respect to Pattern passed */
	virtual double calculateMSE( Pattern* );

	/*! Set the learning rate */
	void setRate( double newrate ) {
		learn_rate = newrate;
	};

	/*! return the learning rate */
	double rate() const {
		return learn_rate;
	};

	/*! Set the momentum value */
	void setMomentum( double newmom ) {
		momentumv = newmom;
	};

	/*! return the momentum */
	double momentum() const {
		return momentumv;
	};

	/*! Enable the momentum */
	void enableMomentum();

	/*! Disable momentum */
	void disableMomentum() {
		useMomentum = false;
	};

	/*! This method returns the deltas calculated by the Back-propagation Algorithm.
	 *  These deltas are set every time new targets are defined for the output layer(s),
	 *  which are then used to update network weights when the method learn() is called.<br>
	 *  They are also useful to calculate the network performance, but for that it must be used outside
	 *  the <em>learning cycle</em> (a full learning iteration, that corresponds to present the network
	 *  with all the patterns of the train data set). For that you must call
	 *  getError( Cluster * anyOutputCluster ) for each line of your training set (you'll get a DoubleVector
	 *  with the deltas for each unit of the cluster considered).<br>
	 *  Then you can use those values to calculate your desired performance measure.<br>
	 *  For instance: if you use it to calculate the Mean Square Error (MSE) of the network
	 *  for your train data set you must accumulate the square of the the getError( anyOutputCluster ) 
	 *  output for each line of the set, and at the end divide it by the length of that same set
	 *  (by definition the MSE is the sum of the squared differences between the target and actual
	 *  output of a sequence of values). Getting the Root Mean Squared Error (RMSE) from this is
	 *  trivial (you just need to calculate the square root of the MSE).
	 *
	 *  \warning The data returned by getError( Cluster * ) is computed every time you set a new output target,
	 *  which means every time you call the setTeachingInput( Cluster * anyOutputCluster, const DoubleVector &
	 *  teaching_input ) method. If your network has more than one output layer you have to call
	 *  setTeachingInput() for all the output clusters before calling getError() for any of the clusters.
	 */
	DoubleVector getError( Cluster* );
	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 * 
	 * From the file you can configure the parameters of the BackPropagation in this way:
	 * \code
	 * [aBackPropagationGroup]
	 * neuralnet = nameOfGroupOfTheNeuralNet
	 * rate = learningRate      ;if it's not present, default is 0.0 !!
	 * momentum = momentumRate  ;if it's not present, means momentum disabled
	 * order = cluster2 linker1 cluster1 ; order of Cluster and Linker on which the error is backpropagated
	 * \endcode
	 * As you can note, there is no configuration parameters for loading the learning set from here.
	 * This is intended. You need to load separately the learning set and call the method learn on the
	 * loaded learning set.
	 * 
	 * You can do that creating groups like the following (see class Pattern):
	 * \code
	 * [learningSet:1]
	 * cluster:1 = cluster1
	 * inputs:1 = 1 2 3     ; input
	 * cluster:2 = cluster2
	 * outputs:2 = 2 4      ; desired output
	 * [learningSet:2]
	 * cluster:1 = cluster1
	 * inputs:1 = 2 4 6     ; input
	 * cluster:2 = cluster2
	 * outputs:2 = 4 8      ; desired output
	 * ...
	 * [learningSet:N]
	 * cluster:1 = cluster1
	 * inputs:1 = 10 20 30    ; input
	 * cluster:2 = cluster2
	 * outputs:2 = 20 40      ; desired output
	 * \endcode
	 * And call the method LearningAlgorithm::loadPatternSet( params, "learningSet" )
	 *
	 */
	virtual void configure();
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
protected:
	/*! Configure internal data for backpropagation on NeuralNet setted */
	virtual void neuralNetChanged();
private:
	//! The double learning rate factor
	double learn_rate;
	//! Momentum
	double momentumv;
	//! bool switch for enable/disable momentum
	double useMomentum;
	//! The update order
	UpdatableList update_order;

	//! The struct of Clusters and Deltas
	class SALSA_NNFW_API cluster_deltas {
	public:
		BiasedCluster* cluster;
		bool isOutput;
		DoubleVector deltas_outputs;
		DoubleVector deltas_inputs;
		DoubleVector last_deltas_inputs;
		QList<MatrixLinker*> incoming_linkers_vec;
		QVector<DoubleVector> incoming_last_outputs;
	};
	//! map to help looking for cluster_deltas info
	QMap<Cluster*, int> mapIndex;
	//! The VectorData of struct of Clusters and Deltas
	QVector<cluster_deltas> cluster_deltas_vec;
	// --- propagate delta through the net
	void propagDeltas();
	// --- add a Cluster into the structures above
	void addCluster( Cluster*, bool );
	// --- add a Linker into the structures above
	void addLinker( Linker* );

};

}

#endif

