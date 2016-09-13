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

#include "evorobotviewer.h"
#include "renderworldwrapperwidget.h"
#include "abstracttest.h"
#include "tests.h"
#include "total99resources.h"
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QEvent>
#include <QCoreApplication>
#include <QTimer>
#include <QSlider>
#include <cmath>

using namespace qglviewer;

// All the suff below is to avoid warnings on Windows about the use of unsafe
// functions. This should be only a temporary workaround, the solution is stop
// using C string and file functions...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

EvoRobotViewer::EvoRobotViewer( EvoRobotComponent* component )
	: QObject()
	, ComponentUI()
	, evorobot(component)
	, ga(evorobot->getGA())
	, statViewer(nullptr)
	, ftv(nullptr)
	, renderworldwrapper(nullptr)
	, infoEvoga(nullptr)
	, simulationThrottle(nullptr)
	, simulationSpeed(nullptr)
	, timer(nullptr)
{
	timer = new QTimer(this);
	timer->setInterval( 40 );
	timer->setSingleShot( false );
	timer->start();

	// !! DO NOT CONNECT TO THE advanced SIGNAL to update the renderworld becuase that signals may be
	//    emitted so fast that the GUI will freeze !!
	//connect( world, SIGNAL(advanced()), this, SLOT(onWorldAdvance()), Qt::QueuedConnection );
	connect( timer, SIGNAL(timeout()), this, SLOT(onWorldAdvance()) );

	// Global connections
	connect( evorobot, SIGNAL(actionFinished()), this, SLOT(onActionFinished()) );
}

EvoRobotViewer::~EvoRobotViewer()
{
	// Nothing to do
	// --- All objects are destroyed in others parts because none of them are owend by this object
}

void EvoRobotViewer::fillActionsMenu( QMenu* actionsMenu ) {
	actionsMenu->addAction( "Evolve", evorobot, SLOT(evolve()) );
	actionsMenu->addAction( "Stop", evorobot, SLOT(stop()) );
	actionsMenu->addAction( "Test Random", evorobot, SLOT(runTestRandom()) );
	actionsMenu->addAction( "Test Current", evorobot, SLOT(runTestCurrent()) );
	actionsMenu->addAction( "Test Individual", evorobot, SLOT(runTestIndividual()) );
}

QList<ComponentUIViewer> EvoRobotViewer::getViewers( QWidget* parent, Qt::WindowFlags flags ) {
	QList<ComponentUIViewer> viewsList;
	viewsList.append( evogaControls( parent, flags ) );
// 	viewsList.append( icubview( parent, flags ) );
	viewsList.append( fitview( parent, flags ) );
	viewsList.append( statview( parent, flags ) );
	viewsList.append( renderWorld( parent, flags ) );
	viewsList.append( testIndividualUI( parent, flags ) );
	return viewsList;
}

void EvoRobotViewer::addAdditionalMenus( QMenuBar* menuBar ) {
// 	QMenu* testMenu = menuBar->addMenu( "Tests" );
// 	QStringList testsList = AvailableTestList::getList();
// 	for (int i = 0; i < testsList.size(); i++) {
// 		AbstractTest* test = AvailableTestList::getTest(testsList[i]);
// 		QAction* action = testMenu->addAction( test->menuText(), evorobot, SLOT(runTestFromQAction()) );
// 		action->setData( QVariant(testsList[i]) );
// 		action->setToolTip( test->tooltip() );
// 		action->setIcon( QIcon( test->iconFilename() ) );
// 	}
}

ComponentUIViewer EvoRobotViewer::renderWorld( QWidget* parent, Qt::WindowFlags flags )
{
	// Creating the wrapper class for RenderWorld and sharing resources
	renderworldwrapper = new RenderWorldWrapperWidget(ga->getEvoRobotExperiment()->getRenderersContainerDataUploaderDownloader(), parent, flags);

	return ComponentUIViewer(renderworldwrapper, "RenderWorld");
}

