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
#include "analysis/AM1DDerivativeAB.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

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
	list.append(BioXASMainBeamline::bioXAS()->mono()->energyControl()->toInfo());
	configuration_->setAxisControlInfos(list);

	useFeedback_ = true;

	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("I0Detector")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("I1Detector")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("I2Detector")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("EnergySetpoint")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("EnergyFeedback")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("DwellTimeFeedback")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("MonoFeedback")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("MonoMoveRetries")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("MonoStepSetpoint")->toInfo());
	detectorSet.addDetectorInfo(BioXASMainBeamline::bioXAS()->exposedDetectorByName("BraggAngle")->toInfo());

	configuration_->setDetectorConfigurations(detectorSet);

	secondsElapsed_ = 0;
	secondsTotal_ = configuration_->totalTime();
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));
}

BioXASMainXASScanActionController::~BioXASMainXASScanActionController()
{

}

void BioXASMainXASScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

QString BioXASMainXASScanActionController::beamlineSettings()
{
	QString notes;

	notes.append(QString("Bragg motor base velocity:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->EGUBaseVelocity()));
	notes.append(QString("Bragg motor acceleration:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->EGUAcceleration()));
	notes.append(QString("Bragg motor velocity:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->EGUVelocity()));
	notes.append(QString("Bragg motor max retries:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->maxRetries()));
	notes.append(QString("Bragg motor encoder move type:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->encoderMovementType()));
	notes.append(QString("Bragg motor encoder step soft ratio:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->encoderStepSoftRatio()));
	notes.append(QString("Bragg motor encoder slope:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->encoderCalibrationSlope()));
	notes.append(QString("Bragg motor step calibration slope:\t%1\n").arg(BioXASMainBeamline::bioXAS()->mono()->braggMotor()->stepCalibrationSlope()));

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

		// Turn beam on.
		AMAction3 *beamOn = BioXASMainBeamline::bioXAS()->createTurnOnBeamActions();

		initializationAction->addSubAction(stage1);
		initializationAction->addSubAction(stage2);
		initializationAction->addSubAction(scaler->createDwellTimeAction3(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));
		initializationAction->addSubAction(stage3);
		initializationAction->addSubAction(stage4);
		initializationAction->addSubAction(darkCurrentSetup);
		initializationAction->addSubAction(beamOn);

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

	AMDataSource *energySetpointSource = 0;
	AMDataSource *energyFeedbackSource = 0;
	AM1DExpressionAB *deltaEnergySource = 0;

	int energySetpointDetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->energySetpointDetector()->name());
	if (energySetpointDetectorIndex) {
		energySetpointSource = scan_->dataSourceAt(energySetpointDetectorIndex);
	}

	int energyFeedbackDetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->energyFeedbackDetector()->name());
	if (energyFeedbackDetectorIndex != -1) {
		energyFeedbackSource = scan_->dataSourceAt(energyFeedbackDetectorIndex);
	}

	if (energySetpointSource && energyFeedbackSource) {
		deltaEnergySource = new AM1DExpressionAB("EnergySetpointFeedback");
		deltaEnergySource->setInputDataSources(QList<AMDataSource *>() << energySetpointSource << energyFeedbackSource);
		deltaEnergySource->setExpression("EnergySetpoint-EnergyFeedback");

		scan_->addAnalyzedDataSource(deltaEnergySource, true, false);
	}

	// Create analyzed data source for the absorbance.

	AMDataSource *i0DetectorSource = 0;
	AMDataSource *i2DetectorSource = 0;
	AM1DExpressionAB *absorbanceSource = 0;

	int i0DetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->i0Detector()->name());
	if (i0DetectorIndex != -1) {
		i0DetectorSource = scan_->dataSourceAt(i0DetectorIndex);
	}

	int i2DetectorIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->i2Detector()->name());
	if (i2DetectorIndex != -1) {
		i2DetectorSource = scan_->dataSourceAt(i2DetectorIndex);
	}

	if (i0DetectorSource && i2DetectorSource) {
		absorbanceSource = new AM1DExpressionAB("Absorbance");
		absorbanceSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << i2DetectorSource);
		absorbanceSource->setExpression("log(I0Detector/I2Detector)");

		scan_->addAnalyzedDataSource(absorbanceSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance.

	AM1DDerivativeAB *derivAbsorbanceSource = 0;

	if (absorbanceSource) {
		derivAbsorbanceSource = new AM1DDerivativeAB("DerivAbsorbance");
		derivAbsorbanceSource->setInputDataSources(QList<AMDataSource*>() << absorbanceSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceSource, true, false);
	}

	// Create analyzed data sources for the dark current corrected measurements of each able detector in the configuration.

	AMDataSource *dwellTimeSource = 0;

	int dwellTimeIndex = scan_->indexOfDataSource(BioXASMainBeamline::bioXAS()->dwellTimeDetector()->name());
	if (dwellTimeIndex != -1) {
		dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);

		for (int i = 0, size = configuration_->detectorConfigurations().count(); i < size; i++) {
			AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(configuration_->detectorConfigurations().at(i));

			if (detector) {
				int detectorIndex = scan_->indexOfDataSource(detector->name());

				// If the detector is valid and able to perform dark current correction,
				// create an instance of the dark current correction analysis block and add it to the list of sources associated with this scan.

				if (detectorIndex != -1 && detector->rank() == 0 && detector->canDoDarkCurrentCorrection()) {
					AMDataSource *detectorSource = scan_->dataSourceAt(detectorIndex);

					AM1DDarkCurrentCorrectionAB *detectorCorrectedSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(detector->name()));
					detectorCorrectedSource->setDescription(QString("%1 Dark Current Corrected").arg(detector->name()));
					detectorCorrectedSource->setDataName(detectorSource->name());
					detectorCorrectedSource->setDwellTimeName(dwellTimeSource->name());
					detectorCorrectedSource->setInputDataSources(QList<AMDataSource*>() << detectorSource << dwellTimeSource);
					detectorCorrectedSource->setTimeUnitMultiplier(0.001);

					connect( detector, SIGNAL(darkCurrentValueChanged(double)), detectorCorrectedSource, SLOT(setDarkCurrent(double)) );

					scan_->addAnalyzedDataSource(detectorCorrectedSource, true, false);
				}

			} else {
				AMErrorMon::alert(this, BIOXASMAINXASSCANACTIONCONTROLLER_DETECTOR_NOT_FOUND, "Unable to find a required detector in scan controller.");
			}
		}
	}
}

void BioXASMainXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}
