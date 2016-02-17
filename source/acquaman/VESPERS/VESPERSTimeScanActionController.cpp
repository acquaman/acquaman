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


#include "VESPERSTimeScanActionController.h"

#include "actions3/AMListAction3.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "dataman/AMLineScan.h"
#include "dataman/export/VESPERS/VESPERSExporterLineScanAscii.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM2DAdditionAB.h"

VESPERSTimeScanActionController::VESPERSTimeScanActionController(VESPERSTimeScanConfiguration *configuration, QObject *parent)
	: AMTimedScanActionController(configuration, parent), VESPERSScanController(configuration)
{
	configuration_ = configuration;

	scan_ = new AMLineScan();
	scan_->setName(configuration_->name());
	scan_->setScanConfiguration(configuration_);
	scan_->setFileFormat("amCDFv1");
	scan_->setIndexType("fileSystem");
	scan_->setNotes(buildNotes());

	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSTimeScanDefault", configuration_->exportSpectraSources(), false, false, configuration_->exportSpectraInRows());
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSTimeScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	scan_->rawData()->addScanAxis(AMAxisInfo("Time", 0, "Time", "s"));

	AMDetectorInfoSet detectors;
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SplitIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PreKBIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MiniIonChamber")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("PostIonChamber")->toInfo());

	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("MasterDwellTime")->toInfo());
	detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("RingCurrent")->toInfo());

	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();

	if (xrfDetector.testFlag(VESPERS::SingleElement)){

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexDeadTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRealTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexLiveTime")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexFastPeaks")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexSlowPeaks")->toInfo());
		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortexRawSpectrum")->toInfo());
	}

	if (xrfDetector.testFlag(VESPERS::FourElement)){

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex")->toInfo());
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

		detectors.addDetectorInfo(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13El")->toInfo());
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

void VESPERSTimeScanActionController::buildScanControllerImplementation()
{
	VESPERS::FluorescenceDetectors xrfDetector = configuration_->fluorescenceDetector();
	AMXRFDetector *detector = 0;

	if (xrfDetector.testFlag(VESPERS::SingleElement))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("SingleElementVortex"));

	else if (xrfDetector.testFlag(VESPERS::FourElement))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("FourElementVortex"));

	else if (xrfDetector.testFlag(VESPERS::Ge13Element))
		detector = qobject_cast<AMXRFDetector *>(VESPERSBeamline::vespers()->exposedDetectorByName("Ge13El"));

	if (detector){

		detector->removeAllRegionsOfInterest();

		QList<AMDataSource *> i0Sources = QList<AMDataSource *>()
				<< scan_->dataSourceAt(scan_->indexOfDataSource("SplitIonChamber"))
				   << scan_->dataSourceAt(scan_->indexOfDataSource("PreKBIonChamber"))
					  << scan_->dataSourceAt(scan_->indexOfDataSource("MiniIonChamber"));

		AMDataSource *spectraSource = 0;

		if (xrfDetector.testFlag(VESPERS::SingleElement) && xrfDetector.testFlag(VESPERS::FourElement)){

			AM2DAdditionAB *sumSpectra = new AM2DAdditionAB("SingleAndFourSpectra");
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

			AM1DNormalizationAB *normalizedRegion = new AM1DNormalizationAB(QString("norm_%1").arg(newRegion->name()));
			normalizedRegion->setInputDataSources(QList<AMDataSource *>() << newRegion << i0Sources);
			normalizedRegion->setDataName(newRegion->name());
			normalizedRegion->setNormalizationName(i0Sources.at(int(configuration_->incomingChoice()))->name());
			scan_->addAnalyzedDataSource(normalizedRegion, true, false);
		}
	}
}

AMAction3* VESPERSTimeScanActionController::createInitializationActions()
{
	AMListAction3 *initializationAction = qobject_cast<AMListAction3 *>(buildBaseInitializationAction(timedRegionsConfiguration_->time()));

	if (!configuration_->ccdDetector().testFlag(VESPERS::NoCCD))
		initializationAction->addSubAction(buildCCDInitializationAction(configuration_->ccdDetector(),
										 configuration_->ccdFileName(),
										 scan_->largestNumberInScansWhere(AMDatabase::database("user"), QString(" name = '%1'").arg(scan_->name()))+1));

	return initializationAction;
}

AMAction3* VESPERSTimeScanActionController::createCleanupActions()
{
	AMListAction3 *cleanupAction = qobject_cast<AMListAction3 *>(buildCleanupAction());

	return cleanupAction;
}

void VESPERSTimeScanActionController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}
