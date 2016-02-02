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
#include "dataman/AMXASScan.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DDerivativeAB.h"
#include "analysis/AM1DDarkCurrentCorrectionAB.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "analysis/AM1DNormalizationAB.h"
#include "dataman/export/AMExporterXDIFormat.h"
#include "dataman/export/AMExporterOptionXDIFormat.h"
#include "application/BioXAS/BioXAS.h"
#include "application/AMAppControllerSupport.h"

BioXASSideXASScanActionController::BioXASSideXASScanActionController(BioXASSideXASScanConfiguration *configuration, QObject *parent) :
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

	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", configuration_->edge().split(" ").first(), configuration_->edge().split(" ").last(), true);

	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASSideXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());

	AMControlInfoList list;
	list.append(BioXASSideBeamline::bioXAS()->mono()->energyControl()->toInfo());
	configuration_->setAxisControlInfos(list);

	useFeedback_ = true;

	AMDetectorInfoSet bioXASDetectors;
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i0Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i1Detector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->i2Detector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->encoderEnergySetpointDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->encoderEnergyFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->stepEnergyFeedbackDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->dwellTimeDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggDetector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggEncoderFeedbackDetector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggMoveRetriesDetector()->toInfo());
	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggStepSetpointDetector()->toInfo());
//	bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->braggStepMotorFeedbackDetector()->toInfo());

	if (configuration_->usingXRFDetector())
		bioXASDetectors.addDetectorInfo(BioXASSideBeamline::bioXAS()->ge32ElementDetector()->toInfo());

	configuration_->setDetectorConfigurations(bioXASDetectors);

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

BioXASSideXASScanActionController::~BioXASSideXASScanActionController()
{

}

void BioXASSideXASScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

QString BioXASSideXASScanActionController::beamlineSettings()
{
	QString notes;

	// Note storage ring current.

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(CLSStorageRing::sr1()->ringCurrent()));

	// Note the mono settling time, if applicable.

	double settlingTime = BioXASSideBeamline::bioXAS()->mono()->braggMotor()->settlingTime();
	if (settlingTime > 0)
		notes.append(QString("Settling time:\t%1 s\n").arg(settlingTime));

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

	// Commented out Aug 24, 2015. The standards wheel motor can't move and would like to proceed with scanning.
//	CLSStandardsWheel *standardsWheel = BioXASSideBeamline::bioXAS()->standardsWheel();

//	if (standardsWheel->indexFromName(configuration_->edge().split(" ").first()) != -1)
//		initializationAction->addSubAction(standardsWheel->createMoveToNameAction(configuration_->edge().split(" ").first()));

//	else
//		initializationAction->addSubAction(standardsWheel->createMoveToNameAction("None"));

	return initializationAction;
}

AMAction3* BioXASSideXASScanActionController::createCleanupActions()
{
	CLSSIS3820Scaler *scaler = BioXASSideBeamline::bioXAS()->scaler();

	AMListAction3 *cleanup = new AMListAction3(new AMListActionInfo3("BioXAS Side Cleanup", "BioXAS Cleanup"), AMListAction3::Sequential);
	cleanup->addSubAction(scaler->createContinuousEnableAction3(true));
	cleanup->addSubAction(scaler->createDwellTimeAction3(1.0));

	// Set the bragg motor power to PowerAutoSoftware.
	cleanup->addSubAction(BioXASSideBeamline::bioXAS()->mono()->braggMotor()->createPowerAction(CLSMAXvMotor::PowerAutoSoftware));

	return cleanup;
}

