#include "BioXASSideXASScanActionController.h"

#include "acquaman/BioXASSide/BioXASSideXASScanConfiguration.h"
#include "dataman/AMXASScan.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"

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
    list.append(BioXASSideBeamline::bioXAS()->m1UpperSlit()->toInfo());
    configuration_->setAxisControlInfos(list);

    AMDetectorInfoSet bioXASDetectors;
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->testDetector()->toInfo());
    configuration_->setDetectorConfigurations(bioXASDetectors);
}

AMAction3* BioXASSideXASScanActionController::createInitializationActions()
{
    return 0;
}

AMAction3* BioXASSideXASScanActionController::createCleanupActions()
{
    return 0;
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
    scanAssembler_ = new AMGenericScanActionControllerAssembler(this);
}

