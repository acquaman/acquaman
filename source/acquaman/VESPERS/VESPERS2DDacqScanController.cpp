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


#include "VESPERS2DDacqScanController.h"

//#include "beamline/VESPERS/VESPERSBeamline.h"
//#include "dataman/AMUser.h"
//#include "analysis/AM2DNormalizationAB.h"
//#include "analysis/AM2DAdditionAB.h"
//#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
//#include "analysis/AM3DAdditionAB.h"
//#include "analysis/AM3DDeadTimeAB.h"
//#include "analysis/AM2DDeadTimeCorrectionAB.h"
//#include "dataman/datastore/AMCDFDataStore.h"

//#include "dataman/export/VESPERS/VESPERSExporter2DAscii.h"
//#include "dataman/export/VESPERS/VESPERSExporterSMAK.h"
//#include "application/AMAppControllerSupport.h"
//#include "dataman/database/AMDbObjectSupport.h"
//#include "dataman/export/AMExporterOptionGeneralAscii.h"

//#include <QDir>
//#include <QStringBuilder>

//VESPERS2DDacqScanController::VESPERS2DDacqScanController(VESPERS2DScanConfiguration *cfg, QObject *parent)
//	: AM2DDacqScanController(cfg, parent), VESPERSScanController(cfg)
//{
//	config_ = cfg;

//	secondsElapsed_ = 0;
//	secondsTotal_ = config_->totalTime(true);
//	elapsedTime_.setInterval(1000);
//	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
//	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
//	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
//	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
//	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
//	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
//	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

//	scan_ = new AM2DScan(); 	// MB: Moved from line 363 in startImplementation.
//	scan_->setName(config_->name());
//	scan_->setScanConfiguration(config_);
//	scan_->setRunId(AMUser::user()->currentRunId());
//	scan_->setIndexType("fileSystem");
//	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
//	scan_->setFileFormat("amCDFv1");
//	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));


//	if (config_->exportAsAscii()){

//		AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERS2DDefault", config_->exportSpectraSources(), false, false, config_->exportSpectraInRows());
//		if(vespersDefault->id() > 0)
//			AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporter2DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());
//	}

//	else{

//		// SMAK format requires a specific spectra file format.
//		AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERS2DDefault", config_->exportSpectraSources(), false, false, true);
//		if(vespersDefault->id() > 0)
//			AMAppControllerSupport::registerClass<VESPERS2DScanConfiguration, VESPERSExporterSMAK, AMExporterOptionGeneralAscii>(vespersDefault->id());
//	}

//	int yPoints = int((config_->yEnd() - config_->yStart())/config_->yStep());
//	if ((config_->yEnd() - config_->yStart() - (yPoints + 0.01)*config_->yStep()) < 0)
//		yPoints += 1;
//	else
//		yPoints += 2;

//	AMPVwStatusControl *control = 0;

//	switch(config_->motor()){

//	case VESPERS::H | VESPERS::V:
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl());
//		xAxisPVName_ = control != 0 ? control->writePVName() : "";
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl());
//		yAxisPVName_ = control != 0 ? control->writePVName() : "";
//		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
//		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
//		break;

//	case VESPERS::X | VESPERS::Z:
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageX());
//		xAxisPVName_ = control != 0 ? control->writePVName() : "";
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageZ());
//		yAxisPVName_ = control != 0 ? control->writePVName() : "";
//		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
//		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
//		break;

//	case VESPERS::AttoH | VESPERS::AttoV:
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->horizontalControl());
//		xAxisPVName_ = control != 0 ? control->writePVName() : "";
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoAttocubeStageMotorGroupObject()->verticalControl());
//		yAxisPVName_ = control != 0 ? control->writePVName() : "";
//		scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
//		scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
//		break;

