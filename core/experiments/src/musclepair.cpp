// /********************************************************************************
//  *  FARSA Experimentes Library                                                  *
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
// #include "musclepair.h"
// #include "phyjoint.h"
//
// /*
// dopo aver inizializzato con i valori di Gianluca
// 1)setActivation()
// 2)aplly ritorna la velocità da applicare al giunto
//
// */
//
// namespace farsa {
//
// MusclePair::MusclePair( PhyDOF* dof, float ksh, float tmax, float rl, float lmax ) {
// 	//vedere parametri di Gianluca
// 	this->dof = dof;
//     this->ksh = ksh;
// 	reduceKsh = 1.0;
//     this->tmax = tmax;
//     this->rl = rl;
//     this->lmax = lmax;
//     lmin = 2.0f*rl - lmax;
//     this->ash = (rl*rl) / ( (lmax-rl)*(lmax-rl) );
//     this->b = 1.0f;
//     getExtension( last_x1, last_x2 );
//     this->dx1 = 0.0f;
//     this->dx2 = 0.0f;
// }
//
// void MusclePair::setRestLength( float rl ) {
//     this->rl = rl;
//     lmin = 2.0f*rl - lmax;
//     ash = (rl*rl) / ( (lmax-rl)*(lmax-rl) );
// }
//
// float MusclePair::getRestLength() {
//     return rl;
// }
//
// void MusclePair::setMaxTension( float tmax ) {
//     this->tmax = tmax;
// }
//
// float MusclePair::getMaxTension( ) {
//     return tmax;
// }
//
// void MusclePair::setMaxLength( float lmax ) {
//     this->lmax = lmax;
//     lmin = 2.0f*rl - lmax;
//     ash = (rl*rl) / ( (lmax-rl)*(lmax-rl) );
// }
//
// float MusclePair::getMaxLength() {
//     return lmax;
// }
//
// float MusclePair::getMinLength() {
//     return lmin;
// }
//
// float MusclePair::getAsh( ) {
//     return ash;
// }
//
// void MusclePair::setAsh( float newash ) {
//     ash = newash;
// }
//
// void MusclePair::setKsh( float ksh, float reduceFactor ) {
//     this->ksh = ksh;
// 	reduceKsh = reduceFactor;
// }
//
// float MusclePair::getKsh( ) {
//     return ksh;
// }
//
// void MusclePair::setViscosity( float b ) {
//     this->b = b;
// }
//
// float MusclePair::getViscosity( ) {
//     return b;
// }
//
// void MusclePair::setActivation( float act1, float act2 ) {
// 	//da fare prima di apply
// 	//act1 e act2 devono andare da 0 a 1
//     this->act1 = act1;
//     this->act2 = act2;
// }
//
// void MusclePair::getActivation( float& act1, float& act2 ) {
//     act1 = this->act1;
//     act2 = this->act2;
// }
//
// void MusclePair::getExtension( float& l1, float& l2 ) {
//     float angle = dof->position();
//     real lowLim, higLim;
//     dof->limits( lowLim, higLim );
//     l1 = linearMap( angle, lowLim, higLim, lmin, lmax );
//     l2 = invLinearMap( angle, lowLim, higLim, lmin, lmax );
// }
//
// float MusclePair::apply() {
//     float x1, x2;
//     getExtension( x1, x2 );
//     // Calculate the current velocity of shortening/lengthing of muscles
//     float timestep = dof->joint()->world()->timeStep();
//     dx1 = ( x1 - last_x1 ) / timestep;
//     dx2 = ( x2 - last_x2 ) / timestep;
//
//     float ta1 = act1 * ( -ash*tmax*(x1-rl)*(x1-rl)/(rl*rl) + tmax );
//     float tp1 = reduceKsh*0.5*tmax * ( exp( ksh*(x1-rl)/(lmax-rl) ) - 1.0f ) / ( exp( ksh ) - 1.0f );
//     float tv1 = b * dx1;
//
//     float ta2 = act2 * ( -ash*tmax*(x2-rl)*(x2-rl)/(rl*rl) + tmax );
//     float tp2 = reduceKsh*0.5*tmax * ( exp( ksh*(x2-rl)/(lmax-rl) ) - 1.0f ) / ( exp( ksh ) - 1.0f );
//     float tv2 = b * dx2;
//
// 	tal1 = ta1;
// 	tal2 = ta2;
//
//     last_x1 = x1;
//     last_x2 = x2;
//
// 	return -(ta1+tp1+tv1) + (ta2+tp2+tv2);
// }
//
// } // end namespace farsa
