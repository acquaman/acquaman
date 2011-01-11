

#include <QApplication>
#include <QtTest/QtTest>
#include "AMSettings.h"
#include "beamline/AMPVNames.h"
#include "dataman/AMFirstTimeController.h"
#include "tests/TestDataman.h"
/*
#include "tests/TestAcquaman.h"
#include "tests/TestUi.h"
#include "tests/TestBeamline.h"
 */

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("AcquamanTest");


	AMErrorMon::enableDebugNotifications(true);
	// Load settings from disk:
	AMSettings::load();
	AMUserSettings::load();
	AMPVNames::load();
	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	if(!AMFirstTimeController::firstTimeCheck())
		return -1;


	QTEST_DISABLE_KEYPAD_NAVIGATION
	int retVal = 0;

	/// Test Running
	// =================================

	TestDataman td;	// run all tests for the dataman module
	retVal |= QTest::qExec(&td, argc, argv);

	/*
	TestAcquaman ta;	// run all tests for the acquaman module
	retVal |= QTest::qExec(&ta, argc, argv);

	TestBeamline tb;	// run all tests for the beamline module
	retVal |= QTest::qExec(&tb, argc, argv);

	TestUi tu;	// run all tests for the user interface
	retVal |= QTest::qExec(&tu, argc, argv);
*/

	AMDatabase::releaseUserDb();

	return retVal;
}
