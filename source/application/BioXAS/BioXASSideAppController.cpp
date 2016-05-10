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

	// Side specific setup.
	mw_->setWindowTitle("Acquaman - BioXAS Side");
}

void BioXASSideAppController::createDetectorPanes()
{
	BioXASAppController::createDetectorPanes();

	addViewToPane( createComponentView(BioXASSideBeamline::bioXAS()->ge32ElementDetector()), "Ge 32-el", detectorPaneCategoryName_, detectorPaneIcon_ );
}

void BioXASSideAppController::createComponentsPane()
{
	BioXASAppController::createComponentsPane();

	QWidget * detectorStageMotorView = createComponentView(BioXASSideBeamline::bioXAS()->detectorStageLateralMotor());
	addMainWindowViewToPane(detectorStageMotorView, "Ge 32-el Stage", componentPaneCategoryName_, generalPaneIcon_);

	// Collapse the 'Components' heading, by default.
	mw_->collapseHeading(componentPaneCategoryName_);
}
