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

#ifndef SALSAGLUTILS_H
#define SALSAGLUTILS_H

#include "worldsimconfig.h"
#include "qglviewer/qglviewer.h"
#include "wvector.h"
#include "wmatrix.h"
#include "mathutils.h"
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QString>
#include <QColor>

// For double use #define GLMultMatrix glMultMatrixd and #define GLTranslate glTranslated
#define GLMultMatrix glMultMatrixf
#define GLTranslate glTranslatef

/**
 * \brief The SALSA namespace
 */
namespace salsa {

class GLContextAndData;

/**
 * \brief The namespace with utility functions for OpenGL
 *
 * All the functions here can only be used if the OpenGL context is already set
 * up
 */
namespace GLUtils {
	/**
	 * \brief Draws sky and gound
	 *
	 * \param contextAndData the object with the gl context and data
	 * \param minPoint the minimum point of the world
	 * \param maxPoint the maximum point of the world
	 */
	void SALSA_WSIM_API drawSkyGroundBox(GLContextAndData* contextAndData, const wVector& minPoint, const wVector& maxPoint);

	/**
	 * \brief Draws a sphere
	 *
	 * \param pos the position of the center of the sphere
	 * \param radius the radius of the sphere
	 */
	void SALSA_WSIM_API drawSphere(const wVector& pos, float radius);

	/**
	 * \brief Draws a cylinder
	 *
	 * \param axis the main axis of the cylinder
	 * \param center the center of the cylinder
	 * \param len the length of the cylinder
	 * \param radius the radius of the cylinder
	 * \param c the color of the cylinder
	 */
	void SALSA_WSIM_API drawCylinder(const wVector& axis, const wVector& centre, float len, float radius, const QColor& c = Qt::green);

	/**
	 * \brief Draws a cylinder
	 *
	 * \param start the center of the lower base of the cylinder
	 * \param end the center of the upper base of the cylinder
	 * \param radius the radius of the cylinder
	 * \param c the color of the cylinder
	 */
	void SALSA_WSIM_API drawCylinder(const wVector& start, const wVector& end, float radius, QColor c = Qt::green);

	/**
	 * \brief Draws a cylinder
	 *
	 * The main axis is z, mat center is on the lower base
	 * \param mat the transformation matrix of the cylinder
	 * \param len the length of the cylinder
	 * \param radius the radius of the cylinder
	 * \param c the color of the cylinder
	 */
	void SALSA_WSIM_API drawCylinder(const wMatrix& mat, float len, float radius, QColor c = Qt::green);

	/**
	 * \brief Draws a cone
	 *
	 * The main axis is z, mat center is on the base
	 * \param mat the transformation matrix of the cone
	 * \param len the length of the cone
	 * \param radius the radius of the cone
	 * \param c the color of the cone
	 */
	void SALSA_WSIM_API drawCone(const wMatrix& mat, float len, float radius, QColor c = Qt::green);

	/**
	 * \brief Draws an arrow
	 *
	 * \param direction the direction of the arrow
	 * \param start the starting point of the arrow
	 * \param radius the radius of the arrow (excluding the tip)
	 * \param tipRadius the radius of the base of the tip (the tip is a
	 *                  cone)
	 * \param tipLength the length of the tip
	 * \param color the color of the arrow
	 */
	void SALSA_WSIM_API drawArrow(const wVector& direction, const wVector& start, float radius, float tipRadius, float tipLength, QColor c = Qt::green);

	/**
	 * \brief Draws an arrow
	 *
	 * \param from the starting point of the arrow
	 * \param to the ending point of the arrow
	 * \param radius the radius of the arrow
	 * \param nbSubdivisions the number of subdivisions
	 * \param c the color of the arrow
	 */
	void SALSA_WSIM_API drawArrow(const wVector& from, const wVector& to, float radius = -1.0f, int nbSubdivisions = 12, QColor c = Qt::red);

	/**
	 * \brief Draws a wireframe box
	 *
	 * \param dims the dimensions of the box
	 * \param matrix the tranformation matrix of the box (the center of the
	 *               box is in local (0, 0, 0))
	 */
	void SALSA_WSIM_API drawWireBox(const wVector& dims, const wMatrix& matrix);

	/**
	 * \brief Draws a wireframe box
	 *
	 * \param minPoint the minimum point of the box (in local coordinates)
	 * \param maxPoint the maximum point of the box (in local coordinates)
	 * \param tm the transformation matrix of the box
	 */
	void SALSA_WSIM_API drawWireBox(const wVector& minPoint, const wVector& maxPoint, const wMatrix& tm);

	/**
	 * \brief Draws a wireframe box
	 *
	 * The box is axis aligned
	 * \param minPoint the minimum point of the box (in global coordinates)
	 * \param maxPoint the maximum point of the box (in global coordinates)
	 */
	void SALSA_WSIM_API drawWireBox(const wVector& minPoint, const wVector& maxPoint);

	/**
	 * \brief Draws a Torus
	 *
	 * \param outRad the outer radius
	 * \param innRad the inner radius
	 * \param mat the transformation matrix of the torus
	 * \param angle the sector of the torus that is drawn
	 * \param c the color of the arrow
	 */
	void SALSA_WSIM_API drawTorus(real outRad, real innRad, const wMatrix& mat, real angle = 2.0 * PI_GRECO, QColor c = Qt::red);

	/**
	 * \brief Draws a Torus
	 *
	 * \param axis the axis of the torus
	 * \param center the center of the torus
	 * \param outRad the outer radius
	 * \param innRad the inner radius
	 * \param angle the sector of the torus that is drawn
	 */
	void SALSA_WSIM_API drawTorus(const wVector& axis, const wVector& centre, real outRad, real innRad, real angle = 2.0 * PI_GRECO);
}

} // end namespace salsa

#endif
