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


#include "REIXSAppController.h"

#include "beamline/REIXS/REIXSBeamline.h"

#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder.h"
#include "ui/REIXS/REIXSXESScanConfigurationDetailedView.h"
#include "acquaman/AMScanController.h"

#include "ui/AMMainWindow.h"
#include "ui/AMWorkflowManagerView.h"

#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"

// For database registration:
#include "dataman/REIXS/REIXSXESCalibration.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

#include "ui/REIXS/REIXSXESHexapodControlEditor.h"
#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "ui/REIXS/REIXSSampleChamberButtonPanel.h"

#include "ui/dataman/AMSamplePlateView.h"
#include "dataman/AMRun.h"
#include "ui/AMStartScreen.h"

#include <QMessageBox>


REIXSAppController::REIXSAppController(QObject *parent) :
	AMAppController(parent)
{
}

bool REIXSAppController::startup() {

	// Initialize central beamline object
	REIXSBeamline::bl();

	if(AMAppController::startup()) {

		AMDbObjectSupport::s()->registerClass<REIXSXESScanConfiguration>();
		AMDbObjectSupport::s()->registerClass<REIXSXESMCPDetectorInfo>();
		AMDbObjectSupport::s()->registerClass<REIXSXESCalibration>();


		// Testing and making the first run in the database, if there isn't one already.  Make this it's own function if you think startup() is getting too big ; )
		////////////////////////////////////////

		AMRun existingRun;
		if(!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {
			// no run yet... let's create one.
			AMRun firstRun("REIXS", 5);	/// \todo For now, we know that 5 is the ID of the REIXS facility, but this is a hardcoded hack. See AMFirstTimeController::onFirstTime() for where the facilities are created.
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		// Show the splash screen, to let the user pick their current run. (It will delete itself when closed)
		AMStartScreen* startScreen = new AMStartScreen(0);
		startScreen->show();





		// Create panes in the main window:
		////////////////////////////////////

		mw_->insertHeading("Experiment Setup", 1);
		//////////
		xesScanConfigurationView_ = new REIXSXESScanConfigurationDetailedView(REIXSBeamline::bl()->mcpDetector());
		scanConfigurationHolder_ = new AMScanConfigurationViewHolder(workflowManagerView_, xesScanConfigurationView_);
		mw_->addPane(scanConfigurationHolder_, "Experiment Setup", "Emission Scan", ":/utilities-system-monitor.png");

		////////////////// Testing junk; move somewhere clean ////////////////////
		QWidget* spectrometerControlWidget = new QWidget();
		QHBoxLayout* hl = new QHBoxLayout();
		hl->addWidget(new REIXSXESHexapodControlEditor(REIXSBeamline::bl()->spectrometer()->hexapod()));

		QGroupBox* gb = new QGroupBox("Motors");
		QVBoxLayout* vl = new QVBoxLayout();
		vl->addWidget(new QLabel("Spectrometer Rotation"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->spectrometerRotationDrive()));

		vl->addWidget(new QLabel("Detector Translation"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTranslation()));

		vl->addWidget(new QLabel("Detector Tilt"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorTiltDrive()));

		vl->addWidget(new QLabel("Detector Rotation"));
		vl->addWidget(new AMBasicControlEditor(REIXSBeamline::bl()->spectrometer()->detectorRotationDrive()));

		gb->setLayout(vl);

		hl->addWidget(gb);

		hl->addWidget(new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer()));
		hl->addWidget(new REIXSSampleChamberButtonPanel());

		hl->addStretch(1);

		// hl->addWidget(new AMSamplePlateView());


//		AMCrosshairOverlayVideoWidget* vw = new AMCrosshairOverlayVideoWidget();
//		// vw->videoItem()->setAspectRatioMode(Qt::IgnoreAspectRatio);
//		vw->setCrosshairPosition(QPointF(0.99, 0.99));
//		vw->show();
//		vw->mediaPlayer()->setMedia(QUrl("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V"));
//		vw->mediaPlayer()->play();


//		AMVideoWidget* vw = new AMVideoWidget();
//		hl->addWidget(vw);
//		vw->openVideoUrl("http://v2e1607-001.cs.clsi.ca/mjpg/1/video.mjpg");
//		// vw->openVideoUrl("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V");
//		vw->play();



//		vw->videoWidget()->openVideoUrl("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V");
//		vw->videoWidget()->play();

//		AMOverlayVideoWidget* vw = new AMOverlayVideoWidget();
//		hl->addWidget(vw, 1);
//		vw->videoPlayer()->play(QString("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V"));

//		Phonon::VideoPlayer* vp = new Phonon::VideoPlayer();
//		hl->addWidget(vp, 1);
//		vp->play(QString("/Users/mboots/Pictures/iPhoto Library/Originals/2010/mine movies/101_0216.M4V"));



		spectrometerControlWidget->setLayout(hl);
		mw_->addPane(spectrometerControlWidget, "Experiment Setup", "Spectrometer controls", ":/utilities-system-monitor.png");


		////////////////// End of testing junk; move somewhere clean ////////////////////

		connect(scanConfigurationHolder_, SIGNAL(showWorkflowRequested()), this, SLOT(goToWorkflow()));



		connect(AMScanControllerSupervisor::scanControllerSupervisor(), SIGNAL(currentScanControllerStarted()), this, SLOT(onCurrentScanControllerCreated()));


		/*! \todo: hook up bottom-bar signals to the active scan controller.
	void MainWindow::onScanControllerReady(AMScanController *scanController){
		qDebug() << "\n\nScan controller is ready\n\n";
		connect(bottomBar_, SIGNAL(pauseScanIssued()), scanController, SLOT(pause()));
		connect(bottomBar_, SIGNAL(stopScanIssued()), scanController, SLOT(cancel()));
		connect(scanController, SIGNAL(progress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));
	}
	*/

		return true;
	}
	else
		return false;
}


void REIXSAppController::shutdown() {
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}


void REIXSAppController::onCurrentPaneChanged(QWidget *pane) {

	Q_UNUSED(pane)

}


#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

void REIXSAppController::onCurrentScanControllerStarted(){

	openScanInEditorAndTakeOwnership(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController()->scan());
}

void REIXSAppController::onCurrentScanControllerCreated(){

	// hook up bottom-bar. This should be moved up in the framework...
	connect(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController(), SIGNAL(progress(double,double)), this, SLOT(onProgressUpdated(double,double)));
}




