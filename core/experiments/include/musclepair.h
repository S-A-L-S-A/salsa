// /********************************************************************************
//  *  SALSA Experimentes Library                                                  *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
//  *                                                                              *
//  *  This program is free software; you can redistribute it and/or modify        *
//  *  it under the terms of the GNU General Public License as published by        *
//  *  the Free Software Foundation; either version 2 of the License, or           *
//  *  (at your option) any later version.                                         *
//  *                                                                              *
//  *  This program is distributed in the hope that it will be useful,             *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
//  *  GNU General Public License for more details.                                *
//  *                                                                              *
//  *  You should have received a copy of the GNU General Public License           *
//  *  along with this program; if not, write to the Free Software                 *
//  *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
//  ********************************************************************************/
//
// #ifndef MUSCLEPAIR_H
// #define MUSCLEPAIR_H
//
// #include "experimentsconfig.h"
//
// namespace salsa {
//
// class PhyDOF;
//
// /*!  MusclePair class
//  *
//  *  \par Motivation
//  *    A MusclePair emulate the muscle dynamics.<br>
//  *  \par Description
//  *  \par Warnings
//  *    Warnings
//  */
// class SALSA_EXPERIMENTS_API MusclePair {
// public:
// 	/*! construct a MusclePair
// 	 *  The two muscles are identical and they generate opposite torques
// 	 *  \param dof the joint where the muscles are attached
// 	 *  \param ksh vedi implementazione_muscolo.lyx
// 	 *  \param tmax vedi implementazione_muscolo.lyx
// 	 *  \param rl vedi implementazione_muscolo.lyx
// 	 *  \param lmax vedi implementazione_muscolo.lyx
// 	 *  ash is automatically determineted by equation described in implementazione_muscolo.lyx
// 	 */
// 	MusclePair( PhyDOF* dof, float ksh, float tmax, float rl, float lmax );
// 	/*! Destructor */
// 	virtual ~MusclePair() { /*Nothing to do*/ };
// 	/*!  Set the resting lenght of muscles */
// 	void setRestLength( float rl );
// 	/*!  Return the resting lenght of muscles */
// 	float getRestLength();
// 	/*!  Set the max tension */
// 	void setMaxTension( float tmax );
//     /*!  Return the max tension */
//     float getMaxTension( );
//     /*!  Set the maximum length */
//     void setMaxLength( float lmax );
//     /*!  Return the maximum length */
//     float getMaxLength();
//     /*!  Return the minimum length
//      *  Automatically calculated by other parameters
//      */
//     float getMinLength();
//     /*!  Return the A_sh (shape factor)
//      *  Automatically calculated by other parameters
//      */
//     float getAsh( );
// 	/*! override the automatically Ash parameter
// 	 *  \warning this value will be recalculated every time other properties will be changed */
// 	void setAsh( float newash );
//     /*!  Set the K_sh (passive shape factor)
// 	 * \param reduceFactor scale the passive curve determined by Ksh by this factor
//      */
//     void setKsh( float ksh, float reduceFactor = 1.0 );
//     /*!  Return the K_sh (shape factor) */
//     float getKsh( );
//     /*!  Set the Viscosity */
//     void setViscosity( float b );
//     /*!  Return the Viscosity */
//     float getViscosity( );
//     /*!  Set the activation of muscles */
//     void setActivation( float act1, float act2 );
//     /*!  Return the activation of muscles */
//     void getActivation( float& act1, float& act2 );
//     /*!  Return the current extension of muscles */
//     void getExtension( float& l1, float& l2 );
//     /*!  Return the current extension of muscles */
// 	void getActiveForces( float& ta1, float& ta2 ) {
// 		ta1 = tal1;
// 		ta2 = tal2;
// 	};
// 	/*! return the Torque the muscles would apply
// 	 *  calculating the Tension forces according with Hill muscle model
// 	 */
// 	float apply();
// private:
// 	/*! DOF where muscles are attached */
// 	PhyDOF* dof;
// 	/*! Active Shape Factor */
// 	float ash;
// 	/*! Passive Shape Factor */
// 	float ksh;
// 	/*! Passive scale factor */
// 	float reduceKsh;
// 	/*! viscosity of muscles */
// 	float b;
// 	/*! Max Torque */
// 	float tmax;
// 	/*! Resting Lenght */
// 	float rl;
// 	/*! Max Lenght */
// 	float lmax;
// 	/*! Minimum Lenght (automatically calculated by setMaxLenght, and setRestLenght) */
// 	float lmin;
// 	/*! Activation of muscle AGONISTA */
// 	float act1;
// 	/*! Activation of muscle ANTAGONISTA */
// 	float act2;
// 	/*! Extension calculated at the last calling of apply() */
// 	float last_x1;
// 	/*! Extension calculated at the last calling of apply() */
// 	float last_x2;
// 	/*! Velocity of length variations */
// 	float dx1;
// 	/*! Velocity of length variations */
// 	float dx2;
// 	/*! last active forces */
// 	float tal1, tal2;
// };
//
// } //end namespace salsa
//
// #endif
//
