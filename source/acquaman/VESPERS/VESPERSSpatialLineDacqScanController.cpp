/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSSpatialLineDacqScanController.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "actions/AMBeamlineActionsList.h"
#include "dataman/AMUser.h"
#include "dataman/AMLineScan.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "analysis/AM1DNormalizationAB.h"
#include "analysis/AM2DAdditionAB.h"
#include "analysis/AM1DSummingAB.h"
#include "analysis/AM2DSummingAB.h"
#include "analysis/AM2DDeadTimeAB.h"
#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
#include "dataman/datastore/AMCDFDataStore.h"
#include "analysis/AM1DDeadTimeAB.h"

#include "dataman/export/VESPERS/VESPERSExporterLineScanAscii.h"
#include "application/AMAppControllerSupport.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"

#include <QDir>
#include <QStringBuilder>

VESPERSSpatialLineDacqScanController::VESPERSSpatialLineDacqScanController(VESPERSSpatialLineScanConfiguration *cfg, QObject *parent)
	: AMDacqScanController(cfg, parent), VESPERSScanController(cfg)
{
	config_ = cfg;

	scan_ = new AMLineScan(); 	// MB: Moved from line 363 in startImplementation.
	scan_->setName(config_->name());
	scan_->setScanConfiguration(config_);
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setIndexType("fileSystem");
	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
	scan_->setFileFormat("amCDFv1");
	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));


	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSLineScanDefault", config_->exportSpectraSources(), false, false, config_->exportSpectraInRows());
	if(vespersDefault->id() > 0)
		AMAppControllerSupport::registerClass<VESPERSSpatialLineScanConfiguration, VESPERSExporterLineScanAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

	AMPVwStatusControl *control = 0;

	switch(config_->motor()){

	case VESPERS::H:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		break;

	case VESPERS::X:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageX());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		break;

	case VESPERS::V:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));
		break;

	case VESPERS::Z:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageZ());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", 0, "Vertical Position", "mm"));
		break;

	case VESPERS::AttoH:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageHorizontal());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		break;

	case VESPERS::AttoV:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageVertical());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));
		break;

	case VESPERS::AttoX:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageX());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		break;

	case VESPERS::AttoZ:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageZ());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Z", 0, "Vertical Position", "mm"));
		break;

	case VESPERS::AttoRz:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageRz());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Rz", 0, "Rotational Position", "deg"));
		break;

	case VESPERS::AttoRy:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageRy());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Ry", 0, "Rotational Position", "deg"));
		break;

	case VESPERS::AttoRx:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageRx());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Rx", 0, "Rotational Position", "deg"));
		break;

	case VESPERS::BigBeamX:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->bigBeamX());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Big Beam X", 0, "Horizontal Position", "mm"));
		break;

	case VESPERS::BigBeamZ:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->bigBeamZ());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("Big Beam Z", 0, "Vertical Position", "mm"));
		break;

	case VESPERS::WireH:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->wireStageHorizontal());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		break;

	case VESPERS::WireV:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->wireStageVertical());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));
		break;

	case VESPERS::WireN:
		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->wireStageNormal());
		pvName_ = control != 0 ? control->writePVName() : "";
		scan_->rawData()->addScanAxis(AMAxisInfo("N", 0, "Normal Position", "mm"));
		break;

	default:
		pvName_ = "";
		break;
	}

	secondsElapsed_ = 0;
	secondsTotal_ = config_->totalTime(true);
	elapsedTime_.setInterval(1000);
	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

	// Build the notes for the scan.
	scan_->setNotes(buildNotes());

	// Add all the data sources.
	////////////////////////////////////////////////

	// Add the feedback coordinates.
	switch(int(config_->motor())){

	case VESPERS::H:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
		break;

	case VESPERS::X:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("X:fbk", "Horizontal Feedback", "mm"));
		break;

	case VESPERS::V:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
		break;

	case VESPERS::Z:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Z:fbk", "Vertical Feedback", "mm"));
		break;

	case VESPERS::AttoH:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
		break;

	case VESPERS::AttoV:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
		break;

	case VESPERS::AttoX:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("X:fbk", "Horizontal Feedback", "mm"));
		break;

	case VESPERS::AttoZ:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Z:fbk", "Vertical Feedback", "mm"));
		break;

	case VESPERS::AttoRz:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Rz:fbk", "Rotation Feedback", "mm"));
		break;

	case VESPERS::AttoRy:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Ry:fbk", "Rotation Feedback", "mm"));
		break;

	case VESPERS::AttoRx:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Rx:fbk", "Rotation Feedback", "mm"));
		break;

	case VESPERS::BigBeamX:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("X:fbk", "Horizontal Feedback", "mm"));
		break;

	case VESPERS::BigBeamZ:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("Z:fbk", "Vertical Feedback", "mm"));
		break;

	case VESPERS::WireH:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
		break;

	case VESPERS::WireV:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
		break;

	case VESPERS::WireN:
		scan_->rawData()->addMeasurement(AMMeasurementInfo("N:fbk", "Normal Feedback", "mm"));
		break;
	}

	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

	switch ((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:
	case VESPERS::FourElement:{

		if (config_->fluorescenceDetector() == VESPERS::SingleElement)
			addSingleElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), false);
		else if (config_->fluorescenceDetector() == VESPERS::FourElement)
			addFourElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList(), false);

		break;
	}

	case VESPERS::SingleElement | VESPERS::FourElement:{

		addSingleElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), true);
		addFourElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList(), true);
		break;
	}
	}

	// Add the rest (includes the ion chambers).  This sets I0 as well; it is the only visible raw data source.
	addExtraDatasources();

	// Add analysis blocks.
	QList<AMDataSource *> i0List(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("Isplit"))
															<< scan_->dataSourceAt(scan_->indexOfDataSource("Iprekb"))
															<< scan_->dataSourceAt(scan_->indexOfDataSource("Imini")));
	QString i0Name("");

	switch (config_->incomingChoice()){

	case VESPERS::Isplit:
		i0Name = i0List.at(0)->name();
		break;

	case VESPERS::Iprekb:
		i0Name = i0List.at(1)->name();
		break;

	case VESPERS::Imini:
		i0Name = i0List.at(2)->name();
		break;

	case VESPERS::Ipost:
		i0Name = "";
		break;
	}

	switch ((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:{

		AMDataSource *rawDataSource = 0;
		AMDataSource *fastPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks"));
		AMDataSource *slowPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks"));
		AMAnalysisBlock *correctedROI = 0;
		AM1DNormalizationAB *normROI = 0;
		int roiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

		for (int i = 0; i < roiCount; i++){

			rawDataSource = scan_->rawDataSources()->at(i+1);
			correctedROI = new AM1DDeadTimeAB("corrected_" % rawDataSource->name());
			correctedROI->setDescription("Corrected " % rawDataSource->description());
			correctedROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << fastPeakSource << slowPeakSource);
			scan_->addAnalyzedDataSource(correctedROI, false, true);
			normROI = new AM1DNormalizationAB("norm_"+rawDataSource->name());
			normROI->setDescription("Normalized "+rawDataSource->description());
			normROI->setDataName(correctedROI->name());
			normROI->setNormalizationName(i0Name);
			normROI->setInputDataSources(QList<AMDataSource *>() << correctedROI << i0List);
			scan_->addAnalyzedDataSource(normROI, true, false);
		}

		AM2DDeadTimeAB *correctedSpectra1El = new AM2DDeadTimeAB("correctedRawSpectra-1el");
		correctedSpectra1El->setDescription("Corrected Spectra 1-El");
		correctedSpectra1El->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("rawSpectra-1el")) << fastPeakSource << slowPeakSource);
		scan_->addAnalyzedDataSource(correctedSpectra1El, true, false);

		break;
	}

	case VESPERS::FourElement:{

		AMDataSource *rawDataSource = 0;
		AM1DNormalizationAB *normROI = 0;
		int roiCount = VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count();

		for (int i = 0; i < roiCount; i++){

			rawDataSource = scan_->rawDataSources()->at(i+1);
			normROI = new AM1DNormalizationAB("norm_"+rawDataSource->name());
			normROI->setDescription("Normalized "+rawDataSource->description());
			normROI->setDataName(rawDataSource->name());
			normROI->setNormalizationName(i0Name);
			normROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << i0List);
			scan_->addAnalyzedDataSource(normROI, true, false);
		}

		break;
	}

	case VESPERS::SingleElement | VESPERS::FourElement:{

		AM2DDeadTimeAB *correctedSpectra1El = new AM2DDeadTimeAB("correctedRawSpectra-1el");
		correctedSpectra1El->setDescription("Corrected Spectra 1-El");
		correctedSpectra1El->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("rawSpectra-1el")) << scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks")) << scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks")));
		scan_->addAnalyzedDataSource(correctedSpectra1El, false, true);

		AM2DAdditionAB *spectraSumAB = new AM2DAdditionAB("sumSpectra");
		spectraSumAB->setDescription("Summed spectra of both detectors.");
		spectraSumAB->setInputDataSources(QList<AMDataSource *>() << correctedSpectra1El << scan_->dataSourceAt(scan_->indexOfDataSource("correctedSum-4el")));
		scan_->addAnalyzedDataSource(spectraSumAB, true, false);

		AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
		pfy->setDescription("PFY");
		pfy->setInputDataSources(QList<AMDataSource *>() << spectraSumAB);
		pfy->setSumAxis(1);
		pfy->setSumRangeMax(spectraSumAB->size(1)-1);
		scan_->addAnalyzedDataSource(pfy, true, false);

		AM1DNormalizationAB *normPFY;
		normPFY = new AM1DNormalizationAB("norm_PFY");
		normPFY->setDescription("Normalized PFY");
		normPFY->setDataName(pfy->name());
		normPFY->setNormalizationName(i0Name);
		normPFY->setInputDataSources(QList<AMDataSource *>() << pfy << i0List);
		scan_->addAnalyzedDataSource(normPFY, true, false);

		AMDataSource *roi1 = 0;
		AMDataSource *roi4 = 0;
		AMDataSource *fastPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks"));
		AMDataSource *slowPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks"));
		AMAnalysisBlock *correctedROI = 0;
		AM1DSummingAB *sumAB = 0;
		QList<QPair<int, int> > sameRois = VESPERS::findRoiPairs(VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		QStringList roiNames;
		int singleElRoiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

		for (int i = 0, count = sameRois.size(); i < count; i++){

			roi1 = scan_->rawDataSources()->at(sameRois.at(i).first+1);
			correctedROI = new AM1DDeadTimeAB("corrected_" % roi1->name());
			correctedROI->setDescription("Corrected " % roi1->description());
			correctedROI->setInputDataSources(QList<AMDataSource *>() << roi1 << fastPeakSource << slowPeakSource);
			scan_->addAnalyzedDataSource(correctedROI, false, true);
			roi4 = scan_->rawDataSources()->at(sameRois.at(i).second+1+singleElRoiCount);
			QString name = roi1->name().left(roi1->name().size()-4);
			roiNames << name;
			sumAB = new AM1DSummingAB("sum_" % name);
			sumAB->setDescription("Summed " % roi1->description());
			sumAB->setInputDataSources(QList<AMDataSource *>() << correctedROI << roi4);
			scan_->addAnalyzedDataSource(sumAB, false, true);
		}

		for (int i = 0, count = scan_->analyzedDataSourceCount(); i < count; i++){

			if (scan_->analyzedDataSources()->at(i)->name().contains("sum_")){

				normPFY = new AM1DNormalizationAB("norm_"+scan_->analyzedDataSources()->at(i)->name());
				normPFY->setDescription("Normalized "+scan_->analyzedDataSources()->at(i)->description());
				normPFY->setDataName(scan_->analyzedDataSources()->at(i)->name());
				normPFY->setNormalizationName(i0Name);
				normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->analyzedDataSources()->at(i) << i0List);
				scan_->addAnalyzedDataSource(normPFY, true, false);
			}
		}

		break;
	}
	}
}

