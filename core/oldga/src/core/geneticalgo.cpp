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

#include "core/geneticalgo.h"
#include "core/genotype.h"
#include "core/genome.h"
#include "core/geneticalgoui.h"
#include <cmath>

namespace farsa {

GeneticAlgo::GeneticAlgo()
	: evolutionEnd(false),
	  evaluationDone(false),
	  genomev(NULL),
	  generationv(0),
	  numGens(0),
	  bestfits(),
	  avgfits(),
	  meanHdists(),
	  varHdists(),
	  stdHdists(),
	  iodelegate(new GeneticAlgoIODelegate()),
	  observers() {
	/* nothing else to do */
}

GeneticAlgo::~GeneticAlgo() {
	delete genomev;
}

ParameterSettableUI* GeneticAlgo::getUIManager() {
	return new GeneticAlgoUI( this );
}

void GeneticAlgo::addObserver( GeneticAlgoObserver* observer ) {
	if ( observers.contains(observer) ) return;
	observers.append( observer );
}

void GeneticAlgo::removeObserver( GeneticAlgoObserver* observer ) {
	observers.removeAll( observer );
}

void GeneticAlgo::evolve( unsigned int generationToReach ) {
	while((!isEvolutionEnded()) &&
	      ((generationv < generationToReach) || ((generationv == generationToReach) && (!evaluationDone)))) {
		gaStep();
	}
}

void GeneticAlgo::evolve( ) {
	while( !isEvolutionEnded() ) {
		gaStep();
	}
}

bool GeneticAlgo::isEvolutionEnded() {
	return evolutionEnd;
}

bool GeneticAlgo::isEvaluationDone() {
	return evaluationDone;
}

void GeneticAlgo::setGenome( Genome* gen ) {
	genomev = gen;
}

Genome* GeneticAlgo::genome() {
	return genomev;
}

const Genome* GeneticAlgo::genome() const {
	return genomev;
}

void GeneticAlgo::setGeneration( unsigned int gen ) {
	generationv = gen;
/*	The vectors will be resized when updateStats() will be called
	bestfits.resize( gen );
	avgfits.resize( gen );
	meanHdists.resize( gen );
	varHdists.resize( gen );
	stdHdists.resize( gen );*/
}

unsigned int GeneticAlgo::generation() const {
	return generationv;
}

void GeneticAlgo::setNumGenerations( int g ) {
	numGens = g;
}

int GeneticAlgo::numGenerations() {
	return numGens;
}

QVector< QVector<double> > GeneticAlgo::bestFits() const {
	return bestfits;
}

QVector< QVector<double> > GeneticAlgo::averageFits() const {
	return avgfits;
}

QVector<double> GeneticAlgo::meanHammingDist() const {
	return meanHdists;
}

QVector<double> GeneticAlgo::varianceHammingDist() const {
	return varHdists;
}

QVector<double> GeneticAlgo::standardDeviationHammingDist() const {
	return stdHdists;
}

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning QUI C È UN MEMORY LEAK, ALMENO PER IL PRIMO IODELEGATE (CHE VIENE CREATO NEL COSTRUTTORE)
#endif
void GeneticAlgo::setIODelegate( GeneticAlgoIODelegate* iodelegate ) {
	this->iodelegate = iodelegate;
}

GeneticAlgoIODelegate* GeneticAlgo::getIODelegate() const {
	return iodelegate;
}

void GeneticAlgo::updateStats() {
	// check if it is necessary to grow-up vectors
	bestfits.resize( generationv+1 );
	avgfits.resize( generationv+1 );
	meanHdists.resize( generationv+1 );
	varHdists.resize( generationv+1 );
	stdHdists.resize( generationv+1 );
	// Calculate statistic data for this generation
	QVector<double> bests;
	QVector<double> avegs;
	int numObjs = genomev->at(0)->numOfObjectives();
	bests.resize( numObjs );
	avegs.resize( numObjs );
	for( int i=0; i<numObjs; i++ ) {
		bests[i] = genomev->at(0)->objective(i);
		avegs[i] = bests[i];
	}
	for( unsigned int g=1; g<genomev->size(); g++ ) {
		for( int i=0; i<numObjs; i++ ) {
			bests[i] = qMax( bests[i], genomev->at(g)->objective(i) );
			avegs[i] += genomev->at(g)->objective(i);
		}
	}
	for( int i=0; i<numObjs; i++ ) {
		avegs[i] = avegs[i]/(genomev->size());
	}
	bestfits[generationv] = bests;
	avgfits[generationv] = avegs;
/* *****
	double meanH = 0;
	double varH = 0;
	// ****** Devo fare per forza O(n^2) calcoli ?!?!?
	Genome& genomeref = *genomev;
	unsigned int dim = genomeref.size();
	for( unsigned int i=0; i<dim; i++ ) {
		for( unsigned int j=0; j<dim; j++ ) {
			meanH += genomeref[i]->hammingDistance( genomeref[j] );
		}
	}
	meanH /= dim*dim;
	for( unsigned int i=0; i<dim; i++ ) {
		for( unsigned int j=0; j<dim; j++ ) {
			varH += pow( genomeref[i]->hammingDistance( genomeref[j] ) - meanH, 2 );
		}
	}
	varH /= (dim*dim)-1;
	//--- set data
	meanHdists[generationv] = meanH;
	varHdists[generationv] = varH;
	stdHdists[generationv] = sqrt( varH );
***** */
}

void GeneticAlgo::notifyEndGeneration() {
	foreach( GeneticAlgoObserver* obs, observers ) {
		obs->onEndGeneration();
	}
}

} // end namespace farsa
