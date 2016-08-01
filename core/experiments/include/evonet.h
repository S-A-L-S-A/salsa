/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#ifndef EVONET_H
#define EVONET_H

#include "experimentsconfig.h"
#include "component.h"
#include "simpletimer.h"
#include "dataexchange.h"
#include "controller.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QDebug>
#include <QObject>
#include <QMutex>
#include <memory>

// All this stuff is to get rid of annoying warnings...
#ifdef __GNUC__
    #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-local-typedefs"
    #else
        #pragma GCC diagnostic ignored "-Wunused-local-typedefs"
    #endif
#endif

#include "Eigen/Dense"

#ifdef __GNUC__
    #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5)
        #pragma GCC diagnostic pop
    #else
        #pragma GCC diagnostic warning "-Wunused-local-typedefs"
    #endif
#endif

namespace salsa {

class EvonetUI;
class EvonetIterator;

/**
 * \brief The class with data exchanged with the GUI
 *
 * Here we only have data exchanged with the neuron monitor (i.e. neurons
 * activations)
 */
struct ActivationsToGui
{
	/**
	 * \brief Whether data in the vector is neurons activations or net
	 *        intput and error
	 *
	 * If true data in the vector is made up of neurons activations,
	 * otherwise it is made up of net inputs and the last element is the
	 * error
	 */
	bool activations;

	/**
	 * \brief The vector with data
	 */
	QVector<float> data;

	/**
	 * \brief The current step
	 */
	int updatesCounter;

	/**
	 * \brief Neuron labels
	 *
	 * This is only valid if updateLabelAndColors is true
	 */
	QVector<QString> neuronl;

	/**
	 * \brief The color for each neuron
	 *
	 * This is only valid if updateLabelAndColors is true
	 */
	QVector<QColor> neurondcolor;

	/**
	 * \brief If true the gui should update labels and colors
	 */
	bool updateLabelAndColors;
};

/**
 * \brief Evonet is the neural network taken from the EvoRobot
 *
 * Simple class to load a evorobot.net configuration file
 * \ingroup experiments_utils
 */
class SALSA_EXPERIMENTS_API Evonet : public Controller
{
// 	friend class NetworkDialog;
// 	friend class RendNetwork;

public:
	static bool configuresInConstructor()
	{
		return false;
	}

	//! \brief The maximum number of stored activation vectors
	static const int MAXSTOREDACTIVATIONS = 100;
	//! \brief Maximum number of neurons for a neural network of this type
	static const int MAXN = 1000;
	//! \brief DEFAULT_VALUE is used for do not assign values to mut and parameters
	static const float DEFAULT_VALUE;

public:
	/**
	 * \brief Constructor
	 */
	Evonet(ConfigurationManager& params);

	/**
	 * \brief Configures the object using
	 */
	void configure();

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 */
	void postConfigureInitialization();

	/**
	 * \brief Describes this component
	 *
	 * \param d the RegisteredComponentDescriptor to use to describe
	 *          parameters and subgroups. Only use inside this function, do
	 *          not store any reference because the object si destroyed by
	 *          the caller
	 */
	static void describe(RegisteredComponentDescriptor& d);

	/**
	 * \brief Returns a new instance of the EvonetUI
	 */
	ComponentUI* getUIManager();

	/**
	 * \brief Returns the uploader for activations
	 *
	 * \return the uploader for activations
	 */
	DataUploader<ActivationsToGui>* getNeuronsMonitorUploader()
	{
		return &neuronsMonitorUploader;
	}

	/**
	 * \brief Sets the name of this neural network
	 *
	 * This is just stored and not used anywhere in this class.
	 * \param name the name of the network
	 */
	void setNetworkName(const QString& name);

	/**
	 * \brief Returns the name of this neural network
	 *
	 * \return the name of this neural network
	 */
	const QString& getNetworkName() const;

	/**
	 * \brief Updates the controller
	 *
	 * This read the inputs and produces the outputs
	 */
	virtual void update();

	/**
	 * \brief Resets the internal status of the controller
	 */
	virtual void resetControllerStatus();

	/*!
	 * Create the block structure that describe the architecture of the network
	 * (unless this structure is created through the graphic interface and loaded froa a .net or .phe file)
	 * Update blocks describe group of neurons to be updated
	 * Connections blocks describe the group of neurons receiving the connections and the group sending the connections
	 * Gain blocks describe a group of neurons that share the same gain of that have a gain regulated by a third neuron
	 * This functions also set whether neurons have biases, timeconstant, and gains
	 * the neuron update function (e.g. logistic, leaky, binary)
	 * and the x,y positions in which the neurons are visualized in the graphic widgets
	 */
	void create_net_block(int inputNeuronType, int hiddenNeuronType, int outputNeuronType, bool recurrentHiddens, bool inputOutputConnections, bool recurrentOutputs, bool biasOnHidden, bool biasOnOutput);

