/********************************************************************************
 *  SALSA Genetic Algorithm Library                                             *
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

#include "core/evaluation.h"
#include "core/genome.h"

namespace salsa {

Evaluation::Evaluation() {
	isDone = false;
	isInitialized = false;
	isFinalized = true;
	genotypev = 0;
	genotypeid = -1;
	genomev = 0;
	ga = 0;
}

Evaluation::~Evaluation() {
	//--- nothing to do
}

void Evaluation::initialize( Genotype* gen ) {
	//--- check if it is initialized already and not finalized yet
	if ( isInitialized && !isFinalized ) {
		//--- nothing do to
		return;
	}
	genotypev = gen;
	if ( genome() ) {
		genotypeid = genome()->find( gen );
	}
	isDone = false;
	init();
	isInitialized = true;
	isFinalized = false;
}

void Evaluation::evaluateStep() {
	if ( !isInitialized ) {
		qFatal( "You Must Initialized Evaluation Process before calling evaluateStep()" );
		return;
	}
	step();
}

void Evaluation::evaluate() {
	while( ! isEvaluationDone() ) {
		evaluateStep();
	}
}

void Evaluation::finalize() {
	if ( !isInitialized && isFinalized ) {
		//--- nothing to do
		return;
	}
	fini();
	isInitialized = false;
	isFinalized = true;
}

bool Evaluation::isEvaluationDone() {
	return isDone;
}

void Evaluation::evaluationDone() {
	isDone = true;
}

} // end namespace salsa
