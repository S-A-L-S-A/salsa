/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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
#include "neuralnetui.h"
#include "configurationhelper.h"
#include "logger.h"

#ifdef SALSA_USE_GRAPHVIZ
	#include "neuralnetconfwidget.h"
#endif

namespace salsa {

NeuralNet::NeuralNet(ConfigurationManager& params, QString prefix, Component* parent) :
	Component(params,prefix,parent) {
	dimUps = 0;
}

NeuralNet::~NeuralNet() {
}

ComponentUI* NeuralNet::getUIManager() {
#ifdef SALSA_USE_GRAPHVIZ
	return new NeuralNetUI( this );
#else
	return nullptr;
#endif
}

void NeuralNet::addObserver( NeuralNetObserver* observer ) {
	if ( observers.contains(observer) ) return;
	observers.append( observer );
}

void NeuralNet::removeObserver( NeuralNetObserver* observer ) {
	observers.removeAll( observer );
}

void NeuralNet::addCluster( Cluster* c, bool isInput, bool isOutput ) {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to addCluster! This operation will be ignored" );
		return;
	}
#endif
	// Check if the Cluster is already added
	if ( find( c ) ) {
#ifdef SALSA_DEBUG
		Logger::warning( "Cluster already added! addCluster will be ignored" );
#endif
		return;
	}
	clustersv.append( c );
	if ( isInput ) {
		inclusters.append( c );
	}
	if ( isOutput ) {
		outclusters.append( c );
	}
	if ( !isInput && !isOutput ) {
		hidclusters.append( c );
	}
	clsMap[c->name()] = c;
	return;
}

bool NeuralNet::removeCluster( Cluster* c ) {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to removeCluster! This operation will return false" );
		return false;
	}
#endif
	if ( !find( c ) ) {
		return false;
	}
	clustersv.removeOne( c );
	inclusters.removeOne( c );
	outclusters.removeOne( c );
	hidclusters.removeOne( c );
	clsMap.remove( c->name() );
	return true;
}

void NeuralNet::markAsInput( Cluster* c ) {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to addCluster! This operation will be ignored" );
		return;
	}
#endif
	// Check if the Cluster exists
	if ( !find( c ) ) {
#ifdef SALSA_DEBUG
		Logger::warning( "attempt to mark a Cluster not present in this net!" );
#endif
		return;
	}
	if ( inclusters.contains( c ) ) {
		return;
	}
	inclusters.append( c );
	hidclusters.removeOne( c );
}

void NeuralNet::markAsOutput( Cluster* c ) {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to addCluster! This operation will be ignored" );
		return;
	}
#endif
	// Check if the Cluster exists
	if ( !find( c ) ) {
#ifdef SALSA_DEBUG
		Logger::warning( "attempt to mark a Cluster not present in this net!" );
#endif
		return;
	}
	if ( outclusters.contains( c ) ) {
		return;
	}
	outclusters.append( c );
	hidclusters.removeOne( c );
}

void NeuralNet::unmark( Cluster* c ) {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to addCluster! This operation will be ignored" );
		return;
	}
#endif
	// Check if the Cluster exists
	if ( !find( c ) ) {
#ifdef SALSA_DEBUG
		Logger::warning( "attempt to unmark a Cluster not present in this net!" );
#endif
		return;
	}
	inclusters.removeOne( c );
	outclusters.removeOne( c );
	hidclusters.append( c );
	return;
}

void NeuralNet::unmarkAll( ) {
	inclusters.clear();
	outclusters.clear();
	hidclusters = clustersv;
	return;
}

bool NeuralNet::isIsolated( Cluster* c ) const {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to isIsolato! This operation will return false" );
		return false;
	}
#endif
	return ( inLinks.count( c ) == 0 && outLinks.count( c ) == 0 );
}

ClusterList NeuralNet::clusters() const {
	return clustersv;
}

ClusterList NeuralNet::inputClusters() const {
	return inclusters;
}

ClusterList NeuralNet::outputClusters() const {
	return outclusters;
}

ClusterList NeuralNet::hiddenClusters() const {
	return hidclusters;
}

