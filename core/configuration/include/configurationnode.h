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

#ifndef CONFIGURATION_NODE_H
#define CONFIGURATION_NODE_H

#include "configurationconfig.h"
#include "configurationexceptions.h"
#include "configurationkey.h"
#include "componentandstatus.h"
#include <QMap>
#include <QString>
#include <QStringList>

namespace farsa {

class Component;
class ConfigurationNode;

/**
 * \brief A node in the configuration tree
 *
 * This can contain other nodes or parameters (name-value pairs). When
 * specifying the node or parameter you can use a path from the current node
 * through child nodes. Each node also has an associated object (see the
 * setObjectForNode() function).
 *
 * \internal
 */
class FARSA_CONF_API ConfigurationNode
{
public:
	/**
	 * \brief A structure holding a path splitted in two
	 *
	 * See the function separateLastElement() for more information
	 */
	struct ElementAndPath
	{
		/**
		 * \brief The last element of a path
		 */
		QString element;

		/**
		 * \brief The path up to element
		 */
		QString elementPath;
	};

	/**
	 * \brief Separates the last element of a given path
	 *
	 * The original path is splitted in two parts: the last element and the
	 * path up to that element
	 * \param path the path to split
	 * \return the splitted path
	 */
	static ElementAndPath separateLastElement(QString path);

public:
	/**
	 * \brief Constructor. This creates an empty node
	 *
	 * A node build using this constructor cannot have any parent. Use this
	 * only when building the root node, otherwise use the addNode()
	 * function
	 * \param name the name of the node
	 */
	ConfigurationNode(QString name);

	/**
	 * \brief Destructor. This also destroys child nodes
	 */
	~ConfigurationNode();

	/**
	 * \brief Returns true if this and other are equal
	 *
	 * Note: the name of the nodes being compared is ignored (i.e. if this
	 * and other have different names, they can nonetheless be considered
	 * equal)
	 * \param other the object to compare with this
	 * \return true if this and other are equal
	 */
	bool operator==(const ConfigurationNode& other) const;

	/**
	 * \brief Returns true if this and other are different
	 *
	 * Note: the name of the nodes being compared is ignored (i.e. if this
	 * and other have different names, they can nonetheless be considered
	 * equal)
	 * \param other the object to compare with this
	 * \return true if this and other are different
	 */
	bool operator!=(const ConfigurationNode& other) const;

	/**
	 * \brief Returns the parent node (const version)
	 *
	 * \return the parent node
	 */
	const ConfigurationNode* getParent() const
	{
		return m_parent;
	}

	/**
	 * \brief Returns the parent node
	 *
	 * \return the parent node
	 */
	ConfigurationNode* getParent()
	{
		return m_parent;
	}

	/**
	 * \brief Returns the depth level of this node, i.e. the distance from
	 *        the root
	 *
	 * \return the depth level of this node
	 */
	int getDepthLevel() const
	{
		return m_depthLevel;
	}

	/**
	 * \brief Returns a list of pointers to ancestor nodes
	 *
	 * The last element of the list is this node
	 * \return a list of pointers to ancestor nodes, from root down to this
	 *         node
	 */
	QList<const ConfigurationNode*> getAncestors() const;

	/**
	 * \brief Returns a list of names of ancestor nodes
	 *
	 * The last element of the list is this node
	 * \return a list of names of ancestor nodes, from root down to  this
	 *         node
	 */
	QStringList getAncestorsNames() const;

	/**
	 * \brief Returns the name of the node
	 *
	 * \return the name of the node
	 */
	QString getName() const
	{
		return m_name;
	}

	/**
	 * \brief Returns the full name of the node (i.e. the full path from
	 *        root to this node)
	 *
	 * \return the full name of the node
	 */
	QString getFullName() const;

	/**
	 * \brief Adds a child node with the given name and returns a pointer to
	 *        it
	 *
	 * If the node already exists an exception is thrown
	 * \param name the name of the child node
	 * \return a pointer to the new node
	 */
	ConfigurationNode* addNode(QString name);

	/**
	 * \brief Adds a child node with the given name and returns a pointer to
	 *        it
	 *
	 * If the node already exists returns the existing node
	 * \param name the name of the child node
	 * \return a pointer to the new node
	 */
	ConfigurationNode* addNodeOrReturnExisting(QString name);

	/**
	 * \brief Returns a pointer to the node having the given path
	 *
	 * The path is relative to this node. An exception is thrown if the node
	 * does not exists
	 * \param path the path of the node to return. The empty string is the
	 *             current node. The parent of the root node is the root
	 *             itself
	 * \return the node with the given path
	 */
	ConfigurationNode* getNode(QString path);

	/**
	 * \brief Returns a pointer to the node having the given path (const
	 *        version)
	 *
	 * The path is relative to this node. An exception is thrown if the node
	 * does not exists
	 * \param path the path of the node to return. The empty string is the
	 *             current node. The parent of the root node is the root
	 *             itself
	 * \return the node with the given path
	 */
	const ConfigurationNode* getNode(QString path) const;

