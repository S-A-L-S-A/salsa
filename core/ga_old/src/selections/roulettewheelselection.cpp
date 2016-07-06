/********************************************************************************
 *  FARSA Genetic Algorithm Library                                             *
 *  Copyright (C) 2007-2009 Gianluca Massera <emmegian@yahoo.it>                *
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
#include "core/genome.h"
#include "selections/roulettewheelselection.h"
#include "configurationparameters.h"
#include "randomgenerator.h"

namespace farsa {

RouletteWheelSelection::RouletteWheelSelection() :
	Selection(),
	m_notSelectedForBreeding(0),
	m_cumulativeFitness()
{
}

RouletteWheelSelection::~RouletteWheelSelection()
{
	//--- nothing to do
}

const Genotype* RouletteWheelSelection::select()
{
	int selected;
	double reference = globalRNG->getDouble(0, m_cumulativeFitness.last() );
	for (selected = m_cumulativeFitness.size() - 2; selected >= 0 ; selected-- ) {
		if( m_cumulativeFitness[selected] < reference ) {
			break;
		}
	}
	return genome()->at( ++selected );
}

void RouletteWheelSelection::setGenome( const Genome* g )
{
	Selection::setGenome( g );
	m_cumulativeFitness.resize(g->size() - m_notSelectedForBreeding );
	double tmp = 0.0;
	for( int i = 0; i < m_cumulativeFitness.size(); i++ ){
		m_cumulativeFitness[i] = tmp + g->at(i)->rank();
		tmp = m_cumulativeFitness[i];
	}
}

void RouletteWheelSelection::setNotSelectedForBreeding( int notSelectedForBreeding )
{
	m_notSelectedForBreeding = notSelectedForBreeding;
}

int RouletteWheelSelection::notSelectedForBreeding()
{
	return m_notSelectedForBreeding;
}

void RouletteWheelSelection::configure( ConfigurationParameters& params, QString prefix )
{
	m_notSelectedForBreeding = params.getValue( prefix + QString( "notSelectedForBreeding" ) ).toInt();
}

void RouletteWheelSelection::save( ConfigurationParameters& params, QString prefix )
{
	params.createParameter( prefix, QString("type"), "RouletteWheelSelection" );
	params.createParameter( prefix, QString("notSelectedForBreeding"), QString("%1").arg( m_notSelectedForBreeding ) );
}

void RouletteWheelSelection::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Selects the individual with a probabilistic roulette wheel method" );
	d.describeInt( "notSelectedForBreeding" ).limits( 0, INT_MAX ).help( "The worst notSelectedForBreeding individuals will be never selected for reproducing. The selection probability of an individual is proportional to its rank value" );
}

} // end namespace farsa
