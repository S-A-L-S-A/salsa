/***************************************************************************
 *  SALSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                    *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ***************************************************************************/

#include "testutils.h"

FunctionCallsSequenceRecorder* FunctionCallsSequenceRecorder::m_curFunctionCallsSequenceRecorder = nullptr;

bool FunctionCallsSequenceRecorder::SingleCall::operator==(const SingleCall& other) const
{
	return (name == other.name) && (subcalls == other.subcalls) && (callEnded == other.callEnded);
}

bool FunctionCallsSequenceRecorder::SingleCall::operator!=(const SingleCall& other) const
{
	return !(*this == other);
}

FunctionCallsSequenceRecorder::FunctionCallsSequenceRecorder()
	: m_root()
	, m_curCall(&m_root)
{
	m_curFunctionCallsSequenceRecorder = this;

	m_root.parent = nullptr;
	m_root.callEnded = true;
}

FunctionCallsSequenceRecorder::~FunctionCallsSequenceRecorder()
{
	if (m_curFunctionCallsSequenceRecorder == this) {
		m_curFunctionCallsSequenceRecorder = nullptr;
	}
}

bool FunctionCallsSequenceRecorder::operator==(const FunctionCallsSequenceRecorder& other) const
{
	return m_root == other.m_root;
}

bool FunctionCallsSequenceRecorder::operator!=(const FunctionCallsSequenceRecorder& other) const
{
	return m_root != other.m_root;
}

void FunctionCallsSequenceRecorder::callBegin(QString funcName)
{
	SingleCall call;
	call.name = funcName;
	call.parent = m_curCall;
	call.callEnded = false;

	m_curCall->subcalls.append(call);
	m_curCall = &(m_curCall->subcalls.last());
}

void FunctionCallsSequenceRecorder::callEnd()
{
	m_curCall->callEnded = true;

	m_curCall = m_curCall->parent;
}

QMap<QString, int> FunctionCallsSequenceRecorder::functionsMap() const
{
	QMap<QString, int> map;

	recursivelyFillFunctionMap(map, m_root);

	return map;
}

QString FunctionCallsSequenceRecorder::toString() const
{
	return singleCallToString(m_root);
}

QString FunctionCallsSequenceRecorder::singleCallToString(const SingleCall& call, int tabLevel) const
{
	QString str;

	for (int i = 0; i < tabLevel; ++i) {
		str += '\t';
	}
	str += call.name;
	if (!call.callEnded) {
		str += " (not ended)";
	}
	str += '\n';

	for (int i = 0; i < call.subcalls.size(); ++i) {
		str += singleCallToString(call.subcalls[i], tabLevel + 1);
	}

	return str;
}

void FunctionCallsSequenceRecorder::recursivelyFillFunctionMap(QMap<QString, int>& map, const SingleCall& call) const
{
	if (!call.name.isEmpty()) {
		if (map.contains(call.name)) {
			map[call.name] += 1;
		} else {
			map[call.name] = 1;
		}
	}

	for (int i = 0; i < call.subcalls.size(); ++i) {
		recursivelyFillFunctionMap(map, call.subcalls[i]);
	}
}

FunctionCall::FunctionCall(QString funcName)
{
	if (FunctionCallsSequenceRecorder::m_curFunctionCallsSequenceRecorder != nullptr) {
		FunctionCallsSequenceRecorder::m_curFunctionCallsSequenceRecorder->callBegin(funcName);
	}
}

FunctionCall::~FunctionCall()
{
	if (FunctionCallsSequenceRecorder::m_curFunctionCallsSequenceRecorder != nullptr) {
		FunctionCallsSequenceRecorder::m_curFunctionCallsSequenceRecorder->callEnd();
	}
}

salsa::ConfigurationManager fillTemporaryConfigurationFileAndLoadParameters(const char* content)
{
	// This is thrown in case of problems because we cannot use
	// QVERIFY and similar macros (they cause compiler errors)
	class InitializationProblemException : public std::exception
	{
	public:
		virtual const char *what() const throw()
		{
			return "Failure in creating an loading test configuration files";
		}
	};

	QTemporaryFile tmpFile("XXXXXX.ini");

	if (!tmpFile.open()) {
		throw InitializationProblemException();
	}
	if (tmpFile.write(content) == -1) {
		throw InitializationProblemException();
	}
	if (!tmpFile.flush()) {
		throw InitializationProblemException();
	}

	salsa::ConfigurationManager manager;
	if (!manager.loadParameters(tmpFile.fileName())) {
		throw InitializationProblemException();
	}

	return manager;
}
