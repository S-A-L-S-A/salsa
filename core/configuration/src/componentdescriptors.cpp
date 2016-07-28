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

#include "componentdescriptors.h"
#include "configurationmanager.h"
#include "component.h"
#include <limits>

#warning FORSE SAREBBE IL CASO DI AVERE NELLE DESCRIZIONI DEI TIPI UN RIFERIMENTO ALLE SUPERCLASSI: IN QUESTO MODO NON SI DEVE CHIAMARE LA PARENT describe IN describe E SI RISPARMIA MEMORIA (ANCHE SE LA RICERCA DI UN PARAMETRO SAREBBE PIÙ COMPLESSA E LENTA)
namespace salsa {

namespace {
#warning MOLTE DI QUESTE FUNZIONI POTREBBERO ESSERE UTILI DA METTERE IN ConfigurationManager O IN ConfigurationHelper

	QString bool2String(bool v)
	{
		return v ? "true" : "false";
	}

	bool string2Bool(QString s)
	{
		if ((s.toUpper() == "TRUE") || (s.toUpper() == "T") || (s.toUpper() == "YES") || (s.toUpper() == "Y") || (s == "1")) {
			return true;
		} else {
			return false;
		}
	}

	// Creates a parameter if it doesn't exist and sets its value
	void overrideParameterValue(ConfigurationManager& descr, QString group, QString paramName, QString value)
	{
		const QString paramPath = group + GroupSeparator + paramName;
		if (!descr.parameterExists(paramPath)) {
			descr.createParameter(group, paramName);
		}
		descr.setValue(paramPath, value);
	}

	// Like the above for const char (to avoid the next one to be used)
	void overrideParameterValue(ConfigurationManager& descr, QString group, QString paramName, const char* value)
	{
		overrideParameterValue(descr, group, paramName, QString(value));
	}

	// Like the one above for bool values
	void overrideParameterValue(ConfigurationManager& descr, QString group, QString paramName, bool value)
	{
		overrideParameterValue(descr, group, paramName, bool2String(value));
	}

	QString getParameterValueIfExisting(const ConfigurationManager& descr, QString group, QString paramName)
	{
		const QString paramPath = group + GroupSeparator + paramName;
		if (descr.parameterExists(paramPath)) {
			return descr.getValue(paramPath);
		} else {
			return QString();
		}
	}

	void overrideParameterType(ConfigurationManager& descr, QString group, DescriptorType t)
	{
		overrideParameterValue(descr, group, "type", descriptorType2String(t));
	}

	template <class T>
	void overrideParameterDefaultValue(ConfigurationManager& descr, QString group, T value)
	{
		overrideParameterValue(descr, group, "default", value);
	}

	QString getParameterDefaultIfSet(const ConfigurationManager& descr, QString group)
	{
		return getParameterValueIfExisting(descr, group, "default");
	}

	bool checkParameterDefaultIsSet(const ConfigurationManager& descr, QString group)
	{
		return descr.parameterExists(group + GroupSeparator + "default");
	}

	ParameterProperties getParameterProperties(const ConfigurationManager& descr, QString group)
	{
		ParameterProperties props(ParamDefault);

		if (string2Bool(getParameterValueIfExisting(descr, group, "isList"))) {
			props |= ParamIsList;
		}
		if (string2Bool(getParameterValueIfExisting(descr, group, "isMandatory"))) {
			props |= ParamIsMandatory;
		}
		if (string2Bool(getParameterValueIfExisting(descr, group, "allowMultiple"))) {
			props |= ParamAllowMultiple;
		}

		return props;
	}

	QString getLongHelp(QString shortHelp, QString longHelp)
	{
		return longHelp.isEmpty() ? shortHelp : longHelp;
	}