void VESPERSSpatialLineDacqScanController::addExtraDatasources()
{
	addStandardMeasurements(scan_, false, false);

	switch((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:{

		addSingleElementDeadTimeMeasurements(scan_);
		break;
	}

	case VESPERS::FourElement:{

		addFourElementDeadTimeMeasurements(scan_);
		break;
	}

	case VESPERS::SingleElement | VESPERS::FourElement:{

		addSingleElementDeadTimeMeasurements(scan_);
		addFourElementDeadTimeMeasurements(scan_);
		break;
	}
	}

	// Adding in the extra ion chambers but not Ipost.
	AMMeasurementInfo temp("", "");
	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++){

		temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
		temp.name = ionChambers->detectorAt(i)->detectorName();
		scan_->rawData()->addMeasurement(temp);

		if (i == int(config_->incomingChoice()) || i == int(config_->transmissionChoice()))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);
		else
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, false);
	}

	// If using the CCD for XRD simultaneously.
	if (config_->ccdDetector() != VESPERS::NoCCD){

		scan_->rawData()->addMeasurement(AMMeasurementInfo("CCDFileNumber", "CCD file number"));
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
	}

	// Add the spectra.
	switch ((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		break;

	case VESPERS::SingleElement:{

		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo()));
		break;
	}

	case VESPERS::FourElement:{

		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo()));
		break;
	}

	case VESPERS::SingleElement | VESPERS::FourElement:{

		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo()));
		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo()));
		break;
	}
	}
}

