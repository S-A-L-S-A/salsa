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

#include "glcontextanddata.h"

namespace farsa {

GLContextAndData::GLContextAndData()
	: m_glContext(NULL)
	, m_textureMap()
	, m_textureIds()
	, m_widget(NULL)
	, m_drawObjects(true)
	, m_drawLabels(false)
	, m_drawLocalAxes(false)
	, m_drawJoints(false)
	, m_drawAABBs(false)
	, m_drawOBBs(false)
	, m_drawForces(false)
	, m_drawWireframes(false)
	, m_drawContacts(false)
{
}

GLContextAndData::~GLContextAndData()
{
	deleteTextures();
}

void GLContextAndData::setGLContext(QGLContext* glContext)
{
	// Deleting all textures for the current context
	deleteTextures();

	// Setting the new context
	m_glContext = glContext;
}

const QGLContext* GLContextAndData::glContext() const
{
	if (m_glContext != NULL) {
		return m_glContext;
	} else if (m_widget != NULL) {
		return m_widget->context();
	}

	return NULL;
}

void GLContextAndData::setTextureMap(const QMap<QString, QImage>& textures)
{
	// Deleting all textures for the current context
	deleteTextures();

	// Setting the new map of textures
	m_textureMap = textures;
}

bool GLContextAndData::textureID(QString texture, GLuint& textureID)
{
	if ((m_glContext == NULL) && (m_widget == NULL)) {
		return false;
	}

	if (m_textureMap.contains(texture)) {
		if (!m_textureIds.contains(texture)) {
			if (m_glContext != NULL) {
				m_textureIds[texture] = m_glContext->bindTexture(m_textureMap[texture], GL_TEXTURE_2D, GL_RGB);
			} else {
				m_textureIds[texture] = m_widget->bindTexture(m_textureMap[texture], GL_TEXTURE_2D, GL_RGB);
			}
		}

		textureID = m_textureIds[texture];

		return true;
	}

	return false;
}

void GLContextAndData::setWidget(QGLWidget* widget)
{
	if (m_glContext == NULL) {
		deleteTextures();
	}

	m_widget = widget;
}

void GLContextAndData::setDrawObjects(bool d)
{
	m_drawObjects = d;
}

void GLContextAndData::setDrawLabels(bool d)
{
	m_drawLabels = d;
}

void GLContextAndData::setDrawLocalAxes(bool d)
{
	m_drawLocalAxes = d;
}

void GLContextAndData::setDrawJoints(bool d)
{
	m_drawJoints = d;
}

void GLContextAndData::setDrawAABBs(bool d)
{
	m_drawAABBs = d;
}

void GLContextAndData::setDrawOBBs(bool d)
{
	m_drawOBBs = d;
}

void GLContextAndData::setDrawForces(bool d)
{
	m_drawForces = d;
}

void GLContextAndData::setDrawWireframes(bool d)
{
	m_drawWireframes = d;
}

void GLContextAndData::setDrawContacts(bool d)
{
	m_drawContacts = d;
}

void GLContextAndData::deleteTextures()
{
	if ((m_glContext == NULL) && (m_widget == NULL)) {
		return;
	}

	for (QMap<QString, GLuint>::iterator it = m_textureIds.begin(); it != m_textureIds.end(); ++it) {
		if (m_glContext != NULL) {
			m_glContext->deleteTexture(it.value());
		} else {
			m_widget->deleteTexture(it.value());
		}
	}
	m_textureIds.clear();
}

} // end namespace farsa
