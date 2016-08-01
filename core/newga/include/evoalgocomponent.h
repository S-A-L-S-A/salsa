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

#ifndef EVOALGOCOMPONENT_H
#define EVOALGOCOMPONENT_H

#include "newgaconfig.h"
#include "parametersettable.h"
#include "evoalgo.h"
#include "evoalgotest.h"
#include "baseexperiment.h"

namespace salsa {

/**
 * \brief This class setups an evolutionary experiment.
 *
 * It inherits from the base experiment.
 */
class SALSA_NEWGA_API EvoAlgoComponent : public BaseExperiment
{
	Q_OBJECT
public:
	/**
	 * \brief Constructor
	 */
	EvoAlgoComponent();
	/**
	 * \brief Destructor
	 */
	~EvoAlgoComponent();
	virtual void configure(ConfigurationParameters& params, QString prefix);
	virtual void save(ConfigurationParameters& params, QString prefix);
    static void describe(QString type);
	virtual void postConfigureInitialization();
public slots:
	void evolve();
	void runTest(int idTest);
private:
	//! batchRunning - if true EvoAlgoComponent will not use the gaThread
	bool m_batchRunning;
	//! The evolutionary algorithm
	EvoAlgo* m_ga;
	//! The number of replications
	int m_replications;
	//! The array of tests
	QVector<NewAbstractTest*> m_tests;
};

} // end namespace salsa

#endif