bool VESPERSSpatialLineDacqScanController::initializeImplementation()
{
	buildBaseInitializationAction(config_->time());

	QString ccdName = buildCCDInitializationAction(config_->ccdDetector(), config_->ccdFileName());

	if (config_->ccdFileName() != ccdName)
		config_->setCCDFileName(ccdName);

	AMBeamlineParallelActionsList *setupActionsList = initializationAction_->list();

	if (config_->hasOtherPosition()){

		setupActionsList->appendStage(new QList<AMBeamlineActionItem*>());

		switch(config_->otherMotor(config_->motor())){

		case VESPERS::H:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createHorizontalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::V:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createVerticalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::X:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createHorizontalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::Z:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createVerticalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::AttoH:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->createHorizontalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::AttoV:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->createVerticalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::AttoX:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->createHorizontalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::AttoZ:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realAttocubeStageMotorGroupObject()->createVerticalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::BigBeamX:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->createHorizontalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::BigBeamZ:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->createVerticalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::WireH:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->createHorizontalMoveAction(config_->otherPosition()));
			break;

		case VESPERS::WireV:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->bigBeamMotorGroupObject()->createVerticalMoveAction(config_->otherPosition()));
			break;

		default:
			break;
		}
	}

	if (config_->normalPosition() != 888888.88){

		setupActionsList->appendStage(new QList<AMBeamlineActionItem*>());

		switch(config_->motor()){

		case VESPERS::H:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::V:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::X:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::Z:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::AttoH:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::AttoV:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::AttoX:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::AttoZ:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::WireH:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		case VESPERS::WireV:
			setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
			break;

		default:	// No normal position for big beam.
			break;
		}
	}

	connect(initializationAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
	connect(initializationAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
	connect(initializationAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
	initializationAction_->start();

	return true;
}

bool VESPERSSpatialLineDacqScanController::startImplementation()
{
	bool configSuccess = false;

	if (config_->fluorescenceDetector() == VESPERS::NoXRF)
		configSuccess = setupIonChamberMap();

	else if (config_->fluorescenceDetector() == VESPERS::SingleElement)
		configSuccess = setupSingleElementMap();

	else if (config_->fluorescenceDetector() == VESPERS::FourElement)
		configSuccess = setupFourElementMap();

	else if (config_->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement))
		configSuccess = setupSingleAndFourElementMap();

	if (!configSuccess){

		AMErrorMon::alert(this, VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE, "Could not recognize the configuration file.");
		return false;
	}

	advAcq_->setStart(0, config_->start());
	advAcq_->setDelta(0, config_->step());
	advAcq_->setEnd(0, config_->end());

	advAcq_->saveConfigFile("/home/hunterd/Desktop/writeTest.cfg");

	return AMDacqScanController::startImplementation();
}

void VESPERSSpatialLineDacqScanController::cleanup()
{
	buildCleanupAction(false);
	connect(cleanupAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
	connect(cleanupAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
	cleanupAction_->start();
}

void VESPERSSpatialLineDacqScanController::onCleanupFinished()
{
	AMDacqScanController::onDacqStop();
}

void VESPERSSpatialLineDacqScanController::onScanTimerUpdate()
{
	if (elapsedTime_.isActive()){

		if (secondsElapsed_ >= secondsTotal_)
			secondsElapsed_ = secondsTotal_;
		else
			secondsElapsed_ += 1.0;

		emit progress(secondsElapsed_, secondsTotal_);
	}
}

void VESPERSSpatialLineDacqScanController::onInitializationActionsSucceeded()
{
	setInitialized();
}

void VESPERSSpatialLineDacqScanController::onInitializationActionsFailed(int explanation)
{
	Q_UNUSED(explanation)

	AMErrorMon::alert(this, VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_INTIALIZE, "Line scan failed to initialize.");
	onInitializationActionFinished();
	setFailed();
}

void VESPERSSpatialLineDacqScanController::onInitializationActionsProgress(double elapsed, double total)
{
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
}

bool VESPERSSpatialLineDacqScanController::setupIonChamberMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(1);
	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
	builder.setPvNameAxis1(pvName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.buildConfigurationFile();

	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS Spatial Line DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = false;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMPVwStatusControl *motor = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());
	if (!motor)
		return false;

	advAcq_->appendRecord(motor->writePVName()+":fbk", true, false, 0);
	addStandardExtraPVs(advAcq_, false, false);

	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	if (config_->ccdDetector() == VESPERS::Roper)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

	else if (config_->ccdDetector() == VESPERS::Mar)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

	return true;
}

bool VESPERSSpatialLineDacqScanController::setupSingleElementMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(1);
	builder.setSingleElement(true);
	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
	builder.setPvNameAxis1(pvName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.buildConfigurationFile();

	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS Spatial Line DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMPVwStatusControl *motor = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());
	if (!motor)
		return false;

	advAcq_->appendRecord(motor->writePVName()+":fbk", true, false, 0);

	addSingleElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count());
	addStandardExtraPVs(advAcq_, false, false);
	addSingleElementDeadTimePVs(advAcq_);

	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	if (config_->ccdDetector() == VESPERS::Roper)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

	if (config_->ccdDetector() == VESPERS::Mar)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

	if (config_->ccdDetector() == VESPERS::Pilatus)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

	addSingleElementSpectraPVs(advAcq_);

	return true;
}

