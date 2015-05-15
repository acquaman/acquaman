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


#include "VESPERS2DScanActionController.h"

#include "actions3/AMListAction3.h"
#include "application/AMAppControllerSupport.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AM2DScan.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionSMAK.h"
#include "dataman/export/VESPERS/VESPERSExporter2DAscii.h"
#include "dataman/export/VESPERS/VESPERSExporterSMAK.h"
#include "analysis/AM2DNormalizationAB.h"
#include "analysis/AM3DAdditionAB.h"
#include "actions3/AMActionSupport.h"

VESPERS2DScanActionController::VESPERS2DScanActionController(VESPERS2DScanConfiguration *configuration, QObject *parent)
	: AMStepScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AM2DScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	if (configuration_->exportAsAscii()){

		AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERS2DDefault", configuration_->exportSpectraSources(), false, false, configuration_->exportSpectraInRows());
		if(vespersDefault->id() > 0)
			AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporter2DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());
	}

	else{

		// SMAK format requires a specific spectra file format.
		AMExporterOptionSMAK *vespersDefault = VESPERS::buildSMAKExporterOption("VESPERS2DDefault", configuration_->exportSpectraSources(), false, false, true);
		if(vespersDefault->id() > 0)
			AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporterSMAK, AMExporterOptionSMAK>(vespersDefault->id());
	}

	int yPoints = int(round((double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()))/double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()))) + 1;

	VESPERS::Motors motor = configuration_->motor();
	AMControlInfoList list;
	AMDetectorInfoSet detectors;

	if (motor.testFlag(VESPERS::H) && motor.testFlag(VESPERS::V)){

		list.append(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->toInfo());
		list.append(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleHFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleVFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::X) && motor.testFlag(VESPERS::Z)){

		list.append(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalControl()->toInfo());
		list.append(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalControl()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleXFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleZFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::AttoH) && motor.testFlag(VESPERS::AttoV)){

		list.append(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalControl()->toInfo());
		list.append(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalControl()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoHFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoVFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::AttoX) && motor.testFlag(VESPERS::AttoZ)){

		list.append(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalControl()->toInfo());
		list.append(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalControl()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoXFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoZFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::BigBeamX) && motor.testFlag(VESPERS::BigBeamZ)){

		list.append(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalControl()->toInfo());
		list.append(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalControl()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("BigBeamXFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("BigBeamZFeedback")->toInfo());
	}

	configuration_->setAxisControlInfos(list);

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MasterDwellTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RingCurrent")->toInfo());

	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();

	AMDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement)){

		detector = VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex");
		detector->setHiddenFromUsers(true);
		detector->setIsVisible(false);
		detectors.addDetectorInfo(detector->toInfo());

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexDeadTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRealTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexLiveTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexFastPeaks")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexSlowPeaks")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRawSpectrum")->toInfo());
	}

	if (xrfDetector.testFlag(VESPERS::FourElement)){

		detector = VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex");
		detector->setHiddenFromUsers(true);
		detector->setIsVisible(false);
		detectors.addDetectorInfo(detector->toInfo());

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexDeadTime4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRealTime4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexLiveTime4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexFastPeaks4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexSlowPeaks4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRawSpectrum1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRawSpectrum2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRawSpectrum3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortexRawSpectrum4")->toInfo());
	}

	VESPERS::CCDDetectors ccdDetector = configuration_->ccdDetector();

	if (ccdDetector.testFlag(VESPERS::Roper))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RoperFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Mar))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MarFileNumber")->toInfo());

	if (ccdDetector.testFlag(VESPERS::Pilatus))
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PilatusFileNumber")->toInfo());

	configuration_->setDetectorConfigurations(detectors);

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

