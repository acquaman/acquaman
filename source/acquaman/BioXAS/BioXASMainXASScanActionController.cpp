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


#include "BioXASMainXASScanActionController.h"

#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"

#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"

BioXASMainXASScanActionController::BioXASMainXASScanActionController(BioXASMainXASScanConfiguration *configuration, QObject *parent) :
	BioXASXASScanActionController(configuration, parent)
{
	configuration_ = configuration;

	AMControlInfoList list;
	list.append(BioXASMainBeamline::bioXAS()->mono()->energyControl()->toInfo());

	configuration_->setAxisControlInfos(list);

	AMDetectorInfoSet bioXASDetectors;
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->i0Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->i1Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->i2Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->encoderEnergySetpointDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->encoderEnergyFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->stepEnergyFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->scalerDwellTimeDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggEncoderFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggMoveRetriesDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggStepSetpointDetector()->toInfo());

	configuration_->setDetectorConfigurations(bioXASDetectors);
}

BioXASMainXASScanActionController::~BioXASMainXASScanActionController()
{

}

AMAction3* BioXASMainXASScanActionController::createInitializationActions()
{
	AMAction3 *result = 0;
	AMAction3 *generalInitialization = BioXASXASScanActionController::createInitializationActions();

	if (generalInitialization) {
		AMListAction3 *mainInitialization = new AMListAction3(new AMListActionInfo3("BioXAS Main XAS Scan Initialization Actions", "BioXAS Main XAS Scan Initialization Actions"));

		mainInitialization->addSubAction(generalInitialization);

		// Set the bragg motor power to PowerOn, must be on to move/scan.
		mainInitialization->addSubAction(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerOn));

		result = mainInitialization;
	}

	return result;
}

AMAction3* BioXASMainXASScanActionController::createCleanupActions()
{
	AMAction3 *result = 0;
	AMAction3 *generalCleanup = BioXASXASScanActionController::createCleanupActions();

	if (generalCleanup) {
		AMListAction3 *mainCleanup = new AMListAction3(new AMListActionInfo3("BioXAS Main XAS Scan Cleanup Actions", "BioXAS Main XAS Scan Cleanup Actions"));

		mainCleanup->addSubAction(generalCleanup);

		// Set the bragg motor power to PowerAutoHardware. The motor can get too warm when left on for too long, that's why we turn it off when not in use.
		mainCleanup->addSubAction(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerAutoHardware));

		result = mainCleanup;
	}

	return result;
}
