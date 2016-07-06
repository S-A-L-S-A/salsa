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

#include "farsaglutils.h"
#include <QImage>
#include <QColor>
#include <QList>
#include <cmath>
#include <QGLWidget>
#include "rendererscontainer.h"
#include "glcontextanddata.h"

// These instructions are needed because QT 4.8 no longer depends on glu, so we
// have to include it here explicitly
#ifdef FARSA_MAC
# include <GLUT/glut.h>
#else
# include <GL/glu.h>
#endif

using namespace qglviewer;

#include "qglviewer/qglviewer.h"
#include "qglviewer/camera.h"
#include "qglviewer/manipulatedCameraFrame.h"
#if QT_VERSION >= 0x040000
	#include <QWheelEvent>
#endif

namespace farsa {

namespace GLUtils {

	void drawSkyGroundBox(GLContextAndData* contextAndData, const wVector& minPoint, const wVector& maxPoint)
	{
		const wVector bsize = wVector( fabs(maxPoint[0]-minPoint[0]), fabs(maxPoint[1]-minPoint[1]), fabs(maxPoint[1]-minPoint[1]) );

		glDisable( GL_LIGHTING );
		glShadeModel( GL_FLAT );
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

		//glEnable(GL_TEXTURE_2D);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		// The ID of the texture we use
		GLuint textureID;

		// the cube will just be drawn as six quads for the sake of simplicity
		// for each face, we specify the quad's normal (for lighting), then
		// specify the quad's 4 vertices's and associated texture coordinates

		// TOP
		if (contextAndData->textureID("skyb0", textureID)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glEnable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex3f( minPoint.x,  maxPoint.y, maxPoint.z);
			glTexCoord2f(0.0, 0.0); glVertex3f( maxPoint.x,  maxPoint.y, maxPoint.z);
			glTexCoord2f(1.0, 0.0); glVertex3f( maxPoint.x,  minPoint.y, maxPoint.z);
			glTexCoord2f(1.0, 1.0); glVertex3f( minPoint.x,  minPoint.y, maxPoint.z);
			glEnd();
		}

		// BACK
		if (contextAndData->textureID("skyb1", textureID)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glEnable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
			glTexCoord2f(1.0, 0.0); glVertex3f( minPoint.x,  maxPoint.y,  minPoint.z);
			glTexCoord2f(1.0, 1.0); glVertex3f( minPoint.x,  maxPoint.y,  maxPoint.z);
			glTexCoord2f(0.0, 1.0); glVertex3f( minPoint.x,  minPoint.y,  maxPoint.z);
			glTexCoord2f(0.0, 0.0); glVertex3f( minPoint.x,  minPoint.y,  minPoint.z);
			glEnd();
		}

		// FRONT
		if (contextAndData->textureID("skyb2", textureID)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glEnable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex3f(maxPoint.x,   maxPoint.y,  maxPoint.z);
			glTexCoord2f(0.0, 0.0); glVertex3f(maxPoint.x,   maxPoint.y,  minPoint.z);
			glTexCoord2f(1.0, 0.0); glVertex3f(maxPoint.x,   minPoint.y,  minPoint.z);
			glTexCoord2f(1.0, 1.0); glVertex3f(maxPoint.x,   minPoint.y,  maxPoint.z);
			glEnd();
		}

		// BOTTOM
		if (contextAndData->textureID("skyb3", textureID)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glEnable(GL_TEXTURE_2D);

			//--- suppose the bottom texture will represent 40x40 cm of ground
			//--- and calculate repeating accordlying
			float bfs = bsize[1]/0.4;
			float bft = bsize[0]/0.4;
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, bft); glVertex3f( maxPoint.x,  maxPoint.y,  minPoint.z);
			glTexCoord2f(0.0, 0.0); glVertex3f( minPoint.x,  maxPoint.y,  minPoint.z);
			glTexCoord2f(bfs, 0.0); glVertex3f( minPoint.x,  minPoint.y,  minPoint.z);
			glTexCoord2f(bfs, bft); glVertex3f( maxPoint.x,  minPoint.y,  minPoint.z);
			glEnd();
		}

		// RIGHT
		if (contextAndData->textureID("skyb4", textureID)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glEnable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
			glTexCoord2f(1.0, 1.0); glVertex3f( minPoint.x,  minPoint.y,  maxPoint.z);
			glTexCoord2f(0.0, 1.0); glVertex3f( maxPoint.x,  minPoint.y,  maxPoint.z);
			glTexCoord2f(0.0, 0.0); glVertex3f( maxPoint.x,  minPoint.y,  minPoint.z);
			glTexCoord2f(1.0, 0.0); glVertex3f( minPoint.x,  minPoint.y,  minPoint.z);
			glEnd();
		}

