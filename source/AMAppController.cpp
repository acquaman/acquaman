#include "AMAppController.h"

#include "AMSettings.h"
#include "acquaman.h"

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

#include <QStandardItemModel>
#include "ui/AMGenericScanEditor.h"

#include "ui/AMStartScreen.h"


#include "acquaman/AMScanController.h"

#include "AMErrorMonitor.h"

#include <QMenuBar>

AMAppController::AMAppController(QObject *parent) :
		QObject(parent)
{
	isStarting_ = true;
	isShuttingDown_ = false;
}

bool AMAppController::startup() {

	AMErrorMon::enableDebugNotifications(true);

	// Load settings from disk:
	AMSettings::load();
	AMUserSettings::load();
	AMPVNames::load();

	// ensure user data folder and database are ready for use, if this is the first time the program is ever run.
	if(!AMFirstTimeController::firstTimeCheck())
		return false;

	// Show the splash screen. (It will delete itself when closed)
	AMStartScreen* startScreen = new AMStartScreen(0);
	startScreen->show();

	//Create the main tab window:
	mw_ = new AMMainWindow();
	mw_->setWindowTitle("Acquaman");

	BottomBar* b = new BottomBar();
	mw_->addBottomWidget(b);
	connect(b, SIGNAL(addButtonClicked()), this, SLOT(onAddButtonClicked()));


	// Create panes in the main window:
	////////////////////////////////////

	//connectionSettings_ = new ConnectionSettings();
	// mw_->addPane(connectionSettings_, "Beamline Control", "Dev Playground", ":/network-workgroup.png");



	//	samplePositionView_ = new AMSamplePositionView();
	samplePositionView_ = new SGMSamplePositionView();
	mw_->addPane(samplePositionView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");

	sampleTransferView_ = new SGMSampleTransferView();
	mw_->addPane(sampleTransferView_, "Beamline Control", "SGM Sample Transfer", ":/system-software-update.png");

	//	samplePositions_ = new SamplePositions();
	//	mw_->addPane(samplePositions_, "Beamline Control", "Sample Positions", ":/system-software-update.png");

	// gratingResolution_ = new GratingResolution();
	// mw_->addPane(gratingResolution_, "Beamline Control", "Gratings and Resolution", ":/system-search.png");

	absorptionScanController_ = new AbsorptionScanController();
	mw_->addPane(absorptionScanController_, "Experiment Setup", "Absorption Scan", ":/utilities-system-monitor.png");

	//emissionScanController_ = new EmissionScanController();
	//mw_->addPane(emissionScanController_, "Experiment Setup", "Emission Scan", ":/multimedia-volume-control.png");

	scanConfigurationHolder_ = new AMXASScanConfigurationHolder();
	mw_->addPane(scanConfigurationHolder_, "Experiment Setup", "SGM XAS Scan", ":/utilities-system-monitor.png");


	workflowManagerView_ = new AMWorkflowManagerView();
	mw_->addPane(workflowManagerView_, "Experiment Tools", "Workflow", ":/user-away.png");
	connect(workflowManagerView_, SIGNAL(freeToScan(bool, bool)), scanConfigurationHolder_, SLOT(onFreeToScan(bool, bool)));
	connect(workflowManagerView_, SIGNAL(lockdownScanning(bool,QString)), scanConfigurationHolder_, SLOT(onLockdownScanning(bool,QString)));
	connect(scanConfigurationHolder_, SIGNAL(addToQueueRequested(AMScanConfiguration*, bool)), workflowManagerView_, SLOT(onAddScanRequested(AMScanConfiguration*, bool)));
	connect(scanConfigurationHolder_, SIGNAL(cancelAddToQueueRequest()), workflowManagerView_, SLOT(onCancelAddScanRequest()));
	connect(workflowManagerView_, SIGNAL(addedScan(AMScanConfiguration*)), scanConfigurationHolder_, SLOT(onAddedToQueue(AMScanConfiguration*)));

	connect(scanConfigurationHolder_, SIGNAL(goToQueueRequested()), this, SLOT(goToWorkflow()));
	connect(scanConfigurationHolder_, SIGNAL(newScanConfigurationView()), workflowManagerView_, SLOT(onNewScanConfigurationView()));


	// mw_->addPane(new Scheduler(), "Experiment Tools", "Scheduler", ":/user-away.png");
	// mw_->addPane(new PeriodicTable(), "Experiment Tools", "Periodic Table", ":/applications-science.png");
	// mw_->addPane(new ProtocolViewer(), "Experiment Tools", "Protocol", ":/accessories-text-editor.png");

	/// \todo clean this up
	scanEditors_ = new QStandardItemModel(this);
	AMGenericScanEditor *scanEditor = new AMGenericScanEditor();
	QStandardItem *item = new QStandardItem("Title Goes Here");
	item->setData(qVariantFromValue(scanEditor), AM::PointerRole);
	scanEditors_->insertRow(scanEditors_->rowCount(), item);

	/// \todo update name with names of open scans...
	mw_->addPane(scanEditor, "Now Playing...", "Scan Editor", ":/applications-science.png");


	// Make a dataview widget and add it under two links/headings: "Runs" and "Experiments". Here we use the AMMainWindowModel for more power
	dataView_ = new AMDataView();
	dataView_->setWindowTitle("Data");

	QStandardItem* dataViewItem = new QStandardItem();
	dataViewItem->setData(qVariantFromValue((QWidget*)dataView_), AM::WidgetRole);
	dataViewItem->setFlags(Qt::ItemIsEnabled);	// enabled, but should not be selectable
	QFont font = QFont("Lucida Grande", 10, QFont::Bold);
	font.setCapitalization(QFont::AllUppercase);
	dataViewItem->setFont(font);
	dataViewItem->setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
	dataViewItem->setData(true, AMWindowPaneModel::DockStateRole);

	mw_->windowPaneModel()->appendRow(dataViewItem);

	QStandardItem* runsItem = new QStandardItem(QIcon(":/22x22/view_calendar_upcoming_days.png"), "Runs");
	mw_->windowPaneModel()->initAliasItem(runsItem, dataViewItem, "Runs", -1);
	dataViewItem->appendRow(runsItem);
	QStandardItem* expItem = new QStandardItem(QIcon(":/applications-science.png"), "Experiments");
	mw_->windowPaneModel()->initAliasItem(expItem, dataViewItem, "Experiments", -1);
	dataViewItem->appendRow(expItem);


	// Hook into the sidebar and add Run and Experiment links below these headings.
	runExperimentInsert_ = new AMRunExperimentInsert(AMDatabase::userdb(), runsItem, expItem, this);
	connect(runExperimentInsert_, SIGNAL(newExperimentAdded(QModelIndex)), this, SLOT(onNewExperimentAdded(QModelIndex)));


	// connect the activated signal from the dataview to our own slot
	connect(dataView_, SIGNAL(activated(QList<QUrl>)), this, SLOT(onDataViewItemsActivated(QList<QUrl>)));
	// When 'alias' links are clicked in the main window sidebar, we might need to notify some widgets of the details
	connect(mw_, SIGNAL(aliasItemActivated(QWidget*,QString,QVariant)), this, SLOT(onMainWindowAliasItemActivated(QWidget*,QString,QVariant)));



	connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerCreated()), this, SLOT(onCurrentScanControllerCreated()));
	connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerDestroyed()), this, SLOT(onCurrentScanControllerDestroyed()));
	connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerReinitialized(bool)), this, SLOT(onCurrentScanControllerReinitialized(bool)));

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

	isStarting_ = false;
	return true;

}


