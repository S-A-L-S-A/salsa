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

#include "parametereditorsfactory.h"
#include "genericgroupeditor.h"
#include "total99resources.h"
#include "typesdb.h"
#include "configurationhelper.h"
#include "configurationwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QMessageBox>
#include <QFrame>
#include <QIcon>
#include <limits>

using namespace salsa;

GenericGroupEditor::GenericGroupEditor( QWidget* parent, Qt::WindowFlags f )
	:	QWidget(parent, f),
		groupInfo(),
		groupParent(),
		confParams(NULL),
		widgetToParam() {
	QVBoxLayout* mainLay = new QVBoxLayout( this );

	QFrame* widget = new QFrame( this );
	widget->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
	widget->setObjectName( "groupMainInfo" );
	mainLay->addWidget( widget );
	QGridLayout* lay = new QGridLayout( widget );
	groupNameLabel = new QLabel( "Name:", widget );
	lay->addWidget( groupNameLabel, 0, 0 );
	groupNameEdit = new QLineEdit( widget );
	connect( groupNameEdit, SIGNAL(textEdited(const QString)), this, SLOT(renameGroup()) );
	lay->addWidget( groupNameEdit, 0, 1 );
	groupHelpLabel = new QLabel( "", widget );
	groupHelpLabel->setWordWrap( true );
	lay->addWidget( groupHelpLabel, 1, 0, 1, 2 );
	groupTypeLabel = new QLabel( "Type:", widget );
	lay->addWidget( groupTypeLabel, 2, 0, 1, 2 );
	groupTypeHelp = new QLabel( "", widget );
	groupTypeHelp->setWordWrap( true );
	lay->addWidget( groupTypeHelp, 3, 0, 1, 2 );
	groupTypeEdit = new QComboBox( widget );
	groupTypeEdit->setEditable( false );
	connect( groupTypeEdit, SIGNAL(currentIndexChanged(const QString)), this, SLOT(showNewTypeHelp()) );
	lay->addWidget( groupTypeEdit, 4, 0 );
	lay->setColumnStretch( 0, 1 );
	lay->setColumnStretch( 1, 1 );
	groupTypeButton = new QPushButton( "Change Type", widget );
	connect( groupTypeButton, SIGNAL(clicked()), this, SLOT(changeGroupType()) );
	lay->addWidget( groupTypeButton, 4, 1 );
	groupNewTypeHelp = new QLabel( "", widget );
	groupNewTypeHelp->setWordWrap( true );
	lay->addWidget( groupNewTypeHelp, 5, 0, 1, 2 );

	groupHelpLabel->setText( "Select a Group" );
	groupHelpLabel->show();
	groupNameLabel->hide();
	groupNameEdit->hide();
	groupTypeLabel->hide();
	groupTypeHelp->hide();
	groupTypeEdit->hide();
	groupTypeButton->hide();
	groupNewTypeHelp->hide();

	groupBox = new QGroupBox( "Group's Parameters", this );
	groupBox->setObjectName( "groupEditorBox" );
	groupBoxLay = new QGridLayout( groupBox );
	groupBoxLay->setObjectName( "groupBoxLayout" );
	groupBoxLay->setContentsMargins( QMargins() );
	groupBoxLay->setSpacing( 2 );
	mainLay->addWidget( groupBox );

	newParamBox = new QGroupBox( "New Parameter", this );
	newParamBox->setObjectName( "newParameterBox" );
	QGridLayout* newParamBoxLay = new QGridLayout( newParamBox );
	newParamList = new QComboBox( newParamBox );
	newParamBoxLay->addWidget( newParamList, 0, 0 );
	QPushButton* newParamBt = new QPushButton( "Add", newParamBox );
	connect( newParamBt, SIGNAL(clicked()), this, SLOT(addParameter()) );
	newParamBoxLay->addWidget( newParamBt, 0, 1 );
	newParamBoxLay->setColumnStretch( 0, 2 );
	mainLay->addWidget( newParamBox );

	newSubgroupBox = new QGroupBox( "New Subgroup", this );
	newSubgroupBox->setObjectName( "newSubgroupBox" );
	QGridLayout* newSubgroupBoxLay = new QGridLayout( newSubgroupBox );
	newSubgroupList = new QComboBox( newSubgroupBox );
	newSubgroupBoxLay->addWidget( newSubgroupList, 0, 0 );
	QPushButton* newSubgroupBt = new QPushButton( "Add", newSubgroupBox );
	connect( newSubgroupBt, SIGNAL(clicked()), this, SLOT(addSubgroup()) );
	newSubgroupBoxLay->addWidget( newSubgroupBt, 0, 1 );
	newSubgroupBoxLay->setColumnStretch( 0, 2 );
	mainLay->addWidget( newSubgroupBox );
	mainLay->addStretch( 2 );

	groupBox->setEnabled( false );
	newParamBox->setEnabled( false );
	newSubgroupBox->setEnabled( false );
}