		// LEFT
		if (contextAndData->textureID("skyb5", textureID)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glEnable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex3f( minPoint.x,  maxPoint.y,  minPoint.z);
			glTexCoord2f(1.0, 0.0); glVertex3f( maxPoint.x,  maxPoint.y,  minPoint.z);
			glTexCoord2f(1.0, 1.0); glVertex3f( maxPoint.x,  maxPoint.y,  maxPoint.z);
			glTexCoord2f(0.0, 1.0); glVertex3f( minPoint.x,  maxPoint.y,  maxPoint.z);
			glEnd();
		}
	}

	void drawSphere(const wVector& pos, float radius)
	{
		GLUquadricObj *pObj;

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );

		wMatrix mat = wMatrix::identity();
		mat.w_pos = pos;
		mat.x_ax = mat.x_ax.scale( radius );
		mat.y_ax = mat.y_ax.scale( radius );
		mat.z_ax = mat.z_ax.scale( radius );
		glPushMatrix();
		GLMultMatrix(&mat[0][0]);

		// Get a new Quadric off the stack
		pObj = gluNewQuadric();
		// Get a new Quadric off the stack
		gluQuadricTexture(pObj, true);
		gluSphere(pObj, 1.0f, 20, 20);

		gluDeleteQuadric(pObj);
		glPopMatrix();
	}

	void drawCylinder(const wVector& axis, const wVector& centre, float len, float radius, const QColor& c)
	{
		GLUquadricObj *pObj;

		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() );

		wVector recentre = axis.scale( -len*0.5 ) + centre;
		glTranslatef( recentre[0], recentre[1], recentre[2] );

		Vec xg(0,0,1);
		Vec ax( axis[0], axis[1], axis[2] );
		Quaternion quad( xg, ax );
		glMultMatrixd( quad.matrix() );

		// Get a new Quadric off the stack
		pObj = gluNewQuadric();
		gluQuadricTexture(pObj, true);
		gluCylinder(pObj, radius, radius, len, 20, 2);

		// render the caps
		gluQuadricOrientation(pObj, GLU_INSIDE);
		gluDisk(pObj, 0.0f, radius, 20, 1);

		glTranslatef (0.0f, 0.0f, len);
		gluQuadricOrientation(pObj, GLU_OUTSIDE);
		gluDisk(pObj, 0.0f, radius, 20, 1);

		gluDeleteQuadric(pObj);
		glPopMatrix();
	}

	void drawCylinder(const wVector& start, const wVector& end, float radius, QColor c)
	{
		// Calling the function above to draw the cylinder
		const float h = (end - start).norm();
		const wVector axis = (end - start).scale(1.0 / h);
		const wVector centre = (start + end).scale(0.5);

		drawCylinder(axis, centre, h, radius, c);

		// float h = wVector::distance( start, end );
		// if ( h < 0.0001 ) return;
		//
		// GLUquadricObj *pObj;
		//
		// glPushMatrix();
		//
		// // set the color
		// glShadeModel( GL_SMOOTH );
		// glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() );
		//
		// wVector zaxis = end - start;
		// zaxis.normalize();
		// wMatrix tm = wMatrix::grammSchmidt( zaxis );
		// tm.w_pos = start;
		// GLMultMatrix( &tm[0][0] );
		//
		// // Get a new Quadric off the stack
		// pObj = gluNewQuadric();
		// gluQuadricTexture(pObj, true);
		// gluCylinder(pObj, radius, radius, h, 20, 2);
		//
		// // render the caps
		// gluQuadricOrientation(pObj, GLU_INSIDE);
		// gluDisk(pObj, 0.0f, radius, 20, 1);
		//
		// glTranslatef (0.0f, 0.0f, h);
		// gluQuadricOrientation(pObj, GLU_OUTSIDE);
		// gluDisk(pObj, 0.0f, radius, 20, 1);
		//
		// gluDeleteQuadric(pObj);
		// glPopMatrix();
	}

	void drawCylinder(const wMatrix& mat, float len, float radius, QColor c)
	{
		GLUquadricObj *pObj;

		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() );

		GLMultMatrix(&mat[0][0]);

		// Get a new Quadric off the stack
		pObj = gluNewQuadric();
		gluQuadricTexture(pObj, true);
		gluCylinder(pObj, radius, radius, len, 20, 2);

		// render the caps
		gluQuadricOrientation(pObj, GLU_INSIDE);
		gluDisk(pObj, 0.0f, radius, 20, 1);

		glTranslatef (0.0f, 0.0f, len);
		gluQuadricOrientation(pObj, GLU_OUTSIDE);
		gluDisk(pObj, 0.0f, radius, 20, 1);

		gluDeleteQuadric(pObj);
		glPopMatrix();
	}

	void drawCone(const wMatrix& mat, float len, float radius, QColor c)
	{
		GLUquadricObj *pObj;

		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() );

		GLMultMatrix(&mat[0][0]);

		// Get a new Quadric off the stack
		pObj = gluNewQuadric();
		gluQuadricTexture(pObj, true);
		gluCylinder(pObj, radius, 0, len, 20, 2);

		// render the caps
		gluQuadricOrientation(pObj, GLU_INSIDE);
		gluDisk(pObj, 0.0f, radius, 20, 1);

		gluDeleteQuadric(pObj);
		glPopMatrix();
	}

	void drawArrow(const wVector& direction, const wVector& start, float radius, float tipRadius, float tipLength, QColor c)
	{
		// Computing some values for the arrow. The tail length is negative if the arrow has no tail, the
		// tip radius is scaled if the arrow has no tail
		const real length = direction.norm();
		const real tailLength = (tipLength > length) ? -1.0 : (length - tipLength);
		//const real effectiveTipRadius = (tipLength > length) ? (tipRadius * (length / tipLength)) : tipRadius;

		// Generating a transformation matrix
		wMatrix mtr = wMatrix::grammSchmidt(direction);
		mtr.w_pos = start;

		// First drawing the tail of the arrow (if present)
		if (tailLength > 0.0) {
			drawCylinder(mtr, tailLength, radius, c);

			// Translating the matrix so that it is ready for drawing the tip
			mtr.w_pos += mtr.z_ax.scale(tailLength);
		}

		// Drawing the tip
		drawCone(mtr, tipLength, tipRadius, c);
	}

	void drawArrow(const wVector& from, const wVector& to, float radius, int nbSubdivisions, QColor c)
	{
		glPushMatrix();
		glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() );

		wVector zaxis = to - from;
		real len = zaxis.norm();
		zaxis.normalize();
		wMatrix tm = wMatrix::grammSchmidt( zaxis );
		tm.w_pos = from;
		GLMultMatrix( &tm[0][0] );

		QGLViewer::drawArrow( len, radius, nbSubdivisions );

		glPopMatrix();
	}

	void drawWireBox(const wVector& dims, const wMatrix& matrix)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );

		float hdx = (dims[0]/2.0);
		float hdy = (dims[1]/2.0);
		float hdz = (dims[2]/2.0);
		GLMultMatrix(&matrix[0][0]);

		// the cube will just be drawn as six quads for the sake of simplicity
		// for each face, we specify the quad's normal (for lighting), then
		// specify the quad's 4 vertices and associated texture coordinates
		glBegin(GL_QUADS);
		// front
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-hdx, -hdy, hdz);
		glVertex3f( hdx, -hdy, hdz);
		glVertex3f( hdx,  hdy, hdz);
		glVertex3f(-hdx,  hdy, hdz);

		// back
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f( hdx, -hdy, -hdz);
		glVertex3f(-hdx, -hdy, -hdz);
		glVertex3f(-hdx,  hdy, -hdz);
		glVertex3f( hdx,  hdy, -hdz);

		// top
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-hdx,  hdy,  hdz);
		glVertex3f( hdx,  hdy,  hdz);
		glVertex3f( hdx,  hdy, -hdz);
		glVertex3f(-hdx,  hdy, -hdz);

		// bottom
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-hdx, -hdy, -hdz);
		glVertex3f( hdx, -hdy, -hdz);
		glVertex3f( hdx, -hdy,  hdz);
		glVertex3f(-hdx, -hdy,  hdz);

		// left
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-hdx, -hdy, -hdz);
		glVertex3f(-hdx, -hdy,  hdz);
		glVertex3f(-hdx,  hdy,  hdz);
		glVertex3f(-hdx,  hdy, -hdz);

		// right
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(hdx,  -hdy,  hdz);
		glVertex3f(hdx,  -hdy, -hdz);
		glVertex3f(hdx,   hdy, -hdz);
		glVertex3f(hdx,   hdy,  hdz);
		glEnd();

		glPopMatrix();
	}

	void drawWireBox(const wVector& minPoint, const wVector& maxPoint, const wMatrix& tm)
	{
		glPushMatrix();
		GLMultMatrix(&tm[0][0]);
		drawWireBox( minPoint, maxPoint );
		glPopMatrix();
	}

	void drawWireBox(const wVector& minPoint, const wVector& maxPoint)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( 1.0f, 1.0f, 0.0f, 1.0f );

		float dx = fabs( maxPoint[0]-minPoint[0] );
		float dy = fabs( maxPoint[1]-minPoint[1] );
		float dz = fabs( maxPoint[2]-minPoint[2] );
		float hdx = (dx/2.0);
		float hdy = (dy/2.0);
		float hdz = (dz/2.0);
		float minX = qMin(minPoint[0], maxPoint[0]);
		float minY = qMin(minPoint[1], maxPoint[1]);
		float minZ = qMin(minPoint[2], maxPoint[2]);
		glTranslatef( minX+hdx, minY+hdy, minZ+hdz );

		// the cube will just be drawn as six quads for the sake of simplicity
		// for each face, we specify the quad's normal (for lighting), then
		// specify the quad's 4 vertices and associated texture coordinates
		glBegin(GL_QUADS);
		// front
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-hdx, -hdy, hdz);
		glVertex3f( hdx, -hdy, hdz);
		glVertex3f( hdx,  hdy, hdz);
		glVertex3f(-hdx,  hdy, hdz);

		// back
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f( hdx, -hdy, -hdz);
		glVertex3f(-hdx, -hdy, -hdz);
		glVertex3f(-hdx,  hdy, -hdz);
		glVertex3f( hdx,  hdy, -hdz);

		// top
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-hdx,  hdy,  hdz);
		glVertex3f( hdx,  hdy,  hdz);
		glVertex3f( hdx,  hdy, -hdz);
		glVertex3f(-hdx,  hdy, -hdz);

		// bottom
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-hdx, -hdy, -hdz);
		glVertex3f( hdx, -hdy, -hdz);
		glVertex3f( hdx, -hdy,  hdz);
		glVertex3f(-hdx, -hdy,  hdz);

		// left
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-hdx, -hdy, -hdz);
		glVertex3f(-hdx, -hdy,  hdz);
		glVertex3f(-hdx,  hdy,  hdz);
		glVertex3f(-hdx,  hdy, -hdz);

		// right
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(hdx,  -hdy,  hdz);
		glVertex3f(hdx,  -hdy, -hdz);
		glVertex3f(hdx,   hdy, -hdz);
		glVertex3f(hdx,   hdy,  hdz);
		glEnd();

		glPopMatrix();
	}

	void drawTorus(real outRad, real innRad, const wMatrix& mat, real angle, QColor c)
	{
		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( c.redF(), c.greenF(), c.blueF(), c.alphaF() );

		GLMultMatrix(&mat[0][0]);

		int numc = 8;
		int numt = 25;
		int i, j, k;
		double s, t, x, y, z, twopi;
		real tubeRad = (outRad-innRad)/2.0;
		real toruRad = outRad - tubeRad;
		twopi = 2 * PI_GRECO;
		for (i = 0; i < numc; i++) {
			glBegin(GL_QUAD_STRIP);
			for (j = 0; j <= numt; j++) {
				for (k = 1; k >= 0; k--) {
					s = (i + k) % numc + 0.5;
					t = j; //% numt;

					x = (toruRad+tubeRad*cos(s*twopi/numc))*cos(t*(angle)/numt);
					y = (toruRad+tubeRad*cos(s*twopi/numc))*sin(t*(angle)/numt);
					z = tubeRad * sin(s * twopi / numc);
					glVertex3f(x, y, z);
				}
			}
			glEnd();
		}

		glPopMatrix();
	}

	void drawTorus(const wVector& axis, const wVector& centre, real outRad, real innRad, real angle)
	{
		glPushMatrix();

		// set the color
		glShadeModel( GL_SMOOTH );
		glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );

		wMatrix tm = wMatrix::grammSchmidt( axis );
		tm.w_pos = centre;
		GLMultMatrix( &tm[0][0] );

		int numc = 8;
		int numt = 25;
		int i, j, k;
		double s, t, x, y, z, twopi;
		real tubeRad = (outRad-innRad)/2.0;
		real toruRad = outRad - tubeRad;
		twopi = 2 * PI_GRECO;
		for (i = 0; i < numc; i++) {
			glBegin(GL_QUAD_STRIP);
			for (j = 0; j <= numt; j++) {
				for (k = 1; k >= 0; k--) {
					s = (i + k) % numc + 0.5;
					t = j; //% numt;

					x = (toruRad+tubeRad*cos(s*twopi/numc))*cos(t*(angle)/numt);
					y = (toruRad+tubeRad*cos(s*twopi/numc))*sin(t*(angle)/numt);
					z = tubeRad * sin(s * twopi / numc);
					glVertex3f(x, y, z);
				}
			}
			glEnd();
		}

		glPopMatrix();
	}
}

}