ComponentUIViewer EvoRobotViewer::evogaControls( QWidget* parent, Qt::WindowFlags flags ) {
	QWidget* widget = new QWidget( parent, flags );
	QGridLayout* lay = new QGridLayout( widget );
	//lay->setContentsMargins(0,0,0,0);

	// --- visualizzaione del trial corrente, e step corrente
	infoEvoga = new QLabel( "Information", widget );
	infoEvoga->setStyleSheet( "QLabel { font: bold normal large \"Courier\" }" );
	lay->addWidget( infoEvoga, 0, 0, 1, 2 );

	QPushButton* bt = new QPushButton( "Do Step", widget );
	bt->setAutoRepeat(true);
	bt->setEnabled( false );
	connect( bt, SIGNAL(clicked(bool)), ga, SLOT(doNextStep()) );

	QCheckBox* cb = new QCheckBox( "Step by Step Mode", widget );
	connect( cb, SIGNAL(clicked(bool)), ga, SLOT(enableStepByStep(bool)) );
	connect( cb, SIGNAL(clicked(bool)), bt, SLOT(setEnabled(bool)) );

	lay->addWidget( cb, 1, 0 );
	lay->addWidget( bt, 1, 1 );

	bt = new QPushButton( "Next Trial", widget );
	connect( bt, SIGNAL(clicked()), this, SLOT(evogaNextTrial()) );
	lay->addWidget( bt, 2, 0, 1, 2 );

	//----- Skip Trial... Go to Trial number N

	// slider to setup the delay for slowing down the simulation
	QLabel* lb = new QLabel( "Simulation Throttle - speed regulator", widget );
	lay->addWidget( lb, 3, 0, 1, 2 );
	simulationThrottle = new QSlider( widget );
	simulationThrottle->setMinimum( 0 );
	simulationThrottle->setMaximum( 100 );
	simulationThrottle->setMinimumHeight( 200 );
	simulationThrottle->setOrientation( Qt::Vertical );
	int currDelay = ga->getEvoRobotExperiment()->getStepDelay();
	simulationThrottle->setValue( floor(13.0*std::log((float)currDelay)) );
	connect( simulationThrottle, SIGNAL(valueChanged(int)), this, SLOT(onSimulationThrottleChanges(int)) );
	lay->addWidget( simulationThrottle, 4, 0, 3, 1 );
	lb = new QLabel( "slow", widget );
	lay->addWidget( lb, 4, 1 );

	QString str = "Running as fast as possible";
	if ( currDelay > 0 ) {
		QString extra;
		if ( fabs( currDelay/1000.0 - ga->getEvoRobotExperiment()->getWorldTimeStep() ) < 0.01 ) {
			extra = "Running approximately at real time";
		}
		str = QString("Speed: %1 frames/second [%2 ms]\n%3")
					.arg(1000.0/currDelay)
					.arg(currDelay)
					.arg(extra);
	}
	simulationSpeed = new QLabel( str, widget );
	lay->addWidget( simulationSpeed, 5, 1 );
	lb = new QLabel( "fast", widget );
	lay->addWidget( lb, 6, 1 );

	return ComponentUIViewer( widget, "Evoga Controls" );
}

void EvoRobotViewer::evogaNextTrial() {
	ga->getEvoRobotExperiment()->stopTrial();
}

void EvoRobotViewer::onSimulationThrottleChanges( int newvalue ) {
	// mapping with an exponential in order to get more sensitivity for small values
	int delay = ceil(std::exp( newvalue/13.0 )-1.0);
	ga->getEvoRobotExperiment()->setStepDelay( delay );
	if ( delay > 0 ) {
		QString extra;
		if ( fabs( delay/1000.0 - ga->getEvoRobotExperiment()->getWorldTimeStep() ) < 0.01 ) {
			extra = "Running approximately at real time";
		}
		simulationSpeed->setText( QString("Speed: %1 frames/second [%2 ms]\n%3")
					.arg(1000.0/delay)
					.arg(delay)
					.arg(extra) );
	} else {
		simulationSpeed->setText( "Running as fast as possible" );
	}
}

ComponentUIViewer EvoRobotViewer::fitview( QWidget* parent, Qt::WindowFlags flags )
{
	// Here we also create the fitness viewer
	ftv = new FitViewer(3,4000,parent,flags);
	ftv->setChunkProperties(0, "MaxFit", Qt::red, true);
	ftv->setChunkProperties(1, "AverageFit", Qt::green, true);
	ftv->setChunkProperties(2, "MinFit", Qt::blue, true);
	ftv->setLabels(QString("EvoICub Fitness Monitor"), QString("Generations"), QString("Fitness"));
	ftv->setGeometry(50, 50, 500, 500);
	ftv->setWindowTitle( "Fitness Curves" );
	connect( ga, SIGNAL(startingReplication(int)),
			 this, SLOT(onEvogaStartingReplication(int)), Qt::QueuedConnection );
	connect( ga, SIGNAL(recoveredInterruptedEvolution(QString)),
			 this, SLOT(onEvogaRecoveredInterruptedEvolution(QString)), Qt::QueuedConnection );
	connect( ga, SIGNAL(endGeneration(int,double,double,double)),
			 this, SLOT(onEvogaEndGeneration(int,double,double,double)), Qt::QueuedConnection );
	return ComponentUIViewer( ftv, "Fitness monitor" );
}

