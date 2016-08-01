/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
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

#ifndef FIXEDSIZE_H
#define FIXEDSIZE_H

#include "gaconfig.h"
#include "core/reproduction.h"

namespace salsa {

class Selection;
class Mutation;
class Crossover;

/*!  \brief FixedSize Reproduction Class
 *
 *  \par Description
 *    The FixedSize Reproduction create a new Genome mantaining exactly the same size of
 *    incoming Genome; i.e.: the dimension of population never increase neither decrease.
 *  \par Warnings
 *
 * \ingroup ga_reprod
 */
class SALSA_GA_API FixedSize : public Reproduction {
public:
	/*! Constructor */
	FixedSize();
	/*! Destructor */
	virtual ~FixedSize();
	/*! Reproduction Process */
	virtual Genome* reproduction( const Genome* );
	/*! Set the Mutation to use */
	void setMutation( Mutation* muta );
	/*! Return the Mutation used */
	Mutation* mutation();
	/*! Set the Selection to use */
	void setSelection( Selection* sel );
	/*! Return the Selection used */
	Selection* selection();
	/*! Set the Crossover to use */
	void setCrossover( Crossover* muta );
	/*! Return the Mutation used */
	Crossover* crossover();
	/*! Set the probability to apply the Crossover operator
	 *  \note set to zero in order to disable Crossover
	 */
	void setCrossoverRate( double prob );
	/*! Return the probability to apply the Crossover */
	double crossoverRate();
	/*! Enable/Disable Elitism */
	void enableElitism( bool enable=true );
	/*! Return true is the Elitism is enabled */
	bool isElitismEnabled();
	/*! Set the number of firstly selected Genotype to copy exactly to new generation (elitism) */
	void setNumberElited( unsigned int nelited );
	/*! Return the number of elited Genotypes */
	int numberElited();

	/*! Configure by settings passed */
	virtual void configure( ConfigurationParameters& params, QString prefix );
	/**
	 * \brief Save the actual status of parameters into the ConfigurationParameters object passed
	 *
	 * \param params the configuration parameters object on which save actual parameters
	 * \param prefix the prefix to use to access the object configuration parameters.
	 */
	virtual void save( ConfigurationParameters& params, QString prefix );
	/*! Add to Factory::typeDescriptions() the descriptions of all parameters and subgroups */
	static void describe( QString type );

protected:
	/*! Selection used */
	Selection* sel;
	/*! Mutation used */
	Mutation* muta;
	/*! Crossover used */
	Crossover* cross;
	/*! Probability to apply Crossover */
	double probCrossover;
	/*! True if elitism is enabled */
	bool isElitism;
	/*! number of firstly selected Genotype to be elited */
	int nElited;
};

} // end namespace salsa

#endif
