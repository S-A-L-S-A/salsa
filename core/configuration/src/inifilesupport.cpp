/***************************************************************************
 *   Copyright (C) 2008 by Tomassino Ferrauto                              *
 *   t_ferrauto@yahoo.it                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "private/inifilesupport.h"
#include "configurationmanager.h"
#include <QtDebug>
#include <QFile>
#include <QRegExp>

namespace farsa {

IniFileStringMangler::IniFileStringMangler(QString original)
	: m_original(original)
{
}

QString IniFileStringMangler::escape() const
{
	QString escaped;

	escaped = escapeSlashes(m_original);
	escaped = escapeNewlines(escaped);
	escaped = escapeEqualSign(escaped);
	escaped = escapeSharpSign(escaped);
	escaped = escapeSemicolonSign(escaped);
	escaped = escapeSquareBrackets(escaped);
	escaped = addInitialSlashIfStartinWithSpace(escaped);

	return escaped;
}

QString IniFileStringMangler::unescape() const
{
	QString unescaped;

	unescaped = unescapeInitialSlashIfPresent(m_original);
	unescaped = unescapeSquareBrackets(unescaped);
	unescaped = unescapeSemicolonSign(unescaped);
	unescaped = unescapeSharpSign(unescaped);
	unescaped = unescapeEqualSign(unescaped);
	unescaped = unescapeNewlines(unescaped);
	unescaped = unescapeSlashes(unescaped);

	return unescaped;
}

QString IniFileStringMangler::escapeSlashes(QString str) const
{
	const QString escaped = str.replace('\\', "\\\\");

	return escaped;
}


QString IniFileStringMangler::escapeNewlines(QString str) const
{
	const QString escaped = str.replace('\n', "\\n").replace('\r', "\\r");

	return escaped;
}

QString IniFileStringMangler::escapeEqualSign(QString str) const
{
	const QString escaped = str.replace('=', "\\=");

	return escaped;
}

QString IniFileStringMangler::escapeSharpSign(QString str) const
{
	const QString escaped = str.replace('#', "\\#");

	return escaped;
}

QString IniFileStringMangler::escapeSemicolonSign(QString str) const
{
	const QString escaped = str.replace(';', "\\;");

	return escaped;
}

QString IniFileStringMangler::escapeSquareBrackets(QString str) const
{
	const QString escaped = str.replace('[', "\\[").replace(']', "\\]");

	return escaped;
}

QString IniFileStringMangler::addInitialSlashIfStartinWithSpace(QString str) const
{
	const QString spaceEscape = (str[0].isSpace()) ? "\\" : "";

	return spaceEscape + str;
}

QString IniFileStringMangler::unescapeInitialSlashIfPresent(QString str) const
{
	const QString unescaped = str.startsWith("\\ ") ? str.mid(1) : str;

	return unescaped;
}

QString IniFileStringMangler::unescapeSquareBrackets(QString str) const
{
	const QString unescaped = str.replace("\\[", "[").replace("\\]", "]");

	return unescaped;
}

QString IniFileStringMangler::unescapeSemicolonSign(QString str) const
{
	const QString unescaped = str.replace("\\;", ";");

	return unescaped;
}

QString IniFileStringMangler::unescapeSharpSign(QString str) const
{
	const QString unescaped = str.replace("\\#", "#");

	return unescaped;
}

QString IniFileStringMangler::unescapeEqualSign(QString str) const
{
	const QString unescaped = str.replace("\\=", "=");

	return unescaped;
}

QString IniFileStringMangler::unescapeNewlines(QString str) const
{
	const QString unescaped = str.replace("\\n", "\n").replace("\\r", "\r");

	return unescaped;
}

QString IniFileStringMangler::unescapeSlashes(QString str) const
{
	const QString unescaped = str.replace("\\\\", "\\");

	return unescaped;
}

IniFileLoaderSaver::IniFileLoaderSaver()
	: ParametersFileLoaderSaver()
	, m_currentGroup()
{
}

IniFileLoaderSaver::~IniFileLoaderSaver()
{
}

bool IniFileLoaderSaver::loadParameters(QTextStream &stream, ConfigurationManager& configParams)
{
	// Regular expressions for groups
	const QRegExp group("^\\[(.*)\\]$");

	m_currentGroup = "";
	while(!stream.atEnd()) {
		// Reading a line and removing comments
		const QString line = stream.readLine();

		// If it is a comment line, skipping it
		if (isCommentOrEmptyLine(line)) {
			continue;
		}

		// Checking whether the line is a group
		if (extractGroup(line)) {
			configParams.createGroup(m_currentGroup);
		} else {
			// If we get here we should have a property of m_currentGroup
			QPair<QString, QString> keyAndValue = extractKeyAndValue(line);

			configParams.createParameter(m_currentGroup, keyAndValue.first, keyAndValue.second);
		}
	}

	return true;
}

bool IniFileLoaderSaver::saveParameters(QTextStream &stream, const ConfigurationManager& configParams)
{
	// Recursevly write all groups starting from root
	return writeGroupToStream("", stream, configParams);
}

const QString& IniFileLoaderSaver::keyValueSeparator()
{
	static const QString separator(" = ");

	return separator;
}

bool IniFileLoaderSaver::writeGroupToStream(QString groupPath, QTextStream &outstream, const ConfigurationManager& configParams) {
	// Writing parameters first
	QStringList paramList = configParams.getParametersList(groupPath);
	foreach(QString param, paramList) {
		QString value = configParams.getValue(groupPath + GroupSeparator + param);
		outstream << IniFileStringMangler(param).escape() << keyValueSeparator() << IniFileStringMangler(value).escape() << "\n";
	}

	// Adding an extra empty row to divide sub-groups...
	outstream << "\n";

	// ...and then writing all subgroups recursively
	QStringList groupList = configParams.getGroupsList(groupPath);
	foreach(QString group, groupList) {
		QString subgroup = groupPath + GroupSeparator + group;

		// The responsability to write the group header is here so that we can avoid
		// writing "[ROOT]" for the root node
		outstream << "[" << IniFileStringMangler(subgroup.section('/',1)).escape() << "]" << "\n";

		writeGroupToStream(subgroup, outstream, configParams);
	}

	return true;
}

bool IniFileLoaderSaver::isCommentOrEmptyLine(QString line) const
{
	return line.startsWith('#') || line.startsWith(';') || line.trimmed().isEmpty();
}

bool IniFileLoaderSaver::extractGroup(QString line)
{
	const QRegExp groupRE("^\\[(.*)\\]$");

	int ret = groupRE.indexIn(line);

	if (ret == -1) {
		return false;
	}

	m_currentGroup = IniFileStringMangler(groupRE.cap(1)).unescape();
	return true;
}

QPair<QString, QString> IniFileLoaderSaver::extractKeyAndValue(QString line) const
{
	QPair<QString, QString> keyAndValue;
	keyAndValue.first = "";
	keyAndValue.second = "";

	// Looking for the first ' = ' sequence
	const int keyValueSeparatorPosition = line.indexOf(keyValueSeparator());

	// If no ' = ' sequence is found, the whole line is set as the value (the
	// key is empty)
	if (keyValueSeparatorPosition == -1) {
		keyAndValue.second = line;
	} else {
		keyAndValue.first = IniFileStringMangler(line.left(keyValueSeparatorPosition)).unescape();
		keyAndValue.second = IniFileStringMangler(line.mid(keyValueSeparatorPosition + keyValueSeparator().size())).unescape();
	}

	return keyAndValue;
}

} // end namespace farsa
