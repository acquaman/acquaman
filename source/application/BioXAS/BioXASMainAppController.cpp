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


#include "BioXASMainAppController.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"

BioXASMainAppController::BioXASMainAppController(QObject *parent)
	: BioXASAppController("BioXAS", parent)
{

}

BioXASMainAppController::~BioXASMainAppController()
{

}

bool BioXASMainAppController::startup()
{
	bool result = false;

	if (BioXASAppController::startup()) {

		result = true;
	}

	return result;
}

void BioXASMainAppController::initializeBeamline()
{
	BioXASMainBeamline::bioXAS();
	setupScanConfigurations();
}

void BioXASMainAppController::setupUserInterface()
{
	// General BioXAS interface setup.
	BioXASAppController::setupUserInterface();

	// Main specific setup.

	mw_->setWindowTitle("Acquaman - BioXAS Main");

	addDetectorView(BioXASMainBeamline::bioXAS()->ge32DetectorInboard(), "Ge 32-el 1");
}

bool BioXASMainAppController::setupDataFolder()
{
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-m/AcquamanMainData", "/home/bioxas-m/AcquamanMainData", "users", QStringList());
}
