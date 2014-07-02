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


#include <QtCore/QString>
#include <QtTest/QtTest>

#include "util/AMErrorMonitor.h"
#include "util/AMSettings.h"

#include "dataman/REIXS/REIXSXESCalibration.h"
#include "application/AMDatamanAppController.h"

class REIXSTest : public QObject
{
	Q_OBJECT

public:
 	virtual ~REIXSTest();
	REIXSTest();

private Q_SLOTS:
	void checkCalibration();
	void testHexapodPVChanges();
};

 REIXSTest::~REIXSTest(){}
REIXSTest::REIXSTest()
{
}

void REIXSTest::checkCalibration()
{
	REIXSXESCalibration c;

	// test for LEG at 395eV.  See commissioning_positions_June22.xlsx for where these comparison numbers come from.

	QVERIFY(fabs(c.alpha(0)-1.50122074) < 0.0001 );
	QVERIFY(fabs(c.beta(395, 0)-1.478234377) < 0.0001);

	QVERIFY(fabs(c.sTheta(0) - 0.00147693) < 0.000001);

	QVERIFY(fabs(c.hexapodU(0) - -0.168234494) < 0.0001);

	QVector3D detectorPos = c.detectorPos(395, 0);
	double theta  = c.spectrometerTheta(detectorPos);
	QVERIFY(fabs(theta - 0.167184) < 0.001);

	QVERIFY(fabs(c.spectrometerRotationDrive(theta)- 315.318) < 1);	// note: barely close here (to excel... don't know who's imprecise)
	QVERIFY(fabs(c.detectorTranslation(detectorPos, theta) - -35.11144558) < 0.0001);

	QVERIFY(fabs(c.tiltStageDrive(395, 0, theta) - -6.30) < 0.1);	// -6.266 vs. 6.300 in excel

	QVector3D hexapodPos = c.hexapodXYZ(0);
	QVERIFY(fabs(hexapodPos.x()- -18.946) < 0.0001);
	QVERIFY(fabs(hexapodPos.y()- 10.917) < 0.0001);
	QVERIFY(fabs(hexapodPos.z()- -9.958) < 0.0001);

	QVector3D hexapodRST = c.hexapodRST(0);
	QVERIFY(fabs(hexapodRST.x()- 18.946) < 0.0001);
	QVERIFY(fabs(hexapodRST.y()- -10.917) < 0.0001);
	QVERIFY(fabs(hexapodRST.z()- 470.132) < 0.0001);



	detectorPos = c.detectorPos(200, 0);
	theta = c.spectrometerTheta(detectorPos);
	double eV200 = c.computeEVFromSpectrometerPosition(0, c.spectrometerRotationDrive(theta), c.detectorTranslation(detectorPos, theta));
	QVERIFY(fabs(eV200-200) < 0.001);

}

#include "beamline/AMPVControl.h"

void REIXSTest::testHexapodPVChanges()
{
	QString baseName = "HXPD1610-4-I21-01:";
	AMPVwStatusControl u("hexapodU", baseName+"U:sp", baseName+"U", baseName+"moving", QString(), this, 0.05);

	QTest::qWait(5000);

	u.move(.5);

	QTest::qWait(10000);

	u.move(0);

	QTest::qWait(10000);
}


int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("REIXSTest");


	AMErrorMon::enableDebugNotifications(true);

	AMDatamanAppController ac;
	if(!ac.startup())
		return -1;


	QTEST_DISABLE_KEYPAD_NAVIGATION
	int retVal = 0;

	/// Test Running
	// =================================

	REIXSTest td;
	retVal |= QTest::qExec(&td, argc, argv);

	AMDatabase::deleteDatabase("user");
	ac.shutdown();

	return retVal;
}


#include "REIXSTest.moc"
