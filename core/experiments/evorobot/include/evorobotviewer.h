/********************************************************************************
 *  FARSA Experiments Library                                                   *
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

#ifndef EVOROBOTVIEWER_H
#define EVOROBOTVIEWER_H

#include "evorobotcomponent.h"
#include "componentui.h"
#include "evoga.h"
#include "renderworld.h"
#include "evodataviewer.h"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QLabel>
#include <QList>
#include <QString>
#include <QWidget>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QComboBox>
#include <QObject>

#warning QUI NON HO AGGIUNTO Render2DGUI, MA TENERLO IN CONSIDERAZIONE (PUÒ ESSERE UTILE)

namespace farsa {

class PhyiCub;
// class VisionMapSensorGui;
class EvoRobotExperiment;
class TestIndividual;
class RenderWorldWrapperWidget;

class FARSA_EXPERIMENTS_API EvoRobotViewer : public QObject, public ComponentUI
{
	Q_OBJECT
public:
	/*! \brief Construct the GUI manager for EvoRobotComponent
	 *  \warning This only works with EvoRobotComponent
	 */
	EvoRobotViewer( EvoRobotComponent* component );
	/*! Destructor */
	~EvoRobotViewer();
	/*! fill the menu "Actions" of Total99 with following actions:
	 *  - Evolve: start the evolutionary process
	 *  - Stop: suspend the evolutionary process
	 *  \param actionsMenu is the actionsMenu display in the menu bar of Total99
	 */
	void fillActionsMenu( QMenu* actionsMenu );
	/*! Return the list of all viewers for the Component with corresponding
	 *  informations for fill the menu "Views" of Total99
	 *  \param parent is the parent widget for all viewers created by ParameterSettableUI
	 *  \param flags are the necessary flags to specify when constructing the viewers
	 */
	QList<ComponentUIViewer> getViewers( QWidget* parent, Qt::WindowFlags flags );
	/*! add the "Tests" menu to the menu bar of Total99
	 *  \param menuBar is the menu bar of the Total99 application
	 */
	void addAdditionalMenus( QMenuBar* menuBar );

private:

	ComponentUIViewer evogaControls( QWidget* parent, Qt::WindowFlags flags );
// 	ParameterSettableUIViewer icubview( QWidget* parent, Qt::WindowFlags flags );
	ComponentUIViewer fitview( QWidget* parent, Qt::WindowFlags flags );
	ComponentUIViewer statview( QWidget* parent, Qt::WindowFlags flags );
	ComponentUIViewer renderWorld( QWidget* parent, Qt::WindowFlags flags );
	ComponentUIViewer testIndividualUI( QWidget* parent, Qt::WindowFlags flags );

	EvoRobotComponent* evorobot;
	Evoga* ga;
	//! allow to load a file with fitness values stored in it
	QWidget* statViewer;
	FitViewer* ftv;			//! fitness viewer (min av max)
// 	VisionMapSensorGui *m_icubview;	//! widget that visualize icub's camera content
	RenderWorldWrapperWidget* renderworldwrapper;	//! renderworld wrapper
	/*! it display informations on EvoGA controls viewer */
	QLabel* infoEvoga;
	/*! slider used to setup the speed of simulation (aka. the step delay) */
	QSlider* simulationThrottle;
	/*! label showing the current speed of simulation */
	QLabel* simulationSpeed;
	/*! The timer to update the GUI */
	QTimer* timer;

private slots:
	/*! this update the graphics when the World has been updated */
	void onWorldAdvance();
	/*! this update the graphics when a replication is going to start
	 *  \param replication is the number of the replication
	 */
	void onEvogaStartingReplication( int replication );
	/*! this update the graphics when an interrupted evolution has been recovered
	 *  \param statfile is the file where there is the data of the interrupted evolution
	 */
	void onEvogaRecoveredInterruptedEvolution( QString statfile );
	/*! this update the graphics when a generation is finished
	 *  \param generation is the number of generation finished
	 *  \param fmax is the maximum fitness value
	 *  \param faverage is the average fitness
	 *  \param fmin is the minimum fitness value
	 */
	void onEvogaEndGeneration( int generation, double fmax, double faverage, double fmin );

	/*! this update the graphics when an action has been finished/stopped */
	void onActionFinished();

	/*! Go directly to next trial of the Evoga */
	void evogaNextTrial();

	/*! Setup the new step delay when simulationThrottle value change */
	void onSimulationThrottleChanges( int newvalue );

	/*! Ask for a file and load the fitness data stored in it */
	void loadStat();
	/*! Load all statistic stored */
	void loadAllStat();
};

// namespace VisionMapSensorGuiInternal {
// 	class ImageDisplayer;
// }
//
// /**
//  * \brief A gui visualizing the camera and the projection of objects on the
//  *        retina
//  *
//  * This keeps the current image and adds marks on it. If you set a new image
//  * all previous marks are lost
//  */
// class FARSA_EXPERIMENTS_API VisionMapSensorGui : public QWidget
// {
// 	Q_OBJECT
//
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * \param mapWidth the width of the neural map
// 	 * \param mapHeight the height of the neural map
// 	 * \param parent the parent of this widget
// 	 */
// 	VisionMapSensorGui(QWidget* parent=0, Qt::WindowFlags flags=0);
//
// 	/**
// 	 * \brief Destructor
// 	 */
// 	virtual ~VisionMapSensorGui();
//
// 	/**
// 	 * \brief Sets the new image to display
// 	 *
// 	 * This resets the old image and also changes the dimension of the
// 	 * widget to match that of the image
// 	 * \param image the new image
// 	 */
// 	void setImage(PhyiCub *icub, bool useRightCamera=true);
//
// 	/**
// 	 * \brief Adds a mark on the image at the given position
// 	 *
// 	 * x and y are normalized (i.e. within 0 and 1) coordinates. They are
// 	 * converted to the actual image coordinates by this function
// 	 * \param x the x coordinate of the mark (must be between 0 and 1)
// 	 * \param y the y coordinate of the mark (must be between 0 and 1)
// 	 * \param markCenter the color of the central part of the mark
// 	 * \param markSurrounding the color of the surrounding part of the mark
// 	 */
// 	void addMark(double x, double y, QColor markCenter = Qt::black, QColor markSurrounding = Qt::white);
//
// private:
// 	/**
// 	 * \brief The image with marks
// 	 */
// 	QImage m_image;
//
// 	/**
// 	 * \brief The helper widget displaying the image
// 	 */
// 	VisionMapSensorGuiInternal::ImageDisplayer* m_imageDisplayer;
// };

/**
 * \brief This widget allows the user to select a seed and test individual agents
 */
#warning THIS DHOULD USE UPLOADERS/DOWNLOADERS. AT THE MOMENT IT IS NOT THREAD SAFE AT ALL
class FARSA_EXPERIMENTS_API TestIndividualGUI : public QWidget
{
	Q_OBJECT
private:
	TestIndividual* test;
	QStringList fileList;
	QComboBox* combo;
	QListWidget* list;
public:
	TestIndividualGUI(TestIndividual* tb, QWidget *parent = NULL, Qt::WindowFlags flags=0);
private slots:
	void agentClicked();
	void populateCombo();
	void seedWasChosen();
};

} //end namespace farsa

#endif

