#include "VESPERSAppController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSEndstationView.h"
#include "ui/AMMainWindow.h"
#include "ui/AMStartScreen.h"
#include "ui/AMScanConfigurationViewHolder.h"

#include "ui/VESPERS/XRFDetectorView.h"
#include "ui/VESPERS/XRFFreeRunView.h"
#include "ui/VESPERS/VESPERSPersistentView.h"
#include "dataman/VESPERS/AMXRFScan.h"
#include "util/AMPeriodicTable.h"
#include "ui/VESPERS/XRFMapSetup.h"
#include "ui/VESPERS/VESPERSDeviceStatusView.h"
#include "ui/VESPERS/VESPERSXASScanConfigurationView.h"

#include "dataman/AMScanEditorModelItem.h"
#include "ui/AMGenericScanEditor.h"

// Helper classes that technically shouldn't need to exist.
#include "util/VESPERS/ROIHelper.h"
#include "util/VESPERS/VortexDetectorStatusHelper.h"

#include "dataman/AMDbObjectSupport.h"

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
	start = start.left(start.lastIndexOf("/"));
	QString dir = QFileDialog::getExistingDirectory(0, "Choose a destination folder for your data.", start, QFileDialog::ShowDirsOnly);
	if (!dir.isEmpty()){

		dir += "/";

		if (dir.compare(AMUserSettings::userDataFolder) != 0){

			AMUserSettings::userDataFolder = dir;
			AMUserSettings::save();
		}
	}

	// Initialize central beamline object
	VESPERSBeamline::vespers();
	// Initialize the periodic table object.
	AMPeriodicTable::table();

	// Start up the main program.
	if(AMAppController::startup()) {

		AMDbObjectSupport::registerClass<XRFDetectorInfo>();
		AMDbObjectSupport::registerClass<VESPERSXRFScanConfiguration>();
		AMDbObjectSupport::registerClass<AMXRFScan>();

		AMDetectorViewSupport::registerClass<XRFBriefDetectorView, XRFDetector>();
		AMDetectorViewSupport::registerClass<XRFDetailedDetectorView, XRFDetector>();

		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::userdb(), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("VESPERS", 4);	/// \todo For now, we know that 4 is the ID of the VESPERS facility, but this is a hardcoded hack. See AMFirstTimeController::onFirstTime() for where the facilities are created.
			firstRun.storeToDb(AMDatabase::userdb());
		}

		// Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
		AMStartScreen* startScreen = new AMStartScreen(0);
		startScreen->show();

		// Create panes in the main window:
		////////////////////////////////////

		scanControllerActiveEditor_ = 0;

		// Setup the general endstation control view.
		VESPERSEndstationView *endstationView_ = new VESPERSEndstationView;
		//VESPERSDeviceStatusView *statusPage = new VESPERSDeviceStatusView;

		mw_->insertHeading("Beamline Control", 0);
		mw_->addPane(endstationView_, "Beamline Control", "Endstation", ":/system-software-update.png");
		//mw_->addPane(statusPage, "Beamline Control", "Device Status", ":/system-software-update.png");

		// Setup the XRF views for the single element vortex and the four element vortex detectors.  Since they have scans that are added to the workflow, it gets the workflow manager view passed into it as well.
		// This means that the FreeRunView kind of doubles as a regular detector view and a configuration view holder.
		XRFFreeRunView *xrf1EFreeRunView = new XRFFreeRunView(new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF1E()), workflowManagerView_);
		XRFFreeRunView *xrf4EFreeRunView = new XRFFreeRunView(new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF4E()), workflowManagerView_);

		// Setup XAS for the beamline.  Builds the config, view, and view holder.
	//	VESPERSXASScanConfiguration *xasScanConfig = new VESPERSXASScanConfiguration();
		//xasScanConfig->addRegion(0, 0, 1, 10);
		//VESPERSXASScanConfigurationView *xasConfigView = new VESPERSXASScanConfigurationView(xasScanConfig);
		//AMScanConfigurationViewHolder *xasConfigViewHolder = new AMScanConfigurationViewHolder( workflowManagerView_, xasConfigView);
		/// \todo this can likely be somewhere else in the framework.
		//connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerStarted()), this, SLOT(onCurrentScanControllerStarted()));

		mw_->insertHeading("Free run", 1);
		mw_->addPane(xrf1EFreeRunView, "Free run", "XRF 1-el", ":/utilities-system-monitor.png");
		mw_->addPane(xrf4EFreeRunView, "Free run", "XRF 4-el", ":/utilities-system-monitor.png");
	//	mw_->addPane(xasConfigViewHolder, "Free run", "XAS test", ":/utilities-system-monitor.png");

		// Setup page to launch the nDMapper software with correct options.
		XRFMapSetup *ndMapSetup = new XRFMapSetup;

		mw_->insertHeading("Scans", 2);
		mw_->addPane(ndMapSetup, "Scans", "Map Setup", ":/utilities-system-monitor.png");

		// This is the right hand panel that is always visible.  Has important information such as shutter status and overall controls status.  Also controls the sample stage.
		VESPERSPersistentView *persistentView = new VESPERSPersistentView;
		mw_->addRightWidget(persistentView);

		// Show the endstation control view first.
		mw_->setCurrentPane(endstationView_);

		/// THIS IS HERE TO PASS ALONG THE INFORMATION TO THE SUM AND CORRECTEDSUM PVS IN THE FOUR ELEMENT DETECTOR.
		ROIHelper *roiHelper = new ROIHelper(this);
		Q_UNUSED(roiHelper)
		/// THIS IS HERE TO PASS ALONG THE INFORMATION TO THE MCA AND DXP STATUS UPDATE PVS THAT DON'T SEEM TO FOLLOW THE STANDARD NAMING CONVENTIONS.
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
}

void VESPERSAppController::onCurrentScanControllerStarted()
{
	if (AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan()->fileFormat() == "vespersXRF")
		return;

	// Build a generic scan editor and put it the scan editors location.
	AMGenericScanEditor *scanEditor = new AMGenericScanEditor();
	scanEditorsParentItem_->appendRow(new AMScanEditorModelItem(scanEditor, ":/applications-science.png"));

	// Add the current scan to the editor and the show the scan editor.
	scanEditor->addScan(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
	mw_->setCurrentPane(scanEditor);

	scanControllerActiveEditor_ = scanEditor;
}

