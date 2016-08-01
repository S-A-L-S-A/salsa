/********************************************************************************
 *  SALSA - Utilities                                                           *
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

#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include "utilitiesconfig.h"

namespace salsa {

/*!  A command line option parser.
 *
 * This class is based on GetOpt class implemented by froglogic GbR (<contact@froglogic.com>).<br>
 * This class helps to overcome the repetitive, tedious and
 * error-prone task of parsing the command line options passed to your
 * application by the user. Specify the acceptable syntax with a
 * minimum of statements in a readable way, check it against the
 * actual arguments passed and find the retrieved values in variables
 * of your program.
 *
 * A command line that a user might have entered is:
 *
 * \code
 * app -v --config=my.cnf -Wall input.dat
 * \endcode
 *
 * The typical usage has three stages:
 *
 * -# Construct a parser specifying what arguments to parse
 * -# Set up the list of allowed and required options
 * -# Run the parser
 *
 * For the first step there are three different constructors that
 * either take arguments directly from \c main(), \c QApplication or a
 * user specified list. Setting up the accepted syntax is done by a
 * set of \c add functions like addSwitch(). The final step of running
 * the parser is simply done by calling parse().
 *
 * A short example implementing a \c --verbose switch:
 *
 * \code
 * int main(int argc, char **argv)
 * {
 *     OptionParser opts(argc, argv);
 *     bool verbose;
 *     opts.addSwitch("verbose", &verbose);
 *     if (!opts.parse())
 *         return 1;
 *     if (verbose)
 *         cout << "VERBOSE mode on" << endl;
 *     ...
 * \endcode
 *
 * For a better understanding of the function names we'll better
 * define some terms used in the API and its documentation:
 *
 * - \em Argument An argument is a plain text token like e.g. a file
 * name one typically passes to an editor when invoking it.
 * - \em Switch A switch is an on/off kind of argument without the need
 *   of additional information. Example: \c --debug.
 * - \em Option An option is a normally optional argument with a key-value
 * syntax like \c --output=out.txt or \c -I/usr/include.
 * - \em Short \em Option A short option is a one letter option with a
 * preceding dash. Like \c -v.
 * - \em Long \em Option A long option has a more verbose,
 * multi-letter name like \c --debug.
 * .
 *
 * \author froglogic GbR (GetOpt class)<contact@froglogic.com>
 * \author G. Massera (this modified and QT4 complaint version)
 */
class SALSA_UTIL_API OptionParser {
public:
    /*!
     * Constructs a command line parser from the arguments stored in a
     * previously created QApplication instance.
     *  Example usage:
     *  \code
     *  QApplication a(argc, argv);
     *
     *  OptionParser opt;
     *  \endcode
     *
     *  This constructor is probably the most convenient one to use in a
     *  regular Qt application. Note that QApplication may already have
     *  removed Qt (or X11) specific arguments. Also see
     *  QApplication::argv() and QApplication::argc().
     */
    OptionParser();

    /**
    Construct a command line parser from the array \a argv of string
    pointers with the size \a argc. Those parameters have the form
    typically found in the \c main() function. That means that you can
    simply pass on the arguments specified by the user of your
    application.

    Example usage:

    \code
    int main(int argc, char **argv) {
        OptionParser opt(argc, argv);
        ...
    }
    \endcode
    */
    OptionParser( int argc, char *argv[] );

    /**
    Construct a command line parser from the arguments specified in the
    list of arguments \a a. This constructor is convenient in those
    cases where you want to parse a command line assembled on-the-fly
    instead of relying on the \c argc and \c arg parameters passed to
    the \c main() function.
    */
    OptionParser( const QStringList &a );

    /*! \internal */
    OptionParser( int offset );

    /*! Boo */
    QString appName() const { return aname; }

    /**
    Adds a switch with the long name \a lname. If the switch is found
    during parsing the bool \a *b will bet set to true. Otherwise the
    bool will be initialized to false.

    Example:

    \code
    OptionParser opt;
    bool verbose;
    opt.addSwitch("verbose", &verbose);
    \endcode

    The boolean flag \c verbose will be set to true if \c --verbose has
    been specified in the command line; false otherwise.
    */
    void addSwitch( const QString &lname, bool *b );

    /**
    Registers an option with the short name \a s and long name \a l to
    the parser. If this option is found during parsing the value will
    be stored in the string pointed to by \a v. By default \a *v will
    be initialized to \c QString::null.
    */
    void addOption( char s, const QString &l, QString *v );

