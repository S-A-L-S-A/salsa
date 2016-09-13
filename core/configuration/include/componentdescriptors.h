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

#ifndef COMPONENTDESCRIPTORS_H
#define COMPONENTDESCRIPTORS_H

#include "configurationconfig.h"
#include "configurationexceptions.h"
#include <QString>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <memory>

// This is needed to declare friends of RegisteredComponentDescriptor
class ComponentDescription_Test;
class ConfigurationHelper_Test;

namespace salsa {

class ConfigurationManager;

// This is needed bacause declare friends of RegisteredComponentDescriptor
class TypesDB;
class Component;

class AbstractDescriptorContainer;
class StringDescriptor;
class IntDescriptor;
class RealDescriptor;
class BoolDescriptor;
class EnumDescriptor;
class ComponentDescriptor;
class SubgroupDescriptor;

/**
 * \brief The possible types of descriptors
 */
enum DescriptorType
{
	UnknownDescriptorType,
	StringDescriptorType,
	IntDescriptorType,
	RealDescriptorType,
	BoolDescriptorType,
	EnumDescriptorType,
	ComponentDescriptorType,
	SubgroupDescriptorType
};

/**
 * \brief This enum is used to describe the properties of a parameter or a
 *        subgroup in a type description
 *
 * The properties of a parameter of a subgroup are specified using one or OR-ing
 * two or more descriptionProperties enumeration. For example:
 * 	AllowMultiple | isMandatory
 * corresponds to a parameter that is a list of values and that is mandatory
 */
enum ParameterProperty
{
	/** The default property for a parameters (no list, not mandatory and
	    multiples not allowed) */
	ParamDefault = 0x0000,
	/** This means that the parameter accepts a list of values */
	ParamIsList = 0x0001,
	/** Whether the parameter is mandatory or not */
	ParamIsMandatory = 0x0002,
	/** This means that more than one of this parameter can exists; they
	 *  are numbered using the ':' syntax, i.e. a multiple parameter "pippo"
	 *  means that in the configuration files there will be parameters like
	 *  "pippo:0", "pippo:1", and so on */
	ParamAllowMultiple = 0x0004
};
Q_DECLARE_FLAGS(ParameterProperties, ParameterProperty)

} // end namesapce salsa

// This macro works only outside namespaces
Q_DECLARE_OPERATORS_FOR_FLAGS(salsa::ParameterProperties)

namespace salsa {

/**
 * \brief Constant indicating an infinity double number
 */
extern const double Infinity;

/**
 * \brief Constant indicating the maximum int value allowed
 */
extern const int MaxInteger;

/**
 * \brief Constant indicating the minimum int value allowed
 */
extern const int MinInteger;

/**
 * \brief Returns the string representing the given descriptor type
 *
 * \param type the type for which a string representation is requested
 * \return the string representation of the descriptor
 */
QString SALSA_CONF_API descriptorType2String(DescriptorType type);

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is an unknown type
 * \return the DescriptorType representation of the parameter
 */
template <class T>
DescriptorType SALSA_CONF_TEMPLATE type2DescriptorType()
{
	return UnknownDescriptorType;
}

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a StringDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<StringDescriptor>();

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a IntDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<IntDescriptor>();

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a RealDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<RealDescriptor>();

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a BoolDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<BoolDescriptor>();

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a EnumDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<EnumDescriptor>();

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a ComponentDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<ComponentDescriptor>();

/**
 * \brief Returns the DescriptorType representing the given descriptor
 *
 * The template parameter T is the type whose DescriptorType representation is
 * requested. This function is called if T is a SubgroupDescriptor
 * \return the DescriptorType representation of the parameter
 */
template <>
DescriptorType SALSA_CONF_API type2DescriptorType<SubgroupDescriptor>();

/**
 * \brief Returns the DescriptorType for the given string
 *
 * \param str the string to convert
 * \return the DescriptorType for the given string. If str does not represent
 *         any type, UnknownDescriptorType is returned
 */
DescriptorType SALSA_CONF_API string2DescriptorType(const QString& str);

/**
 * \brief The abstract base of all descriptors
 *
 * This has a protected costructor and removed copy costructor and operator.
 * Instances of this class and of its subclasses cannot be created directly
 */
class SALSA_CONF_API AbstractDescriptor
{
protected:
	AbstractDescriptor(ConfigurationManager* descr, const QString& path);
	virtual ~AbstractDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const = 0;

