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

#ifndef DETERMINISTICRANK_H
#define DETERMINISTICRANK_H

#include "gaconfig.h"
#include "core/selection.h"

namespace farsa {

/*!  \brief Deterministic Rank Selection
 *
 *  \par Description
 *   DeterministicRank select Genotypes starting from the first and cycling modulo
 *   truncation parameter.
 *  \par Warnings
 *
 * \ingroup ga_sel
 */
class FARSA_GA_API DeterministicRank : public Selection {
public:
	/*! Constructor */
	DeterministicRank();
	/*! Destructor */
	virtual ~DeterministicRank();
	/*! Select a Genotype */
	virtual const Genotype* select();
	/*! It initialize DeterministicRank. In details, it will sort the Genome and
	 *  the next select() will return the first (best) Genotype
	 */
	virtual void setGenome( const Genome* g );
	/*! Set the truncation parameters */
	void setTruncation( int nTruncation );
	/*! Return truncation */
	int truncation();
	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with selection parameters
	 */
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
	/*! Truncation; i.e.: number of selected Genotype */
	int nTruncation;
	/*! Id of the Genotype to be returned on next select() calls */
	int idNext;
};

} // end namespace farsa

#endif
