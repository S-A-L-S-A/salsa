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
#include "configurationhelper.h"
#include "component.h"
#include <memory>

#warning USARE QRegularExpression INVECE DI QRegExp OVUNQUE

namespace farsa {

ConfigurationNode::ElementAndPath ConfigurationNode::separateLastElement(QString path)
{
	ElementAndPath ep;

	ep.element = path.section(GroupSeparator, -1, -1, QString::SectionSkipEmpty);
	ep.elementPath = path.section(GroupSeparator, 0, -2, QString::SectionSkipEmpty);

	return ep;
}

ConfigurationNode::ConfigurationNode(QString name)
	: m_parent(NULL)
	, m_depthLevel(0)
	, m_name(name)
	, m_component()
	, m_children()
	, m_parameters()
{
	throwIfChildNameInvalid(m_name);
}

ConfigurationNode::~ConfigurationNode()
{
	// Removing everything
	clearAll();
}

bool ConfigurationNode::operator==(const ConfigurationNode& other) const
{
	if (m_parameters != other.m_parameters) {
		return false;
	}

	QList<ConfigurationNode*> myChild = m_children.values();
	QList<ConfigurationNode*> otherChild = other.m_children.values();
	if (myChild.size() != otherChild.size()) {
		return false;
	}

	for (int i = 0; i < myChild.size(); ++i) {
		if (*(myChild[i]) != *(otherChild[i])) {
			return false;
		}
	}

	return true;
}

bool ConfigurationNode::operator!=(const ConfigurationNode& other) const
{
	return !(*this == other);
}

QList<const ConfigurationNode*> ConfigurationNode::getAncestors() const
{
	QList<const ConfigurationNode*> list;

	// If we have a parent, calling its getAncestors and appending us to the result
	if (m_parent != NULL) {
		list = m_parent->getAncestors();
	}

	list.append(this);

	return list;
}

QStringList ConfigurationNode::getAncestorsNames() const
{
	QStringList list;

	// If we have a parent, calling its getAncestorsNames and appending us to the result
	if (m_parent != NULL) {
		list = m_parent->getAncestorsNames();
	}

	list.append(m_name);

	return list;
}

QString ConfigurationNode::getFullName() const
{
	// If we have a parent, calling its getFullName and prepending the result to our name
	if (m_parent != NULL) {
		return m_parent->getFullName() + GroupSeparator + m_name;
	} else {
		return m_name;
	}
}

ConfigurationNode* ConfigurationNode::addNode(QString name)
{
	// The validity of the name is checked in the new node constructor

	throwIfChildAlreadyExists(name);

	// auto_ptr is used to stay exception safe
	std::auto_ptr<ConfigurationNode> newNode(new ConfigurationNode(this, name));
	m_children.insert(name, newNode.get());

	return newNode.release();
}

ConfigurationNode* ConfigurationNode::addNodeOrReturnExisting(QString name)
{
	// The validity of the name is checked in the new node constructor

	// Checking if a node with that name already exists
	ConfigurationNode* child = m_children.value(name, NULL);
	if (child != NULL) {
		return child;
	}

	// auto_ptr is used to stay exception safe
	std::auto_ptr<ConfigurationNode> newNode(new ConfigurationNode(this, name));
	m_children.insert(name, newNode.get());

	return newNode.release();
}

ConfigurationNode* ConfigurationNode::getNode(QString path)
{
	// Using the const version and a const_cast
	return const_cast<ConfigurationNode*>((const_cast<const ConfigurationNode*>(this))->getNode(path));
}

const ConfigurationNode* ConfigurationNode::getNode(QString path) const
{
	QString firstNonExistingGroup;
	const ConfigurationNode* n = getNodeOrReturnNullIfNonExistent(path, &firstNonExistingGroup);

	if (n == NULL) {
		throw NonExistentGroupNameException(firstNonExistingGroup.toLatin1().data());
	}

	return n;
}

bool ConfigurationNode::isPathValid(QString path) const
{
	return (getNodeOrReturnNullIfNonExistent(path) != NULL);
}

void ConfigurationNode::deleteChild(QString name)
{
	throwIfChildDoesNotExists(name);

	// We checked above that the child exists
	ConfigurationNode* const childNode = m_children.value(name, NULL);
	// Here we are supposing that QMap::remove is exception safe, but this is not true...
	m_children.remove(name);
	delete childNode;
}

void ConfigurationNode::renameChild(QString oldName, QString newName)
{
	throwIfChildNameInvalid(newName);
	throwIfChildAlreadyExists(newName);
	throwIfChildDoesNotExists(oldName);

	// We checked above that the child exists
	std::auto_ptr<ConfigurationNode> childNode(m_children.value(oldName, NULL));

	// Take the element with oldName and re-insert it with newName
	m_children.remove(oldName);
	childNode->m_name = newName;
	m_children.insert(newName, childNode.release());
}

bool ConfigurationNode::hasChild(QString name) const
{
	return m_children.contains(name);
}

QList<ConfigurationNode*> ConfigurationNode::getChildrenList()
{
	return m_children.values();
}

QStringList ConfigurationNode::getChildrenNamesList() const
{
	// Here we have to generate the list to return by hand...
	QStringList l;
	foreach (const ConfigurationKey& n, m_children.keys()) {
		l.append(n);
	}

	return l;
}

QStringList ConfigurationNode::getFilteredChildrenNamesList(QRegExp filter) const
{
	return getChildrenNamesList().filter(filter);
}

void ConfigurationNode::setComponentForNode(QString path, Component* component, ComponentCreationStatus status)
{
	// Getting the node for path
	ConfigurationNode* node = getNode(path);

	// Setting the object and its status
	if (status == ComponentNotCreated) {
		node->m_component.component = NULL;
	} else if ((component != NULL) || (status == CreatingComponent)) {
		node->m_component.component = component;
	}
	node->m_component.status = status;
}

void ConfigurationNode::resetComponent()
{
	m_component.component = NULL;
	m_component.status = ComponentNotCreated;
}

ComponentAndStatus ConfigurationNode::getComponentForNode(QString path) const
{
	// Getting the node for path
	const ConfigurationNode* node = getNode(path);

	return node->m_component;
}

void ConfigurationNode::addParameter(QString name)
{
	throwIfParameterNameInvalid(name);
	throwIfParameterAlreadyExists(name);

	// Adding the new parameter to the map with an empty value
	m_parameters.insert(name, QString(""));
}

bool ConfigurationNode::parameterExists(QString name) const
{
	return m_parameters.contains(name);
}

void ConfigurationNode::deleteParameter(QString name)
{
	throwIfParameterDoesNotExists(name);

	m_parameters.remove(name);
}

QString ConfigurationNode::getValue(QString path) const
{
	const ElementAndPath ep = separateLastElement(path);

	const ConfigurationNode* node = getNode(ep.elementPath);

	node->throwIfParameterDoesNotExists(ep.element);

	return node->m_parameters.value(ep.element);
}

QString ConfigurationNode::getValueAlsoMatchParents(QString path) const
{
	const ElementAndPath ep = separateLastElement(path);

	// Getting the node, searching parents if necessary
	const ConfigurationNode* node = getNode(ep.elementPath);
	while ((node != NULL) && (!node->parameterExists(ep.element))) {
		node = node->m_parent;
	}

	if (node == NULL) {
		throw NonExistentParameterException(path.toLatin1().data());
	}

	return node->m_parameters.value(ep.element);
}

void ConfigurationNode::setValue(QString path, QString value)
{
	const ElementAndPath ep = separateLastElement(path);

	ConfigurationNode* node = getNode(ep.elementPath);

	node->throwIfParameterDoesNotExists(ep.element);

	node->m_parameters[ep.element] = value;
}

QStringList ConfigurationNode::getParametersList() const
{
	// Here we have to generate the list to return by hand...
	QStringList l;
	foreach (const ConfigurationKey& n, m_parameters.keys()) {
		l.append(n);
	}

	return l;
}

QStringList ConfigurationNode::getFilteredParametersList(QRegExp filter) const
{
	return getParametersList().filter(filter);
}

ConfigurationNode* ConfigurationNode::getLowestCommonAncestor(const ConfigurationNode* other)
{
	// Using the const version and a const_cast
	return const_cast<ConfigurationNode*>((const_cast<const ConfigurationNode*>(this))->getLowestCommonAncestor(other));
}

const ConfigurationNode* ConfigurationNode::getLowestCommonAncestor(const ConfigurationNode* other) const
{
	const ConfigurationNode* curAncestorOfThis = this;
	const ConfigurationNode* curAncestorOfOther = other;

	// We first look for ancestors of this and other that are on the same level, then we can
	// take ancestors of the two in parallel until we find a common one
	while (curAncestorOfThis->getDepthLevel() != curAncestorOfOther->getDepthLevel()) {
		if (curAncestorOfThis->getDepthLevel() < curAncestorOfOther->getDepthLevel()) {
			curAncestorOfOther = curAncestorOfOther->getParent();
		} else {
			curAncestorOfThis = curAncestorOfThis->getParent();
		}
	}

	while ((curAncestorOfThis != curAncestorOfOther) && (curAncestorOfThis != NULL) && (curAncestorOfOther != NULL)) {
		curAncestorOfThis = curAncestorOfThis->getParent();
		curAncestorOfOther = curAncestorOfOther->getParent();
	}

	if ((curAncestorOfThis == NULL) || (curAncestorOfOther == NULL)) {
		throw NoCommonAncestorException();
	}

	// If we get here, the two ancestors are the same
	return curAncestorOfOther;
}

int ConfigurationNode::getDistance(const ConfigurationNode* other) const
{
	return getDepthLevel() + other->getDepthLevel() - 2 * getLowestCommonAncestor(other)->getDepthLevel();
}

void ConfigurationNode::clearAll()
{
	// Removing all child nodes
	for (QMap<ConfigurationKey, ConfigurationNode*>::iterator it = m_children.begin(); it != m_children.end(); it++) {
		delete it.value();
	}
	m_children.clear();

	// Removing all parameters
	m_parameters.clear();

	// Resetting to NULL the associated object
	m_component.status = ComponentNotCreated;
	m_component.component = NULL;
}

ConfigurationNode* ConfigurationNode::duplicateNode(QString soucePath, QString copyName, QString copyPath)
{
	const ConfigurationNode* const sourceNode = getNode(soucePath);

	ConfigurationNode* const destinationParentNode = getNode(copyPath);

	// Creating node and copying into it
	ConfigurationNode* const newNode = destinationParentNode->addNode(copyName);
	recursivelyCopyNode(sourceNode, newNode);

	return newNode;
}

ConfigurationNode* ConfigurationNode::createDeepCopy(ConfigurationNode* dest)
{
	// Allocating memory for the destination root if necessary
	if (dest == NULL) {
		dest = new ConfigurationNode(NULL, m_name);
	}

	// Clearing the content of dest and setting parent to NULL
	dest->clearAll();
	dest->m_parent = NULL;
	dest->m_depthLevel = 0;

	// Recursively copying this into dest
	recursivelyCopyNode(this, dest);

	return dest;
}

ConfigurationNode::ConfigurationNode(ConfigurationNode* parent, QString name)
	: m_parent(parent)
	, m_depthLevel(0)
	, m_name(name)
	, m_component()
	, m_children()
	, m_parameters()
{
	throwIfChildNameInvalid(m_name);

	// Computing the depth level
	ConfigurationNode* p = m_parent;
	while (p != NULL) {
		++m_depthLevel;
		p = p->m_parent;
	}
}

const ConfigurationNode* ConfigurationNode::getNodeOrReturnNullIfNonExistent(QString path, QString* firstNonExistingGroup) const
{
	// Getting the first element of the path
	const QString firstNode = path.section(GroupSeparator, 0, 0, QString::SectionSkipEmpty);

	if (firstNode.isEmpty()) {
		// If the first node is the empty string (i.e. the whole path is the empty string) returning this
		return this;
	} else {
		const QString remainingPath = path.section(GroupSeparator, 1, -1, QString::SectionSkipEmpty);

		if (firstNode == ParentGroup) {
			// The path points to our parent group, recursively calling
			// getNodeOrReturnNullIfNonExistent on it (if we have no parent, using self)
			return ((m_parent == NULL) ? this : m_parent)->getNodeOrReturnNullIfNonExistent(remainingPath);
		} else {
			// Getting the first node from the list of my children and,
			// if it exists, calling its getNodeOrReturnNullIfNonExistent function
			if (m_children.contains(firstNode)) {
				return m_children[firstNode]->getNodeOrReturnNullIfNonExistent(remainingPath);
			} else {
				if (firstNonExistingGroup != NULL) {
					*firstNonExistingGroup = firstNode;
				}
				return NULL;
			}
		}
	}
}

void ConfigurationNode::recursivelyCopyNode(const ConfigurationNode* from, ConfigurationNode* to)
{
	// First copying all parameters
	to->m_parameters = from->m_parameters;

	// We don't copy the object associated to the node

	// Now creating children in to and recursively filling them
	for (QMap<ConfigurationKey, ConfigurationNode*>::const_iterator it = from->m_children.begin(); it != from->m_children.end(); it++) {
		ConfigurationNode* addedNode = to->addNode(it.key());
		recursivelyCopyNode(it.value(), addedNode);
	}
}

void ConfigurationNode::throwIfChildNameInvalid(QString name)
{
	if (name.contains(GroupSeparator) || (name == ParentGroup) || name.isEmpty()) {
		throw InvalidGroupNameException(name.toLatin1().data());
	}
}

void ConfigurationNode::throwIfChildAlreadyExists(QString childName) const
{
	if (m_children.contains(childName)) {
		throw AlreadyExistingGroupNameException(childName.toLatin1().data());
	}
}

void ConfigurationNode::throwIfChildDoesNotExists(QString childName) const
{
	if (!m_children.contains(childName)) {
		throw NonExistentGroupNameException(childName.toLatin1().data());
	}
}

void ConfigurationNode::throwIfParameterNameInvalid(QString name)
{
	if (name.isEmpty() || name.contains(GroupSeparator) || (name == ParentGroup)) {
		throw InvalidParameterNameException(name.toLatin1().data());
	}
}

void ConfigurationNode::throwIfParameterAlreadyExists(QString parameter) const
{
	if (m_parameters.contains(parameter)) {
		throw AlreadyExistingParameterException(parameter.toLatin1().data());
	}
}

void ConfigurationNode::throwIfParameterDoesNotExists(QString parameter) const
{
	if (!m_parameters.contains(parameter)) {
		throw NonExistentParameterException(parameter.toLatin1().data());
	}
}

} // end namespace farsa
