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

#include "configurationnode.h"
#include "configurationmanager.h"
#include "parametersfileloadersaver.h"
#include "component.h"
#include "configurationexceptions.h"
#include <QFileInfo>
#include <QQueue>
#include <QMutexLocker>
#include <memory>


#warning VEDERE QUESTA LISTA DI TODOs

// QUI CREARE UNA RISORSA PER OGNI OGGETTO CREATO TRAMITE LA FACTORY CON IL FILE DI CONFIGURAZIONE E DARE COME NOME IL NOME DEL GRUPPO (NON IL FULL PATH, SOLO IL NOME DEL GRUPPO, IL NOME DOPO L ULTIMO /). NELLA SOTTOCLASSI DI Component SI PUÒ PENSARE DI AGGIUNGERE ANCHE UN ALTRO NOME DI RISORSA, AD ESEMPIO PARI AL PARAMETRO name DEI ROBOT. GLI ALIAS, PERÒ, (NOME DIVERSO PER LA STESSA RISORSA) FORSE NECESSITANO CAMBIAMENTI NEL CODICE PERCHÈ QUANDO SI MODIFICA O CANCELLA UNA RISORSA SI DOVREBBERO CAMBIARE TUTTI GLI ALIAS (FORSE METTERE LA LISTA DEGLI ALIAS NEL ResourceHandler?)

#warning CONTROLLARE CHE getComponentFromGroup FUNZIONI CORRETTAMENTE ANCHE PER PRENDERE UN OGGETTO DOPO LA FASE DI CREAZIONE E CONFIGURAZIONE (AD ESEMPIO PER AVERE IL PARENT DI UN OGGETTO, DEVE ESSERE POSSIBILE SPECIFICARE CHE SE L OGGETTO NON ESISTE DEVE TORNARE NULL E NON CREARE NULLA) E/O AGGIUNGERE FUNZIONI CHE RITORNANO OGGETTI ASSOCIATI AD UN CONFIGURATION NODE E PERMETTANO DI NAVIGARE L ALBERO DEGLI OGGETTI/GRUPPI

#warning VEDI COMMENTO SOTTO
// SEGNARSI QUESTA COSA, È IMPORTANTE: NEL NUOVO GA (E PRESUMIBILMENTE ANCHE
// ALTROVE) NON È SEMPRE CHIARO CHI È L OWNER DI UN OGGETTO (NEL GA, AD ESEMPIO,
// Experiment NON È UN SOTTOTGRUPPO DI GA, E LO STESSO OGGETTO Experiment VIENE
// PRESO ANCHE DAI TEST). PER QUESTO È IMPORTANTE CHE CI SIA UN MECCANISMO PER IL
// DELETE DEGLI OGGETTI CHE FACCIA DUE COSE: QUANDO IL ConfigurationManager VIENE
// DISTRUTTO DISTRUGGE TUTTI GLI OGGETTI CREATI DA QUEL ConfigurationManager.
// INOLTRE CI DEVE ESSERE UNA CHIAMATA SPECULARE A getComponentFromGroup PER
// CANCELLARE GLI OGGETTI: USARE UN REFERENCE COUNTER SUGLI OGGETTI PER OGNI
// CHIAMATA A getComponentFromGroup E POI DECREMETNARE IL CONTATORE OGNI VOLTA CHE SI
// CHIAMA deleteComponentFromGroup FACENDO EFFETTIVAMENTE LA CANCELLAZIONE QUANDO IL
// COUNTER STA A ZERO (PENSARE PRIMA BENE QUEST ULTIMA COSA, NON È CHIARO SE VA
// BENE IL REFERENCE COUNTER COSÌ O SE AD ESEMPIO SI PUÒ RICORCORDARE
// L ASSOCIAZIONE DI UN OGGETTO CON L OGGETTO CHE NE HA RICHIESTO L ISTANZA TRAMITE
// getComponentFromGroup E QUINDI CANCELLARE L OGGETTO QUANDO TUTTI QUELLI CHE NE
// HANNO RICHIESTO UNA ISTANZA SONO STATI CANCELLATI - FORSE QUESTA SECONDA
// IPOTESI È MEGLIO)
// PER IL MOMENTO AGGIUNGERE SOLO LA CANCELLAZIONE ESPLICITA DI TUTTI GLI
// OGGETTI CREATI DA UN CONFIGURATION MANAGER. LA CHIAMATA SPECULARE A
// getComponentFromGroup NON FARLA PER IL MOMENTO, ASPETTARE QUANDO SERVE
// VERAMENTE. CONSIDERARE, PERÒ, COME IMPLEMENTARE COSE TIPO RI-CREARE L'ARENA O
// ALTRI COMPONENTI
// Per quanto riguarda la chiamata speculare a getComponentFromGroup si potrebbe
// fare tramite reference counter. Ogni component deve avere un reference counter
// che viene incrementato quando viene chiamata getComponentFromGroup/Parameter e
// decrementato quando si chiama releaseComponentFromGroup/Parameter. Quando arriva
// a zero il componente viene distrutto. Si potrebbe anche registrare
// automaticamente chi chiama getComponentFromGroup/Parameter (forse) e cancellare
// gli oggetti che sono stati richiesti da oggetti distrutti

