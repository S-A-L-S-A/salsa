/********************************************************************************
 *  SALSA Experiments Library                                                   *
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

#ifndef EVOALGOTESTCURRENT_H
#define EVOALGOTESTCURRENT_H

#include "parametersettable.h"
#include "configurationparameters.h"
#include "evoalgotest.h"
#include <iostream>
#include <cstdlib>
#include <QObject>
#include <QString>
#include <QMap>
#include "logger.h"

namespace salsa {

/**
 * \brief Class for test of the selected individual
 *
 * This is the abstract class for those performing tests on evolved individuals.
 * The main function is runTest(), which is pure virtual. This function is the
 * one that gets executed when a test is requested. The configure and
 * postConfigureInitialization functions have an empty implementaion, while the
 * save implementation simply prints an error message and causes an abort. When
 * writing subclasses, ALWAYS remember to register the class with the factory,
 * so that it can be instantiated when needed.
 */
class SALSA_NEWGA_API EvoAlgoTestCurrent : public EvoAlgoTest
{

public:
	/**
	 * \brief Constructor
	 */
	EvoAlgoTestCurrent();

	/**
	 * \brief Destructor
	 */
	virtual ~EvoAlgoTestCurrent()
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
		Logger::error("NOT IMPLEMENTED (EvoAlgoTestCurrent::save)");
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

	void runTest();
};

} //end namespace salsa

#endif
