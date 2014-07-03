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


#include "util/AMSettings.h"
#include <QtTest/QtTest>
#include "beamline/SGM/SGMBeamline.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/AMXASScan.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"

#include "util/AMErrorMonitor.h"

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
		AMRegionsListModel *rlm2 = 0;
		// First model should be initialized but be empty
		QCOMPARE(rlm1->rowCount(QModelIndex()), 0);
		QVERIFY(rlm2 == 0);

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
};

