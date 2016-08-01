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

#include "gaconfig.h"
#include "factory.h"
#include "core/genome.h"
#include "core/selection.h"
#include "core/mutation.h"
#include "core/crossover.h"
#include "reproductions/fixedsize.h"
#include "randomgenerator.h"

namespace salsa {

FixedSize::FixedSize()
	: Reproduction() {
	sel = 0;
	muta = 0;
	cross = 0;
	isElitism = false;
	nElited = 0;
	probCrossover = 0;
}

FixedSize::~FixedSize() {
	//--- nothing to do
}

Genome* FixedSize::reproduction( const Genome* genome ) {
	Q_ASSERT_X( sel != 0,
				"FixedSize::reproduction",
				"You must set a Selection object before calling this method" );
	Q_ASSERT_X( muta != 0,
				"FixedSize::reproduction",
				"You must set a Mutation object before calling this method" );
	Q_ASSERT_X( cross != 0 || probCrossover == 0 ,
				"FixedSize::reproduction",
				"You must set a Crossover object before calling this method" );
	Q_ASSERT_X( nElited >= 0,
				"FixedSize::reproduction",
				"The number of Elited Genotypes must be greater than zero" );
	
	Genome* ret = new Genome( 0, genome->at(0) );
	muta->setGenome( genome );
	cross->setGenome( genome );
	sel->setGenome( genome );
	int left = genome->size();
	if ( isElitism ) {
		for( int i=0; i<nElited; i++ ) {
			ret->append( genome->at(i) );
		}
		left -= nElited;
	}
	for( int i=0; i<left; i++ ) {
		//--- Select a Genotype, and create a copy of it
		ret->append( sel->select() );
		//--- crossover
		if ( globalRNG->getBool( probCrossover ) ) {
			cross->crossover( ret->last(), sel->select() );
		}
		//--- mutation
		muta->mutate( ret->last() );
	}
	return ret;
}

void FixedSize::setMutation( Mutation* muta ) {
	this->muta = muta;
	this->muta->setGA( GA() );
}

Mutation* FixedSize::mutation() {
	return muta;
}

void FixedSize::setSelection( Selection* sel ) {
	this->sel = sel;
	this->sel->setGA( GA() );
}

Selection* FixedSize::selection() {
	return sel;
}

void FixedSize::setCrossover( Crossover* cross ) {
	this->cross = cross;
	this->cross->setGA( GA() );
}

Crossover* FixedSize::crossover() {
	return cross;
}

void FixedSize::setCrossoverRate( double prob ) {
	this->probCrossover = prob;
}

double FixedSize::crossoverRate() {
	return probCrossover;
}

void FixedSize::enableElitism( bool enable ) {
	isElitism = enable;
}

bool FixedSize::isElitismEnabled() {
	return isElitism;
}

void FixedSize::setNumberElited( unsigned int nelited ) {
	this->nElited = nelited;
}

int FixedSize::numberElited() {
	return nElited;
}

void FixedSize::configure( ConfigurationParameters& params, QString prefix ) {
	setMutation( params.getObjectFromGroup<Mutation>( prefix + QString( "MUTATION" ) ) );
	setSelection( params.getObjectFromGroup<Selection>( prefix + QString( "SELECTION" ) ) );
	setCrossover( params.getObjectFromGroup<Crossover>( prefix + QString( "CROSSOVER" ) ) );
	probCrossover = params.getValue( prefix + QString( "crossoverRate" ) ).toDouble();
	isElitism = ! (params.getValue( prefix + QString( "elitism" ) ).compare( "true", Qt::CaseInsensitive ) );
	nElited = params.getValue( prefix + QString( "nelited" ) ).toInt();
}

void FixedSize::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "FixedSize" );
	params.createParameter( prefix, QString("crossoverRate"), QString("%1").arg( probCrossover ) );
	params.createParameter( prefix, QString("elitism"), isElitism ? "true" : "false" );
	params.createParameter( prefix, QString("nelited"), QString("%1").arg( nElited ) );
	//--- MUTATION
	if ( mutation() ) {
		mutation()->save( params, params.createSubGroup( prefix, "MUTATION" ) );
	}
	//--- SELECTION
	if ( selection() ) {
		selection()->save( params, params.createSubGroup( prefix, "SELECTION" ) );
	}
	//-- CROSSOVER
	if ( crossover() ) {
		crossover()->save( params, params.createSubGroup( prefix, "CROSSOVER" ) );
	}
}

void FixedSize::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Create a new generation mantaining constant the number of individuals", "It apply the selection, crossover and mutation operators without changing the size of the Genome" );
	d.describeReal( "crossoverRate" ).limits( 0, 1 ).def( 0.1 ).help( "The probability to apply the crossover operator" );
	d.describeBool( "elitism" ).def( false ).help( "Enable/Disable the elitism" );
	d.describeInt( "nelited" ).limits( 0, INT_MAX ).def( 10 ).help( "The number of individual to copy exactly to the new generation; only if elitism is true" );
	d.describeSubgroup( "MUTATION" ).type( "Mutation" ).props( IsMandatory ).help( "The mutation operator used to mutate Genotypes" );
	d.describeSubgroup( "SELECTION" ).type( "Selection" ).props( IsMandatory ).help( "The selection operator used to select Genotypes for reproduction" );
	d.describeSubgroup( "CROSSOVER" ).type( "Crossover" ).props( IsMandatory ).help( "The crossover operator used to generate offsprings from Genotypes" );
}

} // end namespace salsa
