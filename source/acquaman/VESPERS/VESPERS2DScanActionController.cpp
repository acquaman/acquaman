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

	int yPoints = configuration_->scanAxisAt(1)->numberOfPoints();

	VESPERS::Motors motor = configuration_->motor();
	AMControlInfoList list;
	AMDetectorInfoSet detectors;

	if (motor.testFlag(VESPERS::H) && motor.testFlag(VESPERS::V)){

		list.append(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->toInfo());
		list.append(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleHFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleVFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::X) && motor.testFlag(VESPERS::Z)){

		list.append(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->toInfo());
		list.append(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleXFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SampleZFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::AttoH) && motor.testFlag(VESPERS::AttoV)){

		list.append(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->toInfo());
		list.append(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoHFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoVFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::AttoX) && motor.testFlag(VESPERS::AttoZ)){

		list.append(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->toInfo());
		list.append(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoXFeedback")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("AttoZFeedback")->toInfo());
	}

	else if (motor.testFlag(VESPERS::BigBeamX) && motor.testFlag(VESPERS::BigBeamZ)){

		list.append(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->toInfo());
		list.append(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->toInfo());
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

	if (xrfDetector.testFlag(VESPERS::Ge13Element)){

		detector = VESPERSBeamline::vespers()->exposedDetectorByName("Ge13El");
		detector->setHiddenFromUsers(true);
		detector->setIsVisible(false);
		detectors.addDetectorInfo(detector->toInfo());

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum1")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum2")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum3")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum4")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum5")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum6")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum7")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum8")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum9")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum10")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum11")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13ElementRawSpectrum12")->toInfo());
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
	VESPERS::FluorescenceDetectors xrfDetectorConfig = configuration_->fluorescenceDetector();

	AMXRFDetector *xrfDetector = VESPERSBeamline::vespers()->xrfDetector(xrfDetectorConfig);
	if (xrfDetector){

		xrfDetector->removeAllRegionsOfInterest();

		QList<AMDataSource *> i0Sources = QList<AMDataSource *>()
				<< scan_->dataSourceAt(scan_->indexOfDataSource("SplitIonChamber"))
				   << scan_->dataSourceAt(scan_->indexOfDataSource("PreKBIonChamber"))
					  << scan_->dataSourceAt(scan_->indexOfDataSource("MiniIonChamber"));

		AMDataSource *spectraSource = 0;

		if (xrfDetectorConfig.testFlag(VESPERS::SingleElement) && xrfDetectorConfig.testFlag(VESPERS::FourElement)){

			AM3DAdditionAB *sumSpectra = new AM3DAdditionAB("SingleAndFourSpectra");
			sumSpectra->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("SingleElementVortex")) << scan_->dataSourceAt(scan_->indexOfDataSource("FourElementVortex")));
			scan_->addAnalyzedDataSource(sumSpectra, false, true);
			spectraSource = sumSpectra;
		}

		else
			spectraSource = scan_->dataSourceAt(scan_->indexOfDataSource(xrfDetector->name()));

		foreach (AMRegionOfInterest *region, configuration_->regionsOfInterest()){

			xrfDetector->addRegionOfInterest(region);

			AMRegionOfInterestAB *regionAB = (AMRegionOfInterestAB *)region->valueSource();
			AMRegionOfInterestAB *newRegion = new AMRegionOfInterestAB(regionAB->name().remove(' '));
			newRegion->setBinningRange(regionAB->binningRange());
			newRegion->setInputDataSources(QList<AMDataSource *>() << spectraSource);
			scan_->addAnalyzedDataSource(newRegion, false, true);

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

	initializationActions->addSubAction(AMActionSupport::buildControlMoveAction(VESPERSBeamline::vespers()->endstation()->shutterControl(), 1.0));

	if (configuration_->normalPosition() != 888888.88){

		VESPERS::Motors motor = configuration_->motor();

		if (motor.testFlag(VESPERS::H) || motor.testFlag(VESPERS::V)
				|| motor.testFlag(VESPERS::AttoH) || motor.testFlag(VESPERS::AttoV))
			initializationActions->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalAxis()->createTranslateMoveAction(configuration_->normalPosition()));

		else if (motor.testFlag(VESPERS::X) || motor.testFlag(VESPERS::Z)
				 || motor.testFlag(VESPERS::AttoX) || motor.testFlag(VESPERS::AttoZ))
			initializationActions->addSubAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->normalAxis()->createTranslateMoveAction(configuration_->normalPosition()));

		else if (motor.testFlag(VESPERS::WireH) || motor.testFlag(VESPERS::WireV))
			initializationActions->addSubAction(VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->normalAxis()->createTranslateMoveAction(configuration_->normalPosition()));
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

	if (xrfDetector.testFlag(VESPERS::Ge13Element)){

		detector = VESPERSBeamline::vespers()->exposedDetectorByName("Ge13El");
		detector->setHiddenFromUsers(false);
		detector->setIsVisible(true);
	}

	AMSequentialListAction3 *cleanupActions = new AMSequentialListAction3(new AMSequentialListActionInfo3("Cleanup actions", "Cleanup actions"));
	cleanupActions->addSubAction(buildCleanupAction());

	if (configuration_->returnToOriginalPosition()){

		VESPERS::Motors motor = configuration_->motor();

		if (motor.testFlag(VESPERS::H) && motor.testFlag(VESPERS::V)){

			cleanupActions->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value()));
			cleanupActions->addSubAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value()));
		}

		else if (motor.testFlag(VESPERS::X) && motor.testFlag(VESPERS::Z)){

			cleanupActions->addSubAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->horizontalAxis()->translationMotor()->value()));
			cleanupActions->addSubAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->verticalAxis()->translationMotor()->value()));
		}

		else if (motor.testFlag(VESPERS::AttoH) && motor.testFlag(VESPERS::AttoV)){

			cleanupActions->addSubAction(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value()));
			cleanupActions->addSubAction(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value()));
		}

		else if (motor.testFlag(VESPERS::AttoX) && motor.testFlag(VESPERS::AttoZ)){

			cleanupActions->addSubAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->horizontalAxis()->translationMotor()->value()));
			cleanupActions->addSubAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->verticalAxis()->translationMotor()->value()));
		}

		else if (motor.testFlag(VESPERS::BigBeamX) && motor.testFlag(VESPERS::BigBeamZ)){

			cleanupActions->addSubAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->horizontalAxis()->translationMotor()->value()));
			cleanupActions->addSubAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->createTranslateMoveAction(VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->verticalAxis()->translationMotor()->value()));
		}
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
