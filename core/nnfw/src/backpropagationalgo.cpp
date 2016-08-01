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

#include "neuralnet.h"
#include "matrixlinker.h"
#include "biasedcluster.h"
#include "backpropagationalgo.h"

namespace salsa {

BackPropagationAlgo::BackPropagationAlgo(ConfigurationManager& params, QString prefix, Component* parent)
	: LearningAlgorithm(params,prefix,parent), learn_rate(0.0) {
	useMomentum = false;
	momentumv = 0.0f;
	neuralNetChanged();
}

BackPropagationAlgo::~BackPropagationAlgo( ) {
	/* nothing to do ?!?! */
}

void BackPropagationAlgo::neuralNetChanged() {
	NeuralNet* net = neuralNet();
	if ( !net ) return;
	//--- clear all data
	mapIndex.clear();
	cluster_deltas_vec.clear();
	//--- insert new data for the new NeuralNet
	Cluster *cluster_temp;
	// pushing the info for output cluster
	ClusterList outs = net->outputClusters();
	for( int i=0; i<(int)outs.size(); i++ ) {
		addCluster( outs[i], true );
	}
	// --- generate information for backpropagation of deltas
	for( int i=0; i<(int)update_order.size(); i++ ) {
		cluster_temp = dynamic_cast<Cluster*>(update_order[i]);
		if ( cluster_temp ) {
			addCluster( cluster_temp, false );
			continue;
		}
		MatrixLinker* linker_temp = dynamic_cast<MatrixLinker*>(update_order[i]);
		if ( linker_temp ) {	//Dot linker subclass of Matrixlinker
			addLinker( linker_temp );
		}
	}
}

void BackPropagationAlgo::setUpdateOrder( const UpdatableList& update_order ) {
	this->update_order = update_order;
	this->neuralNetChanged();
}

void BackPropagationAlgo::setTeachingInput( Cluster* output, const DoubleVector& ti ) {
	if ( mapIndex.count( output ) == 0 ) { 
		return;
	}
	int index = mapIndex[ output ];
	cluster_deltas_vec[index].deltas_outputs = output->outputs() - ti;
	return;
}

DoubleVector BackPropagationAlgo::getError( Cluster* cl ) {
	if ( mapIndex.count( cl ) == 0 ) {
		qWarning() << "Cluster not present in BackPropagationAlgo";
		return DoubleVector();
	}
	int index = mapIndex[ cl ];
	return cluster_deltas_vec[index].deltas_outputs;
}

void BackPropagationAlgo::enableMomentum() {
	for ( int i=0; i<cluster_deltas_vec.size(); ++i ) {
		for ( int j=0;  j<cluster_deltas_vec[i].incoming_linkers_vec.size(); ++j ) {
			// --- zeroing data
			cluster_deltas_vec[i].incoming_last_outputs[j].setZero();
			cluster_deltas_vec[i].last_deltas_inputs.setZero();
		}
	}
	useMomentum = true;
}

void BackPropagationAlgo::propagDeltas() {
	DoubleVector diff_vec;
	for( int i=0; i<(int)cluster_deltas_vec.size(); i++ ) {
		cluster_deltas_vec[i].incoming_linkers_vec;
		// --- propagate DeltaOutput to DeltaInputs
		cluster_deltas_vec[i].deltas_inputs = cluster_deltas_vec[i].deltas_outputs;
		Cluster* cl = cluster_deltas_vec[i].cluster;
		OutputFunction* func = cl->outFunction();
		diff_vec.resize( cluster_deltas_vec[i].deltas_inputs.size() );
		if ( func->derivate( cl->inputs(), cl->outputs(), diff_vec ) ) {
			cluster_deltas_vec[i].deltas_inputs = cluster_deltas_vec[i].deltas_inputs.cwiseProduct(diff_vec);
		}
		// --- propagate DeltaInputs to DeltaOutput through MatrixLinker
		for( int k=0; k<cluster_deltas_vec[i].incoming_linkers_vec.size( ); ++k ) {
			MatrixLinker* link = dynamic_cast<MatrixLinker*>(cluster_deltas_vec[i].incoming_linkers_vec[k]);
			if ( mapIndex.count(link->from()) == 0 ) {
				// --- the from() cluster is not in Learning
				continue;
			}
			int from_index = mapIndex[ link->from() ];
			cluster_deltas_vec[from_index].deltas_outputs = link->matrix()*cluster_deltas_vec[i].deltas_inputs;
		}
	}
	return;
}

void BackPropagationAlgo::learn() {
	// --- zeroing previous step delta information
	for ( int i=0; i<cluster_deltas_vec.size(); ++i ) {
		if ( cluster_deltas_vec[i].isOutput ) continue;
		cluster_deltas_vec[i].deltas_outputs.setZero();
	}
	// --- propagating the error through the net
	propagDeltas();
	// --- make the learn !!
	for ( int i=0; i<cluster_deltas_vec.size(); ++i ) {
		if ( cluster_deltas_vec[i].cluster != NULL) {
			for( unsigned int b=0; b<cluster_deltas_vec[i].cluster->numNeurons(); b++ ) {
				cluster_deltas_vec[i].cluster->biases()[b] += -learn_rate*-cluster_deltas_vec[i].deltas_inputs[b];
			}
		}

		for ( int j=0;  j<cluster_deltas_vec[i].incoming_linkers_vec.size(); ++j ) {
			if ( cluster_deltas_vec[i].incoming_linkers_vec[j] != NULL ) {
				DoubleVector& outputs = cluster_deltas_vec[i].incoming_linkers_vec[j]->from()->outputs();
				DoubleVector& inputs = cluster_deltas_vec[i].deltas_inputs;
				DoubleMatrix& matrix = cluster_deltas_vec[i].incoming_linkers_vec[j]->matrix();
				for ( int r=0; r<outputs.size(); r++ ) {
					for ( int c=0; c<inputs.size(); c++ ) {
						matrix(r,c) += -learn_rate * outputs[r] * inputs[c];
					}
				}
				if ( !useMomentum ) continue;
				// --- add the momentum
				outputs = cluster_deltas_vec[i].incoming_last_outputs[j];
				inputs = cluster_deltas_vec[i].last_deltas_inputs;
				matrix = cluster_deltas_vec[i].incoming_linkers_vec[j]->matrix();
				for ( int r=0; r<outputs.size(); r++ ) {
					for ( int c=0; c<inputs.size(); c++ ) {
						matrix(r,c) += -learn_rate*momentumv * outputs[r] * inputs[c];
					}
				}
				// --- save datas for momentum on the next step
				cluster_deltas_vec[i].incoming_last_outputs[j] = cluster_deltas_vec[i].incoming_linkers_vec[j]->from()->outputs();
				cluster_deltas_vec[i].last_deltas_inputs = cluster_deltas_vec[i].deltas_inputs;
			}
		}
	}
	return;
}

void BackPropagationAlgo::learn( Pattern* pat ) {
	// --- set the inputs of the net
	ClusterList clins = neuralNet()->inputClusters();
	for( int i=0; i<clins.size(); i++ ) {
		clins[i]->inputs() = pat->inputsOf( clins[i] );
	}
	// --- spread the net
	neuralNet()->step();
	// --- set the teaching input
	ClusterList clout = neuralNet()->outputClusters();
	for( int i=0; i<clout.size(); i++ ) {
		setTeachingInput( clout[i], pat->outputsOf( clout[i] ) );
	}
	learn();
}

double BackPropagationAlgo::calculateMSE( Pattern* pat ) {
	// --- set the inputs of the net
	ClusterList clins = neuralNet()->inputClusters();
	for( int i=0; i<clins.size(); i++ ) {
		clins[i]->inputs() = pat->inputsOf( clins[i] );
	}
	// --- spread the net
	neuralNet()->step();
	// --- calculate the MSE
	ClusterList clout = neuralNet()->outputClusters();
	double mseacc = 0.0;
	int dim = (int)clout.size();
	for( int i=0; i<dim; i++ ) {
		DoubleVector diff = clout[i]->outputs()-pat->outputsOf( clout[i] );
		mseacc += diff.dot(diff)/diff.size();
	}
	return mseacc/dim;
}

void BackPropagationAlgo::addCluster( Cluster* cl, bool isOut ) {
	if( mapIndex.count( cl ) == 0 ) {
		cluster_deltas temp;
		int size = cl->numNeurons();
		temp.cluster = dynamic_cast<BiasedCluster*>(cl);
		temp.isOutput = isOut;
		temp.deltas_outputs.resize( size );
		temp.deltas_inputs.resize( size );
		temp.last_deltas_inputs.resize( size );
		cluster_deltas_vec.push_back( temp );
		mapIndex[cl] = cluster_deltas_vec.size()-1;
	}
}

void BackPropagationAlgo::addLinker( Linker* link ) {
	if ( mapIndex.count( link->to() ) == 0 ) {
		cluster_deltas temp;
		int size = link->to()->numNeurons();
		temp.cluster = dynamic_cast<BiasedCluster*>(link->to());
		temp.isOutput = false;
		temp.deltas_outputs.resize( size );
		temp.deltas_inputs.resize( size );
		temp.last_deltas_inputs.resize( size );
		temp.incoming_linkers_vec.push_back( dynamic_cast<MatrixLinker*>(link) );
		temp.incoming_last_outputs.push_back( DoubleVector( link->from()->numNeurons() ) );
		cluster_deltas_vec.push_back( temp );
		mapIndex[temp.cluster] = cluster_deltas_vec.size()-1;
	}
	else {
		int tmp = mapIndex[link->to()];
		cluster_deltas_vec[ tmp ].incoming_linkers_vec.push_back( dynamic_cast<MatrixLinker*>(link) );
		cluster_deltas_vec[ tmp ].incoming_last_outputs.push_back( DoubleVector( link->from()->numNeurons() ) );
	}
}

void BackPropagationAlgo::configure() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	learn_rate = params.getValue( prefix + "rate" ).toDouble();
	momentumv = params.getValue( prefix + "momentum" ).toDouble();
	if ( momentumv == 0.0 ) {
		useMomentum = false;
	} else {
		useMomentum = true;
	}
	QString str = params.getValue( prefix + "order" );
	update_order.clear();
	if ( !str.isEmpty() ) {
		QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		foreach( QString upl, list ) {
			Updatable* up = params.getObjectFromGroup<Updatable>( upl, true );
			update_order << up;
		}
	}
	NeuralNet* net = params.getObjectFromParameter<NeuralNet>( prefix+"neuralnet", false, true );
	setNeuralNet( net );
}

void BackPropagationAlgo::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "BackPropagationAlgo", this);
	params.createParameter( prefix, "neuralnet", neuralNet() );
	params.createParameter( prefix, "rate", QString::number(learn_rate) );
	if ( useMomentum ) {
		params.createParameter( prefix, "momentum", QString::number(momentumv) );
	}
	QStringList list;
	foreach( Updatable* up, update_order ) {
		list << up->name();
	}
	params.createParameter( prefix, "order", list.join(" ") );
	//--- save the neuralnet in the group corresponding to its name
	neuralNet()->save();
}

void BackPropagationAlgo::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Backpropagation Learning Algorithm" );
	d.describeObject( "neuralnet" ).type( "NeuralNet" ).props( ParamIsMandatory ).help( "The neural network to learn by backpropagation" );
	d.describeReal( "rate" ).limits( 0.0, 1.0 ).def( 0.2 ).help( "The learning rate" );
	d.describeReal( "momentum" ).limits( 0.0, 1.0 ).help( "The momentum rate; if zero momentum will be disabled" );
}

}
