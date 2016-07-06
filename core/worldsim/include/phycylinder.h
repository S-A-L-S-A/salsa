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

#ifndef PHYCYLINDER_H
#define PHYCYLINDER_H

#include "phyobject.h"
#include "intervals.h"
#include "dataexchange.h"
#include <QMap>

namespace farsa {

class RenderPhyCylinder;

/**
 * \brief The structure to define the color of intervals of the cylinder
 *
 * Intervals angles are in radians. See the description of setSegmentsColor()
 * for more information
 */
struct FARSA_WSIM_TEMPLATE PhyCylinderSegmentColor {
	/**
	 * \brief Constructor
	 *
	 * Sets color to white
	 */
	PhyCylinderSegmentColor()
		: intervals()
		, color(Qt::white)
	{
	}

	/**
	 * \brief Constructor
	 *
	 * \param i the interval of the cylinder of the given color
	 * \param c the color of the segment
	 */
	PhyCylinderSegmentColor(Intervals i, QColor c) :
		intervals(i),
		color(c)
	{
	}

	/**
	 * \brief The intervals of the cylinder with the given color
	 */
	Intervals intervals;

	/**
	 * \brief The color of the segment
	 */
	QColor color;
};

/**
 * \brief The shared data for the PhyCylinder
 */
class FARSA_WSIM_TEMPLATE PhyCylinderShared : public PhyObjectShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyCylinderShared()
		: PhyObjectShared()
		, radius(0.0)
		, height(0.0)
		, upperBaseColor()
		, lowerBaseColor()
		, segmentsColor()
		, colorChangeTrigger()
	{
	}

	/**
	 * \brief The cylinder radius
	 */
	real radius;

	/**
	 * \brief The cylinder height
	 */
	real height;

	/**
	 * \brief The color of the upper base
	 */
	QColor upperBaseColor;

	/**
	 * \brief The color of the lower base
	 */
	QColor lowerBaseColor;

	/**
	 * \brief The color of cylinder segments
	 *
	 * See PhyCylinder::setSegmentsColor() description for more information
	 * on how to interpret data here
	 */
	QList<PhyCylinderSegmentColor> segmentsColor;

	/**
	 * \brief This should be triggered whenever the colo is changed to tell
	 *        the renderer to update the cylinder representation
	 */
	UpdateTriggerLongNoRecentUpdateCheck colorChangeTrigger;
};

/**
 * \brief The class modelling a physical cylinder
 *
 * The cylinder is described by the radius and the height. The local frame of
 * reference has the x axis along the line connecting the center of the two
 * bases and the center of the frame is in the middle of that line. For this
 * object you can either specify a single color (using methods inherited from
 * WObject) or you can use functions provided here to separately specify the
 * colors of the two bases and of sectors of the cylinder (see function
 * description for more information). If you use functions specific to this
 * class, the color of the object retuned by WObject::color() will be invalid.
 * To set a single color for the whole cylinder, simply use WObject::setColor
 * with a valid color. More precisely, as soon as you use one of the functions
 * defined here to set colors (setUpperBaseColor, setLowerBaseColor or
 * setSegmentsColor) those colors are used to draw the cylinder and the
 * WObject::color property is set to an invalid color (the color of other parts
 * of the cylinder are set to WObject::color before making it invalid); if you
 * explicitly use WObject::setColor, the cylinder is drawn with a single color
 */
