#include "REIXSAppController.h"

#include "beamline/REIXS/REIXSBeamline.h"

#include "ui/AMScanConfigurationView.h"
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

		// Create panes in the main window:
		////////////////////////////////////



		mw_->insertHeading("Experiment Setup", 1);
		//////////
		xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
		scanConfigurationHolder_ = new AMScanConfigurationHolder(xesScanConfigurationView_);
		mw_->addPane(scanConfigurationHolder_, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");



		connect(workflowManagerView_, SIGNAL(freeToScan(bool, bool)), scanConfigurationHolder_, SLOT(onFreeToScan(bool, bool)));
		connect(workflowManagerView_, SIGNAL(lockdownScanning(bool,QString)), scanConfigurationHolder_, SLOT(onLockdownScanning(bool,QString)));
		connect(scanConfigurationHolder_, SIGNAL(addToQueueRequested(AMScanConfiguration*, bool)), workflowManagerView_, SLOT(onAddScanRequested(AMScanConfiguration*, bool)));
		connect(scanConfigurationHolder_, SIGNAL(cancelAddToQueueRequest()), workflowManagerView_, SLOT(onCancelAddScanRequest()));
		connect(workflowManagerView_, SIGNAL(addedScan(AMScanConfiguration*)), scanConfigurationHolder_, SLOT(onAddedToQueue(AMScanConfiguration*)));

		connect(scanConfigurationHolder_, SIGNAL(goToQueueRequested()), this, SLOT(goToWorkflow()));
		// removed: what does this do? connect(scanConfigurationHolder_, SIGNAL(newScanConfigurationView()), workflowManagerView_, SLOT(onNewScanConfigurationView()));






		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerCreated()), this, SLOT(onCurrentScanControllerCreated()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerDestroyed()), this, SLOT(onCurrentScanControllerDestroyed()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerReinitialized(bool)), this, SLOT(onCurrentScanControllerReinitialized(bool)));


		/*! \todo: hook up bottom-bar signals to the active scan controller.
	void MainWindow::onScanControllerReady(AMScanController *scanController){
		qDebug() << "\n\nScan controller is ready\n\n";
		connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
		connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
		connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
	}
	*/

		mw_->addRightWidget(new REIXSXESHexapodControlEditor(REIXSBeamline::bl()->spectrometer()->hexapod()));

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

	// If the scanConfigurationHolder pane was activated, let it know:
	if(pane == scanConfigurationHolder_)
		scanConfigurationHolder_->onBecameCurrentWidget();

}


#include "dataman/AMScanEditorModelItem.h"
#include "ui/AMGenericScanEditor.h"

void REIXSAppController::onCurrentScanControllerCreated(){
	qDebug() << "Detected creation of " << (int)AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();

	AMGenericScanEditor *scanEditor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(scanEditor, ":/applications-science.png"));

	scanEditor->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
	mw_->goToPane(scanEditor);

	scanControllerActiveEditor_ = scanEditor;

	/// \todo add user preference: should new scans open in a new window, or docked?
	// mw_->undock(scanEditor);
	// QPoint newPos;
	// newPos.setX(scanEditor->pos().x()+100);
	// newPos.setY(scanEditor->pos().y()+150);
	// scanEditor->move(newPos);
}

void REIXSAppController::onCurrentScanControllerDestroyed(){
	qDebug() << "Detected deletion of " << (int)AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();
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
