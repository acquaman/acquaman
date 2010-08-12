#include <QtTest/QtTest>

#include "AMSettings.h"
#include "dataman/AMXASScan.h"
#include "dataman/SGMLegacyFileImporter.h"

#include "ui/AMScanView.h"
#include "ui/AMSidebar.h"
#include "ui/AMComboBox.h"
#include "ui/AMNewRunDialog.h"
#include "dataman/AMRun.h"

#include "dataman/AMFirstTimeController.h"
#include "AMErrorMonitor.h"

#include <QTreeView>

class Test1: public QObject
{
	Q_OBJECT
private slots:



	/// This runs before any of the private slots (test cases) get run. It loads the settings and prepares the database tables as required for each Scan object that gets tested.
	void initTestCase()
	{
		AMErrorMon::enableDebugNotifications();
		AMSettings::load();
		AMUserSettings::load();
		AMFirstTimeController();

	}

	/// This is run after all tests are complete.
	void cleanupTestCase()
	{
		AMDatabase::releaseUserDb();
	}

	/// Test to confirm that new run dialog works.
	void testRunDialog() {


		AMComboBox *window = new AMComboBox;
		window->show();



		QTest::qWait(200000);


	}


	void testAMSidebar() {

		AMSidebar b;
		b.resize(200, 500);
		b.show();

		b.addLink("cat1", QVariant(), "Link1 Heavy", QIcon(), -0.1);
		QTest::qWait(1000);
		b.addLink("cat1", QVariant(), "Link 2 Heavier", QIcon());
		QTest::qWait(1000);
		QStandardItem* linky = b.addLink("cat1", QVariant(QString("myPayload")), "Link3 light toDel", QIcon(), -3);
		QTest::qWait(1000);
		b.addLink("cat1", QVariant(), "Link4 mid", QIcon(), -2);

		QTest::qWait(1000);

		b.addHeading("cat2 - light", -1);

		QTest::qWait(1000);

		b.addHeading("cat 3");

		QTest::qWait(1000);

		b.deleteLink(linky);

		QTest::qWait(40000);

	}
	/*

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


		QTest::qWait(4000);


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



	void testAMScanSetModel_MultiChannels() {

		AMScanView sv;
		sv.show();
		sv.changeViewMode(AMScanView::MultiChannels);

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
	void testAMScanSetModel_longTest() {

		AMScanView sv;
		sv.show();
		sv.changeViewMode(AMScanView::MultiChannels);

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

		AMXASScan s2;
		s2.setName("scan 2");
		model->addScan(&s2);
		s1Loader = s2;

		s1Loader.loadFromFile(fileName);

		QTest::qWait(80000);

		QVERIFY(model->removeScan(&s1));

		QTest::qWait(80000);

	}
	*/


protected:
};





QTEST_MAIN(Test1)
#include "tests.moc"
