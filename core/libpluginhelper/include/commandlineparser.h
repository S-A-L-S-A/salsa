/********************************************************************************
 *  FARSA                                                                       *
 *  Copyright (C) 2007-2012                                                     *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
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

#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QString>
#include <QStringList>
#include <QDebug>

namespace farsa {

namespace pluginHelper {

/**
 * \brief The command line parser for the farsapluginhelper application
 *
 * This command line parser is extremely specific, the command line options are
 * hard coded. The parser throws a farsa::RuntimeUserException exception in
 * case of parsing errors
 */
class CommandLineParser
{
public:
	/**
	 * \brief Constructor
	 *
	 * This also executes the parsing of the command line arguments
	 * \param arguments the list of arguments (you can pass here the output
	 *                  of QCoreApplication::arguments())
	 */
	CommandLineParser(QStringList arguments);

	/**
	 * \brief Returns true if the command line help should be printed
	 *
	 * The application should exit after printing the help message
	 * \return true if the help message should be printed
	 */
	bool printHelp() const
	{
		return m_printHelp;
	}

	/**
	 * \brief Returns the name of the plugin
	 *
	 * \return the name of the plugin
	 */
	const QString& pluginName() const
	{
		return m_pluginName;
	}

	/**
	 * \brief Returns the destination directory
	 *
	 * \return the destination directory
	 */
	const QString& destDir() const
	{
		return m_destDir;
	}

	/**
	 * \brief Returns the destination directory of parsed headers
	 *
	 * \return the destination directory of parsed headers
	 */
	const QString& headersDestDir() const
	{
		return m_headersDestDir;
	}

	/**
	 * \brief Returns the name of the source files to generate
	 *
	 * This is the name without extension
	 * \return the name of the source files to generate
	 */
	const QString& genSource() const
	{
		return m_genSource;
	}

	/**
	 * \brief Returns the file with the list of dependencies to add
	 *
	 * \return the file with the list of dependencies to add
	 */
	const QString& dependenciesFile() const
	{
		return m_dependenciesFile;
	}

	/**
	 * \brief Returns the list of headers to parse
	 *
	 * \return the list of headers to parse
	 */
	const QStringList& headers() const
	{
		return m_headers;
	}

	/**
	 * \brief Returns a string with the command line help
	 */
	QString helpMessage() const;

private:
	/**
	 * \brief The function doing the actual parsing
	 *
	 * This throws a farsa::RuntimeUserException exception in case of errors
	 */
	void parse();

	/**
	 * \brief The command line arguments
	 *
	 * m_arguments[0] hsould be the application name
	 */
	const QStringList m_arguments;

	/**
	 * \brief True if the command line help should be printed
	 */
	bool m_printHelp;

	/**
	 * \brief The name of the plugin
	 */
	QString m_pluginName;

	/**
	 * \brief The destination directory
	 */
	QString m_destDir;

	/**
	 * \brief The destination directory of parsed headers
	 */
	QString m_headersDestDir;

	/**
	 * \brief The name of the source files to generate
	 */
	QString m_genSource;

	/**
	 * \brief The file with the list of dependencies to add
	 */
	QString m_dependenciesFile;

	/**
	 * \brief The list of headers to parse
	 */
	QStringList m_headers;
};

}

}

#endif
