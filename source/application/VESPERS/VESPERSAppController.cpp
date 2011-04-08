#include "VESPERSAppController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSBeamlineView.h"
#include "ui/AMMainWindow.h"

#include "ui/VESPERS/XRFDetectorView.h"
#include "ui/VESPERS/VESPERSXRFScanConfigurationView.h"
#include "ui/AMScanConfigurationViewHolder.h"
#include "ui/AMFreeRunScanConfigurationViewHolder.h"
#include "ui/VESPERS/VESPERSTopBarView.h"

#include "dataman/AMDbObjectSupport.h"

// For database registration:
#include "dataman/VESPERS/XRFDetectorInfo.h"

VESPERSAppController::VESPERSAppController(QObject *parent) :
	AMAppController(parent)
{

}

bool VESPERSAppController::startup() {

	// Initialize central beamline object
	VESPERSBeamline::vespers();

	if(AMAppController::startup()) {

		AMDbObjectSupport::registerClass<XRFDetectorInfo>();

		AMDetectorViewSupport::registerClass<XRFBriefDetectorView, XRFDetector>();
		AMDetectorViewSupport::registerClass<XRFDetailedDetectorView, XRFDetector>();

		// Create panes in the main window:
		////////////////////////////////////

		vespersView_ = new VESPERSBeamlineView;
		mw_->insertHeading("Beamline Control", 0);
		mw_->addPane(vespersView_, "Beamline Control", "Endstation", ":/system-software-update.png");

		//XRFDetailedDetectorView *xrf1EViewDetailed = new XRFDetailedDetectorView(VESPERSBeamline::vespers()->vortexXRF4E());

		xrf1EConfigView_ = new VESPERSXRFScanConfigurationView(new VESPERSXRFScanConfiguration(VESPERSBeamline::vespers()->vortexXRF1E()));
		xrf4EConfigView_ = new VESPERSXRFScanConfigurationView(new VESPERSXRFScanConfiguration(VESPERSBeamline::vespers()->vortexXRF4E()));
		xrf1EConfigHolder_ = new AMFreeRunScanConfigurationViewHolder(workflowManagerView_, xrf1EConfigView_);
		xrf4EConfigHolder_ = new AMFreeRunScanConfigurationViewHolder(workflowManagerView_, xrf4EConfigView_);

		mw_->insertHeading("Free run", 1);
		mw_->addPane(xrf1EConfigHolder_, "Free run", "XRF 1-el", ":/utilities-system-monitor.png");
		mw_->addPane(xrf4EConfigHolder_, "Free run", "XRF 4-el", ":/utilities-system-monitor.png");

		topBar_ = new VESPERSTopBarView;
		mw_->addTopWidget(topBar_);

		return true;
	}
	else
		return false;
}


void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}