// ComponentUIViewer EvoRobotViewer::icubview( QWidget* parent, Qt::WindowFlags flags )
// {
// 	m_icubview = new VisionMapSensorGui(parent,flags);
// 	m_icubview->setWindowTitle( "iCub Camera" );
// 	m_icubview->resize( 200, 200 );
// 	m_icubview->hide();
// 	return ComponentUIViewer( m_icubview, "iCub camera" );
// }

ComponentUIViewer EvoRobotViewer::statview( QWidget* parent, Qt::WindowFlags flags )
{
	statViewer = new QWidget( parent, flags );
	statViewer->setWindowTitle( "Statistics Viewer" );
	QGridLayout* lay = new QGridLayout( statViewer );
	QPushButton* but = new QPushButton( "Load a Stat File", statViewer );
	connect( but, SIGNAL(clicked()), this, SLOT(loadStat()) );
	lay->addWidget( but, 0, 0 );
	but = new QPushButton( "Load All Stat", statViewer );
	connect( but, SIGNAL(clicked()), this, SLOT(loadAllStat()) );
	lay->addWidget( but, 0, 1 );
	return ComponentUIViewer( statViewer, "Statistic Viewer" );
}

void EvoRobotViewer::loadStat() {
	QString filename = QFileDialog::getOpenFileName(statViewer, tr("Open Stat File"), ".", tr("Files with statistics (*.fit *.ini)"));
	if (filename.isEmpty()) {
		return;
	}
	FitViewer* fitViewer = statViewer->findChild<FitViewer*>( "statFitViewer" );
	if ( fitViewer ) {
		delete fitViewer;
	}
	fitViewer = new FitViewer(3, 4000, statViewer);
	fitViewer->setObjectName( "statFitViewer" );
	fitViewer->setLabels(QString("Stat monitor. File: ").append(filename), QString("Generation"), QString("Fitnes"));
	fitViewer->setChunkProperties(0, "MaxFit", Qt::red,true);
	fitViewer->setChunkProperties(1, "Average", Qt::green,true);
	fitViewer->setChunkProperties(2, "Minimum", Qt::blue,true);
	fitViewer->loadRawData(0, filename, 0);
	fitViewer->loadRawData(1, filename, 1);
	fitViewer->loadRawData(2, filename, 2);
	QGridLayout* lay = qobject_cast<QGridLayout*>( statViewer->layout() );
	lay->addWidget( fitViewer, 1, 0, 1, 2 );
	lay->setRowStretch( 1, 2 );
	fitViewer->show();
}

void EvoRobotViewer::loadAllStat()
{
	FitViewer* fitViewer = statViewer->findChild<FitViewer*>( "statFitViewer" );
	if ( fitViewer ) {
		delete fitViewer;
	}

	QDir currentDir;
	QFileInfoList statFiles = currentDir.entryInfoList( QStringList() << "statS*.fit", QDir::Files, QDir::Name );

	fitViewer = new FitViewer(statFiles.size(), ga->getNumOfGenerations(), statViewer);
	fitViewer->setObjectName( "statFitViewer" );
	QString title = QString("Stat monitor");
	fitViewer->setLabels(title, QString("Generation"), QString("Fitness"));

	QColor colors[10] = { QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue), QColor(Qt::cyan),
						QColor(Qt::magenta), QColor(Qt::darkYellow), QColor(Qt::gray), QColor(255, 140, 0, 255),
						QColor(153, 50, 204, 255), QColor(Qt::black) };
	for(int i=0; i<statFiles.size(); i++ ) {
		QFileInfo statFile = statFiles[i];
		QColor col = colors[i%10];
		fitViewer->setChunkProperties( i, QString("Seed:").append(statFile.baseName().split("S").last()), col, true );
		fitViewer->loadRawData( i, statFile.fileName(), 0 );
	}
	fitViewer->sortchunks();
	QGridLayout* lay = qobject_cast<QGridLayout*>( statViewer->layout() );
	lay->addWidget( fitViewer, 1, 0, 1, 2 );
	lay->setRowStretch( 1, 2 );
	fitViewer->show();
}

ComponentUIViewer EvoRobotViewer::testIndividualUI( QWidget* parent, Qt::WindowFlags flags ) {
	TestIndividualGUI* testIndUI = new TestIndividualGUI( evorobot->getTestIndividual(), parent, flags );
	testIndUI->setWindowTitle( "Select the Individual to Test" );
	return ComponentUIViewer( testIndUI, "Individual to Test", QString(), "From this view you can select an individual to test using the \"TestIndividual\" from the \"Tests\" menu" );
}

