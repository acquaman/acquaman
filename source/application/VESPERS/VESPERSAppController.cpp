#include "VESPERSAppController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSBeamlineView.h"
#include "ui/AMMainWindow.h"

VESPERSAppController::VESPERSAppController(QObject *parent) :
	AMAppController(parent)
{

}

bool VESPERSAppController::startup() {

	// Initialize central beamline object
	VESPERSBeamline::vespers();

	if(AMAppController::startup()) {

		// Create panes in the main window:
		////////////////////////////////////

		vespersView_ = new VESPERSBeamlineView;
		mw_->insertHeading("VESPERS", 0);
		mw_->addPane(vespersView_, "VESPERS", "Endstation Control", ":/utilities-system-monitor.png");

		return true;
	}
	else
		return false;
}


void VESPERSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
}

