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

#ifndef INI_FILE_SUPPORT_H
#define INI_FILE_SUPPORT_H

#include "configurationconfig.h"
#include "parametersfileloadersaver.h"
#include <QString>
#include <QTextStream>
#include <QList>
#include <QPair>

namespace salsa {

/**
 * \brief A class to escape a string that has to be saved as a value or key in
 *        an ini file
 *
 * Construct an object passing the string to mangle in the constructor. Then
 * call either escape() or unescape() to get the mangled string
 * The characters that are escaped are:
 * 	- leading space is substituted with '\ '. Spaces in the middle of a
 * 	  string or at the end are not modified;
 * 	- newlines and carriage returns are substituted with '\n' and '\r'
 * 	  respectively;
 * 	- the equal sign ('=') is  substituted with '\=';
 * 	- the forward slash ('/') is substituted with '//';
 * 	- '#' and ';' are substituted with '\#' and '\;' respectively;
 * 	- '[' and ']' are substituted with '\[' and '\]' respectively.
 * \internal
 */
class IniFileStringMangler
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param original the string to escape or unescape
	 */
	IniFileStringMangler(QString original);

	/**
	 * \brief Returns the escaped version of the original string
	 *
	 * \return the escaped version of the original string
	 */
	QString escape() const;

	/**
	 * \brief Returns the unescaped version of the original string\
	 *
	 * \return the unescaped version of the original string\
	 */
	QString unescape() const;

private:
	QString escapeSlashes(QString str) const;
	QString escapeNewlines(QString str) const;
	QString escapeEqualSign(QString str) const;
	QString escapeSharpSign(QString str) const;
	QString escapeSemicolonSign(QString str) const;
	QString escapeSquareBrackets(QString str) const;
	QString addInitialSlashIfStartinWithSpace(QString str) const;
	QString unescapeInitialSlashIfPresent(QString str) const;
	QString unescapeSquareBrackets(QString str) const;
	QString unescapeSemicolonSign(QString str) const;
	QString unescapeSharpSign(QString str) const;
	QString unescapeEqualSign(QString str) const;
	QString unescapeNewlines(QString str) const;
	QString unescapeSlashes(QString str) const;

	QString m_original;
};

/**
 * \brief The class to load/save configuration parameters to ini files
 *
 * Comments in the INI file can only be present at the beginning of a line (not
 * after a parameter value). To start a comment put # or ; at the beginning of a
 * line. Key and value are separated exactly by " = " (the spaces are important,
 * they are taken into account when reading the key and value).
 * \note The root group is not explicitly saved to file and all its parameters
 *       are written outside any group
 *
 * \ingroup configuration_configuration
 */
class SALSA_CONF_API IniFileLoaderSaver : public ParametersFileLoaderSaver
{
public:
	/**
	 * \brief Constructor
	 */
	IniFileLoaderSaver();

	/**
	 * \brief Destructor
	 */
	virtual ~IniFileLoaderSaver();

protected:
	/**
	 * \brief The function actually load configuration from the given stream
	 *
	 * \param stream the stream from which data should be read
	 * \return true if loading was successful, false otherwise
	 */
	virtual bool loadParameters(QTextStream &stream, ConfigurationManager& configParams);

	/**
	 * \brief The function actually save configuration to the given stream
	 *
	 * \param stream the stream to which data should be write
	 * \return true if loading was successful, false otherwise
	 */
	virtual bool saveParameters(QTextStream &stream, const ConfigurationManager& configParams);

private:
	// Returns the sequence that separates the key and the value
	static const QString& keyValueSeparator();

	// Recursively write to the outstream all parameters and subgroups
	// rooted at groupPath
	bool writeGroupToStream(QString groupPath, QTextStream &outstream, const ConfigurationManager& configParams);

	bool isCommentOrEmptyLine(QString line) const;
	// Returns false if line does not contain a group. Also sets m_currentGroup if
	// line contains the declaration of a group (unescaping it)
	bool extractGroup(QString line);
	// The first element of the pair is the key and the second is the value. Both
	// are unescaped
	QPair<QString, QString> extractKeyAndValue(QString line) const;

	// The current group when loading from file
	QString m_currentGroup;
};

} // end namespace salsa

#endif
