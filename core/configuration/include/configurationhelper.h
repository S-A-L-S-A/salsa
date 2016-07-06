/***************************************************************************
 *  FARSA Configuration Library                                            *
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

#ifndef CONFIGURATION_HELPER_H
#define CONFIGURATION_HELPER_H

#include "configurationconfig.h"
#include <QVector>

namespace farsa {

class ConfigurationManager;
class AbstractDescriptor;
class AbstractDescriptorContainer;

/**
 * \brief The namespace containing some helper function for working with
 *        configuration parameters
 *
 * \ingroup configuration_helper
 */
namespace ConfigurationHelper {
	/**
	 * \brief Returns the value of parameter as int
	 *
	 * If the parameter has not been set, it will return the default value.
	 * This function can only be used if the parameter is in a group that
	 * has been described and the parameter itself has been described and
	 * has a default value. An exception is thrown otherwise. An exception
	 * is also thrown if the parameter is not of type int or has a value
	 * that cannot be converted to int.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName".
	 * \return the value of the parameter converted to int
	 */
	FARSA_CONF_API int getInt(const ConfigurationManager& params, QString paramPath);

	/**
	 * \brief Returns the value of parameter as int
	 *
	 * If the parameter has not been set, it will return the provided
	 * default value.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \param defaultValue the value to return if the parameter doesn't
	 *                     exists
	 * \return the value of the parameter converted to int
	 */
	FARSA_CONF_API int getInt(const ConfigurationManager& params, QString paramPath, int defaultValue);

	/**
	 * \brief Returns the value of parameter as double
	 *
	 * If the parameter has not been set, it will return the default value.
	 * This function can only be used if the parameter is in a group that
	 * has been described and the parameter itself has been described and
	 * has a default value. An exception is thrown otherwise. An exception
	 * is also thrown if the parameter is not of type double or has a value
	 * that cannot be converted to double.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName".
	 * \return the value of the parameter converted to double
	 */
	FARSA_CONF_API double getReal(const ConfigurationManager& params, QString paramPath);

	/**
	 * \brief Returns the value of parameter as double
	 *
	 * If the parameter has not been set, it will return the provided
	 * default value. If the parameter value is +Inf or -Inf (case
	 * insensitive) it will be converted to +Infinity and -Infinity,
	 * respectively
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \param defaultValue the value to return if the parameter doesn't
	 *                     exists
	 * \return the value of the parameter converted to double
	 */
	FARSA_CONF_API double getReal(const ConfigurationManager& params, QString paramPath, double defaultValue);

	/**
	 * \brief Returns the value of parameter as bool
	 *
	 * If the parameter has not been set, it will return the default value.
	 * This function can only be used if the parameter is in a group that
	 * has been described and the parameter itself has been described and
	 * has a default value. An exception is thrown otherwise. An exception
	 * is also thrown if the parameter is not of type bool or has a value
	 * that cannot be converted to bool.
	 * The strings "true", "t" and "1" are converted to true (regardless of
	 * case), while "false", "f" and "0" are converted to false (regardless
	 * of case. Any other value is not accepted.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \return the value of the parameter converted to bool
	 */
	FARSA_CONF_API bool getBool(const ConfigurationManager& params, QString paramPath);

	/**
	 * \brief Returns the value of parameter as bool
	 *
	 * If the parameter has not been set, it will return the provided
	 * default value.
	 * The strings "true", "t" and "1" are converted to true (regardless of
	 * case), while "false", "f" and "0" are converted to false (regardless
	 * of case. Any other value is not accepted.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \param defaultValue the value to return if the parameter doesn't
	 *                     exists
	 * \return the value of the parameter converted to bool
	 */
	FARSA_CONF_API bool getBool(const ConfigurationManager& params, QString paramPath, bool defaultValue);

	/**
	 * \brief Returns the value of parameter as QString
	 *
	 * If the parameter has not been set, it will return the default value.
	 * This function can only be used if the parameter is in a group that
	 * has been described and the parameter itself has been described and
	 * has a default value. An exception is thrown otherwise. An exception
	 * is also thrown if the parameter is not of type string
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \return the value of the parameter
	 */
	FARSA_CONF_API QString getString(const ConfigurationManager& params, QString paramPath);

	/**
	 * \brief Returns the value of parameter as QString
	 *
	 * If the parameter has not been set, it will return the provided
	 * default value.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \param defaultValue the value to return if the parameter doesn't
	 *                     exists
	 * \return the value of the parameter
	 */
	FARSA_CONF_API QString getString(const ConfigurationManager& params, QString paramPath, QString defaultValue);

	/**
	 * \brief Returns the value of an enum parameter as QString
	 *
	 * If the parameter has not been set, it will return the default value.
	 * This function can only be used if the parameter is in a group that
	 * has been described and the parameter itself has been described and
	 * has a default value. An exception is thrown otherwise. An exception
	 * is also thrown if the parameter is not of type enum
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \return the value of the parameter
	 */
	FARSA_CONF_API QString getEnum(const ConfigurationManager& params, QString paramPath);

	/**
	 * \brief Returns the value of an enum parameter as QString
	 *
	 * If the parameter has not been set, it will return the provided
	 * default value.
	 * \param params the object with configuration parameters
	 * \param paramPath the full path of the parameter. In a typical use of
	 *                  a configuration function you whould use here
	 *                  prefixPath() + "paramName"
	 * \param defaultValue the value to return if the parameter doesn't
	 *                     exists
	 * \return the value of the parameter
	 * \note This is basically the same as getString(), because if the
	 *       parameter has not been declared we cannot know which are the
	 *       valid enum values
	 */
	FARSA_CONF_API QString getEnum(const ConfigurationManager& params, QString paramPath, QString defaultValue);