GenericGroupEditor::~GenericGroupEditor() {
}

void GenericGroupEditor::setGroup( GroupInfo group, salsa::ConfigurationManager* confParams ) {
	this->confParams = confParams;
	this->groupInfo = group;
	//--- extract the path of the parent
	groupParent = groupInfo.fullPath.section( '/', 0, -2 );
	//--- remove all previous widget
	foreach( QObject* widget, groupBox->children() ) {
		if ( widget->objectName() == "groupBoxLayout" ) continue;
		delete widget;
	}
	widgetToParam.clear();
	if ( !groupInfo.isValid() ) {
		groupHelpLabel->setText( QString("The Group ")+groupInfo.groupName+" has NO VALID Configuration for Total99" );
		groupHelpLabel->show();
		groupBox->setEnabled( false );
		newParamBox->setEnabled( false );
		groupNameLabel->hide();
		groupNameEdit->hide();
		groupTypeLabel->hide();
		groupTypeHelp->hide();
		groupTypeEdit->hide();
		groupTypeButton->hide();
		groupNewTypeHelp->hide();
		return;
	}
	//--- Filling widgets with main informations about group
	groupNameEdit->setText( groupInfo.groupName );
	groupNameEdit->show();
	groupNameLabel->show();
	if ( groupInfo.isSubgroup ) {
		//--- subgroup: disable edit and show help
		QString help = groupInfo.shortHelp() + " " + groupInfo.longHelp();
		groupHelpLabel->setText( help );
		groupHelpLabel->show();
		groupNameEdit->setEnabled( false );
	} else {
		//--- no subgroup: enable edit and hide help
		groupHelpLabel->setText( "" );
		groupHelpLabel->hide();
		groupNameEdit->setEnabled( true );
	}

	ConfigurationWidget* graphicalEditor = NULL;
	if ( groupInfo.hasType ) {
		// hasType: show type information and change combobox+button
		QString currType = confParams->getValue( groupInfo.fullPath+"/type" );
		groupTypeLabel->setText( QString("Type: ") + currType );
		const QString shortHelp = groupInfo.shortHelp();
		const QString longHelp = groupInfo.longHelp();
		QString help = shortHelp + " " + longHelp;
		groupTypeHelp->setText( QString("Type description: ") + help );
		groupTypeEdit->clear();
		QStringList allTypes = TypesDB::instance().getAllTypeSubclasses( groupInfo.baseType, -1, true );
		if ( TypesDB::instance().typeInfo( groupInfo.baseType ).canBeCreated ) {
			allTypes << groupInfo.baseType;
		}
		allTypes.sort();
		allTypes.prepend( "" );
		groupTypeEdit->addItems( allTypes );
		groupNewTypeHelp->setText( "" );
		groupTypeLabel->show();
		groupTypeHelp->show();
		groupTypeEdit->show();
		groupTypeButton->show();
		groupNewTypeHelp->hide();

		//--- check if there is a graphical editor
		graphicalEditor = TypesDB::instance().getEditorForType( *confParams, groupInfo.fullPath, groupBox );
	} else {
		// noType: hide type information and change combobox+button
		groupTypeLabel->hide();
		groupTypeHelp->hide();
		groupTypeEdit->hide();
		groupTypeButton->hide();
		groupNewTypeHelp->hide();
	}

	if ( graphicalEditor ) {
		// use the graphical editor instead of creating the default widget for setting parameters
		groupBox->setTitle( "Graphical Editor" );
		groupBoxLay->addWidget( graphicalEditor, 0, 0 );
		groupBox->setEnabled( true );
		newParamList->clear();
		newParamBox->hide();
	} else {
		groupBox->setTitle( "Group's Parameters" );
		//--- creating the widget for editing the group's paramenters
		QStringList parameters = confParams->getParametersList( groupInfo.fullPath );
		parameters.sort();
		for( int i=0; i<parameters.size(); i++ ) {
			if (parameters[i] == "type") {
				continue;
			}
			const QString paramFullPath = groupInfo.fullPath + GroupSeparator + parameters[i];

			QWidget* widget = createParameterEditorWidget(groupBox,
									confParams,
									groupInfo.fullPath+"/"+parameters[i],
									parameters[i],
									this,
									SLOT(changeParamValue()),
									true);
			widgetToParam[ widget->findChild<QWidget*>( "parameterEditorEditor" ) ] = parameters[i];
			groupBoxLay->addWidget( widget, i+2, 0 );
			bool mandatoryParameter = false;
			try {
				if (ConfigurationHelper::getDescriptorForParameter(*confParams, paramFullPath).props().testFlag(ParamIsMandatory)) {
					mandatoryParameter = true;
				}
			} catch (ParameterOrSubgroupNotDescribedException&) {
			}

			if (mandatoryParameter) {
				// it's a mandatory parameter cannot be deleted
				QToolButton* del = new QToolButton( groupBox );
				del->setIcon( QIcon(Total99Resources::findResource("mandatoryParameter.png")) );
				del->setAutoRaise( true );
				groupBoxLay->addWidget( del, i+2, 1 );
				widgetToParam[del] = parameters[i];
			} else {
				// create the del button
				QToolButton* del = new QToolButton( groupBox );
				del->setIcon( QIcon(Total99Resources::findResource("deleteParameter.png")) );
				del->setAutoRaise( true );
				groupBoxLay->addWidget( del, i+2, 1 );
				widgetToParam[del] = parameters[i];
				connect( del, SIGNAL(clicked()), this, SLOT(deleteParameter()) );
			}
		}
		groupBox->setEnabled( true );

		//--- fill the newParamList
		QStringList newList;
		try {
			const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(*confParams, groupInfo.fullPath);
			foreach(QString aParam, dc.parameters()) {
				QStringList params = confParams->getParametersWithPrefixList( groupInfo.fullPath, aParam );
				bool allowMultiple = dc.parameterDescriptor(aParam).props().testFlag(ParamAllowMultiple);
				QString postfix = allowMultiple ? QString(":%1").arg(params.size()) : "";
				if ( params.size() == 0 || allowMultiple ) {
					// add this to the ComboBox
					newList << aParam+postfix;
				}
			}
		} catch (ParameterOrSubgroupNotDescribedException&) {
		}

		newList.sort();
		newParamList->clear();
		newParamList->addItems( newList );
		newParamBox->setEnabled( true );
		newParamBox->show();
	}
	if ( graphicalEditor==NULL ||
		 ( graphicalEditor!=NULL && !graphicalEditor->configuringSubgroups() ) ) {
		//--- fill the newSubgroupList
		QStringList newList;
		try {
			const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(*confParams, groupInfo.fullPath);
			foreach(QString aSubgroup, dc.subgroups()) {
				QStringList subgroups = confParams->getGroupsWithPrefixList( groupInfo.fullPath, aSubgroup );
				bool allowMultiple = dc.subgroupDescriptor(aSubgroup).props().testFlag(ParamAllowMultiple);
				QString postfix = allowMultiple ? QString(":%1").arg(subgroups.size()) : "";
				if ( subgroups.size() == 0 || allowMultiple ) {
					// add this to the ComboBox
					newList << aSubgroup+postfix;
				}
			}
		} catch (ParameterOrSubgroupNotDescribedException&) {
		}

		newList.sort();
		newSubgroupList->clear();
		newSubgroupList->addItems( newList );
		newSubgroupBox->setEnabled( true );
		newSubgroupBox->show();
	} else {
		newSubgroupList->clear();
		newSubgroupBox->hide();
	}
}

