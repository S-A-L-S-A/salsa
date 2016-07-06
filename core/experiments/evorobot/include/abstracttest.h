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

#ifndef ABSTRACT_TEST_H
#define ABSTRACT_TEST_H

// #include <iostream>
#include <cstdlib>
#include <QObject>
#include <QString>
#include <QMap>
#include "logger.h"
#include "experimentsconfig.h"

#warning TESTS MUST CHANGE, SEE COMMENT BELOW
// Tests must change: those needing a GUI (like TestIndividual) must declare the
// GUI themselves and communicate with it using DataUploaderDownloader. In general
// EvorobotComponent must keep the list of tests and perhaps an internal map from
// test names to tests (remove the AvailableTestList class). There can also be
// controls in the control gui to select and run tests. However do not implement
// those things now: first think how to change the whole structure (the aim is to
// get rid of all the Evo* mess). See also comment in baseexperiment.h
// For the moment we re-add tests as simple classes (not Components) and
// directly instantiate them from EvorobotComponent. This means that new tests
// cannot be added at runtime

namespace farsa {

class EvoRobotComponent;

/**
 * \brief An abstract class for tests of evolved individuals
 *
 * This is the abstract class for those performing tests on evolved individuals.
 * The main function is runTest(), which is pure virtual. This function is the
 * one that gets executed when a test is requested. The configure and
 * postConfigureInitialization functions have an empty implementaion, while the
 * save implementation simply prints an error message and causes an abort. When
 * writing subclasses, ALWAYS remember to register the class with the factory,
 * so that it can be instantiated when needed.
 */
class FARSA_EXPERIMENTS_TEMPLATE AbstractTest
{
public:
	/**
	 * \brief Constructor
	 */
	AbstractTest() :
		m_menuText("Abstract Text"),
		m_tooltip("Abstract Text"),
		m_iconFilename(QString()),
		m_component(NULL)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~AbstractTest()
	{
	}

	/**
	 * \brief Sets the Component
	 *
	 * We need it to obtain all the data for the test: the experiment object, genotype loading,
	 * and so on.
	 * \param component a pointer to the EvoRobotComponent
	 * \warning this hierarchy of class for testing works only with EvoRobotComponent
	 */
	virtual void setComponent(EvoRobotComponent* component)
	{
		m_component = component;
	}

	/**
	 * \brief Returns a pointer to the Component
	 *
	 * Use it in subclasses for get all the data necessary for implementing the test
	 * \return a pointer to the EvoRobotComponent
	 * \warning this hierarchy of class for testing works only with EvoRobotComponent
	 */
	EvoRobotComponent* component()
	{
		return m_component;
	}

	/*! \brief the text to show on the menu/toolbar
	 *
	 *  Subclasses can change the protected variable m_menuText in order to customize it
	 */
	QString menuText() {
		return m_menuText;
	}

	/*! \brief the tooltip text to show on the menu/toolbar
	 *
	 *  Subclasses can change the protected variable m_tooltip in order to customize it
	 */
	QString tooltip() {
		return m_tooltip;
	}

	/*! \brief the filename of the icon to show on the menu/toolbar
	 *
	 *  Subclasses can change the protected variable m_iconFilename in order to customize it
	 */
	QString iconFilename() {
		return m_iconFilename;
	}

	/**
	 * \brief Runs the test
	 */
	virtual void runTest() = 0;

protected:
	//! change in in subclasses for setting the text will appear on the menu/toolbar
	QString m_menuText;
	//! change in in subclasses for setting the tooltip will appear on the menu/toolbar
	QString m_tooltip;
	//! change in in subclasses for setting the icon will appear on the menu/toolbar
	QString m_iconFilename;

private:
	/**
	 * \brief The EvoRobotComponent object.
	 * \note Subclasses must use the component() function to get this
	 */
	EvoRobotComponent *m_component;
};

// /**
//  * \brief A class with only static members holding the list of available tests
//  *
//  * This class allows registering and retrieving the tests which are available
//  * in the current execution of the program. The list of tests available in the
//  * current execution depends on the content of the configuration file (see the
//  * main() function for details)
//  */
// class FARSA_EXPERIMENTS_API AvailableTestList
// {
// public:
// 	/**
// 	 * \brief Adds a test to the list of available ones
// 	 *
// 	 * \param name the name of the test
// 	 * \param test the test
// 	 */
// 	static void addTest(QString name, AbstractTest *test);
//
// 	/**
// 	 * \brief Returns the list of the names of available tests
// 	 *
// 	 * \return the list of the names of available tests
// 	 */
// 	static QStringList getList();
//
// 	/**
// 	 * \brief Returns the test with the given name
// 	 *
// 	 * \return the test with the given name or NULL if no test exists with
// 	 *         that name
// 	 */
// 	static AbstractTest* getTest(QString name);
//
// private:
// 	/**
// 	 * \brief The map of available tests
// 	 */
// 	static QMap<QString, AbstractTest*> m_map;
// };

} //end namespace farsa

#endif
