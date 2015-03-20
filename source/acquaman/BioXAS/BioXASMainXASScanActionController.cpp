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
#include "dataman/AMXASScan.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

BioXASMainXASScanActionController::BioXASMainXASScanActionController(BioXASMainXASScanConfiguration *configuration, QObject *parent) :
    AMStepScanActionController(configuration, parent)
{
    configuration_ = configuration;

    scan_ = new AMXASScan();
    scan_->setName(configuration_->name());
    scan_->setFileFormat("amCDFv1");
    scan_->setScanConfiguration(configuration);
    scan_->setIndexType("fileSystem");
    scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
    scan_->setNotes(beamlineSettings());

    AMControlInfoList list;
    configuration_->setAxisControlInfos(list);

    AMDetectorInfoSet detectorSet;
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->i0Detector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->iTDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->i2Detector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->energyFeedbackDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->dwellTimeDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggMoveRetriesDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggMoveRetriesMaxDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggStepSetpointDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggDegreeSetpointDetector()->toInfo());
    detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggAngleDetector()->toInfo());

    configuration_->setDetectorConfigurations(detectorSet);
}

BioXASMainXASScanActionController::~BioXASMainXASScanActionController()
{

}

QString BioXASMainXASScanActionController::beamlineSettings()
{
	QString notes;

	notes.append(QString("Bragg motor base velocity:\t%1").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->EGUBaseVelocity()));
	notes.append(QString("Bragg motor acceleration:\t%1").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->EGUAcceleration()));
	notes.append(QString("Bragg motor velocity:\t%1").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->EGUVelocity()));

	return notes;
}

AMAction3* BioXASMainXASScanActionController::createInitializationActions()
{
    AMSequentialListAction3 *initializationAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Main Scan Initialization Actions", "BioXAS Main Scan Initialization Actions"));
    CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();
    double regionTime = double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

    AMListAction3 *stage1 = new AMListAction3(new AMListActionInfo3("BioXAS Main Initialization Stage 1", "BioXAS Main Initialization Stage 1"), AMListAction3::Parallel);
    stage1->addSubAction(scaler->createContinuousEnableAction3(false));

    AMListAction3 *stage2 = new AMListAction3(new AMListActionInfo3("BioXAS Main Initialization Stage 2", "BioXAS Main Initialization Stage 2"), AMListAction3::Parallel);

    stage2->addSubAction(scaler->createStartAction3(false));
    stage2->addSubAction(scaler->createScansPerBufferAction3(1));
    stage2->addSubAction(scaler->createTotalScansAction3(1));

    AMListAction3 *stage3 = new AMListAction3(new AMListActionInfo3("BioXAS Main Initialization Stage 3", "BioXAS Main Initialization Stage 3"), AMListAction3::Parallel);
    stage3->addSubAction(scaler->createStartAction3(true));
    stage3->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));

    AMListAction3 *stage4 = new AMListAction3(new AMListActionInfo3("BioXAS Main Initialization Stage 4", "BioXAS Main Initialization Stage 4"), AMListAction3::Parallel);
    stage4->addSubAction(scaler->createStartAction3(true));
    stage4->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));

    initializationAction->addSubAction(stage1);
    initializationAction->addSubAction(stage2);
    initializationAction->addSubAction(scaler->createDwellTimeAction3(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));
    initializationAction->addSubAction(stage3);
    initializationAction->addSubAction(stage4);

    return initializationAction;
}

AMAction3* BioXASMainXASScanActionController::createCleanupActions()
{
    CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();

    AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("BioXAS Main Cleanup", "BioXAS Main Cleanup"), AMListAction3::Sequential);
    cleanup->addSubAction(scaler->createDwellTimeAction3(1.0));
    cleanup->addSubAction(scaler->createContinuousEnableAction3(true));

    return cleanup;
}

void BioXASMainXASScanActionController::buildScanControllerImplementation()
{

}

void BioXASMainXASScanActionController::createScanAssembler()
{
    scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}