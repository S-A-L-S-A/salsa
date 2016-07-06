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

#include <QtTest/QtTest>
#include <QTemporaryFile>
#include "parametersfileloadersaver.h"
#include "configurationmanager.h"

// NOTES AND TODOS
//
//

using namespace farsa;

/**
 * \brief The class to perform unit tests
 *
 * Each private slot is a test
 */
class ParametersFileLoaderSaver_Test : public QObject
{
	Q_OBJECT

	class DummyLoaderSaver : public ParametersFileLoaderSaver
	{
	public:
		DummyLoaderSaver()
			: m_numLoadCalls(0)
			, m_numSaveCalls(0)
		{
		}

		int getNumLoadCalls() const
		{
			return m_numLoadCalls;
		}

		int getNumSaveCalls() const
		{
			return m_numSaveCalls;
		}

		void resetCounters()
		{
			m_numLoadCalls = 0;
			m_numSaveCalls = 0;
		}

	private:
		virtual bool loadParameters(QTextStream& /*stream*/, ConfigurationManager& /*configParams*/)
		{
			++m_numLoadCalls;

			return true;
		}

		virtual bool saveParameters(QTextStream& /*stream*/, const ConfigurationManager& /*configParams*/)
		{
			++m_numSaveCalls;

			return true;
		}

		int m_numLoadCalls;
		int m_numSaveCalls;
	};

public:
	ParametersFileLoaderSaver_Test()
		: m_loaderSaver(new DummyLoaderSaver)
	{
	}

private slots:
	// Before performing any test, we need to register the loader/saver
	void initTestCase()
	{
		QVERIFY(ConfigurationManager::registerFileFormat("DUMMY", m_loaderSaver, "dummy"));
	}

	void loadFile()
	{
		// Here we only check that the test class function is called

		QTemporaryFile tmpFile("XXXXXX.dummy");
		QVERIFY(tmpFile.open());

		m_loaderSaver->resetCounters();

		ConfigurationManager manager;

		manager.createGroup("test");
		QVERIFY(manager.loadParameters(tmpFile.fileName(), false));
		QVERIFY(!manager.groupExists("test"));
		QCOMPARE(m_loaderSaver->getNumLoadCalls(), 1);

		manager.createGroup("test");
		QVERIFY(manager.loadParameters(tmpFile.fileName(), true));
		QVERIFY(manager.groupExists("test"));
		QCOMPARE(m_loaderSaver->getNumLoadCalls(), 2);
	}

	void saveFile()
	{
		// Here we only check that the test class function is called

		QTemporaryFile tmpFile("XXXXXX.dummy");
		QVERIFY(tmpFile.open());

		m_loaderSaver->resetCounters();

		// Here we only check that the test class function is called
		ConfigurationManager manager;

		QVERIFY(manager.saveParameters(tmpFile.fileName()));
		QCOMPARE(m_loaderSaver->getNumSaveCalls(), 1);
	}

private:
	// This instance is registered, so ConfigurationManager destroys it
	DummyLoaderSaver* m_loaderSaver;
};

QTEST_MAIN(ParametersFileLoaderSaver_Test)
#include "parametersfileloadersaver_test.moc"
