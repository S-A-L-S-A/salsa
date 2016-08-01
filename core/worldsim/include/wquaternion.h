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

#ifndef WQUATERNION_H
#define WQUATERNION_H

#include "worldsimconfig.h"
#include "wvector.h"

namespace salsa {

class wMatrix;

/*!  wQuaternion class
 *
 *  \
 *  \par Motivation
 *    Quaternion
 *   useful for rotation represenation<br>
 *  \par Description
 *    Description
 *  \par Warnings
 *
 */
class SALSA_WSIM_TEMPLATE wQuaternion {
public:
	/*! constructor */
	wQuaternion();
	/*! constructor */
	wQuaternion( const wMatrix &matrix);
	/*! constructor */
	wQuaternion( real q0, real q1, real q2, real q3 );
	/*! constructor */
	wQuaternion( const wVector &unit_Axis, real Angle = 0.0 );

	void scale( real scale );
	void normalize();
	real dotProduct( const wQuaternion &QB ) const;
	wQuaternion inverse() const;

	wVector rotateVector( const wVector& vector) const;
	wVector unrotateVector( const wVector& vector ) const;

	wVector getEuleroAngles() const;
	wQuaternion slerp( const wQuaternion &q1, real t ) const;
	wVector calcAverageOmega( const wQuaternion &q1, real dt ) const;

	wQuaternion operator*( const wQuaternion &B ) const;
	wQuaternion operator+( const wQuaternion &B ) const;
	wQuaternion operator-( const wQuaternion &B ) const;

	real q0;
	real q1;
	real q2;
	real q3;
};

} // end namespace salsa

//--- this is included here because wMatrix and wQuaternion has mutual dependencies
#include "wmatrix.h"

