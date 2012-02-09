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


#include "VESPERSAppController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSEndstationView.h"
#include "ui/AMMainWindow.h"
#include "ui/AMStartScreen.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"

#include "ui/VESPERS/XRFDetectorView.h"
#include "ui/VESPERS/VESPERSXRFFreeRunView.h"
#include "ui/VESPERS/VESPERSPersistentView.h"
#include "dataman/VESPERS/AMXRFScan.h"
#include "util/AMPeriodicTable.h"
#include "ui/VESPERS/VESPERSDeviceStatusView.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "ui/VESPERS/VESPERSExperimentConfigurationView.h"
#include "ui/VESPERS/VESPERSRoperCCDDetectorView.h"

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

// Helper classes that technically shouldn't need to exist.
#include "util/VESPERS/ROIHelper.h"
#include "util/VESPERS/VortexDetectorStatusHelper.h"
#include "util/VESPERS/VESPERSWorkflowAssistant.h"
#include "ui/VESPERS/VESPERSWorkflowAssistantView.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "application/AMAppControllerSupport.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "dataman/export/AMExporterGeneralAscii.h"
#include "dataman/export/AMExporterAthena.h"

#include <QFileDialog>

// For database registration:
#include "dataman/VESPERS/XRFDetectorInfo.h"

VESPERSAppController::VESPERSAppController(QObject *parent) :
	AMAppController(parent)
{

}