	void createGroupIfNotExisting(ConfigurationManager& descr, QString group)
	{
		if (!descr.groupExists(group)) {
			descr.createGroup(group);
		}
	}
}

// NOTE: These are here and not directly in the header because if we put them there sometimes
// the value of MaxInteger and MinInteger is 0 in some compilation units and the correct value
// in others (perhaps a bug in gcc?). Putting them here makes sure that the value is always
// the same (and it seems to be the correct one).
const double Infinity = std::numeric_limits<double>::infinity();
const int MaxInteger = std::numeric_limits<int>::max();
const int MinInteger = std::numeric_limits<int>::min();

QString descriptorType2String(DescriptorType type)
{
	switch (type) {
		case UnknownDescriptorType:
			return "unknown";
		case StringDescriptorType:
			return "string";
		case IntDescriptorType:
			return "int";
		case RealDescriptorType:
			return "real";
		case BoolDescriptorType:
			return "bool";
		case EnumDescriptorType:
			return "enum";
		case ComponentDescriptorType:
			return "component";
		case SubgroupDescriptorType:
			return "subgroup";
	}

	// This is only to avoid compiler warnings
	return "unknown";
}

template <>
DescriptorType type2DescriptorType<StringDescriptor>()
{
	return StringDescriptorType;
}

template <>
DescriptorType type2DescriptorType<IntDescriptor>()
{
	return IntDescriptorType;
}

template <>
DescriptorType type2DescriptorType<RealDescriptor>()
{
	return RealDescriptorType;
}

template <>
DescriptorType type2DescriptorType<BoolDescriptor>()
{
	return BoolDescriptorType;
}

template <>
DescriptorType type2DescriptorType<EnumDescriptor>()
{
	return EnumDescriptorType;
}

template <>
DescriptorType type2DescriptorType<ComponentDescriptor>()
{
	return ComponentDescriptorType;
}

template <>
DescriptorType type2DescriptorType<SubgroupDescriptor>()
{
	return SubgroupDescriptorType;
}

DescriptorType string2DescriptorType(const QString& str)
{
	if (str == "string") {
		return StringDescriptorType;
	} else if (str == "int") {
		return IntDescriptorType;
	} else if (str == "real") {
		return RealDescriptorType;
	} else if (str == "bool") {
		return BoolDescriptorType;
	} else if (str == "enum") {
		return EnumDescriptorType;
	} else if (str == "component") {
		return ComponentDescriptorType;
	} else if (str == "subgroup") {
		return SubgroupDescriptorType;
	} else {
		return UnknownDescriptorType;
	}
}

AbstractDescriptor::AbstractDescriptor(ConfigurationManager* descr, const QString& path)
	: m_descr(descr)
	, m_path(path)
{
	createGroupIfNotExisting(typeDescr(), m_path);
	setProperties(ParamDefault);
}

AbstractDescriptor::~AbstractDescriptor()
{
}

const QString& AbstractDescriptor::path() const
{
	return m_path;
}

QString AbstractDescriptor::shortHelp() const
{
	return getParameterValueIfExisting(typeDescr(), m_path, "shortHelp");
}

QString AbstractDescriptor::longHelp() const
{
	return getLongHelp(shortHelp(), getParameterValueIfExisting(typeDescr(), m_path, "longHelp"));
}

ParameterProperties AbstractDescriptor::props() const
{
	return getParameterProperties(typeDescr(), m_path);
}

void AbstractDescriptor::setHelp(QString shortHelp, QString longHelp)
{
	overrideParameterValue(typeDescr(), m_path, "shortHelp", shortHelp);
	overrideParameterValue(typeDescr(), m_path, "longHelp", getLongHelp(shortHelp, longHelp));
}

void AbstractDescriptor::setProperties(ParameterProperties properties)
{
	overrideParameterValue(typeDescr(), m_path, "isList", properties.testFlag(ParamIsList));
	overrideParameterValue(typeDescr(), m_path, "isMandatory", properties.testFlag(ParamIsMandatory));
	overrideParameterValue(typeDescr(), m_path, "allowMultiple", properties.testFlag(ParamAllowMultiple));
}

ConfigurationManager& AbstractDescriptor::typeDescr()
{
	return *m_descr;
}

const ConfigurationManager& AbstractDescriptor::typeDescr() const
{
	return *m_descr;
}

AbstractDescriptorContainer::AbstractDescriptorContainer()
	: m_descriptors()
	, m_mutex()
{
}

AbstractDescriptorContainer::~AbstractDescriptorContainer()
{
	foreach (AbstractDescriptor* d, m_descriptors) {
		delete d;
	}
}

bool AbstractDescriptorContainer::hasParameterOrSubgroup(const QString& name) const
{
	return (descriptorType(name) != UnknownDescriptorType);
}

DescriptorType AbstractDescriptorContainer::descriptorType(const QString& name) const
{
	QMutexLocker locker(&m_mutex);

	// Checking both among parameters and subgroups
	const DescriptorType pType = descriptorTypeFromFullPath(pathForParameter(name));
	if (pType == UnknownDescriptorType) {
		// If not a parameter, checking among subgroups
		return descriptorTypeFromFullPath(pathForSubgroup(name));
	}

	// If we got here this was a parameter
	return pType;
}

QStringList AbstractDescriptorContainer::parameters() const
{
	QStringList l;

	const QString groupWithParameters = path() + GroupSeparator + "Parameters";
	if (typeDescr()->groupExists(groupWithParameters)) {
		l = typeDescr()->getGroupsList(groupWithParameters);
	}

	return l;
}

QStringList AbstractDescriptorContainer::subgroups() const
{
	QStringList l;

	const QString groupWithParameters = path() + GroupSeparator + "Subgroups";
	if (typeDescr()->groupExists(groupWithParameters)) {
		l = typeDescr()->getGroupsList(groupWithParameters);
	}

	return l;
}

StringDescriptor& AbstractDescriptorContainer::describeString(QString parameter)
{
	return createParameterDescriptor<StringDescriptor>(parameter);
}

IntDescriptor& AbstractDescriptorContainer::describeInt(QString parameter)
{
	return createParameterDescriptor<IntDescriptor>(parameter);
}

RealDescriptor& AbstractDescriptorContainer::describeReal(QString parameter)
{
	return createParameterDescriptor<RealDescriptor>(parameter);
}

BoolDescriptor& AbstractDescriptorContainer::describeBool(QString parameter)
{
	return createParameterDescriptor<BoolDescriptor>(parameter);
}

EnumDescriptor& AbstractDescriptorContainer::describeEnum(QString parameter)
{
	return createParameterDescriptor<EnumDescriptor>(parameter);
}

ComponentDescriptor& AbstractDescriptorContainer::describeComponent(QString parameter)
{
	return createParameterDescriptor<ComponentDescriptor>(parameter);
}

SubgroupDescriptor& AbstractDescriptorContainer::describeSubgroup(QString subgroup)
{
	return createSubgroupDescriptor(subgroup);
}

const AbstractDescriptor& AbstractDescriptorContainer::parameterDescriptor(const QString& parameter) const
{
	return *(descriptor(pathForParameter(parameter)));
}

const StringDescriptor& AbstractDescriptorContainer::stringDescriptor(const QString& parameter) const
{
	return typedParameterDescriptor<StringDescriptor>(parameter);
}

const IntDescriptor& AbstractDescriptorContainer::intDescriptor(const QString& parameter) const
{
	return typedParameterDescriptor<IntDescriptor>(parameter);
}

const RealDescriptor& AbstractDescriptorContainer::realDescriptor(const QString& parameter) const
{
	return typedParameterDescriptor<RealDescriptor>(parameter);
}

const BoolDescriptor& AbstractDescriptorContainer::boolDescriptor(const QString& parameter) const
{
	return typedParameterDescriptor<BoolDescriptor>(parameter);
}

const EnumDescriptor& AbstractDescriptorContainer::enumDescriptor(const QString& parameter) const
{
	return typedParameterDescriptor<EnumDescriptor>(parameter);
}

const ComponentDescriptor& AbstractDescriptorContainer::componentDescriptor(const QString& parameter) const
{
	return typedParameterDescriptor<ComponentDescriptor>(parameter);
}

const SubgroupDescriptor& AbstractDescriptorContainer::subgroupDescriptor(const QString& subgroup) const
{
	return typedSubgroupDescriptor(subgroup);
}

QString AbstractDescriptorContainer::relPathForParameter(const QString& parameter) const
{
	return QString("Parameters") + GroupSeparator + parameter;
}

QString AbstractDescriptorContainer::relPathForSubgroup(const QString& subgroup) const
{
	return QString("Subgroups") + GroupSeparator + subgroup;
}

QString AbstractDescriptorContainer::pathForParameter(const QString& parameter) const
{
	return path() + GroupSeparator + relPathForParameter(parameter);
}

QString AbstractDescriptorContainer::pathForSubgroup(const QString& subgroup) const
{
	return path() + GroupSeparator + relPathForSubgroup(subgroup);
}

const AbstractDescriptor* AbstractDescriptorContainer::descriptor(const QString& path) const
{
	QMutexLocker locker(&m_mutex);

	if (!m_descriptors.contains(path)) {
		throw ParameterOrSubgroupNotDescribedException(path.toLatin1().data());
	}

	return m_descriptors.value(path);
}

DescriptorType AbstractDescriptorContainer::descriptorTypeFromFullPath(const QString& path) const
{
	if (m_descriptors.contains(path)) {
		return m_descriptors.value(path)->type();
	} else {
		return UnknownDescriptorType;
	}
}

StringDescriptor::StringDescriptor(ConfigurationManager* descr, QString paramPath)
	: AbstractDescriptor(descr, paramPath)
{
	overrideParameterType(typeDescr(), path(), StringDescriptorType);
}

StringDescriptor::~StringDescriptor()
{
}

DescriptorType StringDescriptor::type() const
{
	return StringDescriptorType;
}

StringDescriptor& StringDescriptor::def(QString defaultValue)
{
	overrideParameterDefaultValue(typeDescr(), path(), defaultValue);

	return *this;
}

bool StringDescriptor::hasDefault() const
{
	return checkParameterDefaultIsSet(typeDescr(), path());
}

QString StringDescriptor::def() const
{
	if (!hasDefault()) {
		throw ParameterNotRegisteredOrWithoutDefaultException(path().toLatin1().data());
	}

	return getParameterDefaultIfSet(typeDescr(), path());
}

StringDescriptor& StringDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

StringDescriptor& StringDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

IntDescriptor::IntDescriptor(ConfigurationManager* descr, QString paramPath)
	: AbstractDescriptor(descr, paramPath)
{
	overrideParameterType(typeDescr(), path(), IntDescriptorType);
}

IntDescriptor::~IntDescriptor()
{
}

DescriptorType IntDescriptor::type() const
{
	return IntDescriptorType;
}

IntDescriptor& IntDescriptor::def(int defaultValue)
{
	overrideParameterDefaultValue(typeDescr(), path(), QString::number(defaultValue));

	return *this;
}

bool IntDescriptor::hasDefault() const
{
	return checkParameterDefaultIsSet(typeDescr(), path());
}

int IntDescriptor::def() const
{
	if (!hasDefault()) {
		throw ParameterNotRegisteredOrWithoutDefaultException(path().toLatin1().data());
	}

	const QString strDef = getParameterDefaultIfSet(typeDescr(), path());
	bool ok;
	const int intDef = strDef.toInt(&ok);
	if (Q_UNLIKELY(!ok)) {
		qFatal("Invalid registered default value (should never happend)");
	}

	return intDef;
}

IntDescriptor& IntDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

IntDescriptor& IntDescriptor::limits(int lowerBound, int upperBound)
{
	if (lowerBound != MinInteger) {
		overrideParameterValue(typeDescr(), path(), "lowerBound", QString::number(lowerBound));
	}
	if (upperBound != MaxInteger) {
		overrideParameterValue(typeDescr(), path(), "upperBound", QString::number(upperBound));
	}

	return *this;
}

int IntDescriptor::lowerBound() const
{
	const QString pathWithLowerLimit = path() + GroupSeparator + "lowerBound";
	int l = MinInteger;

	if (typeDescr().parameterExists(pathWithLowerLimit)) {
		bool ok;
		const QString& lowerStr = typeDescr().getValue(pathWithLowerLimit);
		l = lowerStr.toInt(&ok);

		if (Q_UNLIKELY(!ok)) {
			qFatal("Fatal error invalid value for lower limit (in %s)", pathWithLowerLimit.toLatin1().data());
		}
	}

	return l;
}

int IntDescriptor::upperBound() const
{
	const QString pathWithUpperLimit = path() + GroupSeparator + "upperBound";
	int u = MaxInteger;

	if (typeDescr().parameterExists(pathWithUpperLimit)) {
		bool ok;
		const QString& upperStr = typeDescr().getValue(pathWithUpperLimit);
		u = upperStr.toInt(&ok);

		if (Q_UNLIKELY(!ok)) {
			qFatal("Fatal error invalid value for upper limit (in %s)", pathWithUpperLimit.toLatin1().data());
		}
	}

	return u;
}

IntDescriptor& IntDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

RealDescriptor::RealDescriptor(ConfigurationManager* descr, QString paramPath)
	: AbstractDescriptor(descr, paramPath)
{
	overrideParameterType(typeDescr(), path(), RealDescriptorType);
}

RealDescriptor::~RealDescriptor()
{
}

DescriptorType RealDescriptor::type() const
{
	return RealDescriptorType;
}

RealDescriptor& RealDescriptor::def(double defaultValue)
{
	overrideParameterDefaultValue(typeDescr(), path(), QString::number(defaultValue));

	return *this;
}

bool RealDescriptor::hasDefault() const
{
	return checkParameterDefaultIsSet(typeDescr(), path());
}

double RealDescriptor::def() const
{
	if (!hasDefault()) {
		throw ParameterNotRegisteredOrWithoutDefaultException(path().toLatin1().data());
	}

	const QString strDef = getParameterDefaultIfSet(typeDescr(), path());
	bool ok;
	const double realDef = strDef.toDouble(&ok);
	if (Q_UNLIKELY(!ok)) {
		qFatal("Invalid registered default value (should never happend)");
	}

	return realDef;
}

RealDescriptor& RealDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

RealDescriptor& RealDescriptor::limits(double lowerBound, double upperBound)
{
	if (lowerBound != -Infinity) {
		overrideParameterValue(typeDescr(), path(), "lowerBound", QString::number(lowerBound));
	}
	if (upperBound != +Infinity) {
		overrideParameterValue(typeDescr(), path(), "upperBound", QString::number(upperBound));
	}

	return *this;
}

double RealDescriptor::lowerBound() const
{
	const QString pathWithLowerLimit = path() + + GroupSeparator + "lowerBound";
	double l = -Infinity;

	if (typeDescr().parameterExists(pathWithLowerLimit)) {
		bool ok;
		const QString& lowerStr = typeDescr().getValue(pathWithLowerLimit);
		l = lowerStr.toDouble(&ok);

		if (Q_UNLIKELY(!ok)) {
			qFatal("Fatal error invalid value for lower limit (in %s)", pathWithLowerLimit.toLatin1().data());
		}
	}

	return l;
}

double RealDescriptor::upperBound() const
{
	const QString pathWithUpperLimit = path() + GroupSeparator + "upperBound";
	double u = +Infinity;

	if (typeDescr().parameterExists(pathWithUpperLimit)) {
		bool ok;
		const QString& upperStr = typeDescr().getValue(pathWithUpperLimit);
		u = upperStr.toDouble(&ok);

		if (Q_UNLIKELY(!ok)) {
			qFatal("Fatal error invalid value for upper limit (in %s)", pathWithUpperLimit.toLatin1().data());
		}
	}

	return u;
}

RealDescriptor& RealDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

BoolDescriptor::BoolDescriptor(ConfigurationManager* descr, QString paramPath)
	: AbstractDescriptor(descr, paramPath)
{
	overrideParameterType(typeDescr(), path(), BoolDescriptorType);
}

BoolDescriptor::~BoolDescriptor()
{
}

DescriptorType BoolDescriptor::type() const
{
	return BoolDescriptorType;
}

BoolDescriptor& BoolDescriptor::def(bool defaultValue)
{
	overrideParameterDefaultValue(typeDescr(), path(), defaultValue);

	return *this;
}

bool BoolDescriptor::hasDefault() const
{
	return checkParameterDefaultIsSet(typeDescr(), path());
}

bool BoolDescriptor::def() const
{
	if (!hasDefault()) {
		throw ParameterNotRegisteredOrWithoutDefaultException(path().toLatin1().data());
	}

	const QString strDef = getParameterDefaultIfSet(typeDescr(), path());

	return string2Bool(strDef);
}

BoolDescriptor& BoolDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

BoolDescriptor& BoolDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

EnumDescriptor::EnumDescriptor(ConfigurationManager* descr, QString paramPath)
	: AbstractDescriptor(descr, paramPath)
{
	overrideParameterType(typeDescr(), path(), EnumDescriptorType);
}

EnumDescriptor::~EnumDescriptor()
{
}

DescriptorType EnumDescriptor::type() const
{
	return EnumDescriptorType;
}

EnumDescriptor& EnumDescriptor::def(QString defaultValue)
{
	overrideParameterDefaultValue(typeDescr(), path(), defaultValue);

	return *this;
}

bool EnumDescriptor::hasDefault() const
{
	return checkParameterDefaultIsSet(typeDescr(), path());
}

QString EnumDescriptor::def() const
{
	if (!hasDefault()) {
		throw ParameterNotRegisteredOrWithoutDefaultException(path().toLatin1().data());
	}

	return getParameterDefaultIfSet(typeDescr(), path());
}

EnumDescriptor& EnumDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

EnumDescriptor& EnumDescriptor::values(QStringList allValues)
{
	for (int i = 0; i < allValues.size(); ++i) {
		overrideParameterValue(typeDescr(), path(), "enum:" + QString::number(i), allValues[i]);
	}

	return *this;
}

QStringList EnumDescriptor::values() const
{
	const QStringList enumParams = typeDescr().getParametersWithPrefixList(path(), "enum:");

	QStringList values;
	foreach (QString p, enumParams) {
		values.append(typeDescr().getValue(path() + GroupSeparator + p));
	}

	return values;
}

EnumDescriptor& EnumDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

ComponentDescriptor::ComponentDescriptor(ConfigurationManager* descr, QString paramPath)
	: AbstractDescriptor(descr, paramPath)
{
	overrideParameterType(typeDescr(), path(), ComponentDescriptorType);
}

ComponentDescriptor::~ComponentDescriptor()
{
}

DescriptorType ComponentDescriptor::type() const
{
	return ComponentDescriptorType;
}

ComponentDescriptor& ComponentDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

ComponentDescriptor& ComponentDescriptor::componentType(QString typeName)
{
	overrideParameterValue(typeDescr(), path(), "componentType", typeName);

	return *this;
}

QString ComponentDescriptor::componentType() const
{
	return getParameterValueIfExisting(typeDescr(), path(), "componentType");
}

ComponentDescriptor& ComponentDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

SubgroupDescriptor::SubgroupDescriptor(ConfigurationManager* descr, QString subgroupPath)
	: AbstractDescriptor(descr, subgroupPath)
	, AbstractDescriptorContainer()
{
}

SubgroupDescriptor::~SubgroupDescriptor()
{
}

DescriptorType SubgroupDescriptor::type() const
{
	return SubgroupDescriptorType;
}

SubgroupDescriptor& SubgroupDescriptor::props(ParameterProperties properties)
{
	setProperties(properties);

	return *this;
}

SubgroupDescriptor& SubgroupDescriptor::componentType(QString typeName)
{
	overrideParameterValue(AbstractDescriptor::typeDescr(), path(), "componentType", typeName);

	return *this;
}

QString SubgroupDescriptor::componentType() const
{
	return getParameterValueIfExisting(AbstractDescriptor::typeDescr(), path(), "componentType");
}

SubgroupDescriptor& SubgroupDescriptor::help(QString shortHelp, QString longHelp)
{
	setHelp(shortHelp, longHelp);

	return *this;
}

ConfigurationManager* SubgroupDescriptor::typeDescr()
{
	return &(AbstractDescriptor::typeDescr());
}

const ConfigurationManager* SubgroupDescriptor::typeDescr() const
{
	return &(AbstractDescriptor::typeDescr());
}

const QString& SubgroupDescriptor::path() const
{
	return AbstractDescriptor::path();
}

RegisteredComponentDescriptor::RegisteredComponentDescriptor(ConfigurationManager* descr, QString type)
	: AbstractDescriptorContainer()
	, m_descr(descr)
	, m_type(type)
{
	createGroupIfNotExisting(*m_descr, m_type);
}

RegisteredComponentDescriptor::~RegisteredComponentDescriptor()
{
}

const QString& RegisteredComponentDescriptor::type() const
{
	return m_type;
}

QString RegisteredComponentDescriptor::shortHelp() const
{
	return getParameterValueIfExisting(*m_descr, m_type, "shortHelp");
}

QString RegisteredComponentDescriptor::longHelp() const
{
	return getLongHelp(shortHelp(), getParameterValueIfExisting(*m_descr, m_type, "longHelp"));
}

void RegisteredComponentDescriptor::help(QString shortHelp, QString longHelp)
{
	overrideParameterValue(*m_descr, m_type, "shortHelp", shortHelp);
	overrideParameterValue(*m_descr, m_type, "longHelp", getLongHelp(shortHelp, longHelp));
}

ConfigurationManager* RegisteredComponentDescriptor::typeDescr()
{
	return m_descr;
}

const ConfigurationManager* RegisteredComponentDescriptor::typeDescr() const
{
	return m_descr;
}

const QString& RegisteredComponentDescriptor::path() const
{
	return m_type;
}

} // end namespace salsa
