#include <QApplication>
#include "Settings.h"

#include "ui/MainWindow.h"

#include "beamline/PVNames.h"
#include "beamline/Beamline.h"
#include "dataman/Database.h"

// debug only:
#include "dataman/Scan.h"

int main(int argc, char *argv[])
{
	
	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");

	// Load settings from disk:
	Settings::load();
	UserSettings::load();
	PVNames::load();

	// Open up the database:
	Database::db();

	//Create the main tab window:
	// Memory management: all QObjects are children of this guy...will be deleted when he goes out of scope.
	MainWindow mw;
	//mw.showFullScreen();
	mw.show();

	// app.setStyleSheet("QLabel {font: 16pt \"Lucida Grande\";}");

	/// Temporary debug session: testing database inserts
	// =================================
	Scan jimbo;
	jimbo.setName("myFirstScan");
	jimbo.setNumber(3);
	jimbo.setSampleName("Carbon_60");
	jimbo.setComments("This is\n a three-line\n comment!");
	jimbo.setStartTime(QDateTime::currentDateTime());
	// Save (or update) jimbo in the database
	(*Database::db()) << jimbo;

	qDebug() << "Jimbo inserted into db.  Jimbo's new id:" << jimbo.id();

	QList<int> jimboIds = Database::db()->scansMatching(Database::Name, "myFirstScan");
	qDebug() << "matching myFirstScan: Found this many: " << jimboIds.count();

	jimboIds = Database::db()->scansMatching(Database::Name, "First");
	qDebug() << "matching First: Found this many: " << jimboIds.count();

	jimboIds = Database::db()->scansContaining(Database::Name, "First");
	qDebug() << "containing First: Found this many: " << jimboIds.count();
	// End of database insert / search testing
	// =====================================


	/// Program Run-loop:
	// =================================
	int retVal = app.exec();

	/// Program Shutdown:
	// =================================
	// Make sure we release/clean-up the beamline interface
	Beamline::releaseBl();
	// Close down connection to the Database
	Database::releaseDb();

	// Debug only: store settings to files to ensure created:
	// Not recommended for future... if anything changes these variables in memory, will be stored permanently
	Settings::save();
	UserSettings::save();
	PVNames::save();

	return retVal;
}

