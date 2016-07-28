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

#ifndef PARAMETERS_FILE_LOADER_SAVER_H
#define PARAMETERS_FILE_LOADER_SAVER_H

#include "configurationconfig.h"
#include <QString>
#include <QTextStream>

namespace salsa {

class ConfigurationManager;

/**
 * \brief The base for all classes able to load/save configuration parameters
 *        from/to file
 *
 * When subclassing this, you should only override loadParameters and
 * saveParameters
 *
 * \ingroup configuration_configuration
 */
class SALSA_CONF_API ParametersFileLoaderSaver
{
public:
	/**
	 * \brief Constructor
	 */
	ParametersFileLoaderSaver();

	/**
	 * \brief Destructor
	 */
	virtual ~ParametersFileLoaderSaver();

	/**
	 * \brief Load configuration from the given file and store them to
	 *        ConfigurationManager
	 *
	 * \param filename the name of the file from which configuration should
	 *                 be loaded
	 * \param configParams the ConfigurationManager whose data will be
	 *                     stored
	 * \param keepOld if true previous parameters are deleted before loading
	 *                the new ones, otherwise they are kept (however they
	 *                can be overridden)
	 * \return true on success, false otherwise
	 */
	bool load(QString filename, ConfigurationManager& configParams, bool keepOld = false);

	/**
	 * \brief Save configuration from the ConfigurationManager to the given
	 *        file
	 *
	 * \param filename the name of the file to which configuration will be
	 *                 saved
	 * \param configParams the ConfigurationManager to save on file
	 */
	bool save(QString filename, const ConfigurationManager& configParams);

protected:
	/**
	 * \brief The function to actually load configuration parameters from
	 *        the given stream
	 *
	 * Implement in subclasses
	 * \param stream the stream from which data should be read
	 * \return true if loading was successful, false otherwise
	 */
	virtual bool loadParameters(QTextStream& stream, ConfigurationManager& configParams) = 0;

	/**
	 * \brief The function to actually save configuration to the given
	 *        stream
	 *
	 * Implement in subclasses
	 * \param stream the stream to which data should be written
	 * \return true if saving was successful, false otherwise
	 */
	virtual bool saveParameters(QTextStream& stream, const ConfigurationManager& configParams) = 0;
};

} // end namespace salsa

#endif
