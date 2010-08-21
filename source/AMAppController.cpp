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
#include "ui/AMSamplePositionView.h"
#include "ui/SamplePositions.h"
#include "ui/GratingResolution.h"
#include "ui/AbsorptionScanController.h"
#include "ui/AMScanConfigurationView.h"
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/EmissionScanController.h"
#include "ui/AMWorkflowManagerView.h"
#include "ui/Scheduler.h"
#include "ui/PeriodicTable.h"
#include "ui/ProtocolViewer.h"
#include "ui/ExpAlbum.h"
#include "ui/BottomBar.h"
#include "ui/AMDataView.h"
#include "ui/AMRunExperimentInsert.h"

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

	BottomBar* b = new BottomBar();
	mw_->addBottomWidget(b);
	connect(b, SIGNAL(addButtonClicked()), this, SLOT(onAddButtonClicked()));


	// Create panes in the main window:
	////////////////////////////////////

	connectionSettings_ = new ConnectionSettings();
	mw_->addPane(connectionSettings_, "Beamline Control", "Dev Playground", ":/network-workgroup.png");

	sampleTransferView_ = new SGMSampleTransferView();
	mw_->addPane(sampleTransferView_, "Beamline Control", "SGM Sample Transfer", ":/system-software-update.png");

//	samplePositionView_ = new AMSamplePositionView();
	samplePositionView_ = new SGMSamplePositionView();
	mw_->addPane(samplePositionView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");

//	samplePositions_ = new SamplePositions();
//	mw_->addPane(samplePositions_, "Beamline Control", "Sample Positions", ":/system-software-update.png");

	gratingResolution_ = new GratingResolution();
	mw_->addPane(gratingResolution_, "Beamline Control", "Gratings and Resolution", ":/system-search.png");

	absorptionScanController_ = new AbsorptionScanController();
	mw_->addPane(absorptionScanController_, "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");

	emissionScanController_ = new EmissionScanController();
	mw_->addPane(emissionScanController_, "Experiment Setup", "Emission Scan", ":/multimedia-volume-control.png");

	scanConfigurationHolder_ = new AMXASScanConfigurationHolder();
	mw_->addPane(scanConfigurationHolder_, "Experiment Setup", "SGM XAS Scan", ":/utilities-system-monitor.png");


	workflowManagerView_ = new AMWorkflowManagerView();
	mw_->addPane(workflowManagerView_, "Experiment Tools", "Workflow", ":/user-away.png");
	connect(scanConfigurationHolder_, SIGNAL(startScanRequested()), workflowManagerView_, SLOT(onStartScanRequested()));
	connect(workflowManagerView_, SIGNAL(freeToScan(bool)), scanConfigurationHolder_, SLOT(onFreeToScan(bool)));
	connect(scanConfigurationHolder_, SIGNAL(addToQueueRequested(AMScanConfiguration*)), workflowManagerView_, SLOT(onAddToQueueRequested(AMScanConfiguration*)));
	connect(workflowManagerView_, SIGNAL(addedToQueue(AMScanConfiguration*)), scanConfigurationHolder_, SLOT(onAddedToQueue(AMScanConfiguration*)));

	connect(scanConfigurationHolder_, SIGNAL(goToQueueRequested()), this, SLOT(goToWorkflow()));



	mw_->addPane(new Scheduler(), "Experiment Tools", "Scheduler", ":/user-away.png");
	mw_->addPane(new PeriodicTable(), "Experiment Tools", "Periodic Table", ":/applications-science.png");
	mw_->addPane(new ProtocolViewer(), "Experiment Tools", "Protocol", ":/accessories-text-editor.png");


	// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments"
	dataView_ = new AMDataView();
	QStandardItem* runsItem = mw_->addPane(dataView_, "Data", "Runs", ":/22x22/view_calendar_upcoming_days.png");
	QStandardItem* expItem = mw_->addPane(dataView_, "Data", "Experiments", ":/applications-science.png");

	// Hook into the sidebar and add Run and Experiment links below these headings.
	runExperimentInsert_ = new AMRunExperimentInsert(AMDatabase::userdb(), runsItem, expItem, mw_->sidebar(), this);
	connect(mw_->sidebar()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), runExperimentInsert_, SLOT(onItemSelected(QModelIndex,QModelIndex)));
	connect(runExperimentInsert_, SIGNAL(runSelected(int)), dataView_, SLOT(showRun(int)));
	connect(runExperimentInsert_, SIGNAL(experimentSelected(int)), dataView_, SLOT(showExperiment(int)));




	// Make connections:
	//////////////////////////////

	connect(mw_, SIGNAL(currentPaneChanged(QWidget*)), this, SLOT(onCurrentPaneChanged(QWidget*)));



	/*! \todo: hook up bottom-bar signals to the active scan controller.
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

	// need to delete the runs/experiments tree insert first, before the tree gets deleted
	delete runExperimentInsert_;

	// destroy the main window. This will delete everything else within it.
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

void AMAppController::goToWorkflow() {
	mw_->goToPane(workflowManagerView_);
}

void AMAppController::onCurrentPaneChanged(QWidget *pane) {

	// If the scanConfigurationHolder pane was activated, let it know:
	if(pane == scanConfigurationHolder_)
		scanConfigurationHolder_->onBecameCurrentWidget();

}

#include "dataman/AMExperiment.h"
void AMAppController::onAddButtonClicked() {

	// For now, we simply create a new experiment
	AMExperiment e("New Experiment");
	e.storeToDb(AMDatabase::userdb());
}
