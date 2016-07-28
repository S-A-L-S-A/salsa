/***************************************************************************
 *  SALSA Configuration Library                                            *
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

#ifndef CONFIGURATION_WIDGET_H
#define CONFIGURATION_WIDGET_H

#include "configurationconfig.h"
#include <QWidget>

namespace salsa {

class ConfigurationManager;

/**
 * \brief The base class for widgets for editing configuration parameters
 *
 * This is the base class for all widgets that are used to change parameters of
 * groups. If a Component child class needs a particular widget to be configured
 * (because it would be difficult to manually set parameter values, think of
 * e.g. neural networks) it can declare which class to use to edit its
 * parameters in its describe() method. The TypesDB class can then be used to
 * obtain an instance of the editor. If no editor is registered, a default one
 * is used (which basically allows editing of parameters using LineEdits). The
 * ConfigurationManager object on which an instance of this will work (i.e. the
 * one which it will edit) is passed as a reference to the constructor along
 * with the prefix. Both are saved and accessible to subclasses. Once set, the
 * ConfigurationManager and prefix cannot be changed: you need to destroy and
 * recreate a new editor if you need to act on a new set of parameters. This
 * class has only one pure virtual function, namely configuringSubgroups(). It
 * returns a boolean value depending on whether this class will take care of all
 * the subgroups of the group given by prefix or not.
 */
class SALSA_CONF_API ConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * \brief Constructor
	 *
	 * \param params the ConfigurationManager instance with the
	 *               parameters we have to edit
	 * \param prefix the group whose parameters we edit
	 * \param parent the parent widget
	 * \param f the window flags for this widget
	 */
	ConfigurationWidget(ConfigurationManager& params, QString prefix, QWidget* parent = NULL, Qt::WindowFlags f = 0) :
		QWidget(parent, f),
		m_params(params),
		m_prefix(prefix)
	{
	}

	/**
	 * \brief Destructor
	 */
	virtual ~ConfigurationWidget()
	{
	}

	/**
	 * \brief Returns whether this will take care of editing the parameters
	 *        of subgroups or not
	 *
	 * Reimplement in subclasses to return true or false depending on
	 * whether this widget will take care of editing the parameters of the
	 * subgroups of the given group or not
	 * \return true if this editor will edit the parameters of subgroups,
	 *         false otherwise
	 */
	virtual bool configuringSubgroups() const = 0;

signals:
	/**
	 * \brief The signal emitted when parameters are changed
	 *
	 * This signal should be emitted by subclasses whenever some parameter
	 * changes. This does not give any information on which parameter has
	 * changed, it is only meant to signal the receiver that the
	 * ConfigurationManager has been modified. This way it can, for
	 * example, allow the user to save the new parameters
	 */
	void parametersChanged();

protected:
	/**
	 * \brief The ConfigurationManager instance with the parameters we
	 *        have to edit
	 */
	ConfigurationManager& m_params;

	/**
	 * \brief The group with parameters to edit
	 */
	const QString m_prefix;
};

} // end namespace salsa

#endif