void VESPERS2DScanActionController::buildScanControllerImplementation()
{
	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();
	AMXRFDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex"));

	else if (xrfDetector.testFlag(VESPERS::FourElement))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex"));

	if (detector){

		detector->removeAllRegionsOfInterest();

		QList<AMDataSource *> i0Sources = QList<AMDataSource *>()
				<< scan_->dataSourceAt(scan_->indexOfDataSource("SplitIonChamber"))
				   << scan_->dataSourceAt(scan_->indexOfDataSource("PreKBIonChamber"))
					  << scan_->dataSourceAt(scan_->indexOfDataSource("MiniIonChamber"));

		AMDataSource *spectraSource = 0;

		if (xrfDetector.testFlag(VESPERS::SingleElement) && xrfDetector.testFlag(VESPERS::FourElement)){

			AM3DAdditionAB *sumSpectra = new AM3DAdditionAB("SingleAndFourSpectra");
			sumSpectra->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("SingleElementVortex")) << scan_->dataSourceAt(scan_->indexOfDataSource("FourElementVortex")));
			scan_->addAnalyzedDataSource(sumSpectra, false, true);
			spectraSource = sumSpectra;
		}

		else
			spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(detector->name()));

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, false, true);
			detector->addRegionOfInterest(region);

			AM2DNormalizationAB *normalizedRegion = new AM2DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0Sources.at(int(configuration_->incomingChoice()))->name());
			scan_->addAnalyzedDataSource(normalizedRegion, true, false);
		}
	}
}

void VESPERS2DScanActionController::createAxisOrderMap()
{
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
	axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
}

AMAction3* VESPERS2DScanActionController::createInitializationActions()
{
	AMSequentialListAction3 *initializationActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Initialization actions", "Initialization actions"));
	initializationActions->addSubAction(buildBaseInitializationAction(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime())));

	if (!configuration_->ccdDetector().testFlag(VESPERS::NoCCD))
		initializationActions->addSubAction(buildCCDInitializationAction(configuration_->ccdDetector(),
										 configuration_->ccdFileName(),
										 scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString(" name = '%1'").arg(scan_->name()))+1));

	if (configuration_->normalPosition() != 888888.88){

		VESPERS::Motors motor = configuration_->motor();

		if (motor.testFlag(VESPERS::H) || motor.testFlag(VESPERS::V)
				|| motor.testFlag(VESPERS::AttoH) || motor.testFlag(VESPERS::AttoV))
			initializationActions->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(configuration_->normalPosition()));

		else if (motor.testFlag(VESPERS::X) || motor.testFlag(VESPERS::Z)
				 || motor.testFlag(VESPERS::AttoX) || motor.testFlag(VESPERS::AttoZ))
			initializationActions->addSubAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(configuration_->normalPosition()));

		else if (motor.testFlag(VESPERS::WireH) || motor.testFlag(VESPERS::WireV))
			initializationActions->addSubAction(VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->createNormalMoveAction(configuration_->normalPosition()));
	}

	return initializationActions;
}

AMAction3* VESPERS2DScanActionController::createCleanupActions()
{
	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();
	AMDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement)){

		detector = VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex");
		detector->setHiddenFromUsers(false);
		detector->setIsVisible(true);
	}

	if (xrfDetector.testFlag(VESPERS::FourElement)){

		detector = VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex");
		detector->setHiddenFromUsers(false);
		detector->setIsVisible(true);
	}

	AMSequentialListAction3 *cleanupActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Cleanup actions", "Cleanup actions"));
	cleanupActions->addSubAction(buildCleanupAction());

	VESPERS::Motors motor = configuration_->motor();

	if (motor.testFlag(VESPERS::H) && motor.testFlag(VESPERS::V)){

		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl(), VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value()));
		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl(), VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value()));
	}

	else if (motor.testFlag(VESPERS::X) && motor.testFlag(VESPERS::Z)){

		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalControl(), VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalControl()->value()));
		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalControl(), VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalControl()->value()));
	}

	else if (motor.testFlag(VESPERS::AttoH) && motor.testFlag(VESPERS::AttoV)){

		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalControl(), VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalControl()->value()));
		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalControl(), VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalControl()->value()));
	}

	else if (motor.testFlag(VESPERS::AttoX) && motor.testFlag(VESPERS::AttoZ)){

		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalControl(), VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalControl()->value()));
		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalControl(), VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalControl()->value()));
	}

	else if (motor.testFlag(VESPERS::BigBeamX) && motor.testFlag(VESPERS::BigBeamZ)){

		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalControl(), VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalControl()->value()));
		cleanupActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalControl(), VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalControl()->value()));
	}

	return cleanupActions;
}

void VESPERS2DScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
