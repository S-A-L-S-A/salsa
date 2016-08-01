/********************************************************************************
 *  SALSA                                                                       *
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

#ifndef WORLDSIMUTILS_H
#define WORLDSIMUTILS_H

#include "worldsimconfig.h"
#include "wvector.h"
#include "wmatrix.h"

namespace salsa {

/**
 * \brief Merges two AABBs
 *
 * \param minPointA the min point of the first AABB. This is modified to return
 *                  the min point of the merged AABB
 * \param maxPointA the max point of the first AABB. This is modified to return
 *                  the max point of the merged AABB
 * \param minPointB the min point of the second AABB
 * \param maxPointB the max point of the second AABB
 */
void mergeAABBs(wVector& minPointA, wVector& maxPointA, const wVector& minPointB, const wVector& maxPointB);

/**
 * \brief Computes the AABB of a planar ellipse with any orientation in 3D space
 *
 * This function computes the AABB of a planar ellipse. The ellipse center is in
 * the origin of the frame of reference and the two vectors u and v are the
 * vectors for the two axes (both direction and length). You must make sure the
 * two vectors are perpendicular, otherwise results could be wrong.
 * \param u the first axis of the ellipsoid (direction and length). This MUST be
 *          perpendicular to v
 * \param v the second axis of the ellipsoid (direction and length). This MUST
 *          be perpendicular to u
 * \param minPoint the min point of the computed AABB
 * \param maxPoint the max point of the computed AABB
 */
void computeEllipseAABB(const wVector& u, const wVector& v, wVector& minPoint, wVector& maxPoint);

}

#endif
