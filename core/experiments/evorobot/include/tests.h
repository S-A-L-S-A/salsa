/********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Piero Savastano <piero.savastano@istc.cnr.it>                               *
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

#ifndef TESTS_H
#define TESTS_H

#include "abstracttest.h"

namespace salsa {

/*! \brief Test a robot using a random genotype
 */
class SALSA_EXPERIMENTS_API TestRandom : public AbstractTest
{
public:
	/*! Constructor
	 */
	TestRandom();
	/*! Destructor */
	virtual ~TestRandom();
	/*! Run the test setting random genotype */
	virtual void runTest();
	/*! Create a random genotype */
	int* buildRandomDNA();
};

/*! \brief Test a specific individual taken from evolutionary data
 */
class SALSA_EXPERIMENTS_API TestIndividual : public AbstractTest
{
public:
	/*! Constructor
	 */
	TestIndividual();
	/*! Destructor */
	virtual ~TestIndividual();
	/*! Run the test on the individual selected */
	virtual void runTest();
	/*! Set id of the individual to test
	 *  \param idIndividual is the id of the individual in the current population
	 *  \note it takes the individual from the current population loaded into GA.
	 */
	void setIndividualToTest( int idIndividual );
	/*! Replace the current population loaded into GA with population loaded from file
	 *  \param filename the file from which load the population
	 *  \param deferLoading if true (default behaviour) the loading of the population will
	 *    take when the runTest() will be called, if false the loading of the population will
	 *    be done immediately
	 */
	void setPopulationToTest( QString filename, bool deferLoading = true );
private:
	//! the individual to test
	int idIndividual;
	//! the file name containing the population to test
	QString populationFile;
	//! boolean indicating if the population has been already loaded
	bool populationLoaded;
};

/*! \brief Tests the current individual, i.e. doesn't load nor randomize the
 *         genome
 */
class SALSA_EXPERIMENTS_API TestCurrent : public AbstractTest
{
public:
	/*! Constructor
	 */
	TestCurrent();
	/*! Destructor */
	virtual ~TestCurrent();
	/*! Run the test on the individual selected */
	virtual void runTest();
};

} //end namespace salsa

#endif
