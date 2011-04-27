#include "REIXSAppController.h"

#include "beamline/REIXS/REIXSBeamline.h"

#include "ui/AMScanConfigurationView.h"
#include "ui/AMScanConfigurationViewHolder.h"
#include "ui/REIXS/REIXSXESScanConfigurationDetailedView.h"
#include "acquaman/AMScanController.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"

#include "util/AMErrorMonitor.h"
#include "dataman/AMDbObjectSupport.h"

// For database registration:
#include "dataman/REIXS/REIXSXESCalibration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

#include "ui/REIXS/REIXSXESHexapodControlEditor.h"

#include "ui/AMSamplePlateView.h"
#include "dataman/AMRun.h"
#include "ui/AMStartScreen.h"

#include <QMessageBox>

REIXSAppController::REIXSAppController(QObject *parent) :
	AMAppController(parent)
{
	scanControllerActiveEditor_ = 0;
}

bool REIXSAppController::startup() {

	// Initialize central beamline object
	REIXSBeamline::bl();

	if(AMAppController::startup()) {

		AMDbObjectSupport::registerClass<REIXSXESScanConfiguration>();
		AMDbObjectSupport::registerClass<REIXSXESMCPDetectorInfo>();
		AMDbObjectSupport::registerClass<REIXSXESCalibration>();


		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::userdb(), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("REIXS", 5);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack. See AMFirstTimeController::onFirstTime() for where the facilities are created.
			firstRun.storeToDb(AMDatabase::userdb());
		}

		// Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
		AMStartScreen* startScreen = new AMStartScreen(0);
		startScreen->show();





		// Create panes in the main window:
		////////////////////////////////////

		mw_->insertHeading("Experiment Setup", 1);
		//////////
		xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
		scanConfigurationHolder_ = new AMScanConfigurationViewHolder(workflowManagerView_, xesScanConfigurationView_);
		mw_->addPane(scanConfigurationHolder_, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");

		////////////////// Testing junk; move somewhere clean ////////////////////
		QWidget* spectrometerControlWidget = new QWidget();
		QHBoxLayout* hl = new QHBoxLayout();
		hl->addWidget(new REIXSXESHexapodControlEditor(REIXSBeamline::bl()->spectrometer()->hexapod()));

		QGroupBox* gb = new QGroupBox("Motors");
		QVBoxLayout* vl = new QVBoxLayout();
		vl->addWidget(new QLabel("Spectrometer Rotation"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->angleDrive()));

		vl->addWidget(new QLabel("Detector Translation"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTranslation()));

		vl->addWidget(new QLabel("Detector Tilt"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()));

		vl->addWidget(new QLabel("Detector Rotation"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorRotationDrive()));

		gb->setLayout(vl);

		hl->addWidget(gb);

		hl->addStretch(1);

		hl->addWidget(new AMSamplePlateView());

//		AMVideoWidget* vw = new AMVideoWidget();
//		hl->addWidget(vw);
//		vw->openVideoUrl("http://v2e1607-001.cs.clsi.ca/mjpg/1/video.mjpg");
//		// vw->openVideoUrl("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V");
//		vw->play();



//		vw->videoWidget()->openVideoUrl("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V");
//		vw->videoWidget()->play();

//		AMOverlayVideoWidget* vw = new AMOverlayVideoWidget();
//		hl->addWidget(vw, 1);
//		vw->videoPlayer()->play(QString("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V"));

//		Phonon::VideoPlayer* vp = new Phonon::VideoPlayer();
//		hl->addWidget(vp, 1);
//		vp->play(QString("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V"));



		spectrometerControlWidget->setLayout(hl);
		mw_->addPane(spectrometerControlWidget, "Experiment Setup", "Spectrometer controls", ":/utilities-system-monitor.png");



		connect(scanConfigurationHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));



		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerCreated()), this, SLOT(onCurrentScanControllerCreated()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerDestroyed()), this, SLOT(onCurrentScanControllerDestroyed()));
		// connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerReinitialized(bool)), this, SLOT(onCurrentScanControllerReinitialized(bool)));


		/*! \todo: hook up bottom-bar signals to the active scan controller.
	void MainWindow::onScanControllerReady(AMScanController *scanController){
		qDebug() << "\n\nScan controller is ready\n\n";
		connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
		connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
		connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
	}
	*/

		return true;
	}
	else
		return false;
}


void REIXSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}