class FARSA_WSIM_API PhyCylinder : public PhyObject
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyCylinderShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyCylinder Renderer;

	/**
	 * \brief Returns the min and max points of the Axis-Aligned Bounding
	 *        Box (AABB, aligned to axes of the given frame of reference)
	 *
	 * The box is aligned with the axes of the frame of reference described
	 * by the tm matrix. If you pass the object transformation matrix as tm
	 * you get the AABB aligned to world axes
	 * \param sharedData the object with data from WObject
	 * \param minPoint the minimum point of the AABB
	 * \param maxPoint the maximum point of the AABB
	 * \param tm the frame of reference in which the AABB is to be computed
	 */
	static void calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm);

	/**
	 * \brief Returns the dimension of the Minimum Bounding Box (MBB) of the
	 *        object
	 *
	 * This should returns the dimensions of the arbitrarily oriented
	 * minimum bounding box. In other words this should return the size of
	 * the smallest possible bounding box containing the object
	 * \param sharedData the object with data from WObject
	 * \return the size of the MBB
	 */
	static wVector calculateMBBSize(const Shared* sharedData);

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param radius radius of cylinder
	 * \param height the height of cylinder (cylinder 'grows up' along its
	 *               local X axis)
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PhyCylinder(World* world, SharedDataWrapper<Shared> shared, real radius, real height, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyCylinder();

public:
	/**
	 * \brief Returns the radius of the cylinder
	 *
	 * \return the radius of the cylinder
	 */
	real radius() const
	{
		return m_shared->radius;
	}

	/**
	 * \brief Returns the height of the cylinder
	 *
	 * \return the height of the cylinder
	 */
	real height() const
	{
		return m_shared->height;
	}

	/**
	 * \brief Sets the color of the upper base
	 *
	 * \param color the new color of the upper base. If this is invalid, the
	 *              base is drawn with slices corresponding to those defined
	 *              by setSegmentsColor
	 */
	void setUpperBaseColor(QColor color);

	/**
	 * \brief Returns the color of the upper base
	 *
	 * \return the color of the upper base. If this is invalid, the base is
	 *         drawn with slices corresponding to those defined by
	 *         setSegmentsColor()
	 */
	const QColor& upperBaseColor() const
	{
		return m_shared->upperBaseColor;
	}

	/**
	 * \brief Sets the color of the lower base
	 *
	 * \param color the new color of the upper base. If this is invalid, the
	 *              base is drawn with slices corresponding to those defined
	 *              by setSegmentsColor()
	 */
	void setLowerBaseColor(QColor color);

	/**
	 * \brief Returns the color of the lower base
	 *
	 * \return the color of the lower base· If this is invalid, the base is
	 *         drawn with slices corresponding to those defined by
	 *         setSegmentsColor()
	 */
	const QColor& lowerBaseColor() const
	{
		return m_shared->lowerBaseColor;
	}

	/**
	 * \brief Sets the color of segments of the cylinder
	 *
	 * This function takes a base color and a list of
	 * PhyCylinderSegmentColor objects. The base color is used for all the
	 * portions of the cylinder which are not covered by
	 * PhyCylinderSegmentColor specified in the list. The intervals in the
	 * list are clamped to [-pi, pi], values outside are discarded. A color
	 * later in the list is covered by earlier one. All angles are in
	 * radians and are counterclockwise from the Y axis looking at the
	 * cylinder from the tip of the X axis (on the YZ plane, remember that
	 * the cylinder grows along X). Note that internally the list of colors
	 * is stored in a different format, so the list you pass here is
	 * probably not the one you get with the function segmentsColor()
	 * \param base the base color of the cylinder
	 * \param segmentColors the list of colors of segments
	 */
	void setSegmentsColor(QColor base, const QList<PhyCylinderSegmentColor>& segmentsColor);

	/**
	 * \brief Returns the color of segments of the cylinder
	 *
	 * \note All intervals are simple intervals, contiguous and go from -pi
	 *       to pi
	 * \return the color of segments of the cylinder
	 */
	const QList<PhyCylinderSegmentColor>& segmentsColor() const
	{
		if (m_shared->color.isValid()) {
			m_uniformColor[0].color = m_shared->color;

			return m_uniformColor;
		} else {
			return m_shared->segmentsColor;
		}
	}

	// Adding code for virtual bounding box functions
	FARSA_IMPLEMENT_VIRTUAL_BB

protected:
	/**
	 * \brief Creates the objects needed by the underlying physics engine
	 *
	 * This is called by world after the object has been constructed (so
	 * that the correct virtual function is called)
	 * \param onlyCollisionShape if true only the collision shape is created
	 * \param collisionShapeOffset if not NULL, the offset of the collision
	 *                             shape is set to this matrix
	 */
	virtual void createPrivateObject(bool onlyCollisionShape, const wMatrix* collisionShapeOffset);

private:
	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief The color of cylinder segments when there is only one color
	 *
	 * This is set and returned in segmentsColor() when there is a uniform
	 * color
	 */
	mutable QList<PhyCylinderSegmentColor> m_uniformColor;

	/**
	 * \brief World is friend to access m_priv and call both
	 *        createPrivateObject() and postCreatePrivateObject() and to
	 *        create instances
	 */
	friend class World;
};

/**
 * \brief The class rendering the PhyCylinder
 */
