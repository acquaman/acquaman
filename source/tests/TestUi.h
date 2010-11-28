#include <QtTest/QtTest>

#include "AMSettings.h"
#include "dataman/AMXASScan.h"
#include "dataman/SGM2004FileLoader.h"

#include "ui/AMScanView.h"
#include "ui/AMSidebar.h"
#include "ui/AMStartScreen.h"
#include "ui/AMNewRunDialog.h"
#include "dataman/AMRun.h"
#include "ui/AMThumbnailScrollViewer.h"

#include "dataman/AMFirstTimeController.h"
#include "AMErrorMonitor.h"

#include "analysis/AM1DExpressionAB.h"
#include <QTreeView>

class TestUi: public QObject
{
	Q_OBJECT
private slots:


	/// This runs before any of the private slots (test cases) get run. It loads the settings and prepares the database tables as required for each Scan object that gets tested.
	void initTestCase()
	{

	}


	/// This is run after all tests are complete.
	void cleanupTestCase()
	{
	}


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


		SGM2004FileLoader s1Loader(&s1);
		QString fileName = AMUserSettings::userDataFolder + "/2010/06/CarbonTape_C1.dat";
		qDebug() << fileName;

		QVERIFY(s1Loader.loadFromFile(fileName));


		QTest::qWait(4000);


		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("tey_n")));


		QTest::qWait(2000);


		AM1DExpressionAB* channel = new AM1DExpressionAB("const5");
		channel->setExpression("1+2+2");
		s1.addAnalyzedDataSource(channel);
		QVERIFY(s1.indexOfDataSource("const5") != -1);


		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tfy_n");

		QTest::qWait(4000);

		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("tfy_n")));

		QTest::qWait(1000);

		AMXASScan s2;
		s2.setName("scan 2");
		model->addScan(&s2);
		//s1Loader = s2;
		s1Loader.setTarget(&s2);

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveDataSourceByName("const5");

		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteDataSourceAt(s2.indexOfDataSource("tey_raw")));




		QTest::qWait(8000);

		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("const5")));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}






	void testThumbnailGraphicsWidget() {
		QGraphicsView* view = new QGraphicsView();
		view->resize(300,300);
		view->show();

		QGraphicsScene* scene = new QGraphicsScene(view);
		view->setScene(scene);

		AMThumbnailScrollGraphicsWidget* g = new AMThumbnailScrollGraphicsWidget(0);
		g->setSource(AMDatabase::userdb(), 1, 4);
		scene->addItem(g);
		g->setVisible(true);
		g->setGeometry(QRectF(0,0,240,180));
		g->setWidth(240);
		view->setSceneRect(0,0,400,400);
		qDebug() << "widget is active:" << g->isActive();
		qDebug() << "scene is active:" << scene->isActive();
		qDebug() << "view is active" << view->isActiveWindow();
		qDebug() << "accepts hover events?" << g->acceptsHoverEvents();

		qDebug() << "accepts hover events?" << g->acceptHoverEvents();

		qDebug() << "geometry" << g->geometry();
		qDebug() << "bounding rect" << g->boundingRect();
		qDebug() << "shape" << g->shape();


		//scene->setFocus(Qt::MouseFocusReason);
		//view->setMouseTracking(true);

		//g->grabMouse();

		QTest::qWait(2000);

		delete view;

	}



	/// Test to confirm that new run dialog works.
	void testRunDialog() {


		AMStartScreen *ss;
		ss->show();



		QTest::qWait(2000);

		delete ss;

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


		SGM2004FileLoader s1Loader(&s1);
		QString fileName = AMUserSettings::userDataFolder + "001.dat";

		QVERIFY(s1Loader.loadFromFile(fileName));


		QTest::qWait(2000);


		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("tey_n")));


		QTest::qWait(2000);


		AM1DExpressionAB* const5 = new AM1DExpressionAB("const5");
		const5->setExpression("1+2+2");
		QVERIFY(const5->isValid());
		s1.addAnalyzedDataSource(const5);
		QVERIFY(s1.indexOfDataSource("const5") != -1);


		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tfy_n");

		QTest::qWait(4000);

		AMXASScan s2;
		s2.setName("scan2");
		model->addScan(&s2);
		s1Loader.setTarget(&s2);

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveDataSourceByName("const5");

		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteDataSourceAt(s2.indexOfDataSource("tey_raw")));




		QTest::qWait(8000);

		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("const5")));


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


		SGM2004FileLoader s1Loader(&s1);
		QString fileName = AMUserSettings::userDataFolder + "001.dat";

		QVERIFY(s1Loader.loadFromFile(fileName));


		QTest::qWait(2000);


		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("tey_n")));


		QTest::qWait(2000);


		AM1DExpressionAB* const5 = new AM1DExpressionAB("const5");
		const5->setExpression("1+2+2");
		QVERIFY(const5->isValid());
		s1.addAnalyzedDataSource(const5);
		QVERIFY(s1.indexOfDataSource("const5") != -1);


		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tfy_n");

		QTest::qWait(4000);

		AMXASScan s2;
		s2.setName("scan2");
		model->addScan(&s2);
		s1Loader.setTarget(&s2);

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveDataSourceByName("const5");

		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteDataSourceAt(s2.indexOfDataSource("tey_raw")));




		QTest::qWait(8000);

		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("const5")));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}



	void testAMScanSetModel_MultiChannels() {

		AMScanView sv;
		sv.show();
		sv.changeViewMode(AMScanView::MultiSources);

		AMScanSetModel* model = sv.model();

		QTreeView tree;
		tree.show();
		tree.setModel(model);





		QTest::qWait(2000);

		AMXASScan s1;
		s1.setName("scan1");
		model->addScan(&s1);


		QTest::qWait(2000);


		SGM2004FileLoader s1Loader(&s1);
		QString fileName = AMUserSettings::userDataFolder + "001.dat";

		QVERIFY(s1Loader.loadFromFile(fileName));


		QTest::qWait(2000);


		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("tey_n")));


		QTest::qWait(2000);


		AM1DExpressionAB* const5 = new AM1DExpressionAB("const5");
		const5->setExpression("1+2+2");
		QVERIFY(const5->isValid());
		s1.addAnalyzedDataSource(const5);
		QVERIFY(s1.indexOfDataSource("const5") != -1);


		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tfy_n");

		QTest::qWait(4000);

		AMXASScan s2;
		s2.setName("scan2");
		model->addScan(&s2);
		s1Loader.setTarget(&s2);

		QVERIFY(s1Loader.loadFromFile(fileName));

		QTest::qWait(2000);

		model->setExclusiveDataSourceByName("const5");

		QTest::qWait(4000);

		model->setExclusiveDataSourceByName("tey_n");

		QTest::qWait(4000);

		QVERIFY(s2.deleteDataSourceAt(s2.indexOfDataSource("tey_raw")));




		QTest::qWait(8000);

		QVERIFY(s1.deleteDataSourceAt(s1.indexOfDataSource("const5")));


		QTest::qWait(2000);


		QVERIFY(model->removeScan(&s1));


		QTest::qWait(2000);

	}


	// tests creation/deletion of scans and channels within an AMScanSetModel (using a standard QTreeView to watch)
	void testAMScanSetModel_longTest() {

		AMScanView sv;
		sv.show();
		sv.changeViewMode(AMScanView::MultiSources);

		AMScanSetModel* model = sv.model();

		QTreeView tree;
		tree.show();
		tree.setModel(model);

		QTest::qWait(2000);

		AMXASScan s1;
		s1.setName("scan1");
		model->addScan(&s1);

		QTest::qWait(2000);


		SGM2004FileLoader s1Loader(&s1);
		QString fileName = AMUserSettings::userDataFolder + "001.dat";

		QVERIFY(s1Loader.loadFromFile(fileName));


		QTest::qWait(2000);

		AMXASScan s2;
		s2.setName("scan 2");
		model->addScan(&s2);
		s1Loader.setTarget(&s2);

		s1Loader.loadFromFile(fileName);

		QTest::qWait(80000);

		QVERIFY(model->removeScan(&s1));

		QTest::qWait(80000);

	}




protected:
};

