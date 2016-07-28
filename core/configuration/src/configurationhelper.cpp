/***************************************************************************
 *  SALSA Configuration Library                                            *
 *  Copyright (C) 2007-2013                                                *
 *  Gianluca Massera <emmegian@yahoo.it>                                   *
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

#include "configurationhelper.h"
#include "configurationexceptions.h"
#include "configurationnode.h"
#include "configurationmanager.h"
#include "componentdescriptors.h"
#include "typesdb.h"

#warning TROVARE UN MODO PER SEMPLIFICARE L USO DELLE FUNZIONI IN UN COMPONENT, VEDI COMMENTO SOTTO
// Nei component si fa sempre una cosa del tipo:
// 	ConfigurationHelper::getReal(configurationManager(), confPath() + "playgroundHeight");
// Semplificare questa cosa, in modo da poter scrivere
// 	ConfigurationHelper::getReal("playgroundHeight")
// o qualcosa del genere

namespace salsa {

namespace ConfigurationHelper {

// Anonymous namespace with helper functions
namespace {
	double stringToDoubleAcceptingInfinite(const QString& str, bool* ok)
	{
		if (ok != NULL) {
			*ok = true;
		}

		bool internalOk;
		double d = str.toDouble(&internalOk);
		if (!internalOk) {
			QString trimmedStr = str.trimmed();
			if ((trimmedStr == "+inf") || (trimmedStr == "inf")) {
				d = +Infinity;
			} else if (trimmedStr == "-inf") {
				d = -Infinity;
			} else {
				if (ok != NULL) {
					*ok = false;
				}
			}
		}

		return d;
	}

	bool boolToString(QString paramPath, QString value)
	{
		bool retVal;

		if ((value == "t") || (value == "true") || (value == "1")) {
			retVal = true;
		} else if ((value == "f") || (value == "false") || (value == "0")) {
			retVal = false;
		} else {
			throw CannotConvertParameterValueToTypeException(paramPath.toLatin1().data(), "bool");
		}

		return retVal;
	}

	template <class T>
	const T& getTypedDescriptorForParameter(const ConfigurationManager& params, QString paramPath)
	{
		const AbstractDescriptor& abstractDescriptor = getDescriptorForParameter(params, paramPath);

		const DescriptorType type = type2DescriptorType<T>();
		if (abstractDescriptor.type() != type) {
			throw WrongParameterTypeException(
				paramPath.toLatin1().data(),
				descriptorType2String(abstractDescriptor.type()).toLatin1().data(),
				descriptorType2String(type).toLatin1().data());
		}

		return dynamic_cast<const T&>(abstractDescriptor);
	}
}

#warning ====================== SEMPLIFICARE QUESTE FUNZIONI USANDO I NUOVI DESCRIPTORS!!! ======================

int getInt(const ConfigurationManager& params, QString paramPath)
{
	const IntDescriptor& descriptor = getTypedDescriptorForParameter<IntDescriptor>(params, paramPath);

	const int value = getInt(params, paramPath, descriptor.def());

	if ((value < descriptor.lowerBound()) || (value > descriptor.upperBound())) {
		throw ParameterValueOutOfRangeException(paramPath.toLatin1().data(), QString::number(value).toLatin1().data());
	}

	return value;
}

int getInt(const ConfigurationManager& params, QString paramPath, int defaultValue)
{
	if (!params.parameterExists(paramPath)) {
		return defaultValue;
	}

	const QString value = params.getValue(paramPath).toLower().trimmed();
	bool ok;
	const int valueInt = value.toInt(&ok);
	if (!ok) {
		CannotConvertParameterValueToTypeException(paramPath.toLatin1().data(), "int");
	}

	return valueInt;
}

double getReal(const ConfigurationManager& params, QString paramPath)
{
	const RealDescriptor& descriptor = getTypedDescriptorForParameter<RealDescriptor>(params, paramPath);

	const double value = getReal(params, paramPath, descriptor.def());

	if ((value < descriptor.lowerBound()) || (value > descriptor.upperBound())) {
		throw ParameterValueOutOfRangeException(paramPath.toLatin1().data(), QString::number(value).toLatin1().data());
	}

	return value;
}

double getReal(const ConfigurationManager& params, QString paramPath, double defaultValue)
{
	if (!params.parameterExists(paramPath)) {
		return defaultValue;
	}

	const QString value = params.getValue(paramPath).toLower().trimmed();
	bool ok;
	double valueDouble = stringToDoubleAcceptingInfinite(value, &ok);
	if (!ok) {
		throw CannotConvertParameterValueToTypeException(paramPath.toLatin1().data(), "real");
	}

	return valueDouble;
}

bool getBool(const ConfigurationManager& params, QString paramPath)
{
	const BoolDescriptor& descriptor = getTypedDescriptorForParameter<BoolDescriptor>(params, paramPath);

	return getBool(params, paramPath, descriptor.def());
}

bool getBool(const ConfigurationManager& params, QString paramPath, bool defaultValue)
{
	if (!params.parameterExists(paramPath)) {
		return defaultValue;
	}

	const QString value = params.getValue(paramPath).toLower().trimmed();

	return boolToString(paramPath, value);
}

QString getString(const ConfigurationManager& params, QString paramPath)
{
	const StringDescriptor& descriptor = getTypedDescriptorForParameter<StringDescriptor>(params, paramPath);

	return getString(params, paramPath, descriptor.def());
}

QString getString(const ConfigurationManager& params, QString paramPath, QString defaultValue)
{
	if (params.parameterExists(paramPath)) {
		return params.getValue(paramPath);
	} else {
		return defaultValue;
	}
}

QString getEnum(const ConfigurationManager& params, QString paramPath)
{
	const EnumDescriptor& descriptor = getTypedDescriptorForParameter<EnumDescriptor>(params, paramPath);

	const QStringList enumValues = descriptor.values();

	if (!enumValues.contains(descriptor.def())) {
		throw InvalidEnumParameterDefaultException(paramPath.toLatin1().data(), descriptor.def().toLatin1().data());
	}

	const QString value = getEnum(params, paramPath, descriptor.def());

	if (!enumValues.contains(value)) {
		throw InvalidEnumValueException(paramPath.toLatin1().data(), value.toLatin1().data());
	}

	return value;
}

QString getEnum(const ConfigurationManager& params, QString paramPath, QString defaultValue)
{
	if (params.parameterExists(paramPath)) {
		return params.getValue(paramPath);
	} else {
		return defaultValue;
	}
}

QString encodeListOfInts(const QVector<int>& list)
{
	QString str;

	for (int i = 0; i < list.size(); ++i) {
		if (i != 0) {
			str += " ";
		}

		str += QString::number(list[i]);
	}

	return str;
}

QVector<int> decodeListOfInts(const QString& encodedList)
{
	QVector<int> list;

	QStringList strList = encodedList.split(" ", QString::SkipEmptyParts);
	foreach (QString str, strList) {
		bool ok;
		const int v = str.toInt(&ok);
		if (!ok) {
			throw ListDecodeFailedException("int");
		}

		list.append(v);
	}

	return list;
}

QString encodeListOfReals(const QVector<double>& list)
{
	QString str;

	for (int i = 0; i < list.size(); ++i) {
		if (i != 0) {
			str += " ";
		}

		const double v = list[i];
		if (v == +Infinity) {
			str += "+Inf";
		} else if (v == -Infinity) {
			str += "-Inf";
		} else {
			str += QString::number(v);
		}
	}

	return str;
}

QVector<double> decodeListOfReals(const QString& encodedList)
{
	QVector<double> list;

	QStringList strList = encodedList.split(" ", QString::SkipEmptyParts);
	foreach (QString str, strList) {
		bool ok;
		const double v = stringToDoubleAcceptingInfinite(str, &ok);
		if (!ok) {
			throw ListDecodeFailedException("real");
		}

		list.append(v);
	}

	return list;
}

QString encodeListOfBools(const QVector<bool>& list)
{
	QString str;

	foreach (bool v, list) {
		str += v ? "1" : "0";
	}

	return str;
}

QVector<bool> decodeListOfBools(const QString& encodedList)
{
	QVector<bool> list;

	for (int i = 0; i < encodedList.length(); ++i) {
		if (encodedList[i] == '1') {
			list.append(true);
		} else if (encodedList[i] == '0') {
			list.append(false);
		} else {
			throw ListDecodeFailedException("bool");
		}
	}

	return list;
}

QString encodeListOfStrings(const QStringList& list)
{
	QString encodedList;

	for (int i = 0; i < list.size(); ++i) {
		if (i != 0) {
			encodedList += "/";
		}

		encodedList += QString(list[i]).replace("/", "\\/");
	}

	return encodedList;
}

QStringList decodeListOfStrings(const QString& encodedList)
{
	QStringList list;

	if (encodedList.isEmpty()) {
		return list;
	}

	int startElement = 0;
	int charPastPrevSlash = 0;
	while (true) {
		int nextSlash = encodedList.indexOf("/", charPastPrevSlash);
		if (nextSlash == -1) {
			break;
		}

		charPastPrevSlash = nextSlash + 1;

		if ((nextSlash != 0) && (encodedList[nextSlash - 1] == '\\')) {
			continue;
		}

		list.append(encodedList.mid(startElement, nextSlash - startElement).replace("\\/", "/"));
		startElement = nextSlash + 1;
	}

	list.append(encodedList.mid(startElement).replace("\\/", "/"));

	return list;
}

// QStringList ConfigurationHelper::getStringList( ConfigurationManager& params, QString paramPath, QString delimiter ) {
// 	return params.getValue(paramPath).split( delimiter, QString::SkipEmptyParts );
// }
//
// QVector<double> ConfigurationHelper::getVector( ConfigurationManager& params, QString paramPath, QString def ) {
// 	// param will be treated as a vector, that is a list of space-separated values
// 	const QString paramValue = params.getValue(paramPath);
// 	QString str = paramValue.isEmpty() ? def : paramValue;
// 	QVector<double> ret;
// 	if ( str.isEmpty() ) return ret;
// 	// split the string and convert the element into double
// 	QStringList list = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
// 	for( int i=0; i<list.size(); i++ ) {
// 		// if toDouble will fail a 0.0 will be added
// 		ret << list[i].toDouble();
// 	}
// 	return ret;
// }
//
// QVector<int> ConfigurationHelper::getIntegerVector(ConfigurationManager& params, QString paramPath, QString def) {
// 	QVector<int> vector;
// 	const QString paramValue = params.getValue(paramPath);
// 	const QString stringVector = paramValue.isEmpty() ? def : paramValue;
//
// 	if (stringVector.isEmpty()) {
// 		return vector;
// 	}
//
// 	// Split the string and convert the element into integers
// 	QStringList list = stringVector.split(QRegExp("\\s+"), QString::SkipEmptyParts);
// 	for (int i = 0; i < list.size(); i++) {
// 		// If toInt fails, a 0 will be added
// 		vector << list[i].toInt();
// 	}
//
// 	return vector;
// }
//
// QVector<bool> ConfigurationHelper::getBoolVector(ConfigurationManager& params, QString paramPath, QString def) {
// 	QVector<bool> vector;
// 	const QString paramValue = params.getValue(paramPath);
// 	const QString stringVector = paramValue.isEmpty() ? def : paramValue;
//
// 	for (int i = 0; i < stringVector.size(); i++) {
// 		if (stringVector[i] == '0') {
// 			vector.append(false);
// 		} else if (stringVector[i] == '1') {
// 			vector.append(true);
// 		} else {
// 			return QVector<bool>();
// 		}
// 	}
//
// 	return vector;
// }

bool configKeysEqual(const QString& s1, const QString& s2)
{
	// First of all taking the position of :
	const int colonPosS1 = s1.indexOf(':');
	const int colonPosS2 = s2.indexOf(':');

	// Comparing the part before the colon. If colonPosS1 or colonPosS2 is -1 (i.e. no
	// colon is present), letfRef returns the whole string.
	const bool equalBeforeColon = (s1.leftRef(colonPosS1) == s2.leftRef(colonPosS2));

	if (!equalBeforeColon) {
		return false;
	} else {
		// Now computing the numeric value of the string after :, if a valid number if present
		bool numberAfterColonS1Present;
		const int numberAfterColonS1 = s1.mid(colonPosS1 + 1).toUInt(&numberAfterColonS1Present);
		bool numberAfterColonS2Present;
		const int numberAfterColonS2 = s2.mid(colonPosS2 + 1).toUInt(&numberAfterColonS2Present);

		if (numberAfterColonS1Present && numberAfterColonS2Present) {
			return (numberAfterColonS1 == numberAfterColonS2);
		} else {
			return (s1.midRef(colonPosS1 + 1) == s2.midRef(colonPosS2 + 1));
		}
	}
}

bool configKeysLessThan(const QString& s1, const QString& s2)
{
	// First of all taking the position of :
	const int colonPosS1 = s1.indexOf(':');
	const int colonPosS2 = s2.indexOf(':');

	// Comparing the string before the colon. We do more tests only if colons are actually present and
	// the strings before them are equal
	if ((colonPosS1 == -1) || (colonPosS2 == -1) || (s1.leftRef(colonPosS1) != s2.leftRef(colonPosS2))) {
		return (s1 < s2);
	} else {
		// Now computing the numeric value of the string after :, if a valid number if present
		bool numberAfterColonS1Present;
		const int numberAfterColonS1 = s1.mid(colonPosS1 + 1).toUInt(&numberAfterColonS1Present);
		bool numberAfterColonS2Present;
		const int numberAfterColonS2 = s2.mid(colonPosS2 + 1).toUInt(&numberAfterColonS2Present);

		if (numberAfterColonS1Present && numberAfterColonS2Present) {
			return (numberAfterColonS1 < numberAfterColonS2);
		} else if (numberAfterColonS1Present) {
			return true;
		} else if (numberAfterColonS2Present) {
			return false;
		} else {
			return (s1.midRef(colonPosS1 + 1) < s2.midRef(colonPosS2 + 1));
		}
	}
}

void throwUserConfigError(QString paramName, QString paramValue, QString description)
{
	throw UserDefinedCheckFailureException(paramName.toLatin1().data(), paramValue.toLatin1().data(), description.toLatin1().data());
}

const AbstractDescriptor& getDescriptorForParameter(const ConfigurationManager& params, QString paramPath)
{
	// Extracting the path
	ConfigurationNode::ElementAndPath curSplit = ConfigurationNode::separateLastElement(paramPath);

	// From the innermost to the outermost. The first element is the parameter
	QStringList pathToParam;
	pathToParam.append(curSplit.element);

	QString type;
	do {
		const QString groupTypePath = curSplit.elementPath + GroupSeparator + "type";
		if (params.parameterExists(groupTypePath)) {
			type = params.getValue(groupTypePath);
		} else {
			if (curSplit.elementPath.isEmpty()) {
				break;
			}

			curSplit = ConfigurationNode::separateLastElement(curSplit.elementPath);
#warning DIRTY WORKAROUND FOR GROUPS WHICH HAVE PROPERTY ALLOW MULTIPLE
			pathToParam.append(curSplit.element.split(":")[0]);
		}
	} while (type.isEmpty());

	if (type.isEmpty()) {
		throw ParameterOrSubgroupNotDescribedException(paramPath.toLatin1().data());
	}

	const AbstractDescriptorContainer* d = &(TypesDB::instance().typeDescriptor(type));

	for (int i = (pathToParam.size() - 1); i > 0; --i) {
		d = &(d->subgroupDescriptor(pathToParam[i]));
	}

	return d->parameterDescriptor(pathToParam[0]);
}

const AbstractDescriptorContainer& getDescriptorForGroup(const ConfigurationManager& params, QString groupPath)
{
	// From the innermost to the outermost
	QStringList pathToGroup;

	QString type;
	QString curGroup = groupPath;
	do {
		const QString groupTypePath = curGroup + GroupSeparator + "type";
		if (params.parameterExists(groupTypePath)) {
			type = params.getValue(groupTypePath);
		} else {
			ConfigurationNode::ElementAndPath curSplit = ConfigurationNode::separateLastElement(curGroup);
#warning DIRTY WORKAROUND FOR GROUPS WHICH HAVE PROPERTY ALLOW MULTIPLE
			pathToGroup.append(curSplit.element.split(":")[0]);

			if (curSplit.elementPath.isEmpty()) {
				break;
			} else {
				curGroup = curSplit.elementPath;
			}
		}
	} while (type.isEmpty());

	if (type.isEmpty()) {
		throw ParameterOrSubgroupNotDescribedException(groupPath.toLatin1().data());
	}

	const AbstractDescriptorContainer* d = &(TypesDB::instance().typeDescriptor(type));

	for (int i = (pathToGroup.size() - 1); i >= 0; --i) {
		d = &(d->subgroupDescriptor(pathToGroup[i]));
	}

	return *d;
}

QString getDefaultForDescriptorAsString(const AbstractDescriptor& d)
{
	QString def;

	switch (d.type()) {
		case StringDescriptorType:
			def = (dynamic_cast<const StringDescriptor&>(d)).def();
			break;
		case IntDescriptorType:
			def = QString::number((dynamic_cast<const IntDescriptor&>(d)).def());
			break;
		case RealDescriptorType:
			def = QString::number((dynamic_cast<const RealDescriptor&>(d)).def());
			break;
		case BoolDescriptorType:
			def = (dynamic_cast<const BoolDescriptor&>(d)).def() ? "true" : "false";
			break;
		case EnumDescriptorType:
			def = (dynamic_cast<const EnumDescriptor&>(d)).def();
			break;
		default:
			break;
	}

	return def;
}

}

} // end namespace salsa

