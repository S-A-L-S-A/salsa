/*******************************************************************************
 * FARSA New Genetic Algorithm Library                                        *
 * Copyright (C) 2007-2012                                                    *
 * Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                  *
 * Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                        *
 * Paolo Pagliuca <paolo.pagliuca@istc.cnr.it>                                *
 * Jonata Tyska Carvalho <jonata.carvalho@istc.cnr.it>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA *
 ******************************************************************************/

#ifndef EXPERIMENTOUTPUT_H
#define EXPERIMENTOUTPUT_H

#include "newgaconfig.h"

namespace farsa {

/**
 * \brief The interface for output data from an experiment
 *
 * An experiment should in general inherit both from this and ExperimentInput
 * (to provide functions to set the input and to get the output of the
 * experiment)
 */
class FARSA_NEWGA_TEMPLATE ExperimentOutput
{
public:
	/**
	 * \brief Destructor
	 *
	 * This is here just to declare it virtual
	 */
	virtual ~ExperimentOutput()
	{
	}
};

} // end namespace farsa

#endif