void GenericGroupEditor::unsetGroup() {
	//--- remove all previous widget
	foreach( QWidget* widget, widgetToParam.keys() ) {
		delete widget;
	}
	widgetToParam.clear();
	confParams = NULL;
	groupInfo.groupName = QString();
	groupParent = QString();
	groupBox->setEnabled( false );
	newParamBox->setEnabled( false );
}

void GenericGroupEditor::reloadGroup() {
	setGroup( groupInfo, confParams );
}

void GenericGroupEditor::renameGroup() {
	QLineEdit* field = dynamic_cast<QLineEdit*>( sender() );
	if ( !field ) return;
	try {
		confParams->renameGroup( groupInfo.fullPath, field->text() );
	} catch (...) {
		QMessageBox::warning( this, QString("Error on renaming group ") + groupInfo.groupName, QString("Error on renaming group ") + groupInfo.groupName + " to " + field->text() );
		return;
	}
	QString oldname = groupInfo.groupName;
	groupInfo.groupName = field->text();
	groupInfo.fullPath = groupParent + "/" + field->text();
	emit groupRenamed( groupInfo.groupName, oldname, groupParent, confParams );
	return;
}

void GenericGroupEditor::showNewTypeHelp() {
	QString newType = groupTypeEdit->currentText();
	if (TypesDB::instance().isTypeRegisteredAndComponent(newType)) {
		const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor(newType);
		if (d.shortHelp().isEmpty() && d.longHelp().isEmpty()) {
			groupNewTypeHelp->setText("");
			groupNewTypeHelp->hide();
		} else {
			groupNewTypeHelp->setText(d.shortHelp() + " "  + d.longHelp());
			groupNewTypeHelp->show();
		}
	}
}

