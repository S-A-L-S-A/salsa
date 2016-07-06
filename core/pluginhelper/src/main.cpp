/********************************************************************************
 *  FARSA - Total99                                                             *
 *  Copyright (C) 2005-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#include <QCoreApplication>
#include <iostream>
#include "utilitiesexceptions.h"
#include "commandlineparser.h"
#include "pluginhelper.h"

using namespace farsa;
using namespace farsa::pluginHelper;

int main(int argc, char* argv[])
{
	// The qt core application
	QCoreApplication app(argc, argv);

	// Whether execution was ok or not
	bool ok = true;

	// Doing the actual work
	try {
		// Parsing the command line
		CommandLineParser parser(app.arguments());

		// Now checking what we have to do
		if (parser.printHelp()) {
			QTextStream output(stdout);
			output << parser.helpMessage() << "\n";
		} else {
			doParse(parser.pluginName(), parser.destDir(), parser.headersDestDir(), parser.genSource(), parser.dependenciesFile(), parser.headers());
		}
	} catch (farsa::RuntimeUserException& e) {
		std::cerr << "Execution exception: " << e.what() << std::endl;
		ok = false;
	} catch (farsa::CircularDependencyException& e) {
		std::cerr << "Circular dependency: " << e.what() << std::endl;
		ok = false;
	} catch (std::exception& e) {
		std::cerr << "Generic exception: " << e.what() << std::endl;
		ok = false;
	} catch (...) {
		std::cerr << "Unknown exception" << std::endl;
		ok = false;
	}

	return ok ? 0 : 1;
}

