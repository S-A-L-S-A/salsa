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

#ifndef GLCONTEXTANDDATA_H
#define GLCONTEXTANDDATA_H

#include <QList>
#include <QMap>
#include <QString>
#include <QImage>
#include <QGLContext>
#include "worldsimconfig.h"

namespace salsa {

/**
 * \brief The class with the OpenGL context and shared data
 *
 * This class contains the OpenGL context and shared data such as textures. This
 * lives in the thread where objects are rendered and is passed to renderers'
 * functions. A note about glContext and the widget. In Qt5 QGLWidget returns a
 * non-const pointer to the QGLContext, so we can use it. In Qt4 QGLWidget only
 * returns a const pointer, so we cannot use it e.g. for texture binding. To try
 * to have code that can work on both we leave the possibility to set both the
 * QGLContext directly (via a non-const pointer) and the widget. If the
 * glContext is set directly, that one is used, otherwise we use the function in
 * QGLWidet to bind textures. This distinction should probably be removed once
 * we move to Qt5.
 */
class SALSA_WSIM_API GLContextAndData
{
public:
	/**
	 * \brief Construtor
	 */
	GLContextAndData();

	/**
	 * \brief Destructor
	 */
	~GLContextAndData();

	/**
	 * \brief Sets the OpenGL context
	 *
	 * Changing this pointer invalidates all texture bindings
	 * \param context the OpenGL context
	 */
	void setGLContext(QGLContext* glContext);

	/**
	 * \brief Returns the OpenGL context
	 *
	 * This function always returns the context set using QGLContext. See
	 * class description for more information
	 * \return the OpenGL context
	 */
	QGLContext* glContext()
	{
		return m_glContext;
	}

	/**
	 * \brief Returns the OpenGL context (const version)
	 *
	 * This function returns the context set using setGLContext or the
	 * result of widget->context() if the widget has been set and the
	 * context hasn't. See class description for more information
	 * \return the OpenGL context
	 */
	const QGLContext* glContext() const;

	/**
	 * \brief Sets the map of textures
	 *
	 * Changing this invalidates all texture bindings
	 * \param textures the map of textures
	 */
	void setTextureMap(const QMap<QString, QImage>& textures);

	/**
	 * \brief Returns the OpenGL id of the texture
	 *
	 * This function creates and remembers bindings, so subsequent calls
	 * with the same texture will return the same ID unless the bindings
	 * have been removed by e.g. a call to setTextureMap()
	 * \param texture the name of the texture
	 * \param textureID the variable filled with the id of the texture
	 * \return true if the texture exists and binding was successful, false
	 *         otherwise
	 */
	bool textureID(QString texture, GLuint& textureID);

	/**
	 * \brief Sets a pointer to the QGLWidget inside which we draw
	 *
	 * The QGLWidget is currently required to draw labels. We also use it to
	 * bind textures if setGLContext hasn't been called. Changing this
	 * pointer invalidates all texture bindings if the QGLContext hasn't
	 * been set directly. See class description for more information
	 * \param widget the QGLWidget inside which we draw
	 */
	void setWidget(QGLWidget* widget);

	/**
	 * \brief Returns the QGLWidget inside which we draw
	 *
	 * This can be NULL if we are not drawn inside a QGLWidget
	 * \return the QGLWidget inside which we draw
	 */
	QGLWidget* widget()
	{
		return m_widget;
	}

	/**
	 * \brief Returns the QGLWidget inside which we draw (const version)
	 *
	 * This can be NULL if we are not drawn inside a QGLWidget
	 * \return the QGLWidget inside which we draw
	 */
	const QGLWidget* widget() const
	{
		return m_widget;
	}

	/**
	 * \brief Sets whether to draw objects or not
	 *
	 * \param d if true draws objects
	 */
	void setDrawObjects(bool d);

	/**
	 * \brief Whether to draw objects or not
	 *
	 * \return true if objects are drawn
	 */
	bool drawObjects() const
	{
		return m_drawObjects;
	}

	/**
	 * \brief Sets whether to draw labels or not
	 *
	 * \param d if true draws labels
	 */
	void setDrawLabels(bool d);

	/**
	 * \brief Whether to draw labels or not
	 *
	 * \return true if labels are drawn
	 */
	bool drawLabels() const
	{
		return m_drawLabels;
	}

	/**
	 * \brief Sets whether to draw local axes or not
	 *
	 * \param d if true draws local axes
	 */
	void setDrawLocalAxes(bool d);

