/********************************************************************************
 *  FARSA - Total99                                                             *
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

#include "groupstreeeditor.h"
#include "total99resources.h"
#include "configurationhelper.h"
#include "typesdb.h"
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QTreeWidgetItemIterator>
#include <QMenu>
#include <QAction>

using namespace farsa;

QString GroupInfo::shortHelp() const
{
	const RegisteredComponentDescriptor& typeDescr = TypesDB::instance().typeDescriptor(actualType);

	if (hasType) {
		return typeDescr.shortHelp();
	} else {
		const SubgroupDescriptor* d = &(typeDescr.subgroupDescriptor(subgroups[0]));
		for (int i = 1; i < subgroups.size(); ++i) {
			d = &(d->subgroupDescriptor(subgroups[i]));
		}
		return d->longHelp();
	}
}

QString GroupInfo::longHelp() const
{
	const RegisteredComponentDescriptor& typeDescr = TypesDB::instance().typeDescriptor(actualType);

	if (hasType) {
		return typeDescr.shortHelp();
	} else {
		const SubgroupDescriptor* d = &(typeDescr.subgroupDescriptor(subgroups[0]));
		for (int i = 1; i < subgroups.size(); ++i) {
			d = &(d->subgroupDescriptor(subgroups[i]));
		}
		return d->longHelp();
	}
}

GroupsTreeEditor::GroupsTreeEditor( farsa::ConfigurationManager* confParams, QWidget* parent, Qt::WindowFlags f )
	:	QWidget( parent, f ),
		confParams( confParams ),
		groupInfos() {
	QGridLayout* mainLay = new QGridLayout( this );
	confTree = new QTreeWidget( this );
	confTree->setHeaderLabels( QStringList() << "Group" << "Type" );
	connect( confTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(itemChanged(QTreeWidgetItem*)) );
	connect( confTree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), SLOT(adjustColumns()) );
	connect( confTree, SIGNAL(itemExpanded(QTreeWidgetItem*)), SLOT(adjustColumns()) );
	mainLay->addWidget( confTree, 0, 0 );
	initConfTree();
}

GroupsTreeEditor::~GroupsTreeEditor() {
}

void GroupsTreeEditor::setConfigurationManager( farsa::ConfigurationManager* confParams ) {
	this->confParams = confParams;
	initConfTree();
	confTree->setCurrentItem( confTree->topLevelItem(0) );
}

void GroupsTreeEditor::parseConfigurationManager() {
	initConfTree();
}

void GroupsTreeEditor::updateGroupsInformation() {
	updateAllSubGroupsOf( "" );
}

void GroupsTreeEditor::contextMenuEvent( QContextMenuEvent* event ) {
	QTreeWidgetItem* current = confTree->currentItem();
	//--- get its group infos
	GroupInfo groupInfo = getGroupInfo( current );
	//--- create and populate the context menu
	QMenu menu(confTree);
	// --- actions for adding subgroups
	const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(*confParams, groupInfo.fullPath);
	foreach( QString aSubgroup, dc.subgroups() ) {
		const QStringList subgroups = confParams->getGroupsWithPrefixList( groupInfo.fullPath, aSubgroup );
		const bool allowMultiple = dc.subgroupDescriptor(aSubgroup).props().testFlag(ParamAllowMultiple);
		const QString postfix = allowMultiple ? QString(":%1").arg(subgroups.size()) : "";
		if ( subgroups.size() == 0 || allowMultiple ) {
			menu.addAction( QString("Add Subgroup %1%2").arg( aSubgroup, postfix ) );
		}
	}
	menu.addSeparator();
	// --- delete action
	bool deletable = true;
	if ( !groupInfo.treeItem->parent() ) {
		// cannot remove the root group [Component]
		deletable = false;
	}
	if ( groupInfo.isSubgroup ) {
		GroupInfo parentInfo = getGroupInfo( groupInfo.treeItem->parent() );
		const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(*confParams, parentInfo.fullPath);
		if ( dc.subgroupDescriptor(groupInfo.groupName).props().testFlag(ParamIsMandatory)) {
			// is a mandatory subgroup
			deletable = false;
		}
	}
	if ( deletable ) {
		menu.addAction( QString("Delete Group %1").arg( current->text(0) ), this, SLOT(removeCurrentItemGroup()) );
	}
	menu.exec( event->globalPos() );
}

GroupInfo GroupsTreeEditor::findGroupInfo( QString fullPath ) {
	if ( groupInfos.contains(fullPath) ) {
		return groupInfos[fullPath];
	}
	return GroupInfo();
}

void GroupsTreeEditor::groupRenamed( QString, QString, QString, farsa::ConfigurationManager* confs ) {
	//--- if the slot is regarding something different the configuration parameters setted on this object
	//    it will do nothing
	if ( confs != confParams ) return;
	// FIXME: instead of reloading information concerning the changed things, reload all groups !!
	parseConfigurationManager();
	return;
}

void GroupsTreeEditor::addNewGroup( QString groupName, QString parentPath ) {
	GroupInfo newitem = insertGroup( groupName, groupInfos[parentPath] );
	insertAllSubGroupsOf( newitem );
	// this is necessary for cross-references
	updateAllSubGroupsOf( "" );
}

void GroupsTreeEditor::removeGroup( GroupInfo group ) {
	// it is not possible to remove mainComponent
	QString total99Exp = "/"+confParams->getValue( "TOTAL99/mainComponent" );
	if ( group.fullPath == total99Exp ) {
		QMessageBox::warning( this, QString("Error on removing group"), QString("It is not possible to remove the group associated with the main Component") );
		return;
	}
	QString fullPath = group.fullPath;
	// remove the groupInfos and treeItems
	QTreeWidgetItem* treeItem = group.treeItem;
	foreach( QTreeWidgetItem* subItem, treeItem->takeChildren() ) {
		// get the GroupInfo associated and call removeGroup recursively
		removeGroup( groupInfos[ fullPath+"/"+subItem->text(0) ] );
	}
	delete treeItem;
	// remove the group and all subgroups from confParams
	confParams->deleteGroup( fullPath );
	groupInfos.take( fullPath );
}

void GroupsTreeEditor::removeAllSubGroupsOf( GroupInfo parentGroup ) {
	// Call removeGroup for all children of parentGroup
	QTreeWidgetItem* treeItem = parentGroup.treeItem;
	foreach( QTreeWidgetItem* subItem, treeItem->takeChildren() ) {
		// get the GroupInfo associated and call removeGroup
		removeGroup( groupInfos[ parentGroup.fullPath+"/"+subItem->text(0) ] );
	}
}

void GroupsTreeEditor::itemChanged( QTreeWidgetItem* current ) {
	if ( !current ) return;
	//--- calculate the full path of the group
	QString groupPath = current->text(0);
	QTreeWidgetItem* parent = current->parent();
	while( parent ) {
		groupPath = parent->text(0) + "/" + groupPath;
		parent = parent->parent();
	}
	emit currentGroupChanged( groupInfos[QString("/")+groupPath], confParams );
}

void GroupsTreeEditor::adjustColumns() {
	confTree->resizeColumnToContents( 0 );
	confTree->resizeColumnToContents( 1 );
}

void GroupsTreeEditor::removeCurrentItemGroup() {
	QTreeWidgetItem* current = confTree->currentItem();
	if ( QMessageBox::question( this, "Deleting Groups", QString("Are you sure to delete the group %1 and its all subgroups ??\nThis action cannot be undone").arg( current->text(0) ), QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes ) {
		return;
	}
	// !! OK DELETING !!
	//--- calculate the full path of the group
	QString groupPath = current->text(0);
	QTreeWidgetItem* parent = current->parent();
	while( parent ) {
		groupPath = parent->text(0) + "/" + groupPath;
		parent = parent->parent();
	}
	groupPath = QString("/")+groupPath;
	removeGroup( groupInfos[groupPath] );
}

void GroupsTreeEditor::initConfTree() {
	confTree->clear();
	groupInfos.clear();
	insertAllSubGroupsOf( GroupInfo() );
	initGroupInfoReferences();
	// add a specific reference for handling the TOTAL99/mainComponent parameter
	if (confParams->parameterExists("TOTAL99/mainComponent")) {
		QString total99Exp = "/"+confParams->getValue( "TOTAL99/mainComponent" );
		groupInfos[total99Exp].references.append( "TOTAL99/mainComponent" );
		// add a specific settings for mainComponent
		groupInfos[total99Exp].hasType = true;
		groupInfos[total99Exp].baseType = "Component";
	}
	//--- columns:
	//    0 -> the name of the group
	//    1 -> the type selected (it will be none if it's not assignable)
	confTree->expandAll();
	confTree->resizeColumnToContents( 0 );
	confTree->resizeColumnToContents( 1 );
}

void GroupsTreeEditor::insertAllSubGroupsOf( GroupInfo parentGroup ) {
	QString parent = "";
	if ( parentGroup.treeItem ) {
		parent = parentGroup.fullPath;
	}
	QStringList groupList = confParams->getGroupsList( parent );
	foreach( QString aGroup, groupList ) {
		//--- Skip the Total99 Group
		if ( aGroup == "TOTAL99" ) continue;
		GroupInfo newitem = insertGroup( aGroup, parentGroup );
		insertAllSubGroupsOf( newitem );
	}
}

GroupInfo GroupsTreeEditor::insertGroup( QString aGroup, GroupInfo parentGroup ) {
	QString parent = "";
	if ( parentGroup.treeItem ) {
		parent = parentGroup.fullPath;
	}
	//--- create full path for the map, and
	//    it collect data about: hasType, isSubgroup and baseType
	GroupInfo newinfo;
	newinfo.groupName = aGroup;
	newinfo.fullPath = parent + "/" + aGroup;
	//--- references here is not possibile
	//    they should be calculated after the insertion of all data
	newinfo.references = QStringList();
	//--- insert the item
	QStringList columns;
	columns << aGroup << "";
	if ( parentGroup.treeItem ) {
		newinfo.treeItem = new QTreeWidgetItem( parentGroup.treeItem, columns );
	} else {
		newinfo.treeItem = new QTreeWidgetItem( confTree, columns );
	}
	groupInfos[ newinfo.fullPath ] = newinfo;
	//--- calculate the GroupInfo
	updateGroup( newinfo.fullPath, parentGroup );
	return newinfo;
}

void GroupsTreeEditor::updateAllSubGroupsOf( QString parentFullPath ) {
	QStringList groupList = confParams->getGroupsList( parentFullPath );
	foreach( QString aGroup, groupList ) {
		//--- Skip the Total99 Group
		if ( aGroup == "TOTAL99" ) continue;
		QString aGroupFullPath = parentFullPath+"/"+aGroup;
		if ( groupInfos.contains( aGroupFullPath ) ) {
			//--- update it
			updateGroup( aGroupFullPath, groupInfos[ parentFullPath ] );
		} else {
			//--- it is a new group to insert
			insertGroup( aGroup, groupInfos[parentFullPath] );
		}
		updateAllSubGroupsOf( aGroupFullPath );
	}
}

void GroupsTreeEditor::updateGroup( QString aGroupFullPath, GroupInfo parentGroup ) {
	//--- update the information about this group
	GroupInfo& info = groupInfos[ aGroupFullPath ];
	QString type = ConfigurationHelper::getString( *confParams, info.fullPath+"/type", "no type" );
	info.treeItem->setText( 1, type );
	if ( type != "no type" ) {
		info.hasType = true;
		info.baseType = type;
		info.actualType = type;
	} else {
		info.hasType = false;
		info.baseType = QString("");
		info.actualType = QString("");
	}
	info.isSubgroup = false;
	if ( parentGroup.treeItem ) {
		QStringList groupsToType;
		if (!parentGroup.hasType) {
			groupsToType = parentGroup.subgroups;
		}

		try {
			const RegisteredComponentDescriptor& parentType = TypesDB::instance().typeDescriptor(parentGroup.actualType);
			const AbstractDescriptorContainer* dc = &parentType;
			foreach (QString g, groupsToType) {
				dc = &(dc->subgroupDescriptor(g));
			}

			const QString prefixGroup = info.groupName.split(':')[0];
			if (dc->subgroups().contains(prefixGroup)) {
				info.isSubgroup = true;
				if (info.hasType) {
					info.baseType = dc->subgroupDescriptor(prefixGroup).componentType();
				} else {
					// if it has no type, then the description is provided by parent group
					info.subgroups = groupsToType;
					groupsToType.append(prefixGroup);
				}
			}
		} catch (ParameterOrSubgroupNotDescribedException&) {
		} catch (ClassNameNotRegisteredException&) {
		}
	} else if (aGroupFullPath == ("/" + confParams->getValue("TOTAL99/mainComponent"))) {
		// If this is the main component, we set its base type to "Component". We have to
		// explicitly do this because the main component has no parent, so the type is not
		// changed in the if above
		info.baseType = "Component";
	}
}

void GroupsTreeEditor::initGroupInfoReferences() {
	//--- for each group get the full parameter list and
	//    if a parameter has a value corresponding to a group registered in the map
	//    it will add the reference
	// FIXME: it does not handle parameters containing a list of objects/groups
	foreach( QString fullPath, groupInfos.keys() ) {
		foreach( QString param, confParams->getParametersList( fullPath ) ) {
			QString refGroup = confParams->getValue( fullPath+"/"+param );
			if ( groupInfos.contains( refGroup ) ) {
				// now add the reference
				groupInfos[refGroup].references.append( fullPath+"/"+param );
			}
		}
	}
}

GroupInfo& GroupsTreeEditor::getGroupInfo( QTreeWidgetItem* item ) {
	//--- calculate the full path of the group
	QString groupPath = item->text(0);
	QTreeWidgetItem* parent = item->parent();
	while( parent ) {
		groupPath = parent->text(0) + "/" + groupPath;
		parent = parent->parent();
	}
	groupPath = QString("/")+groupPath;
	//--- return its group infos
	return groupInfos[groupPath];
}
