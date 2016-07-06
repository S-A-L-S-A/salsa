/********************************************************************************
 *  FARSA Utilities Library                                                     *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#ifndef LOGGER_H
#define LOGGER_H

#include "utilitiesconfig.h"
#include <QString>

class QTextEdit;

namespace farsa {

/*! \brief Logger class provides methods for logging messages
 *
 * Logger has only static method. There is no need to instantiate it, just
 * use the static method appropriate to log your message.
 *
 * The Logger automatically add a timestamp and the information about the level
 * of message. You just need to provide the message to log.
 *
 * Be careful which methods you call from which thread: not every method in this
 * class is thread safe (see method description). When you call a method that is
 * not thread-safe you must be sure no other thread is accessing the logger
 *
 * \note It does not provides methods for formatting a message, but it uses
 * QString. Hence, use the QString methods for formatting the message
 *
 * \note Do not use the logger after returning from the main method (during
 * static data cleanup)
 *
 *  \ingroup utilities_misc
 */
class FARSA_UTIL_API Logger {
public:
	enum LogLevel {
		//! Logs everything
		LogAll = 0,
		//! Only warnings and errors are logged
		Warning,
		//! Only errors are logged
		Quiet,
		//! Nothing is logged
		Superquiet
	};

public:
	/*! Log an informative message. This method is thread-safe
	 * \param msg the message to log (timestamp & info level info are prepend to msg)
	 */
	static void info( QString msg );
	/*! Log a warning message. This method is thread-safe
	 * \param msg the message to log (timestamp & info level info are prepend to msg)
	 */
	static void warning( QString msg );
	/*! Log an error message. This method is thread-safe
	 * \param msg the message to log (timestamp & info level info are prepend to msg)
	 */
	static void error( QString msg );
	/*! Set the filename where to log messages. This method is NOT thread-safe
	 * \param logfile is the filename (full path or relative to the working directory) where to store
	 * log messages
	 * \note the filename is always open in append mode
	 */
	static void setLogFilename( QString logfile );
	/*! Set a QTextEdit dedicated for showing log messages. This method is NOT thread-safe */
	static void setQTextEdit( QTextEdit* textedit );
	/*! Enable/Disable the printing of log messages on the standard out. This method is NOT thread-safe
	 * \param enabled if true Logger will print on standard out the messages, otherwise no
	 */
	static void enableStdOut( bool enabled );
	/*! Sets the current log level (which influences what is logged and what isn't). This method is NOT thread-safe
	 * \param level the new log level
	 */
	static void setLogLevel(LogLevel level);
	/*! Returns the string representation of the given log level. This method is thread-safe
	 * \param level the log level
	 * \return the string representation of the given log level
	 */
	static QString logLevelToString(LogLevel level);
	/*! Returns the log level given its string representation. This method is thread-safe
	 * \param level the string representation of a log level (this is case insensitive)
	 * \return the log level corresponding to the given string (or Default if the string is invalid)
	 */
	static LogLevel stringToLogLevel(QString level);
private:
	/*! Private Constructor (not implemented) */
	Logger();
	/*! Private Destructor (not implemented) */
	~Logger();
};

} // end namespace farsa

#endif
