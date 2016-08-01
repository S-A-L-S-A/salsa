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

#ifndef PHYELLIPSOID_H
#define PHYELLIPSOID_H

#include "phyobject.h"

namespace salsa {

class RenderPhyEllipsoid;

/**
 * \brief The shared data for the PhyEllipsoid
 */
class SALSA_WSIM_TEMPLATE PhyEllipsoidShared : public PhyObjectShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PhyEllipsoidShared()
		: PhyObjectShared()
		, radiusX(0.0)
		, radiusY(0.0)
		, radiusZ(0.0)
	{
	}

	/**
	 * \brief The the radius along the local X axis
	 */
	real radiusX;

	/**
	 * \brief The the radius along the local Y axis
	 */
	real radiusY;

	/**
	 * \brief The the radius along the local Z axis
	 */
	real radiusZ;
};

/**
 * \brief The class modelling a physical ellipsoid
 */
class SALSA_WSIM_API PhyEllipsoid : public PhyObject
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyEllipsoidShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyEllipsoid Renderer;

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
	 * \param radiusX dimension of the radius along the X axis
	 * \param radiusY dimension of the radius along the Y axis
	 * \param radiusZ dimension of the radius along the Z axis
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PhyEllipsoid(World* world, SharedDataWrapper<Shared> shared, real radiusX, real radiusY, real radiusZ, QString name = "unamed", const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~PhyEllipsoid();

public:
	/**
	 * \brief Returns the radius along the X axis of the object
	 *
	 * \return the radius along the X axis of the object
	 */
	real radiusX() const
	{
		return m_shared->radiusX;
	}

	/**
	 * \brief Returns the radius along the Y axis of the object
	 *
	 * \return the radius along the Y axis of the object
	 */
	real radiusY() const
	{
		return m_shared->radiusY;
	}

	/**
	 * \brief Returns the radius along the Z axis of the object
	 *
	 * \return the radius along the Z axis of the object
	 */
	real radiusZ() const
	{
		return m_shared->radiusZ;
	}

	// Adding code for virtual bounding box functions
	SALSA_IMPLEMENT_VIRTUAL_BB

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
 * \brief The class rendering the PhyEllipsoid
 */
class SALSA_WSIM_API RenderPhyEllipsoid : public RenderPhyObject
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
	RenderPhyEllipsoid(const PhyEllipsoid* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyEllipsoid();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const PhyEllipsoidShared* sharedData, GLContextAndData* contextAndData);

	// Adding code for virtual bounding box functions
	SALSA_IMPLEMENT_VIRTUAL_RENDERER_BB(PhyEllipsoid)
};

} // end namespace salsa

#endif
