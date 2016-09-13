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

#ifndef LEARNINGALGORITHM_H
#define LEARNINGALGORITHM_H

#include "nnfwconfig.h"
#include "neuralnet.h"
#include "configurationmanager.h"
#include <QMap>
#include <QVector>
#include <cmath>

namespace salsa {

class NeuralNet;

/*! \brief Pattern object
 *
 *  The Pattern object represent a configuration (partial or complete) of the network that can
 *  be used by a learning algorithm as model to learn.
 *  A LearningAlgorithm will use Pattern for setup network's inputs, spread network and eventually
 *  calculate the error, and modify the network's paramenters on the basis of neurons activities
 *  in order to get closer to neuron activities presented by the Pattern
 *
 *  Pattern simply associates inputs/outputs pairs to Clusters:
 *  \code
 *  Pattern pat;
 *  pat->setInputsOf( anInputCluster, Inputs );
 *  pat->setOutputsOf( anOutputCluster, Outputs );
 *  pat->setInputOutputsOf( anHiddenClusterToReset, Inputs, Outputs );
 *  //--- retrieve stored information:
 *  pat[aCluster].inputs;
 *  pat[anotherCluster].outputs;
 *  //--- add/modify informations:
 *  pat[aNewCluster].outputs.assign( aRealVecOfData );
 *  pat[aNewCluster].outputs += 3.0;
 *  \endcode
 *
 *  \warning Pay attention when you use operator[] because it silently add new data. Like QMap::operator[]
 *
 */
class SALSA_NNFW_API Pattern : public Component {
public:
	class PatternInfo {
	public:
		DoubleVector inputs;
		DoubleVector outputs;
	};
	/*! Construct an empty Pattern */
	Pattern(ConfigurationManager& params, QString prefix, Component* parent = nullptr) : 
		Component(params,prefix,parent), pinfo() { /*nothing else to do*/ };
	/*! Destructor */
	~Pattern() { /*nothing to do*/ };
	/*! set the inputs associated with Cluster passed */
	void setInputsOf( Cluster*, const DoubleVector& );
	/*! set the outputs associated with Cluster passed */
	void setOutputsOf( Cluster*, const DoubleVector& );
	/*! set the both inputs and outputs associated with Cluster passed */
	void setInputsOutputsOf( Cluster*, const DoubleVector& inputs, const DoubleVector& outputs );
	/*! return stored information if exists, otherwise it return a zero vector */
	const DoubleVector& inputsOf( Cluster* ) const;
	/*! return stored information if exists, otherwise it return a zero vector */
	const DoubleVector& outputsOf( Cluster* ) const;
	/*! return the stored information
	 *  \warning it silently create a new one if the Cluster passed is not present */
	PatternInfo& operator[]( Cluster* );
	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 * 
	 * The Pattern has to be declared using a sequence of parameters with the following schema:
	 * \code
	 * [aPattern]
	 * cluster:1 = nameOfCluster
	 * inputs:1 = list of the inputs associated to cluster:1
	 * outputs:1 = list of the outputs associated to cluster:1
	 * ...
	 * cluster:i = nameOf_ith_Cluster
	 * inputs:i = list of the inputs associated to cluster:i
	 * outputs:i = list of the outputs associated to cluster:i
	 * \endcode
	 * Essentialy, the parameters are grouped using the identifier string after the ':' char.
	 * And the inputs and outputs with a given identifier are associated to the cluster with the same identifier.
	 * If a cluster:i is present, then at least inputs:i or outputs:i parameter must be present
	 *
	 */
	virtual void configure();
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 *
	 */
	virtual void save();
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );
private:
	mutable QMap<Cluster*, PatternInfo> pinfo;
};

/*! \brief PatternSet type
 *  It represent a collection of Pattern object. It could be a Learning Set or a Training Set
 *  PatternSet is simply a QVector Pattern objects
 */
typedef QVector<Pattern*> PatternSet;

/*! \brief LearningAlgorithm object
 *
 *  The LearningAlgorithm object is a the abstract class from which to implement learning algorithms
 */
class SALSA_NNFW_API LearningAlgorithm : public Component {
public:
	/*! Constructor */
	LearningAlgorithm( ConfigurationManager& params, QString prefix, Component* parent = nullptr );
	/*! Constructor */
	LearningAlgorithm();
	/*! Destructor */
	virtual ~LearningAlgorithm();
	/*! Set the NeuralNet to learn */
	void setNeuralNet( NeuralNet* net ) {
		netp = net;
		this->neuralNetChanged();
	};
	/*! Return the NeuralNet setted */
	NeuralNet* neuralNet() {
		return netp;
	};
	/*! a single step of learning algorithm */
	virtual void learn() = 0;
	/*! Modify the object tring to learn the pattern passed */
	virtual void learn( Pattern* ) = 0;
	/*! Modify the object tring to learn all patterns present into PatternSet passed */
	virtual void learnOnSet( const PatternSet& set ) {
		for( int i=0; i<(int)set.size(); i++ ) {
			learn( set[i] );
		}
	};
	/*! Calculate the Mean Square Error respect to Pattern passed */
	virtual double calculateMSE( Pattern* ) = 0;
	/*! Calculate the Mean Square Error respect to all Patterns passed */
	virtual double calculateMSEOnSet( const PatternSet& set ) {
		double mseacc = 0.0;
		int dim = (int)set.size();
		for( int i=0; i<dim; i++ ) {
			mseacc += calculateMSE( set[i] );
		}
		return mseacc/dim;
	};
	/*! Calculate the Root Mean Square Deviation, i.e. the square root of MSE */
	double calculateRMSD( Pattern* p ) {
		return sqrt( calculateMSE( p ) );
	};
	/*! Calculate the Root Mean Square Deviation, i.e. the square root of MSE */
	double calculateRMSDOnSet( const PatternSet& p ) {
		return sqrt( calculateMSEOnSet( p ) );
	};
	/*! Utility function for loading a PatternSet from a ConfigurationManager */
	PatternSet loadPatternSet( ConfigurationManager& params, QString path, QString prefix );
	/*! Utility function for saving a PatternSet to a ConfigurationManager */
	void savePatternSet( PatternSet& set, ConfigurationManager& params, QString prefix );
protected:
	/*! Implemented by subclasses for configuring internal structure when the NeuralNet has been setted */
	virtual void neuralNetChanged() = 0;
private:
	NeuralNet* netp;
};

}

#endif

