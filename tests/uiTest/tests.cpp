#include <QtTest/QtTest>

#include "AMSettings.h"
#include "dataman/AMXASScan.h"
#include "dataman/SGMLegacyFileImporter.h"

#include "ui/AMScanView.h"

#include <QTreeView>

class Test1: public QObject
{
	Q_OBJECT
private slots:



	/// This runs before any of the private slots (test cases) get run. It loads the settings and prepares the database tables as required for each Scan object that gets tested.
	void initTestCase()
	{
		AMSettings::load();
		AMUserSettings::load();
		AMScan::dbPrepareTables(AMDatabase::userdb());

	}

	/// This is run after all tests are complete.
	void cleanupTestCase()
	{
		AMDatabase::releaseUserDb();
	}


	// tests creation/deletion of scans and channels within an AMScanSetModel (using a standard QTreeView to watch)
	void testAMScanSetModel() {



		AMScanView sv;
		sv.show();

		AMScanSetModel* model = sv.model();

		QTreeView tree;
		tree.show();
		tree.setModel(model);





		QTest::qWait(2000);

		AMXASScan s1;
		s1.setName("scan1");
		model->addScan(&s1);


		QTest::qWait(2000);


		SGMLegacyFileImporter s1Loader(&s1);
		QString fileName = AMUserSettings::userDataFolder + "001.dat";

		QVERIFY(s1Loader.loadFromFile(fileName));


		QTest::qWait(2000);


		QVERIFY(s1.deleteChannel("tey_n"));


		QTest::qWait(2000);


		QVERIFY(s1.addChannel("const5", "1+2+2"));


		QTest::qWait(4000);

		AMXASScan s2;
		s2.setName("scan 2!");
		model->addScan(&s2);
		s1Loader = s2;

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(40000);

		QVERIFY(s1.deleteChannel("const5"));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}


protected:
};

QTEST_MAIN(Test1)
#include "tests.moc"
