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

#include "renderworld.h"
#include "phyjoint.h"

#include <QImage>
#include <QColor>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QList>
#include <cmath>
#include <QDir>
#include <QLinkedList>
#include <QMutexLocker>
#include <QEvent>
#include <QGLWidget>
#include "farsaglutils.h"

// These instructions are needed because QT 4.8 no longer depends on glu, so we
// have to include it here explicitly
#ifdef FARSA_MAC
# include <GLUT/glut.h>
#else
# include <GL/glu.h>
#endif

using namespace qglviewer;

#include "qglviewer/camera.h"
#include "qglviewer/manipulatedCameraFrame.h"
#if QT_VERSION >= 0x040000
# include <QWheelEvent>
#endif

namespace farsa {

/**
 * \brief The namespace with internal helper classes
 *
 * \internal
 */
namespace {
	class StandardCamera : public qglviewer::Camera
	{
	public :
		StandardCamera()
		{
			orthoSize = 1.0;
		}

		virtual float zNear() const
		{
			return 0.001f;
		}

		virtual float zFar() const
		{
			return 1000.0f;
		}

		void changeOrthoFrustumSize(int delta)
		{
			if (delta > 0) {
				orthoSize *= 1.1f;
			} else {
				orthoSize /= 1.1f;
			}
		}

		virtual void getOrthoWidthHeight(GLdouble &halfWidth, GLdouble &halfHeight) const
		{
			halfHeight = orthoSize;
			halfWidth = aspectRatio() * orthoSize;
		}
	private:
		float orthoSize;
	};

	class MyCameraConstraint : public qglviewer::Constraint
	{
	public:
		/*! \p min and \p max are the bouding-box on which the camera will be constrained */
		MyCameraConstraint( qglviewer::Vec min, qglviewer::Vec max )
		{
			//--- the pad allow to avoid to reach exactly the bounding-box of the world
			qglviewer::Vec pad(0.1,0.1,0.1);
			minP = min + pad;
			maxP = max - pad;
		}

		virtual void constrainTranslation( qglviewer::Vec& t, qglviewer::Frame* const fr )
		{
			//--- Local System
			const qglviewer::Frame* ls = fr;
			if ( fr->referenceFrame() != NULL ) {
				qDebug() << "Using Reference";
				ls = fr->referenceFrame();
			}

			//--- Convert t to world coordinate system
			qglviewer::Vec tw = ls->inverseTransformOf( t );
			qglviewer::Vec pos = fr->position();
			if ( (pos.x + tw.x > maxP.x && t.x > 0) || (pos.x + tw.x < minP.x && t.x < 0) ||
			     (pos.y + tw.y > maxP.y && t.y > 0) || (pos.y + tw.y < minP.y && t.y < 0) ||
			     (pos.z + tw.z > maxP.z && t.z > 0) || (pos.z + tw.z < minP.z && t.z < 0) ) {
				t.z = 0.0;
				t.x = 0.0;
				t.y = 0.0;
			}
		}

