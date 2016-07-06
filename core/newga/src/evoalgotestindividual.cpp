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

#include "evoalgotestindividual.h"
#include "configurationhelper.h"
#include "parametersettableui.h"
#include <memory>
#include <QMutexLocker>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>

namespace farsa {

namespace {
	// The UI manager
	class EvoAlgoTestIndividualUIManager : public ParameterSettableUI
	{
	public:
		EvoAlgoTestIndividualUIManager(DataUploaderDownloader<EvoAlgoTestIndividualDataToGUI, EvoAlgoTestIndividualDataFromGUI>* expDataExchange, QString testName)
			: ParameterSettableUI()
			, m_expDataExchange(expDataExchange)
			, m_testName(testName)
		{
		}

		virtual QList<ParameterSettableUIViewer> getViewers(QWidget* parent, Qt::WindowFlags flags)
		{
			QList<ParameterSettableUIViewer> viewers;

			EvoAlgoTestIndividualGUI* v = new EvoAlgoTestIndividualGUI(m_testName, parent, flags);
			GlobalUploaderDownloader::associate(v, m_expDataExchange);
			viewers.append(ParameterSettableUIViewer(v, "Individual for " + m_testName));

			return viewers;
		}

	private:
		DataUploaderDownloader<EvoAlgoTestIndividualDataToGUI, EvoAlgoTestIndividualDataFromGUI>* const m_expDataExchange;
		const QString m_testName;
	};
}

EvoAlgoTestIndividual::EvoAlgoTestIndividual()
	: EvoAlgoTest()
	, DataUploaderDownloader<EvoAlgoTestIndividualDataToGUI, EvoAlgoTestIndividualDataFromGUI>(1, OverrideOlder, static_cast<NewDatumNotifiable<EvoAlgoTestIndividualDataFromGUI>*>(this))
	, m_mutex()
	, m_filename()
	, m_individual(0)
	, m_genotypes()
	, m_genotypesChanged(false)
{
}

EvoAlgoTestIndividual::~EvoAlgoTestIndividual()
{
	// Freeing memory of genotypes
	freeGenotypes();
}

void EvoAlgoTestIndividual::configure(ConfigurationParameters& params, QString prefix)
{
	EvoAlgoTest::configure(params, prefix);
	m_filename = ConfigurationHelper::getString(params, prefix + "filename", m_filename);
	m_individual = ConfigurationHelper::getInt(params, prefix + "individual", m_individual);
	if (m_individual < 0)
	{
		ConfigurationHelper::throwUserConfigError(prefix + "individual", params.getValue(prefix + "individual"), "The individual must be a positive integer");
	}
}

void EvoAlgoTestIndividual::describe( QString type )
{
	EvoAlgoTest::describe(type);
	Descriptor d = addTypeDescription( type, "Individual test" );
	d.describeString("filename").help("The filename from which the individual must be retrieved", "When not running in batch, this is ignored if a filename is specified using the test GUI");
	d.describeInt("individual").limits(0,INT_MAX).def(0).help("The individual to be tested", "When not running in batch, this is ignored if an individual is specified using the test GUI");
}

ParameterSettableUI* EvoAlgoTestIndividual::getUIManager()
{
	return new EvoAlgoTestIndividualUIManager(this, name());
}

void EvoAlgoTestIndividual::runTest()
{
	Logger::info("Start test");

	// Locking mutex because we have to access the list of genotypes
	QMutexLocker locker(&m_mutex);

	// Resetting this variable to understand if the list of genotypes has changed during the test (see
	// the comment below)
	m_genotypesChanged = false;

	// Checking if genotypes have been loaded. If they have not, loading them from m_filename
	if (m_genotypes.isEmpty()) {
		loadGenotypes();
	}
	// To release the lock on the mutex (and allow loading new files while a test is running), we
	// extract the genotype to test from the list. We will re-insert it at the end of this function
	// unless the list of genotypes has changed (in this case we simply delete it). Moreover we need
	// to store the index of the genotype we are testing (in case it changes during the test)
	const int testedIndividual = m_individual;
	if ((testedIndividual < 0) || (testedIndividual >= m_genotypes.size())) {
		Logger::error(QString("The specified genotype index (%1) is not valid").arg(testedIndividual));
		return;
	}
	std::auto_ptr<Genotype> g(m_genotypes[testedIndividual]);
	// This is necessary in case the list of genotypes gets freed to avoid deleting also the genotype
	// we are testing
	m_genotypes[testedIndividual] = NULL;

	// We can now release the lock and perform the test
	locker.unlock();

	getGenotypeTester()->setGenotype(g.get());
	getGAEvaluator()->evaluate();

	// Acquiring the lock again to put the genotype back in the vector
	locker.relock();

	// If the list of genotypes has changed, we do nothing, the destructor of std::auto_ptr will delete
	// the tested genotype, otherwise we put the genotype back in the list
	if (!m_genotypesChanged) {
		m_genotypes[testedIndividual] = g.release();
	}

	Logger::info("End test");
}

void EvoAlgoTestIndividual::newDatumAvailable(DataDownloader<DownloadedData>* /*downloader*/)
{
	QMutexLocker locker(&m_mutex);

	const EvoAlgoTestIndividualDataFromGUI* dd = downloadDatum();

	if (dd->filenameChanged) {
		// We have to load genotypes from the new file
		m_filename = dd->filename;
		if (loadGenotypes()) {
			// Sending the number of gentypes in the file to the GUI
			DatumToUpload du(*this);
			du->numIndividuals = m_genotypes.size();

			m_genotypesChanged = true;

			farsa::Logger::info(QString("Loaded %1 genotypes from file: %2").arg(m_genotypes.size()).arg(m_filename));
		} else {
			farsa::Logger::info(QString("Failed loading genotypes from file: %1").arg(m_filename));
		}
	}

	// Saving the index of the genotype to test
	m_individual = dd->individual;
}

bool EvoAlgoTestIndividual::loadGenotypes()
{
	// Removing all old genotypes
	freeGenotypes();

	// Loading all genotypes
	QFile inData(m_filename);
	if (inData.open(QFile::ReadOnly)) {
		QTextStream in(&inData);
		while (!in.atEnd()) {
			std::auto_ptr<Genotype> g(getPrototype()->clone());
			bool loaded = g->loadGen(in);
			if (!loaded) {
				// This could be not a real error, only notifying the user
				Logger::warning(QString("Error loading the genotype at index %1 from file %2").arg(m_genotypes.size()).arg(m_filename));

				break;
			}
			m_genotypes.append(g.release());
		}
		inData.close();
	} else {
		Logger::error(QString("Cannot open file %1 to read genotypes").arg(m_filename));

		return false;
	}

	return true;
}

void EvoAlgoTestIndividual::freeGenotypes()
{
	for (int i = 0; i < m_genotypes.size(); ++i) {
		delete m_genotypes[i];
	}
	m_genotypes.clear();
}

EvoAlgoTestIndividualGUI::EvoAlgoTestIndividualGUI(QString testName, QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, DataUploaderDownloader<EvoAlgoTestIndividualDataFromGUI, EvoAlgoTestIndividualDataToGUI>(1, OverrideOlder, this)
	, m_filenameLabel(NULL)
	, m_individualSpinBox(NULL)
	, m_filename()
{
	setWindowTitle("Individual for " + testName);

	QGridLayout* layout = new QGridLayout(this);

	m_filenameLabel = new QLabel("File with genotypes:", this);
	layout->addWidget(m_filenameLabel, 0, 0, 1, 2);

	QPushButton* loadFile = new QPushButton("Select file to load", this);
	layout->addWidget(loadFile, 1, 0, 1, 2);

	QLabel* individualLabel = new QLabel("Individual to test: ", this);
	layout->addWidget(individualLabel, 2, 0);

	m_individualSpinBox = new QSpinBox(this);
	m_individualSpinBox->setRange(0, 0);
	QSizePolicy p = m_individualSpinBox->sizePolicy();
	p.setHorizontalStretch(1);
	m_individualSpinBox->setSizePolicy(p);
	layout->addWidget(m_individualSpinBox, 2, 1);

	// Connecting signals and slots
	connect(loadFile, SIGNAL(clicked()), this, SLOT(selectFilename()));
	connect(m_individualSpinBox, SIGNAL(valueChanged(int)), this, SLOT(uploadIndividual()));
}

bool EvoAlgoTestIndividualGUI::event(QEvent* e)
{
	if (e->type() == farsa::NewDatumEvent<EvoAlgoTestIndividualDataToGUI>::newDatumEventType) {
		// We need a cylce because there could be more than one datum, when the uploader is connected
		// (see dataexchange.h documentation for more information)
		const EvoAlgoTestIndividualDataToGUI* d;
		while ((d = downloadDatum()) != NULL) {
			m_individualSpinBox->setRange(0, d->numIndividuals - 1);
		}

		updateLabel();

		return true;
	} else {
		return QWidget::event(e);
	}
}

void EvoAlgoTestIndividualGUI::selectFilename()
{
	QString filename = QFileDialog::getOpenFileName(this, "Select file with genotypes");

	if (!filename.isEmpty()) {
		m_filename = filename;

		updateLabel();

		uploadFilename();
	}
}

void EvoAlgoTestIndividualGUI::uploadFilename()
{
	DatumToUpload d(*this);

	d->filenameChanged = true;
	d->filename = m_filename;
	d->individual = m_individualSpinBox->value();
}

void EvoAlgoTestIndividualGUI::uploadIndividual()
{
	DatumToUpload d(*this);

	d->filenameChanged = false;
	d->individual = m_individualSpinBox->value();
}

void EvoAlgoTestIndividualGUI::updateLabel()
{
	m_filenameLabel->setText(QString("File with genotypes: %1 (%2 genotypes loaded)").arg(m_filename).arg(m_individualSpinBox->maximum() + 1));
}

} //end namespace farsa
