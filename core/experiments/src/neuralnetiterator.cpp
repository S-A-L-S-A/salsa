// /********************************************************************************
//  *  FARSA Experiments Library                                                   *
//  *  Copyright (C) 2007-2013                                                     *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #include "neuralnetiterator.h"
// #include "logger.h"
//
// namespace farsa {
//
// NeuralNetIterator::NeuralNetIterator() :
// 	neuralnet(NULL),
// 	cluster(NULL),
// 	currIndex(-1) {
// }
//
// NeuralNetIterator::~NeuralNetIterator() {
// 	/* nothing to do */
// }
//
// void NeuralNetIterator::setNeuralNet( NeuralNet* neuralnet ) {
// 	this->neuralnet = neuralnet;
// 	cluster = NULL;
// 	currIndex = -1;
// }
//
// bool NeuralNetIterator::setCurrentBlock( QString blockName ) {
// 	neuralnet->byName( blockName, cluster );
// 	if ( !cluster ) {
// 		currIndex = -1;
// 		Logger::error( QString("NeuralNetIterator - the block %1 does not exist").arg(blockName) );
// 		return false;
// 	}
// 	currIndex = 0;
// 	return true;
// }
//
// bool NeuralNetIterator::nextNeuron() {
// 	checkCurrentStatus( "nextNeuron" );
//
// 	currIndex++;
// 	if ( currIndex >= int(cluster->numNeurons()) ) {
// 		return false;
// 	}
// 	return true;
// }
//
// void NeuralNetIterator::setInput( double value ) {
// 	checkCurrentStatus( "setInput" );
//
// 	cluster->setInput( currIndex, value );
// }
//
// double NeuralNetIterator::getInput() {
// 	checkCurrentStatus( "getInput" );
//
// 	return cluster->getInput( currIndex );
// }
//
// double NeuralNetIterator::getOutput() {
// 	checkCurrentStatus( "getOutput" );
// 	return cluster->getOutput( currIndex );
// }
//
// void NeuralNetIterator::setGraphicProperties( QString /*label*/, double /*minValue*/, double /*maxValue*/, QColor /*color*/ ) {
// 	checkCurrentStatus( "setGraphicProperties" );
// 	Logger::warning("NeuralNetIterator - it is not possible to set graphics properties for NeuralNet");
// }
//
// void NeuralNetIterator::checkCurrentStatus( const QString& funcName ) const {
// 	if ( !neuralnet ) {
// 		throw EvonetIteratorInvalidStatusException( funcName.toLatin1().data(), "no NeuralNet object has ben set");
// 	}
// 	if ( !cluster ) {
// 		throw EvonetIteratorInvalidStatusException( funcName.toLatin1().data(), "you should call setCurrentBlock first");
// 	}
// 	if ( currIndex >= int(cluster->numNeurons()) ) {
// 		throw EvonetIteratorInvalidStatusException( funcName.toLatin1().data(), "attempt to access beyond the size of the current block");
// 	}
// }
//
// } // end namespace farsa
