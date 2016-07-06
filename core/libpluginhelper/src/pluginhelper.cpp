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

#include "pluginhelper.h"
#include <QTextStream>
#include <QFileInfo>
#include <QFile>
#include <QRegularExpression>
#include <QtAlgorithms>
#include "utilitiesexceptions.h"

namespace farsa {

namespace pluginHelper {

HeaderParser::NamespacePosition::NamespacePosition()
	: name()
	, start(0)
	, end(0)
{
}

HeaderParser::NamespacePosition::NamespacePosition(const NamespacePosition& other)
	: name(other.name)
	, start(other.start)
	, end(other.end)
{
}

HeaderParser::NamespacePosition& HeaderParser::NamespacePosition::operator=(const NamespacePosition& other)
{
	if (this == &other) {
		return *this;
	}

	name = other.name;
	start = other.start;
	end = other.end;

	return *this;
}

bool HeaderParser::NamespacePosition::operator<(const NamespacePosition& other) const
{
	return start < other.start;
}

HeaderParser::FoundPattern::FoundPattern()
	: index(0)
	, position(0)
{
}

HeaderParser::FoundPattern::FoundPattern(unsigned int i, unsigned int p)
	: index(i)
	, position(p)
{
}

HeaderParser::FoundPattern::FoundPattern(const FoundPattern& other)
	: index(other.index)
	, position(other.position)
{
}

HeaderParser::FoundPattern& HeaderParser::FoundPattern::operator=(const FoundPattern& other)
{
	if (this == &other) {
		return *this;
	}

	index = other.index;
	position = other.position;

	return *this;
}

bool HeaderParser::FoundPattern::operator<(const FoundPattern& other) const
{
	return position < other.position;
}

HeaderParser::HeaderParser(const QString& header)
	: m_header(header)
	, m_commentsAndStringsPos(findCommentsAndStrings(m_header))
	, m_classes()
	, m_mangledHeader()
	, m_preRegistrationFunctions()
	, m_postRegistrationFunctions()
{
	parse();
}

bool HeaderParser::classesToRegisterPresent() const
{
	return !m_classes.isEmpty();
}

int HeaderParser::numClassesToRegister() const
{
	return m_classes.size();
}

QString HeaderParser::className(int i) const
{
	return m_classes[i].name;
}

QStringList HeaderParser::namespacesForClass(int i) const
{
	return m_classes[i].namespaces;
}

QString HeaderParser::fullyQualifiedClassName(int i) const
{
	const QString prefix = m_classes[i].namespaces.isEmpty() ? QString() : (m_classes[i].namespaces.join("::") + "::");

	return prefix + className(i);
}

QStringList HeaderParser::parentsForClass(int i) const
{
	return m_classes[i].parents;
}

QString HeaderParser::mangledHeader() const
{
	return m_mangledHeader;
}

QStringList HeaderParser::preRegistrationFunctions() const
{
	return m_preRegistrationFunctions;
}

QStringList HeaderParser::postRegistrationFunctions() const
{
	return m_postRegistrationFunctions;
}

void HeaderParser::parse()
{
	const QVector<int> registrationMacroStartPos = findRegistrationMacroPositions();

	QVector<ClassInfo> allClasses = findAllClassesCandidateForRegistration();

	QVector<NamespacePosition> namespaces = extractAllNamespaces();

	extractListOfClassesToRegister(registrationMacroStartPos, allClasses);

	extractClassesNamespaces(namespaces);

	extractClassesParents();

	generateMangledHeader();

	extractPreRegistrationFunctions();

	extractPostRegistrationFunctions();
}

QVector<int> HeaderParser::findRegistrationMacroPositions() const
{
	const QRegularExpression farsaRegisterClassRE("FARSA_REGISTER_CLASS");

	int curIndexOfMacro = -1;
	QVector<int> registrationMacroStartPos;
	do {
		curIndexOfMacro = m_header.indexOf(farsaRegisterClassRE, curIndexOfMacro + 1);

		if ((curIndexOfMacro != -1) &&
		    !positionInsideCommentOrString(curIndexOfMacro)) {
			registrationMacroStartPos.append(curIndexOfMacro);
		}
	} while (curIndexOfMacro != -1);

	return registrationMacroStartPos;
}

QVector<HeaderParser::ClassInfo> HeaderParser::findAllClassesCandidateForRegistration() const
{
	const QRegularExpression classNameRE("class\\s*FARSA_PLUGIN_(?:API|TEMPLATE|INTERNAL)\\s*(\\w+)\\s*");

	QRegularExpressionMatchIterator it = classNameRE.globalMatch(m_header);
	QVector<ClassInfo> classes;
	while (it.hasNext()) {
		QRegularExpressionMatch m = it.next();

		if (!positionInsideCommentOrString(m.capturedStart())) {
			ClassInfo ci;
			ci.name = m.captured(1);
			ci.classPos = m.capturedStart();
			classes.append(ci);
		}
	}

	return classes;
}

QVector<HeaderParser::NamespacePosition> HeaderParser::extractAllNamespaces() const
{
	// Here we have to find all { and } and put them in a stack. We use a list of NamespacePosition, setting
	// the name to the empty string if the bracket doesn't belong to a namespace. We then copy only namespaces
	// to the list we return
	QVector<NamespacePosition> brackets;
	QVector<NamespacePosition> namespaces;

	// Here we ignore parenthesis that don't match, the program shouldn't compile anyway
	const QRegularExpression namespaceRE("\\bnamespace\\s+(\\S+)\\s*[{]");
	int lastNamespaceSearchPos = 0;
	int curPos = -1;
	while (true) {
		const int nextOpen = m_header.indexOf('{', curPos + 1);
		const int nextClosed = m_header.indexOf('}', curPos + 1);

		if ((nextOpen == -1) && (nextClosed == -1)) {
			break;
		} else if (nextOpen == -1) {
			curPos = nextClosed;
		} else if (nextClosed == -1) {
			curPos = nextOpen;
		} else {
			curPos = std::min(nextOpen, nextClosed);
		}

		if (positionInsideCommentOrString(curPos)) {
			continue;
		}

		if (m_header[curPos] == '{') {
			NamespacePosition n;
			n.start = curPos;

			QRegularExpressionMatch m = namespaceRE.match(m_header, lastNamespaceSearchPos);
			if (m.hasMatch()) {
				n.name = m.captured(1);
			}
			lastNamespaceSearchPos = curPos + 1;

			brackets.append(n);
		} else if (brackets.size() != 0) {
			if (!brackets.last().name.isEmpty()) {
				brackets.last().end = curPos;
				namespaces.append(brackets.last());
			}
			brackets.removeLast();
		}
	}

	// Now sorting namespaces by starting position (this will guarantee they are also sorted from the
	// outermost to the innermost)
	qSort(namespaces);

	return namespaces;
}

void HeaderParser::extractListOfClassesToRegister(const QVector<int>& registrationMacroPos, const QVector<HeaderParser::ClassInfo>& candidates)
{
	m_classes = candidates;

	for (int i = 0; i < registrationMacroPos.size(); ++i) {
		const int curMacroPos = registrationMacroPos[i];
		while (true) {
			if (m_classes.size() <= i) {
				farsa::throwUserRuntimeError(QString("Error parsing the header, no class matching the registration macro with index %1").arg(i));
			} else if ((m_classes.length() != (i + 1)) && (m_classes[i + 1].classPos < curMacroPos)) {
				// The current class does not correspond to the current macro (i.e. we don't register
				// the class)
				m_classes.removeAt(i);
			} else if (m_classes[i].classPos >= curMacroPos) {
				farsa::throwUserRuntimeError(QString("Error parsing the header, no class matching the registration macro with index %1 (class after the macro)").arg(i));
			} else {
				// To check that we are associating the right class, we check that between the position of the
				// class keyword and the macro there is exactly one "{" and no ";" or "}". This is only an heuristic,
				// but should work well
				if (!checkNoSemicolonNoClosedCurlyBracketAndOnlyOneOpenCurlyBracket(m_classes[i].classPos, curMacroPos)) {
					farsa::throwUserRuntimeError(QString("Error parsing the header, no class matching the registration macro with index %1 (invalid characters between class and macro)").arg(i));
				}

				m_classes[i].macroPos = curMacroPos;
				break;
			}
		}
	}

	// Removing all classes following the last registration macro
	if (m_classes.size() > registrationMacroPos.size()) {
		m_classes.resize(registrationMacroPos.size());
	}
}

void HeaderParser::extractClassesNamespaces(const QVector<NamespacePosition>& namespaces)
{
	for (int i = 0; i < m_classes.size(); ++i) {
		ClassInfo& curClass = m_classes[i];

		foreach (NamespacePosition n, namespaces) {
			if ((curClass.classPos >= n.start) && (curClass.classPos <= n.end)) {
				curClass.namespaces.append(n.name);
			}
		}
	}
}

void HeaderParser::extractClassesParents()
{
	for (int i = 0; i < m_classes.size(); ++i) {
		ClassInfo& curClass = m_classes[i];

		curClass.parents = extractClassParents(curClass);
	}
}

void HeaderParser::generateMangledHeader()
{
	const QStringList patternsToSubstitute = QStringList() << "FARSA_PLUGIN_API" << "FARSA_PLUGIN_TEMPLATE" << "FARSA_PLUGIN_INTERNAL";
	const QStringList replacements = QStringList() <<"FARSA_PLUGIN_API_IMPORT" << "FARSA_PLUGIN_TEMPLATE_IMPORT" << "FARSA_PLUGIN_INTERNAL_IMPORT";

	QVector<FoundPattern> foundPatterns = findAllPatterns(patternsToSubstitute);

	qSort(foundPatterns);

	substituteAllPatterns(foundPatterns, patternsToSubstitute, replacements);
}

void HeaderParser::extractPreRegistrationFunctions()
{
	m_preRegistrationFunctions = extractRegistrationFunctions("FARSA_PRE_REGISTRATION_FUNCTION");
}

void HeaderParser::extractPostRegistrationFunctions()
{
	m_postRegistrationFunctions = extractRegistrationFunctions("FARSA_POST_REGISTRATION_FUNCTION");
}

QStringList HeaderParser::extractClassParents(const ClassInfo& info) const
{
	// Inherited classes are in the text between the class keyword and the registration macro
	const QString parentsText = m_header.mid(info.classPos, info.macroPos - info.classPos);

	const QRegularExpression parentsRE("\\bpublic\\s+(\\w+)");
	QRegularExpressionMatchIterator it = parentsRE.globalMatch(parentsText);
	QStringList parents;
	while (it.hasNext()) {
		const QRegularExpressionMatch m = it.next();

		const QString parent = m.captured(1);
		if (parent != "FARSA_NR") {
			parents.append(parent);
		}
	}

	return parents;
}

QVector<HeaderParser::FoundPattern> HeaderParser::findAllPatterns(const QStringList& patternsToSubstitute) const
{
	QVector<FoundPattern> foundPatterns;
	for (int i = 0; i < patternsToSubstitute.size(); i++) {
		int curPos = 0;
		while ((curPos = m_header.indexOf(patternsToSubstitute[i], curPos)) != -1) {
			if (!positionInsideCommentOrString(curPos)) {
				foundPatterns.append(FoundPattern(i, curPos));
			}
			curPos += patternsToSubstitute[i].size();
		}
	}

	return foundPatterns;
}

void HeaderParser::substituteAllPatterns(const QVector<FoundPattern>& foundPatterns, const QStringList& patternsToSubstitute, const QStringList& replacements)
{
	m_mangledHeader = QString();

	int curPos = 0;
	foreach (FoundPattern p, foundPatterns) {
		m_mangledHeader += m_header.mid(curPos, p.position - curPos);
		m_mangledHeader += replacements[p.index];
		curPos = p.position + patternsToSubstitute[p.index].size();
	}

	// Copying the remaining part of the string
	m_mangledHeader += m_header.mid(curPos);
}

QStringList HeaderParser::extractRegistrationFunctions(const QString& macroName) const
{
	QStringList functions;

	const QRegularExpression registrationFunctionRE(QRegularExpression::escape(macroName) + "[(](.*?)[)]");
	QRegularExpressionMatchIterator it = registrationFunctionRE.globalMatch(m_header);
	while (it.hasNext()) {
		const QRegularExpressionMatch m = it.next();

		if (positionInsideCommentOrString(m.capturedStart())) {
			continue;
		}

		// Adding the function to the list of pre registration functions
		functions.append(m.captured(1));
	}

	return functions;
}

bool HeaderParser::checkNoSemicolonNoClosedCurlyBracketAndOnlyOneOpenCurlyBracket(int start, int end) const
{
	const QString txt = m_header.mid(start, end - start);

	if (!checkCharacterNotInString(';', txt, start)) {
		return false;
	}

	if (!checkCharacterNotInString('}', txt, start)) {
		return false;
	}

	if (!checkOnlyOneCharacterInString('{', txt, start)) {
		return false;
	}

	return true;
}

bool HeaderParser::checkCharacterNotInString(char ch, const QString& txt, int offset) const
{
	int pos = -1;
	while (true) {
		pos = txt.indexOf(ch, pos + 1);
		if (pos == -1) {
			break;
		}

		if (!positionInsideCommentOrString(offset + pos)) {
			return false;
		}
	}

	return true;
}

bool HeaderParser::checkOnlyOneCharacterInString(char ch, const QString& txt, int offset) const
{
	int pos = -1;
	bool oneFound = false;
	while(true) {
		pos = txt.indexOf(ch, pos + 1);
		if (pos == -1) {
			break;
		}

		if (!positionInsideCommentOrString(offset + pos)) {
			if (oneFound) {
				return false;
			}

			oneFound = true;
		}
	}

	return oneFound;
}

bool HeaderParser::positionInsideCommentOrString(int pos) const
{
	return positionInsideBlock(pos, m_commentsAndStringsPos.first) || positionInsideBlock(pos, m_commentsAndStringsPos.second);
}

const QString PluginRegistrationCodeGenerator::registrationHeaderTemplate = "\
#ifndef %1\n\
#define %1\n\
\n\
#include <QObject>\n\
#include \"farsaplugin.h\"\n\
\n\
class %2 : public QObject, public FarsaPlugin\n\
{\n\
	Q_OBJECT\n\
	FARSA_PLUGIN_METADATA(IID FarsaPlugin_IID)\n\
	Q_INTERFACES(FarsaPlugin)\n\
\n\
public:\n\
	virtual void registerTypes();\n\
	virtual QStringList getDependencies();\n\
};\n\
\n\
#endif\n\
\n\
";

const QString PluginRegistrationCodeGenerator::registrationSourceTemplate = "\
#include <QtPlugin>\n\
#include \"%1\"\n\
#include \"typesdb.h\"\n\
%2\
\n\
void %3::registerTypes()\n\
{\n\
%4\
}\n\
\n\
QStringList %3::getDependencies()\n\
{\n\
	return QStringList()%5;\n\
}\n\
\n\
";

const QString PluginRegistrationCodeGenerator::registrationSourceIncludedHeadersTemplate = "#include \"%1\"\n";

const QString PluginRegistrationCodeGenerator::registrationCommandTemplate = "\tfarsa::TypesDB::instance().registerType<%1>(\"%2\", QStringList()%3);\n";

PluginRegistrationCodeGenerator::PluginRegistrationCodeGenerator(const QString& generatedFilename, const QString& pluginName, const QList<FilenameAndParsedHeader>& headers, const QStringList& dependencies)
	: m_generatedFilename(generatedFilename)
	, m_pluginName(pluginName)
	, m_headers(headers)
	, m_dependencies(dependencies)
	, m_headerGuard()
	, m_generatedClassName()
	, m_headerFilename()
	, m_dependenciesString()
	, m_headerIncludeString()
	, m_registrationString()
	, m_dependencySorter()
	, m_header()
	, m_source()
{
	generateCode();
}

const QString& PluginRegistrationCodeGenerator::registrationHeader() const
{
	return m_header;
}

const QString& PluginRegistrationCodeGenerator::registrationSource() const
{
	return m_source;
}

void PluginRegistrationCodeGenerator::generateCode()
{
	if (checkNothingToRegister()) {
		return;
	}

	extractNeededData();
	generateHeader();
	generateSource();
}

bool PluginRegistrationCodeGenerator::checkNothingToRegister() const
{
	if (!m_dependencies.isEmpty()){
		return false;
	}

	foreach (const FilenameAndParsedHeader& h, m_headers) {
		if (h.parsedHeader.classesToRegisterPresent() ||
		    !h.parsedHeader.preRegistrationFunctions().isEmpty() ||
		    !h.parsedHeader.postRegistrationFunctions().isEmpty()) {
			return false;
		}
	}

	return true;
}

void PluginRegistrationCodeGenerator::extractNeededData()
{
	m_headerGuard = m_generatedFilename.toUpper() + "_H";
	m_generatedClassName = m_pluginName + "Plugin";
	m_headerFilename = m_generatedFilename + ".h";

	if (m_dependencies.size() != 0) {
		m_dependenciesString = QString(" << \"") + m_dependencies.join("\" << \"") + QString("\"");
	}

	foreach (const FilenameAndParsedHeader& h, m_headers) {
		m_headerIncludeString += registrationSourceIncludedHeadersTemplate.arg(h.filename);
	}

	generateRegistrationString();
}

void PluginRegistrationCodeGenerator::generateRegistrationString()
{
	// Adding pre-registration functions
	foreach (const FilenameAndParsedHeader& h, m_headers) {
		m_registrationString += generateFunctionCalls(h.parsedHeader.preRegistrationFunctions());
	}

	// Adding class registrations
	fillDependencySorter();
	m_registrationString += generateClassesRegistrations();

	// Adding post-registration functions
	foreach (const FilenameAndParsedHeader& h, m_headers) {
		m_registrationString += generateFunctionCalls(h.parsedHeader.postRegistrationFunctions());
	}
}

QString PluginRegistrationCodeGenerator::generateFunctionCalls(const QStringList& funcs) const
{
	QString calls;

	foreach (const QString& f, funcs) {
		calls += "\t" + f + "();\n";
	}

	return calls;
}

void PluginRegistrationCodeGenerator::fillDependencySorter()
{
	foreach (const FilenameAndParsedHeader& h, m_headers) {
		const HeaderParser& p = h.parsedHeader;
		for (int i = 0; i < p.numClassesToRegister(); ++i) {
			m_dependencySorter.add(p.className(i), p.parentsForClass(i));
		}
	}
}

QString PluginRegistrationCodeGenerator::generateClassesRegistrations() const
{
	// Sorting dependencies
	DependencySorter<QString>::ElementAndDepenciesList classesToRegister = m_dependencySorter.sortWithDependencies();

	QString regs;

	foreach (const DependencySorter<QString>::ElementAndDepencies& e, classesToRegister) {
		const QString parents = e.dependencies.isEmpty() ? QString() : QString(" << \"") + QStringList(e.dependencies).join("\" << \"") + QString("\"");
		regs += registrationCommandTemplate.arg(e.element).arg(e.element).arg(parents);
	}

	return regs;
}

void PluginRegistrationCodeGenerator::generateHeader()
{
	m_header = registrationHeaderTemplate
		.arg(m_headerGuard)
		.arg(m_generatedClassName);
}

void PluginRegistrationCodeGenerator::generateSource()
{
	m_source = registrationSourceTemplate
		.arg(m_headerFilename)
		.arg(m_headerIncludeString)
		.arg(m_generatedClassName)
		.arg(m_registrationString)
		.arg(m_dependenciesString);
}

void doParse(const QString& pluginName, const QString& destDir, const QString& headersDestDir, const QString& genSource, const QString& dependenciesFile, const QStringList& headers)
{
	// The names of the files to generate
	const QString genSourceHeader = genSource + ".h";
	const QString genSourceImpl = genSource + ".cpp";

	// First of all extracting filenames from headers (i.e. we remove the directory). Here we also check that
	// no header has the same name of genSource (otherwise we would overwrite it)
	QStringList headersFilenames;
	foreach(QString h, headers) {
		const QString headerFilename = QFileInfo(h).fileName();

		if (headerFilename == genSourceHeader) {
			farsa::throwUserRuntimeError("One of the header to parse has the same filename of the header to generate: \"" + h + "\"");
		}

		headersFilenames.append(headerFilename);
	}

	// Now we read one header at a time and parse it
	QList<PluginRegistrationCodeGenerator::FilenameAndParsedHeader> l;
	for (int i = 0; i < headers.size(); i++) {
		// Reading the whole content of the header into a QString
		QFile origHeader(headers[i]);
		if (!origHeader.open(QIODevice::ReadOnly | QIODevice::Text)) {
			farsa::throwUserRuntimeError("Error when trying to open original header \"" + headers[i] + "\"");
		}

		QString headerString = origHeader.readAll();

		if (origHeader.error() != QFile::NoError) {
			farsa::throwUserRuntimeError("Error when trying to read original header \"" + headers[i] + "\"");
		}

		const HeaderParser& parser(headerString);
		l.append(PluginRegistrationCodeGenerator::FilenameAndParsedHeader(headersFilenames[i], parser));

		// Finally writing the parsed header to the destination directory
		const QString parsedHeaderFilename = headersDestDir + "/" + headersFilenames[i];
		QFile parsedHeader(parsedHeaderFilename);
		if (!parsedHeader.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
			farsa::throwUserRuntimeError("Error when trying to open parsed header \"" + parsedHeaderFilename + "\" for writing");
		}

		const qint64 writeErr = parsedHeader.write(parser.mangledHeader().toLatin1().data());

		if ((writeErr == -1) || (parsedHeader.error() != QFile::NoError)) {
			farsa::throwUserRuntimeError("Error when trying to write parsed header \"" + parsedHeaderFilename + "\"");
		}
	}

	// Reading dependencies
	QFile depFile(dependenciesFile);
	if (!depFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		farsa::throwUserRuntimeError("Error when trying to open the file with dependencies \"" + dependenciesFile + "\"");
	}

	QStringList dependencies = QString(depFile.readAll()).split(";", QString::SkipEmptyParts);

	if (depFile.error() != QFile::NoError) {
		farsa::throwUserRuntimeError("Error when trying to read the file with dependencies \"" + dependenciesFile + "\"");
	}

	// Generating registration code
	PluginRegistrationCodeGenerator generator(genSource, pluginName, l, dependencies);

	// Finally, we can write the files

	// First writing the header
	const QString genSourceHeaderPath = destDir + "/" + genSourceHeader;
	QFile registrationHeader(genSourceHeaderPath);
	if (!registrationHeader.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		farsa::throwUserRuntimeError("Error when trying to open generated header \"" + genSourceHeaderPath + "\"");
	}

	const qint64 writeErrHeader = registrationHeader.write(generator.registrationHeader().toLatin1().data());

	if ((writeErrHeader == -1) || (registrationHeader.error() != QFile::NoError)) {
		farsa::throwUserRuntimeError("Error when trying to write generated header \"" + genSourceHeaderPath + "\"");
	}

	// Finally writing the source
	const QString genSourceImplPath = destDir + "/" + genSourceImpl;
	QFile registrationSource(genSourceImplPath);
	if (!registrationSource.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		farsa::throwUserRuntimeError("Error when trying to open generated source \"" + genSourceImplPath + "\"");
	}

	const qint64 writeErrSource = registrationSource.write(generator.registrationSource().toLatin1().data());

	if ((writeErrSource == -1) || (registrationSource.error() != QFile::NoError)) {
		farsa::throwUserRuntimeError("Error when trying to write generated source \"" + genSourceImplPath + "\"");
	}
}

}

}
