#include <QApplication>
#include "ui/MainWindow.h"

#include "beamline/Beamline.h"
//#include "acquaman/ScanController.h"
//#include "acquaman/DacqScanController.h"


int main(int argc, char *argv[])
{
//    ScanController *myScan;
//    myScan = new DacqScanController();
	
	QApplication app(argc, argv);
	app.setApplicationName("reixsdaq");
	
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
	
	return retVal;
}