	/*!
	 * Load the description of the neural architecture from .net of .phe file (see the create_net_block method description)
	 *
	 * \param filename the filename in which the description is contained
	 * \param mode 0=load only the architecture (.net file) 1=load also the value of the parameters (.phe file)
	 */
	int load_net_blocks(const char *filename, int mode);

	/*!
	 * Save the description of the neural architecture into a .net of .phe file (see the create_net_block method description)
	 *
	 * \param filename the filename in which the description is contained
	 * \param mode 0=save only the architecture (.net file) 1=save also the value of the parameters (.phe file)
	 */
	void save_net_blocks(const char *filename, int mode);

	/*!
	 * Assign to a free parameter and to the free parameter mutation rate the value extracted from a string loaded from a file .phe file
	 *
	 * \param QStringList the string from which the parameter is extracted,
	 		the first element contain the value or an "*" which means that the value of the parameter is initialized randomly
	 		the second element contain a mutation rate or "*" which means that the parameter has the default mutation rate
	 * \param float* the pointer to the free parameter
	 * \param float* the pointer to the mutation rate of the free parameter
	 */
	void readNewPheLine(QStringList, float*, float*);

	/*!
	 * Assign to a free parameter and to the free parameter mutation rate the value extracted from a string loaded from a file .phe file
	 * This function is used instead of the readNewPheLine() method to parse .phe files that contain a single QstringList element
	 *
	 * \param QStringList the string from which the parameter is extracted,
	 		the first element contain the value or an "*" which means that the value of the parameter is initialized randomly
	 		since the second element is missing, the parameter is considered as fixed (i.e. the mutation rate of the parameter is set to 0.0)
	 * \param float* the pointer to the free parameter
	 * \param float* the pointer to the mutation rate of the free parameter
	 */
	void readOldPheLine(QStringList, float*, float*);

	/*!
	 * Compute the required number of free parameters on the basis of:
	 * (i) the property of the neurons (stored in the vectors neurontype[], neuronbias[], neurongain[])
	 * (ii) the connection blocks (stored in the matrix net_block[b][0])
	 */
	void computeParameters();

	/*!
	 * set the value of a sensory neuron
	 *
	 * \param inp the id number of the sensory neuron
	 * \param value the value to be set
	 */
	int setInput(int inp, float value);

	/*!
	 * return the value of a motor neuron (-1 if the specified id is out of range)
	 *
	 * \param out the id number of the motor neuron
	 */
	float getOutput(int out);

	/*!
	 * return the value of a sensory neuron
	 *
	 * \param in the id number of the motor neuron
	 */
	float getInput(int in);

	/*!
	 * return the value of a hidden neuron (-999 if the specified id is out of range)
	 *
	 * \param h the id number of the motor neuron
	 */
	float getHidden(int h);

	/*!
	 * return number of defined free parameters
	 */
	int freeParameters();

	/*!
	 * return the value of the ith parameter (normalized in the range [-wrange, wrange]
	 */
	float getFreeParameter(int i);

	/*!
	 * set the free parameters on the basis of a genotype string
	 *
	 * \param df the pointer to the vector of genotype floats
	 */
	void setParameters(const float* dt);

	/*!
	 * set the free parameters on the basis of a genotype string
	 *
	 * \param df the pointer to the vector of genotype integers set in the range [0, 255]
	 */
	void setParameters(const int* dt);

	/*!
	 * set the mutation vector of the genetic algorithm that determine how parameters are mutated
	 *
	 * \param mut the pointer to the evonet mutation vector loaded from a.phe file
	 *
	 */
	void getMutations(float* mut);

	/*!
	 * transorm floating point parameters normalized in the range [-wrange,range] into integer parameters in the range [0,255]
	 *
	 * \param pheGene the vector of integers to be filled with the current parameters
	 *
	 */
	void copyPheParameters(int* pheGene);

	/*!
	 * Print the activation state of sensory, internal, and motor neurons
	 *
	 */
	void printIO();//to print inputs and outputs

	/*!
	 * return the value of a bias
	 *
	 * \param h the id number of the neuron
	 */
	int getParamBias(int nbias);

	/*!
	 * return the value of wrange (which also determine the range in which all parameters are normalized)
	 */
	float getWrange();

	/*!
	 * return the value of brange
	 */
	float getBrange();

	/*!
	 * return the value of grange
	 */
	float getGrange();

