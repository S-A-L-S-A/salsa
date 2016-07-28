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

#ifndef COMPONENTUI_H
#define COMPONENTUI_H

#include "configurationconfig.h"
#include <QString>

class QMenu;
class QMenuBar;
class QWidget;

namespace salsa {

/**
 * \brief Helper class for storing information about a viewer for a Component
 *
 * When the Total99 interface is used, some Components define viewer widgets in
 * order to display graphic information about it and to allow user to
 * edit/manage it using the graphic interface.
 *
 * \ingroup configuration_factory
 */
class SALSA_CONF_TEMPLATE ComponentUIViewer
{
public:
	/**
	 * \brief Constructor
	 */
	ComponentUIViewer()
	{
	}

	/**
	 * \brief Constructor - initializes all data members
	 *
	 * \param viewer the viewer
	 * \param menuText the name to display on the views menu for this viewer
	 * \param iconFilename the icon filename to display on the toolbar for
	 *                     this viewer
	 * \param tooltip the description of the viewer displayed in a tooltip
	 */
	ComponentUIViewer(QWidget* viewer, QString menuText, QString iconFilename = QString(), QString tooltip = QString())
		: viewer(viewer)
		, menuText(menuText)
		, iconFilename(iconFilename)
		, tooltip(tooltip)
	{
	}

	/**
	 * \brief The viewer widget
	 */
	QWidget* viewer;

	/**
	 * \brief The name to display on the views menu for this viewer
	 */
	QString menuText;

	/**
	 * \brief The icon filename to display on the toolbar for this viewer
	 */
	QString iconFilename;

	/**
	 * \brief The description of the viewer displayed in a tooltip
	 */
	QString tooltip;
};

/**
 * \brief The ComponentUI is the base (abstract) class that manage/create the
 *        graphic user interface of a Component
 *
 * Subclasses of Component that have a graphic user interface should override
 * the Component::getUIManager() function to return an instance of a subclass of
 * ComponentUI with information about the uis and the actions that can be run on
 * the Component
 * \ingroup configuration_factory
 */
class SALSA_CONF_TEMPLATE ComponentUI
{
public:
	/**
	 * \brief Destructor
	 */
	virtual ~ComponentUI()
	{
	}

	/**
	 * \brief Fills the menu "Actions" of Total99 with the actions of the
	 *        Component
	 *
	 * Re-implement to fill the menu with all actions that the component
	 * makes available. The default implementation does nothing
	 * \param actionsMenu the menu to fill with actions
	 */
	virtual void fillActionsMenu(QMenu* actionsMenu)
	{
		Q_UNUSED(actionsMenu)
	}

	/**
	 * \brief Returns the list of all viewers for the Component with all
	 *        information needed to fill the menu "Views" of Total99
	 *
	 * Re-implement in subclasses to return all viewers for the component.
	 * The default implementation does nothing and returns an empty list
	 * \param parent the parent widget for all viewers created by this
	 *               ComponentUI
	 * \param flags the flags to pass to the constructor of viewers
	 * \return the list of viewers
	 */
	virtual QList<ComponentUIViewer> getViewers(QWidget* parent, Qt::WindowFlags flags)
	{
		Q_UNUSED(parent)
		Q_UNUSED(flags)

		return QList<ComponentUIViewer>();
	}

	/**
	 * \brief Adds additional menus to the menu bar of Total99
	 *
	 * Re-implement to add additional menus to the menu bar of Total99. The
	 * default implementation does noting
	 * \param menuBar is the menu bar of the Total99 application
	 */
	virtual void addAdditionalMenus(QMenuBar* menuBar)
	{
		Q_UNUSED(menuBar)
	}
};

} // end namespace salsa

#endif
