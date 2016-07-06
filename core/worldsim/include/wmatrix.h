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

#ifndef WMATRIX_H
#define WMATRIX_H

#include "worldsimconfig.h"
#include "wvector.h"
#include <QString>
#include <cstring>
#include "mathutils.h"

#ifdef FARSA_USE_GSL
	#include <gsl/gsl_blas.h>
#endif

namespace farsa {

class wQuaternion;

/*!  wMatrix class
 *
 *  \
 *  \par Motivation
 *    4x4 Matrix of real numbers;
 *   useful for transformation matrix<br>
 *  \par Description
 *    Description
 *  \par Warnings
 *
 */
class FARSA_WSIM_TEMPLATE wMatrix {
public:
	/*! Construct an unintialized matrix */
	wMatrix();
	/*! Construct the matrix
	 *  \param X x axis representation
	 *  \param Y y axis representation
	 *  \param Z z axis representation
	 *  \param P world position
	 */
	wMatrix( const wVector &X, const wVector &Y, const wVector &Z, const wVector &P );
	/*! Constructor
	 *  \param rotation quaternion representation of rotation part
	 *  \param position world position
	 */
	wMatrix( const wQuaternion &rotation, const wVector &position );
	/*! Constructor
	 *	\param position a vector containg three position value
	 *  \param rotation a vector containing 12 values representing a 3x4 rotation matrix
	 *  useful for converting data from ODE to wMatrix
	 */
	wMatrix( const real* position, const real* rotation );

	/*! Returns a string representation of the matrix */
	operator QString() const;

	/*! Copy constructor */
	wMatrix( const wMatrix &other );
	/*! Copy operator */
	wMatrix& operator=( const wMatrix &other );

	/*! indexing operator */
	wVectorT<true>& operator[](int i);
	/*! indexing operator (const version) */
	const wVectorT<true>& operator[](int i) const;

	/*! calculate the inverse of transformation matrix */
	wMatrix inverse() const;
	/*! transpose the rotation sub-matrix */
	wMatrix transpose() const;
	/*! full transposition of the transformation matrix */
	wMatrix transpose4X4() const;
	/*! return the Eulero angle of rotation */
	wVector getEuleroAngles() const;
	/*! Rotate this matrix around the axis, and position passed of radians specified
	 *  \param axis of rotation in world coordinate
	 *  \param centre of rotation in world coordinate
	 *  \param angle of rotation in radians
	 */
	wMatrix rotateAround( const wVector& axis, const wVector& centre, real angle ) const;
	/*! rotate the matrix passed by rotation specified by this and return a copy */
	wMatrix rotateMatrix( const wMatrix& tm ) const;
	/*! rotate the vector v, it doesn't apply position transformation */
	wVector rotateVector(const wVector &v) const;
	/*! inverte the rotation of this matrix on the vector v */
	wVector unrotateVector(const wVector &v) const;
	/*! apply both rotation and translation to the vector v */
	wVector transformVector(const wVector &v) const;
	/*! invert the rotation and translation on the vector v */
	wVector untransformVector(const wVector &v) const;
	/*! ... doc missing ... */
	wVector transformPlane(const wVector &localPlane) const;
	/*! ... doc missing ... */
	wVector untransformPlane(const wVector &globalPlane) const;
	/*! apply transformation to a chunck of vectors
	 *  \param dst where to store transformed vectors
	 *  \param src where to get vectors to transform
	 *  \param count how many vector you want to transform
	 */
	void transformTriplex( real* dst, int dstStrideInBytes, real* src, int srcStrideInBytes, int count) const;

	/*! matrix multiplication */
	wMatrix operator*( const wMatrix &B ) const;

	/*! return true if the transformation matrix is valid */
	bool sanityCheck();
	/*! change the matrix in order to get a valid one that represent the same rotation/translation */
	void sanitifize();

