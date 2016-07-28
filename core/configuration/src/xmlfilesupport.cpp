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

#include "private/xmlfilesupport.h"
#include "configurationmanager.h"
#include <QtDebug>
#include <QFile>
#include <QDomElement>

namespace salsa {

XMLFileLoaderSaver::XMLFileLoaderSaver() :
	ParametersFileLoaderSaver()
{
}

XMLFileLoaderSaver::~XMLFileLoaderSaver()
{
}

bool XMLFileLoaderSaver::saveParameters(QTextStream &stream, const ConfigurationManager& configParams)
{
	//--- create the XML Document
	QDomDocument xmldoc;
	//--- create the root node
	QDomElement root = xmldoc.createElement( "configurationparameters" );
	root.setAttribute( "version", "1.0" );
	xmldoc.appendChild( root );
	//--- recursively fill up xmldoc starting from root
	writeGroupToXMLDoc( xmldoc, root, "", configParams );
	//--- write the xmldoc to the stream
	//    because the method toString does not allow indentation with tab
	//    a substitution of all spaces inserted with tab is done here
	QString xmlstr = xmldoc.toString( 4 );
	stream << xmlstr.replace( "    ", "\t" );
	return true;
}

void XMLFileLoaderSaver::writeGroupToXMLDoc( QDomDocument& xmldoc, QDomNode& xmlnode, QString groupPath, const ConfigurationManager& configParams ) {
	//--- write parameters first
	QStringList paramList = configParams.getParametersList( groupPath );
	foreach( QString param, paramList ) {
		QString value = configParams.getValue( groupPath + GroupSeparator + param );
		if ( ! value.isEmpty() ) {
			QDomElement xmlparam = xmldoc.createElement( "param" );
			xmlparam.setAttribute( "name", param );
			xmlparam.appendChild( xmldoc.createTextNode( value ) );
			xmlnode.appendChild( xmlparam );
		}
	}
	//--- and then all subgroups recursively
	QStringList groupList = configParams.getGroupsList( groupPath );
	foreach( QString group, groupList ) {
		//--- it create the xmlnode representing the group and call recursively writeGroupToXMLDoc
		QDomElement xmlgroup = xmldoc.createElement( "group" );
		xmlgroup.setAttribute( "name", group );
		xmlnode.appendChild( xmlgroup );
		writeGroupToXMLDoc( xmldoc, xmlgroup, groupPath + GroupSeparator + group, configParams );
	}
	return;
}

bool XMLFileLoaderSaver::loadParameters(QTextStream &stream, ConfigurationManager& configParams)
{
	//--- create the XML Document
	QDomDocument xmldoc;
	QString parseError;
	int line, col;
	if ( !xmldoc.setContent( stream.device(), &parseError, &line, &col ) ) {
		qWarning() << line << ":" << col << "Error while parsing XML file:" << parseError;
		return false;
	}
	QDomElement root = xmldoc.documentElement().toElement();
	if ( root.tagName() != "configurationparameters" ) {
		qWarning() << "The root node should be configurationparameters. Parsing this file could generate unexcepted results";
	}
	if ( root.attribute( "version" ) != "1.0" ) {
		qWarning() << "Only version '1.0' of configurationparameters XML syntax is supported. Parsing this file could generate unexcepted results";
	}
	loadGroupFromXMLDoc( root, "", configParams );
	return true;
}

void XMLFileLoaderSaver::loadGroupFromXMLDoc( const QDomNode& xmlnode, QString groupPath, ConfigurationManager& configParams ) {
	//--- traverse all the child of the current node
	QDomNode node = xmlnode.firstChild();
	while( !node.isNull() ) {
		//--- check that it's an Element
		QDomElement e = node.toElement();
		if ( e.isNull() ) {
			//--- skip this tag
			node = node.nextSibling();
			continue;
		}
		//--- check the tagname
		if ( e.tagName() == "param" ) {
			//--- add the parameter to configParams to the current groupPath
			QString name = e.attribute( "name" );
			if ( name.isEmpty() ) throw XMLFileMandatoryAttributeMissing( "Tag <param>: attribute 'name' is mandatory" );
			configParams.createParameter( groupPath, name );
			configParams.setValue( groupPath + GroupSeparator + name, e.text().simplified() );
		} else if ( e.tagName() == "group" ) {
			//--- add the group and call loadGroupFromXMLDoc recursevily
			QString name = e.attribute( "name" );
			if ( name.isEmpty() ) throw XMLFileMandatoryAttributeMissing( "Tag <group>: attribute 'name' is mandatory" );
			QString newgroup = configParams.createSubGroup( groupPath, name );
			loadGroupFromXMLDoc( node, newgroup, configParams );
		}
		node = node.nextSibling();
	}
}

} // end namespace salsa
