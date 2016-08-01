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

#ifndef WVECTOR_H
#define WVECTOR_H

#include "worldsimconfig.h"
#include "qglviewer/vec.h"
#include <QString>
#include <cmath>
#include "mathutils.h"

namespace salsa {

/*! Internal data for wVectorT
 *
 * \par Motivation
 *   The wVectorT class can work with shared data (when the Shared template
 *   parameter is true) or with its own data. This InternalData version is only
 *   used when Shared is false, so this contains the actual vector of 4 real
 */
template <bool Shared>
struct SALSA_WSIM_TEMPLATE InternalData
{
	real data[4];
};

/*! Internal data for wVectorT
 *
 * \par Motivation
 *   This is the template specialization of the class above. This is used when
 *   Shared is true, so this contains nothing as wVector\<true\> uses shared
 *   data
 */
template <>
struct SALSA_WSIM_TEMPLATE InternalData<true>
{
};

/*!  wVector class
 *
 *  \
 *  \par Motivation
 *    4 dimensional Vector of real numbers;
 *   useful for transformation matrix, quaternion and 3d points and vectors.<br>
 *  \par Description
 *    This class has a template parameter which allows to choose whether to have
 *    inner data or shared data. In case of shared data this can only be
 *    constructed using the constructor having a pointer to a vector of real as
 *    a parameter.
 *  \par Warnings
 *    When it is used as 3-dimensional vector the fourth value is set to 1.0
 */
template <bool Shared = false>
class SALSA_WSIM_TEMPLATE wVectorT : protected InternalData<Shared> {
public:
	/*! Constructor */
	wVectorT();
	/*! The function to copy from another kind of wVectorT */
	template <bool OtherShared>
	wVectorT( const wVectorT<OtherShared>& );
	/*! Copy-Constructor */
	wVectorT( const wVectorT& );
	/*! Constructor */
	wVectorT( const real *ptr );
	/*! Constructor */
	wVectorT( real *ptr );
	/*! Constructor */
	wVectorT( real x, real y, real z, real w=1.0 );

	/*! Returns a string representation of the vector */
	operator QString() const;

	/*! Convert to qglviewer::Vec */
	operator qglviewer::Vec() const;

	/*! return a new wVectorT with element scaled by s */
	wVectorT<false> scale( real s ) const;
 	/*! Normalize the vector
	 *  \return itself normalized
	 */
	wVectorT& normalize();
 	/*! Returns a normalized vector with the same direction of the current
 	 *  vector
	 *  \return the normalized vector
	 */
	wVectorT<false> normalized() const;

	/*! indexing operator */
	real& operator[]( int i );
	/*! indexing operator (const version) */
	const real& operator[]( int i ) const;

	/*! Operator - (unary) */
	wVectorT<false> operator-() const;
	/*! Operator + (unary) */
	const wVectorT& operator+() const;

	/*! Operator + */
	template <bool OtherShared>
	wVectorT<false> operator+( const wVectorT<OtherShared> &A ) const;
	/*! Operator - */
	template <bool OtherShared>
	wVectorT<false> operator-( const wVectorT<OtherShared> &A ) const;
	/*! Assignment */
	wVectorT& operator=( const wVectorT &A );
	/*! Assignment */
	template <bool OtherShared>
	wVectorT& operator=( const wVectorT<OtherShared> &A );
	/*! Operator += */
	template <bool OtherShared>
	wVectorT& operator+=( const wVectorT<OtherShared> &A );
	/*! Operator -= */
	template <bool OtherShared>
	wVectorT& operator-=( const wVectorT<OtherShared> &A );

	/*! Compare only the first three elements and return true if their are equals */
	template <bool OtherShared>
	bool operator==( const wVectorT<OtherShared> &A ) const;

	/*! return dot product */
	template <bool OtherShared>
	real operator%( const wVectorT<OtherShared> &A ) const;

	/*! return cross product */
	template <bool OtherShared>
	wVectorT<false> operator*( const wVectorT<OtherShared> &B ) const;

	/*! component wise multiplication */
	template <bool OtherShared>
	wVectorT<false> compProduct( const wVectorT<OtherShared> &A ) const;

	/*! return the norm of this vector */
	real norm() const;

	/*! return the distance from A to B */
	template <bool SharedA, bool SharedB>
	static real distance( const wVectorT<SharedA> &A, const wVectorT<SharedB> &B );

