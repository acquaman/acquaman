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
#include "ui/AMStartScreen.h"

SGMAppController::SGMAppController(QObject *parent) :
	AMAppController(parent)
{
}

bool SGMAppController::startup() {

	// Initialize AMBeamline::bl() as an SGMBeamline::sgm() instance. FIRST!
	//SGMBeamline::sgm();

	if(AMAppController::startup()) {
		SGMBeamline::sgm();

		AMDbObjectSupport::s()->registerClass<MCPDetectorInfo>();
		AMDbObjectSupport::s()->registerClass<PGTDetectorInfo>();
		AMDbObjectSupport::s()->registerClass<OceanOptics65000DetectorInfo>();
		AMDbObjectSupport::s()->registerClass<SGMXASScanConfiguration>();
		AMDbObjectSupport::s()->registerClass<SGMFastScanConfiguration>();

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
		if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("SGM", 3);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
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
			sgmDefault->storeToDb(AMDatabase::database("user"));
		}

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
	AMAppController::shutdown();
}


void SGMAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane)
}

void SGMAppController::onSGMBeamlineConnected(){
	if(SGMBeamline::sgm()->isConnected() && !xasScanConfigurationView_ && !fastScanConfigurationView_){
		SGMXASScanConfiguration *sxsc = new SGMXASScanConfiguration(this);
		//sxsc->addRegion(0, 950, 1, 960);
		double goodEnergy = 10 * floor(SGMBeamline::sgm()->energy()->value() / 10);
		sxsc->addRegion(0, goodEnergy, 1, goodEnergy+10, 1);
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

}

void SGMAppController::onCurrentScanControllerDestroyed(){
}

void SGMAppController::onCurrentScanControllerStarted(){
	openScanInEditorAndTakeOwnership(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
}

