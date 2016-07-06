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

#include "typesdb.h"
#include "component.h"
#include "configurationmanager.h"
#include <QStringList>
#include <QSet>

#warning VEDI COMMENTO SOTTO
// INOLTRE, FORSE, AGGIUNGERE UNA FUNZIONE VIRTUALE AL CREATOR CHE DISTRUGGE UN OGGETTO. L'IMPLEMENTAZIONE DI DEFAULT USA DELETE, MA IN QUESTO MODO SI POSSONO SUPPORTARE ANCHE CREAZIONI E DISTRUZIONI ALTERNATIVE

namespace farsa {

Component* ComponentCreator::create(ConfigurationManager& settings, QString prefix, bool configure)
{
	std::auto_ptr<Component> t;

	{
		// Before creating the component, we create a ResourceAccessor instance that could be used
		// in the instantiate method. We define a block so that resourceAccessor is destroyed after
		// the component has been created
		std::auto_ptr<ResourceAccessor> resourceAccessor(new ResourceAccessor(settings, settings.getConfigurationNodeForCurrentComponent()));
		t.reset(instantiate(settings, prefix, resourceAccessor.get()));
	}

	if (typeInfo().configuresInConstructor) {
		// Telling the ConfigurationManager object that the object for the group in prefix has been
		// created and configured
		settings.setComponentFromGroupStatusToCreatedAndConfigured(prefix, t.get());
	} else {
		// Telling the ConfigurationManager that this object for the group in prefix has been
		// created but not yet configured
		settings.setComponentFromGroupStatusToCreatedNotConfigured(prefix, t.get());

		if (configure) {
			// Telling the ConfigurationManager object that the object for the group in prefix is about
			// to be configured
			settings.setComponentFromGroupStatusToConfiguring(prefix, t.get());
			t->configure();
			// Telling the ConfigurationManager object that the object for the group in prefix has been
			// created and configured
			settings.setComponentFromGroupStatusToCreatedAndConfigured(prefix, t.get());
		}
	}

	return t.release();
}

TypesDB& TypesDB::instance()
{
	// The meyer singleton
	static TypesDB typesDB;

	return typesDB;
}

TypesDB::TypesDB()
	: m_typesMap()
	, m_creators()
	, m_childrenMap()
	, m_editorsMap()
	, m_typeDescriptions(new ConfigurationManager())
	, m_typeDescriptors()
{
	// Registering Component. This is needed because it is the root of the class hierarchy
	const QString name = "Component";
	std::auto_ptr<const RegisteredTypeInfo> componentInfo(new RegisteredTypeInfo(name, QStringList(), false, false, Component::configuresInConstructor()));
	m_typesMap.insert(name, componentInfo.get());
	componentInfo.release();
	m_childrenMap[name] = QStringList();
	Component::describe(editableTypeDescriptor(name));
}

TypesDB::~TypesDB()
{
	// Deleting all creators
	for(QMap<QString, ComponentCreator*>::iterator i = m_creators.begin(); i != m_creators.end(); i++) {
		delete i.value();
	}

	// Deleting all creators for editors
	for(QMap<QString, ConfigurationWidgetCreator*>::iterator i = m_editorsMap.begin(); i != m_editorsMap.end(); i++) {
		delete i.value();
	}
}

const RegisteredComponentDescriptor& TypesDB::typeDescriptor(QString typeName) const
{
	if (!isTypeRegisteredAndComponent(typeName)) {
		throw ClassNameNotRegisteredException(typeName.toLatin1().data());
	}

	return *(m_typeDescriptors.value(typeName));
}

bool TypesDB::isTypeRegistered(QString className) const
{
	return m_typesMap.contains(className);
}

bool TypesDB::isTypeRegisteredAndComponent(QString className) const
{
	// Here we can check whether the type is in m_typeDescriptors because all Component
	// subclasses have a descriptor that we store
	return m_typeDescriptors.contains(className);
}

const RegisteredTypeInfo& TypesDB::typeInfo(QString className) const
{
	if (!isTypeRegistered(className)) {
		throw ClassNameNotRegisteredException(className.toLatin1().data());
	}

	return *(m_typesMap[className]);
}

ComponentCreator* TypesDB::getComponentCreator(QString className)
{
	if (!isTypeRegistered(className)) {
		throw ClassNameNotRegisteredException(className.toLatin1().data());
	} else if (!m_creators.contains(className)) {
		throw ClassNameIsAbstractException(className.toLatin1().data());
	}

	return m_creators[className];
}

QStringList TypesDB::getAllTypeSubclasses(QString className, int levelToStop, bool noAbstractClasses)
{
	if (!isTypeRegistered(className)) {
		throw ClassNameNotRegisteredException(className.toLatin1().data());
	}

	QStringList subclasses;
	QStringList currents;
	currents.append(className);
	int level = 0;
	while (((level < levelToStop) || (levelToStop == -1)) && (currents.count() > 0)) {
		QStringList subs;
		foreach (QString current, currents) {
			subs.append(m_childrenMap[current]);
		}
		subclasses.append(subs);
		currents = subs;
		level++;
	}
	if (noAbstractClasses) {
		// filter QStringList
		QStringList filtered;
		foreach (QString current, subclasses) {
			if (!m_typesMap[current]->canBeCreated) {
				continue;
			}
			filtered << current;
		}
		return filtered;
	}
	return subclasses;
}

ConfigurationWidget* TypesDB::getEditorForType(ConfigurationManager& params, QString prefix, QWidget* parent, Qt::WindowFlags f)
{
	const QString typeForGroup = params.getValue(prefix + GroupSeparator + "type");

	// Creating the configuration widget. If we have a specific editor for the type creating it,
	// otherwise returing the default widget
	if ((!typeForGroup.isEmpty()) && (m_editorsMap.contains(typeForGroup))) {
		return m_editorsMap[typeForGroup]->create(params, prefix, parent, f);
	}
	return NULL;
}

bool TypesDB::dumpTypeDescription(QString filename, QString format) const
{
	return m_typeDescriptions->saveParameters(filename, format);
}

void TypesDB::checkReRegistrationIsPossible(const RegisteredTypeInfo* info)
{
	if (!m_typesMap.contains(info->name)) {
		return;
	}

	const RegisteredTypeInfo* oldInfo = m_typesMap[info->name];

	// Same parents?
	if (QSet<QString>::fromList(info->parents) != QSet<QString>::fromList(oldInfo->parents)) {
		throw CannotReRegisterType(info->name.toLatin1().data(), "different parents");
	}

	// Same characteristics?
	if (info->canBeCreated) {
		if (!oldInfo->canBeCreated) {
			throw CannotReRegisterType(info->name.toLatin1().data(), "the already registered class is abstract or interface, the new one is a concrete Component");
		}

		if (info->configuresInConstructor != oldInfo->configuresInConstructor) {
			throw CannotReRegisterType(info->name.toLatin1().data(), "different configuration strategies");
		}
	} else {
		if (oldInfo->canBeCreated) {
			throw CannotReRegisterType(info->name.toLatin1().data(), "the already registered class is a concrete Component, the new one is abstract or interface");
		}

		if (info->isInterface) {
			if (!oldInfo->isInterface) {
				throw CannotReRegisterType(info->name.toLatin1().data(), "the already registered class is an abstract component, the new one is an interface");
			}
		} else {
			if (oldInfo->isInterface) {
				throw CannotReRegisterType(info->name.toLatin1().data(), "the already registered class is an interface, the new one is an abstract component");
			}

			if (info->configuresInConstructor != oldInfo->configuresInConstructor) {
				throw CannotReRegisterType(info->name.toLatin1().data(), "different configuration strategies");
			}
		}
	}

	// Everything is ok
}

void TypesDB::checkParentsRegistered(const RegisteredTypeInfo* info)
{
	if (!info->canBeCreated && info->isInterface && info->parents.isEmpty()) {
		return;
	}

	foreach (QString parent, info->parents) {
		if (!m_typesMap.contains(parent)) {
			throw AncestorNotRegisteredException(info->name.toLatin1().data(), parent.toLatin1().data());
		}
	}
}

void TypesDB::componentRegistrationChecks(const RegisteredTypeInfo* info)
{
	bool atLeastOneParentComponent = false;
	const bool configuresInConstructor = info->configuresInConstructor;

	foreach (QString parent, info->parents) {
		const RegisteredTypeInfo* parentInfo = m_typesMap[parent];

		if (parentInfo->canBeCreated || !parentInfo->isInterface) {
			atLeastOneParentComponent = true;

			if (configuresInConstructor && !parentInfo->configuresInConstructor) {
				throw IncompatibleConfigurationStrategiesException(info->name.toLatin1().data(), info->name.toLatin1().data(), parent.toLatin1().data());
			}
		}
	}

	if (!atLeastOneParentComponent) {
		throw ComponentHasNoParentComponentException(info->name.toLatin1().data());
	}
}

RegisteredComponentDescriptor& TypesDB::editableTypeDescriptor(QString typeName)
{
	RegisteredComponentDescriptor* ret = NULL;

	if (m_typeDescriptors.contains(typeName)) {
#warning FORSE DOVREMMO PREVEDERE UN MECCANISMO PER RESETTARE UN DESCRIPTOR NEL CASO DI RE-REGISTRAZIONI...
		ret = m_typeDescriptors.value(typeName);
	} else {
		// This is to be exception safe
		try {
			ret = new RegisteredComponentDescriptor(m_typeDescriptions.get(), typeName);
			m_typeDescriptors.insert(typeName, ret);
		} catch(...) {
			delete ret;
			throw;
		}
	}

	return *ret;
}

} // end namespace farsa
