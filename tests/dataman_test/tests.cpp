#include <QtTest/QtTest>
#include "dataman/AMDatabase.h"
#include "dataman/AMScan.h"
#include "dataman/AMDbLoader.h"
#include "AMSettings.h"

class Test1: public QObject
{
	Q_OBJECT
private slots:

	// This runs before any of the private slots (test cases) get run:
	void initTestCase()
	{
		AMSettings::load();
		AMUserSettings::load();
		AMScan::dbPrepareTables(AMDatabase::userdb());
	}

	void cleanupTestCase()
	 { qDebug("called after all tests complete"); }


	// A test (or bunch of tests, actually...)
	void dbInsertRetrieve() {
		/// testing database inserts
		// =================================
		AMScan jimbo;
		// generate unique name:
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		jimbo.setName("myTestScan" + uniqueName);
		jimbo.setNumber(3);
		jimbo.setSampleName("Carbon_60");
		jimbo.setComments("This is\n a three-line\n comment!");
		jimbo.setDateTime(QDateTime::currentDateTime());
		// Save (or update) jimbo in the database
		*AMDatabase::userdb() << jimbo;

		qDebug() << "Testing insert into database: non-zero id?" << jimbo.id();
		QVERIFY(jimbo.id() > 0);	// check that insert succeeded.


		QList<int> jimboIds = AMDatabase::userdb()->scansMatching("name", jimbo.name());
		qDebug() << "Testing for scans with name matching" << jimbo.name() << ". Found: " << jimboIds.count();
		QCOMPARE(jimboIds.count(), 1);	// should not be 0 (insert failed)

		jimboIds = AMDatabase::userdb()->scansMatching("name", uniqueName);
		qDebug() << "testing for scans matching section of unique name. Should be 0:" << jimboIds.count();
		QCOMPARE(jimboIds.count(), 0);	// should not match any

		jimboIds = AMDatabase::userdb()->scansContaining("name", uniqueName);
		qDebug() << "Testing for scans containing section of unique name. Should find 1:" << jimboIds.count();
		QCOMPARE(jimboIds.count(), 1);	// should only match one

		qDebug() << "Testing type of object with userdb()->scanType()" << AMDatabase::userdb()->scanType(jimbo.id());
		QCOMPARE(AMDatabase::userdb()->scanType(jimbo.id()), jimbo.type());

		qDebug() << "Testing type of nonexistent object:" << AMDatabase::userdb()->scanType(1223982323);
		QCOMPARE(AMDatabase::userdb()->scanType(1223982323), QString(""));	// should not be found.

		// Use the powerful anyType loader to load from the database:
		AMDbObject* loadedObject = AMDbLoader::createAndLoad(AMDatabase::userdb(), jimbo.id());
		qDebug() << "test loading from database: createAndLoad(): not zero?" << loadedObject;
		QVERIFY(loadedObject != 0);	// should have found it

		// Can ask for it's type using
		if(loadedObject) {
			qDebug() << "loaded database object @ id dynamically; his type is: " << loadedObject->type();
			QCOMPARE(loadedObject->type(), jimbo.type());
		}
		// If you want, convert to a Scan using qt's dynamic cast:
		AMScan* loadedScan = qobject_cast<AMScan*>(loadedObject);
		if(loadedScan) {
			qDebug() << "Dynamic object was a Scan... his sampleName is" << loadedScan->sampleName();
			QVERIFY(loadedScan != 0);
			qDebug() << "checking for matching retrieved scan parameters in AMScan:";
			QCOMPARE(loadedScan->sampleName(), jimbo.sampleName());
			QCOMPARE(loadedScan->comments(), jimbo.comments());
			QCOMPARE(loadedScan->channelNames(), jimbo.channelNames());
			qDebug() << "checking for matching retrieved scan parameters in DbObject:";
			QCOMPARE(loadedScan->id(), jimbo.id());
			QCOMPARE(loadedScan->dateTime().toString("yyyy MM/dd hh:mm:ss"), jimbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
			QCOMPARE(loadedScan->number(), jimbo.number());
			QCOMPARE(loadedScan->name(), jimbo.name());
		}
		else {
			qDebug() << "Dynamically loaded an object, but it's not a Scan.";
			QVERIFY(false);	// this should never happen
		}

		// But all this is silly if you already know the type of the object.  Just do:
		AMScan newScan2;
		qDebug() << "Attempting to load scan at last id. Found it: " << newScan2.loadFromDb(AMDatabase::userdb(), jimbo.id());
		qDebug() << "scan at last id: comment is: " << newScan2.comments();
		QCOMPARE(newScan2.comments(), jimbo.comments());
		qDebug() << "dateTime is: " << newScan2.dateTime();
		QCOMPARE(newScan2.sampleName(), jimbo.sampleName());
		qDebug() << "id is: " << newScan2.id();
		qDebug() << "check for all parameters in AMScan to match:";
		QCOMPARE(newScan2.id(), jimbo.id());
		QCOMPARE(newScan2.dateTime().toString("yyyy MM/dd hh:mm:ss"), jimbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
		QCOMPARE(newScan2.channelNames(), jimbo.channelNames());
		QCOMPARE(newScan2.number(), jimbo.number());
		QCOMPARE(newScan2.name(), jimbo.name());

	}
};

QTEST_MAIN(Test1)
#include "tests.moc"
