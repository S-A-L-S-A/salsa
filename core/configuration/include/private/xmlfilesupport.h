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

#ifndef XML_FILE_SUPPORT_H
#define XML_FILE_SUPPORT_H

#include "configurationconfig.h"
#include "parametersfileloadersaver.h"
#include <QString>
#include <QTextStream>
#include <QDomDocument>
#include <QDomNode>
#include "baseexception.h"

namespace salsa {

/**
 * \brief Exception throw by loadParameters when a mandatory attribute is
 *        missing
 */
class SALSA_CONF_TEMPLATE XMLFileMandatoryAttributeMissing : public BaseException
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param why the description of why the exception was thrown. The
	 *            buffer for this is at most 512 characters (including the
	 *            '\0' terminator)
	 */
	XMLFileMandatoryAttributeMissing(const char* why) throw() :
		BaseException()
	{
		strncpy(m_why, why, 512);
		m_why[511] = '\0';
	}

	/**
	 * \brief Copy constructor
	 *
	 * \param other the exception to copy
	 */
	XMLFileMandatoryAttributeMissing(const XMLFileMandatoryAttributeMissing& other) throw() :
		BaseException(other)
	{
		strncpy(m_why, other.m_why, 512);
		m_why[511] = '\0';
	}

	/**
	 * \brief Copy operator
	 *
	 * \param other the exception to copy
	 */
	XMLFileMandatoryAttributeMissing& operator=(const XMLFileMandatoryAttributeMissing& other) throw()
	{
		if (&other == this) {
			return *this;
		}

		BaseException::operator=(other);
		strncpy(m_why, other.m_why, 512);
		m_why[511] = '\0';

		return *this;
	}

	/**
	 * \brief Destructor
	 */
	virtual ~XMLFileMandatoryAttributeMissing() throw()
	{
	}

	/**
	 * \brief Returns a C string describing the exception
	 *
	 * \return a C string describing the exception
	 */
	virtual const char *what() const throw()
	{
		return m_why;
	}

	/**
	 * \brief Returns the description of why the exception was thrown
	 *
	 * \return the description of why the exception was thrown
	 */
	const char *why() const throw()
	{
		return m_why;
	}

	/**
	 * \brief Adding the default implementation of the clone() and raise()
	 *        functions
	 */
	EXCEPTION_HELPER_FUNCTIONS(XMLFileMandatoryAttributeMissing)

private:
	/**
	 * \brief The description of why the exception was thrown
	 */
	char m_why[512];
};

/**
 * \brief The file loader/saver on XML files
 *
 * \note The root category is not explicitly saved to file
 * \warning This class is experimental and not unit-tested
 *
 * \ingroup configuration_configuration
 */
class SALSA_CONF_API XMLFileLoaderSaver : public ParametersFileLoaderSaver
{
public:
	/**
	 * \brief Constructor
	 */
	XMLFileLoaderSaver();

	/**
	 * \brief Destructor
	 */
	virtual ~XMLFileLoaderSaver();

protected:
	/**
	 * \brief The function actually load configuration from the given stream
	 *
	 * Implement in subclasses
	 * \param stream the stream from which data should be read
	 * \return true if loading was successful, false otherwise
	 */
	virtual bool loadParameters(QTextStream &stream, ConfigurationManager& configParams);

	/**
	 * \brief The function actually save configuration to the given stream
	 *
	 * Implement in subclasses
	 * \param stream the stream to which data should be write
	 * \return true if loading was successful, false otherwise
	 */
	virtual bool saveParameters(QTextStream &stream, const ConfigurationManager& configParams);
private:
	/*! Helper function for recursevily traverse the parameters and write into XML format */
	void writeGroupToXMLDoc( QDomDocument& xmldoc, QDomNode& xmlnode, QString groupPath, const ConfigurationManager& configParams );
	/*! Helper function for recursevily traverse the XML and write into parameters */
	void loadGroupFromXMLDoc( const QDomNode& xmlnode, QString groupPath, ConfigurationManager& configParams );
};

} // end namespace salsa

#endif