namespace farsa {

bool ConfigurationManager::registerFileFormat(QString format, ParametersFileLoaderSaver *fileLoaderSaver, QString defaultExtension)
{
	// Checking whether the format already exists
	QMap<QString, FileFormat>::iterator it = getFormatsMap().find(format);
	if (it != getFormatsMap().end()) {
		return false;
	}

	// If defaultExtension is the empty string, changing it to format
	if (defaultExtension.isEmpty()) {
		defaultExtension = format;
	}

	// Filling the structure with format informations
	FileFormat f(format, fileLoaderSaver, defaultExtension);

	// Adding format to maps
	getFormatsMap().insert(format, f);
	getFileExtensionsMap().insert(defaultExtension, format);

	return true;
}

QMap<QString, ConfigurationManager::FileFormat>& ConfigurationManager::getFormatsMap()
{
	static QMap<QString, ConfigurationManager::FileFormat> formatsMap;

	return formatsMap;
}

QMap<QString, QString>& ConfigurationManager::getFileExtensionsMap()
{
	static QMap<QString, QString> fileExtensionsMap;

	return fileExtensionsMap;
}

ConfigurationManager::ConfigurationManager()
	: m_shared(new SharedData())
{
	m_shared->root.reset(new ConfigurationNode("root"));
}

ConfigurationManager::ConfigurationManager(const ConfigurationManager &other)
	: m_shared(other.m_shared)
{
}

ConfigurationManager& ConfigurationManager::operator=(const ConfigurationManager &other)
{
	// Not locking: copying the shared data is thread-safe

	// Checking for self-assignement
	if (&other == this) {
		return *this;
	}

	// Sharing data
	m_shared = other.m_shared;

	return *this;
}

ConfigurationManager::~ConfigurationManager()
{
	// Nothing to do here
}

void ConfigurationManager::clearAll()
{
	QMutexLocker locker(&(m_shared->mutex));

	destroyAllComponents();
	m_shared->root->clearAll();
}

void ConfigurationManager::destroyAllComponents()
{
	QMutexLocker locker(&(m_shared->mutex));

	recursivelyDestroyComponents(m_shared->root.get());
}

ConfigurationManager* ConfigurationManager::createDeepCopy(ConfigurationManager* dest)
{
	QMutexLocker locker(&(m_shared->mutex));

	// Allocating memory for the destination if necessary. The auto_ptr for the allocated object
	// is needed to stay exception-safe
	std::auto_ptr<ConfigurationManager> allocatedObj;
	if (dest == NULL) {
		dest = new ConfigurationManager();
		allocatedObj.reset(dest);
	} else {
		dest->m_shared = new SharedData();
		dest->m_shared->root.reset(new ConfigurationNode("root"));
	}

	// Recursively copying our root into dest
	m_shared->root->createDeepCopy(dest->m_shared->root.get());

	allocatedObj.release();
	return dest;
}

QStringList ConfigurationManager::getGroupsList(QString group) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->getNode(group)->getChildrenNamesList();
}

QStringList ConfigurationManager::getGroupsWithPrefixList(QString group, QString prefix) const
{
	// No need to lock, we call the other function here

	// Building the regular expression matching the given prefix
	QRegExp filter(QString("^") + QRegExp::escape(prefix), Qt::CaseSensitive);

	return getFilteredGroupsList(group, filter);
}

QStringList ConfigurationManager::getFilteredGroupsList(QString group, QRegExp filter) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->getNode(group)->getFilteredChildrenNamesList(filter);
}

void ConfigurationManager::createGroup(QString groupPath)
{
	QMutexLocker locker(&(m_shared->mutex));

	// Splitting path
	QStringList splittedPath = groupPath.split(GroupSeparator, QString::SkipEmptyParts, Qt::CaseSensitive);
	if (splittedPath.isEmpty()) {
		throw InvalidGroupNameException(groupPath.toLatin1().data());
	}

	// Now creating all nodes
	ConfigurationNode* lastNode = m_shared->root.get();
	for (QStringList::iterator it = splittedPath.begin(); it != splittedPath.end(); ++it) {
		if (!lastNode->hasChild(*it)) {
			lastNode = lastNode->addNode(*it);
		} else {
			lastNode = lastNode->getNode(*it);
		}
	}
}