	/**
	 * \brief Encodes a list of ints as a single string
	 *
	 * The vector of ints is encoded as a space-separated list of numbers
	 * \param list the list to encode
	 * \return the encoded list
	 */
	FARSA_CONF_API QString encodeListOfInts(const QVector<int>& list);

	/**
	 * \brief Decodes a string into a list of ints
	 *
	 * See the description of encodeListOfInts for details. This function
	 * throws an exception in case of errors
	 * \param encodedList the string containing the encoded list of ints
	 * \return the list of ints
	 */
	FARSA_CONF_API QVector<int> decodeListOfInts(const QString& encodedList);

	/**
	 * \brief Encodes a list of reals as a single string
	 *
	 * The vector of reals is encoded as a space-separated list of numbers
	 * \param list the list to encode
	 * \return the encoded list
	 */
	FARSA_CONF_API QString encodeListOfReals(const QVector<double>& list);

	/**
	 * \brief Decodes a string into a list of reals
	 *
	 * See the description of encodeListOfReals for details. This function
	 * throws an exception in case of errors
	 * \param encodedList the string containing the encoded list of reals
	 * \return the list of reals
	 */
	FARSA_CONF_API QVector<double> decodeListOfReals(const QString& encodedList);

	/**
	 * \brief Encodes a list of bools as a single string
	 *
	 * The vector of bools is encoded as a string made up of 1 and 0
	 * (without any separator)
	 * \param list the list to encode
	 * \return the encoded list
	 */
	FARSA_CONF_API QString encodeListOfBools(const QVector<bool>& list);

	/**
	 * \brief Decodes a string into a list of bools
	 *
	 * See the description of encodeListOfBools for details. This function
	 * throws an exception in case of errors
	 * \param encodedList the string containing the encoded list of bools
	 * \return the list of bools
	 */
	FARSA_CONF_API QVector<bool> decodeListOfBools(const QString& encodedList);

	/**
	 * \brief Encodes a list of strings as a single string
	 *
	 * Component strings are separated by "/". If "/" appears inside a
	 * string, it is converted to "\/"
	 * \param list the list to encode
	 * \return the encoded list
	 */
	FARSA_CONF_API QString encodeListOfStrings(const QStringList& list);

	/**
	 * \brief Decodes a string into a list of strings
	 *
	 * See the description of encodeListOfStrings for details
	 * \param encodedList the string containing the encoded list of strings
	 * \return the list of strings
	 */
	FARSA_CONF_API QStringList decodeListOfStrings(const QString& encodedList);

	/**
	 * \brief Helper function to compare a couple of group or property names
	 *
	 * This function is the one that is used internally to check if two
	 * group or property names are the same, as explained in the comment of
	 * the ConfigurationManager class
	 * \param s1 the first string to compare
	 * \param s2 the second string to compare
	 * \return true if s1 is equal to s2
	 */
	FARSA_CONF_API bool configKeysEqual(const QString& s1, const QString& s2);

	/**
	 * \brief Helper function to order a list of group or property names
	 *
	 * This function is the one that is used internally to sort group and
	 * property names, as explained in the comment of the
	 * ConfigurationManager class. You can use it as the comparison function
	 * object for functions like QT's qSort or std::sort.
	 * \param s1 the first string to compare
	 * \param s2 the second string to compare
	 * \return true if s1 is less than (i.e. comes before) s2
	 */
	FARSA_CONF_API bool configKeysLessThan(const QString& s1, const QString& s2);

	/**
	 * \brief A utility function to ease throwing the exception
	 *        UserDefinedCheckFailureException
	 *
	 * This function accepts QStrings instead of const char* to allow easier
	 * formatting of the error messages. This does never return as the
	 * exception is always thrown
	 * \param paramName the name of the parameter on which the check failed
	 * \param paramValue the value of the parameter that caused the check to
	 *                   fail
	 * \param description a description of the error
	 */
	FARSA_CONF_API void throwUserConfigError(QString paramName, QString paramValue, QString description);

	/**
	 * \brief Returns the descriptor for the given parameter
	 *
	 * An exception is thrown if the parameter is not described
	 * \param params the configuration manager object that contains the
	 *               parameter
	 * \param paramPath the full path to the parameter
	 * \return the descriptor for the parameter
	 */
	const AbstractDescriptor& getDescriptorForParameter(const ConfigurationManager& params, QString paramPath);

	/**
	 * \brief Returns the descriptor for the given group
	 *
	 * An exception is thrown if the group is not described
	 * \param params the configuration manager object that contains the
	 *               group
	 * \param groupPath the full path to the group
	 * \return the descriptor for the group
	 */
	const AbstractDescriptorContainer& getDescriptorForGroup(const ConfigurationManager& params, QString groupPath);

	/**
	 * \brief Returns the default for the given descriptor as a string
	 *
	 * If the descriptor type does not provides any default value, an empty
	 * string is returned
	 * \param d the descriptor for which a default is requested
	 * \return the default as a string
	 */
	QString getDefaultForDescriptorAsString(const AbstractDescriptor& d);
}

} // end namespace farsa

#endif