void GenericGroupEditor::changeGroupType() {
	QString newType = groupTypeEdit->currentText();
	const QString typeParameterPath = groupInfo.fullPath + GroupSeparator + "type";
	if (!confParams->parameterExists(typeParameterPath)) {
		confParams->createParameter(groupInfo.fullPath, "type");
	}
	confParams->setValue(typeParameterPath, newType);

	// set the new help string
	const RegisteredComponentDescriptor& d = TypesDB::instance().typeDescriptor(newType);
	groupTypeHelp->setText(d.shortHelp());
	// this is important for updating the tree view
	emit parameterChanged( "type", newType, groupInfo.fullPath, confParams );
	//--- reload the parameters,
	//    this is necessary in order to reload the correct descriptions for the new type
	groupInfo.actualType = newType;
	setGroup( groupInfo, confParams );
}

void GenericGroupEditor::changeParamValue() {
	//--- call the delegate depending on the type of editor used
	QString delegateMethod = QString( "changeParamValue" ) + sender()->metaObject()->className();
	this->metaObject()->invokeMethod( this, delegateMethod.toLatin1().data() );
};

void GenericGroupEditor::changeParamValueQLineEdit() {
	QLineEdit* field = dynamic_cast<QLineEdit*>( sender() );
	if ( !field ) return;
	QString paramName = widgetToParam[field];
	try {
		confParams->setValue( groupInfo.fullPath+"/"+paramName, field->text() );
	} catch (...) {
		QMessageBox::warning( this, QString("Error on setting value"), QString("Error on setting value ") + field->text() + " for the parameter " + paramName + " of group " + groupInfo.groupName );
		return;
	}
	emit parameterChanged( paramName, field->text(), groupInfo.fullPath, confParams );
	return;
}

void GenericGroupEditor::changeParamValueQComboBox() {
	QComboBox* field = dynamic_cast<QComboBox*>( sender() );
	if ( !field ) return;
	QString paramName = widgetToParam[field];
	QString value = field->currentText();
	//--- the type is handled in a special way, this check is for safety and it should never happen
	if ( paramName == "type" ) return;
	try {
		confParams->setValue( groupInfo.fullPath+"/"+paramName, value );
	} catch (...) {
		QMessageBox::warning( this, QString("Error on setting value"), QString("Error on setting value ") + value + " for the parameter " + paramName + " of group " + groupInfo.groupName );
		return;
	}
	emit parameterChanged( paramName, value, groupInfo.fullPath, confParams );
	return;
}

void GenericGroupEditor::deleteParameter() {
	QToolButton* button = dynamic_cast<QToolButton*>( sender() );
	if ( !button ) return;
	QString paramName = widgetToParam[button];
	if ( ( QMessageBox::question( this, "Confirm elimination of parameter", "Are you sure to delete the parameter "+paramName+" ??", QMessageBox::Yes | QMessageBox::No) ) == QMessageBox::Yes ) {
		QString wasValue = confParams->getValue( groupInfo.fullPath+"/"+paramName );
		confParams->deleteParameter( groupInfo.fullPath, paramName );
		emit parameterRemoved( paramName, wasValue, groupInfo.fullPath, confParams );
		//--- reload the parameters for reconstructing the widgets
		setGroup( groupInfo, confParams );
	}
	return;
}

void GenericGroupEditor::addParameter() {
	QString newParam = newParamList->currentText();
	if ( newParam.isEmpty() ) return;
	QString defValue;
	try {
		const QString newParamPath = groupInfo.fullPath + GroupSeparator + newParam;
		const AbstractDescriptor& d = ConfigurationHelper::getDescriptorForParameter(*confParams, newParamPath);
		defValue = ConfigurationHelper::getDefaultForDescriptorAsString(d);
	} catch (ParameterOrSubgroupNotDescribedException&) {
	}
	confParams->createParameter( groupInfo.fullPath, newParam, defValue );
	emit parameterAdded( newParam, defValue, groupInfo.fullPath, confParams );
	//--- reload the parameters for reconstructing the widgets
	setGroup( groupInfo, confParams );
}

void GenericGroupEditor::addSubgroup() {
	QString newSubgroup = newSubgroupList->currentText();
	if ( newSubgroup.isEmpty() ) return;
	confParams->createSubGroup( groupInfo.fullPath, newSubgroup );
	// --- check if it has a type
	try {
		const AbstractDescriptorContainer& dc = ConfigurationHelper::getDescriptorForGroup(*confParams, groupInfo.fullPath);
		const SubgroupDescriptor& sd = dc.subgroupDescriptor(newSubgroup.split(':')[0]);
		if (!sd.componentType().isEmpty()) {
			confParams->createParameter(groupInfo.fullPath + GroupSeparator + newSubgroup, "type", sd.componentType());
		}
	} catch (ParameterOrSubgroupNotDescribedException&) {
	}
	emit groupAdded( newSubgroup, groupInfo.fullPath, confParams );
	//--- reload the parameters for reconstructing the widgets
	setGroup( groupInfo, confParams );
}
