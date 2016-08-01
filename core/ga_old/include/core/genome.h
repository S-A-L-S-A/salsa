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

#ifndef GENOME_H
#define GENOME_H

#include "gaconfig.h"
#include "parametersettable.h"
#include "core/genotype.h"
#include <QVector>

namespace salsa {

/*!  \brief Genome class
 *
 *  \par Description
 *    Contains Genotype
 *  \par Warnings
 *    When a Genotype is inserted into a Genome using append or set, then Genome will create
 *    an internal copy of such Genotype (so, it's responsability of the user to delete the
 *    copy passed to append (or set))
 * \ingroup ga_core
 */
class SALSA_GA_API Genome : public ParameterSettableWithConfigureFunction {
public:
	/*! Construct an un-initialized Genome with no genotypes */
	Genome();
	/*! Construct a Genome with random Genotypes
	 * \param numGenotype is the number of Genotype contained by this Genome
	 * \param numBits is the number of bits of Genotypes (see Genotype::size())
	 */
	Genome( unsigned int numGenotype, unsigned int numBits );
	/*! Construct a Genome contains clones of Genotype prototype passed
	 *
	 * After creation from prototype genomes are randomized
	 * \param numGenotype is the number of Genotype contained by this Genome
	 * \param prototype is the Genotype prototype that Genome contains
	 */
	Genome( unsigned int numGenotype, const Genotype* prototype );
	/*! Copy-Constructor */
	Genome( const Genome& genome );
	/*! Destructor
	 *
	 * This destroys the prototipe and all genotypes
	 */
	~Genome();
	/*! assignment operato */
	Genome& operator=( const Genome& right );
	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with genome parameters
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

	/*! Return the size of Genome, i.e. the number of Genotype contained */
	unsigned int size() const;
	/*! Erase all element and set the size to zero
	 *  \warning It doesn't destroy Genotype, it only remove them from Genome
	 */
	void clearAll();
	/*! Append a clone of Genotype passed to this Genome */
	void append( const Genotype* );
	/*! Return the i-th Genotype */
	Genotype* at( unsigned int i );
	/*! Return the i-th Genotype (const version)*/
	const Genotype* at( unsigned int i ) const;
	/*! Return the last Genotype contained (i.e.: at( size()-1 ) */
	Genotype* last();
	/*! Return the last Genotype contained (i.e.: at( size()-1 ) */
	const Genotype* last() const;
	/*! Find the Genotype requested and return its index in this Genome
	 *  \return -1 Genotype there isn't, the index if there is
	 */
	unsigned int find( const Genotype* g );
	/*! Set the i-th Genotype */
	void set( unsigned int i, Genotype* g );
	/*! Return the i-th Genotype (const version) */
	const Genotype* operator[]( unsigned int i ) const;
	/*! Return the i-th Genotype (non-const version) */
	Genotype* operator[]( unsigned int i );
	/*! Return the prototype used for creating all others Genotypes */
	Genotype* prototypeGenotype() {
		return prototype;
	};
	/*! Randomize the Genotype inserted */
	void randomize();
	/*! Return the begin iterator for use QtAlgorithms */
	QVector<Genotype*>::iterator begin() {
		return data.begin();
	};
	/*! Return the begin iterator for use QtAlgorithms */
	QVector<Genotype*>::const_iterator begin() const {
		return data.begin();
	};
	/*! Return the end iterator for use QtAlgorithms */
	QVector<Genotype*>::iterator end() {
		return data.end();
	};
	/*! Return the end iterator for use QtAlgorithms */
	QVector<Genotype*>::const_iterator end() const {
		return data.end();
	};

private:
	/*! Data */
	QVector<Genotype*> data;
	/*! Prototype Genotype used during resizing and configuration */
	Genotype* prototype;
};

} // end namespace salsa

#endif