		//--- boundings
		qglviewer::Vec minP;
		qglviewer::Vec maxP;
	};
}

RenderWorld::RenderWorld(GUIRenderersContainerDataExchange* dataProvider, QWidget* parent, Qt::WindowFlags flags)
	: QGLViewer(parent, NULL, flags)
	, m_dataProvider(dataProvider)
	, m_contextAndData()
	, m_showskygroundbox(true)
	, m_showcontacts(false)
	, m_showtimeandstep(true)
	, m_worldMin(m_dataProvider->worldGraphicalInfo().minP)
	, m_worldMax(m_dataProvider->worldGraphicalInfo().maxP)
{
	// QGLViewer initialization
	setStateFileName(QString());
	setCamera(new StandardCamera());
	setContextMenuPolicy(Qt::CustomContextMenu);

	// This is needed to draw labels...
	m_contextAndData.setWidget(this);

	// Connecting signals from m_dataProvider
	connect(m_dataProvider, SIGNAL(newTextures()), this, SLOT(newTextures()));
	connect(m_dataProvider, SIGNAL(worldGraphicalInfoChanged()), this, SLOT(worldGraphicalInfoChanged()));
	connect(m_dataProvider, SIGNAL(updated()), this, SLOT(updated()));
}

RenderWorld::~RenderWorld()
{
	// Saving state if we have to
	if (!stateFileName().isEmpty()) {
		saveStateToFile();
	}
}

void RenderWorld::wireframe(bool b)
{
	m_contextAndData.setDrawWireframes(b);
}

void RenderWorld::showSkyGround(bool b)
{
	m_showskygroundbox = b;
}

void RenderWorld::showObjects(bool b)
{
	m_contextAndData.setDrawObjects(b);
}

void RenderWorld::showJoints(bool b)
{
	m_contextAndData.setDrawJoints(b);
}

void RenderWorld::showAABBs(bool b)
{
	m_contextAndData.setDrawAABBs(b);
}

void RenderWorld::showContacts(bool b)
{
	m_showcontacts = b;
}

void RenderWorld::showForces(bool b)
{
	m_contextAndData.setDrawForces(b);
}

void RenderWorld::showLocalAxes(bool b)
{
	m_contextAndData.setDrawLocalAxes(b);
}

void RenderWorld::showLabels(bool b)
{
	m_contextAndData.setDrawLabels(b);
}

void RenderWorld::showTimeAndStep(bool b)
{
	m_showtimeandstep = b;
}

void RenderWorld::contextMenu(const QPoint& p)
{
	//! create actions needed
	QList<QAction*> acts;
	QAction* act;
	if ( m_showskygroundbox ) {
		act = new QAction( "Hide Sky-Ground", this );
	} else {
		act = new QAction( "Show Sky-Ground", this );
	}
	act->setCheckable( true );
	act->setChecked( m_showskygroundbox );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showSkyGround(bool) ) );
	acts.append( act );
	if ( m_contextAndData.drawWireframes() ) {
		act = new QAction( "Hide Wireframe", this );
	} else {
		act = new QAction( "Show Wireframe", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawWireframes() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( wireframe(bool) ) );
	acts.append( act );

	if ( m_contextAndData.drawObjects() ) {
		act = new QAction( "Hide Objects", this );
	} else {
		act = new QAction( "Show Objects", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawObjects() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showObjects(bool) ) );
	acts.append( act );

	if ( m_contextAndData.drawJoints() ) {
		act = new QAction( "Hide Joints", this );
	} else {
		act = new QAction( "Show Joints", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawJoints() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showJoints(bool) ) );
	acts.append( act );

	if ( m_contextAndData.drawAABBs() ) {
		act = new QAction( "Hide AABBs", this );
	} else {
		act = new QAction( "Show AABBs", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawAABBs() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showAABBs(bool) ) );
	acts.append( act );

	if ( m_showcontacts ) {
		act = new QAction( "Hide Contacts", this );
	} else {
		act = new QAction( "Show Contacts", this );
	}
	act->setCheckable( true );
	act->setChecked( m_showcontacts );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showContacts(bool) ) );
	acts.append( act );

	if ( m_contextAndData.drawForces() ) {
		act = new QAction( "Hide Forces", this );
	} else {
		act = new QAction( "Show Forces", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawForces() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showForces(bool) ) );
	acts.append( act );

	if ( m_contextAndData.drawLocalAxes() ) {
		act = new QAction( "Hide Local Axes", this );
	} else {
		act = new QAction( "Show Local Axes", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawLocalAxes() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showLocalAxes(bool) ) );
	acts.append( act );

	if ( m_contextAndData.drawLabels() ) {
		act = new QAction( "Hide Labels", this );
	} else {
		act = new QAction( "Show Labels", this );
	}
	act->setCheckable( true );
	act->setChecked( m_contextAndData.drawLabels() );
	connect( act, SIGNAL( toggled(bool) ),
	         this, SLOT( showLabels(bool) ) );
	acts.append( act );

	QMenu::exec( acts, p );
}

void RenderWorld::newTextures()
{
	// Setting the new map in the m_contextAndData object
	m_contextAndData.setTextureMap(m_dataProvider->textures());
}

void RenderWorld::worldGraphicalInfoChanged()
{
	// Saving the dimensions from the data dataProvider
	m_worldMin = m_dataProvider->worldGraphicalInfo().minP;
	m_worldMax = m_dataProvider->worldGraphicalInfo().maxP;

	// Here we have to change the dimension of the scene
	MyCameraConstraint* tmp = dynamic_cast<MyCameraConstraint*>(camera()->frame()->constraint());
	if ( tmp != NULL ) {
		setSceneBoundingBox( qglviewer::Vec( m_worldMin[0], m_worldMin[1], m_worldMin[2] ),
		                     qglviewer::Vec( m_worldMax[0], m_worldMax[1], m_worldMax[2] ) );
		tmp->minP = qglviewer::Vec( m_worldMin[0], m_worldMin[1], m_worldMin[2] );
		tmp->maxP = qglviewer::Vec( m_worldMax[0], m_worldMax[1], m_worldMax[2] );
	}
}

