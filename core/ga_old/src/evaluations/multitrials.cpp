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

#include "evaluations/multitrials.h"
#include "configurationparameters.h"

namespace farsa {

MultiTrials::MultiTrials( int steps, int trials )
	: Evaluation() {
	numTrials = steps;
	numSteps = trials;
	incrementStep = true;
}

MultiTrials::~MultiTrials() {
	//--- nothing to do
}

void MultiTrials::setTrials( int t ) {
	numTrials = t;
}

int MultiTrials::trials() const {
	return numTrials;
}

int MultiTrials::currentTrial() const {
	return currTrial;
}

void MultiTrials::setSteps( int s ) {
	numSteps = s;
}

int MultiTrials::steps() const {
	return numSteps;
}

int MultiTrials::currentStep() const {
	return currStep;
}

void MultiTrials::trialDone() {
	currStep = numSteps+1;
}

void MultiTrials::configure( ConfigurationParameters& params, QString prefix ) {
	setSteps( params.getValue( prefix + QString( "nsteps" ) ).toInt() );
	setTrials( params.getValue( prefix + QString( "ntrials" ) ).toInt() );
	if ( steps() == 0 ) {
		qWarning( "Setting the number of Steps to ZERO!! Check you config file" );
	}
	if ( trials() == 0 ) {
		qWarning( "Setting the number of Trials to ZERO!! Check you config file" );
	}
}

void MultiTrials::save( ConfigurationParameters& params, QString prefix ) {
	params.createParameter( prefix, QString("type"), "MultiTrials" );
	params.createParameter( prefix, QString("nsteps"), QString("%1").arg(steps()) );
	params.createParameter( prefix, QString("ntrials"), QString("%1").arg(trials()) );
}

void MultiTrials::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Evaluation of the fitness based on more trials", "This evaluation of the fitness suppose that each individual will be tested on ntrials trials on which each lasts nsteps steps" );
	d.describeInt( "nsteps" ).limits( 1, INT_MAX ).help( "The number of steps for each trial" );
	d.describeInt( "ntrials" ).limits( 1, INT_MAX).help( "The number of trials to do" );
}

void MultiTrials::remainInCurrentStep(bool remain)
{
	incrementStep = !remain;
}

bool MultiTrials::remainingInCurrentStep() const
{
	return !incrementStep;
}

void MultiTrials::init() {
	currStep = 0;
	currTrial = 0;
	mainInit();
	trialInited = false;
}

void MultiTrials::step() {
	if ( !trialInited ) {
		//--- initialize the Trial
		if ( !trialInit( currTrial ) ) return;
		trialInited = true;
	}
	//--- do a step
	trialStep( currStep, currTrial );
	if (incrementStep) {
		currStep++;
		if ( currStep >= numSteps ) {
			//--- reached the maximum number of steps,
			//--- pass to next trial
			trialFini( currTrial );
			currTrial++;
			currStep = 0;
			trialInited = false;
			if ( currTrial >= numTrials ) {
				//--- Evaluation ended
				evaluationDone();
			}
		}
	}
}

void MultiTrials::fini() {
	mainFini();
}

} // end namespace farsa
