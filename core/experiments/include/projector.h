// /********************************************************************************
//  *  SALSA Experiments Library                                                   *
//  *  Copyright (C) 2007-2012                                                     *
//  *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
//  *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
//  *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
//  *  Gianluca Massera <emmegian@yahoo.it>                                        *
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
// #ifndef PROJECTOR_H
// #define PROJECTOR_H
//
// #include "experimentsconfig.h"
// #include "wmatrix.h"
// #include "mathutils.h"
//
// namespace salsa {
//
// /**
//  * \brief The structure modelling a single point on a 2D image
//  */
// class SALSA_EXPERIMENTS_TEMPLATE ImagePoint {
// public:
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * This builds an invalid image point
// 	 */
// 	ImagePoint() :
// 		x(0.0),
// 		y(0.0),
// 		valid(false)
// 	{
// 	}
//
// 	/**
// 	 * \brief Constructor
// 	 *
// 	 * This builds a valid image point
// 	 * \param _x the x coordinate of the point
// 	 * \param _y the y coordinate of the point
// 	 */
// 	ImagePoint(double _x, double _y) :
// 		x(_x),
// 		y(_y),
// 		valid(true)
// 	{
// 	}
//
// 	/**
// 	 * \brief Returns true if the point is valid
// 	 *
// 	 * \return true if the point is valid
// 	 */
// 	bool isValid() const
// 	{
// 		return valid;
// 	}
//
// 	/**
// 	 * \brief The x coordinate of the point
// 	 */
// 	double x;
//
// 	/**
// 	 * \brief The y coordinate of the point
// 	 */
// 	double y;
//
// 	/**
// 	 * \brief If true the point is valid
// 	 */
// 	bool valid;
// };
//
// /**
//  * \brief A class projecting a 3D space point into a 2D image point
//  *
//  * The resulting image is the one we would obtain if the focal distance was 1.0;
//  * the center of the image is in <0, 0>. Both the vertical and horizontal
//  * aperture are in degrees.
//  */
// class SALSA_EXPERIMENTS_TEMPLATE Projector
// {
// public:
// 	/**
// 	 * \brief Constructor
// 	 */
// 	Projector() :
// 		m_horizontalAperture(112.0),
// 		m_verticalAperture(94.0),
// 		m_halfImageWidth(0.0),
// 		m_halfImageHeight(0.0),
// 		m_inverseEyeMatrix(),
// 		m_imagePoint()
// 	{
// 		// Computing the width and height of the image we would obtain if the
// 		// focal distance was 1.0
// 		m_halfImageWidth = 1.0 * tan(toRad(m_horizontalAperture / 2.0));
// 		m_halfImageHeight = 1.0 * tan(toRad(m_verticalAperture / 2.0));
// 	};
//
// 	/**
// 	 * \brief Sets the current eye matrix
// 	 *
// 	 * The z axis is taken as the direction of view and the x axis as the
// 	 * opposite of the upvector (see comment in set3DPointEye())
// 	 * \return the current eye matrix
// 	 */
// 	void setEyeMatrix(const wMatrix &eye)
// 	{
// 		m_inverseEyeMatrix = eye.inverse();
// 	}
//
// 	/**
// 	 * \brief Sets the 3D point to project on the 2D image
// 	 *
// 	 * Points coordinates must be in the world frame of reference
// 	 * \param point the 3D point to project on the 2D image
// 	 */
// 	void set3DPointWorld(const wVector &point)
// 	{
// 		set3DPointEye(m_inverseEyeMatrix.transformVector(point));
// 	}
//
// 	/**
// 	 * \brief Sets the 3D point to project on the 2D image
// 	 *
// 	 * Points coordinates must be in the eye frame of reference
// 	 * \param point the 3D point to project on the 2D image
// 	 */
// 	void set3DPointEye(const wVector &point)
// 	{
// 		// Computing the position of the object on the image (remember
// 		// that the focal distance is 1.0). Also note that the upvector
// 		// for the camera is -x where x is the eye object x axis, so we
// 		// have to use -point.y to get the x position on the image and
// 		// -point.x to get the y position on the image. The formula
// 		// below is derived using a simple proportion
//
// 		m_imagePoint.x = -point.y / point.z;
// 		m_imagePoint.y = -point.x / point.z;
//
// 		if(point.z >= 0)
// 			m_imagePoint.valid = true;
// 		else
// 			m_imagePoint.valid = false;
// 	}
//
// 	/**
// 	 * \brief Returns the position on the 2D image of the point to project
// 	 *
// 	 * The point to project is the last one passed to set3DPointWorld() or
// 	 * set3DPointEye()
// 	 * \return the position on the 2D image
// 	 */
// 	const ImagePoint& getImagePoint() const
// 	{
// 		return m_imagePoint;
// 	}
//
// 	/**
// 	 * \brief Returns the normalized position on the 2D image of the point
// 	 *        to project
// 	 *
// 	 * The point to project is the last one passed to set3DPointWorld() or
// 	 * set3DPointEye(). The point returned by this function is normalized
// 	 * (i.e. is in [0, 1]x[0, 1])
// 	 * \return the normlized position on the 2D image
// 	 */
// 	ImagePoint getImagePoint01() const
// 	{
// 		ImagePoint p((m_imagePoint.x + m_halfImageWidth) / (2.0 * m_halfImageWidth), (m_imagePoint.y + m_halfImageHeight) / (2.0 * m_halfImageHeight));
// 		return p;
// 	}
//
// 	/**
// 	 * \brief Returns true if the point is inside the image
// 	 *
// 	 * \return true if the point is inside the image, false otherwise
// 	 */
// 	bool pointInsideImage() const
// 	{
// 		return (m_imagePoint.x <= m_halfImageWidth) && (m_imagePoint.x >= -m_halfImageWidth) &&
// 		       (m_imagePoint.y <= m_halfImageHeight) && (m_imagePoint.y >= -m_halfImageHeight)
// 			   && m_imagePoint.valid;	//if not, works also for z < 0
// 	}
//
// private:
// 	/**
// 	 * \brief The horizontal aperture of the camera (in degrees)
// 	 */
// 	double m_horizontalAperture;
//
// 	/**
// 	 * \brief The vertical aperture of the camera (in degrees)
// 	 */
// 	double m_verticalAperture;
//
// 	/**
// 	 * \brief Half the width of the image given the horizontal aperture as
// 	 *        if the focal distance was 1.0
// 	 *
// 	 * This is computed in initialize(), after the horizontal aperture has
// 	 * been set
// 	 */
// 	double m_halfImageWidth;
//
// 	/**
// 	 * \brief Half the height of the image given the vertual aperture as if
// 	 *        the focal distance was 1.0
// 	 *
// 	 * This is computed in initialize(), after the vertical aperture has
// 	 * been set
// 	 */
// 	double m_halfImageHeight;
//
// 	/**
// 	 * \brief The inverse of the eye matrix
// 	 *
// 	 * The z axis of the direct matrix is taken as the direction of view and
// 	 * the x axis as the opposite of the upvector (see comment in
// 	 * set3DPointEye())
// 	 */
// 	wMatrix m_inverseEyeMatrix;
//
// 	/**
// 	 * \brief The point on the 2D image corresponding to the last point
// 	 *        passed to set3DPointWorld() or set3DPointEye()
// 	 */
// 	ImagePoint m_imagePoint;
// };
//
// } //end namespace salsa
//
// #endif
