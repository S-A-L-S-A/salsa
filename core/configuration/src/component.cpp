/***************************************************************************
 *  SALSA Configuration Library                                            *
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
#include "componentdescriptors.h"
#include "component.h"


#warning SEE THE COMMENTS BELOW
// Aggiungere classe ResourceHolder che mantiene una risorsa (il puntatore) e lo aggiorna automaticamente. Per quanto riguarda la notifica ai ResourceHolder, usare un meccanismo parallelo a resourceChanged. Inoltre, forse, fare in modo che i ResourceHolder vengano creati e restituiti da una funzione di questa classe (servirebbe avere la move-semantic, ma non possiamo averla, quindi forse meglio condividerli). Inoltre deve essere possibile (ma non obbligatorio) aggiungere un lock sulla risorsa (vedi nota sul multithreading nella descrizione di questa classe)

// USARE SEMPRE salsa::real INVECE DI double. IL PROBLEMA È CHE salsa::real È IN UTILITIES E NOI NON POSSIAMO DIPENDERE DA UTILITIES

// NEL COMPONENT UI O QUI DENTRO FARE IN MODO DI DEFINIRE UNA SERIE DI AZIONI, CHE POI FINISCONO ANCHE NEL MENU - FORSE NO, FARLO AD UN LIVELLO PIÙ ALTO CON BaseExperiment

#warning SEVERE PROBLEM: IF A PARENT CONFIGURES ITSELF IN THE CONSTRUCTOR AND A CHILD IN CONFIGURE, THE CHECK FOR DEPENDENCY CYCLES DOES NOT WORK, BECAUSE WE THINK THAT THE COMPONENT CONFIGURES ITSELF IN configure() BUT PART OF THE CONFIGURATION IS IN THE CONSTRUCTOR!!! WE SHOULD EITHER MAKE THE CHOICE IN THE FIRST Component SUBCLASS OR REMOVE THE CHIOCE ALTOGETHER AND ONLY ALLOW CONFIGURATION WITH CONFIGURE (PERHAPS THIS IS BETTER). PERHAPS WE CAN MAKE DEPENDENCIES BETWEEN COMPONENTS MORE EXPLICIT AND USE DependencySorter TO DECIDE THE ORDER OF CONFIGURATION...

namespace salsa {

Component::Component(ConfigurationManager& params)
	: ResourceAccessor(params, params.getConfigurationNodeForCurrentComponent())
	, m_type(params.getTypeForCurrentComponent())
	, m_configurationNode(params.getConfigurationNodeForCurrentComponent())
	, m_prefix(params.getPrefixForCurrentComponent())
{
	// Telling the ConfigurationManager that the object for the group in prefix is about
	// to be created
	m_confManager->setComponentFromGroupStatusToCreating(m_prefix, this);

	// Declaring self as a resource of ours
	declareResource(ConfigurationNode::separateLastElement(confPath()).element, this);
}

Component::~Component()
{
	// Telling the configuration manager we are being destroyed
	m_confManager->notifyObserversOfComponentDestruction(this);

	// Deleting all notifications
	removeAllNotifiedResources();

	// Deleting all resources
	deleteAllResources();

	// Removing association with configuration node
	m_configurationNode->setComponentForNode("", NULL, ComponentNotCreated);
}

void Component::describe(RegisteredComponentDescriptor& d)
{
	d.help("Describe is missing");
}

void Component::declareResourceAsNull(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	ResourceHandler* h = getResourceHandlerForOurResource(name);

	// Now setting the resource to NULL. If this throws an exception we could have simply created a non-existing
	// resource above that doesn't modify the external behaviour of this and other classes (so we can say
	// that this function is strongly exception-safe)
	h->declareAsNull();
}

void Component::deleteResource(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	ResourceHandler* h = getResourceHandlerWithOwner(name, this);

	if (h == NULL) {
		throw ResourceNotDeclaredException(name.toLatin1().data());
	}

	delete h;
	if (Q_UNLIKELY(resources()[name].remove(this) != 1)) {
		qFatal("Internal error, not one single resource with the given name for one owner");
	}
}

void Component::deleteAllResources()
{
	ConfigurationManagerLocker locker(m_confManager);

	// This is not the most efficient algorithm, but for a limited number of different
	// resource names it should be quick enough. Here we also remove owner-resource
	// maps that become empty
	QMap<QString, QMap<Component*, ResourceHandler*> >::iterator it = resources().begin();
	while (it != resources().end()) {
		QMap<Component*, ResourceHandler*>::iterator ownerResourceIt = it.value().find(this);

		if (ownerResourceIt != it.value().end()) {
			// This will not call resourceChanged of self if this function is
			// called by the destructor because when we get here the child classes
			// have already been destroyed and their vtable removed
			delete ownerResourceIt.value();
			it.value().erase(ownerResourceIt);
		}

		if (it.value().isEmpty()) {
			it = resources().erase(it);
		} else {
			++it;
		}
	}
}

void Component::addNotifiedResource(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	// If in configuration phase, we only record the request
	if (inConfigurationPhase()) {
		recordRequestOfResourceChangeNotification(name, NULL);

		return;
	}

	QList<ResourceHandler*> candidates = getAllCandidateResourceHandlers(name);

	if (candidates.size() == 0) {
		throw ResourceNotDeclaredException(name.toLatin1().data());
	} else if (candidates.size() != 1) {
		throw ResourceCannotResolveAmbiguityException(name.toLatin1().data(), candidates.size());
	}

	candidates[0]->addNotifee(this);
}

void Component::addNotifiedResource(QString name, Component* owner)
{
	ConfigurationManagerLocker locker(m_confManager);

	// If in configuration phase, we only record the request
	if (inConfigurationPhase()) {
		recordRequestOfResourceChangeNotification(name, owner);

		return;
	}

	// Getting the resource handler. It will be created if doesn't exists
	ResourceHandler* h = owner->getResourceHandlerForOurResource(name);

	h->addNotifee(this);
}

void Component::removeNotifiedResource(QString name)
{
	ConfigurationManagerLocker locker(m_confManager);

	// If in configuration phase, we remove the recorded notification if it exists
	if (inConfigurationPhase()) {
		removeRequestOfResourceChangeNotification(name, NULL);

		return;
	}

	QList<ResourceHandler*> candidates = getAllCandidateResourceHandlers(name);

	if (candidates.size() == 0) {
		throw ResourceNotDeclaredException(name.toLatin1().data());
	} else if (candidates.size() != 1) {
		throw ResourceCannotResolveAmbiguityException(name.toLatin1().data(), candidates.size());
	}

	candidates[0]->removeNotifee(this);
}

void Component::removeNotifiedResource(QString name, Component* owner)
{
	ConfigurationManagerLocker locker(m_confManager);

	// If in configuration phase, we remove the recorded notification if it exists
	if (inConfigurationPhase()) {
		removeRequestOfResourceChangeNotification(name, owner);

		return;
	}

	// Getting the resource handler. It will be created if doesn't exists
	ResourceHandler* h = getResourceHandlerWithOwner(name, owner);

	if (h != NULL) {
		h->removeNotifee(this);
	}
}

void Component::removeAllNotifiedResources()
{
	ConfigurationManagerLocker locker(m_confManager);

	// If in configuration phase, we remove the recorded notification if it exists
	if (inConfigurationPhase()) {
		removeAllOurRequestOfResourceChangeNotification();

		return;
	}

#warning THIS IS VERY INEFFICIENT, CHANGE IF POSSIBLE
	for (QMap<QString, QMap<Component*, ResourceHandler*> >::iterator it = resources().begin(); it != resources().end(); ++it) {
		for (QMap<Component*, ResourceHandler*>::iterator rit = it.value().begin(); rit != it.value().end(); ++rit) {
			rit.value()->removeNotifee(this);
		}
	}
}

ResourceHandler* Component::getResourceHandlerForOurResource(QString name)
{
	ResourceHandler* h = getResourceHandlerWithOwner(name, this);

	if (h == NULL) {
		// Adding an non-existant resource (in an exception-safe way)
		std::auto_ptr<ResourceHandler> hp(new ResourceHandler(name, this));
		resources()[name][this] = hp.get();
		h = hp.release();
	}

	return h;
}

bool Component::inConfigurationPhase() const
{
	return (m_confManager->m_shared->getComponentFromGroupRecursionLevel != 0);
}

void Component::recordRequestOfResourceChangeNotification(QString resourceName, Component* owner)
{
	m_confManager->m_shared->requestedNotifications.append(ConfigurationManager::RegisteredRequestForResourceChangeNotification(this, resourceName, owner));
}

void Component::removeRequestOfResourceChangeNotification(QString resourceName, Component* owner)
{
	m_confManager->m_shared->requestedNotifications.removeAll(ConfigurationManager::RegisteredRequestForResourceChangeNotification(this, resourceName, owner));
}

void Component::removeAllOurRequestOfResourceChangeNotification()
{
	QList<ConfigurationManager::RegisteredRequestForResourceChangeNotification>::iterator it = m_confManager->m_shared->requestedNotifications.begin();
	while (it != m_confManager->m_shared->requestedNotifications.end()) {
		if (it->notifee == this) {
			it = m_confManager->m_shared->requestedNotifications.erase(it);
		} else {
			++it;
		}
	}
}

} // end namespace salsa