	/*! create an identity matrix */
	static wMatrix identity();
	/*! create a zero matrix */
	static wMatrix zero();
	/*! calculate an orthonormal matrix with the Z vector pointing on the
	 *  dir direction, and the Y and Z are determined by using the Gramm-Schmidth procedure
	 */
	static wMatrix grammSchmidt(const wVector& dir);
	/*! create a rotation around X axis of ang radians */
	static wMatrix pitch( real ang );
	/*! create a rotation around Y axis of ang radians */
	static wMatrix yaw( real ang );
	/*! create a rotation around Z axis of ang radians */
	static wMatrix roll( real ang );

	wVectorT<true> x_ax;
	wVectorT<true> y_ax;
	wVectorT<true> z_ax;
	wVectorT<true> w_pos;

private:
	real data[16];
	wVectorT<true>* vecs[4]; // This is only useful for operator[] functions
};

} // end namespace farsa

//--- this is included here because wMatrix and wQuaternion have mutual dependencies
#include "wquaternion.h"

namespace farsa {

inline wMatrix::wMatrix() : x_ax(&data[0]), y_ax(&data[4]), z_ax(&data[8]), w_pos(&data[12]) {
	vecs[0] = &x_ax;
	vecs[1] = &y_ax;
	vecs[2] = &z_ax;
	vecs[3] = &w_pos;
}

inline wMatrix::wMatrix ( const wVector &X, const wVector &Y, const wVector &Z, const wVector &P) : x_ax(&data[0]), y_ax(&data[4]), z_ax(&data[8]), w_pos(&data[12])  {
	vecs[0] = &x_ax;
	vecs[1] = &y_ax;
	vecs[2] = &z_ax;
	vecs[3] = &w_pos;
	x_ax = X;
	y_ax = Y;
	z_ax = Z;
	w_pos = P;
}

inline wMatrix::wMatrix( const wQuaternion &rotation, const wVector &position ) : x_ax(&data[0]), y_ax(&data[4]), z_ax(&data[8]), w_pos(&data[12])  {
	vecs[0] = &x_ax;
	vecs[1] = &y_ax;
	vecs[2] = &z_ax;
	vecs[3] = &w_pos;
	real x2;
	real y2;
	real z2;
// 	real w2;
	real xy;
	real xz;
	real xw;
	real yz;
	real yw;
	real zw;

// 	w2 = 2.0 * rotation.q0 * rotation.q0; COMMENTED OUT BECAUSE IT IS UNUSED
	x2 = 2.0 * rotation.q1 * rotation.q1;
	y2 = 2.0 * rotation.q2 * rotation.q2;
	z2 = 2.0 * rotation.q3 * rotation.q3;
	// INVARIANT: fabs(w2 + x2 + y2 + z2 - 2.0) < 1.5e-3f;

	xy = 2.0 * rotation.q1 * rotation.q2;
	xz = 2.0 * rotation.q1 * rotation.q3;
	xw = 2.0 * rotation.q1 * rotation.q0;
	yz = 2.0 * rotation.q2 * rotation.q3;
	yw = 2.0 * rotation.q2 * rotation.q0;
	zw = 2.0 * rotation.q3 * rotation.q0;

	x_ax = wVector( 1.0-y2-z2, xy+zw, xz-yw, 0.0 );
	y_ax = wVector( xy-zw, 1.0-x2-z2, yz+xw, 0.0 );
	z_ax = wVector( xz+yw, yz-xw, 1.0-x2-y2, 0.0 );

	w_pos.x = position.x;
	w_pos.y = position.y;
	w_pos.z = position.z;
	w_pos.w = 1.0;
}

inline wMatrix::wMatrix( const real* pos, const real* rot ) : x_ax(&data[0]), y_ax(&data[4]), z_ax(&data[8]), w_pos(&data[12])  {
	vecs[0] = &x_ax;
	vecs[1] = &y_ax;
	vecs[2] = &z_ax;
	vecs[3] = &w_pos;
	 x_ax[0]=rot[0];  x_ax[1]=rot[1];  x_ax[2]=rot[2];  x_ax[3]=0.0 /*rot[3]*/;
	 y_ax[0]=rot[4];  y_ax[1]=rot[5];  y_ax[2]=rot[6];  y_ax[3]=0.0 /*rot[7]*/;
	 z_ax[0]=rot[8];  z_ax[1]=rot[9];  z_ax[2]=rot[10]; z_ax[3]=0.0 /*rot[11]*/;
	w_pos[0]=pos[0]; w_pos[1]=pos[1]; w_pos[2]=pos[2]; w_pos[3]=1.0 /*pos[3*/;
}

inline wMatrix::operator QString() const {
	return QString("[%1, %2, %3, %4]").arg(x_ax).arg(y_ax).arg(z_ax).arg(w_pos);
}

inline wMatrix::wMatrix( const wMatrix &other ) : x_ax(&data[0]), y_ax(&data[4]), z_ax(&data[8]), w_pos(&data[12]) {
	vecs[0] = &x_ax;
	vecs[1] = &y_ax;
	vecs[2] = &z_ax;
	vecs[3] = &w_pos;
	memcpy(data, other.data, 16 * sizeof(real));
}

inline wMatrix& wMatrix::operator=( const wMatrix &other )
{
	if (&other == this) {
		return *this;
	}

	memcpy(data, other.data, 16 * sizeof(real));

	return *this;
}

inline wVectorT<true>& wMatrix::operator[](int i) {
	return *(vecs[i]);
}

inline const wVectorT<true>& wMatrix::operator[](int i) const {
	return *(vecs[i]);
}

inline wMatrix wMatrix::inverse() const {
	return wMatrix( wVector(x_ax.x, y_ax.x, z_ax.x, 0.0f),
					wVector(x_ax.y, y_ax.y, z_ax.y, 0.0f),
		            wVector(x_ax.z, y_ax.z, z_ax.z, 0.0f),
		            wVector(- (w_pos % x_ax), - (w_pos % y_ax), - (w_pos % z_ax), 1.0f));
}

inline wMatrix wMatrix::transpose() const {
	return wMatrix( wVector(x_ax.x, y_ax.x, z_ax.x, 0.0f),
					wVector(x_ax.y, y_ax.y, z_ax.y, 0.0f),
					wVector(x_ax.z, y_ax.z, z_ax.z, 0.0f),
					wVector(0.0f, 0.0f, 0.0f, 1.0f));
}

inline wMatrix wMatrix::transpose4X4() const {
	return wMatrix( wVector(x_ax.x, y_ax.x, z_ax.x, w_pos.x),
					wVector(x_ax.y, y_ax.y, z_ax.y, w_pos.y),
					wVector(x_ax.z, y_ax.z, z_ax.z, w_pos.z),
					wVector(x_ax.w, y_ax.w, z_ax.w, w_pos.w));
}

inline wVector wMatrix::getEuleroAngles() const {
	real yaw;
	real roll = 0.0;
	real pitch = 0.0;
	const real minSin = 0.99995f;
	const wMatrix& matrix = *this;

	yaw = asin( -min( max( matrix[0][2], -0.999999 ), 0.999999) );
	if( matrix[0][2] < minSin ) {
		if( matrix[0][2] > -minSin ) {
			roll = atan2( matrix[0][1], matrix[0][0] );
			pitch = atan2( matrix[1][2], matrix[2][2] );
		} else {
			pitch = atan2( matrix[1][0], matrix[1][1] );
		}
	} else {
		pitch = -atan2( matrix[1][0], matrix[1][1] );
	}

	return wVector(pitch, yaw, roll, 0.0);
}

inline wMatrix wMatrix::rotateAround( const wVector& axis, const wVector& centre, real angle ) const {
	//--- OPTIMIZE_ME this routine is very inefficient. Please Optimize Me
/*	wMatrix ret = *this;
	wQuaternion ql( axis, angle );
	wMatrix rot = wMatrix( ql, wVector(0,0,0) );
	ret = rot.rotateMatrix( ret );
	wVector c = w_pos - centre; //centre-w_pos;
	qDebug() << c[0] << c[1] << c[2];
	//ret.w_pos += rot.rotateVector( c ) - c;
	ret.w_pos = centre + rot.rotateVector( c );
	return ret;*/

	/*
	wMatrix ret = *this;
	wVector centrel = ret.w_pos - centre; //centre - ret.w_pos;
	ret.w_pos = centrel;
	wQuaternion ql( axis, angle );
	ret = ret * wMatrix( ql, wVector(0,0,0) );
	ret.w_pos += w_pos - centrel;
	return ret;
	*/

	// --- using rotateAround of wVector
	wMatrix ret = *this;
	ret.x_ax.rotateAround( axis, angle );
	ret.y_ax.rotateAround( axis, angle );
	ret.z_ax.rotateAround( axis, angle );
	wVector diffCentre = ret.w_pos - centre;
	diffCentre.rotateAround( axis, angle );
	ret.w_pos = diffCentre + centre;
	return ret;
}

inline wMatrix wMatrix::rotateMatrix( const wMatrix& B ) const {
	const wMatrix& A = *this;
	return wMatrix (wVector (A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0],
							 A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1],
							 A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2],
	                         A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3]),
					wVector (A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0],
						     A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1],
							 A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2],
							 A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3]),
					wVector (A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0],
							 A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1],
							 A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2],
							 A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3]),
					wVector ( A[3][3] * B[3][0],
							  A[3][3] * B[3][1],
							  A[3][3] * B[3][2],
							  A[3][3] * B[3][3]));
}

