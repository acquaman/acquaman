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


#include "BioXASSideXASScanActionController.h"

#include "acquaman/BioXASSide/BioXASSideXASScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

BioXASSideXASScanActionController::BioXASSideXASScanActionController(BioXASSideXASScanConfiguration *configuration, QObject *parent) :
    AMStepScanActionController(configuration, parent)
{
    configuration_ = configuration;

    scan_ = new AMXASScan();
    scan_->setFileFormat("amRegionAscii2013");
    scan_->setScanConfiguration(configuration);
    scan_->setIndexType("fileSystem");
    scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

    AMControlInfoList list;
    list.append(BioXASSideBeamline::bioXAS()->mono()->energyControl()->toInfo());
    configuration_->setAxisControlInfos(list);

    AMDetectorInfoSet bioXASDetectors;
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i0Detector()->toInfo());
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->iTDetector()->toInfo());
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->energyFeedbackDetector()->toInfo());
    configuration_->setDetectorConfigurations(bioXASDetectors);
}

BioXASSideXASScanActionController::~BioXASSideXASScanActionController()
{
}

AMAction3* BioXASSideXASScanActionController::createInitializationActions()
{
    AMListActionInfo3 *info = new AMListActionInfo3("BioXASSide Scan Initialization", "BioXASSide Scan Initialization");
    AMListAction3 *initializationActions = new AMListAction3(info);
    initializationActions->addSubAction(BioXASSideBeamline::bioXAS()->mono()->createSetBraggMotorPowerOnAction());

    return initializationActions;
}

AMAction3* BioXASSideXASScanActionController::createCleanupActions()
{
    AMListActionInfo3 *info = new AMListActionInfo3("BioXASSide Scan Cleanup", "BioXASSide Scan Cleanup");
    AMListAction3 *cleanupActions = new AMListAction3(info);
    cleanupActions->addSubAction(BioXASSideBeamline::bioXAS()->mono()->createSetBraggMotorPowerAutoAction());

    return cleanupActions;
}

void BioXASSideXASScanActionController::cancelImplementation()
{

}

void BioXASSideXASScanActionController::onInitializationActionsListSucceeded()
{

}

void BioXASSideXASScanActionController::buildScanControllerImplementation()
{

}

void BioXASSideXASScanActionController::createScanAssembler()
{
    scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

