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

    AMControlInfoList list;
    list.append(BioXASSideBeamline::bioXAS()->mono()->energyControl()->toInfo());
    configuration_->setAxisControlInfos(list);

    AMDetectorInfoSet bioXASDetectors;
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i0Detector()->toInfo());
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->iTDetector()->toInfo());
    bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->energyFeedbackDetector()->toInfo());

    configuration_->setDetectorConfigurations(bioXASDetectors);

    scan_ = new AMXASScan();
    scan_->setFileFormat("amRegionAscii2013");
    scan_->setScanConfiguration(configuration);
    scan_->setIndexType("fileSystem");
    scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));
}

BioXASSideXASScanActionController::~BioXASSideXASScanActionController()
{

}

AMAction3* BioXASSideXASScanActionController::createInitializationActions()
{
    AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3("BioXASSide XAS Scan Initialization", "BioXASSide XAS Scan Initialization"));

    // turn on the mono bragg motor.

    AMListAction3 *braggMotorSetup = new AMListAction3(new AMListActionInfo3("BioXAS Bragg Motor Setup", "BioXAS Bragg Motor Setup"), AMListAction3::Sequential);
    braggMotorSetup->addSubAction(BioXASSideBeamline::bioXAS()->mono()->createSetBraggMotorPowerOnAction());

    // set up dark current correction.

    AMListAction3 *darkCurrentSetup = new AMListAction3(new AMListActionInfo3("BioXAS Dark Current Setup", "BioXAS Dark Current Setup"), AMListAction3::Sequential);

    for (int i = 0, size = configuration_->detectorConfigurations().count(); i < size; i++) {
        AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(i));

        bool sharedSouceFound = false;

        if (detector) {
            int detectorIndex = scan_->indexOfDataSource(detector->name());

            if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {
                bool isSourceShared = detector->sharesDetectorTriggerSource();

                if (isSourceShared && !sharedSourceFound) {
                    sharedSourceFound = true;
                    darkCurrentSetup->addSubAction(detector->createDarkCurrentCorrectionActions(10));
                } else if (!isSourceShared) {
                    darkCurrentSetup->addSubAction(detector->createDarkCurrentCorrectionActions(10));
                }
            }
        }
    }

    // add setup actions to total list of initialization actions.

    initializationActions->addSubAction(braggMotorSetup);
    initializationActions->addSubAction(darkCurrentSetup);

    return initializationActions;
}

AMAction3* BioXASSideXASScanActionController::createCleanupActions()
{
    AMListActionInfo3 *info = new AMListActionInfo3("BioXASSide Scan Cleanup", "BioXASSide Scan Cleanup");
    AMListAction3 *cleanupActions = new AMListAction3(info);
    cleanupActions->addSubAction(BioXASSideBeamline::bioXAS()->mono()->createSetBraggMotorPowerAutoAction());

    return cleanupActions;
}

void BioXASSideXASScanActionController::buildScanControllerImplementation()
{
    // Create data sources of dark current corrected measurements of each detector in the configuration_ that can perform dark current correction.
    // Add data sources to list of analyzed data sources.

    int dwellTimeIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->dwellTimeDetector()->name());

    if (dwellTimeIndex != -1) {
        AMDataSource *dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);

        for (int i = 0, size = configuration_->detectorConfigurations().count(); i < size; i++) {
            AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(i));

            if (detector) {
                int detectorIndex = scan_->indexOfDataSource(detector->name());

                if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {
                    AMDataSource *detectorSource = scan_->dataSourceAt(detectorIndex);

                    AM1DDarkCurrentCorrectionAB *detectorCorrection = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(detector->name()));
                    detectorCorrection->setDescription(QString("%1 Dark Current Correction").arg(detector->name()));
                    detectorCorrection->setDataName(detectorSource->name());
                    detectorCorrection->setDwellTimeName(dwellTimeSource->name());
                    detectorCorrection->setInputDataSources(QList<AMDataSource*>() << detectorSource << dwellTimeSource);
                    detectorCorrection->setTimeUnitMultiplier(0.001);

                    connect( detector, SIGNAL(newDarkCurrentMeasurementValueReady(double)), detectorCorrection, SLOT(setDarkCurrent(double)) );

                    scan_->addAnalyzedDataSource(detectorCorrection, true, false);
                }
            }
        }
    }
}

void BioXASSideXASScanActionController::createScanAssembler()
{
    scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