    /**
    Registers a long option \a l that can have a variable number of
    corresponding value parameters. As there currently is no way to
    tell the end of the value list the only sensible use of this option
    is at the end of the command line.

    Example:

    \code
    QStringList args;
    opt.addVarLengthOption("exec", &args);
    \endcode

    Above code will lead to "-f" and "test.txt" being stored in \a args
    upon

    \code
    myapp --exec otherapp -f test.txt
    \endcode
    */
    void addVarLengthOption( const QString &l, QStringList *v );

    /**
    Registers an option with the short name \a s that can be specified
    repeatedly in the command line. The option values will be stored in
    the list pointed to by \a v. If no \a s option is found \a *v will
    remain at its default value of an empty QStringList instance.

    Example:

    To parse the \c -I options in a command line like
    \code
    myapp -I/usr/include -I/usr/local/include
    \endcode

    you can use code like this:

    \code
    OptionParser opt;
    QStringList includes;
    opt.addRepeatableOption('I', &includes);
    opt.parse();
    \endcode
    */
    void addRepeatableOption( char s, QStringList *v );

    /**
    Registers an option with the long name \a l that can be specified
    repeatedly in the command line.

    \sa addRepeatableOption( char, QStringList* )
    */
    void addRepeatableOption( const QString &l, QStringList *v );

    /**
    Adds a long option \a l that has an optional value parameter. If
    the value is not specified by the user it will be set to \a def.

    Example:

    \code
    OptionParser opt;
    QString file;
    opt.addOptionalOption("dump", &file, "<stdout>");
    \endcode

    \sa addOption
    */
    void addOptionalOption( const QString &l, QString *v, const QString &def );

    /**
    Adds a short option \a s that has an optional value parameter. If
    the value is not specified by the user it will be set to \a def.
    */
    void addOptionalOption( char s, const QString &l, QString *v, const QString &def );

    /**
    Registers a required command line argument \a name. If the argument
    is missing parse() will return false to indicate an error and \a *v
    will remain with its default QString::null value. Otherwise \a *v
    will be set to the value of the argument.

    Example:

    To accept simple arguments like

    \code
    myeditor letter.txt
    \endcode

    use a call like:

    \code
    QString &file;
    opt.addArgument("file", &file);
    \endcode

    Note: the \a name parameter has a rather descriptive meaning for
    now. It might be used for generating a usage or error message in
    the future. Right now, the only current use is in relation with the
    isSet() function.
    */
    void addArgument( const QString &name, QString *v );

    /**
    Registers an optional command line argument \a name. For a more
    detailed description see the addArgument() documentation.

    */
    void addOptionalArgument( const QString &name, QString *v );

    /*! \internal */
    bool parse( bool untilFirstSwitchOnly );

    /**
    Parse the command line arguments specified in the constructor under
    the conditions set by the various \c add*() functions. On success,
    the given variable reference will be initialized with their
    respective values and true will be returned. Returns false
    otherwise.

    In the future there'll be a way to retrieve an error message. In
    the current version the message will be printed to \c stderr.
    */
    bool parse() { return parse( false ); }

    /**
    Returns true if the (long) option or switch \a name has been found
    in the command line; returns false otherwise. Leading hyphens are
    not part of the name.

    As the set/not set decision can also be made depending on the value
    of the variable reference used in the respective \c add*() call
    there's generally little use for this function.
    */
    bool isSet( const QString &name ) const;

    /*! \internal */
    int currentArgument() const { return currArg; }

private:
    enum OptionType { OUnknown, OEnd, OSwitch, OArg1, OOpt, ORepeat, OVarLen };

    struct Option;
    friend struct Option;

    struct Option {
        Option( OptionType t = OUnknown,
                char s = 0, const QString &l = QString::null )
            : type( t ),
              sname( s ),
              lname( l ),
              boolValue( 0 ) { }

        OptionType type;
        char sname;		// short option name (0 if none)
        QString lname;	// long option name  (null if none)
        union {
            bool *boolValue;
            QString *stringValue;
            QStringList *listValue;
        };
        QString def;
    };

    QList<Option> options;
    typedef QList<Option>::const_iterator OptionConstIterator;
    QMap<QString, int> setOptions;

    void init( int argc, char *argv[], int offset = 1 );
    void init( const QStringList& arguments, int offset = 1 );
    void addOption( Option o );
    void setSwitch( const Option &o );

    QStringList args;
    QString aname;

    int numReqArgs;
    int numOptArgs;
    Option reqArg;
    Option optArg;

    int currArg;
};

} // end namespace salsa

#endif

