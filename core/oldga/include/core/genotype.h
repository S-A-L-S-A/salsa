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

#ifndef GENOTYPE_H
#define GENOTYPE_H

#include "gaconfig.h"
#include "parametersettable.h"
#include <QString>
#include <QVector>

namespace farsa {

/*!  \brief Genotype class
 *
 *  \par Description
 *    Represent a bit-string
 *  \par Warnings
 *
 * \ingroup ga_core
 */
class FARSA_GA_API Genotype : public ParameterSettableWithConfigureFunction {
public:
	/*! Construct a un-initialized Genotype
	 *  \warning you must explicity call randomize() to get a randomized Genotype
	 */
	Genotype( unsigned int size = 0 );
	/*! Destructor */
	virtual ~Genotype();
	/*! Read the values from QString and set the size accordlying to length of QString */
	Genotype( QString, bool compressed=false );
	/*! Copy-Constructor (generate a clone of the genoma) */
	Genotype( const Genotype& genotype );
	/*! Assign operator (performs same operation of Copy-constructor) <br>
	 *  Due to virtual attribute, Subclasses can customize the behaviour of this operator=,
	 *  for an easy way to copy Genotype data the protected method copyDataFrom is provided
	 *  and it behaves like this implementation of operator=
	 */
	virtual Genotype& operator=( const Genotype& genotype );
	/*! Assign method (it call operator=) */
	void assign( const Genotype* genotype );
	/*! Create an exact copy of this Genotype */
	virtual Genotype* clone() const;
	/*! Return the size of Genotype */
	unsigned int size() const;
	/*! Set the fitness value of this Genotype
	 *  \note it is the same of setObjective( 0, value );
	 */
	void setFitness( double value );
	/*! Return the fitness of this Genotype
	 *  \note it is the same of objective(0);
	 */
	double fitness() const;
	/*! Set the i-th objective value of the (multi-objective) fitness
	 *	\note multi-objective fitness are used only with the GeneticAlgo that support them
	 *	\note setFitness( aValue ) correspond to setObjectiveValue( 0, value );
	 *	\note in a single objective fitness, the setObjectiveValue could be used to print out
	 *	      some intermediate values used for calculating the total fitness
	 */
	void setObjective( int i, double value );
	/*! Return the value of the i-th objective of the fitness
	 *  \note fitness() return the same value of objective(0)
	*/
	double objective( int i ) const;
	/*! Return the number of Objectives setted
	 *  The number of objectives is automatically configured by setObjective method
	 */
	int numOfObjectives() const;
	/*! Return True if this Genotype is dominated by the Genotype specified by param
		\warning the two Genotypes should have the same number of objectives; if this
		 constraint is not meet then the number of objectives compared will be the
		 minimum between the two genotypes
		\note Definition of Domination: a Genotype A is dominated by Genotype B iff all
		 objective values of A are less-equal of corresponding objective values of B and
		 for at least one objective the value of A is strictly less that value in B
	 */
	bool dominatedBy( const Genotype* genotype );
	/*! Set the rank of the genotype
	 *  \note the rank is a general single value used to order the genotypes inside the genome;
	 *    it is different from fitness and it is GA-dependent.
	 *    In simple cases, like SimpleGA, where the GA is driven by only one objective, then
	 *    tipically the rank correspond to the fitness.
	 *    But in multi-objective GAs (MOGA), like NSGA2, the rank depends on how the MOGA orders
	 *    the genotypes
	 *  \note better genotype will have higher rank value
	 */
	void setRank( double rank );
	/*! Return the rank of this Genotype */
	double rank() const;
	/*! Return the i-th bit */
	bool bit( unsigned int i ) const;
	/*! Set to One the i-th bit */
	void set( unsigned int i );
	/*! set to zero the i-th bit */
	void unset( unsigned int i );
	/*! toggle the i-th bit */
	void toggle( unsigned int i );
	/*! Return the notes of this Genotype */
	QString notes() const;
	/*! Set the notes of this Genotype */
	void setNotes( QString notes );
	/*! Calculate the Hamming distance from the Genotype */
	int hammingDistance( const Genotype* );
	/*! Randomize the value contained (attention, this method destroys previous data) */
	virtual void randomize();
	/*! Return a QString representing this Genotype */
	QString toString() const;
	/*! Configure the bits accordlying to its string representation passed */
	void fromString( QString );
	/*! Compress the data; It return a printable String */
	QString toCompressedString() const;
	/*! Read from compressed data with toCompressedString()
	 *  \return true on successfull decompression, false otherwise
	 */
	bool fromCompressedString( QString str );
	/*!  Utility function for extracting an integer value
	 *  \param startPos is the index of starting position of the bits to decode
	 *  \param stopPos is the index of end position of the bits to decode
	 *  <pre>
	 *      +----------------------------+
	 *      | 0 1 0 0 1 0 1 1 0 1 0 1 0  |
	 *      +----------------------------+
	 *  </pre>
	 *  if startPos is 4 and stopPos is 8 the bits 1011 is decode and this methods will returns 11 <br>
	 *  if stopPos is beyond the length of Genotype, ad example startPos 9 and stopPos 15 then the
	 *  bits 1010 will be decoded and this methods will return 10.<br>
	*/
	unsigned int extractUInt( unsigned int startPos, unsigned int stopPos ) const;
	/*!  Utility function for settings an integer value into Genotype
	 *  \param value is the value to be converted and inserted into genotype
	 *  \param startPos is the index of starting position of the bits to decode
	 *  \param stopPos is the index of end position of the bits to decode
	 *  The parameters has the same meanings of extractUInt, and the behaviour is exactly the opposite.
	*/
	void insertUInt( unsigned int value, unsigned int startPos, unsigned int stopPos );

	/*!
	 * Convert the index of the given gene to the index of the start of the
	 * bit(s) that encode this gene.
	 */
	virtual unsigned int geneToBitIndex( unsigned int gene ) const;
	/*!
	 * Convert the index of the given bit to the index of the gene that this
	 * bit is part of.
	 */
	virtual unsigned int bitToGeneIndex( unsigned int bit ) const;

	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with genotype parameters
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

	/*! Utility function comparing two genotype pointers on their rank */
	static bool rankGreaterThanComparator( const Genotype *g1, const Genotype *g2 ) {
		return (g1->rank() > g2->rank());
	};
	/*! Utility class for generating a function for comparing genotype of an objective */
	class nObjectiveGreaterThanComparator {
	public:
		/*! Comparator Function, it return true if g1 has the objectiveToCompare value
		 *  greater than g2
		 */
		bool operator()( const Genotype* g1, const Genotype* g2 ) {
			return g1->objective( objectiveToCompare ) > g2->objective( objectiveToCompare );
		};
		int objectiveToCompare;
	};

protected:
	/*! Data represented as a char vector allocated in memory */
	unsigned char* data;
	/*! Size */
	unsigned int sizev;
	/*! Allocated memory */
	unsigned int allocated;
	/*! Objective values of the Fitness */
	QVector<double> fitnessv;
	/*! Rank */
	double rankv;
	/*! Notes about this Genotype. Usage is left open. */
	QString notesv;
	/*! Utility method for copying data among Genotype */
	void copyDataFrom( Genotype* source );

	/*! Resize the Genotype
	 *  \param newsize is the number of bits after resizing
	 *  \warning if the new dimension is shorter than the old one, some data will be discarded
	 *  \warning The data pointer will be changed after this method execution
	 */
	void resize( unsigned int newsize );
};

} // end namespace farsa

#endif
