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




#include <QApplication>
#include <QtTest/QtTest>
#include "util/AMSettings.h"
#include "beamline/AMPVNames.h"
#include "tests/TestDataman.h"
#include "tests/TestUi.h"

#include "application/AMDatamanAppController.h"

/*
#include "tests/TestAcquaman.h"
#include "tests/TestBeamline.h"
 */

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("AcquamanTest");


	AMErrorMon::enableDebugNotifications(true);

	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	AMDatamanAppController ac;
	if(!ac.startup())
		return -1;


	QTEST_DISABLE_KEYPAD_NAVIGATION
	int retVal = 0;

	/// Test Running
	// =================================

	TestDataman td;	// run all tests for the dataman module
	retVal |= QTest::qExec(&td, argc, argv);

	TestUi tu;	// run all tests for the user interface
	retVal |= QTest::qExec(&tu, argc, argv);

	/*
	TestAcquaman ta;	// run all tests for the acquaman module
	retVal |= QTest::qExec(&ta, argc, argv);

	TestBeamline tb;	// run all tests for the beamline module
	retVal |= QTest::qExec(&tb, argc, argv);
*/

	AMDatabase::deleteDatabase("user");

	return retVal;
}