	/**
	 * \brief Returns the path of the parameter/group of this descriptor
	 *
	 * \return the path of the parameter/group of this descriptor
	 */
	const QString& path() const;

	/**
	 * \brief Returns the short help string
	 *
	 * \return the short help string
	 */
	QString shortHelp() const;

	/**
	 * \brief Returns the long help string
	 *
	 * \return the long help string
	 */
	QString longHelp() const;

	/**
	 * \brief Returns the properties of this parameter
	 *
	 * \return the properties of this parameter
	 */
	ParameterProperties props() const;

protected:
	// This is protected because all subclasses should have a public
	// help(QString, QString) function returning a reference to this that
	// calls this function. If the long help is not specified, it will be
	// set to shortHelp. Rich text is only allowed in longHelp
	void setHelp(QString shortHelp, QString longHelp = QString());

	// This is protected because all subclasses should have a public props
	// function that returns a reference to this (of the correct type)
	void setProperties(ParameterProperties properties);

	ConfigurationManager& typeDescr();
	const ConfigurationManager& typeDescr() const;

private:
	ConfigurationManager* const m_descr;
	const QString m_path;

	// These are here to disable them
	AbstractDescriptor(const AbstractDescriptor& other);
	AbstractDescriptor& operator=(const AbstractDescriptor& other);

	// This is friend to be able to destroy instances of this class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief The abstract base class of all descriptors that contain other
 *        descriptors
 */
class SALSA_CONF_API AbstractDescriptorContainer
{
public:
	/**
	 * \brief Returns true if a parameter or subgroup with the given name
	 *        exists
	 *
	 * \param name the name of the parameter or subgroup to check
	 * \return true if a parameter or subgroup with the given name exists
	 */
	bool hasParameterOrSubgroup(const QString& name) const;

	/**
	 * \brief Returns the type of the parameter or subgroup with the given
	 *        name
	 *
	 * If no parameter nor subgroup exists with the given name, returns
	 * UnknownDescriptorType
	 * \param name the name of the parameter or subgroup to check
	 * \return the type of the parameter
	 */
	DescriptorType descriptorType(const QString& name) const;

	/**
	 * \brief Returns the list of parameters
	 *
	 * \return the list of parameters
	 */
	QStringList parameters() const;

	/**
	 * \brief Returns the list of subgroups
	 *
	 * \return the list of subgroups
	 */
	QStringList subgroups() const;

	/**
	 * \brief Adds or modifies the description of a string parameter
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of a string parameter
	 * \param parameter the name of the parameter to describe
	 * \return an instance of StringDescriptor for setting all others fields
	 *         of the description
	 */
	StringDescriptor& describeString(QString parameter);

	/**
	 * \brief Adds or modifies the description of an int parameter
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of an int parameter
	 * \param parameter the name of the parameter to describe
	 * \return an instance of IntDescriptor for setting all others fields
	 *         of the description
	 */
	IntDescriptor& describeInt(QString parameter);

	/**
	 * \brief Adds or modifies the description of a real parameter
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of a real parameter
	 * \param parameter the name of the parameter to describe
	 * \return an instance of RealDescriptor for setting all others fields
	 *         of the description
	 */
	RealDescriptor& describeReal(QString parameter);

	/**
	 * \brief Adds or modifies the description of a bool parameter
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of a bool parameter
	 * \param parameter the name of the parameter to describe
	 * \return an instance of BoolDescriptor for setting all others fields
	 *         of the description
	 */
	BoolDescriptor& describeBool(QString parameter);

	/**
	 * \brief Adds or modifies the description of a parameter which can take
	 *        one value over a fixed set of values
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of an enum-like parameter
	 * \param parameter the name of the parameter to describe
	 * \return an instance of EnumDescriptor for setting all others fields
	 *         of the description
	 */
	EnumDescriptor& describeEnum(QString parameter);

	/**
	 * \brief Adds or modifies the description of a component parameter
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of a component parameter
	 * \param parameter the name of the parameter to describe
	 * \return an instance of ComponentDescriptor for setting all others
	 *         fields of the description
	 */
	ComponentDescriptor& describeComponent(QString parameter);

	/**
	 * \brief Adds or modifies the description of a subgroup
	 *
	 * This method adds (if there was no previous description of the
	 * parameter) or modifies (changing the previous description) the
	 * description of a subgroup of this subgroup
	 * \param subgroup the name of the subgroup to describe
	 * \return an instance of SubgroupDescriptor for setting all others
	 *         fields of the description
	 */
	SubgroupDescriptor& describeSubgroup(QString subgroup);

