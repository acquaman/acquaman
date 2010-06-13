/*!

  \mainpage Acquaman: The Comic-book Hero of Synchrotron Science since 2011

  \section intro What is Acquaman?

  Acquaman is a fausome framework used for the <em>acqu</em>isition and <em>man</em>agement of experimental data.  It is used initially on the SGM and REIXS beamlines at the Canadian Light Source.

  \section userDocIntro For Beamline users...


  \section programmerDocIntro For Programmers...

  \subsection architecture Architecture and Modules
  \image html architecture.png "Figure 1: Architecture of the Acquaman Framework"

  The Acquaman framework is divided into 6 modules that group related functionality.  The dependency of the modules is shown in Figure 1: each block makes use of the components directly below it.

  \b Modules
  - \c beamline : Control interface to the actual beamline hardware
  - \c acquaman : Manages setting up and executing scans
  - \c dataman : Responsible for the representation (top layer), and storage and retrieval (bottom layer) of experimental data.
  - \c workflow : A program-wide system for scheduling and running a series of "user-level" actions
  - \c script : Provides the advanced user with powerful and direct access to the beamline, acquaman, and dataman layers
  - \c ui : Contains all user-interface widgets

  Some rules:
	- Any classes from the QtGui Qt module are used only in the ui module.
	- ...


  \section miscJunk For Boots and Dave...
  \subsection todo Misc. Todos

  \todo check out QFlags and figure out where to standardize flags

  \todo cooler notifications: http://blog.mpathirage.com/2009/09/12/using-libnotify-in-ubuntu-9-04/

  */



#include <QApplication>
#include "AMSettings.h"

#include "ui/AMMainWindow.h"

#include "beamline/AMPVNames.h"
#include "beamline/AMBeamline.h"
#include "beamline/SGMBeamline.h"
#include "beamline/AMControlState.h"
//#include "acquaman/AMXASDacqScanController.h"
#include "dataman/AMDatabase.h"
#include "dataman/AMDbLoader.h"
#include "dataman/AMFirstTimeController.h"


#include "ui/ConnectionSettings.h"
#include "ui/SamplePositions.h"
#include "ui/GratingResolution.h"
#include "ui/AbsorptionScanController.h"
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/EmissionScanController.h"
#include "ui/Scheduler.h"
#include "ui/PeriodicTable.h"
#include "ui/ProtocolViewer.h"
#include "ui/ExpAlbum.h"
#include "ui/BottomBar.h"

#include "AMErrorMonitor.h"


int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");

	// Load settings from disk:
	AMSettings::load();
	AMUserSettings::load();
	AMPVNames::load();


	// ensure user data folder and database are ready for use
	AMFirstTimeController();

	// Open up the user database, and ensure tables loaded as required for Scan storage:
	// AMScan::dbPrepareTables(AMDatabase::userdb());

	//Create the main tab window:
	AMMainWindow mw;
	mw.setWindowTitle("Acquaman");


	/// \todo Move to MainWindowController:

	mw.addPane(new ConnectionSettings(), "Beamline Control", "Dev Playground", ":/network-workgroup.png");
	mw.addPane(new SamplePositions(), "Beamline Control", "Sample Positions", ":/system-software-update.png");
	mw.addPane(new GratingResolution(), "Beamline Control", "Gratings and Resolution", ":/system-search.png");

	mw.addPane(new AbsorptionScanController(), "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");
	mw.addPane(new EmissionScanController(), "Experiment Setup", "Emission Scan", ":/multimedia-volume-control.png");

	SGMXASScanConfigurationViewer* sxscViewer = new SGMXASScanConfigurationViewer();
	mw.addPane(sxscViewer, "Experiment Setup", "David Scan", ":/utilities-system-monitor.png");
	// connect(sxscViewer, SIGNAL(scanControllerReady(AMScanController*)), this, SLOT(onScanControllerReady(AMScanController*)));

	mw.addPane(new Scheduler(), "Experiment Tools", "Scheduler", ":/user-away.png");
	mw.addPane(new PeriodicTable(), "Experiment Tools", "Periodic Table", ":/applications-science.png");
	mw.addPane(new ProtocolViewer(), "Experiment Tools", "Protocol", ":/accessories-text-editor.png");
	mw.addPane(new ExpAlbum(), "Experiment Tools", "My Data", ":/system-file-manager.png");

	/// end move to MainWindowController

	BottomBar* b = new BottomBar();
	mw.addBottomWidget(b);
	/*
void MainWindow::onScanControllerReady(AMScanController *scanController){
	qDebug() << "\n\nScan controller is ready\n\n";
	connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
	connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
	connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
}
*/

	// show main window
	mw.show();

	// app.setStyleSheet("QLabel {font: 16pt \"Lucida Grande\";}");


	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, "Watch out, Acquaman!"));
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "Watch out, Acquaman! The sky is exploding again!"));
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Information, -1, "Now is the winter of our discontent."));
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Information, -1, "Acquaman: Practical and safe."));
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "Acquaman! Watch out for your grill!"));
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, "Acquaman: For kids."));


	/// Program Run-loop:
	// =================================
	int retVal = app.exec();

	/// Program Shutdown:
	// =================================

	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	SGMBeamline::releaseSGM();
	// Close down connection to the user Database
	AMDatabase::releaseUserDb();

	// Debug only: store settings to files to ensure created:
	// Not recommended for future... if anything changes these variables in memory, will be stored permanently
	//AMSettings::save();
	//AMUserSettings::save();
	//AMPVNames::save();

	return retVal;
}

