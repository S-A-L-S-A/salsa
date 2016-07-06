/********************************************************************************
 *  FARSA Utilities Library                                                     *
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

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "utilitiesconfig.h"

#include <QDebug>
#include <QtGlobal>
#include <QMetaType>
#include <cmath>

// On Windows we need the float.h header to use isnan and similar functions
#ifdef WIN32
	#include <float.h>
#endif

namespace farsa {

/**
 * \brief Abstraction on the type of real numbers
 *
 * For Now the library cannot be compiled with double precision
 */
typedef float real;

#define PI_GRECO 3.14159265358979323846f

/**
 * \defgroup math_utils Mathematical Utility Functions
 */

/**
 * \brief Converts degrees to radians
 *
 * \param x the ange in degrees
 * \return the angle in radians
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real toRad(real x)
{
	return x * PI_GRECO / 180.0f;
}

/**
 * \brief Restricts the angle between 0.0 and 2.0 * PI_GRECO
 *
 * \param x the angle to restrict
 * \return the angle restricted between 0.0 and 2.0 * PI_GRECO
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real normalizeRad02pi(real x)
{
	return x - (floor(x / (2.0 * PI_GRECO)) * 2.0 * PI_GRECO);
}

/**
 * \brief Restricts the angle between -PI_GRECO and PI_GRECO
 *
 * \param x the angle to restrict
 * \return the angle restricted between -PI_GRECO and PI_GRECO
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real normalizeRad(real x)
{
	const real n = normalizeRad02pi(x);
	return (n > PI_GRECO) ? (n - 2.0 * PI_GRECO) : n;
}

/**
 * \brief Converts radians to degrees
 *
 * \param x the angle in radians
 * \return the angle in degrees
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real toDegree(real x)
{
	return  x * 180.0f / PI_GRECO;
}

/**
 * \brief Restricts the angle between 0.0° and 360°
 *
 * \param x the angle to restrict
 * \return the angle restricted between 0.0° and 360°
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real normalizeDegree0360(real x)
{
	return x - (floor(x / 360.0) * 360.0);
}

/**
 * \brief Restricts the angle between -180° and 180°
 *
 * \param x the angle to restrict
 * \return the angle restricted between -180° and 180°
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real normalizeDegree(real x)
{
	const real n = normalizeDegree0360(x);
	return (n > 180.0) ? (n - 360.0) : n;
}

/**
 * \brief Template for min calculation
 *
 * \param t1 the first element to compare
 * \param t2 the second element to compare
 * \return the minumum between t1 and t2
 * \ingroup math_utils
 */
template<class T, class U>
FARSA_UTIL_TEMPLATE const T min(const T& t1, const U& t2)
{
	if (t1 < t2) {
		return t1;
	} else {
		return (T)t2;
	}
}

/**
 * \brief Template for max calculation
 *
 * \param t1 the first element to compare
 * \param t2 the second element to compare
 * \return the maxumum between t1 and t2
 * \ingroup math_utils
 */
template<class T, class U>
FARSA_UTIL_TEMPLATE const T max(const T& t1, const U& t2)
{
	if (t1 > t2) {
		return t1;
	} else {
		return (T)t2;
	}
}

/**
 * \brief Clamps the value of a variable in the given range
 *
 * \param minv the minimum value of the range
 * \param maxv the maximum value of the range
 * \param value the value to constraint
 * \return if value is greater than maxv, returns the maxv, if value is less
 *         than minv, returns minv, otherwise returns value
 *  \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE real ramp(real minv, real maxv, real value)
{
	if (value > maxv) {
		return maxv;
	} else if (value < minv) {
		return minv;
	} else {
		return value;
	}
}

/**
 * \brief Linear mapping from [min,max] to [outMin, outMax]
 *
 * <pre>
 *            /--------  outMax
 *           /
 *  --------/            outMin
 *       min   max
 * </pre>
 * \param x the value to map
 * \param min the minumum allowed value of x
 * \param max the maximum allowed value of x
 * \param outMin the minimum value of the mapped value
 * \param outMax the maximum value of the mapped value
 * \return the mapped value
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE float linearMap(float x, float min = -10, float max = 10, float outMin = -1, float outMax = 1)
{
	float m = ( outMax-outMin )/( max-min );
	float q = outMin - m*min;
	float ret = m*x+q;
	if (ret < outMin) {
		return outMin;
	} else if (ret > outMax) {
		return outMax;
	} else {
		return ret;
	}
}

/**
 * \brief Inversed Linear mapping from [min,max] to [outMin, outMax]
 *
 *  <pre>
 *  --------\             outMax
 *           \
 *            \--------   outMin
 *       min   max
 *  </pre>
 * \param x the value to map
 * \param min the minumum allowed value of x
 * \param max the maximum allowed value of x
 * \param outMin the minimum value of the mapped value
 * \param outMax the maximum value of the mapped value
 * \return the mapped value
 * \ingroup math_utils
 */
