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

#ifndef NEURALNET_H
#define NEURALNET_H

#include "nnfwconfig.h"
#include "configurationmanager.h"
#include "cluster.h"
#include "linker.h"

namespace salsa {

/*! \brief An observer interface called during NeuralNet operations
 *
 *  Subclass it and implements the method accordlying on what you need to be notified
 */
class NeuralNetObserver {
public:
	//! virtual destructor
	virtual ~NeuralNetObserver() { }
	//! \brief called just after a complete step has been done
	virtual void onStepDone() { };
};

/*! \brief The Neural Network Class
 *
 * The NeuralNet class can seen as a simple container of Clusters and Linkers<br>
 * The relationship among Clusters and Linkers must be specified by cluster and linker constructors;
 * 
 * When the NeuralNet is configured from a file, it is configured in the following way:
 *  - the parameters for configuring a NeuralNet are: inputClusters, outputClusters, spreadOrder;
 *    all parameters consist in a list of the cluster or linker names
 *  - the only mandatory parameter is spreadOrder
 *  - inputClusters is the list of all Clusters considered the input layer of the NeuralNet
 *  - outputClusters is the list of all Clusters considered the output layer of the NeuralNet
 *  - spreadOrder is the order on which the Clusters and Linkers needs to be updated
 *  - all sub-groups with [CLUSTER:n] pattern will be loaded as Clusters into the NeuralNet
 *  - all sub-groups with [LINKER:n] pattern will be loaded as Linkers into the NeuralNet
 *  - all Clusters not referenced on inputClusters or outputClusters will be considered hidden (internal) Clusters
 *
 * Some examples of configuration files using the INI format.
 * 
 * The first example shows how to create a NeuralNet with two Cluster as input and one Cluster as output connected by two linkers.
 * \code
 * inputClusters = input1 input2
 * outputClusters = output1
 * spreadOrder = input1 input2 linker1 linker2 output1
 * 
 * [CLUSTER:0]
 * name = input1
 * type = FakeCluster
 * 
 * [CLUSTER:1]
 * name = input2
 * type = SimpleCluster
 * [CLUSTER:1/OutFunction]
 * type = StepFunction
 * min = -1.0
 * max = +1.0
 * threshold = 0.5
 * 
 * [CLUSTER:2]
 * name = output1
 * type = BiasedCluster
 * [CLUSTER:2/OutFunction]
 * type = SigmoidFunction
 * lambda = 0.5
 * 
 * [LINKER:0]
 * name = linker1
 * type = DotLinker
 * from = input1
 * to = output1
 * 
 * [LINKER:1]
 * name = linker2
 * type = DotLinker
 * from = input2
 * to = output1
 * \endcode
 *
 * Let's suppose that you want to change the NeuralNet of the example above removing input1 as input Cluster of the NeuralNet
 * then the two following configurations results in the same NeuralNet:
 * \code
 * inputClusters = input2
 * outputClusters = output1
 * spreadOrder = input1 input2 linker1 linker2 output1
 * 
 * ...
 * \endcode
 * \code
 * inputClusters = input2
 * outputClusters = output1
 * spreadOrder = input2 linker1 linker2 output1
 * 
 * ...
 * \endcode
 * Because even if in the last example the input1 is not specified in any of the NeuralNet parameters,
 * it will be created because [CLUSTER:0] is a subgroup of the main group where the NeuralNet parameters are present.
 */
class SALSA_NNFW_API NeuralNet : public Component {
public:
	/*! Construct an empty neural network */
	NeuralNet( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Destructor */
	~NeuralNet();
	/*! Set the name of the NeuralNet
	 *  It's useful when loading from ConfigurationParameters more than NeuralNet or from
	 *  LearningAlgorithms subclasses
	 */
	void setName( QString name ) {
		namev = name;
	};
	/*! return the name of the NeuralNet */
	QString name() {
		return namev;
	};
	/*! return the UI Manager for accessing to the viewers of NeuralNet */
	ComponentUI* getUIManager();
	/*! add the observer to the list of current observers */
	void addObserver( NeuralNetObserver* observer );
	/*! remove the observer from the list of current observers */
	void removeObserver( NeuralNetObserver* observer );
	/*! Add a Cluster into the neural network<br>
	 * If isInput is true then the Cluster will be considered as an Input Cluster of this network<br>
	 * If isOutput is true then the Cluster will be considered as an Output Cluster of this network */
	void addCluster( Cluster* c, bool isInput = false, bool isOutput = false );
	/*! Add a Cluster and mark it as Input<br>
	 *  Behave exactly the same of addCluster( c, true, false ) */
	void addInputCluster( Cluster* c ) {
		addCluster( c, true, false );
	};
	/*! Add a Cluster and mark it as Output<br>
	 *  Behave exactly the same of addCluster( c, false, true ) */
	void addOutputCluster( Cluster* c ) {
		addCluster( c, false, true );
	};
	/*! Remove a Cluster from the network */
	bool removeCluster( Cluster* c );
	/*! Mark a Cluster as an Input Cluster of this network */
	void markAsInput( Cluster* c );
	/*! Mark a Cluster as an Output Cluster of this network */
	void markAsOutput( Cluster* c );
	/*! Eliminate the marks from Cluster passed
	 *  \warning if a Cluster have two marker (Input and Output marks) then both marker are removed */
	void unmark( Cluster* c );
	/*! Eliminate the marks from all Cluster present in this networks */
	void unmarkAll();
	/*! Return true if there isn't any Linker connected with Cluster c */
	bool isIsolated( Cluster* c ) const;
	/*! Returns the vector of Clusters contained */
	ClusterList clusters() const;
	/*! Returns the vector of Input Clusters contained */
	ClusterList inputClusters() const;
	/*! Returns the vector of Output Clusters contained */
	ClusterList outputClusters() const;
	/*! Returns the vector of Hidden Clusters contained (i.e. UnMarked Clusters) */
	ClusterList hiddenClusters() const;
	/*! Add Linker */
	void addLinker( Linker* l );
	/*! Remove Linker */
	bool removeLinker( Linker* );
	/*! Returns the array of Linkers contained */
	LinkerList linkers() const;
	/*! If out is true, return the Linkers outgoing from Cluster c, otherwise return incoming Linkers */
	LinkerList linkers( Cluster* c, bool out = false ) const;
	/*! Set the order */
	void setOrder( Updatable* updatables[], unsigned int dim );
	/*! Set the order */
	void setOrder( const UpdatableList& );
	/*! Return the order */
	UpdatableList order() const {
		return ups;
	};
	/*! Step */
	void step() {
		for( unsigned int i=0; i<dimUps; i++ ) {
			ups[i]->update();
		}
		foreach( NeuralNetObserver* obs, observers ) {
			obs->onStepDone();
		}
	};
	/*! This randomize the free parameters of the all elements of the neural net<br>
	 *  This method call randomize method of every Cluster and Linker inserted
	 *  \param min is the lower-bound of random number generator desired
	 *  \param max is the upper-bound of random number generator desired
	 */
	void randomize( double min, double max );
	/*! Search into the net for the presence of an Updatable with name aName;
	 *  on success set the pointer aPointer and return it,
	 *  otherwise it set aPointer to zero and return zero.<br>
	 *  This allow to use it both into an if-statement and an assignment:
	 *  \code
	 *  BiasedCluster* bias1;
	 *  BiasedCluster* bias2;
	 *  if ( byName("aName", bias1 ) ) {
	 *      //--- ok, there is a BiasedCluster with name "aName"
	 *      //--- now bias1 points to the BiasedCluster with name "aName"
	 *  } else {
	 *      //--- error, there is no BiasedCluster with that name
	 *      //--- now bias1 is nullptr
	 *  }
	 *  //--- you can also use it for assignment:
	 *  bias2 = byName("aName", bias1);
	 *  \endcode
	 */
	template<class PointerTo>
	PointerTo byName( QString aName, PointerTo& aPointer ) {
		aPointer = dynamic_cast<PointerTo>( getByName(aName) );
		return aPointer;
	};
	/*! Return the Updatable with the name specified<br>
	 *  Returns nullptr-pointer if there's no updatable object whit the name specified<br>
	 *  \warning return the first that finds. If you have named different Updatables with same name
	 *   there no way to retrieve all of them with this methods... call them with unique name ;-)
	 */
	Updatable* getByName( QString );
	/*! Return true if the Cluster is in this net
	 */
	bool find( const Cluster* ) const;
	/*! Return true if the Linker is in this net
	 */
	bool find( const Linker* ) const;
	/*! Return true if the Updatable object is in this net
	 */
	bool find( const Updatable* ) const;
	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 */
	virtual void configure();
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
protected:
	/*! name of this NeuralNet */
	QString namev;
	/*! Clusters */
	ClusterList clustersv;
	/*! Input Clusters */
	ClusterList inclusters;
	/*! Output Clusters */
	ClusterList outclusters;
	/*! unmarked Clusters */
	ClusterList hidclusters;
	/*! Linkers */
	LinkerList  linkersv;

	typedef QMap<QString, Cluster*> ClustersMap;
	/*! map name -> Cluster* */
	ClustersMap clsMap;

	typedef QMap<Cluster*, LinkerList> LinkVecMap;
	/*! mappa dei linkers entranti (cluster -> vettore linkers entranti) */
	LinkVecMap inLinks;
	/*! map of outgoing linkers (cluster -> vettore linkers uscenti) */
	LinkVecMap outLinks;

	typedef QMap<QString, Linker*> LinkersMap;
	/*! map name -> Linker* */
	LinkersMap lksMap;

	/*! Array of Updateables ordered as specified */
	UpdatableList ups;
	unsigned int dimUps;
	
	/*! the List of Observers */
	QList<NeuralNetObserver*> observers;
};

}

#endif