inline wVector wMatrix::rotateVector(const wVector &v) const {
	return wVector(  v.x * x_ax.x + v.y * y_ax.x + v.z * z_ax.x,
					 v.x * x_ax.y + v.y * y_ax.y + v.z * z_ax.y,
					 v.x * x_ax.z + v.y * y_ax.z + v.z * z_ax.z);
}

inline wVector wMatrix::unrotateVector(const wVector &v) const {
	return wVector( v%x_ax, v%y_ax, v%z_ax );
}

inline wVector wMatrix::transformVector(const wVector &v) const {
	return w_pos + rotateVector(v);
}

inline wVector wMatrix::untransformVector(const wVector &v) const {
	return unrotateVector( v-w_pos );
}

inline void wMatrix::transformTriplex( real* dst, int dstStrideInBytes, real* src, int srcStrideInBytes, int count ) const {
	real x;
	real y;
	real z;

	dstStrideInBytes /= sizeof(real);
	srcStrideInBytes /= sizeof(real);

	for( int i=0; i<count; i++ ) {
		x = src[0];
		y = src[1];
		z = src[2];
		dst[0] = x*x_ax.x + y*y_ax.x + z*z_ax.x + w_pos.x;
		dst[1] = x*x_ax.y + y*y_ax.y + z*z_ax.y + w_pos.y;
		dst[2] = x*x_ax.z + y*y_ax.z + z*z_ax.z + w_pos.z;
		dst += dstStrideInBytes;
		src += srcStrideInBytes;
	}
}

