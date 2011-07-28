#include <QtCore/QString>
#include <QtTest/QtTest>

#include "util/AMErrorMonitor.h"
#include "util/AMSettings.h"
#include "dataman/AMFirstTimeController.h"

#include "dataman/REIXS/REIXSXESCalibration.h"

class REIXSTest : public QObject
{
	Q_OBJECT

public:
	REIXSTest();

private Q_SLOTS:
	void checkCalibration();
	void testHexapodPVChanges();
};

REIXSTest::REIXSTest()
{
}

void REIXSTest::checkCalibration()
{
	REIXSXESCalibration c;

	// test for LEG at 395eV.  See commissioning_positions_June22.xlsx for where these comparison numbers come from.

	QVERIFY(fabs(c.alpha(0)-1.50122074) < 0.0001 );
	QVERIFY(fabs(c.beta(395, 0)-1.478234377) < 0.0001);

	QVERIFY(fabs(c.sTheta() - 0.00147693) < 0.000001);

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

#include "beamline/AMControl.h"

void REIXSTest::testHexapodPVChanges()
{
	QString baseName = "HXPD1610-401-01:";
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
	// Load settings from disk:
	AMSettings::load();
	AMUserSettings::load();
	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	if(!AMFirstTimeController::firstTimeCheck())
		return -1;


	QTEST_DISABLE_KEYPAD_NAVIGATION
	int retVal = 0;

	/// Test Running
	// =================================

	REIXSTest td;
	retVal |= QTest::qExec(&td, argc, argv);

	AMDatabase::releaseUserDb();

	return retVal;
}


#include "REIXSTest.moc"
