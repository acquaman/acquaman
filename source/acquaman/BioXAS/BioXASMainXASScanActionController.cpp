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
#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"

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
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->braggDetector()->toInfo());
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
    CLSSIS3820Scaler *scaler = CLSBeamline::clsBeamline()->scaler();
    double regionTime = double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

    if (scaler) {
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

	    // if we have a valid scaler on the beamline, perform a dark current measurement for the same length of time as the dwell time.
	    AMAction3 *darkCurrentSetup = scaler->createMeasureDarkCurrentAction((int)scaler->dwellTime());

	    initializationAction->addSubAction(stage1);
	    initializationAction->addSubAction(stage2);
	    initializationAction->addSubAction(scaler->createDwellTimeAction3(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));
	    initializationAction->addSubAction(stage3);
	    initializationAction->addSubAction(stage4);
	    initializationAction->addSubAction(darkCurrentSetup);

	    // Set the bragg motor power to PowerOn, must be on to move/scan.
	    initializationAction->addSubAction(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerOn));

    } else {
	    AMErrorMon::alert(this, BIOXASMAINXASSCANACTIONCONTROLLER_SCALER_NOT_FOUND, "Failed to complete scan initialization--valid scaler not found.");
    }

    return initializationAction;
}

AMAction3* BioXASMainXASScanActionController::createCleanupActions()
{
    CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();

    AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("BioXAS Main Cleanup", "BioXAS Main Cleanup"), AMListAction3::Sequential);
    cleanup->addSubAction(scaler->createDwellTimeAction3(1.0));
    cleanup->addSubAction(scaler->createContinuousEnableAction3(true));

    // Set the bragg motor power to PowerAutoHardware. The motor can get too warm when left on for too long, that's why we turn it off when not in use.
    cleanup->addSubAction(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerAutoHardware));

    return cleanup;
}

void BioXASMainXASScanActionController::buildScanControllerImplementation()
{
	// Create analyzed data sources for the monochromator testing measurements.

	int energyDetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->energyFeedbackDetector()->name());
	if (energyDetectorIndex != -1) {
		AMDataSource *energyFeedbackSource = scan_->dataSourceAt(energyDetectorIndex);

		AM1DExpressionAB *deltaEnergy = new AM1DExpressionAB("Energy feedback - setpoint");
		deltaEnergy->setInputDataSources(QList<AMDataSource*>() << energyFeedbackSource);
		deltaEnergy->setExpression("EnergyFeedback - EnergyFeedback.X");

		scan_->addAnalyzedDataSource(deltaEnergy, true, false);
	}

//	int braggDetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->braggDetector()->name());
//	if (braggDetectorIndex != -1) {
//		AMDataSource *braggDetectorDataSource = scan_->dataSourceAt(braggDetectorIndex);
//		scan_->addRawDataSource(braggDetectorDataSource);
//	}

//	int braggStepSetpointDetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->braggStepSetpointDetector()->name());
//	if (braggStepSetpointDetectorIndex != -1) {
//		AMDataSource *braggStepSetpointDataSource = scan_->dataSourceAt(braggStepSetpointDetectorIndex);
//		scan_->addRawDataSource(braggStepSetpointDataSource);
//	}

	// Create analyzed data sources for the dark current corrected measurements of each able detector in the configuration.
	// Add data sources to list of sources associated with this scan.

	int dwellTimeIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->dwellTimeDetector()->name());

	if (dwellTimeIndex != -1) {
		AMDataSource *dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);

		for (int i = 0, size = configuration_->detectorConfigurations().count(); i < size; i++) {
			AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(i));

			if (detector) {
				int detectorIndex = scan_->indexOfDataSource(detector->name());

				// If the detector is valid and able to perform dark current correction,
				// create an instance of the dark current correction analysis block and add it to the list of sources associated with this scan.
				if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {
					AMDataSource *detectorSource = scan_->dataSourceAt(detectorIndex);

					AM1DDarkCurrentCorrectionAB *detectorCorrected = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(detector->name()));
					detectorCorrected->setDescription(QString("%1 Dark Current Corrected").arg(detector->name()));
					detectorCorrected->setDataName(detectorSource->name());
					detectorCorrected->setDwellTimeName(dwellTimeSource->name());
					detectorCorrected->setInputDataSources(QList<AMDataSource*>() << detectorSource << dwellTimeSource);
					detectorCorrected->setTimeUnitMultiplier(0.001);

					connect( detector, SIGNAL(darkCurrentValueChanged(double)), detectorCorrected, SLOT(setDarkCurrent(double)) );

					scan_->addAnalyzedDataSource(detectorCorrected, true, false);

				} else {
					AMErrorMon::alert(this, BIOXASMAINXASSCANACTIONCONTROLLER_DETECTOR_NOT_FOUND, "Unable to find a required detector in scan controller.");
				}
			}
		}
	}
}

void BioXASMainXASScanActionController::createScanAssembler()
{
    scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}