bool VESPERSAppController::startup() {

	// Get a destination folder.
	AMUserSettings::load();
	QString start = AMUserSettings::userDataFolder;
	start.chop(1);
	start = start.remove("/userData");
	QString dir = QFileDialog::getExistingDirectory(0, "Choose a destination folder for your data.", start, QFileDialog::ShowDirsOnly);
	if (!dir.isEmpty()){

		dir += "/";
		if (!dir.contains("userData")){

			QDir makeNewDir(dir);
			makeNewDir.mkdir("userData");
			makeNewDir.cd("userData");
			dir = makeNewDir.absolutePath() + "/";
		}

		if (dir.compare(AMUserSettings::userDataFolder) != 0){

			AMUserSettings::userDataFolder = dir;
			AMUserSettings::save();
		}
	}

	// Start up the main program.
	if(AMAppController::startup()) {

		// Initialize central beamline object
		VESPERSBeamline::vespers();
		// Initialize the periodic table object.
		AMPeriodicTable::table();

		AMDbObjectSupport::s()->registerClass<XRFDetectorInfo>();
		AMDbObjectSupport::s()->registerClass<VESPERSXRFScanConfiguration>();
		AMDbObjectSupport::s()->registerClass<AMXRFScan>();
		AMDbObjectSupport::s()->registerClass<VESPERSEXAFSScanConfiguration>();

		AMDetectorViewSupport::registerClass<XRFBriefDetectorView, XRFDetector>();
		AMDetectorViewSupport::registerClass<XRFDetailedDetectorView, XRFDetector>();
		AMDetectorViewSupport::registerClass<VESPERSRoperCCDDetectorView, VESPERSRoperCCDDetector>();

		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("VESPERS", 4);	/// \todo For now, we know that 4 is the ID of the VESPERS facility, but this is a hardcoded hack.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "VESPERSDefault");

		AMExporterOptionGeneralAscii *vespersDefault = new AMExporterOptionGeneralAscii();

		if (matchIDs.count() != 0)
			vespersDefault->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

		vespersDefault->setName("VESPERSDefault");
		vespersDefault->setFileName("$name_$fsIndex.dat");
		vespersDefault->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$scanConfiguration[rois]\n\n$notes\nNote that I0.X is the energy feedback.\n\n");
		vespersDefault->setHeaderIncluded(true);
		vespersDefault->setColumnHeader("$dataSetName $dataSetInfoDescription");
		vespersDefault->setColumnHeaderIncluded(true);
		vespersDefault->setColumnHeaderDelimiter("");
		vespersDefault->setSectionHeader("");
		vespersDefault->setSectionHeaderIncluded(true);
		vespersDefault->setIncludeAllDataSources(true);
		vespersDefault->setFirstColumnOnly(true);
		vespersDefault->setSeparateHigherDimensionalSources(true);
		vespersDefault->setSeparateSectionFileName("$name_$dataSetName_$fsIndex.dat");
		vespersDefault->storeToDb(AMDatabase::database("user"));
		qDebug() << "Added the VESPERSDefault to exporter options";

		// HEY DARREN, THIS CAN BE OPTIMIZED TO GET RID OF THE SECOND LOOKUP FOR ID
		matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "VESPERSDefault");
		if(matchIDs.count() > 0)
			AMAppControllerSupport::registerClass<VESPERSEXAFSScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(matchIDs.at(0));

		// Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
		AMStartScreen* startScreen = new AMStartScreen(0);
		startScreen->show();

		// Create panes in the main window:
		////////////////////////////////////

		assistant_ = new VESPERSWorkflowAssistant(workflowManagerView_, this);
		VESPERSWorkflowAssistantView *assistantView = new VESPERSWorkflowAssistantView(assistant_);
		mw_->insertVerticalWidget(2, assistantView);

		// Setup the general endstation control view.
		VESPERSEndstationView *endstationView = new VESPERSEndstationView(VESPERSBeamline::vespers()->endstation());
		VESPERSDeviceStatusView *statusPage = new VESPERSDeviceStatusView;

		mw_->insertHeading("Beamline Control", 0);
		mw_->addPane(endstationView, "Beamline Control", "Endstation", ":/system-software-update.png");
		mw_->addPane(statusPage, "Beamline Control", "Device Status", ":/system-software-update.png");

		// Setup the XRF views for the single element vortex and the four element vortex detectors.  Since they have scans that are added to the workflow, it gets the workflow manager view passed into it as well.
		// This means that the FreeRunView kind of doubles as a regular detector view and a configuration view holder.
		VESPERSXRFFreeRunView *xrf1EFreeRunView = new VESPERSXRFFreeRunView(new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF1E()), workflowManagerView_);
		VESPERSXRFFreeRunView *xrf4EFreeRunView = new VESPERSXRFFreeRunView(new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF4E()), workflowManagerView_);

		VESPERSRoperCCDDetectorView *roperCCDView = new VESPERSRoperCCDDetectorView(VESPERSBeamline::vespers()->roperCCD());

		mw_->insertHeading("Free run", 1);
		mw_->addPane(xrf1EFreeRunView, "Free run", "XRF 1-el", ":/utilities-system-monitor.png");
		mw_->addPane(xrf4EFreeRunView, "Free run", "XRF 4-el", ":/utilities-system-monitor.png");
		mw_->addPane(roperCCDView, "Free run", "XRD - Roper", ":/utilities-system-monitor.png");

		// Setup page that auto-enables detectors.
		VESPERSExperimentConfigurationView *experimentConfigurationView = new VESPERSExperimentConfigurationView(VESPERSBeamline::vespers()->experimentConfiguration());

		// Setup XAS for the beamline.  Builds the config, view, and view holder.
		VESPERSEXAFSScanConfiguration *exafsScanConfig = new VESPERSEXAFSScanConfiguration();
		exafsScanConfig->addRegion(0, -30, 1, 40, 1);
		VESPERSEXAFSScanConfigurationView *exafsConfigView = new VESPERSEXAFSScanConfigurationView(exafsScanConfig);
		AMScanConfigurationViewHolder *exafsConfigViewHolder = new AMScanConfigurationViewHolder( workflowManagerView_, exafsConfigView);

		/// \todo this can likely be somewhere else in the framework.
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerCreated()), this, SLOT(onCurrentScanControllerCreated()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerStarted()), this, SLOT(onCurrentScanControllerStarted()));
		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerDestroyed()), this, SLOT(onCurrentScanControllerFinished()));

		mw_->insertHeading("Scans", 2);
		mw_->addPane(experimentConfigurationView, "Scans", "Experiment Setup", ":/utilities-system-monitor.png");
		mw_->addPane(exafsConfigViewHolder, "Scans", "XAS", ":/utilities-system-monitor.png");


		// This is the right hand panel that is always visible.  Has important information such as shutter status and overall controls status.  Also controls the sample stage.
		VESPERSPersistentView *persistentView = new VESPERSPersistentView;
		mw_->addRightWidget(persistentView);

		// Show the endstation control view first.
		mw_->setCurrentPane(experimentConfigurationView);

		// Bottom bar connections.
		connect(this, SIGNAL(pauseScanIssued()), this, SLOT(onPauseScanIssued()));
		connect(this, SIGNAL(stopScanIssued()), this, SLOT(onCancelScanIssued()));

		// Github setup for adding VESPERS specific comment.
		additionalIssueTypesAndAssignees_.append("I think it's a VESPERS specific issue", "dretrex");

		// THIS IS HERE TO PASS ALONG THE INFORMATION TO THE SUM AND CORRECTEDSUM PVS IN THE FOUR ELEMENT DETECTOR.
		ROIHelper *roiHelper = new ROIHelper(this);
		Q_UNUSED(roiHelper)
		// THIS IS HERE TO PASS ALONG THE INFORMATION TO THE MCA AND DXP STATUS UPDATE PVS THAT DON'T SEEM TO FOLLOW THE STANDARD NAMING CONVENTIONS.
		VortexDetectorStatusHelper *statusHelper = new VortexDetectorStatusHelper(this);
		Q_UNUSED(statusHelper)

		return true;
	}
	else
		return false;
}


