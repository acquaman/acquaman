#include "AMSettings.h"
#include <QtTest/QtTest>
#include "beamline/SGMBeamline.h"
#include "dataman/AMDatabase.h"
#include "dataman/AMXASScan.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"

#include "dataman/AMFirstTimeController.h"
#include "AMErrorMonitor.h"

class TestAcquaman: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase()
	{
		qDebug() << "Starting up SGM";
		SGMBeamline::sgm();
		qDebug() << "Created SGM";
		bool sgmConnected = false;
		while(!sgmConnected){
			if(SGMBeamline::sgm()->isConnected())
				sgmConnected = true;
			QTest::qWait(250);
		}
		QVERIFY(SGMBeamline::sgm()->isConnected());

		/*
		qDebug() << "Prior to waiting";
		QTest::qWait(2000);
		qDebug() << "About to action grating";
		SGMBeamline::sgm()->gratingAction()->start();
		qDebug() << "After action grating";
		QTest::qWait(5000);
		SGMBeamline::sgm()->startTransfer();
		qDebug() << "After final wait";
		*/
	}

	void cleanupTestCase()
	{

	}

	void testAMRegions()
	{
		AMRegionsList *rl1 = new AMRegionsList(this);
		AMRegionsList *rl2 = new AMRegionsList(this);
		// They should initialize to empty
		QCOMPARE(rl1->count(), 0);
		QCOMPARE(rl2->count(), 0);
		AMRegionsListModel *rlm1 = rl1->model();
		AMRegionsListModel *rlm2 = NULL;
		// First model should be initialized but be empty
		QCOMPARE(rlm1->rowCount(QModelIndex()), 0);
		QCOMPARE((int)rlm2, NULL);

		double start = 100;
		double delta = 0.5;
		double end = 200;
		// Can't append until default control defined
		QVERIFY(!rl1->appendRegion(start, delta, end));
		QVERIFY(!rl2->appendRegion(start, delta, end));

		// Set and confirm default control
		rl1->setDefaultControl(SGMBeamline::sgm()->energy());
		rl2->setDefaultControl(SGMBeamline::sgm()->energy());
		QCOMPARE(rl1->defaultControl(), SGMBeamline::sgm()->energy());
		QCOMPARE(rl2->defaultControl(), SGMBeamline::sgm()->energy());

		// Can't append, value out of range
		QVERIFY(!rl1->appendRegion(start, delta, end));
		QVERIFY(!rl2->appendRegion(start, delta, end));

		// Can't add, region out of range
		QVERIFY(!rl1->addRegion(2, start, delta, end));
		QVERIFY(!rl2->addRegion(2, start, delta, end));

		start = 250.1;
		end = 300;

		// Append and confirm
		QVERIFY(rl1->appendRegion(start, delta, end));
		QVERIFY(rl2->appendRegion(start, delta, end));
		QCOMPARE(rl1->count(), 1);
		QCOMPARE(rl2->count(), 1);
		QCOMPARE(rl1->start(rl1->count()-1), start);
		QCOMPARE(rl1->delta(rl1->count()-1), delta);
		QCOMPARE(rl1->end(rl1->count()-1), end);
		QCOMPARE(rl2->start(rl2->count()-1), start);
		QCOMPARE(rl2->delta(rl2->count()-1), delta);
		QCOMPARE(rl2->end(rl2->count()-1), end);

		// Check model pointer
		QCOMPARE(rlm1->data(rlm1->index(rl1->count()-1, 1), Qt::DisplayRole).toDouble(), start);
		QCOMPARE(rlm1->data(rlm1->index(rl1->count()-1, 2), Qt::DisplayRole).toDouble(), delta);
		QCOMPARE(rlm1->data(rlm1->index(rl1->count()-1, 3), Qt::DisplayRole).toDouble(), end);

		// Test insertion
		QList<double> starts, deltas, ends;
		starts << start;
		deltas << delta;
		ends << end;
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

		delta = 0.1;
		start = end + delta;
		end = 350;
		QVERIFY(rl1->appendRegion(start, delta, end));
		QVERIFY(rl2->appendRegion(start, delta, end));
		starts << start;
		deltas << delta;
		ends << end;
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

		start = 200;
		delta = 1;
		end = 250;
		QVERIFY(rl1->addRegion(0, start, delta, end));
		QVERIFY(rl2->addRegion(0, start, delta, end));
		starts.prepend(start);
		deltas.prepend(delta);
		ends.prepend(end);
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

		start = 325;
		delta = 0.25;
		end = 700;
		QVERIFY(rl1->addRegion(2, start, delta, end));
		QVERIFY(rl2->addRegion(2, start, delta, end));
		starts.insert(2, start);
		deltas.insert(2, delta);
		ends.insert(2, end);
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

		// Test altering individual start/delta/end
		start = 201;
		delta = 0.4;
		end = 450;
		QVERIFY(rl1->setStart(0, start));
		QVERIFY(rl2->setStart(0, start));
		starts[0] = start;
		QVERIFY(rl1->setDelta(2, delta));
		QVERIFY(rl2->setDelta(2, delta));
		deltas[2] = delta;
		QVERIFY(rl1->setEnd(1, end));
		QVERIFY(rl2->setEnd(1, end));
		ends[1] = end;
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

		//Test deletion
		QVERIFY(rl1->deleteRegion(rl1->count()-1));
		QVERIFY(rl2->deleteRegion(rl2->count()-1));
		starts.removeAt(starts.count()-1);
		deltas.removeAt(deltas.count()-1);
		ends.removeAt(ends.count()-1);
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

		// Can't delete out of bounds
		QVERIFY(!rl1->deleteRegion(rl1->count()));
		QVERIFY(!rl2->deleteRegion(rl2->count()));

		QVERIFY(rl1->deleteRegion(2));
		QVERIFY(rl2->deleteRegion(2));
		starts.removeAt(2);
		deltas.removeAt(2);
		ends.removeAt(2);
		for(int x = 0; x < starts.count(); x++){
			QCOMPARE(rl1->start(x), starts.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 1), Qt::DisplayRole).toDouble(), starts.at(x));
			QCOMPARE(rl2->start(x), starts.at(x));
			QCOMPARE(rl1->delta(x), deltas.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 2), Qt::DisplayRole).toDouble(), deltas.at(x));
			QCOMPARE(rl2->delta(x), deltas.at(x));
			QCOMPARE(rl1->end(x), ends.at(x));
			QCOMPARE(rlm1->data(rlm1->index(x, 3), Qt::DisplayRole).toDouble(), ends.at(x));
			QCOMPARE(rl2->end(x), ends.at(x));
		}

	}

	void scanConfigurations()
	{
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		// List of regions should be created but empty
		QCOMPARE(sxsc->count(), 0);
		// SGM components should be initialized and be the same as the current beamline values
		QCOMPARE(sxsc->exitSlitGap(), SGMBeamline::sgm()->exitSlitGap()->value());
		QCOMPARE(sxsc->grating(), (SGMBeamline::sgmGrating)SGMBeamline::sgm()->grating()->value());
		QCOMPARE(sxsc->harmonic(), (SGMBeamline::sgmHarmonic)SGMBeamline::sgm()->harmonic()->value());
		QCOMPARE(sxsc->undulatorTracking(), (bool)SGMBeamline::sgm()->undulatorTracking()->value());
		QCOMPARE(sxsc->monoTracking(), (bool)SGMBeamline::sgm()->monoTracking()->value());
		QCOMPARE(sxsc->exitSlitTracking(), (bool)SGMBeamline::sgm()->exitSlitTracking()->value());
		// Should be using the SGM XAS Detectors, check that they are the same
		AMDetectorInfoSet *xasDetectors = SGMBeamline::sgm()->XASDetectors();
		QList<AMDetectorInfo*> xasDefaultDetectors;
		for(int x = 0; x < xasDetectors->count(); x++)
			if(xasDetectors->isDefaultAt(x))
				xasDefaultDetectors << xasDetectors->detectorAt(x);
		for(int x = 0; x < xasDefaultDetectors.count(); x++)
			QCOMPARE(sxsc->usingDetectors().at(x)->name(), xasDefaultDetectors.at(x)->name());
		// Should be using SGM Flux/Resolution ControlSet
		for(int x = 0; x < sxsc->fluxResolutionSet()->count(); x++)
			QCOMPARE(sxsc->fluxResolutionSet()->controlAt(x)->name(), SGMBeamline::sgm()->fluxResolutionSet()->controlAt(x)->name());
		// Should be using SGM Tracking ControlSet
		for(int x = 0; x < sxsc->trackingSet()->count(); x++)
			QCOMPARE(sxsc->trackingSet()->controlAt(x)->name(), SGMBeamline::sgm()->trackingSet()->controlAt(x)->name());
		QString fileName = "testFile.%03d.dat";
		QVERIFY(sxsc->setFileName(fileName));
		QCOMPARE(sxsc->fileName(), fileName);
		QVERIFY(sxsc->setFilePath(AMUserSettings::userDataFolder));
		QCOMPARE(sxsc->filePath(), AMUserSettings::userDataFolder);
	}

	void scanController()
	{
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		// List of regions should be created but empty
		QCOMPARE(sxsc->count(), 0);
		// SGM components should be initialized and be the same as the current beamline values
		QCOMPARE(sxsc->exitSlitGap(), SGMBeamline::sgm()->exitSlitGap()->value());
		QCOMPARE(sxsc->grating(), (SGMBeamline::sgmGrating)SGMBeamline::sgm()->grating()->value());
		QCOMPARE(sxsc->harmonic(), (SGMBeamline::sgmHarmonic)SGMBeamline::sgm()->harmonic()->value());
		QCOMPARE(sxsc->undulatorTracking(), (bool)SGMBeamline::sgm()->undulatorTracking()->value());
		QCOMPARE(sxsc->monoTracking(), (bool)SGMBeamline::sgm()->monoTracking()->value());
		QCOMPARE(sxsc->exitSlitTracking(), (bool)SGMBeamline::sgm()->exitSlitTracking()->value());
		// Should be using the SGM XAS Detectors, check that they are the same
		AMDetectorInfoSet *xasDetectors = SGMBeamline::sgm()->XASDetectors();
		QList<AMDetectorInfo*> xasDefaultDetectors;
		for(int x = 0; x < xasDetectors->count(); x++)
			if(xasDetectors->isDefaultAt(x))
				xasDefaultDetectors << xasDetectors->detectorAt(x);
		for(int x = 0; x < xasDefaultDetectors.count(); x++)
			QCOMPARE(sxsc->usingDetectors().at(x)->name(), xasDefaultDetectors.at(x)->name());
		QVERIFY(sxsc->setUsingPGT(1));
		// Should be using SGM Flux/Resolution ControlSet
		for(int x = 0; x < sxsc->fluxResolutionSet()->count(); x++)
			QCOMPARE(sxsc->fluxResolutionSet()->controlAt(x)->name(), SGMBeamline::sgm()->fluxResolutionSet()->controlAt(x)->name());
		// Should be using SGM Tracking ControlSet
		for(int x = 0; x < sxsc->trackingSet()->count(); x++)
			QCOMPARE(sxsc->trackingSet()->controlAt(x)->name(), SGMBeamline::sgm()->trackingSet()->controlAt(x)->name());
		QString fileName = "testFile.%03d.dat";
		QVERIFY(sxsc->setFileName(fileName));
		QCOMPARE(sxsc->fileName(), fileName);
		QVERIFY(sxsc->setFilePath(AMUserSettings::userDataFolder));
		QCOMPARE(sxsc->filePath(), AMUserSettings::userDataFolder);
		QVERIFY(sxsc->addRegion(0, 930, 2, 980));

		xasCtrl = new SGMXASDacqScanController(sxsc, SGMBeamline::sgm());
		xasCtrl->initialize();
		xasCtrl->start();

		QTest::qWait(1000);
		bool scanDone = false;
		while(!scanDone){
			if(xasCtrl->isStopped())
				scanDone = true;
			QTest::qWait(1000);
		}

		QString filepath = "/home/reixs/beamline/programming/fkbl/fSGMApp/src/Calcium1_2.dat";
		QFile f(filepath);
		QVERIFY(f.open(QIODevice::ReadOnly));
		QTextStream fs(&f);
		QRegExp rx("^\\#");
		QString line;
		QStringList lp;
		double eV;
		QMap<double, QMap<QString, double> > data;
		QMap<QString, double> rowData;
		while( !fs.atEnd() && fs.readLine().contains(rx))
			;
		while( !fs.atEnd() ){
			line = fs.readLine();
			lp = line.split(',');
			eV = lp.at(1).toDouble();
			rowData["I0"] = lp.at(4).toDouble();
			rowData["TEY"] = lp.at(5).toDouble();
			rowData["TFY"] = lp.at(6).toDouble();
			data[eV] = rowData;
		}

		AMXASScan *xs = qobject_cast<AMXASScan*>(xasCtrl->scan());
		QVERIFY(xs);
		int evIndex = xs->indexOfChannel("eV");
		int I0Index = xs->indexOfChannel("I0");
		int teyIndex = xs->indexOfChannel("TEY");
		int tfyIndex = xs->indexOfChannel("TFY");
		for(unsigned int x = 0; x < xs->count(); x++){
			eV = xs->channel(evIndex)->value(x);
			rowData = data.value(eV);
			QCOMPARE(xs->channel(I0Index)->value(x), rowData.value("I0"));
			QCOMPARE(xs->channel(teyIndex)->value(x), rowData.value("TEY"));
			QCOMPARE(xs->channel(tfyIndex)->value(x), rowData.value("TFY"));
		}


		QTest::qWait(5000);
	}



private:
	SGMXASDacqScanController *xasCtrl;
};

