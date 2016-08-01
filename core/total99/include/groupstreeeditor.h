/********************************************************************************
 *  SALSA - Total99                                                             *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef GROUPSTREEEDITOR_H
#define GROUPSTREEEDITOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QMap>
#include <QContextMenuEvent>
#include "configurationmanager.h"

/*! Class for storing information about a group:
 *  + treeItem    => the pointer to QTreeWidgetItem corresponding to this group
 *  + groupName   => the groupName
 *  + hasType     => if the group has a type associated to it (true or false)
 *  + isSubgroup  => if the group is a subgroup specified in the description of corresponding type
 *                   in this case the name of the group can't be changed
 *  + references  => the list of all object parameters where this group is referenced,
 *                   in order to synchronize the group name on changes
 *  + actualType  => the type of this group (if the group has a type) or of the parent group (if this
 *                   is a subgroup of a type)
 *  + subgroups   => the list of subgroups (from the outermost to the innermost) of actualType up
 *                   to this group (included, the same as groupName)
 *  + baseType    => the name of the base type of allowable types for this group
 *  + fullPath    => the full path to the ConfigurationParameters loaded
 */
class GroupInfo {
public:
	GroupInfo() {
		treeItem = NULL;
		hasType = false;
		isSubgroup = false;
	};
	QTreeWidgetItem* treeItem;
	QString groupName;
	bool hasType;
	bool isSubgroup;
	QStringList references;
	QString actualType;
	QStringList subgroups;
	QString baseType;
	QString fullPath;
	/*! The group is not valid in the following conditions:
	 *  - a group with type unspecified AND not a subgroup of the type of parent group
	 *  - a group with baseType specified AND actual type unspecified
	 *  - a group with type specified AND baseType unspecified
	 *  - a group without actualType
	 */
	bool isValid() {
		if ( !hasType && !isSubgroup ) return false;
		if ( baseType != QString("") && !hasType ) return false;
		if ( hasType && baseType == QString("") ) return false;
		if ( actualType == QString("") ) return false;
		return true;
	}
	/*! Returns the short help for the group */
	QString shortHelp() const;
	/*! Returns the long help for the group */
	QString longHelp() const;
};

/*! This widget show a tree view of all groups present in a ConfigurationParameters
 *  and also allow to edit the hierarchy of group adding, deleting and modifying the parent of groups
 */
class GroupsTreeEditor : public QWidget {
	Q_OBJECT
public:
	/*! Constructor */
	GroupsTreeEditor( salsa::ConfigurationManager* confParams, QWidget* parent = 0, Qt::WindowFlags f = 0 );
	/*! Destructor */
	~GroupsTreeEditor();
	/*! set the ConfigurationManager to show/edit
	 *  \param confParams is the ConfigurationManager; it's passed as pointer and it change directly
	 *    the object passed, but it will not take ownership of the object pointerd
	 *  \warning the ConfigurationManager object pointer need to be always valid otherwise GroupsTreeEditor
	 *    will crash
	 */
	void setConfigurationManager( salsa::ConfigurationManager* confParams );
	/*! return the group informations corresponding to the full path specified
	 *  \return if the fullPath correspond to a group it will return the GroupInfo associated, otherwise
	 *  it will return an invalid GroupInfo
	 */
	GroupInfo findGroupInfo( QString fullPath );
public slots:
	/*! it change the name of the group on the tree */
	void groupRenamed( QString newGroupName, QString oldGroupName, QString prefix, salsa::ConfigurationManager* confParams );
	/*! it add a group and its all subgroups */
	void addNewGroup( QString groupName, QString parentPath );
	/*! remove a Group, and all its subgroups */
	void removeGroup( GroupInfo group );
	/*! remove all the subgroups of the specified parentGroup */
	void removeAllSubGroupsOf( GroupInfo parentGroup );
	/*! Re-parse the configuration parameter and recreate the tree and internal informations */
	void parseConfigurationManager();
	/*! Update the information about the groups */
	void updateGroupsInformation();
signals:
	/*! emitted when the current group changed */
	void currentGroupChanged( GroupInfo group, salsa::ConfigurationManager* confParams );
protected:
	/*! populate and show the context menu */
	void contextMenuEvent( QContextMenuEvent* event );
private slots:
	/*! handle the change of the current item into the tree */
	void itemChanged( QTreeWidgetItem* current );
	/*! adjust the visualization of the tree */
	void adjustColumns();
	/*! remove the group related to the current item */
	void removeCurrentItemGroup();
private:
	/*! configure a new tree visulization for the confParams
	 *  \note it cancel all elements and create new elements from scratch
	 */
	void initConfTree();
	/*! add all subgroups of the specified parentGroup
	 *  \param parentGroup is the group representing the parent group on which all its subgroups need to be added
	 *      to the tree widget
	 *  \note if parentGroup is GroupInfo(), it create top level group items
	 */
	void insertAllSubGroupsOf( GroupInfo parentGroup );
	/*! insert a single group as child of parentItem
	 *  \param aGroup is the name of the group for which the correspondin tree item need to be created
	 *  \param parentGroup is the group representing the parent group of aGroup
	 */
	GroupInfo insertGroup( QString aGroup, GroupInfo parentGroup );
	/*! calculate the reference list for each GroupInfo entry */
	void initGroupInfoReferences();
	/*! update the information of all subgroups starting from the specified group path */
	void updateAllSubGroupsOf( QString parentFullPath );
	/*! update the information about the group specified by aGroupFullPath */
	void updateGroup( QString aGroupFullPath, GroupInfo parentGroup );

	/*! return the corresponding GroupInfo */
	GroupInfo& getGroupInfo( QTreeWidgetItem* item );

	/*! ConfigurationManager in view/editing */
	salsa::ConfigurationManager* confParams;
	/*! Tree widget for show the hierarchy of the groups */
	QTreeWidget* confTree;
	/*! Map from full path of the group to additional informations */
	QMap<QString, GroupInfo> groupInfos;
};

#endif
