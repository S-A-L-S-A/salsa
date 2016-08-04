/********************************************************************************
 *  Neural Network Framework.                                                   *
 *  Copyright (C) 2005-2014 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef NNFWCONFIG_H
#define NNFWCONFIG_H

#include "salsaversion.h"

// SALSA_NNFW_TEMPLATE is also for classes that are completely inline
#ifdef WIN32
	#define _CRT_SECURE_NO_DEPRECATE
	#ifdef SALSA_NNFW_BUILDING_DLL
		#define SALSA_NNFW_API __declspec(dllexport)
		#define SALSA_NNFW_TEMPLATE __declspec(dllexport)
	#else
		#define SALSA_NNFW_API __declspec(dllimport)
		#define SALSA_NNFW_TEMPLATE
	#endif
	#define SALSA_NNFW_INTERNAL
#else
	#define SALSA_NNFW_API
	#define SALSA_NNFW_TEMPLATE
	#define SALSA_NNFW_INTERNAL __attribute__ ((visibility ("hidden")))
#endif

#include <QList>
#include <QVector>
#include <QMap>
#include <QString>
#include <QDebug>

#include "Eigen/Core"
namespace salsa {
typedef Eigen::VectorXd DoubleVector;
typedef Eigen::MatrixXd DoubleMatrix;
}

#include "nnfwexceptions.h"

namespace salsa {

class Updatable;
/*! Array of Updatable */
typedef QList<Updatable*> UpdatableList;

class Cluster;
/*! Array of Clusters */
typedef QList<Cluster*> ClusterList;

class Linker;
/*! Array of Linkers */
typedef QList<Linker*> LinkerList;

extern bool SALSA_NNFW_API initNNFWLib();
static const bool nnfwLibInitializer = initNNFWLib();

} // end namespace salsa

#endif
