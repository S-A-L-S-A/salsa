/********************************************************************************
 *  FARSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
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

#ifndef EVOALGOTESTINDIVIDUAL_H
#define EVOALGOTESTINDIVIDUAL_H

#include "parametersettable.h"
#include "configurationparameters.h"
#include "evoalgotest.h"
#include <iostream>
#include <cstdlib>
#include <QObject>
#include <QString>
#include <QMap>
#include <QMutex>
#include <QSpinBox>
#include <QLabel>
#include "logger.h"

namespace farsa {

/**
 * \brief Data exchanged between the GUI and the object performing the test
 */
struct EvoAlgoTestIndividualDataFromGUI
{
	/**
	 * \brief If true only the name of the file changed
	 */
	bool filenameChanged;

	/**
	 * \brief The name of the file to load
	 */
	QString filename;

	/**
	 * \brief The index of the individual to load
	 */
	int individual;
};

/**
 * \brief Data exchanged between the object performing the test and the GUI
 */
struct EvoAlgoTestIndividualDataToGUI
{
	/**
	 * \brief The number of individuals in the file
	 */
	int numIndividuals;
};

/**
 * \brief Class to test of the selected individual
 */
class FARSA_NEWGA_API EvoAlgoTestIndividual : public EvoAlgoTest, private DataUploaderDownloader<EvoAlgoTestIndividualDataToGUI, EvoAlgoTestIndividualDataFromGUI>, private NewDatumNotifiable<EvoAlgoTestIndividualDataFromGUI>
{
public:
	/**
	 * \brief Constructor
	 */
	EvoAlgoTestIndividual();

	/**
	 * \brief Destructor
	 */
	virtual ~EvoAlgoTestIndividual();

	/**
	 * \brief Configures the object using a ConfigurationParameters object
	 *
	 * \param params the configuration parameters object with parameters to
	 *               use
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters. This is guaranteed to end with the
	 *               separator character when called by the factory, so you
	 *               don't need to add one
	 */
	virtual void configure(ConfigurationParameters& params, QString prefix);

	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters
	 *        object passed
	 *
	 * This is not implemented, a call to this function will cause an abort
	 * \param params the configuration parameters object on which save actual
	 *               parameters
	 * \param prefix the prefix to use to access the object configuration
	 *               parameters.
	 */
	virtual void save(ConfigurationParameters& /*params*/, QString /*prefix*/)
	{
		Logger::error("NOT IMPLEMENTED (EvoAlgoTestIndividual::save)");
		abort();
	}

	/**
	 * \brief Describes this class and its parameters
	 *
	 * \param type the name of this type
	 */
	static void describe(QString type);

	/**
	 * \brief Returns a new instance of UI manager
	 *
	 * \return a new instance of the UI manager
	 */
	ParameterSettableUI* getUIManager();

	/**
	 * \brief Performs the test
	 */
	void runTest();

private:
	/**
	 * \brief The function called when a new datum is available
	 *
	 * This function is called when a new datum is available.
	 * \param downloader the downloader with the available datum
	 * \note This should be thread-safe, as the notification arrives from
	 *       the thread where the uploader lives
	 */
	virtual void newDatumAvailable(DataDownloader<DownloadedData>* downloader);

	/**
	 * \brief Loads genotypes from m_filename
	 *
	 * This frees the old genotypes if it has to
	 * \return false in case of errors
	 */
	bool loadGenotypes();

	/**
	 * \brief Frees memory for all genotypes in the vector
	 */
	void freeGenotypes();

	/**
	 * \brief The mutex protecting concurrent access to data structures
	 */
	QMutex m_mutex;

	/**
	 * \brief The filename from which the individual must be retrieved
	 *
	 * This is initially the value of the configuration parameter, it is
	 * overridden if we receive a command to load a specific file from the
	 * GUI
	 */
	QString m_filename;

	/**
	 * \brief The index of the individual to be tested
	 *
	 * This is initially the value of the configuration parameter, it is
	 * overridden if we receive a command to test a specific individual from
	 * the GUI
	 */
	int m_individual;

	/**
	 * \brief The vector with genotypes
	 *
	 * Access to this vector is serialized using m_mutex
	 */
	QVector<Genotype*> m_genotypes;

	/**
	 * \brief This is set to true when we load genotypes from a new file
	 *
	 * We need this to understand if the list of genotypes changed while a
	 * test was running
	 */
	bool m_genotypesChanged;
};

/**
 * \brief The GUI to select the individual to test
 */
class EvoAlgoTestIndividualGUI : public QWidget, public DataUploaderDownloader<EvoAlgoTestIndividualDataFromGUI, EvoAlgoTestIndividualDataToGUI>
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param testName the name of the test
	 * \param parent the parent widget
	 * \param flags the window flags
	 */
	EvoAlgoTestIndividualGUI(QString testName, QWidget* parent, Qt::WindowFlags flags);

	/**
	 * \brief Receives events
	 *
	 * This only processes the
	 * NewProductEvent\<EvoAlgoTestIndividualDataToGUI\>
	 * \param e the event to processes
	 * \return true if the event has been recognized and processed
	 */
	virtual bool event(QEvent* e);

private slots:
	/**
	 * \brief Lets the user select the filename to load
	 */
	void selectFilename();

	/**
	 * \brief Sends the filename to the tester
	 */
	void uploadFilename();

	/**
	 * \brief Sends the index of the individual to the tester
	 */
	void uploadIndividual();

private:
	/**
	 * \brief Updates the text in the m_filenameLabel
	 */
	void updateLabel();

	/**
	 * \brief The label with the file to load
	 */
	QLabel* m_filenameLabel;

	/**
	 * \brief The spinbox with the individual to test
	 */
	QSpinBox* m_individualSpinBox;

	/**
	 * \brief The file with genotypes
	 */
	QString m_filename;
};

} //end namespace farsa

#endif
