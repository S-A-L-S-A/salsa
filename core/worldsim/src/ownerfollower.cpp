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

#include "ownerfollower.h"
#include "world.h"

namespace salsa {

OwnerFollower::OwnerFollower(World* world, SharedDataWrapper<Shared> shared, const wMatrix& disp, QString name, const wMatrix& tm)
	: WObject(world, shared, name, tm)
	, m_shared(shared)
{
	m_shared.getModifiableShared()->displacement = disp;
}

OwnerFollower::~OwnerFollower()
{
	// Nothing to do here
}

void OwnerFollower::setDisplacement(const wMatrix& displacement)
{
	m_shared.getModifiableShared()->displacement = displacement;
}

void OwnerFollower::postUpdate()
{
	if (owner() != nullptr) {
		const WObject* const wObjectOwner = dynamic_cast<WObject*>(owner());
		if (wObjectOwner != nullptr) {
			setMatrix(wObjectOwner->matrix());
		}
	}
}

RenderOwnerFollower::RenderOwnerFollower(const OwnerFollower* entity)
	: RenderWObject(entity)
{
}

RenderOwnerFollower::~RenderOwnerFollower()
{
	// Nothing to do here
}

wMatrix RenderOwnerFollower::finalMatrix(const OwnerFollowerShared* sharedData)
{
	return sharedData->displacement * sharedData->tm;
}

}
