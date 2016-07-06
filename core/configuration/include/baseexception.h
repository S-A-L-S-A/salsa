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

#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H

#include <typeinfo>
#include <exception>
#include <cstring>
#include <cstdio>
#include <QtCore>

namespace farsa {

/**
 * \brief The base class for all exceptions in farsa
 *
 * We simply use QException as the base exception, this typedef is here in case
 * we need to change the base class in the future. We use this class as the base
 * class to be able to propagate exception between threads. This is automatic
 * for QtConcurrent functions, we have to write some more code for our classes.
 * When creating a new exception remember to implement the clone() and raise()
 * functions. You can use the macro EXCEPTION_HELPER_FUNCTIONS (remember to put
 * it in the public part of the class). QException inherits std::exception, so
 * also implement the what() function.
 */
typedef QException BaseException;

/**
 * \brief The macro that provides the default implementation of the clone() and
 *        raise() functions
 */
#define EXCEPTION_HELPER_FUNCTIONS(ClassName) virtual ClassName* clone() const\
                                              {\
                                                      return new ClassName(*this);\
                                              }\
                                              virtual void raise() const\
                                              {\
                                                      ClassName e = *this;\
                                                      throw e;\
                                              }

} // end namespace farsa

#endif
