#include "AMAppController.h"

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
#include "dataman/AMImportController.h"


#include "ui/ConnectionSettings.h"
#include "ui/SGMSampleTransferView.h"
//#include "ui/SGMSamplePositioner.h"
#include "ui/SamplePositions.h"
#include "ui/GratingResolution.h"
#include "ui/AbsorptionScanController.h"
#include "ui/AMScanConfigurationView.h"
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/EmissionScanController.h"
#include "ui/Scheduler.h"
#include "ui/PeriodicTable.h"
#include "ui/ProtocolViewer.h"
#include "ui/ExpAlbum.h"
#include "ui/BottomBar.h"
#include "ui/AMDataView.h"
#include "ui/AMRunExperimentTree.h"

#include "AMErrorMonitor.h"

#include <QMenuBar>

AMAppController::AMAppController(QObject *parent) :
		QObject(parent)
{

	AMErrorMon::enableDebugNotifications(true);

	// Load settings from disk:
	AMSettings::load();
	AMUserSettings::load();
	AMPVNames::load();

	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	AMFirstTimeController();

	//Create the main tab window:
	mw_ = new AMMainWindow();
	mw_->setWindowTitle("Acquaman");


	// Create panes in the main window:
	////////////////////////////////////

	mw_->addPane(new ConnectionSettings(), "Beamline Control", "Dev Playground", ":/network-workgroup.png");
	mw_->addPane(new SGMSampleTransferView(), "Beamline Control", "SGM Sample Transfer", ":/system-software-update.png");
	//	mw_->addPane(new SGMSamplePositioner(), "Beamline Control", "SGM Sample Position", ":/system-software-update.png");
	mw_->addPane(new SamplePositions(), "Beamline Control", "Sample Positions", ":/system-software-update.png");
	mw_->addPane(new GratingResolution(), "Beamline Control", "Gratings and Resolution", ":/system-search.png");

	mw_->addPane(new AbsorptionScanController(), "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");
	mw_->addPane(new EmissionScanController(), "Experiment Setup", "Emission Scan", ":/multimedia-volume-control.png");

	//	SGMXASScanConfigurationViewer* sxscViewer = new SGMXASScanConfigurationViewer();
	AMScanConfigurationView *scanViewer = new AMScanConfigurationView();
	//	mw_->addPane(sxscViewer, "Experiment Setup", "David Scan", ":/utilities-system-monitor.png");
	mw_->addPane(scanViewer, "Experiment Setup", "Scans", ":/utilities-system-monitor.png");
	// connect(sxscViewer, SIGNAL(scanControllerReady(AMScanController*)), this, SLOT(onScanControllerReady(AMScanController*)));

	mw_->addPane(new Scheduler(), "Experiment Tools", "Scheduler", ":/user-away.png");
	mw_->addPane(new PeriodicTable(), "Experiment Tools", "Periodic Table", ":/applications-science.png");
	mw_->addPane(new ProtocolViewer(), "Experiment Tools", "Protocol", ":/accessories-text-editor.png");




	// create widget
	AMRunExperimentTree* dataTree = new AMRunExperimentTree();
	mw_->addPane(new AMDataView(), "Data", dataTree, "Data");



	BottomBar* b = new BottomBar();
	mw_->addBottomWidget(b);



	// Make connections:
	//////////////////////////////



	/*
void MainWindow::onScanControllerReady(AMScanController *scanController){
	qDebug() << "\n\nScan controller is ready\n\n";
	connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
	connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
	connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
}
*/

	// make/install actions:
	/////////////////////////////////
	QAction* importAction = new QAction("Import to Library...", mw_);
	importAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_I));
	importAction->setStatusTip("Import outside data files into the library");
	connect(importAction, SIGNAL(triggered()), this, SLOT(onActionImport()));

	//install menu bar, and add actions
	//////////////////////////////////////
#ifdef Q_WS_MAC
	menuBar_ = new QMenuBar(0);
#else
	menuBar_ = new QMenuBar();
	mw_->addTopWidget(menuBar_);
#endif

	fileMenu_ = menuBar_->addMenu("File");
	fileMenu_->addAction(importAction);



	// show main window
	mw_->show();

}


/// Program shutdown:
AMAppController::~AMAppController() {

	// destroy the main window:
	delete mw_;

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

}


void AMAppController::onActionImport() {

	new AMImportController();

}