	/**
	 * \brief Returns the descriptor of the given parameter
	 *
	 * This throws an exception if the parameter is not described
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const AbstractDescriptor& parameterDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given string parameter
	 *
	 * This throws an exception if the parameter is not described or if it
	 * is not a string.
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const StringDescriptor& stringDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given int parameter
	 *
	 * This throws an exception if the parameter is not described or if it
	 * is not an int.
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const IntDescriptor& intDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given real parameter
	 *
	 * This throws an exception if the parameter is not described or if it
	 * is not a real.
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const RealDescriptor& realDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given bool parameter
	 *
	 * This throws an exception if the parameter is not described or if it
	 * is not a bool.
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const BoolDescriptor& boolDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given enum parameter
	 *
	 * This throws an exception if the parameter is not described or if it
	 * is not an enum.
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const EnumDescriptor& enumDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given component parameter
	 *
	 * This throws an exception if the parameter is not described or if it
	 * is not a component.
	 * \param parameter the parameter whose description to return
	 * \return the descriptor for the parameter
	 */
	const ComponentDescriptor& componentDescriptor(const QString& parameter) const;

	/**
	 * \brief Returns the descriptor for the given subgroup
	 *
	 * This throws an exception if the subgroup is not described or if it
	 * is not a subgroup.
	 * \param subgroup the subgroup whose description to return
	 * \return the descriptor for the subgroup
	 */
	const SubgroupDescriptor& subgroupDescriptor(const QString& subgroup) const;

protected:
	AbstractDescriptorContainer();
	virtual ~AbstractDescriptorContainer();

private:
	// These functions must provide access to the object with type description and the path
	virtual ConfigurationManager* typeDescr() = 0;
	virtual const ConfigurationManager* typeDescr() const = 0;
	virtual const QString& path() const = 0;

	// Creates (if needed) and returns a descriptor for a parameter (not a
	// subgroup). T is the type of the descriptor to create. Descriptors are
	// added to a list and deleted when this class is destroyed. If a
	// descriptor for the parameter/group has already been created, that
	// descriptor is returned instead of creating a new one.
	template <class T>
	T& createParameterDescriptor(const QString& paramName)
	{
		return createDescriptor<T>(pathForParameter(paramName));
	}

	// Creates (if needed) and returns a descriptor for a subgroup (not a
	// parameter). Descriptors are added to a list and deleted when this
	// class is destroyed. If a descriptor for the parameter/group has
	// already been created, that descriptor is returned instead of creating
	// a new one.
	SubgroupDescriptor& createSubgroupDescriptor(const QString& groupName)
	{
		return createDescriptor<SubgroupDescriptor>(pathForSubgroup(groupName));
	}

	// Returns the descriptor for the parameter cast to the given type (if possible, otherwise an exception is thrown)
	template <class T>
	const T& typedParameterDescriptor(const QString& paramName) const
	{
		return typedDescriptor<T>(pathForParameter(paramName));
	}

	// Returns the descriptor for the subgroup (if possible, otherwise an exception is thrown)
	const SubgroupDescriptor& typedSubgroupDescriptor(const QString& groupName) const
	{
		return typedDescriptor<SubgroupDescriptor>(pathForSubgroup(groupName));
	}

	// These functions generate the relative path for parameters and subgroups (relative to path())
	QString relPathForParameter(const QString& parameter) const;
	QString relPathForSubgroup(const QString& subgroup) const;

	// These functions generate the full path for parameters and subgroups
	QString pathForParameter(const QString& parameter) const;
	QString pathForSubgroup(const QString& subgroup) const;

	// Creates (if needed) and returns a descriptor. fullPath is the full
	// path of the descriptor
	template <class T>
	T& createDescriptor(const QString& fullPath)
	{
		QMutexLocker locker(&m_mutex);

		T* ret = NULL;
		if (m_descriptors.contains(fullPath)) {
			AbstractDescriptor* const d = m_descriptors.value(fullPath);
			ret = dynamic_cast<T*>(d);

			if (ret == NULL) {
				throw WrongParameterTypeException(
					fullPath.toLatin1().data(),
					descriptorType2String(type2DescriptorType<T>()).toLatin1().data(),
					descriptorType2String(d->type()).toLatin1().data());
			}
		} else {
			// We need this for exception safety. We cannot use unique_ptr because we would need to make
			// it friend of AbstractDescriptor to make it able to destroy the object
			try {
				ret = new T(typeDescr(), fullPath);
				m_descriptors.insert(fullPath, ret);
			} catch (...) {
				delete ret;
				throw;
			}
		}

		return *ret;
	}