	/*!
	 * set the actiovation state of a hidden neuron
	 *
	 * \param nh the id number of the hidden
	 * \param val the activation value to be set
	 */
	void injectHidden(int nh, float val);

	/*!
	 * logistic function
	 *
	 * \param f the input value of the function
	 */
	float logistic(float f);

	/*!
	 * display the architecture structure
	 */
	void printBlocks();

	/*!
	 * return the number of sensory neurons
	 */
	int getNoInputs();

	/*!
	 * return the number of internal neurons
	 */
	int getNoHiddens();

	/*!
	 * return the number of motor neurons
	 */
	int getNoOutputs();

	/*!
	 * return the total number of neurons
	 */
	int getNoNeurons();

	/*!
	 * return the activation of a neuron
	 *
	 * \param in the id number of the neuron
	 */
	float getNeuron(int in);

	/*!
	 * check whether a .phe file (with parameters description) has been loaded
	 */
	bool pheFileLoaded();

	/*!
	 * activate the neurons monitor update
	 */
	void activateMonitorUpdate();

	/*!
	 * deactivate the neurons monitor update
	 */
	void deactivateMonitorUpdate();

	/*!
	 * the labels of the neurons displayed by the graphic widget
	 */
	char neuronl[MAXN][10];

	/*!
	 * the vectors that specify for each neuron whether it should be displayed or not by the neuron monitor widget
	 */
	int neurondisplay[MAXN];   // whether neurons should be displayed or not

	/*!
	 * the matrix that contain the variation range of neurons
	 * used by the neuron monitor graphic widget
	 */
	double neuronrange[MAXN][2];  // the range of variation of the neuron

	/*!
	 * the color used to display the actiovation state of each neuron in the neuron monitor widget
	 */
	QColor neurondcolor[MAXN];

	/*!
	 * a vector that speficy lesioned and unlesioned neurons
	 */
	bool neuronlesion[MAXN];    // if >0 the n neurons will be silented

	/*!
	 * the value to be assigned to the state of lesioned neurons
	 */
	float neuronlesionVal[MAXN]; //value to be assigned to the lesioned neuron

	/**
	 * \brief Set to true if labels or colors have to be updated in the neuron monitor
	 */
	bool updateNeuronMonitor;

	/*!
	 * set the range of connection weights, biases, and gains
	 *
	 * \param weight the range of weights
	 * \param bias the range of biases
	 * \param gain the range of gains
	 */
	void setRanges(double weight, double bias, double gain);

	/**
	 * \brief Returns the oldest stored activation vector and removes it
	 *
	 * This returns the oldest activation vector and deletes it (memory is
	 * not freed, simply an internal index is incremented), so that
	 * subsequent calls will progressively return newer activation vectors.
	 * Returns NULL when no activation vector is stored
	 * \return the oldest stored activation or NULL if no stored activation
	 *         vector is present
	 */
	float* getOldestStoredActivations();

	/*! \brief return the number of updates (step) done
	 *
	 *  The counter is resetted to zero when the Evonet is resetted (Evonet::resetNet)
	 *  and is incremented by one at each call of Evonet::updateNet
	 */
	int updateCounts();
	/*! \brief pointer to the list pointer-to-parameters selected through the graphic interface
	 *  Enable the user to modify the parameters manually through the graphic widget
	 */
	float **selectedp;
	/*! \brief Number of parameters selected through the graphic interface
	 *  Enable the user to modify the parameters manually through the graphic widget
	 */
	int   nselected;
	/*! \brief Whether one or more neurons have been lesioned
	 */
	int neuronlesions;

protected:
	/**
	 * \brief Creates and returns an input iterator and an output iterator
	 *
	 * See class description for more information
	 * \return the input and output iterators. These objects, after
	 *         creation, are managed by Controller, do not delete them!
	 */
	virtual QPair<AbstractControllerInputIterator*, AbstractControllerOutputIterator*> createIterators();

	/**
	 * \brief Returns the block index for the given input
	 *
	 * \param input the input for which the index is requested
	 * \return the block index for the given input
	 */
	virtual int getIndexForControllerInput(AbstractControllerInput* input);

	/**
	 * \brief Returns the block index for the given output
	 *
	 * \param output the output for which the index is requested
	 * \return the block index for the given output
	 */
	virtual int getIndexForControllerOutput(AbstractControllerOutput* output);

private:
	int computeNumSensorsFromControllerInputsList() const;
	int computeNumMotorsFromControllerOutputsList() const;

