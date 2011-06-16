#include "VESPERSAppController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSBeamlineView.h"
#include "ui/AMMainWindow.h"
#include "ui/AMStartScreen.h"

#include "ui/VESPERS/XRFDetectorView.h"
#include "ui/VESPERS/XRFFreeRunView.h"
#include "ui/AMScanConfigurationViewHolder.h"
#include "ui/AMFreeRunScanConfigurationViewHolder.h"
#include "ui/VESPERS/VESPERSPersistentView.h"
#include "ui/AMWorkflowManagerView.h"
#include "dataman/VESPERS/AMXRFScan.h"
#include "util/AMPeriodicTable.h"
#include "ui/VESPERS/XRFMapSetup.h"
#include "ui/VESPERS/VESPERSDeviceStatusView.h"

#include "util/VESPERS/ROIHelper.h"

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
	QString dir = QFileDialog::getExistingDirectory(0, "Choose a destination folder for your data.", AMUserSettings::userDataFolder, QFileDialog::ShowDirsOnly);
	if (dir.isEmpty())
		dir = AMUserSettings::userDataFolder;
	else{
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

		vespersView_ = new VESPERSBeamlineView;
		//VESPERSDeviceStatusView *statusPage = new VESPERSDeviceStatusView;

		mw_->insertHeading("Beamline Control", 0);
		mw_->addPane(vespersView_, "Beamline Control", "Endstation", ":/system-software-update.png");
		//mw_->addPane(statusPage, "Beamline Control", "Device Status", ":/system-software-update.png");

		xrf1EFreeRunView_ = new XRFFreeRunView(new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF1E()));
		xrf4EFreeRunView_ = new XRFFreeRunView(new XRFFreeRun(VESPERSBeamline::vespers()->vortexXRF4E()));
		xrf1EConfigHolder_ = new AMFreeRunScanConfigurationViewHolder(workflowManagerView_, xrf1EFreeRunView_);
		xrf4EConfigHolder_ = new AMFreeRunScanConfigurationViewHolder(workflowManagerView_, xrf4EFreeRunView_);

		connect(xrf1EFreeRunView_, SIGNAL(startScan()), xrf1EConfigHolder_, SLOT(onFreeRunStartRequested()));
		connect(xrf4EFreeRunView_, SIGNAL(startScan()), xrf4EConfigHolder_, SLOT(onFreeRunStartRequested()));

		mw_->insertHeading("Free run", 1);
		mw_->addPane(xrf1EConfigHolder_, "Free run", "XRF 1-el", ":/utilities-system-monitor.png");
		mw_->addPane(xrf4EConfigHolder_, "Free run", "XRF 4-el", ":/utilities-system-monitor.png");

		XRFMapSetup *ndMapSetup = new XRFMapSetup;

		mw_->insertHeading("Scans", 2);
		mw_->addPane(ndMapSetup, "Scans", "Map Setup", ":/utilities-system-monitor.png");

		persistentView_ = new VESPERSPersistentView;
		mw_->addRightWidget(persistentView_);

		mw_->setCurrentPane(vespersView_);

		/// THIS IS HERE TO PASS ALONG THE INFORMATION TO THE SUM AND CORRECTEDSUM PVS IN THE FOUR ELEMENT DETECTOR.
		ROIHelper *helper = new ROIHelper;
		Q_UNUSED(helper)

		return true;
	}
	else
		return false;
}


void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}

