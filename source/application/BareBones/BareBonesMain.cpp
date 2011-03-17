/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include "application/AMAppController.h"

#include "beamline/AMProcessVariable.h"
#include <QDoubleSpinBox>
#include "ui/AMControlEditor.h"
#include "dataman/AMScan.h"
#include "ui/AMScanView.h"
#include "ui/AMGenericScanEditor.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");


	AMAppController* appController = new AMAppController();

//	// Create a process variable connection:
//	AMProcessVariable* myPV = new AMProcessVariable("PCT1402-01:mA:fbk", true);

//	// Make some widgets to watch it:
//	QDoubleSpinBox* readbackMonitor = new QDoubleSpinBox();
//	QDoubleSpinBox* setpointEditor = new QDoubleSpinBox();

//	// widget layout code here
//	// ...
//	/////////////

//	// Make connections
//	connect(myPV, SIGNAL(valueChanged(double)), readbackMonitor, SLOT(setValue(double)));
//	connect(setpointEditor, SIGNAL(valueChanged(double)), myPV, SLOT(setValue(double)));

//	// Let's assume we have one already:
//	AMProcessVariable* myPV = MyBeamline::bl()->myPV1();
//	// Or create and connect one:
//	AMProcessVariable* myPV = new AMProcessVariable("PCT1402-01:mA:fbk", true);

//	myPV->setValue(3.0);
//	myPV->setValue(QString("3.4"));
//	myPV->setValue(3);

//	myPV->lastValue();	// 3.0 (most recent monitored value)
//						// (If monitoring is on)

//	myPV->getInt();		// 3
//	myPV->getDouble();	// 3.0
//	myPV->getString();	// "3.0"

//	connect(myPV, SIGNAL(valueChanged(double)), someObject, SLOT(someSlot(double)));


//	// Is it connected?
//	myPV->isConnected();	// true  (After some time)
//	// permissions:
//	myPV->canRead();	// true
//	myPV->canWrite();	// also true (interesting, for the ring current)

//	// Other handy tricks:
//	myPV->units();	// "mA"



//	AMPVControl* myControl =
//	        new AMPVwStatusControl("hexapodX",
//	                               "HXPD1610-401:X:sp",
//	                               "HXPD1610-401:X",
//	                               "HXPD1610-401:moving",
//	                               QString(),
//	                               this,
//	                               0.1);

//	AMBasicControlEditor* myEditor =
//	        new AMBasicControlEditor(myControl, this);

//	AMBeamline::bl()



//AMScan s;
//// Get it full and interesting...
//// ...
//int databaseId = s.storeToDb(AMDatabase::userdb());

//// Restore it back from the database:
//AMScan s2;
//s2.loadFromDb(AMDatabase::userdb(), databaseId);

//	AMScanView* view = new AMScanView();
//	AMScan* scan1 = getFromSomewhere();
//	AMScan* scan2 = getFromSomewhere();

//	view->addScan(scan1);
//	view->addScan(scan2);

//	AMGenericScanEditor* editor = new AMGenericScanEditor();






	/// Program Run-loop:
	// =================================
	int retVal = -1;
	if(appController->startup())
		retVal = app.exec();

	/// Program Shutdown:
	// =================================
	delete appController;

	return retVal;
}
