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

#include <QtTest/QtTest>
#include "utilitiesconfig.h"

// NOTES AND TODOS
//
// This is just an empty file, created as placeholder for unit tests of the
// 	salsa utilities library.

using namespace salsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class UtilitiesDummy_Test : public QObject
{
	Q_OBJECT

private slots:
	void dummy()
	{
	}
};

QTEST_MAIN(UtilitiesDummy_Test)
#include "utilitiesdummy_test.moc"
