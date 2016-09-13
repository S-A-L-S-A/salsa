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

#include "salsamiscutilities.h"

namespace salsa {

QPair<CommentsPositions, StringsPositions> findCommentsAndStrings(const QString& program, QString* parsedProgram)
{
	CommentsPositions cpos;
	StringsPositions spos;

	// To remove comments we read one character at a time and check if it is a comment or not
	QString cleaned;
	// The possible states
	enum RemoveCommentsState {
		FirstSlash,
		StartEndCComment,
		InCComment,
		InCppComment,
		InString,
		InCharacter,
		NormalCode
	} state = NormalCode;
	CommentPosition curComment;
	StringPosition curString;
	for (int i = 0; i < program.size(); i++) {
		switch (state) {
			case FirstSlash:
				if (program[i] == '/') {
					state = InCppComment;
					curComment.type = CommentPosition::CppComment;
				} else if (program[i] == '*') {
					state = InCComment;
					curComment.type = CommentPosition::CComment;
				} else {
					cleaned += '/';
					cleaned += program[i];
					state = NormalCode;

					// Checking for strings. We don't change or remove them but we have to report them
					if ((program[i] == '\"') && ((i == 0) || (program[i - 1] != '\\'))) {
						state = InString;
						curString.originalStart = i;
						curString.parsedStart = cleaned.size() - 1;
						curString.type = StringPosition::String;
					} else if ((program[i] == '\'') && ((i == 0) || (program[i - 1] != '\\'))) {
						state = InCharacter;
						curString.originalStart = i;
						curString.parsedStart = cleaned.size() - 1;
						curString.type = StringPosition::Character;
					}
				}
				break;
			case StartEndCComment:
				state = InCComment;
				if (program[i] == '/') {
					state = NormalCode;
					// Adding a whitespace for the cases of comments between two elements which should be separated
					cleaned += ' ';
					curComment.originalEnd = i;
					curComment.parsedPosition = cleaned.size() - 1;
					cpos.append(curComment);
				} else if (program[i] == '*') {
					state = StartEndCComment;
				}
				break;
			case InCComment:
				if (program[i] == '*') {
					state = StartEndCComment;
				}
				break;
			case InCppComment:
				if ((program[i] == '\n') || (program[i] == '\r')) {
					cleaned += program[i];
					state = NormalCode;
					curComment.originalEnd = i - 1;
					curComment.parsedPosition = cleaned.size() - 1;
					cpos.append(curComment);
				}
				break;
			case InString:
				cleaned += program[i];

				if ((program[i] == '"') && ((i == 0) || (program[i - 1] != '\\'))) {
					state = NormalCode;
					curString.originalEnd = i;
					curString.parsedEnd = cleaned.size() - 1;
					spos.append(curString);
				}
				break;
			case InCharacter:
				cleaned += program[i];

				// Here we don't check that there is only one character enclosed in '',
				// the code won't compile anyway if there is more than one character
				if ((program[i] == '\'') && ((i == 0) || (program[i - 1] != '\\'))) {
					state = NormalCode;
					curString.originalEnd = i;
					curString.parsedEnd = cleaned.size() - 1;
					spos.append(curString);
				}
				break;
			case NormalCode:
				if (program[i] == '/') {
					state = FirstSlash;
					curComment.originalStart = i;
				} else {
					cleaned += program[i];

					// Checking for strings. We don't change or remove them but we have to report them
					if ((program[i] == '\"') && ((i == 0) || (program[i - 1] != '\\'))) {
						state = InString;
						curString.originalStart = i;
						curString.parsedStart = cleaned.size() - 1;
						curString.type = StringPosition::String;
					} else if ((program[i] == '\'') && ((i == 0) || (program[i - 1] != '\\'))) {
						state = InCharacter;
						curString.originalStart = i;
						curString.parsedStart = cleaned.size() - 1;
						curString.type = StringPosition::Character;
					}
				}
				break;
		}
	}

	// Whe have to check if we are still in a comment or string/character to add it
	switch (state) {
		case StartEndCComment:
		case InCComment:
		case InCppComment:
			cleaned += ' ';
			curComment.originalEnd = program.size() - 1;
			curComment.parsedPosition = cleaned.size() - 1;
			cpos.append(curComment);
			break;
		case InString:
		case InCharacter:
			curString.originalEnd = program.size() - 1;
			curString.parsedEnd = cleaned.size() - 1;
			spos.append(curString);
			break;
		default:
			break;
	}

	if (parsedProgram != nullptr) {
		*parsedProgram = cleaned;
	}

	return QPair<CommentsPositions, StringsPositions>(cpos, spos);
}

} // end namespace salsa