	/**
	 * \brief Checks if the path is valid (i.e. if it refers to an existing
	 *        node)
	 *
	 * \param path the path to check
	 * \return true if the path is valid, false otherwise
	 */
	bool isPathValid(QString path) const;

	/**
	 * \brief Removes the child with the given name
	 *
	 * If the child doesn't exists, this throws an exception
	 * \param name the name of the child node to remove
	 */
	void deleteChild(QString name);

	/**
	 * \brief Rename the child node with the new given name
	 *
	 * If an error occurs (non-esistent group, invalid new name or existing
	 * new name), an exception is thrown
	 * \param oldName the current name of the child node to rename
	 * \param newName the new name to assign to the child node
	 */
	void renameChild(QString oldName, QString newName);

	/**
	 * \brief Returns true if a child with the given name exists
	 *
	 * \param name the name of the child to search
	 * \return true if the child exists
	 */
	bool hasChild(QString name) const;

	/**
	 * \brief Returns the list of child nodes
	 *
	 * \return the list of child nodes
	 */
	QList<ConfigurationNode*> getChildrenList();

	/**
	 * \brief Returns the list of child nodes (the list of names)
	 *
	 * This is slow as it needs to generate a copy of the list (we
	 * internally use KeyNames instead of QString)
	 * \return the list of names of child nodes
	 */
	QStringList getChildrenNamesList() const;

	/**
	 * \brief Returns the list of child nodes whose name matches the given
	 *        regular expression
	 *
	 * This is slow as it needs to generate a copy of the list (we
	 * internally use KeyNames instead of QString)
	 * \param filter the regular expression that must be matched by child
	 *               names
	 * \return the filtered list of names of child nodes
	 */
	QStringList getFilteredChildrenNamesList(QRegExp filter) const;

	/**
	 * \brief Sets the object corresponding to the given node to object
	 *
	 * \param path the path of the node whose corresponding object has to be
	 *             set (relative to this node)
	 * \param object the object corresponding to the given node. If is is
	 *               NULL and status is not CreatingObject nor
	 *               ObjectNotCreated, only the status of the current object
	 *               is changed; if status is ObjectNotCreated this is
	 *               ignored (object is set to NULL); otherwise the object
	 *               for the node is set to the value of this parameter.
	 * \param status the status of the object
	 */
	void setComponentForNode(QString path, Component* object, ComponentCreationStatus status = ComponentCreatedAndConfigured);

	/**
	 * \brief Resets the object for this node
	 *
	 * This sets the object to NULL and the object status to
	 * ObjectNotCreated
	 */
	void resetComponent();

	/**
	 * \brief Returns the object corresponding to the given node
	 *
	 * \param path the path of the node whose corresponding object has to be
	 *             set (relative to this node)
	 * \return the object for the given node and its status
	 */
	ComponentAndStatus getComponentForNode(QString path) const;

	/**
	 * \brief Adds a new parameter
	 *
	 * The value for the new parameter is the empty string (i.e. ""). If a
	 * parameter with the same name already exists, throws an exception
	 * \param name the name of the parameter to add
	 */
	void addParameter(QString name);

	/**
	 * \brief Checks whether a parameter exists
	 *
	 * \param name the parameter to check
	 * \return true if the parameter exists, false otherwise
	 */
	bool parameterExists(QString name) const;

	/**
	 * \brief Deletes a parameter
	 *
	 * If the parameter to delete does not exist, throws an exception
	 * \param name the name of the parameter to remove
	 */
	void deleteParameter(QString name);

	/**
	 * \brief Returns the value of the parameter with the given path
	 *
	 * The path is relative to this node. This throws an exception if the
	 * parameter does not exist
	 * \param path the path of the parameter (starting from this node)
	 * \return the parameter value
	 */
	QString getValue(QString path) const;

	/**
	 * \brief Returns the value of the parameter with the given path
	 *
	 * The path is relative to this node. The parameter is first searched in
	 * the given path then, if it is not found, it is searched back in
	 * parent groups until found or the main group is reached. This throws
	 * an exception if the parameter does not exist
	 * \param path the path of the parameter (starting from this node)
	 * \return the parameter value
	 */
	QString getValueAlsoMatchParents(QString path) const;

	/**
	 * \brief Sets the value of the parameter with the given path
	 *
	 * The path is relative to this node. This throws an exception if the
	 * parameter does not exist
	 * \param path the path of the parameter (starting from this node)
	 * \param value the new value for the parameter
	 */
	void setValue(QString path, QString value);

	/**
	 * \brief Returns the list of parameter names
	 *
	 * The list includes also the parameters referring to objects. This is
	 * slow as it needs to generate a copy of the list (we internally use
	 * KeyNames instead of QString)
	 * \return the list of names of parameters
	 */
	QStringList getParametersList() const;

