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

#include "parametersfileloadersaver.h"
#include "configurationmanager.h"
#include <QtDebug>
#include <QFile>

namespace farsa {

ParametersFileLoaderSaver::ParametersFileLoaderSaver()
{
}

ParametersFileLoaderSaver::~ParametersFileLoaderSaver()
{
}

bool ParametersFileLoaderSaver::load(QString filename, ConfigurationManager& configParams, bool keepOld)
{
	// If prepareLoading hasn't been called before this function, returning false
	if (filename.isNull()) {
		return false;
	}

	// Opening the file
	QFile file(filename);

	// Checking we were able to open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		//qWarning() << "WARNING: Impossible to read data from " << filename;
		return false;
	}

	// First deleting all old properties if we have to
	if (!keepOld) {
		configParams.clearAll();
	}

	// Creating the textstream to load data
	QTextStream in(&file);

	// Loading file
	try {
		return loadParameters(in, configParams);
	} catch (std::exception& e) {
		qDebug() << "Exception thrown while loading file, reason:" << e.what();

		return false;
	}
}

bool ParametersFileLoaderSaver::save(QString filename, const ConfigurationManager& configParams)
{
	// If prepareSaving hasn't been called before this function, returning false
	if (filename.isNull()) {
		return false;
	}

	// Opening the file
	QFile file(filename);

	// Checking we were able to open the file
	if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		//qWarning() << "WARNING: Impossible to write data to " << filename;
		return false;
	}

	// Creating the textstream to save data
	QTextStream out(&file);

	// Saving file
	try {
		return saveParameters(out, configParams);
	} catch (std::exception& e) {
		qDebug() << "Exception thrown while saving file, reason:" << e.what();

		return false;
	}
}

} // end namespace farsa
