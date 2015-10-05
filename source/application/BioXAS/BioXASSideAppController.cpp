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
#include "ui/BioXAS/BioXASSidePersistentView.h"

BioXASSideAppController::BioXASSideAppController(QObject *parent)
	: BioXASAppController(parent)
{

}

BioXASSideAppController::~BioXASSideAppController()
{

}

bool BioXASSideAppController::startup()
{
	bool result = false;

	if (BioXASAppController::startup()) {

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {

			AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::BioXASSideBeamline), CLSFacilityID::BioXASSideBeamline); //6: BioXAS Side Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		result = true;
	}

	return result;
}

void BioXASSideAppController::initializeBeamline()
{
	BioXASSideBeamline::bioXAS();
}

void BioXASSideAppController::setupUserInterface()
{
	BioXASAppController::setupUserInterface();

	mw_->setWindowTitle("Acquaman - BioXAS Side");

	addPersistentView(new BioXASSidePersistentView());

	// Testing for AMControl::hasChildControl().

	if (BioXASSideBeamline::bioXAS()->jjSlits()->verticalGapControl()->hasChildControl(0))
		qDebug() << "\n\nThe JJ slit vertical gap has null child control. :(";
	else
		qDebug() << "\n\nThe JJ slit vertical gap does NOT have null child control. :)";


	if (BioXASSideBeamline::bioXAS()->jjSlits()->verticalGapControl()->hasChildControl(BioXASSideBeamline::bioXAS()->jjSlits()->upperBladeControl()))
		qDebug() << "\n\nThe JJ slit vertical gap has upper blade as child control. :)";
	else
		qDebug() << "\n\nThe JJ slit vertical gap does NOT have the upper blade as child control. :(";


	if (BioXASSideBeamline::bioXAS()->jjSlits()->verticalGapControl()->hasChildControl(BioXASSideBeamline::bioXAS()->carbonFilterFarm()->filterControl()))
		qDebug() << "\n\nThe JJ slit vertical gap control has the carbon filter farm control as a child. :(";
	else
		qDebug() << "\n\nThe JJ slit vertical gap control does NOT have the carbon filter farm control as child. :)";


	if (BioXASSideBeamline::bioXAS()->carbonFilterFarm()->filterControl()->hasChildControl(BioXASSideBeamline::bioXAS()->carbonFilterFarm()->upstreamPositionControl()))
		qDebug() <<  "\n\nThe carbon filter farm filter control has the upstream position control as a child. :)";
	else
		qDebug() << "\n\nThe carbon filter farm filter control does NOT have the upstream position control as a child. :(";
}

bool BioXASSideAppController::setupDataFolder()
{
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-s/AcquamanSideData", "/home/bioxas-s/AcquamanSideData", "users", QStringList());
}

void BioXASSideAppController::setupXASScanConfiguration(BioXASXASScanConfiguration *configuration)
{
	// Start with default XAS settings.

	BioXASAppController::setupXASScanConfiguration(configuration);

	if (configuration) {

		// Set the configuration detectors.

		AMDetector *encoderEnergyFeedback = BioXASSideBeamline::bioXAS()->encoderEnergyFeedbackDetector();
		if (encoderEnergyFeedback && encoderEnergyFeedback->isConnected())
			configuration->addDetector(encoderEnergyFeedback->toInfo());

		AMDetector *stepEnergyFeedback = BioXASSideBeamline::bioXAS()->stepEnergyFeedbackDetector();
		if (stepEnergyFeedback && stepEnergyFeedback->isConnected())
			configuration->addDetector(stepEnergyFeedback->toInfo());

		AMDetector *goniometerAngle = BioXASSideBeamline::bioXAS()->braggDetector();
		if (goniometerAngle && goniometerAngle->isConnected())
			configuration->addDetector(goniometerAngle->toInfo());

		AMDetector *goniometerStepSetpoint = BioXASSideBeamline::bioXAS()->braggStepSetpointDetector();
		if (goniometerStepSetpoint && goniometerStepSetpoint->isConnected())
			configuration->addDetector(goniometerStepSetpoint->toInfo());
	}
}
