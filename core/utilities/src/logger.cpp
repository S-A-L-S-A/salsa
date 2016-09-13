/********************************************************************************
 *  SALSA Utilities Library                                                     *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
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

#include "logger.h"
#include <QFile>
#include <QTextStream>
#include <QTextEdit>
#include <QDateTime>
#include <iostream>
#include <QObject>
#include <QEvent>
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QMessageBox>

namespace salsa {

namespace {
	//--- This and the following are utilities class for updating textEdit into the GUI Thread
	class TextToAppend : public QEvent {
	public:
		TextToAppend( QString text, QString level, QString pureMessage ) :
			QEvent((Type)type),
			text(text),
			level(level),
			pureMessage(pureMessage) { };
		QString getText() {
			return text;
		};
		QString getLevel() {
			return level;
		};
		QString getPureMessage() {
			return pureMessage;
		};
	private:
		static int type;
		QString text;
		QString level;
		QString pureMessage;
	};
	int TextToAppend::type = QEvent::registerEventType();

	class TextEditUpdater : public QObject {
	public:
		TextEditUpdater() :
			QObject(),
			textEdit(nullptr)
		{
		}

		void setTextEditToUpdate(QTextEdit* e)
		{
			textEdit = e;
		}

		bool hasTextEdit() const
		{
			return (textEdit != nullptr);
		}

	protected:
		virtual void customEvent( QEvent* event ) {
			// If textEdit is nullptr we will crash here (this should never happend)
			TextToAppend* tevent = dynamic_cast<TextToAppend*>( event );
			if ( tevent ) {
				textEdit->append( tevent->getText() );
				textEdit->moveCursor( QTextCursor::End );
				textEdit->moveCursor( QTextCursor::StartOfLine );
				if ( tevent->getLevel() == "ERROR" ) {
					QMessageBox::critical( 0, "Error from Component", tevent->getPureMessage() );
				}
				tevent->accept();
			} else {
				QObject::customEvent( event );
			}
		};

		QTextEdit* textEdit;
	};

	// This class contains the core functionalities for logging. It is implemented
	// as a singleton to have the correct initialization of all needed variables
	// when the Logger is used for the first time
	class LoggerImplementation
	{
	public:
		// Returns the only instance of this class
		static LoggerImplementation& getInstance();

		void info(QString msg);

		void warning(QString msg);

		void error(QString msg);

		void setQTextEdit(QTextEdit* textedit);

		void enableStdOut(bool enabled);

		void setLogLevel(Logger::LogLevel level);

		void setLogFilename(QString logfile);

	private:
		// Constructor
		LoggerImplementation();

		// Destructor
		~LoggerImplementation();

		// This is the main function for logging. This function is thread-safe
		void logIt(QString level, QString msg);

		bool stdOut;
		QFile* file;
		TextEditUpdater* textEditUpdater;
		Logger::LogLevel logLevel;
		// The semaphores protecting the streams. The logger can be called from multiple threads simultaneously
		QMutex outStreamMutex;
		QMutex fileStreamMutex;

	private:
		// Copy constructor, not implemented
		LoggerImplementation(LoggerImplementation&);

		// Copy operator, not implemented
		LoggerImplementation& operator=(LoggerImplementation&);
	};

	LoggerImplementation& LoggerImplementation::getInstance()
	{
		// The meyer singleton
		static LoggerImplementation loggerImplementation;

		return loggerImplementation;
	}

	void LoggerImplementation::info(QString msg)
	{
		if (logLevel <= Logger::LogAll) {
			logIt("INFO", msg);
		}
	}

	void LoggerImplementation::warning(QString msg)
	{
		if (logLevel <= Logger::Warning) {
			logIt("WARNING", msg);
		}
	}

	void LoggerImplementation::error(QString msg)
	{
		if (logLevel <= Logger::Quiet) {
			logIt("ERROR", msg);
		}
	}

	void LoggerImplementation::setQTextEdit(QTextEdit* textedit)
	{
		textEditUpdater->setTextEditToUpdate(textedit);
	}

	void LoggerImplementation::enableStdOut(bool enabled)
	{
		stdOut = enabled;
	}

	void LoggerImplementation::setLogLevel(Logger::LogLevel level)
	{
		logLevel = level;
	}

	void LoggerImplementation::setLogFilename(QString logfile)
	{
		delete file;
		file = new QFile(logfile);
	}

	LoggerImplementation::LoggerImplementation() :
		stdOut(true),
		file(nullptr),
		textEditUpdater(new TextEditUpdater()),
		logLevel(Logger::LogAll),
		outStreamMutex(),
		fileStreamMutex()
	{
	}

	LoggerImplementation::~LoggerImplementation()
	{
		delete textEditUpdater;
		delete file;

		// These lines are here to have a "clean" crash if somebody tries to access
		// the logger after returning from the main function
		textEditUpdater = nullptr;
		file = nullptr;
	}

	//--- this is the main function for logging. This function is thread-safe
	void LoggerImplementation::logIt(QString level, QString msg)
	{
		QString logtmpl("[%1] %2: %3");
		QString timestamp = QDateTime::currentDateTime().toString( "dd-MM-yyyy hh:mm:ss.zzz" );
		QString logmsg = logtmpl.arg( timestamp ).arg( level, -10 ).arg( msg );
		if ( stdOut ) {
			QMutexLocker locker(&outStreamMutex);

			QTextStream outStream(stdout, QIODevice::WriteOnly);
			outStream << logmsg << "\n";
		}
		if ( textEditUpdater->hasTextEdit() ) {
			// No semaphore here as we use the thread-safe postEvent function
			QString color = "#ffffff";
			if ( level == "INFO" ) {
				color = "#afeeee";
			} else if ( level == "WARNING" ) {
				color = "#f0e68c";
			} else if ( level == "ERROR" ) {
				color = "#ff4500";
			}
			// --- here the postEvent is used because it is not possible to modify directly the content of
			//     textEdit because it is not thread-safe. In fact, this function might be called from multiple
			//     threads and outside the GUI thread
			qApp->postEvent( textEditUpdater, new TextToAppend( QString("<pre style=\"margin-top: 0px; margin-bottom: 0px; color: ")+color+";\">"+logmsg+QString("</pre>"), level, msg ) );
			//textEdit->append( QString("<pre style=\"margin-top: 0px; margin-bottom: 0px; color: ")+color+";\">"+logmsg+QString("</pre>") );
			//textEdit->moveCursor( QTextCursor::End );
			//textEdit->moveCursor( QTextCursor::StartOfLine );
		}
		if ( file != nullptr ) {
			QMutexLocker locker(&fileStreamMutex);

			QTextStream fileStream;
			fileStream.setDevice(file);
			fileStream << logmsg << "\n";
		}
	}
} //end anonymous namespace for LoggerImplementation class

void Logger::info(QString msg)
{
	LoggerImplementation::getInstance().info(msg);
}

void Logger::warning(QString msg)
{
	LoggerImplementation::getInstance().warning(msg);
}

void Logger::error(QString msg)
{
	LoggerImplementation::getInstance().error(msg);
}

void Logger::setLogFilename(QString logfile)
{
	LoggerImplementation::getInstance().setLogFilename(logfile);
}

void Logger::setQTextEdit(QTextEdit* textedit)
{
	LoggerImplementation::getInstance().setQTextEdit(textedit);
}

void Logger::enableStdOut(bool enabled)
{
	LoggerImplementation::getInstance().enableStdOut(enabled);
}

void Logger::setLogLevel(LogLevel level) {
	LoggerImplementation::getInstance().setLogLevel(level);
}

QString Logger::logLevelToString(Logger::LogLevel level)
{
	QString str = "unknown";

	switch(level)
	{
		case LogAll:
			str = "LogAll";
			break;
		case Warning:
			str = "Warning";
			break;
		case Quiet:
			str = "Quiet";
			break;
		case Superquiet:
			str = "Superquiet";
			break;
	}

	return str;
}

Logger::LogLevel Logger::stringToLogLevel(QString level)
{
	LogLevel l = LogAll;

	if (level.toUpper() == "LOGALL") {
		l = LogAll;
	} else if (level.toUpper() == "WARNING") {
		l = Warning;
	} else if (level.toUpper() == "QUIET") {
		l = Quiet;
	} else if (level.toUpper() == "SUPERQUIET") {
		l = Superquiet;
	}

	return l;
}

} // end namespace salsa
