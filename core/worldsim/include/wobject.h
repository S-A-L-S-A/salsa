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

#ifndef WOBJECT_H
#define WOBJECT_H

#include "worldsimconfig.h"
#include "wentity.h"
#include "wmatrix.h"
#include "wvector.h"

#include <QString>
#include <QColor>
#include <QGLContext>

namespace salsa {

class World;
class GLContextAndData;
class RenderWObject;

/**
 * \brief The shared data for the WObject
 */
class SALSA_WSIM_TEMPLATE WObjectShared : public WEntityShared
{
public:
	/**
	 * \brief Flags for the WObject
	 */
	enum WObjectFlags {
		Visible = 0x01, /// If set the object is visible (i.e. it is
		                /// rendered)
		DrawLocalReferenceFrame = 0x02, /// If set the local frame of
		                                /// reference is drawn
		DrawLabel = 0x04 /// If true the label is drawn
	};

public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	WObjectShared()
		: WEntityShared()
		, tm(wMatrix::identity())
		, label("")
		, labelPos(0.0f, 0.0f, 0.0f)
		, labelColor(Qt::white)
		, flags(Visible)
	{
	}

	/**
	 * \brief Trasformation matrix
	 */
	wMatrix tm;

	/**
	 * \brief The text label of the object
	 */
	QString label;

	/**
	 * \brief The position of the label in the object frame of reference
	 */
	wVector labelPos;

	/**
	 * \brief The color of the label
	 */
	QColor labelColor;

	/**
	 * \brief Flags for the object
	 *
	 * Valid values are those in WObjectFlags
	 */
	int flags;
};

/**
 * \brief A macro to implement the virtual version of the calculateAABB() and
 *        calculateMBBSize() function (see WObject description for more
 *        information)
 */
#define SALSA_IMPLEMENT_VIRTUAL_BB virtual void calculateAABB(wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const\
                                   {\
                                   	calculateAABB(static_cast<const Shared*>(getShared()), minPoint, maxPoint, tm);\
                                   }\
                                   virtual wVector calculateMBBSize() const\
                                   {\
                                   	return calculateMBBSize(static_cast<const Shared*>(getShared()));\
                                   }

/**
 * \brief World's Object class
 *
 * This represents an abstract object which lives in the World. The constructors
 * and destructor of this class are protected and World is a friend. It is
 * advisable to do the same for subclasses. Subclasses should re-implement the
 * calculateAABB and calculateMBBSize functions, both the static and virtual
 * versions. The virtual versions of the function can simply call the static
 * versions, as in the example below:
 *
 * \code
 * class WObjectSubclass : public WObject
 * {
 * public:
 * 	...
 *
 * 	static void calculateAABB(const Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm);
 * 	static wVector calculateMBBSize(const Shared* sharedData);
 *
 * 	...
 *
 * public:
 * 	...
 *
 * 	virtual void calculateAABB(wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const
 * 	{
 * 		calculateAABB(static_cast<const Shared*>(getShared()), minPoint, maxPoint, tm);
 * 	}
 *
 * 	virtual wVector calculateMBBSize() const
 * 	{
 * 		return calculateMBBSize(static_cast<const Shared*>(getShared()));
 * 	}
 *
 * 	...
 * };
 * \endcode
 *
 * The SALSA_IMPLEMENT_VIRTUAL_BB convenience macro implements the virtual
 * functions inline as in the above example. You simply have to put it inside
 * the class declaration
 */
class SALSA_WSIM_API WObject : public WEntity
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef WObjectShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderWObject Renderer;

	/**
	 * \brief Returns the min and max points of the Axis-Aligned Bounding
	 *        Box (AABB, aligned to axes of the given frame of reference)
	 *
	 * The box is aligned with the axes of the frame of reference described
	 * by the tm matrix. If you pass the object transformation matrix as tm
	 * you get the AABB aligned to world axes. The implementation in this
	 * class does nothing, reimplement in subclasses
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
	 * the smallest possible bounding box containing the object. The
	 * implementation in this class does nothing, reimplement in subclasses
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
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	WObject(World* world, SharedDataWrapper<Shared> shared, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~WObject();

public:
	/**
	 * \brief Returns reference to the transformation matrix
	 */
	const wMatrix& matrix() const;

	/**
	 * \brief Sets a new transformation matrix
	 *
	 * \param newm the new transformation matrix
	 * \return true if the matrix was actually changed
	 */
	bool setMatrix(const wMatrix& newm);

