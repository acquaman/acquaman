/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <QtTest/QtTest>
#include "dataman/database/AMDatabase.h"
#include "dataman/AMScan.h"
#include "dataman/AMXASScan.h"
#include "dataman/SGM/SGM2004FileLoader.h"
#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/datastore/AMInMemoryDataStore.h"
#include "dataman/AMSamplePlatePre2013.h"
#include "util/AMOrderedSet.h"

#include "util/AMTagReplacementParser.h"

/// This subclass of AMDbObject is used only for test purposes.
class AMTestDbObject : public AMDbObject {

	Q_OBJECT
	Q_PROPERTY(AMDbObject* myScan READ myScan WRITE reloadMyScan)
			Q_PROPERTY(AMDbObjectList myDbObjects READ myDbObjects WRITE reloadMyDbObjects)
			Q_PROPERTY(int numScans READ numScans WRITE reloadNumScans)
	Q_PROPERTY(AMIntList emptyList READ emptyList WRITE setEmptyList)
	Q_PROPERTY(AMDoubleList emptyListD READ emptyListD WRITE setEmptyListD)
	Q_PROPERTY(QStringList emptyListS READ emptyListS WRITE setEmptyListS)

public:
 	virtual ~AMTestDbObject(){}
			Q_INVOKABLE explicit AMTestDbObject(QObject* parent = 0) : AMDbObject(parent) {
		mySingleScan_ = 0;
	}//todo

	AMDbObject* myScan() const { return mySingleScan_; }
	void reloadMyScan(AMDbObject* newObj) { if(mySingleScan_) delete mySingleScan_; mySingleScan_ = qobject_cast<AMScan*>(newObj); }

	int numScans() const { return myDbObjects_.count(); }
	void reloadNumScans(int num) { qDebug() << "AMTestDbObject: num scans:" << num; }

	AMDbObjectList myDbObjects() const { return myDbObjects_; }

	void reloadMyDbObjects(const AMDbObjectList& newScans) {
		qDebug() << "AMTestDbObject: creating/loading" << newScans.count() << "brand new scans";
		for(int i=0; i<myDbObjects_.count(); i++)
			if(myDbObjects_.at(i))
				delete myDbObjects_.at(i);
		myDbObjects_ = newScans;
	}


	AMIntList emptyList() const { return emptyList_; }
	AMDoubleList emptyListD() const { return emptyListD_; }
	QStringList emptyListS() const { return emptyListS_; }

	void setEmptyList(const AMIntList& e) { emptyList_ = e; }
	void setEmptyListD(const AMDoubleList& e) { emptyListD_ = e; }
	void setEmptyListS(const QStringList& e) { emptyListS_ = e; }


protected:
	AMScan* mySingleScan_;
	AMDbObjectList myDbObjects_;

