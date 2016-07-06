/********************************************************************************
 *  FARSA - Total99                                                             *
 *  Copyright (C) 2012-2013 Gianluca Massera <emmegian@yahoo.it>                *
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
#include "typesdb.h"
#include "configurationhelper.h"
#include "total99resources.h"
#include "componentdescriptors.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QToolButton>

using namespace farsa;

QWidget* createParameterEditorWidget(QWidget* parent, ConfigurationManager* confParams, QString paramPath, QString displayName, const QObject* receiver, const char* onChangeParamValueSlot, bool editable)
{
	QFrame* widget = new QFrame(parent);
	widget->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	widget->setObjectName("parameterEditorBox");
	QGridLayout* lay = new QGridLayout(widget);
	lay->setContentsMargins(QMargins(4, 0, 2, 0));
	QLabel* lb = new QLabel(displayName, widget);
	lb->setObjectName("parameterEditorName");
	lay->addWidget(lb, 0, 0);

	//--- create the editor depending on the type, and create the typeInfo string for later
	QWidget* edit;
	QString typeInfo;
	QString shortHelp;
	QString longHelp;
	if (!editable) {
		edit = new QLabel(QString(" = ") + confParams->getValue(paramPath), widget);
	} else {
		try {
			const AbstractDescriptor& abstractDescriptor = ConfigurationHelper::getDescriptorForParameter(*confParams, paramPath);
			shortHelp = abstractDescriptor.shortHelp();
			longHelp = abstractDescriptor.longHelp();
			switch (abstractDescriptor.type()) {
				case StringDescriptorType:
					{
						const StringDescriptor& d = dynamic_cast<const StringDescriptor&>(abstractDescriptor);

						QLineEdit* editor = new QLineEdit( confParams->getValue( paramPath ), widget );
						editor->setPlaceholderText( "Empty parameter -- Insert the value here" );

						if (d.props().testFlag(ParamIsList)) {
							editor->setValidator(new QRegExpValidator(QRegExp("(\\S+\\s{0,1})+"), editor));
						} else {
							editor->setValidator(new QRegExpValidator(QRegExp("\\S.*"), editor));
						}

						editor->connect(editor, SIGNAL(textEdited(const QString&)), receiver, onChangeParamValueSlot);

						edit = editor;
						typeInfo = "String (any value)";
					}
					break;
				case IntDescriptorType:
					{
						const IntDescriptor& d = dynamic_cast<const IntDescriptor&>(abstractDescriptor);

						QLineEdit* editor = new QLineEdit(confParams->getValue(paramPath), widget);
						editor->setPlaceholderText("Empty parameter -- Insert the value here");

						const int min = d.lowerBound();
						const int max = d.upperBound();

						if (d.props().testFlag(ParamIsList)) {
							editor->setValidator(new QIntValidator(min, max, editor));
						} else {
							editor->setValidator(new ListValidator(new QIntValidator(min, max, editor), editor));
						}

						editor->connect(editor, SIGNAL(textEdited(const QString&)), receiver, onChangeParamValueSlot);

						edit = editor;
						typeInfo = QString("Int [") + ((min == MinInteger) ? "any" : QString::number(min)) + ", " + ((max == MaxInteger) ? "any" : QString::number(max)) + "]";
					}
					break;
				case RealDescriptorType:
					{
						const RealDescriptor& d = dynamic_cast<const RealDescriptor&>(abstractDescriptor);

						QLineEdit* editor = new QLineEdit(confParams->getValue(paramPath), widget);
						editor->setPlaceholderText("Empty parameter -- Insert the value here");

						const double min = d.lowerBound();
						const double max = d.upperBound();

						if (d.props().testFlag(ParamIsList)) {
							editor->setValidator(new QDoubleValidator(min, max, 6, editor));
						} else {
							editor->setValidator(new ListValidator(new QDoubleValidator(min, max, 6, editor), editor));
						}

						editor->connect(editor, SIGNAL(textEdited(const QString&)), receiver, onChangeParamValueSlot);

						edit = editor;
						typeInfo = QString("Real [") + ((min == -Infinity) ? "any" : QString::number(min)) + ", " + ((max == +Infinity) ? "any" : QString::number(max)) + "]";
					}
					break;
				case BoolDescriptorType:
					{
						const BoolDescriptor& d = dynamic_cast<const BoolDescriptor&>(abstractDescriptor);

						if (d.props().testFlag(ParamIsList)) {
							// --- if it is a list then it will use a LineEdit
							QLineEdit* editor = new QLineEdit(confParams->getValue(paramPath), widget);
							editor->setPlaceholderText("Empty parameter -- Insert the value here");

							editor->setValidator(new ListValidator(new QRegExpValidator(QRegExp("[Tt]|[Tt][Rr][Uu][Ee]|[Ff]|[Ff][Aa][Ll][Ss][Ee]|0|1"), editor), editor));

							editor->connect(editor, SIGNAL(textEdited(const QString&)), receiver, onChangeParamValueSlot);

							edit = editor;
						} else {
							// --- if it is not a list then it will use a ComboBox
							QComboBox* editor = new QComboBox();
							editor->setEditable(false);
							editor->addItems(QStringList() << "true" << "false");

							if (ConfigurationHelper::getBool(*confParams, paramPath)) {
								editor->setCurrentIndex(0);
							} else {
								editor->setCurrentIndex(1);
							}

							editor->connect(editor, SIGNAL(currentIndexChanged(const QString)), receiver, onChangeParamValueSlot);

							edit = editor;
						}

						typeInfo = "Boolean (true, false)";
					}
					break;
				case EnumDescriptorType:
					{
						const EnumDescriptor& d = dynamic_cast<const EnumDescriptor&>(abstractDescriptor);

						if (d.props().testFlag(ParamIsList)) {
							// --- if it is a list then it will use a LineEdit
							QLineEdit* editor = new QLineEdit(confParams->getValue(paramPath), widget);
							editor->setPlaceholderText("Empty parameter -- Insert the value here");

							QStringList valuesForRE;
							const QStringList values = d.values();
							foreach (QString v, values) {
								valuesForRE.append(QRegExp::escape(v));
							}
							const QString valuesRE = valuesForRE.join("|");
							editor->setValidator(new ListValidator(new QRegExpValidator(QRegExp(valuesRE), editor), editor));

							editor->connect(editor, SIGNAL(textEdited(const QString&)), receiver, onChangeParamValueSlot);

							edit = editor;
						} else {
							// --- if it is not a list then it will use a ComboBox
							QComboBox* editor = new QComboBox( widget );
							editor->setEditable(false);
							editor->addItems(d.values());
							editor->setCurrentIndex(editor->findText(confParams->getValue(paramPath)));

							editor->connect(editor, SIGNAL(currentIndexChanged(const QString)), receiver, onChangeParamValueSlot);

							edit = editor;
						}

						typeInfo = "Enumeration (see help for values)";
					}
					break;
				case ComponentDescriptorType:
					{
						const ComponentDescriptor& d = dynamic_cast<const ComponentDescriptor&>(abstractDescriptor);

						QLineEdit* editor = new QLineEdit(confParams->getValue(paramPath), widget);
						editor->setPlaceholderText("Empty parameter -- Insert the value here");

						if (d.props().testFlag(ParamIsList)) {
							editor->setValidator(new QRegExpValidator(QRegExp("(\\S+\\s{0,1})+"), editor));
						} else {
							editor->setValidator(new QRegExpValidator(QRegExp("\\S.*"), editor));
						}

						editor->connect(editor, SIGNAL(textEdited(const QString&)), receiver, onChangeParamValueSlot);

						edit = editor;
						QString classType = d.componentType();
						typeInfo = QString("Group with type %1 (or compatible)").arg(classType);
					}
					break;
				default:
					edit = new QLabel(confParams->getValue(paramPath), widget);
					typeInfo = "Missing";
			}
		} catch (ParameterOrSubgroupNotDescribedException&) {
			edit = new QLabel(confParams->getValue(paramPath), widget);
			typeInfo = "Missing";
		}
	}

	edit->setObjectName("parameterEditorEditor");
	edit->setMinimumWidth(120);
	lay->addWidget(edit, 0, 1);
	lay->setColumnStretch(1, 2);
	lb = new QLabel(typeInfo, widget);
	lb->setObjectName("parameterEditorTypeInfo");
	lay->addWidget(lb, 0, 2);
	QToolButton* bt = new QToolButton(widget);
	bt->setIcon(QIcon(Total99Resources::findResource("toggleHelp.png")));
	bt->setAutoRaise(true);
	bt->setCheckable(true);
	lay->addWidget(bt, 0, 3);

	QString help;
	if (typeInfo == QString("Missing")) {
		help = "Can't resolve parameter's type - this parameter is not described";
	} else {
		help = shortHelp + " " + longHelp;
	}

	lb = new QLabel(help, widget);
	lb->setObjectName("parameterEditorShortHelp");
	lb->setWordWrap(true);
	lb->setVisible(false);
	lay->addWidget(lb, 1, 0, 1, 4);
	bt->connect(bt, SIGNAL(toggled(bool)), lb, SLOT(setVisible(bool)));

	return widget;
}

