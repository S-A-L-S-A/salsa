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

#ifndef GRAPHICALMARKERS_H
#define GRAPHICALMARKERS_H

#include "worldsimconfig.h"
#include "world.h"
#include "ownerfollower.h"

/**
 * \file graphicalmarkers.h
 *
 * This file contains some ready-to-use graphical objects you can put in the
 * world or attach to other objects
 */

namespace salsa {

class RenderCircularGraphicalMarker;
class RenderPlanarArrowGraphicalMarker;

/**
 * \brief The shared data for the CircularGraphicalMarker
 */
class SALSA_WSIM_TEMPLATE CircularGraphicalMarkerShared : public OwnerFollowerShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	CircularGraphicalMarkerShared()
		: OwnerFollowerShared()
		, radius(0.0)
	{
	}

	/**
	 * \brief The radius of the disk
	 */
	real radius;
};

/**
 * \brief A graphical object displaying a circle
 *
 * The circle lies in the local z = 0 plane. It has a solid color (no light
 * effects)
 */
class SALSA_WSIM_API CircularGraphicalMarker : public OwnerFollower
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef CircularGraphicalMarkerShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderCircularGraphicalMarker Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param radius the radius of the disk
	 * \param disp the displacement matrix
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	CircularGraphicalMarker(World* world, SharedDataWrapper<Shared> shared, real radius, const wMatrix& disp = wMatrix::identity(), QString name = "unamed", const wMatrix& tm = wMatrix::identity());

private:
	/**
	 * \brief Destructor
	 */
	virtual ~CircularGraphicalMarker();

	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief World is friend to be able to create and destroy this object
	 */
	friend class World;
};

/**
 * \brief The class rendering the CircularGraphicalMarker
 *
 * We only implement the render function, we don't provide the rendering of the
 * AABB
 */
class SALSA_WSIM_API RenderCircularGraphicalMarker : public RenderOwnerFollower
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
	RenderCircularGraphicalMarker(const CircularGraphicalMarker* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderCircularGraphicalMarker();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const CircularGraphicalMarkerShared* sharedData, GLContextAndData* contextAndData);
};

/**
 * \brief The shared data for the PlanarArrowGraphicalMarker
 */
class SALSA_WSIM_TEMPLATE PlanarArrowGraphicalMarkerShared : public OwnerFollowerShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	PlanarArrowGraphicalMarkerShared()
		: OwnerFollowerShared()
		, arrowLength(0.0)
		, arrowTailWidth(0.0)
		, arrowHeadWidth(0.0)
		, arrowTailPortion(0.0)
	{
	}

	/**
	 * \brief The length of the arrow
	 */
	real arrowLength;

	/**
	 * \brief The width of the tail of the arrow
	 */
	real arrowTailWidth;

	/**
	 * \brief The width of the head of the arrow
	 */
	real arrowHeadWidth;

	/**
	 * \brief The percentage of the arrow occupied by its tail
	 *
	 * This is a value betwee 0.0 and 1.0
	 */
	real arrowTailPortion;
};

/**
 * \brief A graphical object displaying a planar arrow
 *
 * The arrow lies in the local z = 0 plane and points towards x > 0. The tail is
 * in the origin of the local frame of reference. It has a solid color (no light
 * effects).
 */
class SALSA_WSIM_API PlanarArrowGraphicalMarker : public OwnerFollower
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PlanarArrowGraphicalMarkerShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPlanarArrowGraphicalMarker Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param arrowLength the length of the arrow
	 * \param arrowTailWidth the width of the tail of the arrow
	 * \param arrowHeadWidth the width of the head of the arrow
	 * \param arrowTailPortion the percentage of the arrow occupied by its
	 *                         tail. This is a value betwee 0.0 and 1.0
	 * \param disp the displacement matrix
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	PlanarArrowGraphicalMarker(World* world, SharedDataWrapper<Shared> shared, real arrowLength, real arrowTailWidth, real arrowHeadWidth, real arrowTailPortion, const wMatrix& disp = wMatrix::identity(), QString name = "unamed", const wMatrix& tm = wMatrix::identity());

private:
	/**
	 * \brief Destructor
	 */
	virtual ~PlanarArrowGraphicalMarker();

	/**
	 * \brief The pointer to the object with shared data
	 */
	SharedDataWrapper<Shared> m_shared;

	/**
	 * \brief World is friend to be able to create and destroy this object
	 */
	friend class World;
};

/**
 * \brief The class rendering the PlanarArrowGraphicalMarker
 *
 * We only implement the render function, we don't provide the rendering of the
 * AABB
 */
class SALSA_WSIM_API RenderPlanarArrowGraphicalMarker : public RenderOwnerFollower
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
	RenderPlanarArrowGraphicalMarker(const PlanarArrowGraphicalMarker* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPlanarArrowGraphicalMarker();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from WObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const PlanarArrowGraphicalMarkerShared* sharedData, GLContextAndData* contextAndData);
};

}

#endif
