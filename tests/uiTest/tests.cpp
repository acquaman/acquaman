#include <QtTest/QtTest>

#include "AMSettings.h"
#include "dataman/AMXASScan.h"
#include "dataman/SGMLegacyFileImporter.h"

#include "ui/AMScanView.h"
#include "ui/AMSidebar.h"

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


	void testAMSidebar() {

		AMSidebar b;
		b.resize(200, 500);
		b.show();

		b.addLink("cat1", QVariant(), "Link1 Heavy", "", -0.1);
		QTest::qWait(1000);
		b.addLink("cat1", QVariant(), "Link 2 Heavier", "");
		QTest::qWait(1000);
		b.addLink("cat1", QVariant(QString("myPayload")), "Link3 light toDel", "", -3);
		QTest::qWait(1000);
		b.addLink("cat1", QVariant(), "Link4 mid", "", -2);

		QTest::qWait(1000);

		b.addCategory("cat2 - light", -1);

		QTest::qWait(1000);

		b.addCategory("cat 3");

		QTest::qWait(1000);

		b.deleteLink(QVariant(QString("myPayload")));

		QTest::qWait(4000);

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

		model->setExclusiveChannel("tfy_n");

		QTest::qWait(4000);

		QVERIFY(s1.deleteChannel("tfy_n"));

		QTest::qWait(1000);

		AMXASScan s2;
		s2.setName("scan 2");
		model->addScan(&s2);
		s1Loader = s2;

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveChannel("const5");

		QTest::qWait(4000);

		model->setExclusiveChannel("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteChannel("tey_raw"));




		QTest::qWait(8000);

		QVERIFY(s1.deleteChannel("const5"));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}


	// tests creation/deletion of scans and channels within an AMScanSetModel (using a standard QTreeView to watch)
	void testAMScanSetModel_Overplot() {

		AMScanView sv;
		sv.show();
		sv.changeViewMode(AMScanView::OverPlot);

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

		model->setExclusiveChannel("tfy_n");

		QTest::qWait(4000);

		AMXASScan s2;
		s2.setName("scan2");
		model->addScan(&s2);
		s1Loader = s2;

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveChannel("const5");

		QTest::qWait(4000);

		model->setExclusiveChannel("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteChannel("tey_raw"));




		QTest::qWait(8000);

		QVERIFY(s1.deleteChannel("const5"));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}

	// tests creation/deletion of scans and channels within an AMScanSetModel (using a standard QTreeView to watch)
	void testAMScanSetModel_MultiScans() {

		AMScanView sv;
		sv.show();
		sv.changeViewMode(AMScanView::MultiScans);

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

		model->setExclusiveChannel("tfy_n");

		QTest::qWait(4000);

		AMXASScan s2;
		s2.setName("scan2");
		model->addScan(&s2);
		s1Loader = s2;

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveChannel("const5");

		QTest::qWait(4000);

		model->setExclusiveChannel("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteChannel("tey_raw"));




		QTest::qWait(8000);

		QVERIFY(s1.deleteChannel("const5"));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}


protected:
};

QTEST_MAIN(Test1)
#include "tests.moc"
