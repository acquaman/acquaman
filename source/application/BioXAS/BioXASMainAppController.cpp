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
	: BioXASAppController(parent)
{

}

BioXASMainAppController::~BioXASMainAppController()
{

}

bool BioXASMainAppController::startup()
{
	bool result = false;

	if (BioXASAppController::startup()) {

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {

			AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::BioXASMainBeamline), CLSFacilityID::BioXASMainBeamline); //6: BioXAS Main Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		result = true;
	}

	return result;
}

void BioXASMainAppController::initializeBeamline()
{
	BioXASMainBeamline::bioXAS();
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

void BioXASMainAppController::setupXASScanConfiguration(BioXASXASScanConfiguration *configuration)
{
	// Start with default XAS settings.

	BioXASAppController::setupXASScanConfiguration(configuration);

	if (configuration) {

		// Set the configuration detectors.

		AMDetector *energySetpoint = BioXASMainBeamline::bioXAS()->energySetpointDetector();
		if (energySetpoint)
			configuration->addDetector(energySetpoint->toInfo());

		AMDetector *encoderEnergyFeedback = BioXASMainBeamline::bioXAS()->encoderEnergyFeedbackDetector();
		if (encoderEnergyFeedback && encoderEnergyFeedback->isConnected())
			configuration->addDetector(encoderEnergyFeedback->toInfo());

		AMDetector *stepEnergyFeedback = BioXASMainBeamline::bioXAS()->stepEnergyFeedbackDetector();
		if (stepEnergyFeedback && stepEnergyFeedback->isConnected())
			configuration->addDetector(stepEnergyFeedback->toInfo());

		AMDetector *goniometerAngle = BioXASMainBeamline::bioXAS()->braggDetector();
		if (goniometerAngle && goniometerAngle->isConnected())
			configuration->addDetector(goniometerAngle->toInfo());

		AMDetector *goniometerStepSetpoint = BioXASMainBeamline::bioXAS()->braggStepSetpointDetector();
		if (goniometerStepSetpoint && goniometerStepSetpoint->isConnected())
			configuration->addDetector(goniometerStepSetpoint->toInfo());

		AMDetector *goniometerEncoderStepFeedback = BioXASMainBeamline::bioXAS()->braggEncoderStepDegFeedbackDetector();
		if (goniometerEncoderStepFeedback && goniometerEncoderStepFeedback->isConnected())
			configuration->addDetector(goniometerEncoderStepFeedback->toInfo());
	}
}
