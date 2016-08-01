/*******************************************************************************
 * SALSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#ifndef EXPERIMENTINPUT_H
#define EXPERIMENTINPUT_H

#include "newgaconfig.h"
#include <QList>

namespace salsa {

/**
 * \brief The interface for input data to an experiment
 *
 * An experiment should in general inherit both from this and ExperimentOutput
 * (to provide functions to set the input and to get the output of the
 * experiment)
 */
class SALSA_NEWGA_TEMPLATE ExperimentInput
{
public:
	/**
	 * \brief Destructor
	 *
	 * This is here just to declare it virtual
	 */
	virtual ~ExperimentInput()
	{
	}
};

/**
 * \brief The base class for experiments that take one neural network in input
 */
template <class NeuralNetwork_t>
class SALSA_NEWGA_TEMPLATE OneNeuralNetworkExperimentInput : public ExperimentInput
{
public:
	/**
	 * \brief The type of the neural network used here
	 */
	typedef NeuralNetwork_t NeuralNetwork;

public:
	/**
	 * \brief Destructor
	 */
	virtual ~OneNeuralNetworkExperimentInput()
	{
	}

	/**
	 * \brief The function to set the neural network to use
	 *
	 * \param net the neural network to use
	 */
	virtual void setNeuralNetwork(const NeuralNetwork* net) = 0;
	
	virtual int getRequestedSensors() const = 0;
	
	virtual int getRequestedMotors() const = 0;
};

/**
 * \brief The base class for experiments that take multiple neural networks in
 *        input
 */
template <class NeuralNetwork_t>
class SALSA_NEWGA_TEMPLATE MultipleNeuralNetworksExperimentInput : public ExperimentInput
{
public:
	/**
	 * \brief The type of neural networks used here
	 */
	typedef NeuralNetwork_t NeuralNetwork;

public:
	/**
	 * \brief Destructor
	 */
	virtual ~MultipleNeuralNetworksExperimentInput()
	{
	}

	/**
	 * \brief The function to set the neural networks to use
	 *
	 * \param nets the neural networks to use
	 */
	virtual void setNeuralNetwork(const QList<NeuralNetwork>& nets) = 0;
};

} // end namespace salsa

#endif