	/**
	 * \brief Sets the position specified in global coordinate frame
	 *
	 * \param newpos the position in the global frame of reference
	 * \return true if the position was actually changed
	 */
	bool setPosition(const wVector& newpos);

	/**
	 * \brief Sets the position specified in global coordinate frame
	 *
	 * \param x the x coordinate
	 * \param y the y coordinate
	 * \param z the z coordinate
	 * \return true if the position was actually changed
	 */
	bool setPosition(real x, real y, real z);

	/**
	 * \brief Whether we are invisible or not
	 *
	 * \return true if we are invisible
	 */
	bool isInvisible() const;

	/**
	 * \brief Set invisibility
	 *
	 * Invisible means that the object will be never rendered on widgets.
	 * \param i if true we are invisible
	 */
	void setInvisible(bool i);

	/**
	 * \brief Returns true if the local frame of refecence of the object
	 *        should be drawn
	 *
	 * \return true if the local frame of reference should be drawn
	 */
	bool localAxesDrawn() const;

	/**
	 * \brief Sets whether the object local frame of reference should be
	 *        drawn or not
	 *
	 * By default the local frame of reference is not drawn. Note that some
	 * objects don't support drawing the local frame of reference. The X
	 * axis is drawn in red, the Y in green and the Z in blue.
	 * \param d if true the local frame of reference should be drawn
	 */
	void setLocalAxesDrawn(bool d);

	/**
	 * \brief Sets a text label to render along with the object
	 *
	 * \param label the label to show
	 */
	void setLabel(QString label);

	/**
	 * \brief Sets a text label to render along with the object and its
	 *        position
	 *
	 * \param label the label to show
	 * \param pos the position of the label in the object frame of reference
	 */
	void setLabel(QString label, wVector pos);

	/**
	 * \brief Sets a text label to render along with the object and its
	 *        position and color
	 *
	 * \param label the label to show
	 * \param pos the position of the label in the object frame of reference
	 * \param color the color of the label
	 */
	void setLabel(QString label, wVector pos, QColor color);

	/**
	 * \brief Returns the text label to render along with the object
	 *
	 * \return the label to show
	 */
	const QString& label() const;

	/**
	 * \brief Sets the label position relative to this object
	 *
	 * \param pos the label position in the object frame of reference
	 */
	void setLabelPosition(const wVector& pos);

	/**
	 * \brief Returns the label position relative to this object
	 *
	 * \return the label position in the object frame of reference
	 */
	const wVector& labelPosition() const;

	/**
	 * \brief Sets the color of the label
	 *
	 * \param color the color of the label
	 */
	void setLabelColor(const QColor& color);

	/**
	 * \brief Returns the color of the label
	 *
	 * \return the color of the label
	 */
	const QColor& labelColor() const;

	/**
	 * \brief Returns whether the label is shown or not
	 *
	 * \return true if the label is shown
	 */
	bool labelShown() const;

	/**
	 * \brief Sets whether to show the label or not
	 *
	 * \param d if true shows the label
	 */
	void setLabelShown(bool d);

	/**
	 * \brief Returns the min and max points of the Axis-Aligned Bounding
	 *        Box (AABB)
	 *
	 * This simply calls the static version of the function. See WObject
	 * description for more information
	 * \param minPoint the minimum point of the AABB
	 * \param maxPoint the maximum point of the AABB
	 * \param tm the frame of reference in which the AABB is to be computed
	 */
	virtual void calculateAABB(wVector& minPoint, wVector& maxPoint, const wMatrix& tm)
	{
		calculateAABB(m_shared, minPoint, maxPoint, tm);
	}

	/**
	 * \brief Returns the dimension of the Minimum Bounding Box (MBB) of the
	 *        object
	 *
	 * This simply calls the static version of the function. See the
	 * description of WObject and of the static calculateMBBSize() function
	 * for more information
	 * \param sharedData the object with data from WObject
	 * \return the size of the MBB
	 */
	virtual wVector calculateMBBSize() const
	{
		return calculateMBBSize(m_shared);
	}

protected:
	/**
	 * \brief The virtual function called before the transformation matrix
	 *        is changed
	 *
	 * This function can return false to prevent matrix changes. The default
	 * implementation return true
	 * \return true if the matrix can be changed, false if not
	 */
	virtual bool aboutToChangeMatrix();

	/**
	 * \brief The virtual function called when the transformation matrix
	 *        changes
	 *
	 * The default implementation does nothing
	 */
	virtual void changedMatrix();

private:
	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief World is friend to be able to create and destroy this object
	 */
	friend class World;

private:
	/**
	 * \brief Copy constructor
	 *
	 * Here to disallow usage
	 */
	WObject(const WObject& other);