QString ConfigurationManager::createSubGroup(QString parentPath, QString groupName)
{
	// No need to lock, we call the other function

	QString fullpath = parentPath + GroupSeparator + groupName;
	createGroup(fullpath);
	return fullpath;
}

bool ConfigurationManager::groupExists(QString groupPath) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->isPathValid(groupPath);
}

void ConfigurationManager::deleteGroup(QString groupPath)
{
	QMutexLocker locker(&(m_shared->mutex));

	// Extract the group name and the path
	ConfigurationNode::ElementAndPath ep = ConfigurationNode::separateLastElement(groupPath);

	m_shared->root->getNode(ep.elementPath)->deleteChild(ep.element);
}

void ConfigurationManager::renameGroup(QString oldGroupPath, QString newGroupName)
{
	QMutexLocker locker(&(m_shared->mutex));

	// Extract the group name and the path
	ConfigurationNode::ElementAndPath ep = ConfigurationNode::separateLastElement(oldGroupPath);

	m_shared->root->getNode(ep.elementPath)->renameChild(ep.element, newGroupName);
}

void ConfigurationManager::copyGroup(QString sourceGroup, QString destGroup)
{
	QMutexLocker locker(&(m_shared->mutex));

	ConfigurationNode::ElementAndPath ep = ConfigurationNode::separateLastElement(destGroup);
	createGroup(ep.elementPath);

	// Now calling duplicateNode. We need to make sure destPath is an absolute path
	m_shared->root->duplicateNode(sourceGroup, ep.element, GroupSeparator + ep.elementPath);
}

void ConfigurationManager::createParameter(QString groupPath, QString parameter)
{
	QMutexLocker locker(&(m_shared->mutex));

	ConfigurationNode* node = m_shared->root->getNode(groupPath);
	node->addParameter(parameter);
}

void ConfigurationManager::createParameter(QString groupPath, QString parameter, QString value)
{
	QMutexLocker locker(&(m_shared->mutex));

	createParameter(groupPath, parameter);
	setValue(groupPath + GroupSeparator + parameter, value);
}

bool ConfigurationManager::parameterExists(QString path) const
{
	QMutexLocker locker(&(m_shared->mutex));

	// Extract the group name and the parameter
	ConfigurationNode::ElementAndPath ep = ConfigurationNode::separateLastElement(path);
	if (groupExists(ep.elementPath)) {
		return m_shared->root->getNode(ep.elementPath)->parameterExists(ep.element);
	}

	return false;
}

void ConfigurationManager::deleteParameter(QString groupPath, QString parameter)
{
	QMutexLocker locker(&(m_shared->mutex));

	ConfigurationNode* node = m_shared->root->getNode(groupPath);
	node->deleteParameter(parameter);
}

QString ConfigurationManager::getValue(QString path) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->getValue(path);
}

QString ConfigurationManager::getValueAlsoMatchParents(QString path) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->getValueAlsoMatchParents(path);
}

void ConfigurationManager::setValue(QString path, QString value)
{
	QMutexLocker locker(&(m_shared->mutex));

	m_shared->root->setValue(path, value);
}

QStringList ConfigurationManager::getParametersList(QString group) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->getNode(group)->getParametersList();
}

QStringList ConfigurationManager::getParametersWithPrefixList(QString group, QString prefix) const
{
	// No need to lock, we call the other function here

	// Building the regular expression matching the given prefix
	QRegExp filter(QString("^") + QRegExp::escape(prefix), Qt::CaseSensitive);

	return getFilteredParametersList(group, filter);
}

QStringList ConfigurationManager::getFilteredParametersList(QString group, QRegExp filter) const
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->root->getNode(group)->getFilteredParametersList(filter);
}

bool ConfigurationManager::loadParameters(QString filename, bool keepOld, QString format)
{
	QMutexLocker locker(&(m_shared->mutex));

	// If format is the empty string, guessing it from filename extension
	if (format.isEmpty()) {
		format = formatFromFilenameExtension(filename);

		if (format.isNull()) {
			return false;
		}
	}

	// Searching the format on the map
	QMap<QString, FileFormat>::iterator it = getFormatsMap().find(format);

	// If not found, returning false
	if (it == getFormatsMap().end()) {
		return false;
	}

	return it->fileLoaderSaver->load(filename, *this, keepOld);
}

bool ConfigurationManager::saveParameters(QString filename, QString format) const
{
	QMutexLocker locker(&(m_shared->mutex));

	// If format is the empty string, guessing it from filename extension
	if (format.isEmpty()) {
		format = formatFromFilenameExtension(filename);

		if (format.isNull()) {
			return false;
		}
	}

	// Searching the format on the map
	QMap<QString, FileFormat>::iterator it = getFormatsMap().find(format);

	// If not found, returning false
	if (it == getFormatsMap().end()) {
		return false;
	}

	return it->fileLoaderSaver->save(filename, *this);
}