	/*! rotate the position indicated by this wVectorT around the axis by the angle theta
	 * \param axis specify the axis of rotation
	 * \param theta specify the angle of rotation (right-hand system)
	 * \return itself rotated
	 */
	//template <bool OtherShared>
	wVectorT& rotateAround( wVectorT<false> axis, real theta );

	/*! X axis vector */
	static wVectorT<false> X();
	/*! Y axis vector */
	static wVectorT<false> Y();
	/*! Z axis vector */
	static wVectorT<false> Z();

	real &x;
	real &y;
	real &z;
	real &w;
};

/*! wVector is simply the non shared version of wVectorT */
typedef wVectorT<false> wVector;

template <bool Shared>
inline wVectorT<false> wVectorT<Shared>::X() {
	return wVectorT<false>(1,0,0,0);
}

template <bool Shared>
inline wVectorT<false> wVectorT<Shared>::Y() {
	return wVectorT<false>(0,1,0,0);
}

template <bool Shared>
inline wVectorT<false> wVectorT<Shared>::Z() {
	return wVectorT<false>(0,0,1,0);
}

template <>
inline wVectorT<false>::wVectorT() :
	x(InternalData<false>::data[0]),
	y(InternalData<false>::data[1]),
	z(InternalData<false>::data[2]),
	w(InternalData<false>::data[3])
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
	w = 1.0;
}

template <>
template <bool OtherShared>
inline wVectorT<false>::wVectorT( const wVectorT<OtherShared>& src ) :
	x(InternalData<false>::data[0]),
	y(InternalData<false>::data[1]),
	z(InternalData<false>::data[2]),
	w(InternalData<false>::data[3])
{
	x = src.x;
	y = src.y;
	z = src.z;
	w = src.w;
}

template <bool Shared>
inline wVectorT<Shared>::wVectorT( const wVectorT<Shared>& src ) :
	x(src.x),
	y(src.y),
	z(src.z),
	w(src.w)
{
}

template <>
inline wVectorT<false>::wVectorT( const wVectorT<false>& src ) :
	x(InternalData<false>::data[0]),
	y(InternalData<false>::data[1]),
	z(InternalData<false>::data[2]),
	w(InternalData<false>::data[3])
{
	x = src.x;
	y = src.y;
	z = src.z;
	w = src.w;
}

template <>
inline wVectorT<false>::wVectorT(const real *ptr) :
	x(InternalData<false>::data[0]),
	y(InternalData<false>::data[1]),
	z(InternalData<false>::data[2]),
	w(InternalData<false>::data[3])
{
	x = ptr[0];
	y = ptr[1];
	z = ptr[2];
	w = 1.0;
}

template <bool Shared>
inline wVectorT<Shared>::wVectorT(real *ptr) :
	x(InternalData<Shared>::data[0]),
	y(InternalData<Shared>::data[1]),
	z(InternalData<Shared>::data[2]),
	w(InternalData<Shared>::data[3])
{
	x = ptr[0];
	y = ptr[1];
	z = ptr[2];
	w = 1.0;
}

template <>
inline wVectorT<true>::wVectorT(real *ptr) :
	x(ptr[0]),
	y(ptr[1]),
	z(ptr[2]),
	w(ptr[3])
{
	w = 1.0;
}

template <>
inline wVectorT<false>::wVectorT(real _x, real _y, real _z, real _w) :
	x(InternalData<false>::data[0]),
	y(InternalData<false>::data[1]),
	z(InternalData<false>::data[2]),
	w(InternalData<false>::data[3])
{
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->w = _w;
}

template <bool Shared>
inline wVectorT<Shared>::operator QString() const {
	return QString("[%1, %2, %3, %4]").arg(x).arg(y).arg(z).arg(w);
}

template <bool Shared>
inline wVectorT<Shared>::operator qglviewer::Vec() const {
	return qglviewer::Vec( x, y, z );
}

template <bool Shared>
inline real& wVectorT<Shared>::operator[](int i) {
	return (&x)[i];
}

template <bool Shared>
inline const real& wVectorT<Shared>::operator[](int i) const {
	return (&x)[i];
}

template <bool Shared>
inline wVectorT<false> wVectorT<Shared>::scale(real scale) const {
	return wVectorT<false>(x*scale, y*scale, z*scale, w);
}

template <bool Shared>
inline wVectorT<Shared>& wVectorT<Shared>::normalize() {
	wVectorT& self = (*this);
	const real n = sqrt( self%self );
	self.x /= n;
	self.y /= n;
	self.z /= n;
	self.w = 0.0;
	return self;
}