void RenderWorld::updated()
{
	// We only have to update visualization
	updateGL();
}

void RenderWorld::init()
{
	if (!stateFileName().isEmpty()) {
		if (!restoreStateFromFile()) {
			qDebug() << "Error restoring GL viewer state from file" << stateFileName();
		}
	}

	camera()->frame()->setConstraint(new MyCameraConstraint(qglviewer::Vec(m_worldMin[0], m_worldMin[1], m_worldMin[2]), qglviewer::Vec(m_worldMax[0], m_worldMax[1], m_worldMax[2])));

	// Light0 is the default ambient light
	glEnable(GL_LIGHT0);

	// Here we don't set the gl context because we only have a const pointer. We have set the widget in the
	// constructor and that one will be used for texture binding
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning APPENA PASSIAMO DEFINITIVAMENTE ALLE QT5, PASSARE IL CONTESTO OPENGL NON-CONST (E SE POSSIBILE TOGLIERE IL WIDGET DA m_contextAndData TROVANDO UN ALTRO MODO PER RENDERIZZARE IL TESTO (MEGLIO DI COME È FATTO ADESSO))
#endif
}

void RenderWorld::draw()
{
	// Drawing the sky and ground if we have to
	if (m_showskygroundbox) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		drawSkyGroundBox();
		glPopMatrix();
		glPopAttrib();
	}

	// The light follows the movement of the camera
	float pos[4] = {1.0, 0.5, 1.0, 0.0};
	// Directionnal light
	camera()->frame()->getPosition(pos[0], pos[1], pos[2]);
	glLightfv( GL_LIGHT0, GL_POSITION, pos );

	// Display Contacts
	if (m_showcontacts) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		// Wireframe is ignored when drawing Contacts and Joints
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_LIGHTING);
		// Drawing contacts
		ContactMapIterator itera(m_dataProvider->contacts());
		while(itera.hasNext()) {
			itera.next();
			const ContactVec& vec = itera.value();
			for(int i = 0; i < vec.size(); ++i) {
				GLUtils::drawSphere(vec[i].worldPos, 0.008f);
			}
		}
		glPopAttrib();
	}

	// Rendering all entities in the scene
	m_dataProvider->render(&m_contextAndData);

	// Draw some text if we have to
	if (m_showtimeandstep) {
		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		drawText( 80, 15, QString("time: %1  step: %2")
			.arg(m_dataProvider->elapsedTime())
			.arg((int)(m_dataProvider->elapsedTime()/m_dataProvider->timeStep()))
		);
		glPopAttrib();
	}
}

void RenderWorld::keyPressEvent(QKeyEvent *e)
{
	const Qt::KeyboardModifiers modifiers = e->modifiers();

	// A simple switch on e->key() is not sufficient if we want to take state key into account.
	// With a switch, it would have been impossible to separate 'F' from 'CTRL+F'.
	// That's why we use imbricated if...else and a "handled" boolean.
	bool handled = false;
	if ((e->key()==Qt::Key_Left) && (modifiers==Qt::NoButton)) {
		// rotate camera
		Quaternion qcur = camera()->orientation();
		Quaternion qnew = qcur * Quaternion( Vec(0,1,0), 3.1415/30 );
		camera()->setOrientation( qnew );
		handled = true;
		updateGL();
	} else if ((e->key()==Qt::Key_Right) && (modifiers==Qt::NoButton)) {
		// rotate camera
		Quaternion qcur = camera()->orientation();
		Quaternion qnew = qcur * Quaternion( Vec(0,1,0), -3.1415/30 );
		camera()->setOrientation( qnew );
		handled = true;
		updateGL();
	}
	if ((e->key()==Qt::Key_Up) && (modifiers==Qt::NoButton)) {
		// rotate camera
		Quaternion qcur = camera()->orientation();
		Quaternion qnew = qcur * Quaternion( Vec(1,0,0), 3.1415/30 );
		camera()->setOrientation( qnew );
		handled = true;
		updateGL();
	} else if ((e->key()==Qt::Key_Down) && (modifiers==Qt::NoButton)) {
		// rotate camera
		Quaternion qcur = camera()->orientation();
		Quaternion qnew = qcur * Quaternion( Vec(1,0,0), -3.1415/30 );
		camera()->setOrientation( qnew );
		handled = true;
		updateGL();
	}
	//--- Context Menu (not accessible with right-click, because right button as different meaning)
	if ((e->key()==Qt::Key_M) && (modifiers==Qt::NoButton)) {
		contextMenu( mapToGlobal(QPoint(10,10)) );
		handled = true;
	}

	if (!handled) {
		QGLViewer::keyPressEvent(e);
	}
}