void NeuralNet::addLinker( Linker* l ) {
#ifdef SALSA_DEBUG
	if ( !l ) {
		Logger::warning( "Null Pointer passed to addLinker! This operation will be ignored" );
		return;
	}
#endif
	// Check if the Linker is already added
	if ( find( l ) ) {
#ifdef SALSA_DEBUG
		Logger::warning( "Linker already added! addLinker will be ignored" );
#endif
		return;
	}
#ifdef SALSA_DEBUG
	// --- Check: Are There in this net the Clusters that linker l connects ???
	if ( ! find( l->from() ) ) {
		Logger::warning( QString("The linker that you want add links cluster ")+l->from()->name()+" that doesn't exist in this net! This operation will be ignored" );
		return;
	}
	if ( ! find( l->to() ) ) {
		Logger::warning( QString("The linker that you want add links cluster ")+l->to()->name()+" that doesn't exist in this net! This operation will be ignored" );
		return;
	}
#endif
	linkersv.append( l );
	// Adding information in outLinks map
	outLinks[ l->from() ].append( l );
	// Adding information in inLinks map
	inLinks[ l->to() ].append( l );
	lksMap[l->name()] = l;
	return;
}

bool NeuralNet::removeLinker( Linker* l ) {
#ifdef SALSA_DEBUG
	if ( !l ) {
		Logger::warning( "Null Pointer passed to removeLinker! This operation will return false" );
		return false;
	}
#endif
	if ( !find(l) ) {
		return false;
	}
	linkersv.removeOne( l );
	outLinks[ l->from() ].removeOne( l );
	inLinks[ l->to() ].removeOne( l );
	lksMap.remove( l->name() );
	return true;
}

LinkerList NeuralNet::linkers() const {
	return linkersv;
}

LinkerList NeuralNet::linkers( Cluster* c, bool out ) const {
#ifdef SALSA_DEBUG
	if ( !c ) {
		Logger::warning( "Null Pointer passed to linkers! This operation will return an empty LinkerList" );
		return LinkerList();
	}
#endif
	if ( out ) {
		// Return outgoing linkers
		if ( outLinks.contains( c ) ) {
			return outLinks[c];
		}
	} else {
		// Return incoming linkers
		if ( inLinks.contains( c ) ) {
			return inLinks[c];
		}
	}
	return LinkerList();
}

void NeuralNet::setOrder( Updatable* u[], unsigned int dim ) {
	ups.clear();
	for( unsigned int i = 0; i<dim; i++ ) {
		if ( find( u[i] ) ) {
			ups.append( u[i] );
		}
#ifdef SALSA_DEBUG
		else {
			Logger::warning( "In the Updatable order list passed there are some Clusters and/or Linkers not present in this NeuralNet" );
		}
#endif
	}
	dimUps = ups.size();
	return;
}

void NeuralNet::setOrder( const UpdatableList& u ) {
	ups.clear();
	unsigned int dim = u.size();
	for( unsigned int i = 0; i<dim; i++ ) {
		if ( find( u[i] ) ) {
			ups.append( u[i] );
		}
#ifdef SALSA_DEBUG
		else {
			Logger::warning( "In the Updatable order list passed there are some Clusters and/or Linkers not present in this NeuralNet");
		}
#endif
	}
	dimUps = ups.size();
	return;
}

void NeuralNet::randomize( double min, double max ) {
	int dim = clustersv.size();
	for( int i=0; i<dim; i++ ) {
		clustersv[i]->randomize( min, max );
	}
	dim = linkersv.size();
	for( int i=0; i<dim; i++ ) {
		linkersv[i]->randomize( min, max );
	}
}

Updatable* NeuralNet::getByName( QString name ) {
	if ( clsMap.contains( name ) ) {
		return clsMap[name];
	}
	if ( lksMap.contains( name ) ) {
		return lksMap[name];
	}
#ifdef SALSA_DEBUG
	Logger::warning( QString("getByName: Updatable ")+name+" not present in BaseNeuralNet" );
#endif
	return nullptr;
}

bool NeuralNet::find( const Cluster* cl ) const {
	return clustersv.contains( (Cluster*)cl );
}

bool NeuralNet::find( const Linker* l ) const {
	return linkersv.contains( (Linker*)l );
}

bool NeuralNet::find( const Updatable* u ) const {
	return (
		clustersv.contains( (Cluster*)u ) ||
		linkersv.contains( (Linker*)u )
	);
}

