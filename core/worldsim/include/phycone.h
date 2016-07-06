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

#ifndef PHYCONE_H
#define PHYCONE_H

#include "phyobject.h"

namespace farsa {

class RenderPhyCone;

/**
 * \brief The shared data for the PhyCone
 */
class FARSA_WSIM_TEMPLATE PhyConeShared : public PhyObjectShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyConeShared()
		: PhyObjectShared()
		, radius(0.0)
		, height(0.0)
	{
	}

	/**
	 * \brief The radius of the cone
	 */
	real radius;

	/**
	 * \brief The height of the cone
	 */
	real height;
};

/**
 * \brief PhyCone class
 *
 * This represent a physical cone object. The local frame of reference has the x
 * axis along the line connecting the center of the base and the tip of the
 * cone and the center of the frame is in the middle of that line. The base is
 * in on the yz plane
 * \note Newton has a limitation regarding cone height: ti must be greater than
 *       or equal to the diameter of the base. If this is not the case, then the
 *       height will be clamped to the diameter.
 */
class FARSA_WSIM_API PhyCone : public PhyObject
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyConeShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyCone Renderer;

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
	 * \param radius the radius of the cone
	 * \param height the height of the cone
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PhyCone(World* world, SharedDataWrapper<Shared> shared, real radius, real height, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyCone();

public:
	/**
	 * \brief Returns the radius
	 *
	 * \return the radius
	 */
	real radius() const
	{
		return m_shared->radius;
	}

	/**
	 * \brief Returns the height
	 *
	 * \return the height
	 */
	real height() const
	{
		return m_shared->height;
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
	 * \brief World is friend to access m_priv and call both
	 *        createPrivateObject() and postCreatePrivateObject() and to
	 *        create instances
	 */
	friend class World;
};

/**
 * \brief The class rendering the PhyCone
 */
class FARSA_WSIM_API RenderPhyCone : public RenderPhyObject
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
	RenderPhyCone(const PhyCone* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyCone();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const PhyConeShared* sharedData, GLContextAndData* contextAndData);

	// Adding code for virtual bounding box functions
	FARSA_IMPLEMENT_VIRTUAL_RENDERER_BB(PhyCone)
};

} // end namespace farsa

#endif
