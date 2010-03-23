#include <QApplication>
#include "Settings.h"

#include "ui/MainWindow.h"

#include "beamline/PVNames.h"
#include "beamline/Beamline.h"
//#include "acquaman/ScanController.h"
//#include "acquaman/DacqScanController.h"
#include "dataman/Database.h"



int main(int argc, char *argv[])
{
//    ScanController *myScan;
//    myScan = new DacqScanController();
	
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
	
	// Run the program (enter QT event loop)
	int retVal = app.exec();
	
	// Make sure we release/clean-up the beamline interface
	Beamline::releaseBl();

        // Debug only: store settings to files to ensure created:
        // Not recommended for future... if anything changes these variables in memory, will be stored permanently
        Settings::save();
        UserSettings::save();
        PVNames::save();
	
	return retVal;
}