void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void VESPERSAppController::onCurrentScanControllerStarted()
{
	QString fileFormat(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan()->fileFormat());

	if (fileFormat == "vespersXRF" || fileFormat == "vespers2011XRF")
		return;

	openScanInEditorAndTakeOwnership(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());

	VESPERSEXAFSScanConfiguration *config = qobject_cast<VESPERSEXAFSScanConfiguration *>(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan()->scanConfiguration());

	if (!config)
		return;

	switch(config->fluorescenceDetectorChoice()){

	case VESPERSEXAFSScanConfiguration::None:

		scanEditorAt(scanEditorCount()-1)->setExclusiveDataSourceByName("trans");
		break;

	case VESPERSEXAFSScanConfiguration::SingleElement:
	case VESPERSEXAFSScanConfiguration::FourElement:
	{
		QStringList dataSources(scanEditorAt(scanEditorCount()-1)->visibleDataSourceNames());
		int index = 0;

		for (int i = 0; i < dataSources.size(); i++){
			if (dataSources.at(i).contains("norm") && (dataSources.at(i).contains("Ka") || dataSources.contains("La"))){

				index = i;
				break;
			}
		}

		scanEditorAt(scanEditorCount()-1)->setExclusiveDataSourceByName(dataSources.at(index));

		break;
	}
	}
}

void VESPERSAppController::onCurrentScanControllerCreated()
{
	QString fileFormat(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan()->fileFormat());

	if (fileFormat == "vespersXRF" || fileFormat == "vespers2011XRF")
		return;

	connect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), this, SLOT(onProgressUpdated(double,double)));
	connect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), assistant_, SLOT(onCurrentProgressChanged(double,double)));
}

void VESPERSAppController::onCurrentScanControllerFinished()
{
	QString fileFormat(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan()->fileFormat());

	if (fileFormat == "vespersXRF" || fileFormat == "vespers2011XRF")
		return;

	if (AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->state() == AMScanController::Cancelled)
		assistant_->onScanCancelled();

	disconnect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), this, SLOT(onProgressUpdated(double,double)));
	disconnect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), assistant_, SLOT(onCurrentProgressChanged(double,double)));
}

void VESPERSAppController::onPauseScanIssued()
{
	AMScanController *controller = AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();

	if (controller && controller->isRunning())
		controller->pause();

	else if (controller && controller->isPaused())
		controller->resume();
}

void VESPERSAppController::onCancelScanIssued()
{
	AMScanController *controller = AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController();

	if (controller)
		controller->cancel();
}
