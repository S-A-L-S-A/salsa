/***************************************************************************
 *  FARSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Gianluca Massera <emmegian@yahoo.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#include "configurationmanager.h"
#include "componentresource.h"
#include "configurationnode.h"

namespace farsa {

ResourceAccessor::ConfigurationManagerLocker::ConfigurationManagerLocker(const std::auto_ptr<ConfigurationManager>& manager)
	: m_manager(manager.get())
{
	m_manager->m_shared->mutex.lock();
}

ResourceAccessor::ConfigurationManagerLocker::~ConfigurationManagerLocker()
{
	m_manager->m_shared->mutex.unlock();
}

ResourceAccessor::ResourceAccessor(ConfigurationManager& params, const ConfigurationNode* configurationNode)
	: m_configurationNodeForContext(configurationNode)
	, m_confManager(new ConfigurationManager(params))
{
}

ResourceAccessor::~ResourceAccessor()
{
}

QList<Component*> ResourceAccessor::getResourcesOwners(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	return resources().value(name).keys();
}

int ResourceAccessor::resourcesCount(QString name) const
{
	ConfigurationManagerLocker locker(m_confManager);

	return resources().value(name).size();
}

bool ResourceAccessor::resourceExists(QString name) const
{
	ConfigurationManagerLocker locker(m_confManager);

	return (resourcesCount(name) != 0);
}

bool ResourceAccessor::resourceExists(QString name, Component* owner) const
{
	ConfigurationManagerLocker locker(m_confManager);

	return (getResourceHandlerWithOwner(name, owner) != NULL);
}

QList<ResourceHandler*> ResourceAccessor::getAllCandidateResourceHandlers(QString name)
{
	// Trying to retrieve the resource
	QList<ResourceHandler*> resList = resources().value(name).values();

	QList<ResourceHandler*> candidates;

	if (resList.isEmpty()) {
		throw ResourceNotDeclaredException(name.toLatin1().data());
	} else if (resList.size() == 1) {
		candidates.append(resList[0]);
	} else {
		candidates = getNearestResourceHandlers(resList);
	}

	return candidates;
}

QList<ResourceHandler*> ResourceAccessor::getNearestResourceHandlers(QList<ResourceHandler*> list) const
{
	QList<ResourceHandler*> nearest;

	int minDistance = m_configurationNodeForContext->getDistance(list[0]->owner()->m_configurationNodeForContext);
	nearest.append(list[0]);

	for (int i = 1; i < list.size(); ++i) {
		ResourceHandler* const curHandler = list[i];
		const int distance = m_configurationNodeForContext->getDistance(curHandler->owner()->m_configurationNodeForContext);

		if (distance < minDistance) {
			nearest.clear();
			nearest.append(curHandler);
			minDistance = distance;
		} else if (distance == minDistance) {
			nearest.append(curHandler);
		}
	}

	return nearest;
}

const QMap<QString, QMap<Component*, ResourceHandler*> >& ResourceAccessor::resources() const
{
	return m_confManager->m_shared->resources;
}

QMap<QString, QMap<Component*, ResourceHandler*> >& ResourceAccessor::resources()
{
	return m_confManager->m_shared->resources;
}

ResourceHandler* ResourceAccessor::getResourceHandlerWithOwner(QString name, Component* owner) const
{
	ResourceHandler* h = resources().value(name).value(owner, NULL);

	return h;
}

ResourceHandler::ResourceHandler(QString name, Component* owner)
	: m_name(name)
	, m_owner(owner)
	, m_type(t_nonExistent)
	, m_notifees()
{
}

ResourceHandler::~ResourceHandler()
{
	notifyAll(ResourceDeleted);
}

void ResourceHandler::set(int* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_int;
	m_pointers.intp = res;
	notifyAll(changeType);
}

void ResourceHandler::set(float* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_float;
	m_pointers.floatp = res;
	notifyAll(changeType);
}

void ResourceHandler::set(double* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_double;
	m_pointers.doublep = res;
	notifyAll(changeType);
}

void ResourceHandler::set(bool* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_bool;
	m_pointers.boolp = res;
	notifyAll(changeType);
}

void ResourceHandler::set(Resource* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_resource;
	m_pointers.res = res;
	notifyAll(changeType);
}

void ResourceHandler::set(Component* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_component;
	m_pointers.par = res;
	notifyAll(changeType);
}

void ResourceHandler::set(QObject* res)
{
	const ResourceChangeType changeType = (m_type == t_nonExistent) ? ResourceCreated : ResourceModified;
	m_type = t_qObject;
	m_pointers.obj = res;
	notifyAll(changeType);
}

void ResourceHandler::declareAsNull()
{
	m_type = t_declaredNull;
	notifyAll(ResourceDeclaredAsNull);
}

void ResourceHandler::addNotifee(ResourceChangeNotifee* n)
{
	if (m_notifees.contains(n)) {
		return;
	}

	m_notifees.insert(n);

	if (m_type != t_nonExistent) {
		notifyOne(n, ResourceCreated);
	}
}

void ResourceHandler::removeNotifee(ResourceChangeNotifee* n)
{
	if (!m_notifees.contains(n)) {
		return;
	}

	m_notifees.remove(n);

	if (m_type != t_nonExistent) {
		notifyOne(n, ResourceDeleted);
	}
}

bool ResourceHandler::isNotifee(ResourceChangeNotifee* n) const
{
	return m_notifees.contains(n);
}

void ResourceHandler::notifyAll(ResourceChangeType changeType)
{
	foreach (ResourceChangeNotifee *n, m_notifees) {
		notifyOne(n, changeType);
	}
}

void ResourceHandler::notifyOne(ResourceChangeNotifee* notifee, ResourceChangeType changeType)
{
	class ResourceChangeNotifeeResourceHandlerRAII
	{
	public:
		ResourceChangeNotifeeResourceHandlerRAII(ResourceChangeNotifee* notifee, ResourceHandler* handler)
			: m_notifee(notifee)
		{
			m_notifee->m_notifiedResourceHandler = handler;
		}

		~ResourceChangeNotifeeResourceHandlerRAII()
		{
			m_notifee->m_notifiedResourceHandler = NULL;
		}

	private:
		ResourceChangeNotifee* const m_notifee;
	};

	ResourceHandler* handlerToSet = (changeType == ResourceDeleted) ? NULL : this;
	ResourceChangeNotifeeResourceHandlerRAII raii(notifee, handlerToSet);
	notifee->resourceChanged(name(), owner(), changeType);
}

} // end namspace farsa
