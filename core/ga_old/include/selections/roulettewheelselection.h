/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2008 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef ROUTLETTEWHEELSELECTION_H
#define ROUTLETTEWHEELSELECTION_H

#include "gaconfig.h"
#include "core/selection.h"
#include <QVector>

namespace farsa {

/*! \brief Roulette wheel selection operator
 *
 *  \par Description
 *    This class implements the roulette wheel selection scheme: each indivual
 *    has a probability of being selected for reproduction which is proportional
 *    to its fitness. It is possible to exclude from selection the individuals
 *    with the lowest fitness (the exact number is configurable)
 *  \par Warnings
 *
 * \ingroup ga_sel
 */
class FARSA_GA_API RouletteWheelSelection : public Selection {
public:
	/*! Constructor */
	RouletteWheelSelection();

	/*! Destructor */
	virtual ~RouletteWheelSelection();

	/*! Select a Genotype */
	virtual const Genotype* select();

	/*! Initializes RouletteWheelSelection. More in detail, computes the selection
	 *  probabilities for each individual
	 */
	virtual void setGenome( const Genome* g );

	/*! Sets the number of individuals which are discarded among those with the
	 *  lowest fitness
	 */
	void setNotSelectedForBreeding( int notSelectedForBreeding );

	/*! Returns the number of individuals which are discarded among those
	 *  with the lowest fitness
	 */
	int notSelectedForBreeding();

	/*! Configures the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with selection parameters
	 */
	virtual void configure( ConfigurationParameters& params, QString prefix );

	/**
	 * \brief Saves the actual status of parameters into the ConfigurationParameters object passed
	 *
	 * \param params the configuration parameters object on which save actual parameters
	 * \param prefix the prefix to use to access the object configuration parameters.
	 */
	virtual void save( ConfigurationParameters& params, QString prefix );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );

protected:
	/**
	 * \brief The number of individuals not selected for reproduction among
	 *        those with the lowest fitness
	 */
	int m_notSelectedForBreeding;

	/**
	 * \brief The vector with cumulative fitness for individuals
	 */
	QVector<double> m_cumulativeFitness;
};

} // end namespace farsa

#endif

