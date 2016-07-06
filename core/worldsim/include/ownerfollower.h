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

#ifndef OWNERFOLLOWER_H
#define OWNERFOLLOWER_H

#include "worldsimconfig.h"
#include "wobject.h"

namespace farsa {

class World;
class RenderOwnerFollower;

/**
 * \brief The shared data for the OwnerFollower
 */
class FARSA_WSIM_TEMPLATE OwnerFollowerShared : public WObjectShared
{
public:
	/**
	 * \brief Constructor
	 *
	 * This initializes all fields to the default value
	 */
	OwnerFollowerShared()
		: WObjectShared()
		, displacement(wMatrix::identity())
	{
	}

	/**
	 * \brief The displacement matrix
	 */
	wMatrix displacement;
};

/**
 * \brief A WObject following its owner
 *
 * This class sets its matrix equal to the one of its owner in the postUpdate()
 * function, if the owner is a WObject. You can also set a displacement matrix
 * in the frame of reference of the owner which is in the shared data (and thus
 * available to the renderer; the renderer has also methods to obtain the final
 * matrix which results from the displacement). This is useful e.g. if you want
 * to create purely graphical objects which are "attached" to another WObject
 * (such as those in graphicalmarkers.h). If this object has no owner or the
 * owner is not a WObject, its matrix is not changed and you can set it
 * directly. If the ownwer is present and is a WObject you should not set
 * the matrix of this object directly (it will be changed in the postUpdate()
 * function)
 */
class FARSA_WSIM_API OwnerFollower : public WObject
{
public:
	/**
	 * \brief The class with shared data
	 */
	typedef OwnerFollowerShared Shared;

	/**
	 * \brief The class for rendering
	 */
	typedef RenderOwnerFollower Renderer;

protected:
	/**
	 * \brief Constructor
	 *
	 * \param world the world where object lives
	 * \param shared the object with shared data. It is created externally
	 *               and must not be destroyed (it is deleted after this
	 *               object has been deleted)
	 * \param disp the displacement matrix
	 * \param name the name of the object
	 * \param tm rotation and position in global coordinate frame
	 */
	OwnerFollower(World* world, SharedDataWrapper<Shared> shared, const wMatrix& disp = wMatrix::identity(), QString name = "unamed",
const wMatrix& tm = wMatrix::identity());

	/**
	 * \brief Destructor
	 */
	virtual ~OwnerFollower();

public:
	/**
	 * \brief Sets the displacement matrix relative to the owner
	 *
	 * \param displacement the displacement matrix relative to the owner
	 */
	void setDisplacement(const wMatrix& displacement);

	/**
	 * \brief Returns the displacement matrix relative to the owner
	 *
	 * \return the displacement matrix relative to the owner
	 */
	const wMatrix& getDisplacement() const
	{
		return m_shared->displacement;
	}

	/**
	 * \brief The method called at each step of the world just after the
	 *        physic update
	 *
	 * This copies the transformation matrix of the owner. If you need to
	 * re-implement this function in subclasses, remember to call the parent
	 * function
	 */
	virtual void postUpdate();

protected:
	/**
	 * \brief Returns the owner as a WObject, if it is a WObject
	 *
	 * \return the owner as a WObject, if it is a WObject, otherwise NULL
	 */
	WObject* ownerWObject()
	{
		if (owner() != NULL) {
			return dynamic_cast<WObject*>(owner());
		} else {
			return NULL;
		}
	}

	/**
	 * \brief Returns the owner as a WObject, if it is a WObject (const
	 *        version)
	 *
	 * \return the owner as a WObject, if it is a WObject, otherwise NULL
	 */
	const WObject* ownerWObject() const
	{
		if (owner() != NULL) {
			return dynamic_cast<const WObject*>(owner());
		} else {
			return NULL;
		}
	}

private:
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
 * \brief The class rendering the OwnerFollower
 *
 * The implementation of functions in this class is mostly empty.
 */
class FARSA_WSIM_API RenderOwnerFollower : public RenderWObject
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
	RenderOwnerFollower(const OwnerFollower* entity);

	/**
	 * \brief Destructor
	 *
	 * Here just to declare it as virtual
	 */
	virtual ~RenderOwnerFollower();

protected:
	/**
	 * \brief Returns the final matrix of this object
	 *
	 * The returned matrix is the transformation in the world frame of
	 * reference after the displacement has been applied
	 * \return the final matrix
	 */
	wMatrix finalMatrix(const OwnerFollowerShared* sharedData);
};

}

#endif
