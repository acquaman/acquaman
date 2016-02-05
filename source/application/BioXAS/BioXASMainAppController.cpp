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
#include "ui/BioXAS/BioXASMainPersistentView.h"

BioXASMainAppController::BioXASMainAppController(QObject *parent)
	: BioXASAppController(parent)
{
	ssrlMonoTuningConfiguration_ = 0;
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

void BioXASMainAppController::onCurrentScanActionFinishedImplementation(AMScanAction *action)
{
	if (action && action->controller() && action->controller()->scan() && action->controller()->scan()->scanConfiguration()) {
		if (action->controller()->scan()->scanConfiguration()->name() == "SSRL Mono Tuning Scan")
			onSSRLMonochromatorCrystalTuningScanFinished(action->controller()->scan());
	}
}

void BioXASMainAppController::onSSRLMonochromatorCrystalTuningScanFinished(AMScan *scanResults)
{

}

void BioXASMainAppController::initializeBeamline()
{
	BioXASMainBeamline::bioXAS();
}

void BioXASMainAppController::setupUserInterface()
{
	BioXASAppController::setupUserInterface();

	mw_->setWindowTitle("Acquaman - BioXAS Main");

	ssrlMonoTuningConfigurationView_ = createScanConfigurationViewWithHolder(ssrlMonoTuningConfiguration_);
	addViewToScansPane(ssrlMonoTuningConfigurationView_, "SSRL Mono Tuning");

	addPersistentView(new BioXASMainPersistentView());
}

bool BioXASMainAppController::setupDataFolder()
{
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-m/AcquamanMainData", "/home/bioxas-m/AcquamanMainData", "users", QStringList());
}

void BioXASMainAppController::setupScanConfigurations()
{
	BioXASAppController::setupScanConfigurations();

	ssrlMonoTuningConfiguration_ = new AMGenericStepScanConfiguration();
	ssrlMonoTuningConfiguration_->setName("SSRL Mono Tuning Scan");
	ssrlMonoTuningConfiguration_->setUserScanName("SSRL Mono Tuning Scan");
	setupSSRLMonochromatorCrystalTuningScanConfiguration(ssrlMonoTuningConfiguration_);
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

void BioXASMainAppController::setupSSRLMonochromatorCrystalTuningScanConfiguration(AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {

		configuration->setAutoExportEnabled(false);

		// Set the current crystal pitch motor as the scanned motor. This needs to be updated when the region changes!

		AMControl *crystalPitch = BioXASMainBeamline::bioXAS()->mono()->crystal1Pitch();
		ssrlMonoTuningConfiguration_->setControl(0, crystalPitch->toInfo());

		// Set scan detectors.

		AMDetector *i0Detector = BioXASBeamline::bioXAS()->i0Detector();
		if (i0Detector)
			configuration->addDetector(i0Detector->toInfo());
	}
}

