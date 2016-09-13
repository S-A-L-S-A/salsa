/********************************************************************************
 *  SALSA Utilities Library                                                     *
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

#ifndef SALSAMISCUTILITIES_H
#define SALSAMISCUTILITIES_H

#include "utilitiesconfig.h"
#include <QString>
#include <QList>
#include <QPair>

/**
 * \file salsamiscutilities.h
 *
 * \brief a file with miscellaneous functions
 */

/**
 * \brief A macro to deprecate functions
 */
#if defined(__GNUC__) || defined(__clang__)
	#define SALSA_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
	#define SALSA_DEPRECATED __declspec(deprecated)
#endif

namespace salsa {
	/**
	 * \brief A structure keeping the position of C/C++ comments in a string
	 *
	 * This structure is used by the removeComments() function to return the
	 * position of comments both in the original and parsed program
	 */
	struct SALSA_UTIL_TEMPLATE CommentPosition
	{
		/**
		 * \brief The possible types of comments (C or C++ style)
		 */
		enum CommentType {
			CComment, /**< A C-style comment */
			CppComment /**< A C++-style comment */
		};

		/**
		 * \brief Constructor
		 */
		CommentPosition() :
			originalStart(0),
			originalEnd(0),
			parsedPosition(0),
			type(CComment)
		{
		}

		/**
		 * \brief Constructor
		 *
		 * \param s the starting position of the comment in the original
		 *          program
		 * \param e the ending position of the comment in the original
		 *          program
		 * \param p the position of the space with which the comment was
		 *          substituted in the parsed program
		 * \param t the type of comment
		 */
		CommentPosition(unsigned int s, unsigned int e, unsigned int p, CommentType t) :
			originalStart(s),
			originalEnd(e),
			parsedPosition(p),
			type(t)
		{
		}

		/**
		 * \brief The starting position of the comment in the original
		 *        program (i.e. the position of the first character)
		 */
		unsigned int originalStart;

		/**
		 * \brief The ending position of the comment in the original
		 *        program (i.e. the position of the last character)
		 */
		unsigned int originalEnd;

		/**
		 * \brief The position of the space with which the comment was
		 *        substituted in the parsed program
		 */
		unsigned int parsedPosition;

		/**
		 * \brief The type of comment
		 */
		CommentType type;
	};

	/**
	 * \brief A list of positions of comments
	 */
	typedef QList<CommentPosition> CommentsPositions;

	/**
	 * \brief A structure keeping the position of C/C++ strings or
	 *        characters in a program
	 *
	 * This structure is used by the removeComments() function to return the
	 * position of strings and characters in a program, both in the original
	 * program and in the parsed one
	 */
	struct SALSA_UTIL_TEMPLATE StringPosition
	{
		/**
		 * \brief String or character
		 */
		enum Type {
			String, /**< A string (enclosed in "") */
			Character /**< A character (enclosed in '') */
		};

		/**
		 * \brief Constructor
		 */
		StringPosition() :
			originalStart(0),
			originalEnd(0),
			parsedStart(0),
			parsedEnd(0),
			type(String)
		{
		}

		/**
		 * \brief Constructor
		 *
		 * \param os The starting position of the comment in the original
		 *           program (i.e. the position of the opening " or ')
		 * \param oe The ending position of the comment in the original
		 *           program (i.e. the position of the closing " or ')
		 * \param ps The starting position of the comment in the parsed
		 *           program (i.e. the position of the opening " or ')
		 * \param pe The ending position of the comment in the parsed
		 *           program (i.e. the position of the closing " or ')
		 * \param t whether this is a string or a character
		 */
		StringPosition(unsigned int os, unsigned int oe, unsigned int ps, unsigned int pe, Type t) :
			originalStart(os),
			originalEnd(oe),
			parsedStart(ps),
			parsedEnd(pe),
			type(t)
		{
		}

		/**
		 * \brief The starting position of the comment in the original
		 *        program (i.e. the position of the opening " or ')
		 */
		unsigned int originalStart;

		/**
		 * \brief The ending position of the comment in the original
		 *        program (i.e. the position of the closing " or ')
		 */
		unsigned int originalEnd;

		/**
		 * \brief The starting position of the comment in the parsed
		 *        program (i.e. the position of the opening " or ')
		 */
		unsigned int parsedStart;

		/**
		 * \brief The ending position of the comment in the parsed
		 *        program (i.e. the position of the closing " or ')
		 */
		unsigned int parsedEnd;

		/**
		 * \brief Whether this is a string or a character
		 */
		Type type;
	};

	/**
	 * \brief A list of positions of strings
	 */
	typedef QList<StringPosition> StringsPositions;

	/**
	 * \brief A simple function to find the position of all C and C++
	 *        comments and strings from a program
	 *
	 * \param program the program to parse
	 * \param parsedProgram if not nullptr, fills with the program with
	 *                      comments replaced with a space
	 * \return a pair made up of a list with comments positions and a list
	 *         with strings positions
	 */
	QPair<CommentsPositions, StringsPositions> findCommentsAndStrings(const QString& program, QString* parsedProgram = nullptr);

	/**
	 * \brief Returns true if the provided position is inside the list of comments
	 *        or strings
	 *
	 * \param pos the position to check
	 * \param blocks the list of comments or positions to check positions
	 * \return true if pos is inside any comment or string in the list
	 */
	template <class BlockType>
	bool SALSA_UTIL_TEMPLATE positionInsideBlock(unsigned int pos, const QList<BlockType>& blocks)
	{
		foreach (BlockType b, blocks) {
			if ((pos >= b.originalStart) && (pos <= b.originalEnd)) {
				return true;
			}
		}

		return false;
	}
}

#endif