	/**
	 * \brief Copy operator
	 *
	 * Here to disallow usage
	 */
	WObject& operator=(const WObject& other);
};

/**
 * \brief A macro to implement the virtual version of the calculateAABB() and
 *        calculateMBBSize() function in RenderWObject subclasses (see
 *        RenderWObject description for more information)
 */
#define SALSA_IMPLEMENT_VIRTUAL_RENDERER_BB(ClassName) virtual void calculateAABB(const ClassName::Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const\
                                                       {\
                                                       	ClassName::calculateAABB(sharedData, minPoint, maxPoint, tm);\
                                                       }\
                                                       virtual wVector calculateMBBSize(const ClassName::Shared* sharedData) const\
                                                       {\
                                                       	return ClassName::calculateMBBSize(sharedData);\
                                                       }

/**
 * \brief The class rendering the WObject
 *
 * This adds few function to the implementation of RenderWEntity. The
 * drawLabel() and drawAxes() functions should be called inside render() to draw
 * the label and local axes respectively (the functions check if drawing of
 * those elements is required or not). You should also re-implement the
 * calculateAABB() and calculateMBBSize() functions to return the AABB and MBB
 * of the object. You can simply call the static functions in the class to
 * render, like in the example below:
 *
 * \code
 * class RenderWObjectSubclass : public RenderWObject
 * {
 * public:
 * 	...
 *
 * 	virtual void calculateAABB(const WObjectSubclass::Shared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const
 * 	{
 * 		WObjectSubclass::calculateAABB(sharedData, minPoint, maxPoint, tm);
 * 	}
 *
 * 	virtual wVector calculateMBBSize(const ClassName::Shared* sharedData) const
 * 	{
 * 		return WObjectSubclass::calculateMBBSize(sharedData);
 * 	}
 *
 * 	...
 * };
 * \endcode
 *
 * The SALSA_IMPLEMENT_VIRTUAL_RENDERER_BB convenience macro implements the
 * virtual functions inline as in the above example. The only parameter is the
 * name of the class that is rendered. To use the macro, however, the rendered
 * class must have been already defined (you can use a forward declaration of
 * the renderer class to add the typedefs in the rendered object). In the
 * example above you could have used the macro like this:
 * SALSA_IMPLEMENT_VIRTUAL_RENDERER_BB(WObjectSubclass)
 */
class SALSA_WSIM_API RenderWObject : public RenderWEntity
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
	RenderWObject(const WObject* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderWObject();

	/**
	 * \brief The function rendering the Axis-Aligned Bounding Box (AABB)
	 *
	 * Draws the AABB of the object in the world frame of reference. You
	 * should never need to reimplement this function, here we call
	 * calculateAABB() and then render it as a wirebox
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void renderAABB(const WObjectShared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief Returns the min and max points of the Axis-Aligned Bounding
	 *        Box (AABB, aligned to axes of the given frame of reference)
	 *
	 * See WObject::calculateAABB() for more information
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param minPoint the minimum point of the AABB
	 * \param maxPoint the maximum point of the AABB
	 * \param tm the frame of reference in which the AABB is to be computed
	 */
	virtual void calculateAABB(const WObjectShared* sharedData, wVector& minPoint, wVector& maxPoint, const wMatrix& tm) const
	{
		WObject::calculateAABB(sharedData, minPoint, maxPoint, tm);
	}

	/**
	 * \brief Returns the dimension of the Minimum Bounding Box (MBB) of the
	 *        object
	 *
	 * See WObject::calculateMBBSize() for more information
	 * \param sharedData the object with data from WObject
	 * \return the size of the MBB
	 */
	virtual wVector calculateMBBSize(const WObjectShared* sharedData) const
	{
		return WObject::calculateMBBSize(sharedData);
	}

	/**
	 * \brief Draws the label of the object if it has to
	 *
	 * This only draws the label if the sharedData->labelDrawn is true. This
	 * method changes the current OpenGL color. You don't need to
	 * re-implement this for subclasses unless you have particular
	 * needings
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void drawLabel(const WObjectShared* sharedData, GLContextAndData* contextAndData);

	/**
	 * \brief Draws the local axes of the object if it has to
	 *
	 * This draws the axes of the local frame of reference. You don't need
	 * to re-implement this for subclasses unless you have particular
	 * needings
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void drawAxes(const WObjectShared* sharedData, GLContextAndData* contextAndData);
};

} // end namespace salsa

#endif
