/********************************************************************************
 *  SALSA                                                                       *
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

#ifndef PLUGINHELPER_H
#define PLUGINHELPER_H

#include <QString>
#include <QList>
#include <QStringList>
#include "salsamiscutilities.h"
#include "dependencysorter.h"

namespace salsa {

namespace pluginHelper {

/**
 * \brief Parses a C++ header to extract classes (components and interfaces) to
 *        register
 *
 * This extracts all the other salsa-related stuffs from the header:
 * 	- classes to register (both components and interfaces);
 * 	- their parents and namespaces;
 * 	- pre and post registration functions.
 */
class HeaderParser
{
private:
	struct ClassInfo
	{
		QString name;
		// The position of the "class" keyword in the original header
		int classPos;
		// The position of the SALSA_REGISTER_CLASS macro in the
		// original header
		int macroPos;
		// The namespace of the class (from the outermost to the
		// innermost)
		QStringList namespaces;
		// The list of parents
		QStringList parents;
	};

	struct NamespacePosition
	{
		NamespacePosition();
		NamespacePosition(const NamespacePosition& other);
		NamespacePosition& operator=(const NamespacePosition& other);
		// Compares the namespaces starting positions
		bool operator<(const NamespacePosition& other) const;

		QString name;
		// The position of the opening curly bracket
		int start;
		// The position of the closing curly bracket
		int end;
	};

	struct FoundPattern
	{
		FoundPattern();
		FoundPattern(unsigned int i, unsigned int p);
		FoundPattern(const FoundPattern& other);
		FoundPattern& operator=(const FoundPattern& other);
		// Compares the patterns positions
		bool operator<(const FoundPattern& other) const;

		unsigned int index;
		unsigned int position;
	};

public:
	/**
	 * \brief Constructor
	 *
	 * \param header the header to parse
	 */
	HeaderParser(const QString& header);

	/**
	 * \brief Returns true if there are classes to register
	 *
	 * \return true if there are classes to register
	 */
	bool classesToRegisterPresent() const;

	/**
	 * \brief Returns the number of classes to register
	 *
	 * \return the number of classes to register
	 */
	int numClassesToRegister() const;

	/**
	 * \brief Returns the name of the i-th class to register
	 *
	 * \param i the index of the class to return (between 0 and
	 *          numClassesToRegister - 1)
	 * \return the name of the i-th class to register
	 */
	QString className(int i) const;

	/**
	 * \brief Returns the namespace in which the class is defined
	 *
	 * The list is sorted from the outermost namespace to the innermost
	 * \param i the index of the class whose namespace to return (between 0
	 *          and numClassesToRegister - 1)
	 * \return the namespace in which the class is defined
	 */
	QStringList namespacesForClass(int i) const;

	/**
	 * \brief Returns the name of a class along with all the namespaces
	 *
	 * \param i the index of the class whose fully qualified name to return
	 *          (between 0 and numClassesToRegister - 1)
	 * \return the name of a class along with all the namespaces
	 */
	QString fullyQualifiedClassName(int i) const;

	/**
	 * \brief Returns the list of parents of a class
	 *
	 * \param i the index of the class whose parents to return (between 0
	 *          and numClassesToRegister - 1)
	 * \return the parents of the class
	 */
	QStringList parentsForClass(int i) const;

	/**
	 * \brief Returns the mangled header
	 *
	 * This is the original header with export macros substituted by import
	 * ones
	 * \return the mangled header
	 */
	QString mangledHeader() const;

	/**
	 * \brief Returns the list of pre-registration functions
	 *
	 * \return the list of pre-registration functions
	 */
	QStringList preRegistrationFunctions() const;

	/**
	 * \brief Returns the list of post-registration functions
	 *
	 * \return the list of post-registration functions
	 */
	QStringList postRegistrationFunctions() const;

private:
	void parse();
	QVector<int> findRegistrationMacroPositions() const;
	// This finds all classes that use one of the the SALSA_PLUGIN_* macros, regardless of whether
	// they have to be registered or not
	QVector<ClassInfo> findAllClassesCandidateForRegistration() const;
	// Namespaces are returned sorted by starting position (this guarantees that outer namespaces come
	// before inner namespaces)
	QVector<NamespacePosition> extractAllNamespaces() const;
	// This uses the position of the registration macro to extract only the classes to register
	// from the list. This modifies m_classes
	void extractListOfClassesToRegister(const QVector<int>& registrationMacroPos, const QVector<ClassInfo>& candidates);
	// Fills the namespace field of the list of classes. This modifies m_classes
	void extractClassesNamespaces(const QVector<NamespacePosition>& namespaces);
	// Fills the parents field of the list of classes. This modifies m_classes
	void extractClassesParents();
	// Generates the mangled header
	void generateMangledHeader();
	// Extracts the pre-registration functions
	void extractPreRegistrationFunctions();
	// Extracts the post-registration functions
	void extractPostRegistrationFunctions();
	// Extracts the parents of the given class. Only publicly inherited classes not surrounded by the
	// SALSA_NR() macro are returned. Inheriting from template classes is not supported (use typedefs)
	QStringList extractClassParents(const ClassInfo& info) const;
	// Finds all patterns in the header. This ignores patterns inside comments or strings
	QVector<FoundPattern> findAllPatterns(const QStringList& patternsToSubstitute) const;
	// Replaces all found patterns and generates m_mangledHeader
	void substituteAllPatterns(const QVector<FoundPattern>& foundPatterns, const QStringList& patternsToSubstitute, const QStringList& replacements);
	// Extracts the pre/post registration functions (depending on the macroName)
	QStringList extractRegistrationFunctions(const QString& macroName) const;
	// Checks that txt contains no semicolon, no closed curly bracket and only one open bracket. This
	// also takes comments and strings into account. The indexes refer to the original header string. The
	// start index is included, the end index is not
	bool checkNoSemicolonNoClosedCurlyBracketAndOnlyOneOpenCurlyBracket(int start, int end) const;
	// Checks that character ch is not in txt or is in a comment or string block. offset is the offset of
	// txt with respect to the header (of which txt should be a section)
	bool checkCharacterNotInString(char ch, const QString& txt, int offset) const;
	// Checks that ch appears only once in txt (excluding comments and strings). offset is the offset of
	// txt with respect to the header (of which txt should be a section)
	bool checkOnlyOneCharacterInString(char ch, const QString& txt, int offset) const;
	bool positionInsideCommentOrString(int pos) const;