void NeuralNet::configure() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	// take the name from the property if has been setted
	QString myname = ConfigurationHelper::getString( params, prefix+"name", "NeuralNetwork" );
	setName( myname );
	
	QStringList inputList = ConfigurationHelper::getStringList( params, prefix+"inputClusters" );
	QStringList outputList = ConfigurationHelper::getStringList( params, prefix+"outputClusters" );
	QStringList spreadList = ConfigurationHelper::getStringList( params, prefix+"spreadOrder" );
	
	// load all Cluster first
	QStringList clustersGroup = params.getGroupsWithPrefixList( prefix, "CLUSTER:" );
	foreach( QString subgroup, clustersGroup ) {
		Cluster* cl = params.getObjectFromGroup<Cluster>( prefix+subgroup, true );
		// if the name is "unamed" it force to correspond to group name
		if ( cl->name() == "unamed" ) {
			cl->setName( subgroup );
		}
		// it is possible to specify the name of the group or the name of the cluster
		bool isInput = (inputList.removeAll( cl->name() ) > 0) || (inputList.removeAll(subgroup) > 0);
		bool isOutput = (outputList.removeAll( cl->name() ) > 0) || (outputList.removeAll(subgroup) > 0);
		addCluster( cl, isInput, isOutput );
	}
	// inputList and outputList should be empty
	if ( inputList.size() > 0 ) {
		Logger::warning( "NeuralNet Configure - the inputClusters contains Cluster that are not part of this Neural Network: "+inputList.join(", ") );
	}
	if ( outputList.size() > 0 ) {
		Logger::warning( "NeuralNet Configure - the outputClusters contains Cluster that are not part of this Neural Network: "+outputList.join(", ") );
	}
	
	// load all Linkers
	QStringList linkersGroup = params.getGroupsWithPrefixList( prefix, "LINKER:" );
	foreach( QString subgroup, linkersGroup ) {
		Linker* lnk = params.getObjectFromGroup<Linker>( prefix+subgroup, true );
		// if the name is "unamed" it force to correspond to group name
		if ( lnk->name() == "unamed" ) {
			lnk->setName( subgroup );
		}
		addLinker( lnk );
	}

	// create the spread order
	ups.clear();
	QStringList missed;
	foreach( QString upname, spreadList ) {
		Updatable* up = getByName(upname);
		if ( !up ) {
			missed << upname;
		} else {
			ups.append( up );
		}
	}
	dimUps = ups.size();
	if ( missed.size() > 0 ) {
		Logger::warning( "NeuralNet Configure - the spreadOrder contains Clusters and/or Linkers that are not part of this Neural Network: "+spreadList.join(", ") );
	}
	markAsConfigured();
}

void NeuralNet::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters( prefix, "NeuralNet", this );

	//--- save all Clusters
	foreach( Cluster* cl, clustersv ) {
		cl->save();
	}
	//--- save all Linkers
	foreach( Linker* ln, linkersv ) {
		ln->save();
	}
	//--- save parameter inputClusters
	QStringList list;
	foreach( Cluster* cl, inclusters ) {
		list << cl->name();
	}
	params.createParameter( prefix, "inputClusters", list.join(" ") );
	//--- save parameter outputClusters
	list.clear();
	foreach( Cluster* cl, outclusters ) {
		list << cl->name();
	}
	params.createParameter( prefix, "outputClusters", list.join(" ") );
	//--- save parameter spreadOrder
	list.clear();
	foreach( Updatable* up, ups ) {
		list << up->name();
	}
	params.createParameter( prefix, "spreadOrder", list.join(" ") );
}

void NeuralNet::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Neural Network", "The neural network is a container for Clusters and Linkers. The topology and connectivity of the neural network is specified by the Linkers contained, while the order on which the Clusters and Linkers are updated is specified by the spreadOrder parameter of neural network" );
	d.describeString("name").def("NeuralNetwork").help("The name of the neural network");
	d.describeString( "inputClusters" ).props( ParamIsList ).help( "The list names of Clusters marked as input of the neural network" );
	d.describeString( "outputClusters" ).props( ParamIsList ).help( "The list names of Clusters marked as output of the neural network" );
	d.describeString( "spreadOrder" ).props( ParamIsMandatory | ParamIsList ).help( "The order on which the Clusters and Linkers are updated" );
	d.describeSubgroup( "CLUSTER" ).type( "Cluster" ).props( ParamIsList ).help( "A Cluster inside this neural network" );
	d.describeSubgroup( "LINKER" ).type( "Linker" ).props( ParamIsList ).help( "A Linker inside this neural network" );

#ifdef SALSA_USE_GRAPHVIZ
	// set the graphical editor
	setGraphicalEditor<NeuralNetConfWidget>(type);
#endif

}

}
