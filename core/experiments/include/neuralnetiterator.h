// /********************************************************************************
//  *  SALSA Experimentes Library                                                  *
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
// #ifndef NEURALNETITERATOR_H
// #define NEURALNETITERATOR_H
//
// #include "neuroninterfaces.h"
// #include "neuralnet.h"
// #include <QString>
// #include <QColor>
//
// namespace salsa {
//
// /*! \brief This class iterate over the neurons of a NNFW neural network
//  *
//  *  The blocks are identified by the name of the Cluster
//  */
// class SALSA_EXPERIMENTS_API NeuralNetIterator : public NeuronsIterator {
// public:
// 	/*! Constructor */
// 	NeuralNetIterator();
// 	/*! Destructor */
// 	virtual ~NeuralNetIterator();
// 	/*! Set the NeuralNet on which iterate */
// 	void setNeuralNet( salsa::NeuralNet* neuralnet );
// 	/*! Set the current blocks (Cluster) of neurons to iterate
// 	 *
// 	 *  You need to first call this method for iterate over neurons.
// 	 *  \param blockName is the name given to the Cluster on which iterate
// 	 *  \return true if the current block is now the block defined with name blockName; false otherwise
// 	 */
// 	bool setCurrentBlock( QString blockName );
// 	/*! Go to the next neuron of the current block
// 	 * \return true if a next neuron exist and set it as current neuron;
// 	 *  false when it has been reached the end of current block
// 	 * \note if you need to go back at the beginning, use setCurrentBlock again
// 	 */
// 	bool nextNeuron();
// 	/*! Set the input of the current neuron */
// 	void setInput( double value );
// 	/*! Get the input of the current neuron */
// 	double getInput();
// 	/*! Get the output of the current neuron */
// 	double getOutput();
// 	/*! Set the graphic properties for the current neuron (in case it will be visualized on a GUI)
// 	 *  \param label is the name to show on the GUI corresponding to current neuron
// 	 *  \param minValue is the min value for the current neuron (for the GUI visualizing the activation value)
// 	 *  \param maxValue is the max value for the current neuron (for the GUI visualizing the activation value)
// 	 *  \param color is the color on which the above data will be displayed on the GUIs
// 	 */
// 	void setGraphicProperties( QString label, double minValue, double maxValue, QColor color );
// private:
// 	/*! Checks the user is not attempting to do something nasty (e.g. access values outside range)
// 	 *  This is called by setInput(), getOutput() and setGraphicProperties() to check everything is ok
// 	 *  If something goes wrong an exception is thrown
// 	 *  \param funcName the name of the calling function (just to write a more informational message)
// 	 */
// 	void checkCurrentStatus( const QString& funcName = QString() ) const;
// 	/*! the NeuralNet */
// 	salsa::NeuralNet* neuralnet;
// 	/*! the current Cluster selected */
// 	salsa::Cluster* cluster;
// 	/*! the current index while iterating */
// 	int currIndex;
// };
//
// } // end namespace salsa
//
// #endif