	// Returns the descriptor for path cast to the given type (if possible, otherwise an exception is thrown)
	template <class T>
	const T& typedDescriptor(const QString& fullPath) const
	{
		const AbstractDescriptor* const d = descriptor(fullPath);
		const T* const typedDescr = dynamic_cast<const T*>(d);

		if (typedDescr == NULL) {
			throw WrongParameterTypeException(
				fullPath.toLatin1().data(),
				descriptorType2String(type2DescriptorType<T>()).toLatin1().data(),
				descriptorType2String(descriptorTypeFromFullPath(fullPath)).toLatin1().data());
		}

		return *typedDescr;
	}

	// Returns a descriptor if it exists
	const AbstractDescriptor* descriptor(const QString& path) const;

	// Returns the type of the descriptor if it exists or UnknownDescriptorType if
	// it doesn't
	DescriptorType descriptorTypeFromFullPath(const QString& path) const;

	QMap<QString, AbstractDescriptor*> m_descriptors;
	// This is mutable because we use it also in const functions
	mutable QMutex m_mutex;

	// These are here to disable them
	AbstractDescriptorContainer(const AbstractDescriptorContainer& other);
	AbstractDescriptorContainer& operator=(const AbstractDescriptorContainer& other);
};

/**
 * \brief Utility class to customize the description of a String parameters
 */
class SALSA_CONF_API StringDescriptor : public AbstractDescriptor
{
private:
	StringDescriptor(ConfigurationManager* descr, QString paramPath);
	virtual ~StringDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the default value to use when the parameter is not
	 *        present among configuration parameters
	 *
	 * \param defaultValue the default value of the parameter
	 */
	StringDescriptor& def(QString defaultValue);

	/**
	 * \brief Returns true if the parameter has a default value
	 *
	 * \return true if the parameter has a default value
	 */
	bool hasDefault() const;

	/**
	 * \brief Returns the default value for this parameter
	 *
	 * \return the default value for this parameter
	 */
	QString def() const;

