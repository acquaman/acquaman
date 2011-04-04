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

		// Create panes in the main window:
		////////////////////////////////////

		vespersView_ = new VESPERSBeamlineView;
		mw_->insertHeading("VESPERS", 0);
		mw_->addPane(vespersView_, "VESPERS", "Endstation Control", ":/utilities-system-monitor.png");

		XRFDetector *xrf1E = new XRFDetector("Single Element XRF",
					VESPERSBeamline::vespers()->mcaUpdateRate1E(),
					VESPERSBeamline::vespers()->peakingTime1E(),
					VESPERSBeamline::vespers()->maxEnergy1E(),
					VESPERSBeamline::vespers()->integrationTime1E(),
					VESPERSBeamline::vespers()->liveTime1E(),
					VESPERSBeamline::vespers()->elapsedTime1E(),
					VESPERSBeamline::vespers()->start1E(),
					VESPERSBeamline::vespers()->stop1E(),
					VESPERSBeamline::vespers()->deadTime1E(),
					VESPERSBeamline::vespers()->spectrum1E(),
					this);

		XRFBriefDetectorView *xrfView = new XRFBriefDetectorView(xrf1E, false);

		mw_->addPane(xrfView, "VESPERS", "Fluorescence", ":/utilities-system-monitor.png");

		return true;
	}
	else
		return false;
}


void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}