	AMIntList emptyList_;
	AMDoubleList emptyListD_;
	QStringList emptyListS_;

};

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

	}

	/// This is run after all tests are complete.
	void cleanupTestCase()
	{

	}


	void testAMTagReplacementParser() {

		QString inputText("$Today is $date and I am feeling $mood.\n\n"
						  "The weather is $weather and I will wear my $shirtColor[3] shirt today.\n\n"
						  "Some more fun with $words$numbers and $words[asdf fdsa]$numbers[3 4 $notHere ]\n\n"
						  "This should stay as a $$3 dollars  and this too: $$.  $k$t[ ]$s.  Coo?");

		AMTagReplacementParser p('$', '[', ']');
		p.setInitialText(inputText);

		QVERIFY(p.replacementList().count() == 12);
		QVERIFY(p.replacementList().at(0).tag == "Today");
		QVERIFY(p.replacementList().at(1).tag == "date");
		QVERIFY(p.replacementList().at(2).tag == "mood");
		QVERIFY(p.replacementList().at(3).tag == "weather");
		QVERIFY(p.replacementList().at(4).tag == "shirtColor");
		QVERIFY(p.replacementList().at(4).arguments == "3");
		QVERIFY(p.replacementList().at(5).tag == "words");
		QVERIFY(p.replacementList().at(5).arguments == QString());
		QVERIFY(p.replacementList().at(6).tag == "numbers");
		QVERIFY(p.replacementList().at(6).arguments == QString());
		QVERIFY(p.replacementList().at(7).tag == "words");
		QVERIFY(p.replacementList().at(7).arguments == "asdf fdsa");
		QVERIFY(p.replacementList().at(8).tag == "numbers");
		QVERIFY(p.replacementList().at(8).arguments == "3 4 $notHere ");
		QVERIFY(p.replacementList().at(9).tag == "k");
		QVERIFY(p.replacementList().at(10).tag == "t");
		QVERIFY(p.replacementList().at(11).tag == "s");

		p.replacementList()[0].replacement = "Monday";
		p.replacementList()[1].replacement = "March 18";
		p.replacementList()[2].replacement = "bummed out";
		p.replacementList()[3].replacement = "rainy";
		p.replacementList()[4].replacement = "brown";
		p.replacementList()[5].replacement = "words";
		p.replacementList()[6].replacement = "numbers";
		p.replacementList()[7].replacement = "numbers";
		p.replacementList()[8].replacement = "and words";
		p.replacementList()[9].replacement = "k";
		p.replacementList()[10].replacement = "thx";
		p.replacementList()[11].replacement = "bai";


		QString final = p.getReplacedText();
		QString finalShouldBe("Monday is March 18 and I am feeling bummed out.\n\n"
							  "The weather is rainy and I will wear my brown shirt today.\n\n"
							  "Some more fun with wordsnumbers and numbersand words\n\n"
							 "This should stay as a $3 dollars  and this too: $.  kthxbai.  Coo?");

		QVERIFY(final == finalShouldBe);
	}


	void testAMControlInfoList() {

		AMControlInfo* ci = new AMControlInfo("bob", 5, 1, 10, "pF");
		QVERIFY(ci->storeToDb(AMDatabase::database("user")));

		AMControlInfoList* l = new AMControlInfoList();

		l->append(AMControlInfo("mark",3,1,4,"feet"));
		l->append(AMControlInfo("david", 5,4,6,"inches"));
		l->append(AMControlInfo("darren", 30,20,40,"cm"));

		QVERIFY(l->storeToDb(AMDatabase::database("user")));
		QVERIFY(l->modified() == false);


		AMControlInfoList* l2 = new AMControlInfoList();
		QVERIFY(l2->loadFromDb(AMDatabase::database("user"), l->id()));

		// test store and reload from db
		QVERIFY(l2->at(0).name() == "mark");
		QCOMPARE(l2->at(1).value(), double(5));
		QCOMPARE(l2->at(2).minimum(), double(20));

		// modify them...
		(*l2)[0].setName("steve");
		(*l2)[1].setName("mary");
		(*l2)[2].setName("james");

		// test assignment operator
		*l = *l2;

		QVERIFY(l->at(0).name() == "steve");
		QVERIFY(l->at(1).name() == "mary");
		QVERIFY(l->at(2).name() == "james");

		// modified detection for l should be detected already (when the assignment is made)
		QVERIFY(l->modified() == true);

		// modified for l2 won't kick in until flow goes back to event loop...
		QVERIFY(l2->modified() == false);
		qApp->processEvents();
		qApp->processEvents();
		QVERIFY(l2->modified() == true);



		delete l;
		delete l2;
	}

	void testAMSamplePlate() {

		AMSamplePlatePre2013* p1 = new AMSamplePlatePre2013();

		AMControlInfoList pos;
		pos.append(AMControlInfo("testControl", 3.0, 1.0, 5.0, "pF"));

		p1->append(AMSamplePositionPre2013(3, pos, 2));

		int dbId = p1->storeToDb(AMDatabase::database("user"));
		QVERIFY(dbId > 0);

		delete p1;

		AMSamplePlatePre2013* p2 = new AMSamplePlatePre2013();
		QVERIFY(p2->loadFromDb(AMDatabase::database("user"), dbId) == true);
		QVERIFY(p2->modified() == false);

		QVERIFY(p2->at(0).position().at(0).name() == "testControl");
		QVERIFY(p2->at(0).position().at(0).value() == 3.0);
		QVERIFY(p2->at(0).sampleId() == 3);
		QVERIFY(p2->at(0).facilityId() == 2);

		delete p2;

	}

	void testInMemoryDataStore() {

		AMInMemoryDataStore* s = new AMInMemoryDataStore();

		s->addMeasurement(AMMeasurementInfo("meas0", "Measurement 0"));

		// beginning: no axes. Scalar scan space. Should be able to access measurements at scan point index AMnDIndex().
		QVERIFY(s->setValue(AMnDIndex(), 0, AMnDIndex(), AMNumber(3)));
		QVERIFY(s->value(AMnDIndex(), 0, AMnDIndex()) == AMNumber(3));


		// starting with a single axis
		QVERIFY(s->addScanAxis(AMAxisInfo("x", 0, "x axis")));

		// Scalar scan space should not work anymore
		QVERIFY(s->setValue(AMnDIndex(), 0, AMnDIndex(), AMNumber(4)) == false);
		QVERIFY(s->value(AMnDIndex(), 0, AMnDIndex()) == AMNumber(AMNumber::DimensionError));

		QVERIFY(s->scanSpaceIsEmpty());
		QVERIFY(s->scanAxesCount() == 1);
		QVERIFY(s->scanAxisAt(0).name == "x");

		QVERIFY(s->measurementCount() == 1);
		QVERIFY(s->measurementAt(0).name == "meas0");

		// insert single row check size insert values and check
		QVERIFY(s->beginInsertRows(0, 1));
		QVERIFY(s->setValue(AMnDIndex(0), 0, AMnDIndex(), 3.1415));
		QVERIFY(s->setAxisValue(0,0,1.2));
		s->endInsertRows();

		QVERIFY(s->value(AMnDIndex(0), 0, AMnDIndex()) == 3.1415);
		QVERIFY(s->axisValue(0,0) == 1.2);

		QVERIFY(s->beginInsertRows(9, 0));	// prepend 9 rows
		s->endInsertRows();
		// check that initial values are AMNumber::Null.
		for(int i=0; i<9; i++) {
			QVERIFY(s->value(i, 0, AMnDIndex()) == AMNumber());
		}
		for(int i=0; i<9; i++) {
			QVERIFY(s->setValue(AMnDIndex(i), 0, AMnDIndex(), 0));
		}
		for(int i=0; i<9; i++) {
			QVERIFY(s->value(i, 0, AMnDIndex()) == 0.0);
		}


		QVERIFY(s->scanSize(0) == 10);
		QVERIFY(s->value(9, 0, AMnDIndex()) == 3.1415);	// ensure prepend worked
		QVERIFY(s->axisValue(0, 9) == 1.2);


		// create first row of multiplication table for 2D test coming up.
		for(int i=0; i<10; i++) {
			s->setValue(i, 0, AMnDIndex(), i+1);
		}



		// adding another axis.
		QVERIFY(s->addScanAxis(AMAxisInfo("y", 0, "y axis")));
		QVERIFY(s->scanRank() == 2);
		QVERIFY(s->scanSize() == AMnDIndex(10, 1));
		QVERIFY(s->scanSize(1) == 1);
		QVERIFY(s->scanAxisAt(1).name == "y");

		for(int i=0; i<s->scanSize(0); i++) {
			QVERIFY(s->value(AMnDIndex(i,0), 0, AMnDIndex()) == i+1.0);
			QVERIFY(s->value(AMnDIndex(i), 0, AMnDIndex()) == AMNumber(AMNumber::DimensionError)); // wrong dimension of index
		}

#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		QVERIFY(s->value(AMnDIndex(200,0), 0, AMnDIndex()) == AMNumber(AMNumber::OutOfBoundsError));
#endif

		QVERIFY(s->value(AMnDIndex(5,0), 3, AMnDIndex()) == AMNumber(AMNumber::InvalidError));	// invalid measurement

		/// \todo test creating a second axis when first is empty...

		QVERIFY(s->beginInsertRows(1, 9));	// append 9 rows to 2nd dimension
		s->endInsertRows();

		for(int i=0; i<10; i++) {
			for(int j=1; j<10; j++) {
				QVERIFY(s->value(AMnDIndex(i,j), 0, AMnDIndex()) == AMNumber(AMNumber::Null));	// initial values should be AMNumber::Null everywhere except first slice.
			}
		}
		for(int i=0; i<10; i++) {
			for(int j=0; j<10; j++) {
				QVERIFY(s->setValue(AMnDIndex(i,j), 0, AMnDIndex(), (i+1.0)*(j+1.0)));
				QVERIFY(s->setAxisValue(1, j, j+1));
			}
			QVERIFY(s->setAxisValue(0, i, i+1));
		}

// check 2D multiplication table

		QString row = " :\t";
		for(int j=0; j<s->scanSize(1); j++)
			row.append(QString("%1\t").arg((double)s->axisValue(1, j)));
		for(int i=0; i<s->scanSize(0); i++) {
			QString row = QString("%1:\t").arg((double)s->axisValue(0,i));
			for(int j=0; j<s->scanSize(1); j++) {
				double value = s->value(AMnDIndex(i,j), 0, AMnDIndex());
				row.append(QString("%1\t").arg(value));
				QVERIFY(value == (i+1.0)*(j+1.0));
			}
		}


		// adding another measurement to existing points:
		QVERIFY(s->addMeasurement(AMMeasurementInfo("meas1", "measurement 1")));
		QVERIFY(s->measurementCount() == 2);

		for(int i=0; i<s->scanSize(0); i++) {
			for(int j=0; j<s->scanSize(1); j++) {
				QVERIFY(s->value(AMnDIndex(i,j), 1, AMnDIndex()) == AMNumber(AMNumber::Null));
				QVERIFY(s->value(AMnDIndex(i,j), 0, AMnDIndex()).isValid());
			}
		}


		// copy first measurement values to second measurement multiply by 2 in process
		for(int i=0; i<s->scanSize(0); i++) {
			for(int j=0; j<s->scanSize(1); j++) {
				QVERIFY(s->setValue(AMnDIndex(i,j), 1, AMnDIndex(),
									(double)s->value(AMnDIndex(i,j), 0, AMnDIndex())*2.0) );
			}
		}

		// check that this worked now all values shouldbe valid.
		for(int i=0; i<s->scanSize(0); i++) {
			for(int j=0; j<s->scanSize(1); j++) {
				QVERIFY(s->value(AMnDIndex(i,j), 1, AMnDIndex()) == (double)s->value(AMnDIndex(i,j), 0, AMnDIndex())*2.0);
				QVERIFY(s->value(AMnDIndex(i,j), 0, AMnDIndex()).isValid());
				QVERIFY(s->value(AMnDIndex(i,j), 1, AMnDIndex()).isValid());
			}
		}

		QVERIFY(s->scanSize() == AMnDIndex(10,10));
		s->clearScanDataPoints();
		QVERIFY(s->scanSpaceIsEmpty());
		QVERIFY(s->scanSize() == AMnDIndex(0,1));

		// repeat test after clearing, to make sure data structure still has integrity.
			// also tests inserting rows when the final axis has size 0.
		QVERIFY(s->beginInsertRows(1, 9));
		QVERIFY(s->beginInsertRows(0,10) == false);	// can't nest beginInsertRows().
		s->endInsertRows();
		QVERIFY(s->scanSize() == AMnDIndex(0, 10));
		QVERIFY(s->beginInsertRows(0, 10));
		s->endInsertRows();

		for(int i=0; i<s->scanSize(0); i++) {
			for(int j=0; j<s->scanSize(1); j++) {
				QVERIFY(s->setValue(AMnDIndex(i,j), 0, AMnDIndex(), (i+1.0)*(j+1.0)));
			}
		}

		for(int i=0; i<10; i++) {
			for(int j=0; j<10; j++) {
				QVERIFY(s->value(AMnDIndex(i,j), 0, AMnDIndex()) ==(i+1.0)*(j+1.0));
			}
		}



		delete s;



		//////////////////////////

		// test with in-memory data store on stack
		AMInMemoryDataStore s2;

		QVERIFY(s2.addScanAxis(AMAxisInfo("x", 0, "x axis")));
		// test adding second dimension when first dimension is size 0.
		QVERIFY(s2.addScanAxis(AMAxisInfo("y", 0, "y axis")));

		QVERIFY(s2.addMeasurement(AMMeasurementInfo("meas0", "measurement 0")));

		QVERIFY(s2.scanSize() == AMnDIndex(0,1));

		QVERIFY(s2.addScanAxis(AMAxisInfo("z", 0, "z axis")));

		QVERIFY(s2.scanSize() == AMnDIndex(0, 1, 1));

		QVERIFY(s2.beginInsertRows(2, 9));
		s2.endInsertRows();
		QVERIFY(s2.beginInsertRows(0, 10));
		s2.endInsertRows();
		QVERIFY(s2.scanSize() == AMnDIndex(10,1,10));
		QVERIFY(s2.beginInsertRows(1,9));

		for(int i=0; i<10; i++) {
			for(int j=0; j<10; j++) {
				for(int k=0; k<10; k++) {
					QVERIFY(s2.setValue(AMnDIndex(i,j,k), 0, AMnDIndex(), (i+1)*(j+1)*(k+1) ));
				}
			}
		}

		s2.endInsertRows();


		for(int i=0; i<10; i++) {
			for(int j=0; j<10; j++) {
				for(int k=0; k<10; k++) {
					QVERIFY((double)s2.value(AMnDIndex(i,j,k), 0, AMnDIndex()) == (i+1)*(j+1)*(k+1)	);
				}
			}
		}

		/// \todo Test multi-dimensional measurements.  Remove all optimizations for rank() < 5 and re-run tests to check the general handling. Do memory leak test (How?)
	}



	/// Test inserts of DbObjects into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMDbObject_retrieveStatically() {
		AMDbObject dbo;

		// generate unique name, and set properties:
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestDbObject" + uniqueName);


		// Insert into db:
		*AMDatabase::database("user") << dbo;

		// Was it inserted succesfully?
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject dbo2;
		dbo2.loadFromDb(AMDatabase::database("user"), dbo.id());
		QCOMPARE(dbo2.id(), dbo.id());
		QCOMPARE(dbo2.name(), dbo.name());

	}

	/// Test inserts of DbObjects into database, and confirm all values loaded back with dynamic loader:
	void insertAMDbObject_retrieveDynamically() {
		AMDbObject dbo;

		// generate unique name, and set properties:
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestDbObject" + uniqueName);

		// Insert into db:
		*AMDatabase::database("user") << dbo;

		// Was it inserted succesfully?
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject* dbo2 = 0;
		dbo2 = AMDbObjectSupport::s()->createAndLoadObjectAt(AMDatabase::database("user"), dbo.dbTableName(), dbo.id());


		QVERIFY(dbo2 != 0);

		QCOMPARE(dbo2->type(), QString("AMDbObject"));

		QCOMPARE(dbo2->id(), dbo.id());
		QCOMPARE(dbo2->name(), dbo.name());
	}

	/// Test inserts of AMScan into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMScan_retrieveStatically() {
		AMScan dbo;

		// generate unique name, and set properties:
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);
		dbo.setNotes("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::database("user") << dbo;

		// Was it inserted succesfully?
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMScan dbo2;
		dbo2.loadFromDb(AMDatabase::database("user"), dbo.id());
		QCOMPARE(dbo2.id(), dbo.id());
		QCOMPARE(dbo2.name(), dbo.name());
		QCOMPARE(dbo2.number(), dbo.number());
		QCOMPARE(dbo2.dateTime().toString("yyyy MM/dd hh:mm:ss"), dbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
		QCOMPARE(dbo2.sampleName(), dbo.sampleName());
		QCOMPARE(dbo2.notes(), dbo.notes());
	}

	/// Test inserts of AMScan into database, and confirm all values loaded back with dynamic loader:
	void insertAMScan_retrieveDynamically() {
		AMScan dbo;

		// generate unique name, and set properties:
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);
		dbo.setNotes("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::database("user") << dbo;

		// Was it inserted succesfully?
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject* dbo2d = 0;
		QString typeString;
		dbo2d = AMDbObjectSupport::s()->createAndLoadObjectAt(dbo.database(), dbo.dbTableName(), dbo.id());

		QVERIFY(dbo2d != 0);

		QCOMPARE(dbo2d->type(), QString("AMScan"));

		AMScan* dbo2 = qobject_cast<AMScan*>(dbo2d);
		QVERIFY(dbo2 != 0);

		// Load same scan back:
		QCOMPARE(dbo2->id(), dbo.id());
		QCOMPARE(dbo2->name(), dbo.name());
		QCOMPARE(dbo2->number(), dbo.number());
		QCOMPARE(dbo2->dateTime().toString("yyyy MM/dd hh:mm:ss"), dbo.dateTime().toString("yyyy MM/dd hh:mm:ss"));
		QCOMPARE(dbo2->sampleName(), dbo.sampleName());
		QCOMPARE(dbo2->notes(), dbo.notes());
	}

	/// Test inserts of AMScan into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMScan_search() {
		AMScan dbo;

		// generate unique name, and set properties:
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(QDateTime::currentDateTime().toTime_t());
		dbo.setNotes("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::database("user") << dbo;

		// Was it inserted succesfully?
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		QList<int> lr;
		// Check: all columns: scans matching should be 1:
		lr = AMDatabase::database("user")->objectsMatching(dbo.dbTableName(), "name", dbo.name());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::database("user")->objectsMatching(dbo.dbTableName(), "number", dbo.number());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::database("user")->objectsMatching(dbo.dbTableName(), "sampleId", dbo.sampleId());
		QVERIFY(lr.count() > 1);
		lr = AMDatabase::database("user")->objectsMatching(dbo.dbTableName(), "notes", dbo.notes());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::database("user")->objectsMatching(dbo.dbTableName(), "dateTime", dbo.dateTime());
		/// \todo check for 1-minute tolerance on date-time... This fails...
		QCOMPARE(lr.count(), 1);




	}




	/// test loading an AMXASScan from legacy SGM data. Also tests creation of default channels inside SGM2004FileLoader::loadFromFile()

	void loadAMXASScan() {
		AMXASScan s1;
		SGM2004FileLoader s1Loader(&s1);
		/// \todo move this into proper storage location in data dir.
		QString fileName = AMUserSettings::userDataFolder + "/testScriptData/sgm001.dat";
		QVERIFY(s1Loader.loadFromFile(fileName, true, true, true));
		QCOMPARE(s1.scanSize(0), int(301));
		QCOMPARE(s1.notes(), QString("0.916667"));	// NOTE: this test fails, because we're currently putting the Grating and Integration Time inside the comment field (for lack of a better place) Eventually, the Grating and Integration time should become part of the scan configuration, or beamline state.
		QCOMPARE(s1.dateTime().toTime_t(), uint(1285706567));


		int tey_n, tfy_n, tey, tfy, io;
		QVERIFY((tey = s1.rawDataSources()->indexOfKey("tey")) != -1);
		QVERIFY((tfy = s1.rawDataSources()->indexOfKey("tfy")) != -1);
		QVERIFY((io = s1.rawDataSources()->indexOfKey("I0")) != -1);
		QVERIFY((tey_n = s1.analyzedDataSources()->indexOfKey("tey_n")) != -1);
		QVERIFY((tfy_n = s1.analyzedDataSources()->indexOfKey("tfy_n")) != -1);
		QCOMPARE(qobject_cast<AM1DExpressionAB*>(s1.analyzedDataSources()->at(tey_n))->expression(), QString("tey/I0"));
		QCOMPARE(qobject_cast<AM1DExpressionAB*>(s1.analyzedDataSources()->at(tfy_n))->expression(), QString("-1*tfy/I0"));
		// test math parser:
		for(int i=0; i<s1.scanSize(0); i++)
			QVERIFY((double)s1.analyzedDataSources()->at(tey_n)->value(i) == (double)s1.rawDataSources()->at(tey)->value(i) / (double)s1.rawDataSources()->at(io)->value(i));
		// test math parser:
		for(int i=0; i<s1.scanSize(0); i++)
			QVERIFY((double)s1.analyzedDataSources()->at(tfy_n)->value(i) == (double)s1.rawDataSources()->at(tfy)->value(i) / (double)s1.rawDataSources()->at(io)->value(i));
	}


	/// Using test file testScriptData/sgm001.dat, tests AM1DExpressionAB expressions (setting expression, setting xExpression, using default xExpression, evaluating expressions, setting and evaluating invalid expressions)
	void testAM1DExpressionAB() {
		AMXASScan s1;
		SGM2004FileLoader s1Loader(&s1);
		/// \todo move this into proper storage location in data dir.
		QString fileName = AMUserSettings::userDataFolder + "/testScriptData/sgm001.dat";
		QVERIFY(s1Loader.loadFromFile(fileName, true, true, true));

		// create simple math channel
		AM1DExpressionAB* const5Channel = new AM1DExpressionAB("const5");
		QList<AMDataSource*> rawDataSources;
		foreach(AMRawDataSource* ds, s1.rawDataSources()->toList())
			rawDataSources << ds;
		const5Channel->setInputDataSources(rawDataSources);
		QVERIFY(const5Channel->setExpression("3+2"));
		QVERIFY(const5Channel->isValid());
		s1.addAnalyzedDataSource(const5Channel);

		QVERIFY(const5Channel->value(0) == AMNumber(5.0));
		QVERIFY(const5Channel->value(3) == AMNumber(5.0));
		QVERIFY(const5Channel->value(900) == AMNumber(AMNumber::OutOfBoundsError));


		// verify auto-created channels (inside loadFromFile())
		QVERIFY(s1.analyzedDataSources()->indexOfKey("tey_n") == 0);
		QVERIFY(s1.analyzedDataSources()->indexOfKey("tfy_n") == 1);

		AM1DExpressionAB* teyn = qobject_cast<AM1DExpressionAB*>(s1.analyzedDataSources()->at(0));
		AM1DExpressionAB* tfyn = qobject_cast<AM1DExpressionAB*>(s1.analyzedDataSources()->at(1));
		QVERIFY(teyn);
		QVERIFY(tfyn);

		for(int i=0; i<teyn->size(0); i++)
			QVERIFY(teyn->axisValue(0, i) == s1.rawData()->axisValue(0, i));

		// set x expression and check for adjusted values. (ex: calibrating by +1 eV)
		QVERIFY(teyn->setXExpression("tey.x + 1"));
		QVERIFY(teyn->isValid());
		for(int i=0; i<teyn->size(0); i++)
			QVERIFY(double(teyn->axisValue(0, i)) == double(s1.rawData()->axisValue(0, i))+1.0);

		// restore the x expression to default and check values:
		QVERIFY(teyn->setXExpression());
		QVERIFY(teyn->isValid());
		for(int i=0; i<teyn->size(0); i++)
			QVERIFY(teyn->axisValue(0, i) == s1.rawData()->axisValue(0, i));


		// set the y expression and check values:
		QVERIFY(teyn->setExpression("tey/I0 + 4"));
		QVERIFY(teyn->isValid());

		// need to get index of tey column and I0 column in data tree...
		int teyColIndex = -1;
		for(int i=0; i<s1.rawData()->measurementCount(); i++) {
			if(s1.rawData()->measurementAt(i).name == "tey") {
				teyColIndex = i;
				break;
			}
		}
		int ioColIndex = -1;
		for(int i=0; i<s1.rawData()->measurementCount(); i++) {
			if(s1.rawData()->measurementAt(i).name == "I0") {
				ioColIndex = i;
				break;
			}
		}
		QVERIFY(teyColIndex != -1);
		QVERIFY(ioColIndex != -1);

		for(int i=0; i<teyn->size(0); i++)
			QCOMPARE((double)teyn->value(i) , (double)s1.rawData()->value(i, teyColIndex, AMnDIndex()) / (double)s1.rawData()->value(i, ioColIndex, AMnDIndex())+4.0);
		// reset the y expression and check values:
		QVERIFY(teyn->setExpression("tey/I0"));
		QVERIFY(teyn->isValid());
		for(int i=0; i<teyn->size(0); i++)
			QVERIFY(teyn->value(i) == (double)s1.rawData()->value(i, teyColIndex, AMnDIndex()) / (double)s1.rawData()->value(i, ioColIndex, AMnDIndex()));

		// set a bad expression, check for failure, and check all values = InvalidValue.
		QVERIFY(teyn->setExpression("garbage + 4") == false);
		QVERIFY(!teyn->isValid());
		for(int i=0; i<teyn->size(0); i++)
			QVERIFY(teyn->value(i) == AMNumber(AMNumber::InvalidError));

		// set a bad x expression, check for failure, and check all values = c1->invalidValue()
		QVERIFY(teyn->setXExpression("garbage + kj3423") == false);
		QVERIFY(!teyn->isValid());
		for(int i=0; i<teyn->size(0); i++)
			QVERIFY(teyn->axisValue(0, i) == AMNumber(AMNumber::InvalidError));


	}


	void testDbObjectComposition() {
		AMDbObjectSupport::s()->registerClass<AMTestDbObject>();

		AMTestDbObject* t1 = new AMTestDbObject();
		t1->storeToDb(AMDatabase::database("user"));

		t1->loadFromDb(t1->database(), t1->id());
		QVERIFY(t1->myScan() == 0);

		AMScan* s = new AMScan();
		s->setName("special scan test name! With secret sauce.");
		t1->reloadMyScan(s);
		QVERIFY(t1->myScan() == s);

		t1->storeToDb(t1->database());

		s->setName("NO secret sauce.");

		t1->loadFromDb(t1->database(), t1->id());

		QVERIFY(t1->myScan() == s);	// first version of loadFromDb. Since myScan() returns a valid pointer, should simply call loadFromDb() on s.
		QCOMPARE(t1->myScan()->name(), QString("special scan test name! With secret sauce."));	// verifies s was reloaded properly.

		t1->myScan()->setName("NO secret sauce.");
		t1->reloadMyScan(0);	// deletes s.  myScan() will no longer return valid object.
		QVERIFY(t1->myScan() == 0);
		t1->loadFromDb(t1->database(), t1->id());
		// second version of loadFromDb(). Since myScan() didn't return valid, a new object was created.  myScan() should now have the original properties of s, but the pointer should be a different object than s. (But pointer might match s! the memory allocator is sneaky like that... there's an empty hole of just the write size to re-allocate the new one into.)
		QVERIFY(t1->myScan() != 0);
		//this might actually be (and turns out to happen). Could luckily allocate to just-deleted spot of s. QVERIFY(t1->myScan() != s)
		QCOMPARE(t1->myScan()->name(), QString("special scan test name! With secret sauce."));	// verifies "new s" was reloaded properly.


		// reload a completely new one...
		AMTestDbObject* t2 = new AMTestDbObject();
		t2->loadFromDb(t1->database(), t1->id());

		QCOMPARE(t2->myScan()->name(), QString("special scan test name! With secret sauce."));


		// test list of AMDbObjects...

		AMDbObjectList threeObjects;
		AMDbObject* o1 = new AMDbObject(); o1->setName("AMDbObject: object1");
		AMScan* o2 = new AMScan(); o2->setName("AMScan: object2");
		AMXASScan* o3 = new AMXASScan(); o3->setName("AMXASScan: object3");

		threeObjects << o1 << o2 << o3;

		t2->reloadMyDbObjects(threeObjects);
		QCOMPARE(t2->myScan()->name(), QString("special scan test name! With secret sauce."));

		QCOMPARE(t2->numScans(), 3);

		t2->storeToDb(AMDatabase::database("user"));


		QCOMPARE(t1->numScans(), 0);
		t1->loadFromDb(AMDatabase::database("user"), t2->id());	// should create brand new objects t1 didn't have three in the list.

		// catches a bug where member variable locations weren't being stored in the db if the member object wasn't modified. If it's not modified and already in the db, we don't need to re-storeToDb() it, but we still need remember that we own it, and remember where it is.
		QVERIFY(t1->myScan());
		QCOMPARE(t1->myScan()->name(), QString("special scan test name! With secret sauce."));

		QCOMPARE(t1->numScans(), 3);
		AMDbObjectList checkList = t1->myDbObjects();
		QCOMPARE(checkList.count(), 3);
		QCOMPARE(checkList.at(0)->name(), QString("AMDbObject: object1"));
		QCOMPARE(checkList.at(1)->name(), QString("AMScan: object2"));
		QCOMPARE(checkList.at(2)->name(), QString("AMXASScan: object3"));

		// how do we know that these are new objects? Could try accessing o1, o2, o3, but they should have been deleted. A crash would indicate success  Hmmm...


		t2->myDbObjects().at(0)->setName("Going going gone...");
		QCOMPARE(t2->myDbObjects().at(0)->name(), QString("Going going gone..."));

		t2->loadFromDb(t2->database(), t2->id());	// first version of loadFromDb()... all three exist already and match in type... should not call setProperty().  Verify that "AMTestDbObject: creating/loading brand new scans" only appears twice in the test log.
		QCOMPARE(t2->myDbObjects().at(0)->name(), QString("AMDbObject: object1")); // should be restored from last saved state...

	}

	void testListSaving() {
		AMTestDbObject s;

		QVERIFY(s.emptyList().count() == 0);
		QVERIFY(s.emptyListD().count() == 0);
		QVERIFY(s.emptyListS().count() == 0);

		int dbId = s.storeToDb(AMDatabase::database("user"));

		QVERIFY(dbId > 0);

		s.loadFromDb(AMDatabase::database("user"), dbId);

		QVERIFY(s.emptyList().count() == 0);
		QVERIFY(s.emptyListD().count() == 0);
		QVERIFY(s.emptyListS().count() == 0);

		AMTestDbObject s2;

		s2.loadFromDb(AMDatabase::database("user"), dbId);

		QVERIFY(s2.emptyList().count() == 0);
		QVERIFY(s2.emptyListD().count() == 0);
		QVERIFY(s2.emptyListS().count() == 0);

	}


	void testAMOrderedSet() {
		AMOrderedSet<QString, int> s1;

		s1.setAllowsDuplicateKeys(false);

		QVERIFY(s1.append(0, "0"));
		QVERIFY(s1.append(1, "1"));
		QVERIFY(s1.append(2, "2"));
		QVERIFY(!s1.append(3, "2"));	// duplicate key.
		QVERIFY(s1.append(3, "3"));
		QVERIFY(s1.append(4, "4"));

		QVERIFY(s1.at(0) == 0);
		QVERIFY(s1.at(1) == 1);
		QVERIFY(s1.at(2) == 2);
		QVERIFY(s1.at(3) == 3);
		QVERIFY(s1.at(4) == 4);

		QVERIFY(s1.indexOfKey("0") == 0);
		QVERIFY(s1.indexOfKey("1") == 1);
		QVERIFY(s1.indexOfKey("2") == 2);
		QVERIFY(s1.indexOfKey("3") == 3);
		QVERIFY(s1.indexOfKey("4") == 4);

		// test remove in middle:
		s1.remove(2);
		QVERIFY(s1.count() == 4);

		QVERIFY(s1.indexOfKey("0") == 0);
		QVERIFY(s1.indexOfKey("1") == 1);
		QVERIFY(s1.indexOfKey("2") == -1);
		QVERIFY(s1.indexOfKey("3") == 2);	// index moves down
		QVERIFY(s1.indexOfKey("4") == 3);

		QVERIFY(s1.at(0) == 0);
		QVERIFY(s1.at(1) == 1);
		QVERIFY(s1.at(2) == 3);	// value moves down
		QVERIFY(s1.at(3) == 4);


		// check insert inside:
		s1.insert(1, 88, "eighty-eight");

		QVERIFY(s1.count() == 5);

		QVERIFY(s1.indexOfKey("0") == 0);
		QVERIFY(s1.indexOfKey("eighty-eight") == 1);
		QVERIFY(s1.indexOfKey("1") == 2);	// index moves up was inserted in front
		QVERIFY(s1.indexOfKey("2") == -1);
		QVERIFY(s1.indexOfKey("3") == 3);
		QVERIFY(s1.indexOfKey("4") == 4);

		QVERIFY(s1.at(0) == 0);
		QVERIFY(s1.at(1) == 88);
		QVERIFY(s1.at(2) == 1);	// value moves down
		QVERIFY(s1.at(3) == 3);
		QVERIFY(s1.at(4) == 4);

		QVERIFY(s1.replace(1, 77, "seventyseven") == 88);
		QVERIFY(s1.indexOfKey("eighty-eight") == -1);
		QVERIFY(s1.indexOfKey("seventyseven") == 1);

		s1.clear();
		QVERIFY(s1.indexOfKey("seventyseven") == -1);
		QVERIFY(s1.count() == 0);

	}
};