	/**
	 * \brief Whether to draw local axes or not
	 *
	 * \return true if local axes are drawn
	 */
	bool drawLocalAxes() const
	{
		return m_drawLocalAxes;
	}

	/**
	 * \brief Sets whether to draw joints or not
	 *
	 * \param d if true draws joints
	 */
	void setDrawJoints(bool d);

	/**
	 * \brief Whether to draw joints or not
	 *
	 * \return true if joints are drawn
	 */
	bool drawJoints() const
	{
		return m_drawJoints;
	}

	/**
	 * \brief Sets whether to draw AABBs or not
	 *
	 * \param d if true draws AABBs
	 */
	void setDrawAABBs(bool d);

	/**
	 * \brief Whether to draw AABBs or not
	 *
	 * \return true if AABBs are drawn
	 */
	bool drawAABBs() const
	{
		return m_drawAABBs;
	}

	/**
	 * \brief Sets whether to draw OBBs or not
	 *
	 * \param d if true draws OBBs
	 */
	void setDrawOBBs(bool d);

	/**
	 * \brief Whether to draw OBBs or not
	 *
	 * \return true if OBBs are drawn
	 */
	bool drawOBBs() const
	{
		return m_drawOBBs;
	}

	/**
	 * \brief Sets whether to draw forces or not
	 *
	 * \param d if true draws forces
	 */
	void setDrawForces(bool d);

	/**
	 * \brief Whether to draw forces or not
	 *
	 * \return true if forces are drawn
	 */
	bool drawForces() const
	{
		return m_drawForces;
	}

	/**
	 * \brief Sets whether to draw wireframes or not
	 *
	 * \param d if true draws wireframes
	 */
	void setDrawWireframes(bool d);

	/**
	 * \brief Whether to draw wireframes or not
	 *
	 * \return true if wireframes are drawn
	 */
	bool drawWireframes() const
	{
		return m_drawWireframes;
	}

	/**
	 * \brief Sets whether to draw contacts or not
	 *
	 * \param d if true draws contacts
	 */
	void setDrawContacts(bool d);

	/**
	 * \brief Whether to draw contacts or not
	 *
	 * \return true if contacts are drawn
	 */
	bool drawContacts() const
	{
		return m_drawContacts;
	}

protected:
	/**
	 * \brief Un-binds all textures
	 */
	void deleteTextures();

	/**
	 * \brief The OpenGL context
	 */
	QGLContext* m_glContext;

	/**
	 * \brief The map of textures
	 */
	QMap<QString, QImage> m_textureMap;

	/**
	 * \brief A map with the IDs of textures that have been bound
	 */
	QMap<QString, GLuint> m_textureIds;

	/**
	 * \brief The QGLWidget inside which we draw
	 *
	 * This can be NULL if we are not drawn inside a QGLWidget
	 */
	QGLWidget* m_widget;

	/**
	 * \brief Whether to draw objects or not
	 *
	 * Default is true
	 */
	bool m_drawObjects;

	/**
	 * \brief Whether to draw labels or not
	 *
	 * Default is false
	 */
	bool m_drawLabels;

	/**
	 * \brief Whether to draw local axes or not
	 *
	 * Default is false
	 */
	bool m_drawLocalAxes;

	/**
	 * \brief Whether to draw joints or not
	 *
	 * Default is false
	 */
	bool m_drawJoints;

	/**
	 * \brief Whether to draw AABBs or not
	 *
	 * Default is false
	 */
	bool m_drawAABBs;

	/**
	 * \brief Whether to draw OBBs or not
	 *
	 * Default is false
	 */
	bool m_drawOBBs;

	/**
	 * \brief Whether to draw forces or not
	 *
	 * Default is false
	 */
	bool m_drawForces;

	/**
	 * \brief Whether to draw wireframes or not
	 *
	 * Default is false
	 */
	bool m_drawWireframes;

	/**
	 * \brief Whether to draw contacts or not
	 *
	 * Default is false
	 */
	bool m_drawContacts;

private:
	/**
	 * \brief Copy constructor
	 *
	 * Here to disallow usage
	 */
	GLContextAndData(const GLContextAndData& other);

	/**
	 * \brief Copy operator
	 *
	 * Here to disallow usage
	 */
	GLContextAndData& operator=(const GLContextAndData& other);
};

} // end namespace salsa

#endif
