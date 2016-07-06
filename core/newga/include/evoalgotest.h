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

#ifndef EVOALGO_TEST_H
#define EVOALGO_TEST_H

#include "parametersettable.h"
#include "configurationparameters.h"
#include "evoalgo.h"
#include "genotype.h"
#include "genotypetester.h"
#include "robotexperiment.h"
#include <iostream>
#include <cstdlib>
#include <QObject>
#include <QString>
#include <QMap>
#include "logger.h"

namespace farsa {

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
class FARSA_NEWGA_API NewAbstractTest : public ParameterSettableWithConfigureFunction, public FlowControlled
{

public:
	/**
	 * \brief Constructor
	 */
	NewAbstractTest();

	/**
	 * \brief Destructor
	 */
	virtual ~NewAbstractTest()
	{
	}

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
	virtual void save(ConfigurationParameters& params, QString prefix)
	{
		Logger::error("NOT IMPLEMENTED (NewAbstractTest::save)");
		abort();
	}

	/*! Describe Function */
	static void describe( QString type );

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 */
	virtual void postConfigureInitialization()
	{
	}

	
	/*! \brief the text to show on the menu/toolbar
	 *
	 *  Subclasses can change the protected variable m_menuText in order to customize it
	 */
	QString name() const {
		return m_name;
	}

	/*! \brief the tooltip text to show on the menu/toolbar
	 *
	 *  Subclasses can change the protected variable m_tooltip in order to customize it
	 */
	QString tooltip() const {
		return m_tooltip;
	}

	/*! \brief the filename of the icon to show on the menu/toolbar
	 *
	 *  Subclasses can change the protected variable m_iconFilename in order to customize it
	 */
	QString iconFilename() const {
		return m_iconFilename;
	}
	/**
	 * \brief Runs the test
	 */
	virtual void runTest() = 0;
protected:
	//! change in in subclasses for setting the text will appear on the menu/toolbar
	QString m_name;
	//! change in in subclasses for setting the tooltip will appear on the menu/toolbar
	QString m_tooltip;
	//! change in in subclasses for setting the icon will appear on the menu/toolbar
	QString m_iconFilename;
};

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
class FARSA_NEWGA_API EvoAlgoTest : public NewAbstractTest
{

public:
	/**
	 * \brief Constructor
	 */
	EvoAlgoTest();

	/**
	 * \brief Destructor
	 */
	virtual ~EvoAlgoTest()
	{
	}

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
	virtual void save(ConfigurationParameters& params, QString prefix)
	{
		Logger::error("NOT IMPLEMENTED (EvoAlgoTest::save)");
		abort();
	}

	/*! Describe Function */
	static void describe( QString type );

	/**
	 * \brief This function is called after all linked objects have been
	 *        configured
	 */
	virtual void postConfigureInitialization()
	{
	}
protected:
	EvoAlgo* getGA()
	{
		return m_ga;
	}
	const EvoAlgo* getGA() const
	{
		return m_ga;
	}
	Genotype* getPrototype()
	{
		return m_prototype;
	}
	const Genotype* getPrototype() const
	{
		return m_prototype;
	}
	SingleGenotypeTester* getGenotypeTester()
	{
		return m_gt;
	}
	const SingleGenotypeTester* getGenotypeTester() const
	{
		return m_gt;
	}
	RobotExperiment* getGAEvaluator()
	{
		return m_gae;
	}
	const RobotExperiment* getGAEvaluator() const
	{
		return m_gae;
	}
private:
	/**
	 * \brief The genetic algorithm
	 */
	EvoAlgo *m_ga;
	/**
	 * \brief The genotype
	 */
	Genotype* m_prototype;
	/**
	 * \brief The evaluator
	 */
	SingleGenotypeTester* m_gt;
	/**
	 * \brief The evaluator
	 */
	RobotExperiment* m_gae;
};

} //end namespace farsa

#endif