	/**
	 * \brief Sets the Properties of this parameter (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this parameter
	 */
	StringDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	StringDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	// These are private to disable them
	StringDescriptor(const StringDescriptor& other);
	StringDescriptor& operator=(const StringDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to customize the description of an Integer parameter
 */
class SALSA_CONF_API IntDescriptor : public AbstractDescriptor
{
private:
	IntDescriptor(ConfigurationManager* descr, QString paramPath);
	virtual ~IntDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the default value to use when the parameter is not
	 *        present among configuration parameters
	 *
	 * \param defaultValue the default value of the parameter
	 */
	IntDescriptor& def(int defaultValue);

	/**
	 * \brief Returns true if the parameter has a default value
	 *
	 * \return true if the parameter has a default value
	 */
	bool hasDefault() const;

	/**
	 * \brief Returns the default value for this parameter
	 *
	 * \return the default value for this parameter
	 */
	int def() const;

	/**
	 * \brief Sets the Properties of this parameter (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this parameter
	 */
	IntDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Sets the lower and the upper bounds for the parameter's value
	 *
	 * If you don't want to limit the lowerBound, specify MinInteger, while
	 * if you don't want to limit the upperBound, specify MaxInteger.
	 * \param lowerBound the minimum number allowed for the parameter
	 * \param upperBound the maximum number allowed for the parameter
	 */
	IntDescriptor& limits(int lowerBound, int upperBound);

	/**
	 * \brief Returns the lower bound of the parameter
	 *
	 * This returns MinInteger if the parameter has no lower bound
	 * \return the lower bound of the parameter
	 */
	int lowerBound() const;

	/**
	 * \brief Returns the upper bound of the parameter
	 *
	 * This returns MaxInteger if the parameter has no upper bound
	 * \return the upper bound of the parameter
	 */
	int upperBound() const;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	IntDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	// These are private to disable them
	IntDescriptor(const IntDescriptor& other);
	IntDescriptor& operator=(const IntDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to customize the description of a Real parameter
 */
class SALSA_CONF_API RealDescriptor : public AbstractDescriptor
{
private:
	RealDescriptor(ConfigurationManager* descr, QString paramPath);
	virtual ~RealDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the default value to use when the parameter is not
	 *        present among configuration parameters
	 *
	 * \param defaultValue the default value of the parameter
	 */
	RealDescriptor& def(double defaultValue);

	/**
	 * \brief Returns true if the parameter has a default value
	 *
	 * \return true if the parameter has a default value
	 */
	bool hasDefault() const;

	/**
	 * \brief Returns the default value for this parameter
	 *
	 * \return the default value for this parameter
	 */
	double def() const;

	/**
	 * \brief Sets the Properties of this parameter (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this parameter
	 */
	RealDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Set the lower and the upper bounds for the parameter's value
	 *
	 * If you don't want to limit the lowerBound, specify -Infinity, while
	 * if you don't want to limit the upperBound, specify +Infinity.
	 * \param lowerBound the minimum number allowed for the parameter
	 * \param upperBound the maximum number allowed for the parameter
	 */
	RealDescriptor& limits(double lowerBound, double upperBound);

	/**
	 * \brief Returns the lower bound of the parameter
	 *
	 * This returns -Infinity if the parameter has no lower bound
	 * \return the lower bound of the parameter
	 */
	double lowerBound() const;

	/**
	 * \brief Returns the upper bound of the parameter
	 *
	 * This returns +Infinity if the parameter has no upper bound
	 * \return the upper bound of the parameter
	 */
	double upperBound() const;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	RealDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	// These are private to disable them
	RealDescriptor(const RealDescriptor& other);
	RealDescriptor& operator=(const RealDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to customize the description of a Bool parameter
 */
class SALSA_CONF_API BoolDescriptor : public AbstractDescriptor
{
private:
	BoolDescriptor(ConfigurationManager* descr, QString paramPath);
	virtual ~BoolDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the default value to use when the parameter is not
	 *        present among configuration parameters
	 *
	 * \param defaultValue the default value of the parameter
	 */
	BoolDescriptor& def(bool defaultValue);

	/**
	 * \brief Returns true if the parameter has a default value
	 *
	 * \return true if the parameter has a default value
	 */
	bool hasDefault() const;

	/**
	 * \brief Returns the default value for this parameter
	 *
	 * \return the default value for this parameter
	 */
	bool def() const;

	/**
	 * \brief Sets the Properties of this parameter (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this parameter
	 */
	BoolDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	BoolDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	// These are private to disable them
	BoolDescriptor(const BoolDescriptor& other);
	BoolDescriptor& operator=(const BoolDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to customize the description of an Enum-like parameter
 */
class SALSA_CONF_API EnumDescriptor : public AbstractDescriptor
{
private:
	EnumDescriptor(ConfigurationManager* descr, QString paramPath);
	virtual ~EnumDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the default value to use when the parameter is not
	 *        present among configuration parameters
	 *
	 * \param defaultValue the default value of the parameter
	 */
	EnumDescriptor& def(QString defaultValue);

	/**
	 * \brief Returns true if the parameter has a default value
	 *
	 * \return true if the parameter has a default value
	 */
	bool hasDefault() const;

	/**
	 * \brief Returns the default value for this parameter
	 *
	 * \return the default value for this parameter
	 */
	QString def() const;

	/**
	 * \brief Sets the Properties of this parameter (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this parameter
	 */
	EnumDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Sets all the possible values acceptable for this parameter
	 *
	 * \param allValues the list of all values acceptable for this parameter
	 */
	EnumDescriptor& values(QStringList allValues);

	/**
	 * \brief Returns the possible values acceptable for this parameter
	 *
	 * \return the list of possible values acceptable for this parameter
	 */
	QStringList values() const;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	EnumDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	// These are private to disable them
	EnumDescriptor(const EnumDescriptor& other);
	EnumDescriptor& operator=(const EnumDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to customize the description of a Component parameter
 */
class SALSA_CONF_API ComponentDescriptor : public AbstractDescriptor
{
private:
	ComponentDescriptor(ConfigurationManager* descr, QString paramPath);
	virtual ~ComponentDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the Properties of this parameter (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this parameter
	 */
	ComponentDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Sets the the type of the Component
	 *
	 * Subclasses of typeName are allowed
	 * \param typeName the name of the type of this Component
	 */
	ComponentDescriptor& componentType(QString typeName);

	/**
	 * \brief Returns the the type of the Component
	 *
	 * If no component type has been set, the empty string is returned
	 * \return the name of the type of this Component
	 */
	QString componentType() const;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	ComponentDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	// These are private to disable them
	ComponentDescriptor(const ComponentDescriptor& other);
	ComponentDescriptor& operator=(const ComponentDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to customize the description of an subgroup
 */
class SALSA_CONF_API SubgroupDescriptor : public AbstractDescriptor, public AbstractDescriptorContainer
{
private:
	SubgroupDescriptor(ConfigurationManager* descr, QString subgroupPath);
	virtual ~SubgroupDescriptor();

public:
	/**
	 * \brief Returns the type of the descriptor
	 *
	 * \return the type of the descriptor
	 */
	virtual DescriptorType type() const;

	/**
	 * \brief Sets the Properties of this subgroup (see Property
	 *        documentation)
	 *
	 * \param properties the properties to set for this subgroup
	 */
	SubgroupDescriptor& props(ParameterProperties properties);

	using AbstractDescriptor::props;

	/**
	 * \brief Set the the type of the component corresponding to this
	 *        subgroup
	 *
	 * Subclasses of typeName are allowed. If this function is not called
	 * the subgroup is simply a container for parameters of the defining
	 * component
	 * \param typeName the name of the type of the component corresponding
	 *                 to this subgroup
	 */
	SubgroupDescriptor& componentType(QString typeName);

	/**
	 * \brief Returns the the type of the Component
	 *
	 * If no component type has been set, the empty string is returned
	 * \return the name of the type of this Component
	 */
	QString componentType() const;

	/**
	 * \brief Adds a short and a complete help comment
	 *
	 * The short help is typically displayed in a tooltip help, while the
	 * complete help comment will be shown in the help section for the type
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	SubgroupDescriptor& help(QString shortHelp, QString longHelp = QString());

private:
	virtual ConfigurationManager* typeDescr();
	virtual const ConfigurationManager* typeDescr() const;
	virtual const QString& path() const;

	// These are private to disable them
	SubgroupDescriptor(const SubgroupDescriptor& other);
	SubgroupDescriptor& operator=(const SubgroupDescriptor& other);

	// This is friend to be able to create and destroy instances of this
	// class
	friend class AbstractDescriptorContainer;
};

/**
 * \brief Utility class to describe the parameters of a Component
 *
 * This class is used to describe the parameters and subgroups of a registered
 * component. It is not possible to create instances of this class directly,
 * it is only possible to get references from the TypesDB singleton. References
 * are not constant only in the Component::describe() function, so that
 * parameters and subgroups can be defined, as in the following example:
 *
 * \code
 * void MyComponent::describe(RegisteredComponentDescriptor& d)
 * {
 * 	// Always call describe of parent method!!!
 * 	ParentComponent::describe(d);
 *
 * 	d.help("MyComponent", "Bla bla...");
 * 	d.describeString("p1").def("default1").help("Bla bla", "More bla...");
 * 	...
 * }
 * \endcode
 *
 * To get a const reference to an instance of this class (to obtain information
 * about a registered component) use the TypesDB::typeDescription(QString)
 * function, passing the name of the type.
 */
class SALSA_CONF_API RegisteredComponentDescriptor : public AbstractDescriptorContainer
{
private:
	RegisteredComponentDescriptor(ConfigurationManager* descr, QString type);
	virtual ~RegisteredComponentDescriptor();

public:
	/**
	 * \brief Returns the name of the described type
	 *
	 * \return the name of the described type
	 */
	const QString& type() const;

	/**
	 * \brief Sets the help strings for the described type
	 *
	 * \param shortHelp the short comment (rich text here is not allowed)
	 * \param longHelp the long help (rich text here is allowed); if not
	 *                 specified the longHelp will be the same of the
	 *                 shortHelp
	 */
	void help(QString shortHelp, QString longHelp = QString());

	/**
	 * \brief Returns the short help string
	 *
	 * \return the short help string
	 */
	QString shortHelp() const;

	/**
	 * \brief Returns the long help string
	 *
	 * \return the long help string
	 */
	QString longHelp() const;

private:
	virtual ConfigurationManager* typeDescr();
	virtual const ConfigurationManager* typeDescr() const;
	virtual const QString& path() const;

	ConfigurationManager* const m_descr;
	const QString m_type;

	// These are here to disable them
	RegisteredComponentDescriptor(const RegisteredComponentDescriptor& other);
	RegisteredComponentDescriptor& operator=(const RegisteredComponentDescriptor& other);

	// These are friends to be able to create an instances of this
	friend class TypesDB;
	friend class ::ComponentDescription_Test;
	friend class ::ConfigurationHelper_Test;
};

} // end namespace salsa

#endif