class FARSA_WSIM_API RenderPhyCylinder : public RenderPhyObject
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param entity the entity we render. YOU MUST NOT KEEP A REFERENCE TO
	 *               IT!!! This is only passed in case you need to do custom
	 *               initializations, the render() function will be passed
	 *               updated data at each call externally
	 */
	RenderPhyCylinder(const PhyCylinder* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyCylinder();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const PhyCylinderShared* sharedData, GLContextAndData* contextAndData);

	// Adding code for virtual bounding box functions
	FARSA_IMPLEMENT_VIRTUAL_RENDERER_BB(PhyCylinder)

private:
	/**
	 * \brief Updates the graphical representation of the cylinder if needed
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 */
	void updateGraphicalRepresentation(const PhyCylinderShared* sharedData);

	/**
	 * \brief The checker for changes in the color of the cylinder
	 *
	 * If an update is needed we have to update the graphical representation
	 */
	UpdateCheckerLong m_colorChangeChecker;

	/**
	 * \brief The last value of the color of the whole cylinder
	 *
	 * If the value of color in the current shared data is different from
	 * this value, we have to update the graphical representation of the
	 * cylinder
	 */
	QColor m_oldColor;

	/**
	 * \brief The array of vertexes of the cylinder
	 *
	 * Vertexes are meant to be drawn as TRIANGLE_STRIP. This is a plain
	 * array, each block of 3 values is a vertex
	 */
	QVector<GLfloat> m_cylinderVertexes;

	/**
	 * \brief The array of normals to vertexes of the cylinder
	 *
	 * This is a plain array, each block of 3 values is a normal
	 */
	QVector<GLfloat> m_cylinderNormals;

	/**
	 * \brief The array of colors of vertexes of the cylinder
	 *
	 * This is a plain array, each block of 3 values is a color
	 */
	QVector<GLfloat> m_cylinderColors;

	/**
	 * \brief The arrayof texture coordinates for each vertex of the
	 *        cylinder
	 *
	 * This is a plain array, each block of 2 values is a texture coordinate
	 */
	QVector<GLfloat> m_cylinderTextureCoords;

	/**
	 * \brief The array of vertexes of the upper base
	 *
	 * Vertexes are meant to be drawn as TRIANGLE_FAN. This is a plain
	 * array, each block of 3 values is a vertex
	 */
	QVector<GLfloat> m_upperBaseVertexes;

	/**
	 * \brief The array of normals to vertexes of the upper base
	 *
	 * This is a plain array, each block of 3 values is a normal
	 */
	QVector<GLfloat> m_upperBaseNormals;

	/**
	 * \brief The array of colors of vertexes of the upper base
	 *
	 * This is a plain array, each block of 3 values is a color
	 */
	QVector<GLfloat> m_upperBaseColors;

	/**
	 * \brief The arrayof texture coordinates for each vertex of the upper
	 *        base
	 *
	 * This is a plain array, each block of 2 values is a texture coordinate
	 */
	QVector<GLfloat> m_upperBaseTextureCoords;

	/**
	 * \brief The array with the number of vertexes for each segment of the
	 *        upper base (each segment has a different color)
	 */
	QVector<unsigned int> m_upperBaseSegmentsLength;

	/**
	 * \brief The array of vertexes of the lower base
	 *
	 * Vertexes are meant to be drawn as TRIANGLE_FAN. This is a plain
	 * array, each block of 3 values is a vertex
	 */
	QVector<GLfloat> m_lowerBaseVertexes;

	/**
	 * \brief The array of normals to vertexes of the lower base
	 *
	 * This is a plain array, each block of 3 values is a normal
	 */
	QVector<GLfloat> m_lowerBaseNormals;

	/**
	 * \brief The array of colors of vertexes of the lower base
	 *
	 * This is a plain array, each block of 3 values is a color
	 */
	QVector<GLfloat> m_lowerBaseColors;

	/**
	 * \brief The arrayof texture coordinates for each vertex of the lower
	 *        base
	 *
	 * This is a plain array, each block of 2 values is a texture coordinate
	 */
	QVector<GLfloat> m_lowerBaseTextureCoords;

	/**
	 * \brief The array with the number of vertexes for each segment of the
	 *        lower base (each segment has a different color)
	 */
	QVector<unsigned int> m_lowerBaseSegmentsLength;
};

} // end namespace farsa

#endif