void EvoRobotViewer::onWorldAdvance() {
	if (infoEvoga) {
		// Old stuffs, to remove
// 		ResourcesLocker locker(this);
//
// 		EvoRobotExperiment* exp = getResource<EvoRobotExperiment>("experiment");
		EvoRobotExperiment* exp = ga->getEvoRobotExperiment();

		// visualizza su EvoGa Controls informazioni su step e trial
#warning QUESTO VA CAMBIATO PER USARE UPLOADER/DOWNLOADER, AL MOMENTO NON È THREAD-SAFE
		infoEvoga->setText( QString( "Step %1 of %2 --- Trial %3 of %4" )
								.arg( exp->getCurStep(), 5 ).arg( exp->getNSteps() )
								.arg( exp->getCurTrial() + 1, 5 ).arg( exp->getNTrials() ) );
	}

// 	if ( exp->getActivityPhase() == EvoRobotExperiment::INTEST ) {
// // 		if (m_icubview != nullptr)  {
// // 			m_icubview->setImage( exp->getResource<PhyiCub>( "robot" ) );   // update the iCub camera widget
// // 		}
// 	}
}

void EvoRobotViewer::onEvogaStartingReplication( int /*replication*/ ) {
	ftv->reset();
}

void EvoRobotViewer::onEvogaRecoveredInterruptedEvolution( QString statfile ) {
	ftv->loadRawData(0,statfile,0);
	ftv->loadRawData(1,statfile,1);
	ftv->loadRawData(2,statfile,2);
}

void EvoRobotViewer::onEvogaEndGeneration( int generation, double fmax, double faverage, double fmin ) {
	ftv->setChunkValue(0,generation,fmax);
	ftv->setChunkValue(1,generation,faverage);
	ftv->setChunkValue(2,generation,fmin);
	ftv->diplayUntilStep(generation);
	ftv->update();
}

void EvoRobotViewer::onActionFinished() {
	infoEvoga->setText( "Finished" );
}