namespace salsa {

inline wQuaternion::wQuaternion() {
	q0 = 1.0;
	q1 = 0.0;
	q2 = 0.0;
	q3 = 0.0;
}

inline wQuaternion::wQuaternion( const wMatrix &matrix ) {
	enum QUAT_INDEX {
		X_INDEX=0,
		Y_INDEX=1,
		Z_INDEX=2
	};
	static QUAT_INDEX QIndex [] = {Y_INDEX, Z_INDEX, X_INDEX};

	real *ptr;
	real trace;
	QUAT_INDEX i;
	QUAT_INDEX j;
	QUAT_INDEX k;

	trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

	if( trace > 0.0 ) {
		trace = sqrt( trace + 1.0 );
		q0 = 0.5*trace;
		trace = 0.5/trace;
		q1 = (matrix[1][2] - matrix[2][1]) * trace;
		q2 = (matrix[2][0] - matrix[0][2]) * trace;
		q3 = (matrix[0][1] - matrix[1][0]) * trace;
	} else {
		i = X_INDEX;
		if( matrix[Y_INDEX][Y_INDEX] > matrix[X_INDEX][X_INDEX] ) {
			i = Y_INDEX;
		}
		if( matrix[Z_INDEX][Z_INDEX] > matrix[i][i] ) {
			i = Z_INDEX;
		}
		j = QIndex[i];
		k = QIndex[j];

		trace = 1.0 + matrix[i][i] - matrix[j][j] - matrix[k][k];
		trace = sqrt(trace);

		ptr = &q1;
		ptr[i] = 0.5*trace;
		trace = 0.5/trace;
		q0 = (matrix[j][k] - matrix[k][j]) * trace;
		ptr[j] = (matrix[i][j] + matrix[j][i]) * trace;
		ptr[k] = (matrix[i][k] + matrix[k][i]) * trace;
	}
}

inline wQuaternion::wQuaternion(real Q0, real Q1, real Q2, real Q3) {
	q0 = Q0;
	q1 = Q1;
	q2 = Q2;
	q3 = Q3;
	// INVARIANT: fabs( dotProduct(*this) - 1.0) < 1.0e-4f;
}

inline wQuaternion::wQuaternion( const wVector &unitAxis, real angle ) {
	angle *= 0.5;
	q0 = cos( angle );
	real sinAng = sin( angle );

	q1 = unitAxis.x * sinAng;
	q2 = unitAxis.y * sinAng;
	q3 = unitAxis.z * sinAng;
}

inline void wQuaternion::scale(real scale) {
	q0 *= scale;
	q1 *= scale;
	q2 *= scale;
	q3 *= scale;
}

inline void wQuaternion::normalize() {
	scale( 1.0/sqrt( dotProduct(*this)) );
}

inline real wQuaternion::dotProduct( const wQuaternion &QB ) const {
	return q0*QB.q0 + q1*QB.q1 + q2*QB.q2 + q3*QB.q3;
}

inline wQuaternion wQuaternion::inverse() const {
	return wQuaternion(q0, -q1, -q2, -q3);
}

inline wVector wQuaternion::rotateVector( const wVector& vector ) const {
	wMatrix matrix( *this, wVector( 0.0f, 0.0f, 0.0f, 1.0f ) );
	return matrix.rotateVector( vector );
}

inline wVector wQuaternion::unrotateVector( const wVector& vector ) const {
	wMatrix matrix( *this, wVector( 0.0f, 0.0f, 0.0f, 1.0f ) );
	return matrix.unrotateVector( vector );
}

inline wQuaternion wQuaternion::operator+(const wQuaternion &B) const {
	return wQuaternion( q0+B.q0, q1+B.q1, q2+B.q2, q3+B.q3);
}

inline wQuaternion wQuaternion::operator-( const wQuaternion &B ) const {
	return wQuaternion( q0-B.q0, q1-B.q1, q2-B.q2, q3-B.q3);
}

inline wQuaternion wQuaternion::operator*( const wQuaternion &B ) const {
	return wQuaternion( B.q0*q0 - B.q1*q1 - B.q2*q2 - B.q3*q3,
				 		B.q1*q0 + B.q0*q1 - B.q3*q2 + B.q2*q3,
						B.q2*q0 + B.q3*q1 + B.q0*q2 - B.q1*q3,
						B.q3*q0 - B.q2*q1 + B.q1*q2 + B.q0*q3 );
}

inline wVector wQuaternion::getEuleroAngles() const {
	real val;
	real pitch;
	real yaw;
	real roll;

	val = 2.0f*( q2*q0 - q3*q1 );
	if( val >= 0.99995f ) {
		pitch = 2.0f*atan2( q1, q0 );
		yaw = 0.5f*PI_GRECO;
		roll = 0.0f;
	} else if( val <= -0.99995f ) {
		pitch = 2.0f*atan2( q1, q0 );
		yaw = -0.5f*PI_GRECO;
		roll = 0.0f;
	} else {
		yaw = sin (val);
		pitch = atan2( 2.0f*( q1*q0 + q3*q2 ), 1.0f-2.0f*( q1*q1 + q2*q2) );
		roll = atan2( 2.0f*( q3*q0 + q1*q2 ), 1.0f-2.0f*( q2*q2 + q3*q3) );
	}
	return wVector(pitch, yaw, roll);
}

inline wVector wQuaternion::calcAverageOmega( const wQuaternion &QB, real dt ) const {
	wQuaternion dq( inverse()*QB );
	wVector omegaDir( dq.q1, dq.q2, dq.q3 );
	real dirMag2 = omegaDir % omegaDir;
	if( dirMag2 < 1.0e-10 ) {
		return wVector( 0.0, 0.0, 0.0, 0.0 );
	}
	real dirMagInv = 1.0/sqrt( dirMag2 );
	real dirMag = dirMag2 * dirMagInv;
	real omegaMag = 2.0*atan2( dirMag, dq.q0 )/dt;

	return omegaDir.scale( dirMagInv * omegaMag );
}

inline wQuaternion wQuaternion::slerp( const wQuaternion &QB, real t ) const {
	real dot;
	real ang;
	real Sclp;
	real Sclq;
	real den;
	real sinAng;
	wQuaternion Q;

	dot = dotProduct( QB );

	if( (dot+1.0) > 1.0e-5f ) {
		if( dot < 0.995f ) {
			ang = cos( dot );
			sinAng = sin( ang );
			den = 1.0/sinAng;
			Sclp = sin( (1.0-t) * ang ) * den;
			Sclq = sin( t*ang ) * den;
		} else  {
			Sclp = 1.0-t;
			Sclq = t;
		}
		Q.q0 = q0*Sclp + QB.q0*Sclq;
		Q.q1 = q1*Sclp + QB.q1*Sclq;
		Q.q2 = q2*Sclp + QB.q2*Sclq;
		Q.q3 = q3*Sclp + QB.q3*Sclq;
	} else {
		Q.q0 =  q3;
		Q.q1 = -q2;
		Q.q2 =  q1;
		Q.q3 =  q0;

		Sclp = sin(1.0-t) * PI_GRECO * 0.5;
		Sclq = sin( t * PI_GRECO * 0.5 );

		Q.q0 = q0*Sclp + Q.q0*Sclq;
		Q.q1 = q1*Sclp + Q.q1*Sclq;
		Q.q2 = q2*Sclp + Q.q2*Sclq;
		Q.q3 = q3*Sclp + Q.q3*Sclq;
	}

	dot = Q.dotProduct( Q );
	if( dot < (1.0-1.0e-4f) ) {
		dot = 1.0/sqrt( dot );
		Q.q0 *= dot;
		Q.q1 *= dot;
		Q.q2 *= dot;
		Q.q3 *= dot;
	}
	return Q;
}

} // end namespace salsa

#endif
