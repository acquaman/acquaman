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