inline wMatrix wMatrix::operator*( const wMatrix &B ) const {
	const wMatrix& A = *this;
#ifdef FARSA_USE_GSL
	wMatrix res;
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0, A.data, 4, B.data, 4, 0.0, res.data, 4);
	//cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 4, 4, 4, 1.0, A.data, 4, B.data, 4, 0.0, res.data, 4);
	return res;
#else
	return wMatrix (wVector (A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0],
							 A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1],
							 A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2],
	                         A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3]),
					wVector (A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0],
						     A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1],
							 A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2],
							 A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3]),
					wVector (A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0],
							 A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1],
							 A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2],
							 A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3]),
					wVector (A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0],
							 A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1],
							 A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2],
							 A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3]));
#endif
}

inline wVector wMatrix::transformPlane( const wVector &localPlane ) const {
	wVector tmp( rotateVector(localPlane) );
	tmp.w = localPlane.w - ( localPlane%unrotateVector( w_pos ) );
	return tmp;
}

inline wVector wMatrix::untransformPlane( const wVector &globalPlane ) const {
	wVector tmp( unrotateVector( globalPlane ) );
	tmp.w = globalPlane % w_pos + globalPlane.w;
	return tmp;
}

inline bool wMatrix::sanityCheck() {
	// it correspond to (x_ax * y_ax) % z_ax
	real zz = (x_ax.y*y_ax.z - x_ax.z*y_ax.y)*z_ax.x
			+ (x_ax.z*y_ax.x - x_ax.x*y_ax.z)*z_ax.y
			+ (x_ax.x*y_ax.y - x_ax.y*y_ax.x)*z_ax.z;
	//--- Gianluca: era <0.9999f e produceva molti errori, cmq nessuno al di sopra
	//              di 0.999f; forse era dovuto solo ad errori numerici per via della
	//              precisione (float)
	if( fabs( zz ) < 0.99f) {
		//--- comment out by Gianluca - qDebug() << zz;
		return false;
	}
	x_ax.w = 0.0;
	y_ax.w = 0.0;
	z_ax.w = 0.0;
	w_pos.w = 1.0;
	return true;
}