/// Program shutdown:
AMAppController::~AMAppController() {

	if(isRunning())
		shutdown();
}

void AMAppController::shutdown() {

	isShuttingDown_ = true;

	// need to delete the runs/experiments tree insert first, before the tree gets deleted
	/// \tod IMPORTANT replace: delete runExperimentInsert_;

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
	qDebug() << "goToWorkflow goes to queue";
	mw_->goToPane(workflowManagerView_);
}

void AMAppController::onCurrentPaneChanged(QWidget *pane) {

	// If the scanConfigurationHolder pane was activated, let it know:
	if(pane == scanConfigurationHolder_)
		scanConfigurationHolder_->onBecameCurrentWidget();

}

void AMAppController::onMainWindowAliasItemActivated(QWidget *target, const QString &key, const QVariant &value) {

	if(target == dataView_) {
		if(key == "Runs")
			dataView_->showRun(value.toInt());
		if(key == "Experiments")
			dataView_->showExperiment(value.toInt());
	}
}

void AMAppController::onNewExperimentAdded(const QModelIndex &index) {
	mw_->sidebar()->expand(index.parent());
	mw_->sidebar()->setCurrentIndex(index);
	mw_->sidebar()->edit(index);
}

void AMAppController::onCurrentScanControllerCreated(){
	qDebug() << "Detected creation of " << (int)AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();
	AMGenericScanEditor *scanEditor = new AMGenericScanEditor();
	QStandardItem *item = new QStandardItem("Title Goes Here");
	item->setData(qVariantFromValue(scanEditor), AM::PointerRole);
	scanEditors_->insertRow(scanEditors_->rowCount(), item);
	mw_->addPane(scanEditor, "Now Playing...", "Scan Editor", ":/applications-science.png");
	scanEditor->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
	mw_->goToPane(scanEditor);
	mw_->undock(scanEditor);
	QPoint newPos;
	newPos.setX(scanEditor->pos().x()+100);
	newPos.setY(scanEditor->pos().y()+150);
	scanEditor->move(newPos);
}

