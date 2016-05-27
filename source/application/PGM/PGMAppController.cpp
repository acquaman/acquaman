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


#include "PGMAppController.h"

#include "beamline/AMControl.h"
#include "beamline/PGM/PGMBeamline.h"

#include "ui/AMMainWindow.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/PGM/PGMPersistentView.h"
#include "ui/util/AMChooseDataFolderDialog.h"

PGMAppController::PGMAppController(QObject *parent)
	: CLSAppController("PGM", parent)
{
	setDefaultUseLocalStorage(true);

	detectorPaneCategoryName_ = "XRF Detectors";
}

bool PGMAppController::setupDataFolder()
{
	// Get a destination folder.
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/pgm",  //local directory
												   "/home/pgm",               //remote directory
												   "users",                   //data directory
												   QStringList());            //extra data directory
}

void PGMAppController::goToBeamlineStatusView(AMControl *control)
{
	if (beamlineStatusView_) {

		// Set the given control as the view's selected control.

		beamlineStatusView_->setSelectedComponent(control);

// TODO: this has to be uncommented when the code refactor for CLSAppController is merged in
//		// Set the beam status pane as the current pane.

//		QWidget *windowPane = viewPaneMapping_.value(beamlineStatusView_, 0);

//		if (windowPane)
//			mw_->setCurrentPane(windowPane);
	}
}

void PGMAppController::initializeBeamline()
{
	// Initialize central beamline object
	PGMBeamline::pgm();
}

void PGMAppController::registerDBClasses()
{

}

void PGMAppController::registerExporterOptions()
{

}

void PGMAppController::setupScanConfigurations()
{

}

void PGMAppController::setupUserConfiguration()
{
}

void PGMAppController::setupUserInterfaceImplementation()
{
	mw_->setWindowTitle("Acquaman - PGM");
}

void PGMAppController::createPersistentView()
{
	pgmPersistentView_ = new PGMPersistentView;
	connect( pgmPersistentView_, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)), this, SLOT(goToBeamStatusView(AMControl*)) );

	mw_->addRightWidget(pgmPersistentView_);
}

void PGMAppController::createGeneralPanes()
{
	// create beamline status view
	beamlineStatusView_ = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), false);
	mw_->addPane(AMMainWindow::buildMainWindowPane("Beamline Status", generalPaneIcon_, beamlineStatusView_), generalPaneCategeryName_, "Beamline Status", generalPaneIcon_);
}

void PGMAppController::createDetectorPanes()
{
}

void PGMAppController::createScanConfigurationPanes()
{
}

