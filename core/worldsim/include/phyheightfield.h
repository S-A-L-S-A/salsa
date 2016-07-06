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

// This is not converted to FARSA2!!!
// #ifndef PHYHEIGHTFIELD_H
// #define PHYHEIGHTFIELD_H
//
// #include "phyobject.h"
// class NewtonMesh;
//
// namespace farsa {
//
// /*!  PhyHeightField class
//  */
// class FARSA_WSIM_API PhyHeightField : public PhyObject {
// 	Q_OBJECT
// public:
// 	/*!  Create a static height field object in the physic world
// 	 *  \param side_x dimension of the side aligned with X axis
// 	 *  \param side_y dimension of the side aligned with Y axis
// 	 *  \param heights vector of Z elevations at the coordinate X,Y speficied
// 	 *  \param world The World which object will be inserted
// 	 *  \param tm rotation and position at the moment of creation
// 	 */
// 	PhyHeightField( real side_x, real side_y, World* world, QString name="unamed", const wMatrix& tm = wMatrix::identity() );
// 	/*! Destroy this object */
// 	virtual ~PhyHeightField();
// 	/*! return the side X */
// 	real sideX() const {
// 		return sidex;
// 	};
// 	/*! return the side Y */
// 	real sideY() const {
// 		return sidey;
// 	};
// 	/*! return the Z elevation at the point specified inside the height field */
// 	real elevationAt( real x, real y ) const {
// 		return 0.0f;
// 	};
// 	NewtonMesh* mesh;
// private:
// 	real sidex;
// 	real sidey;
// protected:
// 	/*! change the matrix of the phyheightfield rotating it */
// 	virtual void changedMatrix();
// 	/*! Engine encapsulation */
// 	void createPrivateObject();
// };
//
// } // end namespace farsa
//
// #endif