void AMAppController::onCurrentScanControllerDestroyed(){
	qDebug() << "Detected deletion of " << (int)AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();
}

void AMAppController::onCurrentScanControllerReinitialized(bool removeScan){
	qDebug() << "Trying to reinitialize with " << scanEditors_->rowCount() << " editors";
	AMGenericScanEditor *scanEditor = scanEditors_->data(scanEditors_->index(scanEditors_->rowCount()-1, 0), AM::PointerRole).value<AMGenericScanEditor*>();
	if(removeScan)
		scanEditor->removeScan(scanEditor->scanAt(scanEditor->numScans()-1));
	scanEditor->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
}

#include "dataman/AMExperiment.h"
void AMAppController::onAddButtonClicked() {

	// For now, we simply create a new experiment
	AMExperiment e("New Experiment");
	e.storeToDb(AMDatabase::userdb());
}

#include "dataman/AMDbLoader.h"

void AMAppController::onDataViewItemsActivated(const QList<QUrl>& itemUrls) {

	if(itemUrls.count() == 0)
		return;

	AMGenericScanEditor* editor = new AMGenericScanEditor();
	mw_->addPane(editor, "Now Playing...", "Scan Editor", ":/applications-science.png");
	mw_->goToPane(editor);

	foreach(const QUrl& url, itemUrls) {

		if(url.scheme() != "amd")
			break;

		// Make sure this came from the user database... (it's the only one we support for now)
		/// \todo This wouldn't be necessary if the scan editor doesn't make assumptions about which database.  Should check that out.
		if(AMDatabase::userdb()->connectionName() != url.host())
			break;

		QStringList path = url.path().split('/', QString::SkipEmptyParts);
		if(path.count() != 2)
			break;

		QString tableName = path.at(0);
		bool idOkay;
		int id = path.at(1).toInt(&idOkay);
		if(!idOkay || id < 1)
			break;


		// Only show things from the Objects table for now
		if(tableName != AMDatabaseDefinition::objectTableName())
			break;

		AMScan* scan = qobject_cast<AMScan*>( AMDbLoader::createAndLoad(AMDatabase::userdb(), id) );
		if(scan)
			editor->addScan(scan);
	}
}