	QString m_header;
	QPair<CommentsPositions, StringsPositions> m_commentsAndStringsPos;
	QVector<ClassInfo> m_classes;
	QString m_mangledHeader;
	QStringList m_preRegistrationFunctions;
	QStringList m_postRegistrationFunctions;
};

/**
 * \brief Gathers data from a series of headers and generates registration code
 *
 * This class takes a list of parsed headers plus other information and
 * generates the needed registration code
 */
class PluginRegistrationCodeGenerator
{
public:
	/**
	 * \brief A structure keeping the name of a file containing a header to
	 *        parse and the parsed header
	 */
	struct FilenameAndParsedHeader
	{
		/**
		 * \brief Constructor
		 *
		 * \param f the name of the file of the header to parse
		 * \param p the object with parsed header
		 */
		FilenameAndParsedHeader(const QString& f, const HeaderParser& p)
			: filename(f)
			, parsedHeader(p)
		{
		}

		/**
		 * \brief The name of the file of the header to parse
		 */
		QString filename;

		/**
		 * \brief The object with parsed header
		 */
		HeaderParser parsedHeader;
	};

private:
	// The content of a generated header file for registration. The %N
	// patterns will be filled by the QString::arg function.
	// 	%1 is the macro name for the #include guard;
	// 	%2 is the name of the subclass of SalsaPlugin.
	static const QString registrationHeaderTemplate;

	// The content of a generated source file for registration. The %N
	// patterns will be filled by the QString::arg function
	// 	%1 is the name of the generated header file;
	// 	%2 is the list of #include directives of header files with the
	// 	   declaration of classes to register (generated using the
	// 	   registrationSourceIncludedHeadersTemplate);
	// 	%3 is the name of the subclass of SalsaPlugin;
	// 	%4 is the list of instructions to register classes (generated
	// 	   using the registrationCommandTemplate) and calls to pre/post
	// 	   registration functions;
	// 	%5 is the list of dependencies to return. Each element of the
	// 	   list should be in the form ' << "dep"';
	static const QString registrationSourceTemplate;

	// The template for the command to include headers files into the source
	// file for registration. The %N patterns will be filled by the
	// QString::arg function
	// 	%1 is the name of the header file to include
	static const QString registrationSourceIncludedHeadersTemplate;

	// The template for the command to register classes into the salsa
	// factory. The %N patterns will be filled by the QString::arg function
	// 	%1 is the name of the class to register (template argument);
	// 	%2 is the name of the class to register (registration name);
	// 	%3 is the list of parents of the class to register.
	static const QString registrationCommandTemplate;

public:
	/**
	 * \brief Constructor
	 *
	 * \param generatedFilename the name (without extension) of the files
	 *                          with registration code that will be
	 *                          generated. We need this to create both the
	 *                          header guard and the include directive in
	 *                          the source file. Here we make the assumption
	 *                          that the header will have a .h extension
	 * \param pluginName the name of the plugin that is being compiled
	 * \param headers the list of headers (filename and the object with
	 *                header content)
	 * \param dependencies the list of plugins on which this plugin depends
	 */
	PluginRegistrationCodeGenerator(const QString& generatedFilename, const QString& pluginName, const QList<FilenameAndParsedHeader>& headers, const QStringList& dependencies);

	/**
	 * \brief Returns the content of the header file with registration code
	 *
	 * \return the content of the header file with registration code
	 */
	const QString& registrationHeader() const;

	/**
	 * \brief Returns the content of the source file with registration code
	 *
	 * \return the content of the source file with registration code
	 */
	const QString& registrationSource() const;

private:
	void generateCode();
	bool checkNothingToRegister() const;
	void extractNeededData();
	void generateRegistrationString();
	QString generateFunctionCalls(const QStringList& funcs) const;
	void fillDependencySorter();
	QString generateClassesRegistrations() const;
	void generateHeader();
	void generateSource();

	const QString m_generatedFilename;
	const QString m_pluginName;
	const QList<FilenameAndParsedHeader> m_headers;
	const QStringList m_dependencies;
	QString m_headerGuard;
	QString m_generatedClassName;
	QString m_headerFilename;
	QString m_dependenciesString;
	QString m_headerIncludeString;
	QString m_registrationString;
	DependencySorter<QString> m_dependencySorter;
	QString m_header;
	QString m_source;
};

/**
 * \brief Generates the source files with registration and dependencies commands
 *
 * This throws a salsa::RuntimeUserException exception in case of error or a
 * salsa::CircularDependencyException in case of circular dependencies in
 * classes registration
 * \param pluginName the name of plugin being compiled
 * \param destDir the directory where generated files will be written
 * \param headersDestDir the directory where parsed headers will be written
 * \param genSource the name of the source files to generate
 * \param dependenciesFile the name of the file containing the list of plugin
 *                         dependencies
 * \param headers the list of headers to parse
 */
void doParse(const QString& pluginName, const QString& destDir, const QString& headersDestDir, const QString& genSource, const QString& dependenciesFile, const QStringList& headers);

}

}

#endif