void RenderWorld::drawSkyGroundBox()
{
	const wVector worldSize = wVector(fabs(m_worldMax[0]-m_worldMin[0]), fabs(m_worldMax[1]-m_worldMin[1]), fabs(m_worldMax[2]-m_worldMin[2]));

	glDisable( GL_LIGHTING );
	glShadeModel( GL_FLAT );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	//glEnable(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	// the cube will just be drawn as six quads for the sake of simplicity
	// for each face, we specify the quad's normal (for lighting), then
	// specify the quad's 4 vertices's and associated texture coordinates
	// TOP
	GLuint textureID;
	if (m_contextAndData.textureID("skyb0", textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f( m_worldMin.x,  m_worldMax.y, m_worldMax.z);
		glTexCoord2f(0.0, 0.0); glVertex3f( m_worldMax.x,  m_worldMax.y, m_worldMax.z);
		glTexCoord2f(1.0, 0.0); glVertex3f( m_worldMax.x,  m_worldMin.y, m_worldMax.z);
		glTexCoord2f(1.0, 1.0); glVertex3f( m_worldMin.x,  m_worldMin.y, m_worldMax.z);
		glEnd();
	}
	// BACK
	if (m_contextAndData.textureID("skyb1", textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0, 0.0); glVertex3f( m_worldMin.x,  m_worldMax.y,  m_worldMin.z);
		glTexCoord2f(1.0, 1.0); glVertex3f( m_worldMin.x,  m_worldMax.y,  m_worldMax.z);
		glTexCoord2f(0.0, 1.0); glVertex3f( m_worldMin.x,  m_worldMin.y,  m_worldMax.z);
		glTexCoord2f(0.0, 0.0); glVertex3f( m_worldMin.x,  m_worldMin.y,  m_worldMin.z);
		glEnd();
	}
	// FRONT
	if (m_contextAndData.textureID("skyb2", textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(m_worldMax.x,   m_worldMax.y,  m_worldMax.z);
		glTexCoord2f(0.0, 0.0); glVertex3f(m_worldMax.x,   m_worldMax.y,  m_worldMin.z);
		glTexCoord2f(1.0, 0.0); glVertex3f(m_worldMax.x,   m_worldMin.y,  m_worldMin.z);
		glTexCoord2f(1.0, 1.0); glVertex3f(m_worldMax.x,   m_worldMin.y,  m_worldMax.z);
		glEnd();
	}
	// BOTTOM
	if (m_contextAndData.textureID("skyb3", textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		//--- suppose the bottom texture will represent 40x40 cm of ground
		//--- and calculate repeating accordlying
		const float bfs = worldSize[1]/0.4;
		const float bft = worldSize[0]/0.4;
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, bft); glVertex3f( m_worldMax.x,  m_worldMax.y,  m_worldMin.z);
		glTexCoord2f(0.0, 0.0); glVertex3f( m_worldMin.x,  m_worldMax.y,  m_worldMin.z);
		glTexCoord2f(bfs, 0.0); glVertex3f( m_worldMin.x,  m_worldMin.y,  m_worldMin.z);
		glTexCoord2f(bfs, bft); glVertex3f( m_worldMax.x,  m_worldMin.y,  m_worldMin.z);
		glEnd();
	}
	// RIGHT
	if (m_contextAndData.textureID("skyb4", textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1.0); glVertex3f( m_worldMin.x,  m_worldMin.y,  m_worldMax.z);
		glTexCoord2f(0.0, 1.0); glVertex3f( m_worldMax.x,  m_worldMin.y,  m_worldMax.z);
		glTexCoord2f(0.0, 0.0); glVertex3f( m_worldMax.x,  m_worldMin.y,  m_worldMin.z);
		glTexCoord2f(1.0, 0.0); glVertex3f( m_worldMin.x,  m_worldMin.y,  m_worldMin.z);
		glEnd();
	}
	// LEFT
	if (m_contextAndData.textureID("skyb5", textureID)) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f( m_worldMin.x,  m_worldMax.y,  m_worldMin.z);
		glTexCoord2f(1.0, 0.0); glVertex3f( m_worldMax.x,  m_worldMax.y,  m_worldMin.z);
		glTexCoord2f(1.0, 1.0); glVertex3f( m_worldMax.x,  m_worldMax.y,  m_worldMax.z);
		glTexCoord2f(0.0, 1.0); glVertex3f( m_worldMin.x,  m_worldMax.y,  m_worldMax.z);
		glEnd();
	}
}

} // end namespace farsa