void BioXASSideXASScanActionController::buildScanControllerImplementation()
{
	// Identify data sources for the scaler channels.

	AMDataSource *i0DetectorSource = 0;
	AMDataSource *i1DetectorSource = 0;
	AMDataSource *i2DetectorSource = 0;

	int i0DetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->i0Detector()->name());
	if (i0DetectorIndex != -1) {
		i0DetectorSource = scan_->dataSourceAt(i0DetectorIndex);
	}

	int i1DetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->i1Detector()->name());
	if (i1DetectorIndex != -1) {
		i1DetectorSource = scan_->dataSourceAt(i1DetectorIndex);
	}

	int i2DetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->i2Detector()->name());
	if (i2DetectorIndex != -1) {
		i2DetectorSource = scan_->dataSourceAt(i2DetectorIndex);
	}

	// Create analyzed data sources for the monochromator testing measurements.

	AMDataSource *energySetpointSource = 0;
	AMDataSource *energyFeedbackSource = 0;
	AM1DExpressionAB *deltaEnergySource = 0;

	int energySetpointDetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->encoderEnergySetpointDetector()->name());
	if (energySetpointDetectorIndex != -1) {
		energySetpointSource = scan_->dataSourceAt(energySetpointDetectorIndex);
	}

	int energyFeedbackDetectorIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->encoderEnergyFeedbackDetector()->name());
	if (energyFeedbackDetectorIndex != -1) {
		energyFeedbackSource = scan_->dataSourceAt(energyFeedbackDetectorIndex);
	}

	if (energySetpointSource && energyFeedbackSource) {
		deltaEnergySource = new AM1DExpressionAB("EncoderEnergySetpointFeedback");
		deltaEnergySource->setInputDataSources(QList<AMDataSource *>() << energySetpointSource << energyFeedbackSource);
		deltaEnergySource->setExpression(QString("%1-%2").arg(energySetpointSource->name()).arg(energyFeedbackSource->name()));

		scan_->addAnalyzedDataSource(deltaEnergySource, true, false);
	}

	// Create analyzed data source for the absorbance.

	AM1DExpressionAB *absorbanceSource = 0;

	if (i0DetectorSource && i1DetectorSource && i2DetectorSource) {
		absorbanceSource = new AM1DExpressionAB("Absorbance");
		absorbanceSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << i1DetectorSource << i2DetectorSource);
		absorbanceSource->setExpression(QString("ln(%1/%2)").arg(i1DetectorSource->name(), i2DetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance.

	AM1DDerivativeAB *derivAbsorbanceSource = 0;

	if (absorbanceSource) {
		derivAbsorbanceSource = new AM1DDerivativeAB("DerivAbsorbance");
		derivAbsorbanceSource->setInputDataSources(QList<AMDataSource*>() << absorbanceSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceSource, true, false);
	}

	// Create analyzed data sources for the dark current corrected scaler channel detectors.

	AMDataSource *dwellTimeSource = 0;
	AM1DDarkCurrentCorrectionAB *i0CorrectedDetectorSource = 0;
	AM1DDarkCurrentCorrectionAB *i1CorrectedDetectorSource = 0;
	AM1DDarkCurrentCorrectionAB *i2CorrectedDetectorSource = 0;

	int dwellTimeIndex = scan_->indexOfDataSource(BioXASSideBeamline::bioXAS()->dwellTimeDetector()->name());
	if (dwellTimeIndex != -1) {
		dwellTimeSource = scan_->dataSourceAt(dwellTimeIndex);
	}

	if (dwellTimeSource && i0DetectorSource) {
		i0CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i0DetectorSource->name()));
		i0CorrectedDetectorSource->setDataName(i0DetectorSource->name());
		i0CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i0CorrectedDetectorSource->setDarkCurrent(BioXASSideBeamline::bioXAS()->exposedDetectorByName(i0DetectorSource->name())->darkCurrentValue());
		i0CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i0DetectorSource << dwellTimeSource);
		i0CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASSideBeamline::bioXAS()->i0Detector(), SIGNAL(darkCurrentValueChanged(double)), i0CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i0CorrectedDetectorSource, true, false);
	}

	if (dwellTimeSource && i1DetectorSource) {
		i1CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i1DetectorSource->name()));
		i1CorrectedDetectorSource->setDataName(i1DetectorSource->name());
		i1CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i1CorrectedDetectorSource->setDarkCurrent(BioXASSideBeamline::bioXAS()->exposedDetectorByName(i1DetectorSource->name())->darkCurrentValue());
		i1CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i1DetectorSource << dwellTimeSource);
		i1CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASSideBeamline::bioXAS()->i1Detector(), SIGNAL(darkCurrentValueChanged(double)), i1CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i1CorrectedDetectorSource, true, false);
	}

	if (dwellTimeSource && i2DetectorSource) {
		i2CorrectedDetectorSource = new AM1DDarkCurrentCorrectionAB(QString("%1_DarkCorrect").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDescription(QString("%1 Dark Current Corrected").arg(i2DetectorSource->name()));
		i2CorrectedDetectorSource->setDataName(i2DetectorSource->name());
		i2CorrectedDetectorSource->setDwellTimeName(dwellTimeSource->name());
		i2CorrectedDetectorSource->setDarkCurrent(BioXASSideBeamline::bioXAS()->exposedDetectorByName(i2DetectorSource->name())->darkCurrentValue());
		i2CorrectedDetectorSource->setInputDataSources(QList<AMDataSource*>() << i2DetectorSource << dwellTimeSource);
		i2CorrectedDetectorSource->setTimeUnitMultiplier(0.001);

		connect( BioXASSideBeamline::bioXAS()->i2Detector(), SIGNAL(darkCurrentValueChanged(double)), i2CorrectedDetectorSource, SLOT(setDarkCurrent(double)) );

		scan_->addAnalyzedDataSource(i2CorrectedDetectorSource, true, false);
	}

	// Create analyzed data source for the absorbance, dark current corrected values.

	AM1DExpressionAB *absorbanceCorrectedSource = 0;

	if (i0CorrectedDetectorSource && i1CorrectedDetectorSource && i2CorrectedDetectorSource) {
		absorbanceCorrectedSource = new AM1DExpressionAB("Absorbance_DarkCorrect");
		absorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << i0CorrectedDetectorSource << i1CorrectedDetectorSource << i2CorrectedDetectorSource);
		absorbanceCorrectedSource->setExpression(QString("ln(%1/%2)").arg(i1CorrectedDetectorSource->name(), i2CorrectedDetectorSource->name()));

		scan_->addAnalyzedDataSource(absorbanceCorrectedSource, true, false);
	}

	// Create analyzed data source for the derivative of the absorbance, dark current corrected values.

	AM1DDerivativeAB *derivAbsorbanceCorrectedSource = 0;

	if (absorbanceCorrectedSource) {
		derivAbsorbanceCorrectedSource = new AM1DDerivativeAB("DerivAbsorbance_DarkCorrect");
		derivAbsorbanceCorrectedSource->setInputDataSources(QList<AMDataSource*>() << absorbanceCorrectedSource);

		scan_->addAnalyzedDataSource(derivAbsorbanceCorrectedSource, true, false);
	}

	if (configuration_->usingXRFDetector()){

		AMXRFDetector *detector = BioXASSideBeamline::bioXAS()->ge32ElementDetector();
		detector->removeAllRegionsOfInterest();
		AMDataSource *spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));
		QString edgeSymbol = configuration_->edge().split(" ").first();

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, false, true);
			detector->addRegionOfInterest(region);

			AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0CorrectedDetectorSource);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0CorrectedDetectorSource->name());
			scan_->addAnalyzedDataSource(normalizedRegion, newRegion->name().contains(edgeSymbol), !newRegion->name().contains(edgeSymbol));
		}
	}
}

void BioXASSideXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

