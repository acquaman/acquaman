#include <QApplication>
#include "Settings.h"

#include "ui/MainWindow.h"

#include "beamline/PVNames.h"
#include "beamline/Beamline.h"
#include "beamline/SGMBeamline.h"
#include "beamline/ControlState.h"
#include "acquaman/XASDacqScanController.h"
#include "dataman/Database.h"
#include "dataman/DbLoader.h"

// debug only:
#include "dataman/Scan.h"

#include "ErrorMonitor.h"


int main(int argc, char *argv[])
{
//    ScanController *myScan;
//    myScan = new DacqScanController();
	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");

	// Load settings from disk:
        AMSettings::load();
        AMUserSettings::load();
        AMPVNames::load();

	// Open up the user database, and ensure tables loaded as required for Scan storage:
        AMScan::dbPrepareTables(AMDatabase::userdb());

	//Create the main tab window:
	// Memory management: all QObjects are children of this guy...will be deleted when he goes out of scope.
	MainWindow mw;
	//mw.showFullScreen();
	mw.show();

	// app.setStyleSheet("QLabel {font: 16pt \"Lucida Grande\";}");

	/// Temporary debug session: testing database inserts
	// =================================
        AMScan jimbo;
	jimbo.setName("myFirstScan");
	jimbo.setNumber(3);
	jimbo.setSampleName("Carbon_60");
	jimbo.setComments("This is\n a three-line\n comment!");
	jimbo.setDateTime(QDateTime::currentDateTime());
	// Save (or update) jimbo in the database
        *AMDatabase::userdb() << jimbo;

	qDebug() << "Jimbo inserted into db.  Jimbo's new id:" << jimbo.id();

        QList<int> jimboIds = AMDatabase::userdb()->scansMatching("name", "myFirstScan");
	qDebug() << "matching myFirstScan: Found this many: " << jimboIds.count();

        jimboIds = AMDatabase::userdb()->scansMatching("name", "First");
	qDebug() << "matching First: Found this many: " << jimboIds.count();

        jimboIds = AMDatabase::userdb()->scansContaining("name", "First");
	qDebug() << "containing First: Found this many: " << jimboIds.count();

        qDebug() << "What is the type of object at id 1?" << AMDatabase::userdb()->scanType(1);
        qDebug() << "What is the type of object at id 201?" << AMDatabase::userdb()->scanType(201);

	// Use the powerful anyType loader to load from the database:
        AMDbObject* loadedObject = AMDbLoader::createAndLoad(AMDatabase::userdb(), 1);
	// this loaded all the details of the Scan object, even though you have a DbObject pointer
	// Can ask for it's type using
	qDebug() << "loaded database object @ id=1 dynamically; his type is: " << loadedObject->type();
	// If you want, convert to a Scan using qt's dynamic cast:
        AMScan* loadedScan = qobject_cast<AMScan*>(loadedObject);
	if(loadedScan)
		qDebug() << "Dynamic object was a Scan... his sampleName is" << loadedScan->sampleName();
	else
		qDebug() << "Dynamically loaded an object, but it's not a Scan.";

	// But all this is silly if you already know the type of the object.  Just do:
        AMScan newScan2;
        qDebug() << "Attempting to load scan at ID 2. Found it: " << newScan2.loadFromDb(AMDatabase::userdb(), 2);
	qDebug() << "scan at id 2: comment is: " << newScan2.comments();
	qDebug() << "scan at id 2: dateTime is: " << newScan2.dateTime();
	qDebug() << "scan at id 2: id is: " << newScan2.id();

	// End of database insert / search testing
	// =====================================

        AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "Fuck off, dave."));

	/// Program Run-loop:
	// =================================
	int retVal = app.exec();

	/// Program Shutdown:
	// =================================

        //XASScanConfiguration *xasCfg = new

	// Make sure we release/clean-up the beamline interface
        AMBeamline::releaseBl();
	// Close down connection to the user Database
        AMDatabase::releaseUserDb();

	// Debug only: store settings to files to ensure created:
	// Not recommended for future... if anything changes these variables in memory, will be stored permanently
        AMSettings::save();
        AMUserSettings::save();
        AMPVNames::save();

	return retVal;
}

