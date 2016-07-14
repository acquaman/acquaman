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


#include "BioXASSideAppController.h"

#include "beamline/BioXAS/BioXASSideBeamline.h"

BioXASSideAppController::BioXASSideAppController(QObject *parent)
	: BioXASAppController("BioXAS", parent)
{

}

BioXASSideAppController::~BioXASSideAppController()
{

}

void BioXASSideAppController::updateGeDetectorView()
{
    BioXAS32ElementGeDetector *detector = BioXASSideBeamline::bioXAS()->ge32ElementDetector();
    QWidget *detectorView = componentViewMapping_.value(detector, 0);
    QWidget *detectorPane = viewPaneMapping_.value(detectorView, 0);

    if (detector && detectorView && detectorPane) {
		if (detector->isConnected())
			mw_->showPane(detectorPane);
		else
			mw_->hidePane(detectorPane);
	}
}

bool BioXASSideAppController::setupDataFolder()
{
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-s/AcquamanSideData",  //local directory
												   "/home/bioxas-s/AcquamanSideData",               //remote directory
												   "users",                                         //data directory
												   QStringList());                                  //extra data directory
}

void BioXASSideAppController::initializeBeamline()
{
	BioXASSideBeamline::bioXAS();
}

void BioXASSideAppController::setupUserInterfaceImplementation()
{
	BioXASAppController::setupUserInterfaceImplementation();

	mw_->setWindowTitle("Acquaman - BioXAS Side");

	connect( BioXASSideBeamline::bioXAS()->ge32ElementDetector(), SIGNAL(connected(bool)), this, SLOT(updateGeDetectorView()) );
	updateGeDetectorView();
}

void BioXASSideAppController::createDetectorPanes()
{
	BioXASAppController::createDetectorPanes();

	addMainWindowPane( createComponentView(BioXASSideBeamline::bioXAS()->ge32ElementDetector()), "Ge 32-el", detectorPaneCategoryName_, detectorPaneIcon_ );
}

void BioXASSideAppController::createComponentsPane()
{
	BioXASAppController::createComponentsPane();

	addMainWindowView(createComponentView(BioXASSideBeamline::bioXAS()->detectorStageLateralMotor()), "Ge 32-el Stage", componentPaneCategoryName_, componentPaneIcon_);

	// Collapse the 'Components' heading, by default.
	mw_->collapseHeading(componentPaneCategoryName_);
}