	/*!
	 * Update the state of the internal and motor neurons on the basis of:
	 * (i) the property of the neurons (stored in the vectors neurontype[], neuronbias[], neurongain[])
	 * (ii) the update, connection, and gain blocks (stored in the matrix net_block[b][0])
	 * (iii) the value of the free parameters
	 */
	void updateNet();

	/*!
	 * reset to 0.0 the activation state of all neurons
	 */
	void resetNet();

	/*! \brief number of sensory neurons
	 */
	int ninputs;

	/*! \brief number of internal neurons
	 */
	int nhiddens;

	/*! \brief number of motor neurons
	 */
	int noutputs;

	/*! \brief total number of neurons
	 */
	int nneurons;

	/*! \brief number of blocks used to describe the archiecture of the network
	 */
	int net_nblocks;            // network number of connection blocks

	/*! \brief matrix containing the blocks description
	 * the first element specify the block type (0=connection block,1=update block, 2=gain block)
	 * the second and third element contain the id of the first neuron and the number of neurons of the block
	 * the fourth and fifth element contain the id of the first neuron and the number of neurons of the block that send connections (only for connection blocks)
	 * the sixth element describe whether the parameters are genetic or subjected to learning (in the case of connection blocks)
	 */
	int net_block[MAXN][6];

	/*! \brief Vector specifying whether neurons have not or have a bias
	 *
	 */
	int neuronbias[MAXN];

	/*! \brief Vector specifying the neurons type (i.e. the update function associated to the neuron)
	 * 0=logistic neurons, 1=dynamic neurons with timeconstat parameter, 2=binary neuron, 3=logistic neuron with flatter curve
	 */
	int neurontype[MAXN];

	/*! \brief Vector specifying whether neurons have not or have a gain parameter
	 *
	 */
	int neurongain[MAXN];

	/*! \brief The x and y position of the neurons used to display the architecture with the graphic widget
	 */
	int neuronxy[MAXN][2];

	/*! \brief the range of the connection weights
	 */
	float wrange;

	/*! \brief the range of the gains of the neurons
	 */
	float grange;

	/*! \brief the range of the biases of the neurons
	 */
	float brange;

	/*! \brief the number of parameters
	 * Include gains, biases, connection weights, and time constants
	 */
	int nparameters;

	int nparambias;				// number of parametric bias to add to the number of genes need for the nnet

	/*! \brief vector containing the activation state of the neurons
	 */
	float act[MAXN];

	float storedActivations[MAXSTOREDACTIVATIONS][MAXN]; // Stored activations

	int nextStoredActivation;	// The index where the next activation will be stored

	int firstStoredActivation;	// The index of the first activation that was stored (since the stored activations were reset)

	/*! \brief vector containing the activation state of the sensory neurons
	 * This vector is updated by the sensors and then used to activate the sensory neurons
	 */
	float input[MAXN];

	/*! \brief the netinput of the neurons
	 */
	float netinput[MAXN];

	/*! \brief vector containing the value of the parameters (gains, biases, connection weights, time constants)
	 */
	float *freep;

	/*! \brief Flag that activate or deactivate the neurons monitor update
	 */
	bool updateMonitor;

	/*! \brief vector containing the value of the parameters loaded from a .phe file (gains, biases, connection weights, time constants)
	 */
	float *phep;			    //dynamically resized to contain manually set values specified in .phe files

	/*! \brief Specify whether a .phe file has been loaded or not
	 */
	bool  pheloaded;            //whether a .phe file has been loaded

	/*! \brief vector containing the mutation rate of the single parameters
	 */
	float *muts;				//mutation parameters

	/*! \brief range of the gene integers (255,512, 1023 etc.)
	 */
	int geneMaxValue;

	/*! \brief counter of network update
	 * It is resetted by resetNet() and incremeneted by updateNet()
	 */
	int updatescounter;

	int p;					//usato come puntatore per data

	int ndata;				//number of free parameters

	/*! \brief maximum y coordinate for network architecture, used by the graphic widget
	 */
	int drawnymax;

	/*! \brief maximum x coordinate for network architecture, used by the graphic widget
	 */
	int drawnxmax;

	/*! \brief name of the .net or .phe file containing the architecture description
	 */
	QString netFile;

	/**
	 * \brief A name for the network. This is just stored and is unsed nowhere in the class
	 */
	QString networkName;

	/**
	 * \brief The uploader to send activations to the GUI
	 */
	DataUploader<ActivationsToGui> neuronsMonitorUploader;

	// The ui manager. We need to keep this to switch uploaders and downloaders
	// after the UI is created
	EvonetUI* m_evonetUI;

	EvonetIterator* m_evonetIterator;
	int m_inputCurIndex;
	int m_outputCurIndex;
};

} // end namespace salsa

#endif