inline void wMatrix::sanitifize() {
	//x_ax.normalize();
	//y_ax.normalize();
	//z_ax.normalize();
	x_ax.w = 0.0;
	y_ax.w = 0.0;
	z_ax.w = 0.0;
	w_pos.w = 1.0;
}

inline wMatrix wMatrix::identity() {
	return wMatrix( wVector (1.0f, 0.0f, 0.0f, 0.0f),
				    wVector (0.0f, 1.0f, 0.0f, 0.0f),
				    wVector (0.0f, 0.0f, 1.0f, 0.0f),
				    wVector (0.0f, 0.0f, 0.0f, 1.0f) );
}

inline wMatrix wMatrix::zero() {
	return wMatrix( wVector (0.0f, 0.0f, 0.0f, 0.0f),
					wVector (0.0f, 0.0f, 0.0f, 0.0f),
					wVector (0.0f, 0.0f, 0.0f, 0.0f),
					wVector (0.0f, 0.0f, 0.0f, 0.0f) );
}

inline wMatrix wMatrix::grammSchmidt(const wVector& dir) {
	wVector X;
	wVector Y;
	wVector Z( dir );

	Z.normalize();
	if( fabs(Z.z) > 0.577f ) {
		X = Z * wVector( -Z.y, Z.z, 0.0f );
	} else {
		X = Z * wVector( -Z.y, Z.x, 0.0f );
	}
	X.normalize();
	Y = Z * X;

	X.w = 0.0f;
	Y.w = 0.0f;
	Z.w = 0.0f;
	return wMatrix( X, Y, Z, wVector(0.0f, 0.0f, 0.0f, 1.0f) );
}

inline wMatrix wMatrix::pitch( real ang ) {
	real cosAng = cos(ang);
	real sinAng = sin(ang);
	return wMatrix( wVector(1.0f,    0.0f,    0.0f, 0.0f),
					wVector(0.0f,  cosAng,  sinAng, 0.0f),
					wVector(0.0f, -sinAng,  cosAng, 0.0f),
					wVector(0.0f,    0.0f,    0.0f, 1.0f) );
}

inline wMatrix wMatrix::yaw( real ang) {
	real cosAng = cos(ang);
	real sinAng = sin(ang);
	return wMatrix( wVector (cosAng, 0.0f, -sinAng, 0.0f),
					wVector (0.0f,   1.0f,    0.0f, 0.0f),
					wVector (sinAng, 0.0f,  cosAng, 0.0f),
					wVector (0.0f,   0.0f,    0.0f, 1.0f) );
}

inline wMatrix wMatrix::roll( real ang ) {
	real cosAng = cos(ang);
	real sinAng = sin(ang);
	return wMatrix( wVector ( cosAng, sinAng, 0.0f, 0.0f),
					wVector (-sinAng, cosAng, 0.0f, 0.0f),
					wVector (   0.0f,   0.0f, 1.0f, 0.0f),
					wVector (   0.0f,   0.0f, 0.0f, 1.0f) );
}

} // end namespace farsa

#endif
