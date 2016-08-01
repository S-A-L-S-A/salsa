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

#include "core/mutation.h"
#include "core/geneticalgo.h"
#include "configurationparameters.h"
#include "configurationhelper.h"

namespace salsa {

Mutation::Mutation() {
	genomev = 0;
	ga = 0;
	lastGenMutaRatesChange = 0;

	// We put at least one element in the map to avoid crashes
	mutaRates[0] = MutationRate();
}

Mutation::~Mutation() {
	//--- nothing to do
}

void Mutation::setMutationRate( double rate, int start, int length) {
	// This simply calls the other setMutationRate function with equal initial and final rate
	setMutationRate(rate, rate, 0.0, start, length);
}

void Mutation::setMutationRate( double initialRate, double finalRate, double variation, int start, int length) {
	// Create a mutex locker to make this method thread-safe
	QMutexLocker locker(&mutaRatesMutex);

	// First of all we have to change variation sign depending on whether initialMutaRate is
	// greater than finalMutaRate or not
	if (((initialRate > finalRate) && (variation > 0.0)) || ((initialRate < finalRate) && (variation < 0.0))) {
		variation = -variation;
	}

	// Iterate over the map to see what we have to do
	QMap<int, MutationRate>::iterator iter = mutaRates.begin();
	MutationRate current;
	while (iter != mutaRates.end()) {
		if (iter.key() <= start) {
			// Found a value before the start
			current = iter.value();

			iter++;
		} else if ((length <= 0) || (iter.key() <= (start + length))) {
			// Overwrite all the values in the range
			// If length is smaller than one, the range is open-ended and we
			// have to remove all the values after start
			current = iter.value();

			QMap<int, MutationRate>::iterator tmp = iter;
			iter++;
			mutaRates.erase(tmp);
		} else {
			// Passed the end of the range, can stop iterating
			break;
		}
	}

	// Set the new rate at the start
	MutationRate newRate;
	newRate.initial = initialRate;
	newRate.final = finalRate;
	newRate.variation = variation;
	if (ga != NULL) {
		newRate.rateForGeneration(ga->generation());
	} else {
		newRate.mutaRate = initialRate;
	}
	mutaRates[start] = newRate;
	if (length > 0) {
		// If the list has a fixed end, set the value that was there before
		mutaRates[start + length] = current;
	}
}

double Mutation::mutationRate( int bit ) {
	// Create a mutex locker to make this method thread-safe
	QMutexLocker locker(&mutaRatesMutex);

	if ((ga != NULL) && (ga->generation() != lastGenMutaRatesChange)) {
		lastGenMutaRatesChange = ga->generation();
		updateMutationRates();
	}

	return getMutationRateForBit(bit).mutaRate;
}

double Mutation::initialMutationRate( int bit ) {
	return getMutationRateForBit(bit).initial;
}

double Mutation::finalMutationRate( int bit ) {
	return getMutationRateForBit(bit).final;
}

double Mutation::variationMutationRate( int bit ) {
	return getMutationRateForBit(bit).variation;
}

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning IN Mutation: WE SHOULD ALLOW SETTING MUTATION RATES FOR VARIOUS BITS FROM CONFIGURATION FILE
#endif

void Mutation::configure( ConfigurationParameters& params, QString prefix ) {
	double initial_mutation_rate, final_mutation_rate, variation_mutation_rate;

	// The mutation_rate parameter simply sets both initial_mutation_rate and final_mutation_rate
	// to the same value. The value of initial_mutation_rate and final_mutation_rate, however,
	// overwrites the one of mutation_rate
	initial_mutation_rate = final_mutation_rate = ConfigurationHelper::getDouble(params, prefix + "mutation_rate", 0.0);
	initial_mutation_rate = ConfigurationHelper::getDouble(params, prefix + "initial_mutation_rate", initial_mutation_rate);
	final_mutation_rate = ConfigurationHelper::getDouble(params, prefix + "final_mutation_rate", final_mutation_rate);
	variation_mutation_rate = ConfigurationHelper::getDouble(params, prefix + "variation_mutation_rate", 0.01);
	setMutationRate(initial_mutation_rate, final_mutation_rate, variation_mutation_rate);
	if ( mutationRate(0) == 0 ) {
		qWarning( "Setting mutation Rate to ZERO!! Check your config file" );
	}
}

void Mutation::save( ConfigurationParameters& params, QString prefix ) {
	// Here we set only mutation_rate if initial_mutation_rate and final_mutation_rate are equal, otherwise we
	// set both initial_mutation_rate and final_mutation_rate and variation_mutation_rate. Also we only write the
	// mutation rate for the bit 0
	params.createParameter( prefix, QString("type"), "Mutation" );
	if (initialMutationRate(0) == finalMutationRate(0)) {
		params.createParameter( prefix, QString("mutation_rate"), QString("%1").arg(mutationRate(0)) );
	} else {
		params.createParameter( prefix, QString("initial_mutation_rate"), QString("%1").arg(initialMutationRate(0)) );
		params.createParameter( prefix, QString("final_mutation_rate"), QString("%1").arg(finalMutationRate(0)) );
		params.createParameter( prefix, QString("variation_mutation_rate"), QString("%1").arg(variationMutationRate(0)) );
	}
}

void Mutation::describe( QString type ) {
	Descriptor d = addTypeDescription( type, "Mutation operator" );
	d.describeReal( "mutation_rate" ).limits( 0, 1 ).def( 0.05 ).props( IsMandatory ).help( "The probability to apply the mutation operator" );
	d.describeReal( "initial_mutation_rate" ).limits( 0, 1 ).def( 0.05 ).help( "The initial probability to apply the mutation operator; this has to be used in combination with final_mutation_rate and variation_mutation_rate to implement a probability of mutation that vary over generations" );
	d.describeReal( "final_mutation_rate" ).limits( 0, 1 ).def( 0.05 ).help( "The final probability to apply the mutation operator; this has to be used in combination with initial_mutation_rate and variation_mutation_rate to implement a probability of mutation that vary over generations" );
	d.describeReal( "variation_mutation_rate" ).limits( 0, 1 ).def( 0.05 ).help( "The amount of change applied to the current probability to apply the mutation operator in the next generation; this has to be used in combination with initial_mutation_rate and final_mutation_rate to implement a probability of mutation that vary over generations" );
}

void Mutation::updateMutationRates() {
	for (QMap<int, MutationRate>::iterator iter = mutaRates.begin(); iter != mutaRates.end(); iter++) {
		if (ga != NULL) {
			iter.value().rateForGeneration(ga->generation());
		} else {
			iter.value().mutaRate = iter.value().initial;
		}
	}
}

void Mutation::MutationRate::rateForGeneration(unsigned int gen) {
	mutaRate = initial + variation * double(gen);
	if (((initial > final) && (mutaRate < final)) || ((initial < final) && (mutaRate > final))) {
		mutaRate = final;
	}
}

#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning WE SHOULD TRY TO MAKE Mutation::getMutationRateForBit MORE EFFICIENT
#endif

const Mutation::MutationRate& Mutation::getMutationRateForBit( int bit ) const {
	// This is to avoid problems with negative values (which are invalid anyway)
	if (bit < 0) {
		bit = 0;
	}

	// Find the last entry in the map before the given bit
	QMap<int, MutationRate>::const_iterator iter = mutaRates.constBegin();
	for (; (iter != mutaRates.constEnd()) && (iter.key() <= bit); iter++);

	// There is at least one element in the map, so we can safely move backward one step
	iter--;
	return iter.value();
}

} // end namespace salsa