// #include <QPainter>
// #include <QPen>
// #include <QBrush>
// #include <QTransform>
// #include <QVBoxLayout>
//
// namespace VisionMapSensorGuiInternal {
// 	/**
// 	 * \brief An helper class to display an image
// 	 *
// 	 * This takes a pointer to the image to show and also resizes itself to
// 	 * match the image dimension
// 	 */
// 	class ImageDisplayer : public QWidget
// 	{
// 	public:
// 		ImageDisplayer(QWidget* parent = nullptr) :
// 			QWidget(parent),
// 			m_image(nullptr)
// 		{
// 		}
//
// 		~ImageDisplayer()
// 		{
// 		}
//
// 		void setImage(QImage* image)
// 		{
// 			// Saving image
// 			m_image = image;
//
// 			// Also resetting widget dimensions
// 			setMinimumSize(m_image->width(), m_image->height());
// 			setMaximumSize(m_image->width(), m_image->height());
//
// 			// Updating the widget
// 			update();
// 		}
//
// 	private:
// 		void paintEvent(QPaintEvent *)
// 		{
// 			if (m_image == nullptr) {
// 				return;
// 			}
//
// 			QPainter painter(this);
//
// 			// Drawing image. Also flipping image (perhaps yarp doesn't consider top left to be 0, 0)
// 			painter.drawImage(0, 0, m_image->mirrored(false, true));
// 		}
//
// 		QImage* m_image;
// 	};
// }
//
// VisionMapSensorGui::VisionMapSensorGui(QWidget* parent, Qt::WindowFlags flags) :
// 	QWidget(parent,flags),
// 	m_image(),
// 	m_imageDisplayer(new VisionMapSensorGuiInternal::ImageDisplayer(this))
// {
// 	// Creating a layout and adding the image displayer widget and the table with map activations
// 	QVBoxLayout* layout = new QVBoxLayout(this);
// 	layout->addWidget(m_imageDisplayer);
//
//
// }
//
// VisionMapSensorGui::~VisionMapSensorGui()
// {
// 	// Nothing to do here
// }
//
// void VisionMapSensorGui::setImage(PhyiCub *icub, bool useRightCamera)
// {
//
// 	// Taking the image in yarp format
// 	yarp::sig::ImageOf<yarp::sig::PixelRgb> yarpImage;
// 	// yarp::dev::IFrameGrabberImage* frameGrabber = useRightCamera ? icub->rightEyeFrameGrabber() :  icub->leftEyeFrameGrabber();
// 	yarp::dev::IFrameGrabberImage* frameGrabber = nullptr; // The line above crashes with multithread
// 	if (frameGrabber == nullptr) {
// 		return;
// 	}
// 	// frameGrabber->getImage(yarpImage); // Crashes with multithread
//
// 	// Converting to QImage
// 	m_image = QImage(yarpImage.width(), yarpImage.height(), QImage::Format_RGB32);
// 	for (int i = 0; i < yarpImage.width(); i++) {
// 		for (int j = 0; j < yarpImage.height(); j++) {
// 			// Converting pixel and writing it into the QImage
// 			yarp::sig::PixelRgb& yarpPixel = yarpImage.pixel(i, j);
// 			m_image.setPixel(i, j, qRgb(yarpPixel.r, yarpPixel.g, yarpPixel.b));
// 		}
// 	}
//
//
// 	// Drawing an X at the image center
// 	QPainter painter(&m_image);
// 	const int cX = m_image.width() / 2;
// 	const int cY = m_image.height() / 2;
//
// 	// Drawing the X
// 	painter.setPen(QPen(Qt::white, 3));
// 	painter.drawLine(cX - 5, cY - 5, cX + 5, cY + 5);
// 	painter.drawLine(cX + 5, cY - 5, cX - 5, cY + 5);
// 	painter.setPen(QPen(Qt::black, 1));
// 	painter.drawLine(cX - 5, cY - 5, cX + 5, cY + 5);
// 	painter.drawLine(cX + 5, cY - 5, cX - 5, cY + 5);
//
// 	// Setting the image for the image displayer
// 	m_imageDisplayer->setImage(&m_image);
// 	m_imageDisplayer->update();
// }
//
// void VisionMapSensorGui::addMark(double x, double y, QColor markCenter, QColor markSurrounding)
// {
// 	// Drawing a mark directly on the image
// 	QPainter painter(&m_image);
//
// 	// Computing the mark position in image coordinates
// 	const int imageX = int(x * double(m_image.width()));
// 	const int imageY = int(y * double(m_image.height()));
//
// 	// Drawing the mark
// 	painter.setPen(QPen(markSurrounding, 5));
// 	painter.drawPoint(imageX, imageY);
// 	painter.setPen(QPen(markCenter, 3));
// 	painter.drawPoint(imageX, imageY);
//
// 	// Updating the image displayer widget
// 	m_imageDisplayer->update();
// }

TestIndividualGUI::TestIndividualGUI(TestIndividual* tb, QWidget *parent, Qt::WindowFlags flags) :
	QWidget(parent, flags)
{
	test = tb;

	//set layout
	QGridLayout* mainLay = new QGridLayout( this );

	// buttons
	QPushButton* bt = new QPushButton( "Refresh", this );
	connect( bt, SIGNAL(clicked()), this, SLOT(populateCombo()) );
	mainLay->addWidget(bt, 0, 0, 1, 2);
	//creating comboBox and list
	combo = new QComboBox( this );
	list = new QListWidget( this );
	mainLay->addWidget(new QLabel("Select File to load:"), 1, 0);
	mainLay->addWidget(combo, 1, 1);
	mainLay->addWidget(list, 2, 0, 1, 2);

	//checking available best and gen files
	populateCombo();

	// Connecting signals and slots
	connect(combo, SIGNAL(activated(QString)), this, SLOT(seedWasChosen()));
	connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(agentClicked()));
	connect(list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(agentClicked()));
}

void TestIndividualGUI::populateCombo()
{
	combo->clear();
	list->clear();
	//ask abstractGA the template name for the files (i.e. "*P*S*G*.gen")
	QString bestF = test->component()->getGA()->bestsFilename();
	QString genF = test->component()->getGA()->generationFilename();

	//search current folder for files
	QDir* dir = new QDir();
	QStringList expression = (QStringList() << bestF << genF);
	fileList = dir->entryList(expression);

	//insert their name into the combo boxes
	combo->addItems(fileList);
}

void TestIndividualGUI::seedWasChosen()
{
	//load the chosen file
	test->setPopulationToTest( combo->currentText(), false );
	//refresh individuals list on the GUI
	list->clear();
	int loadindi = test->component()->getGA()->numLoadedGenotypes();
	for(int i=1; i<=loadindi; i++)
	{
		list->addItem(QString::number(i));
	}
}

void TestIndividualGUI::agentClicked()
{
	//take index of the chosen individual
	test->setIndividualToTest( list->currentRow() );
}

} //end namespace salsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
