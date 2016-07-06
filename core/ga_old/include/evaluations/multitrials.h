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

#ifndef MULTITRIALS_H
#define MULTITRIALS_H

#include "gaconfig.h"
#include "core/evaluation.h"

namespace farsa {

/*!  \brief MultiTrials is a class that evaluate in more than one trial
 *
 *  \par Description
 *    MultiTrials allow an easy way to implements fitnesses calculation based on more than one trials
 *    where each of them is composed of a maximum number of steps
 *  \par Warnings
 *
 * \ingroup ga_eval
 */
class FARSA_GA_API MultiTrials : public Evaluation {
public:
	/*! Default Constructor */
	MultiTrials( int steps = 1, int trials = 1 );
	/*! Destructor */
	virtual ~MultiTrials();
	/*! set the numbers of Trial to do */
	void setTrials( int t );
	/*! return the numbers of trials */
	int trials() const;
	/*! return the current trial */
	int currentTrial() const;
	/*! set the numbers of Step to do */
	void setSteps( int s );
	/*! return the numbers of Step */
	int steps() const;
	/*! return the current step */
	int currentStep() const;

	/*! Configure the object using the ConfigurationParameters specified
	 * \param params the object with configuration parameters
	 * \param prefix the group in which parameters are expected to be (this
	 *               is terminated by a separator character). We expect to
	 *               receive as prefix the group with evaluation parameters
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

	/*! Create an exact copy of MultiTrials */
	virtual MultiTrials* clone() const = 0;

protected:
	/*! Called at the start of Evaluation Process */
	virtual void mainInit() = 0;
	/*! Called for initialize the i-th trials
	 *  \param trial is the counter of trial to initialize (it starts from zero)
	 *  \return the implementation should return true when initialisation has been done, otherwise
	 *     the method will be called until it returns false; this is helpful for initialisation
	 *     that need more that one step to be done
	 */
	virtual bool trialInit( int trial ) = 0;
	/*! Called at each step of the evaluation process
	 *  \param step is the counter of step done (it starts from zero)
	 *  \param trial is the counter of trial to initialize (it starts from zero)
	 */
	virtual void trialStep( int step, int trial ) = 0;
	/*! Called when Trial is finished */
	virtual void trialFini( int trial ) = 0;
	/*! Called at the end of all trials for finalization */
	virtual void mainFini() = 0;

	/*! This method tell to MultiTrials that the current trial is ended. <br>
	 *  Called before reaching of the maximum steps allowed (setSteps) means
	 *  a premature interruption of Trial for some reasons (task achieved, penalty, etc)
	 */
	void trialDone();

	/*! Sets whether we should stop incrementing the current step counter or not.<br>
	 * If remain is true, the current step counter stops incrementing. This means that
	 * the trialStep function will be called at each step with the same step and trial
	 * parameters. Call with remain set to false to resume the current step counter
	 * increment at each step.
	 * \param remain if true stops incrementing the current step counter
	 */
	void remainInCurrentStep(bool remain);

	/*! Returns true if we are not incrementing the current step counter<br>
	 * See remainInCurrentStep() for more details
	 * \return true if we are not incrementing the current step counter
	 */
	bool remainingInCurrentStep() const;

private:
	/*! Don't re-implement these method; use mainInit, trialInit, trialStep, trialFini, mainFini instead */
	void init();
	/*! Don't re-implement these method; use mainInit, trialInit, trialStep, trialFini, mainFini instead */
	void step();
	/*! Don't re-implement these method; use mainInit, trialInit, trialStep, trialFini, mainFini instead */
	void fini();

	/*! current step */
	int currStep;
	/*! current trial */
	int currTrial;
	/*! num of trials */
	int numTrials;
	/*! num of steps */
	int numSteps;
	/*! if true steps are incremented at each call to step(), otherwise currStep is not incremented */
	bool incrementStep;
	/*! true indicate that the trial has been initialized, false not initialized */
	bool trialInited;
};

} // end namespace farsa

#endif