void REIXSAppController::onCurrentPaneChanged(QWidget *pane) {

//	// If the scanConfigurationHolder pane was activated, let it know:
//	if(pane == scanConfigurationHolder_)
//		scanConfigurationHolder_->onBecameCurrentWidget();

}


#include "dataman/AMScanEditorModelItem.h"
#include "ui/AMGenericScanEditor.h"

void REIXSAppController::onCurrentScanControllerCreated(){
	qDebug() << "Detected creation of " << AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();

	AMGenericScanEditor *scanEditor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(scanEditor, ":/applications-science.png"));

	scanEditor->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
	mw_->setCurrentPane(scanEditor);

	scanControllerActiveEditor_ = scanEditor;

	/// \todo add user preference: should new scans open in a new window, or docked?
	// mw_->undock(scanEditor);
	// QPoint newPos;
	// newPos.setX(scanEditor->pos().x()+100);
	// newPos.setY(scanEditor->pos().y()+150);
	// scanEditor->move(newPos);
}

void REIXSAppController::onCurrentScanControllerDestroyed(){
	qDebug() << "Detected deletion of " << AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();
	scanControllerActiveEditor_ = 0;
}

void REIXSAppController::onCurrentScanControllerReinitialized(bool removeScan){
	qDebug() << "Trying to reinitialize with scan editor " << scanControllerActiveEditor_;

	if(!scanControllerActiveEditor_) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -13, "Error while re-initializing the scan controller; there is no active scan editor window. This is a bug and you should report it to the Acquaman developers."));
		return;
	}

	/// \bug How do you know that the last scan in this scan editor is the one to remove? What if they've opened another scan since onCurrentScanControllerCreated()?
#warning "Bug for David: how do you know that the last scan in this scan editor is the one to remove? What if they've opened another scan since onCurrentScanControllerCreated()?"
	if(removeScan)
		scanControllerActiveEditor_->removeScan(scanControllerActiveEditor_->scanAt(scanControllerActiveEditor_->scanCount()-1));

	scanControllerActiveEditor_->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
}


#include "dataman/AMRunExperimentItems.h"
#include "dataman/AMExperiment.h"

void REIXSAppController::onWindowPaneCloseButtonClicked(const QModelIndex& index) {

	// is this a scan editor to be deleted?
	/////////////////////////////////
	if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == scanEditorsParentItem_) {
		AMGenericScanEditor* editor = qobject_cast<AMGenericScanEditor*>(index.data(AM::WidgetRole).value<QWidget*>());

		if(!editor)
			return;

		/// \todo Move this functionality into the scan window itself. Also provide more usability... ask if they want to stop the scan
		if(editor == scanControllerActiveEditor_) {
			QMessageBox::information(editor, "Cannot close this window while acquiring", "A scan is still acquiring data inside this scan window.\n\nTo close it, stop the scan first.", QMessageBox::Ok);
			return;
		}

		// delete all scans in the editor, and ask the user for confirmation. If they 'cancel' on any, give up here and don't close the window.
		while(editor->scanCount()) {
			if(!editor->deleteScanWithModifiedCheck(editor->scanAt(editor->scanCount()-1)))
				return;
		}
		mw_->removePane(editor);
		delete editor;
	}

	// is this an experiment asking to be deleted?
	/// \todo bad code; improve this with better architecture and functionality in expItem.  Don't like trusting dynamic_cast; there's no guarantee that someone didn't put a non-AMExperimentModelItem into the model under experimentsParentItem_.
	else if(mw_->windowPaneModel()->itemFromIndex(index.parent()) == experimentsParentItem_) {

		AMExperimentModelItem* expItem = dynamic_cast<AMExperimentModelItem*>(mw_->windowPaneModel()->itemFromIndex(index));
		if(!expItem)
			return;

		AMExperiment experiment(expItem->id(), expItem->database());

		QMessageBox deleteConfirmation(mw_);
		deleteConfirmation.setText(QString("Are you sure you want to delete the experiment '%1'?").arg(experiment.name()));
		deleteConfirmation.setInformativeText("The scans in this experiment will remain, but they will no longer be associated with the experiment. You can't undo this action.");
		deleteConfirmation.setIcon(QMessageBox::Question);
		deleteConfirmation.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		deleteConfirmation.setDefaultButton(QMessageBox::Ok);
		deleteConfirmation.setEscapeButton(QMessageBox::Cancel);

		if(deleteConfirmation.exec() == QMessageBox::Ok) {
			AMExperiment::deleteExperiment(experiment.id(), expItem->database());
		}
	}
}
