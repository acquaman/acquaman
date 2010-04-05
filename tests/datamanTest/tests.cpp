#include <QtTest/QtTest>
#include "dataman/AMDatabase.h"
#include "dataman/AMScan.h"
#include "dataman/AMDbLoader.h"
#include "AMSettings.h"

/// This class contains all of the unit tests for the dataman module.
/*! Each private slot corresponds to one test (which can actually contain several individual unit tests.)  The initTestCase() function is run before any of the tests, and the cleanupTestCase is run after all of them finish.
  */
class TestDataman: public QObject
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


	/// Test inserts of DbObjects into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMDbObject_retrieveStatically() {
		AMDbObject dbo;

		// generate unique name, and set properties:
		qDebug() << "Generating DbObject with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestDbObject" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		qDebug() << "Testing DbObject insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject dbo2;
		dbo2.loadFromDb(AMDatabase::userdb(), dbo.id());
		qDebug() << "Retrieving DbObject out of database: comparing all parameters with original:";
		QCOMPARE(dbo2.id(), dbo.id());
		QCOMPARE(dbo2.name(), dbo.name());
		QCOMPARE(dbo2.number(), dbo.number());
		QCOMPARE(dbo2.dateTime().toString("yyyy MM/dd hh:mm:ss"), dbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
	}

	/// Test inserts of DbObjects into database, and confirm all values loaded back with dynamic loader:
	void insertAMDbObject_retrieveDynamically() {
		AMDbObject dbo;

		// generate unique name, and set properties:
		qDebug() << "Generating DbObject with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestDbObject" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		qDebug() << "Testing DbObject insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject* dbo2 = 0;
		QString typeString;
		dbo2 = AMDbLoader::createAndLoad(AMDatabase::userdb(), dbo.id(),this, &typeString);

		qDebug() << "Type of retrieved object should be AMDbObject: " << typeString;
		QCOMPARE(typeString, QString("AMDbObject"));

		qDebug() << "Confirming object was dynamically loaded: pointer is: " << dbo2;
		QVERIFY(dbo2 != 0);

		qDebug() << "Retrieved DbObject out of database: comparing all parameters with original:";
		QCOMPARE(dbo2->id(), dbo.id());
		QCOMPARE(dbo2->name(), dbo.name());
		QCOMPARE(dbo2->number(), dbo.number());
		QCOMPARE(dbo2->dateTime().toString("yyyy MM/dd hh:mm:ss"), dbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
	}

	/// Test inserts of AMScan into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMScan_retrieveStatically() {
		AMScan dbo;

		// generate unique name, and set properties:
		qDebug() << "Generating AMScan with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);
		dbo.setSampleName("mySampleName" + uniqueName);
		dbo.setComments("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		qDebug() << "Testing AMScan insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMScan dbo2;
		dbo2.loadFromDb(AMDatabase::userdb(), dbo.id());
		qDebug() << "Retrieving AMScan out of database: comparing all parameters with original:";
		QCOMPARE(dbo2.id(), dbo.id());
		QCOMPARE(dbo2.name(), dbo.name());
		QCOMPARE(dbo2.number(), dbo.number());
		QCOMPARE(dbo2.dateTime().toString("yyyy MM/dd hh:mm:ss"), dbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
		QCOMPARE(dbo2.sampleName(), dbo.sampleName());
		QCOMPARE(dbo2.comments(), dbo.comments());
	}

	/// Test inserts of AMScan into database, and confirm all values loaded back with dynamic loader:
	void insertAMScan_retrieveDynamically() {
		AMScan dbo;

		// generate unique name, and set properties:
		qDebug() << "Generating AMScan with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);
		dbo.setSampleName("mySampleName" + uniqueName);
		dbo.setComments("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		qDebug() << "Testing AMScan insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject* dbo2d = 0;
		QString typeString;
		dbo2d = AMDbLoader::createAndLoad(AMDatabase::userdb(), dbo.id(),this, &typeString);

		qDebug() << "Type of retrieved object should be AMScan: " << typeString;
		QCOMPARE(typeString, QString("AMScan"));

		qDebug() << "Confirming object was dynamically loaded: pointer is: " << dbo2d;
		QVERIFY(dbo2d != 0);
		qDebug() << "Confirming dynamically loaded object is an AMScan: pointer is: " << dbo2d;
		AMScan* dbo2 = qobject_cast<AMScan*>(dbo2d);
		QVERIFY(dbo2 != 0);

		// Load same scan back:
		qDebug() << "Retrieving AMScan out of database: comparing all parameters with original:";
		QCOMPARE(dbo2->id(), dbo.id());
		QCOMPARE(dbo2->name(), dbo.name());
		QCOMPARE(dbo2->number(), dbo.number());
		QCOMPARE(dbo2->dateTime().toString("yyyy MM/dd hh:mm:ss"), dbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
		QCOMPARE(dbo2->sampleName(), dbo.sampleName());
		QCOMPARE(dbo2->comments(), dbo.comments());
	}

	/// Test inserts of AMScan into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMScan_search() {
		AMScan dbo;

		// generate unique name, and set properties:
		qDebug() << "Generating AMScan with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(QDateTime::currentDateTime().toTime_t());
		dbo.setSampleName("mySampleName" + uniqueName);
		dbo.setComments("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		qDebug() << "Testing AMScan insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		QList<int> lr;
		// Check: all columns: scans matching should be 1:
		qDebug() << "Checking scansMatching finds one matching for each column.";
		//lr = AMDatabase::userdb()->scansMatching("id", dbo.id());
		//QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->scansMatching("name", dbo.name());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->scansMatching("number", dbo.number());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->scansMatching("sampleName", dbo.sampleName());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->scansMatching("comments", dbo.comments());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->scansMatching("dateTime", dbo.dateTime());
		/// \todo check for 1-minute tolerance on date-time... This fails...
		QCOMPARE(lr.count(), 1);




	}

};

QTEST_MAIN(TestDataman)
#include "tests.moc"