void ConfigurationManager::callPostConfigureInitializationForConfiguredComponents()
{
	for (QList<Component*>::iterator it = m_shared->componentsConfiguredNotInitialized.begin(); it != m_shared->componentsConfiguredNotInitialized.end(); it++) {
		(*it)->postConfigureInitialization();
	}

	// Clearing the list
	m_shared->componentsConfiguredNotInitialized.clear();
}

void ConfigurationManager::registerAllStoredNotifications()
{
	for (QList<RegisteredRequestForResourceChangeNotification>::iterator it = m_shared->requestedNotifications.begin(); it != m_shared->requestedNotifications.end(); it++) {
		if (it->resourceOwner == NULL) {
			it->notifee->addNotifiedResource(it->resourceName);
		} else {
			it->notifee->addNotifiedResource(it->resourceName, it->resourceOwner);
		}
	}

	// Clearing the list
	m_shared->requestedNotifications.clear();
}

QString ConfigurationManager::formatFromFilenameExtension(QString filename)
{
	QFileInfo info(filename);

	QMap<QString, QString>::const_iterator it = getFileExtensionsMap().find(info.suffix());
	if (it == getFileExtensionsMap().end()) {
		return QString();
	} else {
		return *it;
	}
}

void ConfigurationManager::setComponentFromGroupStatusToCreating(QString group, Component *object)
{
	if (Q_UNLIKELY(m_shared->getComponentFromGroupRecursionLevel == 0)) {
		// Internal error, aborting
		qFatal("INTERNAL ERROR: setObjectFromGroupStatusToCreating called outside getComponentFromGroup");
	}
	m_shared->root->setComponentForNode(group, object, CreatingComponent);
}

void ConfigurationManager::setComponentFromGroupStatusToCreatedNotConfigured(QString group, Component *object)
{
	if (Q_UNLIKELY(m_shared->getComponentFromGroupRecursionLevel == 0)) {
		// Internal error, aborting
		qFatal("INTERNAL ERROR: setObjectFromGroupStatusToCreatedNotConfigured called outside getComponentFromGroup");
	}
	m_shared->root->setComponentForNode(group, object, ComponentCreatedNotConfigured);
}

void ConfigurationManager::setComponentFromGroupStatusToConfiguring(QString group, Component* object)
{
	if (Q_UNLIKELY(m_shared->getComponentFromGroupRecursionLevel == 0)) {
		// Internal error, aborting
		qFatal("INTERNAL ERROR: setObjectFromGroupStatusToConfiguring called outside getComponentFromGroup");
	}
	m_shared->root->setComponentForNode(group, object, ConfiguringComponent);
}

void ConfigurationManager::setComponentFromGroupStatusToCreatedAndConfigured(QString group, Component *object)
{
	if (Q_UNLIKELY(m_shared->getComponentFromGroupRecursionLevel == 0)) {
		// Internal error, aborting
		qFatal("INTERNAL ERROR: setObjectFromGroupStatusToCreatedAndConfigured called outside getComponentFromGroup");
	}
	m_shared->root->setComponentForNode(group, object, ComponentCreatedAndConfigured);
}

ConfigurationNode* ConfigurationManager::getConfigurationNodeForCurrentComponent()
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->nodeForComponentBeingCreated;
}

QString ConfigurationManager::getTypeForCurrentComponent()
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->typeForComponentBeingCreated;
}

QString ConfigurationManager::getPrefixForCurrentComponent()
{
	QMutexLocker locker(&(m_shared->mutex));

	return m_shared->prefixForComponentBeingCreated;
}

void ConfigurationManager::recursivelyDestroyComponents(ConfigurationNode* node)
{
	ComponentAndStatus o = node->getComponentForNode("");
	node->setComponentForNode("", NULL, ComponentNotCreated);
	delete o.component;

	// Recursively calling self for all child nodes
	foreach (ConfigurationNode* child, node->getChildrenList()) {
		recursivelyDestroyComponents(child);
	}
}

void ConfigurationManager::addObserver(ConfigurationObserver* observer)
{
	m_shared->observers.append(observer);
}

void ConfigurationManager::removeObserver(ConfigurationObserver* observer)
{
	m_shared->observers.removeAll(observer);
}

void ConfigurationManager::notifyObserversOfComponentCreation(Component* component)
{
	foreach (ConfigurationObserver* o, m_shared->observers) {
		o->onComponentCreation(component);
	}
}

void ConfigurationManager::notifyObserversOfComponentDestruction(Component* component)
{
	foreach (ConfigurationObserver* o, m_shared->observers) {
		o->onComponentDestruction(component);
	}
}

ConfigurationManager::SharedData::~SharedData()
{
	foreach (ConfigurationObserver* o, observers) {
		o->observedDestroyed();
	}
}

} // end namespace farsa
