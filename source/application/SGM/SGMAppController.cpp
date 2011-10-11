/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMAppController.h"

#include "beamline/SGM/SGMBeamline.h"

#include "ui/SGM/SGMSampleTransferView.h"
#include "ui/SGM/SGMSampleManipulatorView.h"
#include "ui/dataman/AMSampleManagementWidget.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "ui/SGM/SGMFastScanConfigurationView.h"
#include "ui/SGM/SGMSidebar.h"
#include "acquaman/AMScanController.h"
#include "ui/beamline/AMDetectorView.h"
#include "ui/beamline/AMSingleControlDetectorView.h"
#include "ui/beamline/MCPDetectorView.h"
#include "ui/beamline/PGTDetectorView.h"
#include "ui/beamline/OceanOptics65000DetectorView.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMRun.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "ui/AMStartScreen.h"

#include "application/AMAppControllerSupport.h"

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
	scanControllerActiveEditor_ = 0;
}

bool SGMAppController::startup() {

	// Initialize AMBeamline::bl() as an SGMBeamline::sgm() instance. FIRST!
	//SGMBeamline::sgm();

	if(AMAppController::startup()) {
		SGMBeamline::sgm();

		AMDbObjectSupport::registerClass<MCPDetectorInfo>();
		AMDbObjectSupport::registerClass<PGTDetectorInfo>();
		AMDbObjectSupport::registerClass<OceanOptics65000DetectorInfo>();
		AMDbObjectSupport::registerClass<SGMXASScanConfiguration>();
		AMDbObjectSupport::registerClass<SGMFastScanConfiguration>();

		AMDetectorViewSupport::registerClass<AMSingleControlBriefDetectorView, AMSingleControlDetector>();
		AMDetectorViewSupport::registerClass<MCPBriefDetectorView, MCPDetector>();
		AMDetectorViewSupport::registerClass<MCPDetailedDetectorView, MCPDetector>();
		AMDetectorViewSupport::registerClass<PGTBriefDetectorView, PGTDetector>();
		AMDetectorViewSupport::registerClass<PGTDetailedDetectorView, PGTDetector>();
		AMDetectorViewSupport::registerClass<OceanOptics65000BriefDetectorView, OceanOptics65000Detector>();
		AMDetectorViewSupport::registerClass<OceanOptics65000DetailedDetectorView, OceanOptics65000Detector>();

		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::userdb(), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("SGM", 3);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack. See AMFirstTimeController::onFirstTime() for where the facilities are created.
			firstRun.storeToDb(AMDatabase::userdb());
			AMExporterOptionGeneralAscii *sgmDefault = new AMExporterOptionGeneralAscii();
			sgmDefault->setName("SGMDefault");
			sgmDefault->setFileName("$name_$exportIndex.txt");
			sgmDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription");
			sgmDefault->setHeaderIncluded(true);
			sgmDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
			sgmDefault->setColumnHeaderIncluded(true);
			sgmDefault->setColumnHeaderDelimiter("==========");
			sgmDefault->setSectionHeader("");
			sgmDefault->setSectionHeaderIncluded(true);
			sgmDefault->setIncludeAllDataSources(false);
			sgmDefault->addDataSource("EnergyFeedback", false, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("I0", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("Photodiode", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("TEY", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("TFY", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("TEYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("TFYNorm", true, AMExporterOptionGeneral::CombineInColumnsMode, true);
			sgmDefault->addDataSource("PFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
			sgmDefault->addDataSource("IPFY", true, AMExporterOptionGeneral::CombineInColumnsMode, false);
			sgmDefault->addDataSource("SDD", false, AMExporterOptionGeneral::SeparateFilesMode, false);
			sgmDefault->setSeparateSectionFileName("$name_$dataSetName_$exportIndex.txt");
			sgmDefault->storeToDb(AMDatabase::userdb());
		}

		AMAppControllerSupport::registerClass<SGMXASScanConfiguration, AMExporterGeneralAscii, AMExporterOptionGeneralAscii>();

		// Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
		AMStartScreen* startScreen = new AMStartScreen(0);
		startScreen->show();



		// Create panes in the main window:
		////////////////////////////////////

		mw_->insertHeading("Beamline Control", 0);
		//////////
		samplePositionView_ = new AMSampleManagementWidget(new SGMSampleManipulatorView(),
									QUrl("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg"),
									SGMBeamline::sgm()->currentSamplePlate());
		mw_->addPane(samplePositionView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");
		connect(samplePositionView_, SIGNAL(newSamplePlateSelected(AMSamplePlate*)), SGMBeamline::sgm(), SLOT(setCurrentSamplePlate(AMSamplePlate*)));

		connect(SGMBeamline::sgm(), SIGNAL(currentSamplePlateChanged(AMSamplePlate*)), workflowManagerView_, SLOT(setCurrentSamplePlate(AMSamplePlate*)));

		sampleTransferView_ = new SGMSampleTransferView();
		mw_->addPane(sampleTransferView_, "Beamline Control", "SGM Sample Transfer", ":/system-software-update.png");


		mw_->insertHeading("Experiment Setup", 1);
		//////////
		xasScanConfigurationView_ = 0; //NULL
		xasScanConfigurationHolder_ = new AMScanConfigurationViewHolder(workflowManagerView_);
		mw_->addPane(xasScanConfigurationHolder_, "Experiment Setup", "SGM XAS Scan", ":/utilities-system-monitor.png");

		fastScanConfigurationView_ = 0; //NULL
		fastScanConfigurationHolder_ = new AMScanConfigurationViewHolder(workflowManagerView_);
		mw_->addPane(fastScanConfigurationHolder_, "Experiment Setup", "SGM Fast Scan", ":/utilities-system-monitor.png");

		connect(xasScanConfigurationHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));
		connect(fastScanConfigurationHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));

		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerCreated()), this, SLOT(onCurrentScanControllerCreated()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerDestroyed()), this, SLOT(onCurrentScanControllerDestroyed()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerStarted()), this, SLOT(onCurrentScanControllerStarted()));

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineConnected()));

		/*! \todo: hook up bottom-bar signals to the active scan controller.
	void MainWindow::onScanControllerReady(AMScanController *scanController){
		qDebug() << "\n\nScan controller is ready\n\n";
		connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
		connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
		connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
	}
	*/
		sgmSidebar_ = new SGMSidebar();
		mw_->addRightWidget(sgmSidebar_);

		mw_->move(0, 0);

		return true;
	}
	else
		return false;
}


void SGMAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}


void SGMAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void SGMAppController::onSGMBeamlineConnected(){
	if(SGMBeamline::sgm()->isConnected() && !xasScanConfigurationView_ && !fastScanConfigurationView_){
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		//sxsc->addRegion(0, 950, 1, 960);
		double goodEnergy = 10 * floor(SGMBeamline::sgm()->energy()->value() / 10);
		sxsc->addRegion(0, goodEnergy, 1, goodEnergy+10);
		xasScanConfigurationView_ = new SGMXASScanConfigurationView(sxsc);
		xasScanConfigurationHolder_->setView(xasScanConfigurationView_);

		SGMFastScanConfiguration *sfsc = new SGMFastScanConfiguration(this);
		fastScanConfigurationView_ = new SGMFastScanConfigurationView(sfsc);
		fastScanConfigurationHolder_->setView(fastScanConfigurationView_);
	}
}

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

void SGMAppController::onCurrentScanControllerCreated(){
	connect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), this, SLOT(onProgressUpdated(double,double)));

	/// \todo add user preference: should new scans open in a new window, or docked?
	// mw_->undock(scanEditor);
	// QPoint newPos;
	// newPos.setX(scanEditor->pos().x()+100);
	// newPos.setY(scanEditor->pos().y()+150);
	// scanEditor->move(newPos);
}

void SGMAppController::onCurrentScanControllerDestroyed(){
	scanControllerActiveEditor_ = 0;
}

void SGMAppController::onCurrentScanControllerStarted(){
	AMGenericScanEditor *scanEditor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(scanEditor, ":/applications-science.png"));

	scanEditor->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
	mw_->setCurrentPane(scanEditor);

	scanControllerActiveEditor_ = scanEditor;
}

void SGMAppController::onCurrentScanControllerReinitialized(bool removeScan){
	if(!scanControllerActiveEditor_) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -13, "Error while re-initializing the scan controller; there is no active scan editor window. This is a bug and you should report it to the Acquaman developers."));
		return;
	}

	/* NTBA - August 25th, 2011 (David Chevrier)
			How do you know that the last scan in this scan editor is the one to remove?
			What if they've opened another scan since onCurrentScanControllerCreated()?"
	*/
	if(removeScan)
		scanControllerActiveEditor_->removeScan(scanControllerActiveEditor_->scanAt(scanControllerActiveEditor_->scanCount()-1));

	scanControllerActiveEditor_->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
}


#include "dataman/AMRunExperimentItems.h"
#include "dataman/AMExperiment.h"

void SGMAppController::onWindowPaneCloseButtonClicked(const QModelIndex& index) {

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
