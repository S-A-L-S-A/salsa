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

#ifndef GENERICGROUPEDITOR_H
#define GENERICGROUPEDITOR_H

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QMap>
#include "configurationmanager.h"
#include "groupstreeeditor.h"

class QLineEdit;
class QLabel;
class QComboBox;
class QPushButton;

/*! This widget show a detailed view of parameter-value pairs for a given group
 */
class GenericGroupEditor : public QWidget {
	Q_OBJECT
public:
	/*! Constructor */
	GenericGroupEditor( QWidget* parent = 0, Qt::WindowFlags f = 0 );
	/*! Destructor */
	~GenericGroupEditor();
public slots:
	/*! set the group to show
	 *  \param group is the group informations
	 *  \param confParams is the ConfigurationManager where groupName belongs to;
	 *    it's passed as pointer and it's changed directly by GenericGroupEditor,
	 *    but GenericGroupEditor will not take ownership of the object pointerd
	 *  \warning the ConfigurationManager object pointer need to be always valid otherwise GenericGroupEditor
	 *    will crash
	 */
	void setGroup( GroupInfo group, farsa::ConfigurationManager* confParams );
	/*! unset the group, and it will display nothing */
	void unsetGroup();
	/*! reload the group; useful if something change about the current group */
	void reloadGroup();
signals:
	/*! emitted when the corresponding parameter of the group has been changed
	 *  \param parameter is the name of the parameter
	 *  \param value is the value of the parameter
	 *  \param prefix is the full path of the group containing the parameter
	 *  \param confParams is the ConfigurationManager where parameter belongs to
	 */
	void parameterChanged( QString parameter, QString value, QString prefix, farsa::ConfigurationManager* confParams );
	/*! emitted when the corresponding parameter of the group has been added
	 *  \param parameter is the name of the parameter
	 *  \param value is the value of the parameter
	 *  \param prefix is the full path of the group containing the parameter
	 *  \param confParams is the ConfigurationManager where parameter belongs to
	 */
	void parameterAdded( QString parameter, QString value, QString prefix, farsa::ConfigurationManager* confParams );
	/*! emitted when the corresponding parameter of the group has been deleted
	 *  \param parameter is the name of the parameter
	 *  \param value is the value of the parameter
	 *  \param prefix is the full path of the group containing the parameter
	 *  \param confParams is the ConfigurationManager where parameter belongs to
	 *  \note The values of parameter, value and prefix was valid before the parameter removing
	 */
	void parameterRemoved( QString parameter, QString value, QString prefix, farsa::ConfigurationManager* confParams );
	/*! emitted when the name of the group has been changed
	 *  \param newGroupName is the new name of the group
	 *  \param oldGroupName is the previous name of the group
	 *  \param prefix is the full path of the group containing groupName
	 *  \param confParams is the ConfigurationManager where parameter belongs to
	 */
	void groupRenamed( QString newGroupName, QString oldGroupName, QString prefix, farsa::ConfigurationManager* confParams );
	/*! emitted when a new subgroup has been added
	 *  \param SubgroupName is the name of the subgroup added
	 *  \param prefix is the full path of the group containing SubgroupName
	 *  \param confParams is the ConfigurationManager where parameter belongs to
	 */
	void groupAdded( QString SubgroupName, QString prefix, farsa::ConfigurationManager* confParams );
private slots:
	/*! change the name of the group */
	void renameGroup();
	/*! show the help for the new type selected */
	void showNewTypeHelp();
	/*! change the group type */
	void changeGroupType();
	/*! change the parameter's value
	 *  In this case the QObject::sender() is the widget that contains the new value
	 *  and depending on the type of sender a delegate will be called
	 */
	void changeParamValue();
	/*! delegate when the sender is a QLineEdit */
	void changeParamValueQLineEdit();
	/*! delegate when the sender is a QComboBox */
	void changeParamValueQComboBox();
	/*! delete the parameter */
	void deleteParameter();
	/*! add a new parameter */
	void addParameter();
	/*! add a new subgroup */
	void addSubgroup();

private:
	/*! the group informations */
	GroupInfo groupInfo;
	/*! the full path to the parent of the group */
	QString groupParent;
	/*! ConfigurationManager where groupName belongs to */
	farsa::ConfigurationManager* confParams;
	/*! GroupBox for editing the group's parameters */
	QGroupBox* groupBox;
	/*! layout of groupBox */
	QGridLayout* groupBoxLay;
	/*! GroupBox for adding new parameter */
	QGroupBox* newParamBox;
	/*! ComboBox for choosing the new parameter to add */
	QComboBox* newParamList;
	/*! GroupBox for adding new subgroup */
	QGroupBox* newSubgroupBox;
	/*! ComboBox for choosing the new subgroup to add */
	QComboBox* newSubgroupList;
	/*! widget for editing/showing group main informations */
	QLabel* groupNameLabel;
	QLineEdit* groupNameEdit;
	QLabel* groupHelpLabel;
	QLabel* groupTypeLabel;
	QLabel* groupTypeHelp;
	QComboBox* groupTypeEdit;
	QPushButton* groupTypeButton;
	QLabel* groupNewTypeHelp;
	/*! Mapping of editor widget to corresponding parameter name */
	QMap<QWidget*, QString> widgetToParam;
};

#endif
