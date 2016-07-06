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
#include "learningalgorithm.h"

namespace farsa {

void Pattern::setInputsOf( Cluster* cl, const DoubleVector& ins ) {
	pinfo[cl].inputs.resize( cl->numNeurons() );
	pinfo[cl].inputs = ins;
};

void Pattern::setOutputsOf( Cluster* cl, const DoubleVector& ous ) {
	pinfo[cl].outputs.resize( cl->numNeurons() );
	pinfo[cl].outputs = ous;
};

void Pattern::setInputsOutputsOf( Cluster* cl, const DoubleVector& ins, const DoubleVector& ous ) {
	pinfo[cl].inputs.resize( cl->numNeurons() );
	pinfo[cl].inputs = ins;
	pinfo[cl].outputs.resize( cl->numNeurons() );
	pinfo[cl].outputs = ous;
};

const DoubleVector& Pattern::inputsOf( Cluster* cl ) const {
	if ( pinfo.count(cl) != 0 ) {
		return pinfo[cl].inputs;
	} else {
		// FIXME: maybe we should raise an exception
		static DoubleVector vecNull;
		return vecNull;
	}
};

const DoubleVector& Pattern::outputsOf( Cluster* cl ) const {
	if ( pinfo.count(cl) != 0 ) {
		return pinfo[cl].outputs;
	} else {
		// FIXME: maybe we should raise an exception
		static DoubleVector vecNull;
		return vecNull;
	}
};

Pattern::PatternInfo& Pattern::operator[]( Cluster* cl ) {
	return pinfo[cl];
};

void Pattern::configure() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	//--- get all parameters with the prefix 'cluster:'
	QStringList clusterList = params.getParametersWithPrefixList( prefix, "cluster:" );
	foreach( QString cluster, clusterList ) {
		QString id = cluster.split(':')[1];
		if ( id.isEmpty() ) continue;
		//--- now, it check if there is a inputs and outputs parameter and load it
		QString str = params.getValue( prefix + "inputs:" + id );
		DoubleVector inputs;
		if (!str.isEmpty()) {
			QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			inputs.resize( list.size() );
			for( int i=0; i<list.size(); i++) {
				inputs[i] = list[i].toDouble();
			}
		}
		str = params.getValue( prefix + "outputs:" + id );
		DoubleVector outputs;
		if (!str.isEmpty()) {
			QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			outputs.resize( list.size() );
			for( int i=0; i<list.size(); i++) {
				outputs[i] = list[i].toDouble();
			}
		}
		if ( inputs.size() == 0 && outputs.size() == 0 ) continue;
		Cluster* cl = params.getObjectFromParameter<Cluster>( prefix+cluster, false, true );
		if ( inputs.size() > 0 ) {
			setInputsOf( cl, inputs );
		}
		if ( outputs.size() > 0 ) {
			setOutputsOf( cl, outputs );
		}
	}
}

void Pattern::save() {
	ConfigurationManager& params = configurationManager();
	QString prefix = prefixPath();
	params.startObjectParameters(prefix, "Pattern", this);
	QString tmpl = "%1:%2";
	QList<Cluster*> cls = pinfo.keys();
	for( int i=0; i<cls.size(); i++ ) {
		PatternInfo& info = pinfo[ cls[i] ];
		params.createParameter(prefix, tmpl.arg("cluster").arg(i), cls[i]);
		if ( info.inputs.size() > 0 ) {
			QStringList list;
			for( int j=0; j<info.inputs.size(); j++ ) {
				list.push_back(QString::number(info.inputs[j]));
			}
			params.createParameter(prefix, tmpl.arg("inputs").arg(i), list.join(" "));
		}
		if ( info.outputs.size() > 0 ) {
			QStringList list;
			for( int j=0; j<info.outputs.size(); j++ ) {
				list.push_back(QString::number(info.outputs[j]));
			}
			params.createParameter(prefix, tmpl.arg("outputs").arg(i), list.join(" "));
		}
	}
}

void Pattern::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Represent a pattern of inputs/outputs for Clusters", "A Pattern is specified by groups of three parameters: cluster, inputs and outputs. The inputs and outputs parameters specify the values to set on the neurons of the cluster specified by the corresponding cluster parameter. The inputs and outputs parameter are not mandatory but specify a cluster without setting inputs or outputs has no effect" );
	d.describeObject( "cluster" ).type( "Cluster" ).props( ParamIsMandatory | ParamAllowMultiple ).help( "The Cluster on which the inputs and outputs parameters referes" );
	d.describeReal( "inputs" ).props( ParamIsList | ParamAllowMultiple ).help( "The values to set on the cluster's input neurons" );
	d.describeReal( "outputs" ).props( ParamIsList | ParamAllowMultiple ).help( "The values to set on the cluster's output neurons" );
}

LearningAlgorithm::LearningAlgorithm( ConfigurationManager& params, QString prefix, Component* parent )
	: Component(params,prefix,parent) {
	this->netp = NULL;
}

LearningAlgorithm::~LearningAlgorithm() {
}

PatternSet LearningAlgorithm::loadPatternSet( ConfigurationManager& params, QString path, QString prefix ) {
	//--- convert to PatternSet
	PatternSet patternSet;
	foreach( QString group, params.getGroupsWithPrefixList(path, prefix) ) {
		patternSet.append( params.getObjectFromGroup<Pattern>( path + "/" + group ) );
	}
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning Se patternSet copia il Pattern creato all interno, allora quelli creati qui creano un leak perche non vengono mai distrutti !!
#endif
	return patternSet;
}

void LearningAlgorithm::savePatternSet( PatternSet& set, ConfigurationManager& params, QString prefix ) {
	QString tmpl = prefix+":%1";
	for( int i=0; i<set.size(); i++ ) {
		QString group = tmpl.arg(i);
		params.createGroup( group );
		set[i]->save();
	}
}

}
