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

#include "acquaman/BioXAS/BioXASSideXASScanConfiguration.h"

#include "analysis/AM1DExpressionAB.h"

#include "beamline/BioXAS/BioXASSideBeamline.h"

BioXASSideXASScanActionController::BioXASSideXASScanActionController(BioXASSideXASScanConfiguration *configuration, QObject *parent) :
	BioXASXASScanActionController(configuration, parent)
{
	configuration_ = configuration;

	// Set exporter option.

	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", configuration_->edge().split(" ").first(), configuration_->edge().split(" ").last(), true);
	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASSideXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());

	// Set detectors.

	AMDetectorInfoSet bioXASDetectors;
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i0Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i1Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i2Detector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->encoderEnergySetpointDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->encoderEnergyFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->stepEnergyFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->scalerDwellTimeDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggDetector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggEncoderFeedbackDetector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggMoveRetriesDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggStepSetpointDetector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggStepMotorFeedbackDetector()->toInfo());

	if (configuration_->usingXRFDetector())
		bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->ge32ElementDetector()->toInfo());

	setConfigurationDetectorConfigurations(bioXASDetectors);
}

BioXASSideXASScanActionController::~BioXASSideXASScanActionController()
{

}

QString BioXASSideXASScanActionController::beamlineSettings()
{
	QString notes;

	// Note storage ring current.

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(CLSStorageRing::sr1()->ringCurrent()));

	return notes;
}

AMAction3* BioXASSideXASScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *initializationAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("BioXAS Side Scan Initialization Actions", "BioXAS Side Scan Initialization Actions"));
	CLSSIS3820Scaler *scaler = BioXASSideBeamline::bioXAS()->scaler();

	double regionTime = double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());

	initializationAction->addSubAction(scaler->createContinuousEnableAction3(false));
	initializationAction->addSubAction(scaler->createDwellTimeAction3(regionTime));
	initializationAction->addSubAction(scaler->createStartAction3(true));
	initializationAction->addSubAction(scaler->createWaitForDwellFinishedAction(regionTime + 5.0));

	// Set the bragg motor power to PowerOn.
	initializationAction->addSubAction(BioXASSideBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerOn));

	if (configuration_->usingXRFDetector()){

		AMXspress3XRFDetector *detector = BioXASSideBeamline::bioXAS()->ge32ElementDetector();
		AMSequentialListAction3 *xspress3Setup = new AMSequentialListAction3(new AMSequentialListActionInfo3("Xspress3 setup", "Xspress3 Setup"));
		xspress3Setup->addSubAction(detector->createDisarmAction());
		xspress3Setup->addSubAction(detector->createFramesPerAcquisitionAction(int(configuration_->scanAxisAt(0)->numberOfPoints()*1.1)));	// Adding 10% just because.
		xspress3Setup->addSubAction(detector->createInitializationAction());
		initializationAction->addSubAction(xspress3Setup);
	}

	CLSStandardsWheel *standardsWheel = BioXASSideBeamline::bioXAS()->standardsWheel();

	if (standardsWheel->indexFromName(configuration_->edge().split(" ").first()) != -1)
		initializationAction->addSubAction(standardsWheel->createMoveToNameAction(configuration_->edge().split(" ").first()));

	else
		initializationAction->addSubAction(standardsWheel->createMoveToNameAction("None"));

	return initializationAction;
}

AMAction3* BioXASSideXASScanActionController::createCleanupActions()
{
	CLSSIS3820Scaler *scaler = BioXASSideBeamline::bioXAS()->scaler();

	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("BioXAS Side Cleanup", "BioXAS Cleanup"), AMListAction3::Sequential);
	cleanup->addSubAction(scaler->createDwellTimeAction3(1.0));
	cleanup->addSubAction(scaler->createContinuousEnableAction3(true));

	// Set the bragg motor power to PowerAutoSoftware.
	cleanup->addSubAction(BioXASSideBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerAutoSoftware));

	return cleanup;
}

void BioXASSideXASScanActionController::buildScanControllerImplementation()
{
	BioXASXASScanActionController::buildScanControllerImplementation();

//	// Create analyzed data sources for the monochromator testing measurements.

//	AMDataSource *energySetpointSource = 0;
//	AMDataSource *energyFeedbackSource = 0;
//	AM1DExpressionAB *deltaEnergySource = 0;

//	int energySetpointDetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->encoderEnergySetpointDetector()->name());
//	if (energySetpointDetectorIndex != -1) {
//		energySetpointSource = scan_->dataSourceAt(energySetpointDetectorIndex);
//	}

//	int energyFeedbackDetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->encoderEnergyFeedbackDetector()->name());
//	if (energyFeedbackDetectorIndex != -1) {
//		energyFeedbackSource = scan_->dataSourceAt(energyFeedbackDetectorIndex);
//	}

//	if (energySetpointSource && energyFeedbackSource) {
//		deltaEnergySource = new AM1DExpressionAB("EncoderEnergySetpointFeedback");
//		deltaEnergySource->setInputDataSources(QList<AMDataSource *>() << energySetpointSource << energyFeedbackSource);
//		deltaEnergySource->setExpression(QString("%1-%2").arg(energySetpointSource->name()).arg(energyFeedbackSource->name()));

//		scan_->addAnalyzedDataSource(deltaEnergySource, true, false);
//	}
}
