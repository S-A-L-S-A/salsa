/********************************************************************************
 *  FARSA - Utilities                                                           *
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

#define QT_NO_CAST_ASCII
#define QT_NO_ASCII_CAST

#include "optionparser.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QStack>
#include <cstdlib>
#include <cassert>

namespace farsa {

OptionParser::OptionParser() {
    QCoreApplication* qApp1 = QCoreApplication::instance();
    if ( !qApp1 ) {
	   qFatal( "OptionParser: requires a QCoreApplication instance to be constructed first" );
    }
    init( qApp1->arguments(), 1 );
}

OptionParser::OptionParser( int offset ) {
    QCoreApplication* qApp1 = QCoreApplication::instance();
    if ( !qApp1 ) {
	   qFatal( "OptionParser: requires a QApplication instance to be constructed first" );
    }
    init( qApp1->arguments(), offset );
}

OptionParser::OptionParser( int argc, char *argv[] ) {
    init( argc, argv );
}

OptionParser::OptionParser( const QStringList &a )
    : args( a ) {
    init( 0, 0 );
}

void OptionParser::init( int argc, char *argv[], int offset ) {
    numReqArgs = numOptArgs = 0;
    currArg = 1; // appname is not part of the arguments
    if ( argc ) {
        // application name
        aname = QFileInfo( QString::fromUtf8( argv[0] ) ).fileName();
        // arguments
        for ( int i = offset; i < argc; ++i ) {
            args.append( QString::fromUtf8( argv[i] ) );
        }
    }
}

void OptionParser::init( const QStringList& arguments, int offset ) {
    numReqArgs = numOptArgs = 0;
    currArg = 1; // appname is not part of the arguments
    if ( !arguments.isEmpty() ) {
        // application name
        aname = QFileInfo( arguments[0] ).fileName();
        // arguments
        for ( int i = offset; i < arguments.size(); ++i ) {
            args.append( arguments[i] );
        }
    }
}


bool OptionParser::parse( bool untilFirstSwitchOnly ) {
    //    qDebug( "parse(%s)", args.join( QString( "," ) ).ascii() );
    // push all arguments as we got them on a stack
    // more pushes might following when parsing condensed arguments
    // like --key=value.
    QStack<QString> stack;
    {
        QStringListIterator it(args);
        it.toBack();
        while( it.hasPrevious() ) {
            stack.push( it.previous() );
        }
    }

    const OptionConstIterator obegin = options.begin();
    const OptionConstIterator oend = options.end();
    enum { StartState, ExpectingState, OptionalState } state = StartState;
    Option currOpt;
    enum TokenType { LongOpt, ShortOpt, Arg, End } t, currType = End;
    bool extraLoop = true; // we'll do an extra round. fake an End argument
    while ( !stack.isEmpty() || extraLoop ) {
        QString a;
        QString origA;
        // identify argument type
        if ( !stack.isEmpty() ) {
            a = stack.pop();
            currArg++;
            origA = a;
            //	    qDebug( "popped %s", a.ascii() );
            if ( a.startsWith( QString::fromLatin1( "--" ) ) ) {
                // recognized long option
                a = a.mid( 2 );
                if ( a.isEmpty() ) {
                    qWarning( "'--' feature not supported, yet" );
                    exit( 2 );
                }
                t = LongOpt;
                // split key=value style arguments
                int equal = a.indexOf( '=' );
                if ( equal >= 0 ) {
                    stack.push( a.mid( equal + 1 ) );
                    currArg--;
                    a = a.left( equal );
                }
            } else if ( a.length() == 1 ) {
                t = Arg;
            } else if ( a[0] == '-' ) {
#if 0 // compat mode for -long style options
                if ( a.length() == 2 ) {
                    t = ShortOpt;
                    a = a[1];
                } else {
                    a = a.mid( 1 );
                    t = LongOpt;
                    // split key=value style arguments
                    int equal = a.find( '=' );
                    if ( equal >= 0 ) {
                        stack.push( a.mid( equal + 1 ) );
                        currArg--;
                        a = a.left( equal );
                    }
                }
#else
                // short option
                t = ShortOpt;
                // followed by an argument ? push it for later processing.
                if ( a.length() > 2 ) {
                    stack.push( a.mid( 2 ) );
                    currArg--;
                }
                a = a[1];
#endif
            } else {
                t = Arg;
            }
        } else {
            // faked closing argument
            t = End;
        }
        // look up among known list of options
        Option opt;
        if ( t != End ) {
            OptionConstIterator oit = obegin;
            while ( oit != oend ) {
                const Option &o = *oit;
                if ( ( t == LongOpt && a == o.lname ) || // ### check state
                    ( t == ShortOpt && a[0].unicode() == o.sname ) ) {
                    opt = o;
                    break;
                }
                ++oit;
            }
            if ( t == LongOpt && opt.type == OUnknown ) {
                if ( currOpt.type != OVarLen ) {
                    qWarning( "Unknown option --%s", a.toLatin1().data() );
                    return false;
                } else {
                    // VarLength options support arguments starting with '-'
                    t = Arg;
                }
            } else if ( t == ShortOpt && opt.type == OUnknown ) {
                if ( currOpt.type != OVarLen ) {
                    qWarning( "Unknown option -%c", a[0].unicode() );
                    return false;
                } else {
                    // VarLength options support arguments starting with '-'
                    t = Arg;
                }
            }
        } else {
            opt = Option( OEnd );
        }

        // interpret result
        switch ( state ) {
        case StartState:
            if ( opt.type == OSwitch ) {
                setSwitch( opt );
                setOptions.insert( opt.lname, 1 );
                setOptions.insert( QString( QChar( opt.sname ) ), 1 );
            } else if ( opt.type == OArg1 || opt.type == ORepeat ) {
                state = ExpectingState;
                currOpt = opt;
                currType = t;
                setOptions.insert( opt.lname, 1 );
                setOptions.insert( QString( QChar( opt.sname ) ), 1 );
            } else if ( opt.type == OOpt || opt.type == OVarLen ) {
                state = OptionalState;
                currOpt = opt;
                currType = t;
                setOptions.insert( opt.lname, 1 );
                setOptions.insert( QString( QChar( opt.sname ) ), 1 );
            } else if ( opt.type == OEnd ) {
                // we're done
            } else if ( opt.type == OUnknown && t == Arg ) {
                if ( numReqArgs > 0 ) {
                    if ( reqArg.stringValue->isNull() ) { // ###
                        *reqArg.stringValue = a;
                    } else {
                        qWarning( "Too many arguments" );
                        return false;
                    }
                } else if ( numOptArgs > 0 ) {
                    if ( optArg.stringValue->isNull() ) { // ###
                        *optArg.stringValue = a;
                    } else {
                        qWarning( "Too many arguments" );
                        return false;
                    }
                }
            } else {
                qFatal( "unhandled StartState case %d",  opt.type );
            }
            break; //--- fino a qui ad indentare
        case ExpectingState:
            if ( t == Arg ) {
                if ( currOpt.type == OArg1 ) {
                    *currOpt.stringValue = a;
                    state = StartState;
                } else if ( currOpt.type == ORepeat ) {
                    currOpt.listValue->append( a );
                    state = StartState;
                } else {
                    abort();
                }
            } else {
                QString n = currType == LongOpt ?
                    currOpt.lname : QString( QChar( currOpt.sname ) );
                qWarning( "Expected an argument after '%s' option", n.toLatin1().data() );
                return false;
            }
            break;
        case OptionalState:
            if ( t == Arg ) {
                if ( currOpt.type == OOpt ) {
                    *currOpt.stringValue = a;
                    state = StartState;
                } else if ( currOpt.type == OVarLen ) {
                    currOpt.listValue->append( origA );
                    // remain in this state
                } else {
                    abort();
                }
            } else {
                // optional argument not specified
                if ( currOpt.type == OOpt )
                    *currOpt.stringValue = currOpt.def;
                if ( t != End ) {
                    // re-evaluate current argument
                    stack.push( origA );
                    currArg--;
                }
                state = StartState;
            }
            break;
        }

        if ( untilFirstSwitchOnly && opt.type == OSwitch )
            return true;

        // are we in the extra loop ? if so, flag the final end
        if ( t == End )
            extraLoop = false;
    }

    if ( numReqArgs > 0 && reqArg.stringValue->isNull() ) {
        qWarning( "Lacking required argument" );
        return false;
    }

    return true;
}

void OptionParser::addOption( Option o ) {
    // ### check for conflicts
    options.append( o );
}

void OptionParser::addSwitch( const QString &lname, bool *b ) {
    Option opt( OSwitch, 0, lname );
    opt.boolValue = b;
    addOption( opt );
    // ### could do all inits at the beginning of parse()
    *b = false;
}

void OptionParser::setSwitch( const Option &o ) {
    assert( o.type == OSwitch );
    *o.boolValue = true;
}

void OptionParser::addOption( char s, const QString &l, QString *v ) {
    Option opt( OArg1, s, l );
    opt.stringValue = v;
    addOption( opt );
    *v = QString::null;
}

void OptionParser::addVarLengthOption( const QString &l, QStringList *v ) {
    Option opt( OVarLen, 0, l );
    opt.listValue = v;
    addOption( opt );
    *v = QStringList();
}

void OptionParser::addRepeatableOption( char s, QStringList *v ) {
    Option opt( ORepeat, s, QString::null );
    opt.listValue = v;
    addOption( opt );
    *v = QStringList();
}

void OptionParser::addRepeatableOption( const QString &l, QStringList *v ) {
    Option opt( ORepeat, 0, l );
    opt.listValue = v;
    addOption( opt );
    *v = QStringList();
}

void OptionParser::addOptionalOption( const QString &l, QString *v, const QString &def ) {
    addOptionalOption( 0, l, v, def );
}

void OptionParser::addOptionalOption( char s, const QString &l, QString *v, const QString &def ) {
    Option opt( OOpt, s, l );
    opt.stringValue = v;
    opt.def = def;
    addOption( opt );
    *v = QString::null;
}

void OptionParser::addArgument( const QString &name, QString *v ) {
    Option opt( OUnknown, 0, name );
    opt.stringValue = v;
    reqArg = opt;
    ++numReqArgs;
    *v = QString::null;
}

void OptionParser::addOptionalArgument( const QString &name, QString *v ) {
    Option opt( OUnknown, 0, name );
    opt.stringValue = v;
    optArg = opt;
    ++numOptArgs;
    *v = QString::null;
}


bool OptionParser::isSet( const QString &name ) const {
    return setOptions.find( name ) != setOptions.end();
}

} // end namespace farsa