	/**
	 * \brief Returns the list of parameters whose name matches the given
	 *        regular expression
	 *
	 * The list includes also the parameters referring to objects.  This is
	 * slow as it needs to generate a copy of the list (we internally use
	 * KeyNames instead of QString)
	 * \param filter the regular expression that must be matched by
	 *               parameters names
	 * \return the filtered list of names of parameters
	 */
	QStringList getFilteredParametersList(QRegExp filter) const;

	/**
	 * \brief Computes and return the lowest common ancestor of this and the
	 *        other node
	 *
	 * See http://en.wikipedia.org/wiki/Lowest_common_ancestor for the
	 * definition. This function throws an exception if no common ancestor
	 * exists.
	 * \param other the other node whose lowest common ancestor with this
	 *              should be found
	 * \return the lowest common ancestor
	 */
	ConfigurationNode* getLowestCommonAncestor(const ConfigurationNode* other);

	/**
	 * \brief Computes and return the lowest common ancestor of this and the
	 *        other node (const version)
	 *
	 * See http://en.wikipedia.org/wiki/Lowest_common_ancestor for the
	 * definition. This function throws an exception if no common ancestor
	 * exists.
	 * \param other the other node whose lowest common ancestor with this
	 *              should be found
	 * \return the lowest common ancestor
	 */
	const ConfigurationNode* getLowestCommonAncestor(const ConfigurationNode* other) const;

	/**
	 * \brief Computes and return the distance between two nodes
	 *
	 * This function throws an exception if no common ancestor exists
	 * between the two nodes
	 * \param other the node whose distance from this to compute
	 * \return the distance between this and other
	 */
	int getDistance(const ConfigurationNode* other) const;

	/**
	 * \brief Deletes all children and parameters
	 *
	 * This also resets to NULL the associated object, if present
	 */
	void clearAll();

	/**
	 * \brief Creates a duplicate of a node
	 *
	 * This recursively duplicates all properties and subgroups of the given
	 * node. If a node already exists with copyName, this function throws an
	 * exception. Objects associated with copied nodes are not copied (i.e.
	 * new nodes will have no object associated with them)
	 * \param soucePath the name of the node to duplicate. This is the full
	 *                  path to the node, always relative to this node (a
	 *                  leading "/" will be ignored)
	 * \param copyName the name of the copy of the child node
	 * \param copyPath copies the node under path. By default path is empty,
	 *                 meaning the node will be created as a child of this.
	 *                 The path is always relative to this node (a leading
	 *                 "/" will be ignored)
	 * \return the new node
	 */
	ConfigurationNode* duplicateNode(QString soucePath, QString copyName, QString copyPath = "");

	/**
	 * \brief Creates a duplicate this node and all subnodes
	 *
	 * The new node is a deep copy of this node and is the root of a new
	 * tree (its parent is set to NULL). If dest is not NULL, the given node
	 * is used as the root (clearAll() is called on it before doing anything
	 * else) and it is also the return value of the function. If it is NULL,
	 * a ConfigurationNode object is allocated using new and is returned.
	 * The name of the new root node is set to be the same as this name.
	 * Objects associated with copied nodes are not copied (i.e. new nodes
	 * will have no object associated with them)
	 * \warning the name of dest is not changed by this function (if dest
	 *          is not NULL)
	 * \param dest the node that will become the root of the new tree. If
	 *             NULL a node is new-allocated and returned
	 * \return the new node
	 */
	ConfigurationNode* createDeepCopy(ConfigurationNode* dest = NULL);

private:
	// This constructor is used when adding nodes (this allows to specify
	// the parent)
	ConfigurationNode(ConfigurationNode* parent, QString name);

	// This is like getNode() but does not throw an exception if the path
	// is not valid. If firstNonExistingGroup is not NULL, it is set to the
	// name of the first group in path that does not exist
	const ConfigurationNode* getNodeOrReturnNullIfNonExistent(QString path, QString* firstNonExistingGroup = NULL) const;

	// Recursively copies parameters and child nodes from one node to
	// another. Note that this doesn't clear the destination node! Moreover
	// this doesn't copy the object associated to the from node
	static void recursivelyCopyNode(const ConfigurationNode* from, ConfigurationNode* to);

	static void throwIfChildNameInvalid(QString name);
	void throwIfChildAlreadyExists(QString childName) const;
	void throwIfChildDoesNotExists(QString childName) const;
	static void throwIfParameterNameInvalid(QString name);
	void throwIfParameterAlreadyExists(QString parameter) const;
	void throwIfParameterDoesNotExists(QString parameter) const;

	ConfigurationNode* m_parent;
	int m_depthLevel;
	ConfigurationKey m_name;
	ComponentAndStatus m_component;
	QMap<ConfigurationKey, ConfigurationNode*> m_children;
	QMap<ConfigurationKey, QString> m_parameters;

	// Copy constructor and assignement operator are here to disallow usage
	ConfigurationNode(const ConfigurationNode &);
	ConfigurationNode& operator=(const ConfigurationNode &);
};

} // end namespace farsa

#endif
