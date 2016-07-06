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

#include "commandlineparser.h"
#include "utilitiesexceptions.h"

namespace farsa {

namespace pluginHelper {

CommandLineParser::CommandLineParser(QStringList arguments)
	: m_arguments(arguments)
	, m_printHelp(false)
	, m_pluginName()
	, m_destDir()
	, m_headersDestDir()
	, m_genSource()
	, m_dependenciesFile()
	, m_headers()
{
	// Parsing the command line
	parse();
}

QString CommandLineParser::helpMessage() const
{
	const QString appName = m_arguments[0];

	const QString msg = "\
The " + appName + " command is used to parse headers and generate commands to\n\
register classes into the FARSA Factory and to generate the function returning\n\
the list of dependencies.\n\
\n\
Usage:\n\
\t" + appName + " <pluginname> <destdir> <headersdestdir> <gensource> <depfile> <headers>\n\
\n\
The program parses the headers listed on the command line and generates a header\n\
and source files in the destination directory containing the declaration and\n\
definition of a subclass of the FarsaPlugin class to register classes into the\n\
factory and to return the list of dependencies read from depfile. Moreover for\n\
each header a file is created inside headersdestdir with the same name and\n\
content of the input header except that the FARSA_PLUGIN_API,\n\
FARSA_PLUGIN_TEMPLATE and FARSA_PLUGINS_INTERNAL macros are replaced with\n\
FARSA_PLUGIN_API_IMPORT, FARSA_PLUGIN_TEMPLATE_IMPORT and \n\
FARSA_PLUGINS_INTERNAL_IMPORT respectively. The program return value is 0 for\n\
successfull execution, 1 in case of error. If an error occurs, an explanation is\n\
printed on the standard error stream.\n\
\t<pluginname>     The name of the plugin being compiled. The name\n\
\t                 cannot be \"--wn\"\n\
\t<destdir>        The directory that will contain generated files. The\n\
\t                 directory must exist\n\
\t<headersdestdir> The directory that will contain parsed headers. The\n\
\t                 directory must exist\n\
\t<gensource>      The name (without extension) of the files to generate\n\
\t                 with registration commands. <gensource>.h and\n\
\t                 <gensource>.cpp will be generated inside <destdir>\n\
\t<depfile>        The name of the file containing the list of list of\n\
\t                 dependencies. Dependencies inside the file must be\n\
\t                 separated by ;\n\
\t<headers>        The list of headers to parse. For each header, a\n\
\t                 parsed file having the same name will be generated\n\
\t                 inside <headersdestdir>\n\
\n\
Help message:\n\
\t" + appName + " --help\n\
If called this way, the application prints this help message and exists.\n\
\n";

	return msg;
}

void CommandLineParser::parse()
{
	if (m_arguments.size() < 2) {
		farsa::throwUserRuntimeError("Error parsing the command line: invalid usage: you must provide at least one argument. Call with --help to see the help message");
	} else if (m_arguments[1] == "--help") {
		m_printHelp = true;

		// Ending pasing here
		return;
	} else if (m_arguments.size() < 7) {
		farsa::throwUserRuntimeError("Error parsing the command line: invalid usage: when not called with --help we need at least 6 arguments. Call with --help to see the help message");
	}

	// Getting all the parameters
	m_pluginName = m_arguments[1];
	m_destDir = m_arguments[2];
	m_headersDestDir = m_arguments[3];
	m_genSource = m_arguments[4];
	m_dependenciesFile = m_arguments[5];
	m_headers = m_arguments.mid(6);
}

}

}
