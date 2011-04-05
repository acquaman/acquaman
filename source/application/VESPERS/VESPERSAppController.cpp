#include "VESPERSAppController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSBeamlineView.h"
#include "ui/AMMainWindow.h"

#include "ui/VESPERS/XRFDetectorView.h"

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

		// Create panes in the main window:
		////////////////////////////////////

		vespersView_ = new VESPERSBeamlineView;
		mw_->insertHeading("VESPERS", 0);
		mw_->addPane(vespersView_, "VESPERS", "Endstation Control", ":/utilities-system-monitor.png");

		XRFBriefDetectorView *xrf1EView = new XRFBriefDetectorView(VESPERSBeamline::vespers()->vortexXRF1E());
		XRFBriefDetectorView *xrf4EView = new XRFBriefDetectorView(VESPERSBeamline::vespers()->vortexXRF4E());

		mw_->addPane(xrf1EView, "VESPERS", "Fluorescence", ":/utilities-system-monitor.png");
		mw_->addPane(xrf4EView, "VESPERS", "4-el Vortex", ":/utilities-system-monitor.png");

		return true;
	}
	else
		return false;
}


void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}