bool VESPERSSpatialLineDacqScanController::setupFourElementMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(1);
	builder.setFourElement(true);
	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
	builder.setPvNameAxis1(pvName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.buildConfigurationFile();

	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS Spatial Line DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMPVwStatusControl *motor = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());
	if (!motor)
		return false;

	advAcq_->appendRecord(motor->writePVName()+":fbk", true, false, 0);

	addFourElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count());
	addStandardExtraPVs(advAcq_, false, false);
	addFourElementDeadTimePVs(advAcq_);

	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	if (config_->ccdDetector() == VESPERS::Roper)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

	else if (config_->ccdDetector() == VESPERS::Mar)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

	addFourElementSpectraPVs(advAcq_);

	return true;
}

bool VESPERSSpatialLineDacqScanController::setupSingleAndFourElementMap()
{
	VESPERSConfigurationFileBuilder builder;
	builder.setDimensions(1);
	builder.setSingleElement(true);
	builder.setFourElement(true);
	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
	builder.setPvNameAxis1(pvName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
	builder.buildConfigurationFile();

	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

	if(!loadSuccess){
		AMErrorMon::alert(this,
				VESPERSSPATIALLINEDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
		return false;
	}

	usingSpectraDotDatFile_ = true;

	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
	while (advAcq_->deleteRecord(1)){}

	AMPVwStatusControl *motor = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());
	if (!motor)
		return false;

	advAcq_->appendRecord(motor->writePVName()+":fbk", true, false, 0);

	addSingleElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count());
	addFourElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count());

	addStandardExtraPVs(advAcq_, false, false);
	addSingleElementDeadTimePVs(advAcq_);
	addFourElementDeadTimePVs(advAcq_);

	// Ion chambers.
	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

	for (int i = 0; i < ionChambers->count(); i++)
		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

	// Using the CCD?
	if (config_->ccdDetector() == VESPERS::Roper)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

	else if (config_->ccdDetector() == VESPERS::Mar)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

	else if (config_->ccdDetector() == VESPERS::Pilatus)
		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

	addSingleElementSpectraPVs(advAcq_);
	addFourElementSpectraPVs(advAcq_);

	return true;
}