inline FARSA_UTIL_TEMPLATE float invLinearMap(float x, float min = -10, float max = 10, float outMin = -1, float outMax = 1)
{
	double m = -( outMax-outMin )/( max-min );
	double q = outMax - m*min;
	double ret = m*x+q;
	if (ret < outMin) {
		return outMin;
	} else if (ret > outMax) {
		return outMax;
	} else {
		return ret;
	}
}

/**
 * \brief Macros for isnan and isinf
 *
 * These macros are needed to be able to use isnan and isinf on all platforms
 */
#ifdef WIN32
	#define isnan(x) _isnan(x)
	#define isinf(x) (!_finite(x))
#else
	#define isnan(x) std::isnan(x)
	#define isinf(x) std::isinf(x)
#endif

/**
 * \brief A macro to test whether a variable is NaN
 *
 * This will call qFatal if the value is NaN (abort execution if no message
 * handler has been installed)
 */
#define FARSA_TEST_NAN(__V__) if (isnan(__V__)) { qFatal("Variable " #__V__ " has and invalid value: NaN in %s at line %d", __FILE__, __LINE__); }

/**
 * \brief A macro to test whether a variable is Inf
 *
 * This will call qFatal if the value is Inf (abort execution if no message
 * handler has been installed)
 */
#define FARSA_TEST_INF(__V__) if (isinf(__V__)) { qFatal("Variable " #__V__ " has and invalid value: Infinite in %s at line %d", __FILE__, __LINE__); }

/**
 * \brief A macro to test whether a variable is NaN or Inf
 *
 * This will call qFatal if the value is NaN or Inf (abort execution if no
 * message handler has been installed)
 */
#define FARSA_TEST_INVALID(__V__) if (isnan(__V__)) { qFatal("Variable " #__V__ " has and invalid value: NaN in %s at line %d", __FILE__, __LINE__); } else if (isinf(__V__)) { qFatal("Variable " #__V__ " has and invalid value: Infinite in %s at line %d", __FILE__, __LINE__); }

/**
 * \brief A macro to test whether a variable is NaN in DEBUG builds
 *
 * This is like FARSA_TEST_NAN if building in DEBUG, empty otherwise
 */
#ifdef FARSA_DEBUG
	#define FARSA_DEBUG_TEST_NAN(__V__) FARSA_TEST_NAN(__V__)
#else
	#define FARSA_DEBUG_TEST_NAN(__V__)
#endif

/**
 * \brief A macro to test whether a variable is Inf in DEBUG builds
 *
 * This is like FARSA_TEST_INF if building in DEBUG, empty otherwise
 */
#ifdef FARSA_DEBUG
	#define FARSA_DEBUG_TEST_INF(__V__) FARSA_TEST_INF(__V__)
#else
	#define FARSA_DEBUG_TEST_INF(__V__)
#endif

/**
 * \brief A macro to test whether a variable is NaN or Inf in DEBUG builds
 *
 * This is like FARSA_TEST_VALID if building in DEBUG, empty otherwise
 */
#ifdef FARSA_DEBUG
	#define FARSA_DEBUG_TEST_INVALID(__V__) FARSA_TEST_INVALID(__V__)
#else
	#define FARSA_DEBUG_TEST_INVALID(__V__)
#endif

} // end namespace farsa

#endif
