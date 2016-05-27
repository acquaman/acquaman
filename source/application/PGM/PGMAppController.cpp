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
}

bool PGMAppController::startup()
{
    // Get a destination folder.
	if (!AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/pgm", "/home/pgm", "users"))
		return false;

	// Start up the main program.
	if(CLSAppController::startup()) {
		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);
		return true;
	}
	else
		return false;
}

void PGMAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	CLSAppController::shutdown();
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

void PGMAppController::registerClasses()
{

}

void PGMAppController::setupExporterOptions()
{

}

void PGMAppController::setupUserConfiguration()
{

}

void PGMAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - PGM");

	// Create panes in the main window:
	////////////////////////////////////

	QString generalPaneIcon_ = ":/system-software-update.png";
	QString generalPaneCategeryName_ = "General";

	mw_->insertHeading("General", 0);
	beamlineStatusView_ = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), false);
	mw_->addPane(AMMainWindow::buildMainWindowPane("Beamline Status", generalPaneIcon_, beamlineStatusView_), generalPaneCategeryName_, "Beamline Status", generalPaneIcon_);

	mw_->insertHeading("XRF Detectors", 1);

	mw_->insertHeading("Scans", 2);


    pgmPersistentView_ = new PGMPersistentView;
	connect( pgmPersistentView_, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)), this, SLOT(goToBeamStatusView(AMControl*)) );
	mw_->addRightWidget(pgmPersistentView_);
}

void PGMAppController::makeConnections()
{

}

