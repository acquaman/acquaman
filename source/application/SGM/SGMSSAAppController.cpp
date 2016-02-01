/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "SGMSSAAppController.h"

#include "beamline/CLS/CLSFacilityID.h"
#include "beamline/SGM/SGMBeamline.h"

#include "ui/SGM/SGMSampleManipulatorView.h"
#include "ui/dataman/AMSampleManagementPre2013Widget.h"
#include "ui/SGM/SGMSidebar.h"

#include "ui/AMMainWindow.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMRun.h"

SGMSSAAppController::SGMSSAAppController(QObject *parent) :
	AMAppController(parent)
{

}

bool SGMSSAAppController::startup() {

	// Initialize AMBeamline::bl() as an SGMBeamline::sgm() instance. FIRST!
	SGMBeamline::sgm();

	if(AMAppController::startup()) {

		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
			// no run yet... let's create one.
			AMRun firstRun(CLSFacilityID::SGMSSBeamlineName, CLSFacilityID::SGMSSBeamline); //3: SGMSS Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}




		// Create panes in the main window:
		////////////////////////////////////

		mw_->insertHeading("Beamline Control", 0);
		//////////
		/*
		samplePositionView_ = new AMSampleManagementWidget(new SGMSampleManipulatorView(),
									QUrl("http://ccd1611-403/axis-cgi/mjpg/video.cgi?resolution=1280x1024&.mjpg"),
									SGMBeamline::sgm()->currentSamplePlate());
		mw_->addPane(samplePositionView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");
		connect(samplePositionView_, SIGNAL(newSamplePlateSelected(AMSamplePlate*)), SGMBeamline::sgm(), SLOT(setCurrentSamplePlate(AMSamplePlate*)));

		connect(SGMBeamline::sgm(), SIGNAL(currentSamplePlateChanged(AMSamplePlate*)), workflowManagerView_, SLOT(setCurrentSamplePlate(AMSamplePlate*)));
		*/
		sampleManipulatorView_ = new SGMSampleManipulatorView(true);
		mw_->addPane(sampleManipulatorView_, "Beamline Control", "SGM Sample Position", ":/system-software-update.png");


		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineConnected()));

		/*! \todo: hook up bottom-bar signals to the active scan controller.
	void MainWindow::onScanControllerReady(AMScanController *scanController){
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


void SGMSSAAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}


void SGMSSAAppController::onCurrentPaneChanged(QWidget *pane) {
	Q_UNUSED(pane);
}

void SGMSSAAppController::onSGMBeamlineConnected(){

}
