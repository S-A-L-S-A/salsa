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

#include "nnfwconfig.h"
#include "simplecluster.h"
#include "biasedcluster.h"
#include "copylinker.h"
#include "dotlinker.h"
#include "normlinker.h"
#include "liboutputfunctions.h"
#include "libperiodicfunctions.h"
#include "libradialfunctions.h"
#include "libcompetitivefunctions.h"
#include "learningalgorithm.h"
#include "backpropagationalgo.h"
#include "typesdb.h"
#include "logger.h"

#include <QStringList>
#include <QDebug>

namespace salsa {

bool SALSA_NNFW_API initNNFWLib() {
	static bool dummy = false;
	if ( dummy ) return true;
	// Registering all types to factory
	TypesDB::getInstance().registerComponent<NeuralNet>("NeuralNet", "Component");
	TypesDB::getInstance().registerComponent<Pattern>("Pattern", "Component");
	TypesDB::getInstance().registerComponent<LearningAlgorithm>("LearningAlgorithm", "Component");
	TypesDB::getInstance().registerComponent<BackPropagationAlgo>("BackPropagationAlgo", "LearningAlgorithm");
	// Clusters
	TypesDB::getInstance().registerComponent<Updatable>("Updatable", "ParameterSettableInConstructor");
	TypesDB::getInstance().registerComponent<Cluster>("Cluster", "Updatable");
	TypesDB::getInstance().registerComponent<SimpleCluster>("SimpleCluster", "Cluster");
	TypesDB::getInstance().registerComponent<BiasedCluster>("BiasedCluster", "Cluster");

	// Linkers
	TypesDB::getInstance().registerComponent<Linker>("Linker", "Updatable");
	TypesDB::getInstance().registerComponent<CopyLinker>("CopyLinker", "Linker");
	TypesDB::getInstance().registerComponent<MatrixLinker>("MatrixLinker", "Linker");
	TypesDB::getInstance().registerComponent<DotLinker>("DotLinker", "MatrixLinker");
	TypesDB::getInstance().registerComponent<NormLinker>("NormLinker", "MatrixLinker");

	// Output Functions
	TypesDB::getInstance().registerComponent<OutputFunction>("OutputFunction", "Component");
	
	TypesDB::getInstance().registerComponent<FakeSigmoidFunction>("FakeSigmoidFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<IdentityFunction>("IdentityFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<GainFunction>("GainFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<LinearFunction>("LinearFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<RampFunction>("RampFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<ScaleFunction>("ScaleFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<ScaledSigmoidFunction>("ScaledSigmoidFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<SigmoidFunction>("SigmoidFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<StepFunction>("StepFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<LeakyIntegratorFunction>("LeakyIntegratorFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<LogLikeFunction>("LogLikeFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<CompositeFunction>("CompositeFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<LinearComboFunction>("LinearComboFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<GaussFunction>("GaussFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<PeriodicFunction>("PeriodicFunction", "OutputFunction");
	TypesDB::getInstance().registerComponent<PseudoGaussFunction>("PseudoGaussFunction", "PeriodicFunction");
	TypesDB::getInstance().registerComponent<SawtoothFunction>("SawtoothFunction", "PeriodicFunction");
	TypesDB::getInstance().registerComponent<SinFunction>("SinFunction", "PeriodicFunction");
	TypesDB::getInstance().registerComponent<TriangleFunction>("TriangleFunction", "PeriodicFunction");
	TypesDB::getInstance().registerComponent<WinnerTakeAllFunction>("WinnerTakeAllFunction", "OutputFunction");
	dummy = true;
	Logger::info( "Library NNFW Initialized" );
	return true;
}

}
