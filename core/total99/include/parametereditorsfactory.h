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

#ifndef PARAMETEREDITORSFACTORY_H
#define PARAMETEREDITORSFACTORY_H

#include <QWidget>
#include <QValidator>
#include <QString>
#include <QStringList>

namespace farsa {
	class ConfigurationManager;
};

/*! a Validator for validating a list of element.
 *  It use another Validator for each element, and it use it
 *  on all elements assuming that they can be separated by spaces
 */
class ListValidator : public QValidator {
public:
	ListValidator( QValidator* itemValidator, QObject* parent=0 )
		: QValidator(parent), itemValidator(itemValidator) { /* nothing to do */ };
	QValidator::State validate( QString& input, int& pos ) const {
		QStringList splitted = input.split( ' ', QString::SkipEmptyParts );
		QValidator::State state = QValidator::Acceptable;
		foreach( QString item, splitted ) {
			QValidator::State itemState = itemValidator->validate( item, pos );
			if ( itemState == QValidator::Invalid ) {
				return QValidator::Invalid;
			}
			if ( itemState == QValidator::Intermediate ) {
				state = itemState;
			}
		}
		return state;
	};
private:
	QValidator* itemValidator;
};

/*! \brief create an editor widget for the parameter accordlying to its type description
 *  \param parent will be the parent of the returned widget
 *  \param confParams is the ConfigurationParameters owning the parameter
 *  \param paramPath is the path into the ConfigurationParameters
 *  \param displayName is the name to display for labelling the editor for this parameter; usually the parameter name
 *  \param receiver is the target object of the slot onChangeParamValueSlot
 *  \param onChangeParamValueSlot is the slot called when the value of the parameter is edited
 */
QWidget* createParameterEditorWidget(QWidget* parent, farsa::ConfigurationManager* confParams, QString paramPath, QString displayName, const QObject* receiver, const char* onChangeParamValueSlot, bool editable);

#endif
