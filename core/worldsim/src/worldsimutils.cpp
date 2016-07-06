/********************************************************************************
 *  FARSA                                                                       *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#include "worldsimutils.h"
#include <cmath>

namespace farsa {

void mergeAABBs(wVector& minPointA, wVector& maxPointA, const wVector& minPointB, const wVector& maxPointB)
{
	minPointA[0] = min(minPointA[0], minPointB[0]);
	minPointA[1] = min(minPointA[1], minPointB[1]);
	minPointA[2] = min(minPointA[2], minPointB[2]);
	maxPointA[0] = max(maxPointA[0], maxPointB[0]);
	maxPointA[1] = max(maxPointA[1], maxPointB[1]);
	maxPointA[2] = max(maxPointA[2], maxPointB[2]);
}

void computeEllipseAABB(const wVector& u, const wVector& v, wVector& minPoint, wVector& maxPoint)
{
	// To compute the bounding box of an ellipse we follow instructions at
	// http://www.iquilezles.org/www/articles/ellipses/ellipses.htm
	// Documentation is also in the farsa repository under miscDocumentation/Bounding Box Ellipse (by extension cylinder and cone)

	minPoint.x = -sqrt(u.x * u.x + v.x * v.x);
	minPoint.y = -sqrt(u.y * u.y + v.y * v.y);
	minPoint.z = -sqrt(u.z * u.z + v.z * v.z);
	maxPoint.x = sqrt(u.x * u.x + v.x * v.x);
	maxPoint.y = sqrt(u.y * u.y + v.y * v.y);
	maxPoint.z = sqrt(u.z * u.z + v.z * v.z);
}

} // end namespace farsa
