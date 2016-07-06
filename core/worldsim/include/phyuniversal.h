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

#ifndef PHYUNIVERSAL_H
#define PHYUNIVERSAL_H

#include "worldsimconfig.h"
#include "world.h"
#include "phyobject.h"
#include "wvector.h"
#include "wmatrix.h"
#include "wquaternion.h"
#include "phyjoint.h"

namespace farsa {

class RenderPhyUniversal;

/**
 * \brief The shared data for a PhyUniversal
 */
class FARSA_WSIM_TEMPLATE PhyUniversalShared : public PhyJointShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default values
	 */
	PhyUniversalShared()
		: PhyJointShared()
		, localMatrixParent()
		, localMatrixChild()
		, globalMatrixParent()
		, globalMatrixChild()
		, forceOnJoint(0.0, 0.0, 0.0)
	{
	}

	/**
	 * \brief The local frame of the joint respect to the parent matrix
	 *
	 * This matrix is calculated from the axis and centre passed to
	 * constructor of PhyUniversal. Axis will became the Z-axis of local
	 * frame (axis of rotation), and the two others axes are calculated in
	 * order to get a local frame. Centre is copied as it is into w_pos
	 */
	wMatrix localMatrixParent;

	/**
	 * \brief The local frame of the joint respect to the child matrix
	 */
	wMatrix localMatrixChild;

	/**
	 * \brief The global frame of the joint respect to parent
	 *
	 * This represent the local frame of joint in global coordinate starting
	 * from localMatrixParent representation.
	 * \note: The differences between globalMatrixParent and
	 *        globalMatrixChild are due to numerical error of the joint
	 *        solver of the physic engine used.
	 */
	wMatrix globalMatrixParent;

	/**
	 * \brief The global frame of the joint respect to child
	 *
	 * This represent the local frame of joint in global coordinate starting
	 * from localMatrixChild representation.
	 */
	wMatrix globalMatrixChild;

	/**
	 * \brief The force applied to this joint
	 */
	wVector forceOnJoint;
};

/**
 * \brief PhyUniversal class
 *
 * This class models a universal joint (also known as Cardan joint). This joint
 * adds two degrees of freedom around two axes
 */
class FARSA_WSIM_API PhyUniversal : public PhyJoint
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef PhyUniversalShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderPhyUniversal Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * The local frame is translated toward centre and the Z axis is aligned
	 * with the first axis and the X axis is aligned with the second Axis
	 * \param world the world where entity lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               entity has been deleted)
	 * \param parent the parent object. The local frame is translated
	 *               towards centre to create the complete local coordinate
	 *               frame of this joint
	 * \param child the child object
	 * \param firstAxis the first axis of rotation (respect to parent object
	 *                  frame)
	 * \param secondAxis the second axis of rotation (respect to parent object
	 *                   frame, MUST be ortogonal respect to firstAxis)
	 * \param centre the center of wheel rotation (respect to parent object
	 *               frame)
	 * \param name the name of the object
	 */
	PhyUniversal(World* world, SharedDataWrapper<Shared> shared, PhyObject* parent, PhyObject* child, const wVector& firstAxis, const wVector& secondAxis, const wVector& centre, QString name = "unamed");

	/**
	 * \brief Destructor
	 */
	virtual ~PhyUniversal();

public:
	/**
	 * \brief Returns the centre of this joint in world coordinate
	 *
	 * \return the centre of this joint in world coordinate
	 */
	virtual wVector centre() const;

	/**
	 * \brief Returns the force applied to this joint
	 *
	 * \return the force applied to this joint
	 */
	virtual wVector getForceOnJoint() const;

	/**
	 * \brief Updates the joint information
	 *
	 * All PhyDOFs will be updated
	 */
	virtual void updateJointInfo();

protected:
	/**
	 * \brief Creates the objects needed by the underlying physics engine
	 *
	 * This is called by world after the joint has been constructed (so
	 * that the correct virtual function is called)
	 */
	virtual void createPrivateJoint();

	/**
	 * \brief Updates the joint (called by Newton callback)
	 *
	 * \param timestep the size of the timestep
	 */
	virtual void updateJoint(real timestep);

	/**
	 * \brief The shared data
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
 * \brief The class rendering the PhyUniversal
 */
class FARSA_WSIM_API RenderPhyUniversal : public RenderPhyJoint
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
	RenderPhyUniversal(const PhyUniversal* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderPhyUniversal();

	/**
	 * \brief The function performing the rendering
	 *
	 * \param sharedData the object with data from PhyObject to use for
	 *                   rendering
	 * \param contextAndData the OpenGL context and shared data (e.g.
	 *                       textures)
	 */
	void render(const PhyUniversalShared* sharedData, GLContextAndData* contextAndData);
};

} // end namespace farsa

#endif