//	case VESPERS::AttoX | VESPERS::AttoZ:
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageX());
//		xAxisPVName_ = control != 0 ? control->writePVName() : "";
//		control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->attoStageZ());
//		yAxisPVName_ = control != 0 ? control->writePVName() : "";
//		scan_->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
//		scan_->rawData()->addScanAxis(AMAxisInfo("Z", yPoints, "Vertical Position", "mm"));
//		break;

//	default:
//		xAxisPVName_ = "";
//		yAxisPVName_ = "";
//		break;
//	}

//	// Build the notes for the scan.
//	scan_->setNotes(buildNotes());

//	// Add all the data sources.
//	////////////////////////////////////////////////

//	// Add the feedback coordinates.
//	switch(int(config_->motor())){

//	case VESPERS::H | VESPERS::V:
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		break;

//	case VESPERS::X | VESPERS::Z:
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("X:fbk", "Horizontal Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("Z:fbk", "Vertical Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		break;

//	case VESPERS::AttoH | VESPERS::AttoV:
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		break;

//	case VESPERS::AttoX | VESPERS::AttoZ:
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("X:fbk", "Horizontal Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		scan_->rawData()->addMeasurement(AMMeasurementInfo("Z:fbk", "Vertical Feedback", "mm"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//		break;
//	}

//	switch ((int)config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:
//		break;

//	case VESPERS::SingleElement:
//	case VESPERS::FourElement:{

//		if (config_->fluorescenceDetector() == VESPERS::SingleElement)
//			addSingleElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), false);
//		else if (config_->fluorescenceDetector() == VESPERS::FourElement)
//			addFourElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList(), false);

//		break;
//	}

//	case VESPERS::SingleElement | VESPERS::FourElement:{

//		addSingleElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), true);
//		addFourElementRegionsOfInterestMeasurements(scan_, *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList(), true);
//		break;
//	}
//	}

//	// Add the rest (includes the ion chambers).  This sets I0 as well; it is the only visible raw data source.
//	addExtraDatasources();

//	// Add analysis blocks.
//	QList<AMDataSource *> i0List(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("Isplit"))
//															<< scan_->dataSourceAt(scan_->indexOfDataSource("Iprekb"))
//															<< scan_->dataSourceAt(scan_->indexOfDataSource("Imini")));

//	QString i0Name("");

//	switch (config_->incomingChoice()){

//	case VESPERS::Isplit:
//		i0Name = i0List.at(0)->name();
//		break;

//	case VESPERS::Iprekb:
//		i0Name = i0List.at(1)->name();
//		break;

//	case VESPERS::Imini:
//		i0Name = i0List.at(2)->name();
//		break;

//	case VESPERS::Ipost:
//		i0Name = "";
//		break;
//	}

//	switch ((int)config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:
//		break;

//	case VESPERS::SingleElement:{

//		AMDataSource *rawDataSource = 0;
//		AMDataSource *fastPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks"));
//		AMDataSource *slowPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks"));
//		AMAnalysisBlock *correctedROI = 0;
//		AM2DNormalizationAB *normROI = 0;
//		int roiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

//		for (int i = 0; i < roiCount; i++){

//			rawDataSource = scan_->rawDataSources()->at(i+2);
//			correctedROI = new AM2DDeadTimeCorrectionAB("corrected_" % rawDataSource->name());
//			correctedROI->setDescription("Corrected " % rawDataSource->description());
//			correctedROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << fastPeakSource << slowPeakSource);
//			scan_->addAnalyzedDataSource(correctedROI, false, true);
//			normROI = new AM2DNormalizationAB("norm_"+rawDataSource->name());
//			normROI->setDescription("Normalized "+rawDataSource->description());
//			normROI->setDataName(correctedROI->name());
//			normROI->setNormalizationName(i0Name);
//			normROI->setInputDataSources(QList<AMDataSource *>() << correctedROI << i0List);
//			scan_->addAnalyzedDataSource(normROI, true, false);
//		}

//		AM3DDeadTimeAB *correctedSpectra1El = new AM3DDeadTimeAB("correctedRawSpectra-1el");
//		correctedSpectra1El->setDescription("Corrected Spectra 1-El");
//		correctedSpectra1El->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("rawSpectra-1el")) << scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks")) << scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks")));
//		scan_->addAnalyzedDataSource(correctedSpectra1El, false, true);

//		break;
//	}

//	case VESPERS::FourElement:{

//		AMDataSource *rawDataSource = 0;
//		AM2DNormalizationAB *normROI = 0;
//		int roiCount = VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count();

//		for (int i = 0; i < roiCount; i++){

//			rawDataSource = scan_->rawDataSources()->at(i+2);
//			normROI = new AM2DNormalizationAB("norm_"+rawDataSource->name());
//			normROI->setDescription("Normalized "+rawDataSource->description());
//			normROI->setDataName(rawDataSource->name());
//			normROI->setNormalizationName(i0Name);
//			normROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << i0List);
//			scan_->addAnalyzedDataSource(normROI, true, false);
//		}

//		break;
//	}

//	case VESPERS::SingleElement | VESPERS::FourElement:{

//		AMDataSource *roi1 = 0;
//		AMDataSource *roi4 = 0;
//		AMDataSource *fastPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks"));
//		AMDataSource *slowPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks"));
//		AMAnalysisBlock *correctedROI = 0;
//		AM2DAdditionAB *sumAB = 0;

//		QList<QPair<int, int> > sameRois = VESPERS::findRoiPairs(VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
//		QStringList roiNames;
//		int singleElRoiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

//		for (int i = 0, count = sameRois.size(); i < count; i++){

//			roi1 = scan_->rawDataSources()->at(sameRois.at(i).first+2);
//			correctedROI = new AM2DDeadTimeCorrectionAB("corrected_" % roi1->name());
//			correctedROI->setDescription("Corrected " % roi1->description());
//			correctedROI->setInputDataSources(QList<AMDataSource *>() << roi1 << fastPeakSource << slowPeakSource);
//			scan_->addAnalyzedDataSource(correctedROI, false, true);
//			roi4 = scan_->rawDataSources()->at(sameRois.at(i).second+2+singleElRoiCount);
//			QString name = roi1->name().left(roi1->name().size()-4);
//			roiNames << name;
//			sumAB = new AM2DAdditionAB("sum_" % name);
//			sumAB->setDescription("Summed " % roi1->description());
//			sumAB->setInputDataSources(QList<AMDataSource *>() << correctedROI << roi4);
//			scan_->addAnalyzedDataSource(sumAB, false, true);
//		}

//		AM2DNormalizationAB *normROI = 0;
//		AMDataSource *source = 0;

//		for (int i = 0, count = scan_->analyzedDataSourceCount(); i < count; i++){

//			source = scan_->analyzedDataSources()->at(i);
//			normROI = new AM2DNormalizationAB("norm_" % roiNames.at(i));
//			normROI->setDescription("Normalized " % source->description());
//			normROI->setDataName(source->name());
//			normROI->setNormalizationName(i0Name);
//			normROI->setInputDataSources(QList<AMDataSource *>() << source << i0List);
//			scan_->addAnalyzedDataSource(normROI, true, false);
//		}

//		AM3DDeadTimeAB *correctedSpectra1El = new AM3DDeadTimeAB("correctedRawSpectra-1el");
//		correctedSpectra1El->setDescription("Corrected Spectra 1-El");
//		correctedSpectra1El->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("rawSpectra-1el")) << scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks")) << scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks")));
//		scan_->addAnalyzedDataSource(correctedSpectra1El, false, true);

//		AM3DAdditionAB *spectraSum = new AM3DAdditionAB("sumSpectra-1eland4el");
//		spectraSum->setDescription("Sum of Single and Four Element detectors");
//		spectraSum->setInputDataSources(QList<AMDataSource *>() << correctedSpectra1El << scan_->dataSourceAt(scan_->indexOfDataSource("correctedSum-4el")));
//		scan_->addAnalyzedDataSource(spectraSum, false, true);

//		break;
//	}
//	}
//}

//void VESPERS2DDacqScanController::addExtraDatasources()
//{
//	addStandardMeasurements(scan_, false, false);

//	switch ((int)config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:
//		break;

//	case VESPERS::SingleElement:{

//		addSingleElementDeadTimeMeasurements(scan_);
//		break;
//	}

//	case VESPERS::FourElement:{

//		addFourElementDeadTimeMeasurements(scan_);
//		break;
//	}

//	case VESPERS::SingleElement | VESPERS::FourElement:{

//		addSingleElementDeadTimeMeasurements(scan_);
//		addFourElementDeadTimeMeasurements(scan_);
//		break;
//	}
//	}

//	// Adding in the extra ion chambers but not Ipost.
//	AMMeasurementInfo temp("", "");
////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++){

////		if (ionChambers->detectorAt(i)->detectorName() != "Ipost"){

////			temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
////			temp.name = ionChambers->detectorAt(i)->detectorName();
////			scan_->rawData()->addMeasurement(temp);
////			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, false);
////		}
////	}

//	// If using the CCD for XRD simultaneously.
//	if (config_->ccdDetector() != VESPERS::NoCCD){

//		scan_->rawData()->addMeasurement(AMMeasurementInfo("CCDFileNumber", "CCD file number"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//	}

//	// Add the spectra.
//	switch ((int)config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:
//		break;

//	case VESPERS::SingleElement:{

//		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo()));
//		break;
//	}

//	case VESPERS::FourElement:{

//		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo()));
//		break;
//	}

//<<<<<<< HEAD
////	case VESPERS::SingleElement | VESPERS::FourElement:{
//=======
//	AMBeamlineParallelActionsList *setupActionsList = initializationAction_->list();
//	setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());

//	switch(int(config_->motor())){

//	case VESPERS::H | VESPERS::V:

//		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
//		break;

//	case VESPERS::X | VESPERS::Z:

//		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
//		break;

//	case VESPERS::AttoH | VESPERS::AttoV:

//		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));  // focusing isn't done with attocube motors.
//		break;

//	case VESPERS::AttoX | VESPERS::AttoZ:

//		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createNormalMoveAction(config_->normalPosition()));
//		break;
//	}

//	connect(initializationAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
//	connect(initializationAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
//	connect(initializationAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
//	initializationAction_->start();
//>>>>>>> master

//		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo()));
//		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo()));
//		break;
//	}
//	}
//}

//bool VESPERS2DDacqScanController::initializeImplementation()
//{
////	buildBaseInitializationAction(config_->timeStep());

////	QString ccdName = buildCCDInitializationAction(config_->ccdDetector(), config_->ccdFileName());

////	if (config_->ccdFileName() != ccdName)
////		config_->setCCDFileName(ccdName);

////	connect(initializationAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
////	connect(initializationAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
////	connect(initializationAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
////	initializationAction_->start();

//	return true;
//}

//bool VESPERS2DDacqScanController::startImplementation()
//{
//	bool configSuccess = false;

//	if (config_->fluorescenceDetector() == VESPERS::SingleElement)
//		configSuccess = setupSingleElementMap();

//	else if (config_->fluorescenceDetector() == VESPERS::FourElement)
//		configSuccess = setupFourElementMap();

//	else if (config_->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement))
//		configSuccess = setupSingleAndFourElementMap();

//	if (!configSuccess){

//		AMErrorMon::alert(this, VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE, "Could not recognize the configuration file.");
//		return false;
//	}

//	return AM2DDacqScanController::startImplementation();
//}

//void VESPERS2DDacqScanController::cleanup()
//{
//	buildCleanupAction(false);
////	connect(cleanupAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
////	connect(cleanupAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
////	cleanupAction_->start();
//}

//void VESPERS2DDacqScanController::onCleanupFinished()
//{
//	AM2DDacqScanController::onDacqStop();
//}

//void VESPERS2DDacqScanController::onScanTimerUpdate()
//{
//	if (elapsedTime_.isActive()){

//		if (secondsElapsed_ >= secondsTotal_)
//			secondsElapsed_ = secondsTotal_;
//		else
//			secondsElapsed_ += 1.0;

//		emit progress(secondsElapsed_, secondsTotal_);
//	}
//}

//void VESPERS2DDacqScanController::onInitializationActionsSucceeded()
//{
//	setInitialized();
//}

//void VESPERS2DDacqScanController::onInitializationActionsFailed(int explanation)
//{
//	Q_UNUSED(explanation)

//	AMErrorMon::alert(this, VESPERS2DDACQSCANCONTROLLER_CANT_INTIALIZE, "2D scan failed to initialize.");
////	onInitializationActionFinished();
//	setFailed();
//}

//void VESPERS2DDacqScanController::onInitializationActionsProgress(double elapsed, double total)
//{
//	Q_UNUSED(elapsed)
//	Q_UNUSED(total)
//}

//bool VESPERS2DDacqScanController::setupSingleElementMap()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(2);
//	builder.setSingleElement(true);
//	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
//	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
//	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
//	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
//	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
//	builder.buildConfigurationFile();

//	bool loadSuccess = 	setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(2)){}

//	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);

//	addSingleElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count());
//	addStandardExtraPVs(advAcq_, false, false);
//	addSingleElementDeadTimePVs(advAcq_);

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (ionChambers->detectorAt(i)->detectorName() != "Ipost")
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	if (config_->ccdDetector() == VESPERS::Roper)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

//	else if (config_->ccdDetector() == VESPERS::Mar)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

//	else if (config_->ccdDetector() == VESPERS::Pilatus)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

//	addSingleElementSpectraPVs(advAcq_);

//	return true;
//}

//bool VESPERS2DDacqScanController::setupFourElementMap()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(2);
//	builder.setFourElement(true);
//	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
//	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
//	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
//	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
//	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
//	builder.buildConfigurationFile();

//	bool loadSuccess = setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(2)){}

//	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);

//	addFourElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count());
//	addStandardExtraPVs(advAcq_, false, false);
//	addFourElementDeadTimePVs(advAcq_);

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (ionChambers->detectorAt(i)->detectorName() != "Ipost")
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	if (config_->ccdDetector() == VESPERS::Roper)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

//	else if (config_->ccdDetector() == VESPERS::Mar)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

//	else if (config_->ccdDetector() == VESPERS::Pilatus)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

//	addFourElementSpectraPVs(advAcq_);

//	return true;
//}

//bool VESPERS2DDacqScanController::setupSingleAndFourElementMap()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(2);
//	builder.setSingleElement(true);
//	builder.setFourElement(true);
//	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
//	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
//	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
//	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
//	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
//	builder.buildConfigurationFile();

//	bool loadSuccess = setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERS2DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS 2D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(2)){}

//	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);

//	addSingleElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count());
//	addFourElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count());

//	addStandardExtraPVs(advAcq_, false, false);
//	addSingleElementDeadTimePVs(advAcq_);
//	addFourElementDeadTimePVs(advAcq_);

//	// Ion chambers.
////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (ionChambers->detectorAt(i)->detectorName() != "Ipost")
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	// Using the CCD?
//	if (config_->ccdDetector() == VESPERS::Roper)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

//	else if (config_->ccdDetector() == VESPERS::Mar)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

//	else if (config_->ccdDetector() == VESPERS::Pilatus)
//		advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

//	addSingleElementSpectraPVs(advAcq_);
//	addFourElementSpectraPVs(advAcq_);

//	return true;
//}
