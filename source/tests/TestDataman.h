
#include <QtTest/QtTest>
#include "dataman/AMDatabase.h"
#include "dataman/AMScan.h"
#include "dataman/AMDataTree.h"
#include "dataman/AMXASScan.h"
#include "dataman/SGM2004FileLoader.h"
#include "AMErrorMonitor.h"
#include "dataman/AMDbObjectSupport.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMDataTreeDataStore.h"
#include "dataman/AMInMemoryDataStore.h"

#include "util/AMOrderedSet.h"

/// This subclass of AMDbObject is used only for test purposes.
class AMTestDbObject : public AMDbObject {

	Q_OBJECT
	Q_PROPERTY(AMDbObject* myScan READ myScan WRITE reloadMyScan)
			Q_PROPERTY(AMDbObjectList myDbObjects READ myDbObjects WRITE reloadMyDbObjects)
			Q_PROPERTY(int numScans READ numScans WRITE reloadNumScans)

public:
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



protected:
	AMScan* mySingleScan_;
	AMDbObjectList myDbObjects_;

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

	void testInMemoryDataStore() {

		AMInMemoryDataStore* s = new AMInMemoryDataStore();

		s->addMeasurement(AMMeasurementInfo("meas0", "Measurement 0"));

		// starting with a single axis
		s->addScanAxis(AMAxisInfo("x", 0, "x axis"));

		QVERIFY(s->isEmpty());
		QVERIFY(s->scanAxesCount() == 1);
		QVERIFY(s->scanAxisAt(0).name == "x");

		QVERIFY(s->measurementCount() == 1);
		QVERIFY(s->measurementAt(0).name == "meas0");

		// insert single row; check size; insert values and check
		QVERIFY(s->beginInsertRows(0, 1));
		QVERIFY(s->setValue(AMnDIndex(0), 0, AMnDIndex(), 3.1415));
		QVERIFY(s->setAxisValue(0,0,1.2));
		s->endInsertRows();

		QVERIFY(s->value(AMnDIndex(0), 0, AMnDIndex()) == 3.1415);
		QVERIFY(s->axisValue(0,0) == 1.2);

		QVERIFY(s->beginInsertRows(0, 9, 0));	// prepend 9 rows
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
		//qDebug() << row;
		//qDebug() << "======================================================";
		for(int i=0; i<s->scanSize(0); i++) {
			QString row = QString("%1:\t").arg((double)s->axisValue(0,i));
			for(int j=0; j<s->scanSize(1); j++) {
				double value = s->value(AMnDIndex(i,j), 0, AMnDIndex());
				row.append(QString("%1\t").arg(value));
				QVERIFY(value == (i+1.0)*(j+1.0));
			}
			//qDebug() << row;
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


		// copy first measurement values to second measurement; multiply by 2 in process
		for(int i=0; i<s->scanSize(0); i++) {
			for(int j=0; j<s->scanSize(1); j++) {
				QVERIFY(s->setValue(AMnDIndex(i,j), 1, AMnDIndex(),
									(double)s->value(AMnDIndex(i,j), 0, AMnDIndex())*2.0) );
			}
		}

		// check that this worked; now all values shouldbe valid.
		for(int i=0; i<s->scanSize(0); i++) {
			for(int j=0; j<s->scanSize(1); j++) {
				QVERIFY(s->value(AMnDIndex(i,j), 1, AMnDIndex()) == (double)s->value(AMnDIndex(i,j), 0, AMnDIndex())*2.0);
				QVERIFY(s->value(AMnDIndex(i,j), 0, AMnDIndex()).isValid());
				QVERIFY(s->value(AMnDIndex(i,j), 1, AMnDIndex()).isValid());
			}
		}

		QVERIFY(s->scanSize() == AMnDIndex(10,10));
		s->clearScanDataPoints();
		QVERIFY(s->isEmpty());
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

	// Using David's testof AMDataTreeDataStore to test AMInMemoryDataStore.
	/*
	void testInMemoryDataStoreWithAMDataTreeDataStoreTest() {

		// qDebug() << "\n\n\n DAVID STARTS HERE \n\nTesting data tree data store\n\n\n";


		AMMeasurementInfo teyInfo("tey", "Total Electron Yield", "amps");
		AMMeasurementInfo tfyInfo("tfy", "Total Fluoresence Yield", "amps");
		AMMeasurementInfo i0Info("I0", "I0", "amps");

		AMAxisInfo sddEVAxisInfo("energy", 1024, "SDD Energy", "eV");
		QList<AMAxisInfo> sddAxes;
		sddAxes << sddEVAxisInfo;
		AMMeasurementInfo sddInfo("sdd", "Silicon Drift Detector", "counts", sddAxes);

		AMAxisInfo xosWavelengthAxisInfo("wavelength", 512, "XOS Wavelength", "nm");
		QList<AMAxisInfo> xosAxes;
		xosAxes << xosWavelengthAxisInfo;
		AMMeasurementInfo xosInfo("xos", "XEOL Optical Spectrometer", "counts", xosAxes);

		AMAxisInfo xessXAxisInfo("x", 1280, "x pixel", "index");
		AMAxisInfo xessYAxisInfo("y", 1024, "y pixel", "index");
		QList<AMAxisInfo> xessAxes;
		xessAxes << xessXAxisInfo << xessYAxisInfo;
		AMMeasurementInfo xessInfo("xess", "XES Spectrometer", "counts", xessAxes);

		AMAxisInfo uglyRAxisInfo("r", 100, "Radial Distance", "percent");
		AMAxisInfo uglyPhiAxisInfo("phi", 180, "Phi Angle", "degrees");
		AMAxisInfo uglyThetaAxisInfo("theta", 360, "Theta Distance", "degrees");
		QList<AMAxisInfo> uglyAxes;
		uglyAxes << uglyRAxisInfo << uglyPhiAxisInfo << uglyThetaAxisInfo;
		AMMeasurementInfo uglyInfo("ugly", "Some Ugly Detector", "events", uglyAxes);

		AMAxisInfo scanAxis("energy", 1000, "Beamline Energy", "eV");

		AMInMemoryDataStore dtds;
		dtds.addScanAxis(scanAxis);

		QVERIFY(dtds.addMeasurement(teyInfo));
		QVERIFY(dtds.addMeasurement(tfyInfo));
		QCOMPARE(dtds.measurementCount(), 2);
		QVERIFY(dtds.addMeasurement(i0Info));
		QVERIFY(dtds.addMeasurement(sddInfo));
		QVERIFY(dtds.addMeasurement(xosInfo));
		QCOMPARE(dtds.measurementCount(), 5);
		QVERIFY(dtds.addMeasurement(xessInfo));
		QVERIFY(dtds.addMeasurement(uglyInfo));
		QCOMPARE(dtds.measurementCount(), 7);

		AMMeasurementInfo sameAsTeyInfo("tey", "something else", "counts");
		QVERIFY(!dtds.addMeasurement(teyInfo));
		QVERIFY(!dtds.addMeasurement(sameAsTeyInfo));
		QCOMPARE(dtds.measurementCount(), 7);

		QCOMPARE(dtds.idOfMeasurement("tey"), 0);
		QCOMPARE(dtds.idOfMeasurement("ugly"), 6);
		QCOMPARE(dtds.idOfMeasurement("sdd"), 3);
		QCOMPARE(dtds.idOfMeasurement("not in here"), -1);

		QCOMPARE(dtds.measurementAt(1).name, tfyInfo.name);
		QCOMPARE(dtds.measurementAt(1).description, tfyInfo.description);
		QCOMPARE(dtds.measurementAt(1).units, tfyInfo.units);

		QCOMPARE(dtds.measurementAt(5).name, xessInfo.name);
		QCOMPARE(dtds.measurementAt(5).description, xessInfo.description);
		QCOMPARE(dtds.measurementAt(5).units, xessInfo.units);
		QCOMPARE(dtds.measurementAt(5).axes.at(0).name, xessInfo.axes.at(0).name);
		QCOMPARE(dtds.measurementAt(5).axes.at(0).description, xessInfo.axes.at(0).description);
		QCOMPARE(dtds.measurementAt(5).axes.at(0).size, xessInfo.axes.at(0).size);
		QCOMPARE(dtds.measurementAt(5).axes.at(1).name, xessInfo.axes.at(1).name);
		QCOMPARE(dtds.measurementAt(5).axes.at(1).description, xessInfo.axes.at(1).description);
		QCOMPARE(dtds.measurementAt(5).axes.at(1).size, xessInfo.axes.at(1).size);


		AMnDIndex curSize = dtds.scanSize();
		qDebug() << "[1] nDIndex for axes: " << curSize.i();

		AMAxisInfo scanAxis2("temperature", 10, "Sample Temperature", "K");
		QVERIFY(dtds.addScanAxis(scanAxis2));

		QCOMPARE(dtds.idOfScanAxis("energy"), 0);
		QCOMPARE(dtds.idOfScanAxis(scanAxis.name), 0);
		QCOMPARE(dtds.idOfScanAxis("temperature"), 1);
		QCOMPARE(dtds.scanAxisAt(1).name, scanAxis2.name);
		QCOMPARE(dtds.scanAxisAt(1).description, scanAxis2.description);
		// QCOMPARE(dtds.scanAxisAt(1).size, 0); //regardless of incoming size, axes get sized to 0 in an empty tree (NO... sized to 1 if they are parent axes)
		QCOMPARE(dtds.scanAxesCount(), 2);


		qDebug() << "\n\n\nADDED SCAN AXIS\n\n\n";
		curSize = dtds.scanSize();
		qDebug() << "[2] nDIndex for axes: " << curSize.i() << curSize.j();

		AMAxisInfo scanAxis3("exitslitsize", 20, "Exit Slit Size", "um");
		QVERIFY(dtds.addScanAxis(scanAxis3));
		QCOMPARE(dtds.idOfScanAxis(scanAxis3.name), 2);
		QCOMPARE(dtds.scanAxisAt(2).name, scanAxis3.name);
		QCOMPARE(dtds.scanAxisAt(2).description, scanAxis3.description);
		// QCOMPARE(dtds.scanAxisAt(2).size, 0); //regardless of incoming size, axes get sized to 0 in an empty tree (NO... sized to 1 if they are parent axes)
		QCOMPARE(dtds.scanAxesCount(), 3);

		qDebug() << "\n\n\nADDED SCAN AXIS\n\n\n";

		AMAxisInfo scanAxis3Imposter("exitslitsize", 25, "Not the same", "either");
		QVERIFY(!dtds.addScanAxis(scanAxis2));
		QVERIFY(!dtds.addScanAxis(scanAxis3Imposter));
		QCOMPARE(dtds.scanAxesCount(), 3);

		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);


		QVERIFY(dtds.beginInsertRows(2));

		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);
		AMnDIndex curIndex(0, 0, 0);
		AMnDIndex scalarMeasurement;
		AMnDIndex d1Measurement(0);
		AMnDIndex d2Measurement(0,0);
		AMnDIndex d3Measurement(0,0,0);
		double dArray[1];
		dArray[0] = 12.27;
		int iArray[1];
		iArray[0] = 99;
//		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("tey"), scalarMeasurement, 12.27));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("tey"), dArray, 1));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("tfy"), scalarMeasurement, 1.001));
//		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("I0"), scalarMeasurement, 99));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("I0"), iArray, 1));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("sdd"), d1Measurement, 36));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("xos"), d1Measurement, 1023));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("xess"), d2Measurement, 12));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("ugly"), d3Measurement, 62.2));

		double retVal = dtds.value(curIndex, dtds.idOfMeasurement("tey"), scalarMeasurement);
		QCOMPARE(retVal, 12.27);
		retVal = dtds.value(curIndex, dtds.idOfMeasurement("tfy"), scalarMeasurement);
		QCOMPARE(retVal, 1.001);
		int retVali = dtds.value(curIndex, dtds.idOfMeasurement("I0"), scalarMeasurement);
		QCOMPARE(retVali, 99);
		retVali = dtds.value(curIndex, dtds.idOfMeasurement("sdd"), d1Measurement);
		QCOMPARE(retVali, 36);
		retVali = dtds.value(curIndex, dtds.idOfMeasurement("xos"), d1Measurement);
		QCOMPARE(retVali, 1023);
		retVali = dtds.value(curIndex, dtds.idOfMeasurement("xess"), d2Measurement);
		QCOMPARE(retVali, 12);
		retVal = dtds.value(curIndex, dtds.idOfMeasurement("ugly"), d3Measurement);
		QCOMPARE(retVal, 62.2);

		QVERIFY(dtds.setAxisValue(dtds.idOfScanAxis("exitslitsize"), 0, 25.0));
		retVal = dtds.axisValue(dtds.idOfScanAxis("exitslitsize"), 0);
		QCOMPARE(retVal, 25.0);
		QVERIFY(dtds.setAxisValue(dtds.idOfScanAxis("temperature"), 0, 76.7));
		retVal = dtds.axisValue(dtds.idOfScanAxis("temperature"), 0);
		QCOMPARE(retVal, 76.7);
		QVERIFY(dtds.setAxisValue(dtds.idOfScanAxis("energy"), 0, 282.5));
		retVal = dtds.axisValue(dtds.idOfScanAxis("energy"), 0);
		QCOMPARE(retVal, 282.5);

		int *intArray = (int*)malloc(1280*1024*sizeof(int));
		for(int x = 0; x < 1280*1024; x++)
			intArray[x] = x;
		double *doubleArray = (double*)malloc(512*sizeof(double));
		for(int x = 0; x < 512; x++)
			doubleArray[x] = x + 0.01*x;
		int *int2Array = (int*)malloc(100*180*360*sizeof(int));
		for(int x = 0; x < 100*180*360; x++)
			int2Array[x] = x;
		double *double2Array = (double*)malloc(100*180*360*sizeof(double));
		for(int x = 0; x < 100*180*360; x++)
			double2Array[x] = ((double)x) + 0.01;
		dtds.setValue(curIndex, dtds.idOfMeasurement("xos"), doubleArray, 512);
		dtds.setValue(curIndex, dtds.idOfMeasurement("ugly"), double2Array, 100*180*360);

		AMnDIndex check3Index(0,0,302);
		double ans = dtds.value(curIndex, dtds.idOfMeasurement("ugly"), check3Index);
		qDebug() << ans;

		AMnDIndex check1Index(124);
		ans = dtds.value(curIndex, dtds.idOfMeasurement("xos"), check1Index);
		qDebug() << ans;

		dtds.endInsertRows();

		AMnDIndex scanInsertIndex(0, 0, 1);
		AMnDIndex scanRetrieveIndex(0, 0, 1);
		qDebug() << "\n\n\n";
		for(int x=0; x<3; x++){
			if(dtds.beginInsertRows(0, 1, x))
				;
			//else
			//	qDebug() << "No need for x = " << x;
			dtds.endInsertRows();
			for(int y=0; y<4; y++){

				dtds.endInsertRows();
				for(int z=0; z<2; z++){

					dtds.endInsertRows();
					if(z == 1 && y == 0 && x == 0){
						dtds.setValue(scanInsertIndex, dtds.idOfMeasurement("tey"), AMnDIndex(), 12.2);
						AMNumber retVal = dtds.value(scanRetrieveIndex, dtds.idOfMeasurement("tey"), AMnDIndex());
						if(retVal.state() == AMNumber::Null)
							qDebug() << "Null valued number";
						else
							qDebug() << "Set that mud trucka as " << double(retVal);
						dtds.setAxisValue(0, 0, 12.27);
						retVal = dtds.axisValue(0,0);
						if(retVal.state() == AMNumber::Null)
							qDebug() << "Axis is Null valued number";
						else
							qDebug() << "Set that axis mud trucka as " << double(retVal);
					}
				}
			}
		}

		dtds.beginInsertRows(2, 1, 2);
		dtds.endInsertRows();

		AMAxisInfo scanAxis4("polarization", 10, "Polarization", "degrees");
		QVERIFY(dtds.addScanAxis(scanAxis4));
		QCOMPARE(dtds.idOfScanAxis(scanAxis4.name), 3);
		QCOMPARE(dtds.scanAxisAt(3).name, scanAxis4.name);
		QCOMPARE(dtds.scanAxisAt(3).description, scanAxis4.description);
		QCOMPARE(dtds.scanAxisAt(3).size, 1); //Set as 1 because other data already exists so data store is not empty
		QCOMPARE(dtds.scanAxesCount(), 4);



		QVERIFY(dtds.axisValue(3, 0).state() == AMNumber::Null);


		// qDebug() << "\n\n\nEND TESTING\n\n\n";

	}
*/
/*
	void testDataTreeDataStore() {
		qDebug() << "\n\n\n DAVID STARTS HERE \n\nTesting data tree data store\n\n\n";


		AMMeasurementInfo teyInfo("tey", "Total Electron Yield", "amps");
		AMMeasurementInfo tfyInfo("tfy", "Total Fluoresence Yield", "amps");
		AMMeasurementInfo i0Info("I0", "I0", "amps");

		AMAxisInfo sddEVAxisInfo("energy", 1024, "SDD Energy", "eV");
		QList<AMAxisInfo> sddAxes;
		sddAxes << sddEVAxisInfo;
		AMMeasurementInfo sddInfo("sdd", "Silicon Drift Detector", "counts", sddAxes);

		AMAxisInfo xosWavelengthAxisInfo("wavelength", 512, "XOS Wavelength", "nm");
		QList<AMAxisInfo> xosAxes;
		xosAxes << xosWavelengthAxisInfo;
		AMMeasurementInfo xosInfo("xos", "XEOL Optical Spectrometer", "counts", xosAxes);

		AMAxisInfo xessXAxisInfo("x", 1280, "x pixel", "index");
		AMAxisInfo xessYAxisInfo("y", 1024, "y pixel", "index");
		QList<AMAxisInfo> xessAxes;
		xessAxes << xessXAxisInfo << xessYAxisInfo;
		AMMeasurementInfo xessInfo("xess", "XES Spectrometer", "counts", xessAxes);

		AMAxisInfo uglyRAxisInfo("r", 100, "Radial Distance", "percent");
		AMAxisInfo uglyPhiAxisInfo("phi", 180, "Phi Angle", "degrees");
		AMAxisInfo uglyThetaAxisInfo("theta", 360, "Theta Distance", "degrees");
		QList<AMAxisInfo> uglyAxes;
		uglyAxes << uglyRAxisInfo << uglyPhiAxisInfo << uglyThetaAxisInfo;
		AMMeasurementInfo uglyInfo("ugly", "Some Ugly Detector", "events", uglyAxes);

		AMAxisInfo scanAxis("energy", 1000, "Beamline Energy", "eV");

		AMDataTreeDataStore dtds(scanAxis, this);
		QVERIFY(dtds.addMeasurement(teyInfo));
		QVERIFY(dtds.addMeasurement(tfyInfo));
		QCOMPARE(dtds.measurementCount(), 2);
		QVERIFY(dtds.addMeasurement(i0Info));
		QVERIFY(dtds.addMeasurement(sddInfo));
		QVERIFY(dtds.addMeasurement(xosInfo));
		QCOMPARE(dtds.measurementCount(), 5);
		QVERIFY(dtds.addMeasurement(xessInfo));
		QVERIFY(dtds.addMeasurement(uglyInfo));
		QCOMPARE(dtds.measurementCount(), 7);

		AMMeasurementInfo sameAsTeyInfo("tey", "something else", "counts");
		QVERIFY(!dtds.addMeasurement(teyInfo));
		QVERIFY(!dtds.addMeasurement(sameAsTeyInfo));
		QCOMPARE(dtds.measurementCount(), 7);

		QCOMPARE(dtds.idOfMeasurement("tey"), 0);
		QCOMPARE(dtds.idOfMeasurement("ugly"), 6);
		QCOMPARE(dtds.idOfMeasurement("sdd"), 3);
		QCOMPARE(dtds.idOfMeasurement("not in here"), -1);

		QCOMPARE(dtds.measurementAt(1).name, tfyInfo.name);
		QCOMPARE(dtds.measurementAt(1).description, tfyInfo.description);
		QCOMPARE(dtds.measurementAt(1).units, tfyInfo.units);

		QCOMPARE(dtds.measurementAt(5).name, xessInfo.name);
		QCOMPARE(dtds.measurementAt(5).description, xessInfo.description);
		QCOMPARE(dtds.measurementAt(5).units, xessInfo.units);
		QCOMPARE(dtds.measurementAt(5).axes.at(0).name, xessInfo.axes.at(0).name);
		QCOMPARE(dtds.measurementAt(5).axes.at(0).description, xessInfo.axes.at(0).description);
		QCOMPARE(dtds.measurementAt(5).axes.at(0).size, xessInfo.axes.at(0).size);
		QCOMPARE(dtds.measurementAt(5).axes.at(1).name, xessInfo.axes.at(1).name);
		QCOMPARE(dtds.measurementAt(5).axes.at(1).description, xessInfo.axes.at(1).description);
		QCOMPARE(dtds.measurementAt(5).axes.at(1).size, xessInfo.axes.at(1).size);

		AMDataTree *dt = dtds.dataTree();

		dataTreeColumnsPuke(dt);
		AMnDIndex curSize = dtds.scanSize();
		qDebug() << "[1] nDIndex for axes: " << curSize.i();

		AMAxisInfo scanAxis2("temperature", 10, "Sample Temperature", "K");
		QVERIFY(dtds.addScanAxis(scanAxis2));

		QCOMPARE(dtds.idOfScanAxis("energy"), 0);
		QCOMPARE(dtds.idOfScanAxis(scanAxis.name), 0);
		QCOMPARE(dtds.idOfScanAxis("temperature"), 1);
		QCOMPARE(dtds.scanAxisAt(1).name, scanAxis2.name);
		QCOMPARE(dtds.scanAxisAt(1).description, scanAxis2.description);
		QCOMPARE(dtds.scanAxisAt(1).size, 0); //regardless of incoming size, axes get sized to 0 in an empty tree
		QCOMPARE(dtds.scanAxesCount(), 2);

		dt = dtds.dataTree();
		qDebug() << "\n\n\nADDED SCAN AXIS\n\n\n";
		dataTreeColumnsPuke(dt);
		curSize = dtds.scanSize();
		qDebug() << "[2] nDIndex for axes: " << curSize.i() << curSize.j();

		AMAxisInfo scanAxis3("exitslitsize", 20, "Exit Slit Size", "um");
		QVERIFY(dtds.addScanAxis(scanAxis3));
		QCOMPARE(dtds.idOfScanAxis(scanAxis3.name), 2);
		QCOMPARE(dtds.scanAxisAt(2).name, scanAxis3.name);
		QCOMPARE(dtds.scanAxisAt(2).description, scanAxis3.description);
		QCOMPARE(dtds.scanAxisAt(2).size, 0); //regardless of incoming size, axes get sized to 0 in an empty tree
		QCOMPARE(dtds.scanAxesCount(), 3);

		dt = dtds.dataTree();
		qDebug() << "\n\n\nADDED SCAN AXIS\n\n\n";
		dataTreeColumnsPuke(dt);

		AMAxisInfo scanAxis3Imposter("exitslitsize", 25, "Not the same", "either");
		QVERIFY(!dtds.addScanAxis(scanAxis2));
		QVERIFY(!dtds.addScanAxis(scanAxis3Imposter));
		QCOMPARE(dtds.scanAxesCount(), 3);

		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);

		qDebug() << "PUKING DIMENSIONS";
		dtds.dataStoreDimensionsPuke();
		QVERIFY(dtds.beginInsertRows(2));
		dtds.dataStoreDimensionsPuke();
		/*
		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);
		AMnDIndex curIndex(0, 0, 0);
		AMnDIndex scalarMeasurement;
		AMnDIndex d1Measurement(0);
		AMnDIndex d2Measurement(0,0);
		AMnDIndex d3Measurement(0,0,0);
		double dArray[1];
		dArray[0] = 12.27;
		int iArray[1];
		iArray[0] = 99;
//		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("tey"), scalarMeasurement, 12.27));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("tey"), dArray, 1));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("tfy"), scalarMeasurement, 1.001));
//		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("I0"), scalarMeasurement, 99));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("I0"), iArray, 1));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("sdd"), d1Measurement, 36));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("xos"), d1Measurement, 1023));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("xess"), d2Measurement, 12));
		QVERIFY(dtds.setValue(curIndex, dtds.idOfMeasurement("ugly"), d3Measurement, 62.2));

		double retVal = dtds.value(curIndex, dtds.idOfMeasurement("tey"), scalarMeasurement);
		QCOMPARE(retVal, 12.27);
		retVal = dtds.value(curIndex, dtds.idOfMeasurement("tfy"), scalarMeasurement);
		QCOMPARE(retVal, 1.001);
		int retVali = dtds.value(curIndex, dtds.idOfMeasurement("I0"), scalarMeasurement);
		QCOMPARE(retVali, 99);
		retVali = dtds.value(curIndex, dtds.idOfMeasurement("sdd"), d1Measurement);
		QCOMPARE(retVali, 36);
		retVali = dtds.value(curIndex, dtds.idOfMeasurement("xos"), d1Measurement);
		QCOMPARE(retVali, 1023);
		retVali = dtds.value(curIndex, dtds.idOfMeasurement("xess"), d2Measurement);
		QCOMPARE(retVali, 12);
		retVal = dtds.value(curIndex, dtds.idOfMeasurement("ugly"), d3Measurement);
		QCOMPARE(retVal, 62.2);

		QVERIFY(dtds.setAxisValue(dtds.idOfScanAxis("exitslitsize"), 0, 25.0));
		retVal = dtds.axisValue(dtds.idOfScanAxis("exitslitsize"), 0);
		QCOMPARE(retVal, 25.0);
		QVERIFY(dtds.setAxisValue(dtds.idOfScanAxis("temperature"), 0, 76.7));
		retVal = dtds.axisValue(dtds.idOfScanAxis("temperature"), 0);
		QCOMPARE(retVal, 76.7);
		QVERIFY(dtds.setAxisValue(dtds.idOfScanAxis("energy"), 0, 282.5));
		retVal = dtds.axisValue(dtds.idOfScanAxis("energy"), 0);
		QCOMPARE(retVal, 282.5);
		*/


		/*
		int *intArray = (int*)malloc(1280*1024*sizeof(int));
		for(int x = 0; x < 1280*1024; x++)
			intArray[x] = x;
		double *doubleArray = (double*)malloc(512*sizeof(double));
		for(int x = 0; x < 512; x++)
			doubleArray[x] = x + 0.01*x;
		int *int2Array = (int*)malloc(100*180*360*sizeof(int));
		for(int x = 0; x < 100*180*360; x++)
			int2Array[x] = x;
		double *double2Array = (double*)malloc(100*180*360*sizeof(double));
		for(int x = 0; x < 100*180*360; x++)
			double2Array[x] = ((double)x) + 0.01;
		dtds.setValue(curIndex, dtds.idOfMeasurement("xos"), doubleArray, 512);
		dtds.setValue(curIndex, dtds.idOfMeasurement("ugly"), double2Array, 100*180*360);

		AMnDIndex check3Index(0,0,302);
		double ans = dtds.value(curIndex, dtds.idOfMeasurement("ugly"), check3Index);
		qDebug() << ans;

		AMnDIndex check1Index(124);
		ans = dtds.value(curIndex, dtds.idOfMeasurement("xos"), check1Index);
		qDebug() << ans;
		*/

		/*
		dtds.endInsertRows();

		AMnDIndex scanInsertIndex(0, 0, 1);
		AMnDIndex scanRetrieveIndex(0, 0, 1);
		qDebug() << "\n\n\n";
		for(int x=0; x<3; x++){
			if(dtds.beginInsertRows(0, 1, x))
				dtds.dataStoreDimensionsPuke();
			//else
			//	qDebug() << "No need for x = " << x;
			dtds.endInsertRows();
			for(int y=0; y<4; y++){
				if(dtds.beginInsertRows(1, 1, y))
					dtds.dataStoreDimensionsPuke();
				//else
				//	qDebug() << "No need for y = " << y;
				dtds.endInsertRows();
				for(int z=0; z<2; z++){
					if(dtds.beginInsertRows(2, 1, z))
						dtds.dataStoreDimensionsPuke();
					//else
					//	qDebug() << "No need for z = " << z;
					dtds.endInsertRows();
					if(z == 1 && y == 0 && x == 0){
						dtds.setValue(scanInsertIndex, dtds.idOfMeasurement("tey"), AMnDIndex(), 12.2);
						AMNumber retVal = dtds.value(scanRetrieveIndex, dtds.idOfMeasurement("tey"), AMnDIndex());
						if(retVal.state() == AMNumber::Null)
							qDebug() << "Null valued number";
						else
							qDebug() << "Set that mud trucka as " << double(retVal);
						dtds.setAxisValue(0, 0, 12.27);
						retVal = dtds.axisValue(0,0);
						if(retVal.state() == AMNumber::Null)
							qDebug() << "Axis is Null valued number";
						else
							qDebug() << "Set that axis mud trucka as " << double(retVal);
					}
				}
			}
		}

		dtds.beginInsertRows(2, 1, 2);
		dtds.dataStoreDimensionsPuke();
		dtds.endInsertRows();

		AMAxisInfo scanAxis4("polarization", 10, "Polarization", "degrees");
		QVERIFY(dtds.addScanAxis(scanAxis4));
		QCOMPARE(dtds.idOfScanAxis(scanAxis4.name), 3);
		QCOMPARE(dtds.scanAxisAt(3).name, scanAxis4.name);
		QCOMPARE(dtds.scanAxisAt(3).description, scanAxis4.description);
		QCOMPARE(dtds.scanAxisAt(3).size, 1); //Set as 1 because other data already exists so data store is not empty
		QCOMPARE(dtds.scanAxesCount(), 4);

		qDebug() << "\n\n\n";
		dtds.dataStoreDimensionsPuke();

		if(dtds.axisValue(3, 0).state() == AMNumber::Null)
			qDebug() << "Good, it started as null valued";
		else
			qDebug() << "Ah sheit! Dat is not good.";

		/*
		dt->append(12);
		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);
		dt->deeper(0,0)->append(20);
		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);
		dt->deeper(0,0)->deeper(0,0)->append(100);
		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);
		dt->deeper(0,0)->deeper(0,0)->append(101);
		curSize = dtds.scanSize();
		qDebug() << "[3] nDIndex for axes: " << curSize.i() << curSize.j() << curSize.k();
		qDebug() << "[3] or: " << dtds.scanSize(0) << dtds.scanSize(1) << dtds.scanSize(2);
		*/
/*
		qDebug() << "\n\n\nEND TESTING\n\n\n";
	}

*/


	/// Test inserts of DbObjects into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMDbObject_retrieveStatically() {
		AMDbObject dbo;

		// generate unique name, and set properties:
		//qDebug() << "Generating DbObject with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestDbObject" + uniqueName);


		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		//qDebug() << "Testing DbObject insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject dbo2;
		dbo2.loadFromDb(AMDatabase::userdb(), dbo.id());
		//qDebug() << "Retrieving DbObject out of database: comparing all parameters with original:";
		QCOMPARE(dbo2.id(), dbo.id());
		QCOMPARE(dbo2.name(), dbo.name());

	}

	/// Test inserts of DbObjects into database, and confirm all values loaded back with dynamic loader:
	void insertAMDbObject_retrieveDynamically() {
		AMDbObject dbo;

		// generate unique name, and set properties:
		//qDebug() << "Generating DbObject with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestDbObject" + uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		//qDebug() << "Testing DbObject insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject* dbo2 = 0;
		dbo2 = AMDbObjectSupport::createAndLoadObjectAt(AMDatabase::userdb(), dbo.dbTableName(), dbo.id());


		//qDebug() << "Confirming object was dynamically loaded: pointer is: " << dbo2;
		QVERIFY(dbo2 != 0);

		//qDebug() << "Type of retrieved object should be AMDbObject: " << typeString;
		QCOMPARE(dbo2->type(), QString("AMDbObject"));

		//qDebug() << "Retrieved DbObject out of database: comparing all parameters with original:";
		QCOMPARE(dbo2->id(), dbo.id());
		QCOMPARE(dbo2->name(), dbo.name());
	}

	/// Test inserts of AMScan into the database, and confirm all values loaded back with DbObject::loadFromDb().
	void insertAMScan_retrieveStatically() {
		AMScan dbo;

		// generate unique name, and set properties:
		//qDebug() << "Generating AMScan with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);
		/// \todo dbo.setSampleName("mySampleName" + uniqueName);
		dbo.setNotes("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		//qDebug() << "Testing AMScan insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMScan dbo2;
		dbo2.loadFromDb(AMDatabase::userdb(), dbo.id());
		//qDebug() << "Retrieving AMScan out of database: comparing all parameters with original:";
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
		//qDebug() << "Generating AMScan with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(42);
		/// \todo dbo.setSampleName("mySampleName" + uniqueName);
		dbo.setNotes("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		//qDebug() << "Testing AMScan insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		// Load same scan back:
		AMDbObject* dbo2d = 0;
		QString typeString;
		dbo2d = AMDbObjectSupport::createAndLoadObjectAt(dbo.database(), dbo.dbTableName(), dbo.id());

		//qDebug() << "Confirming object was dynamically loaded: pointer is: " << dbo2d;
		QVERIFY(dbo2d != 0);

		//qDebug() << "Type of retrieved object should be AMScan: " << typeString;
		QCOMPARE(dbo2d->type(), QString("AMScan"));

		//qDebug() << "Confirming dynamically loaded object is an AMScan: pointer is: " << dbo2d;
		AMScan* dbo2 = qobject_cast<AMScan*>(dbo2d);
		QVERIFY(dbo2 != 0);

		// Load same scan back:
		//qDebug() << "Retrieving AMScan out of database: comparing all parameters with original:";
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
		//qDebug() << "Generating AMScan with unique properties.";
		QString uniqueName = QDateTime::currentDateTime().toString("ddddMMddhh:mm:ss.zzz");
		dbo.setName("myTestAMScan" + uniqueName);
		dbo.setDateTime(QDateTime::currentDateTime());
		dbo.setNumber(QDateTime::currentDateTime().toTime_t());
		/// \todo dbo.setSampleName("mySampleName" + uniqueName);
		dbo.setNotes("my Comments\n"+uniqueName);

		// Insert into db:
		*AMDatabase::userdb() << dbo;

		// Was it inserted succesfully?
		//qDebug() << "Testing AMScan insert into database: id should not be 0:" << dbo.id();
		QVERIFY(dbo.id() > 0);	// check that insert succeeded.

		QList<int> lr;
		// Check: all columns: scans matching should be 1:
		//qDebug() << "Checking objectsMatching finds one matching for each column.";
		//lr = AMDatabase::userdb()->objectsMatching("id", dbo.id());
		//QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->objectsMatching(dbo.dbTableName(), "name", dbo.name());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->objectsMatching(dbo.dbTableName(), "number", dbo.number());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->objectsMatching(dbo.dbTableName(), "sampleId", dbo.sampleId());
		QVERIFY(lr.count() > 1);
		lr = AMDatabase::userdb()->objectsMatching(dbo.dbTableName(), "notes", dbo.notes());
		QCOMPARE(lr.count(), 1);
		lr = AMDatabase::userdb()->objectsMatching(dbo.dbTableName(), "dateTime", dbo.dateTime());
		/// \todo check for 1-minute tolerance on date-time... This fails...
		QCOMPARE(lr.count(), 1);




	}




	/// test loading an AMXASScan from legacy SGM data. Also tests creation of default channels inside SGM2004FileLoader::loadFromFile()

	void loadAMXASScan() {
		AMXASScan s1;
		SGM2004FileLoader s1Loader(&s1);
		/// \todo move this into proper storage location in data dir.
		QString fileName = AMUserSettings::userDataFolder + "testScriptData/sgm001.dat";
		//		qDebug() << "loading sgm data from file and checking for proper read:" << fileName;
		QVERIFY(s1Loader.loadFromFile(fileName));
		QCOMPARE(s1.scanSize(0), int(301));
		QCOMPARE(s1.notes(), QString("0.916667"));	// NOTE: this test fails, because we're currently putting the Grating and Integration Time inside the comment field (for lack of a better place) Eventually, the Grating and Integration time should become part of the scan configuration, or beamline state.
		QCOMPARE(s1.dateTime().toTime_t(), uint(1285706567));
		//qDebug() << "s1 raw data columns ('detectors')" << s1.detectors();


		int tey_n, tfy_n, tey, tfy, io;
		QVERIFY((tey = s1.rawDataSources()->indexOf("tey")) != -1);
		QVERIFY((tfy = s1.rawDataSources()->indexOf("tfy")) != -1);
		QVERIFY((io = s1.rawDataSources()->indexOf("I0")) != -1);
		QVERIFY((tey_n = s1.analyzedDataSources()->indexOf("tey_n")) != -1);
		QVERIFY((tfy_n = s1.analyzedDataSources()->indexOf("tfy_n")) != -1);
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
		QString fileName = AMUserSettings::userDataFolder + "testScriptData/sgm001.dat";
		//qDebug() << "loading sgm data from file and checking for proper read:" << fileName;
		QVERIFY(s1Loader.loadFromFile(fileName));

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
		QVERIFY(s1.analyzedDataSources()->indexOf("tey_n") == 0);
		QVERIFY(s1.analyzedDataSources()->indexOf("tfy_n") == 1);

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
		AMDbObjectSupport::registerClass<AMTestDbObject>();

		AMTestDbObject* t1 = new AMTestDbObject();
		t1->storeToDb(AMDatabase::userdb());

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
		//this might actually be (and turns out to happen). Could luckily allocate to just-deleted spot of s. QVERIFY(t1->myScan() != s);
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

		t2->storeToDb(AMDatabase::userdb());


		QCOMPARE(t1->numScans(), 0);
		t1->loadFromDb(AMDatabase::userdb(), t2->id());	// should create brand new objects; t1 didn't have three in the list.

		// catches a bug where member variable locations weren't being stored in the db if the member object wasn't modified. If it's not modified and already in the db, we don't need to re-storeToDb() it, but we still need remember that we own it, and remember where it is.
		QVERIFY(t1->myScan());
		QCOMPARE(t1->myScan()->name(), QString("special scan test name! With secret sauce."));

		QCOMPARE(t1->numScans(), 3);
		AMDbObjectList checkList = t1->myDbObjects();
		QCOMPARE(checkList.count(), 3);
		QCOMPARE(checkList.at(0)->name(), QString("AMDbObject: object1"));
		QCOMPARE(checkList.at(1)->name(), QString("AMScan: object2"));
		QCOMPARE(checkList.at(2)->name(), QString("AMXASScan: object3"));

		// how do we know that these are new objects? Could try accessing o1, o2, o3, but they should have been deleted. A crash would indicate success ;p  Hmmm...


		t2->myDbObjects().at(0)->setName("Going going gone...");
		QCOMPARE(t2->myDbObjects().at(0)->name(), QString("Going going gone..."));

		t2->loadFromDb(t2->database(), t2->id());	// first version of loadFromDb()... all three exist already and match in type... should not call setProperty().  Verify that "AMTestDbObject: creating/loading brand new scans" only appears twice in the test log.
		QCOMPARE(t2->myDbObjects().at(0)->name(), QString("AMDbObject: object1")); // should be restored from last saved state...

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

		QVERIFY(s1.indexOf("0") == 0);
		QVERIFY(s1.indexOf("1") == 1);
		QVERIFY(s1.indexOf("2") == 2);
		QVERIFY(s1.indexOf("3") == 3);
		QVERIFY(s1.indexOf("4") == 4);

		// test remove in middle:
		s1.remove(2);
		QVERIFY(s1.count() == 4);

		QVERIFY(s1.indexOf("0") == 0);
		QVERIFY(s1.indexOf("1") == 1);
		QVERIFY(s1.indexOf("2") == -1);
		QVERIFY(s1.indexOf("3") == 2);	// index moves down
		QVERIFY(s1.indexOf("4") == 3);

		QVERIFY(s1.at(0) == 0);
		QVERIFY(s1.at(1) == 1);
		QVERIFY(s1.at(2) == 3);	// value moves down
		QVERIFY(s1.at(3) == 4);


		// check insert inside:
		s1.insert(1, 88, "eighty-eight");

		QVERIFY(s1.count() == 5);

		QVERIFY(s1.indexOf("0") == 0);
		QVERIFY(s1.indexOf("eighty-eight") == 1);
		QVERIFY(s1.indexOf("1") == 2);	// index moves up; was inserted in front
		QVERIFY(s1.indexOf("2") == -1);
		QVERIFY(s1.indexOf("3") == 3);
		QVERIFY(s1.indexOf("4") == 4);

		QVERIFY(s1.at(0) == 0);
		QVERIFY(s1.at(1) == 88);
		QVERIFY(s1.at(2) == 1);	// value moves down
		QVERIFY(s1.at(3) == 3);
		QVERIFY(s1.at(4) == 4);

		QVERIFY(s1.replace(1, 77, "seventyseven") == 88);
		QVERIFY(s1.indexOf("eighty-eight") == -1);
		QVERIFY(s1.indexOf("seventyseven") == 1);

		s1.clear();
		QVERIFY(s1.indexOf("seventyseven") == -1);
		QVERIFY(s1.count() == 0);

	}



};