template <bool Shared>
inline wVectorT<false> wVectorT<Shared>::normalized() const {
	wVector ret;
	const real n = norm();
	ret.x = x / n;
	ret.y = y / n;
	ret.z = z / n;
	ret.w = 0.0;
	return ret;
}

template <bool Shared>
inline wVectorT<false> wVectorT<Shared>::operator-() const {
	return wVectorT<false>( -x, -y, -z, w );
}

template <bool Shared>
inline const wVectorT<Shared>& wVectorT<Shared>::operator+() const {
	return (*this);
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<false> wVectorT<Shared>::operator+(const wVectorT<OtherShared> &B) const {
	return wVectorT<false>(x+B.x, y+B.y, z+B.z, w);
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<false> wVectorT<Shared>::operator-(const wVectorT<OtherShared> &A) const {
	return wVectorT<false>(x-A.x, y-A.y, z-A.z, w);
}

template <bool Shared>
inline wVectorT<Shared>& wVectorT<Shared>::operator=( const wVectorT &A )
{
	x = A.x;
	y = A.y;
	z = A.z;
	w = A.w;
	return *this;
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<Shared>& wVectorT<Shared>::operator=(const wVectorT<OtherShared> &A) {
	x = A.x;
	y = A.y;
	z = A.z;
	w = A.w;
	return *this;
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<Shared>& wVectorT<Shared>::operator+=(const wVectorT<OtherShared> &A) {
	x += A.x;
	y += A.y;
	z += A.z;
	w = 1.0;
	return *this;
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<Shared>& wVectorT<Shared>::operator-=(const wVectorT<OtherShared> &A) {
	x -= A.x;
	y -= A.y;
	z -= A.z;
	w = 1.0;
	return *this;
}

template <bool Shared>
template <bool OtherShared>
inline bool wVectorT<Shared>::operator==( const wVectorT<OtherShared> &A ) const {
	return ( x==A.x && y==A.y && z==A.z );
}

template <bool Shared>
template <bool OtherShared>
inline real wVectorT<Shared>::operator%(const wVectorT<OtherShared> &A) const {
	return x*A.x + y*A.y + z*A.z;
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<false> wVectorT<Shared>::operator*(const wVectorT<OtherShared> &B) const {
	return wVectorT<false>(
			y*B.z - z*B.y,
			z*B.x - x*B.z,
			x*B.y - y*B.x,
			w);
}

template <bool Shared>
template <bool OtherShared>
inline wVectorT<false> wVectorT<Shared>::compProduct(const wVectorT<OtherShared> &A) const {
	return wVectorT<false>(x*A.x, y*A.y, z*A.z, A.w);
}

template <bool Shared>
inline real wVectorT<Shared>::norm() const {
	return sqrt( x*x + y*y + z*z );
}

template <bool Shared>
template <bool SharedA, bool SharedB>
inline real wVectorT<Shared>::distance( const wVectorT<SharedA> &A, const wVectorT<SharedB> &B ) {
    return sqrt( (A.x-B.x)*(A.x-B.x) + (A.y-B.y)*(A.y-B.y) + (A.z-B.z)*(A.z-B.z) );
}

template <bool Shared>
//template <bool OtherShared>
inline wVectorT<Shared>& wVectorT<Shared>::rotateAround( wVectorT<false> axis, real theta ) {
	wVector q(0,0,0);
	double costheta, sintheta;
	axis.normalize();
	costheta = cos(theta);
	sintheta = sin(theta);
	q.x += (costheta + (1 - costheta) * axis.x * axis.x) * x;
	q.x += ((1 - costheta) * axis.x * axis.y - axis.z * sintheta) * y;
	q.x += ((1 - costheta) * axis.x * axis.z + axis.y * sintheta) * z;

	q.y += ((1 - costheta) * axis.x * axis.y + axis.z * sintheta) * x;
	q.y += (costheta + (1 - costheta) * axis.y * axis.y) * y;
	q.y += ((1 - costheta) * axis.y * axis.z - axis.x * sintheta) * z;

	q.z += ((1 - costheta) * axis.x * axis.z - axis.y * sintheta) * x;
	q.z += ((1 - costheta) * axis.y * axis.z + axis.x * sintheta) * y;
	q.z += (costheta + (1 - costheta) * axis.z * axis.z) * z;
	x = q.x;
	y = q.y;
	z = q.z;
	return (*this);
}
} // end namespace salsa

#endif
