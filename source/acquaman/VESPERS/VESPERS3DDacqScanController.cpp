#include "VESPERS3DDacqScanController.h"

//#include "beamline/VESPERS/VESPERSBeamline.h"
//#include "dataman/AMUser.h"
//#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
//#include "dataman/datastore/AMCDFDataStore.h"

//#include "application/AMAppControllerSupport.h"
//#include "dataman/database/AMDbObjectSupport.h"
//#include "dataman/export/AMExporterOptionGeneralAscii.h"
//#include "analysis/AMOrderReductionAB.h"
//#include "analysis/AM2DNormalizationAB.h"
//#include "analysis/AM2DAdditionAB.h"
//#include "dataman/export/VESPERS/VESPERSExporter3DAscii.h"
//#include "analysis/AM3DDeadTimeCorrectionAB.h"

//#include <QDir>
//#include <QStringBuilder>

//VESPERS3DDacqScanController::VESPERS3DDacqScanController(VESPERS3DScanConfiguration *config, QObject *parent)
//	: AM3DDacqScanController(config, parent), VESPERSScanController(config)
//{
//	config_ = config;

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

//	scan_ = new AM3DScan();
//	scan_->setName(config_->name());
//	scan_->setScanConfiguration(config_);
//	scan_->setRunId(AMUser::user()->currentRunId());
//	scan_->setIndexType("fileSystem");
//	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
//	scan_->setFileFormat("amCDFv1");
//	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));

//	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERS3DDefault", config_->exportSpectraSources(), false, false, config_->exportSpectraInRows());
//	if(vespersDefault->id() > 0)
//		AMAppControllerSupport::registerClass<VESPERS3DScanConfiguration, VESPERSExporter3DAscii, AMExporterOptionGeneralAscii>(vespersDefault->id());

//	int yPoints = int((config_->yEnd() - config_->yStart())/config_->yStep());
//	if ((config_->yEnd() - config_->yStart() - (yPoints + 0.01)*config_->yStep()) < 0)
//		yPoints += 1;
//	else
//		yPoints += 2;

//	int zPoints = int((config_->yEnd() - config_->yStart())/config_->yStep());
//	if ((config_->yEnd() - config_->yStart() - (zPoints + 0.01)*config_->yStep()) < 0)
//		zPoints += 1;
//	else
//		zPoints += 2;

//	AMPVwStatusControl *control = 0;
//	control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl());
//	xAxisPVName_ = control != 0 ? control->writePVName() : "";
//	control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl());
//	yAxisPVName_ = control != 0 ? control->writePVName() : "";
//	control = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalControl());
//	zAxisPVName_ = control != 0 ? control->writePVName() : "";
//	scan_->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
//	scan_->rawData()->addScanAxis(AMAxisInfo("V", yPoints, "Vertical Position", "mm"));
//	scan_->rawData()->addScanAxis(AMAxisInfo("Wire", zPoints, "Wire Position", "mm"));

//	// Build the notes for the scan.
//	scan_->setNotes(buildNotes());

//	scan_->rawData()->addMeasurement(AMMeasurementInfo("H:fbk", "Horizontal Feedback", "mm"));
//	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//	scan_->rawData()->addMeasurement(AMMeasurementInfo("V:fbk", "Vertical Feedback", "mm"));
//	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//	scan_->rawData()->addMeasurement(AMMeasurementInfo("Wire:fbk", "Wire Feedback", "mm"));
//	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

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
//	QList<AMDataSource *> i0ReducedList;
//	AMOrderReductionAB *temp = 0;

//	for (int i = 0, size = i0List.size(); i < size; i++){

//		AMDataSource *tempInput = i0List.at(i);
//		temp = new AMOrderReductionAB(QString("%1-r").arg(tempInput->name()));
//		temp->setDescription("Reduced " % tempInput->description());
//		temp->setSelectedName(tempInput->name());
//		temp->setReducedAxis(2);
//		temp->setInputDataSources(QList<AMDataSource *>() << tempInput);
//		i0ReducedList << temp;
//		scan_->addAnalyzedDataSource(temp, true, false);
//	}

//	QString i0Name("");

//	switch (config_->incomingChoice()){

//	case VESPERS::Isplit:
//		i0Name = i0ReducedList.at(0)->name();
//		break;

//	case VESPERS::Iprekb:
//		i0Name = i0ReducedList.at(1)->name();
//		break;

//	case VESPERS::Imini:
//		i0Name = i0ReducedList.at(2)->name();
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
//		AMOrderReductionAB *reducedROI = 0;
//		AM2DNormalizationAB *normROI = 0;
//		AMDataSource *fastPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks"));
//		AMDataSource *slowPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks"));
//		AMAnalysisBlock *correctedROI = 0;
//		int roiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();
//		QList<AMDataSource *> roiList;

//		for (int i = 0; i < roiCount; i++){

//			rawDataSource = scan_->rawDataSources()->at(i+3);
//			correctedROI = new AM3DDeadTimeCorrectionAB("corrected_" % rawDataSource->name());
//			correctedROI->setDescription("Corrected " % rawDataSource->description());
//			correctedROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << fastPeakSource << slowPeakSource);
//			scan_->addAnalyzedDataSource(correctedROI, false, true);
//			reducedROI = new AMOrderReductionAB("reduced_"+rawDataSource->name());
//			reducedROI->setDescription("Reduced "+rawDataSource->description());
//			reducedROI->setSelectedName(correctedROI->name());
//			reducedROI->setReducedAxis(2);
//			reducedROI->setInputDataSources(QList<AMDataSource *>() << correctedROI);
//			roiList << reducedROI;
//			scan_->addAnalyzedDataSource(reducedROI, false, true);
//		}

//		for (int i = 0; i < roiCount; i++){

//			rawDataSource = roiList.at(i);
//			normROI = new AM2DNormalizationAB(QString(rawDataSource->name()).replace("reduced_", "norm_"));
//			normROI->setDescription(QString(rawDataSource->description().replace("Reduced ", "Normalized ")));
//			normROI->setDataName(rawDataSource->name());
//			normROI->setNormalizationName(i0Name);
//			normROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << i0ReducedList);
//			scan_->addAnalyzedDataSource(normROI, true, false);
//		}

//		break;
//	}
//	case VESPERS::FourElement:{

//		AMDataSource *rawDataSource = 0;
//		AMOrderReductionAB *reducedROI = 0;
//		AM2DNormalizationAB *normROI = 0;
//		int roiCount = VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count();
//		QList<AMDataSource *> roiList;

//		for (int i = 0; i < roiCount; i++){

//			rawDataSource = scan_->rawDataSources()->at(i+3);
//			reducedROI = new AMOrderReductionAB("reduced_"+rawDataSource->name());
//			reducedROI->setDescription("Reduced "+rawDataSource->description());
//			reducedROI->setSelectedName(rawDataSource->name());
//			reducedROI->setReducedAxis(2);
//			reducedROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource);
//			scan_->addAnalyzedDataSource(reducedROI, true, false);
//		}

//		for (int i = 0; i < roiCount; i++){

//			rawDataSource = roiList.at(i);
//			normROI = new AM2DNormalizationAB(QString(rawDataSource->name()).replace("reduced_", "norm_"));
//			normROI->setDescription(QString(rawDataSource->description().replace("Reduced ", "Normalized ")));
//			normROI->setDataName(rawDataSource->name());
//			normROI->setNormalizationName(i0Name);
//			normROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << i0ReducedList);
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
//		AMOrderReductionAB *reducedRoi1 = 0;
//		AMOrderReductionAB *reducedRoi4 = 0;
//		AM2DAdditionAB *sumAB = 0;

//		QList<QPair<int, int> > sameRois = VESPERS::findRoiPairs(VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
//		QStringList roiNames;
//		int singleElRoiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();
//		QList<AMDataSource *> roiList;

//		for (int i = 0, count = sameRois.size(); i < count; i++){

//			roi1 = scan_->rawDataSources()->at(sameRois.at(i).first+3);
//			correctedROI = new AM3DDeadTimeCorrectionAB("corrected_" % roi1->name());
//			correctedROI->setDescription("Corrected " % roi1->description());
//			correctedROI->setInputDataSources(QList<AMDataSource *>() << roi1 << fastPeakSource << slowPeakSource);
//			scan_->addAnalyzedDataSource(correctedROI, false, true);
//			reducedRoi1 = new AMOrderReductionAB("reduced_"+roi1->name());
//			reducedRoi1->setDescription("Reduced "+roi1->description());
//			reducedRoi1->setSelectedName(correctedROI->name());
//			reducedRoi1->setReducedAxis(2);
//			reducedRoi1->setInputDataSources(QList<AMDataSource *>() << correctedROI);
//			scan_->addAnalyzedDataSource(reducedRoi1, false, true);

//			roi4 = scan_->rawDataSources()->at(sameRois.at(i).second+3+singleElRoiCount);
//			reducedRoi4 = new AMOrderReductionAB("reduced_"+roi4->name());
//			reducedRoi4->setDescription("Reduced "+roi4->description());
//			reducedRoi4->setSelectedName(roi4->name());
//			reducedRoi4->setReducedAxis(2);
//			reducedRoi4->setInputDataSources(QList<AMDataSource *>() << roi4);
//			scan_->addAnalyzedDataSource(reducedRoi4, false, true);

//			QString name = roi1->name().left(roi1->name().size()-4);
//			roiNames << name;
//			sumAB = new AM2DAdditionAB("sum_" % name);
//			sumAB->setDescription("Summed " % roi1->description());
//			sumAB->setInputDataSources(QList<AMDataSource *>() << roi1 << roi4);
//			scan_->addAnalyzedDataSource(sumAB, false, true);

//			roiList << sumAB;
//		}

//		AM2DNormalizationAB *normROI = 0;
//		AMDataSource *source = 0;

//		for (int i = 0, count = roiList.size(); i < count; i++){

//			source = roiList.at(i);
//			normROI = new AM2DNormalizationAB("norm_" % roiNames.at(i));
//			normROI->setDescription("Normalized " % source->description());
//			normROI->setDataName(source->name());
//			normROI->setNormalizationName(i0Name);
//			normROI->setInputDataSources(QList<AMDataSource *>() << source << i0List);
//			scan_->addAnalyzedDataSource(normROI, true, false);
//		}

//		break;
//	}
//	}
//}

//void VESPERS3DDacqScanController::addExtraDatasources()
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
////			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
////		}
////	}

//	// Adding the CCD for XRD simultaneously.
//	scan_->rawData()->addMeasurement(AMMeasurementInfo("CCDFileNumber", "CCD file number"));
//	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);

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

//	case VESPERS::SingleElement | VESPERS::FourElement:{

//		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF1E()->toXRFInfo()));
//		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(VESPERSBeamline::vespers()->vortexXRF4E()->toXRFInfo()));
//		break;
//	}
//	}
//}

//bool VESPERS3DDacqScanController::initializeImplementation()
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

//bool VESPERS3DDacqScanController::startImplementation()
//{
//	bool configSuccess = false;

//	if (config_->fluorescenceDetector() == VESPERS::SingleElement)
//		configSuccess = setupSingleElementMap();

//	else if (config_->fluorescenceDetector() == VESPERS::FourElement)
//		configSuccess = setupFourElementMap();

//	else if (config_->fluorescenceDetector() == (VESPERS::SingleElement | VESPERS::FourElement))
//		configSuccess = setupSingleAndFourElementMap();

//	if (!configSuccess){

//		AMErrorMon::alert(this, VESPERS3DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE, "Could not recognize the configuration file.");
//		return false;
//	}

//	advAcq_->saveConfigFile("/home/hunterd/beamline/programming/acquaman/devConfigurationFiles/VESPERS/writeTest.cfg");
//	return AM3DDacqScanController::startImplementation();
//}

//void VESPERS3DDacqScanController::cleanup()
//{
//	buildCleanupAction(false);
////	connect(cleanupAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
////	connect(cleanupAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
////	cleanupAction_->start();
//}

//void VESPERS3DDacqScanController::onCleanupFinished()
//{
//	AM3DDacqScanController::onDacqStop();
//}

//void VESPERS3DDacqScanController::onScanTimerUpdate()
//{
//	if (elapsedTime_.isActive()){

//		if (secondsElapsed_ >= secondsTotal_)
//			secondsElapsed_ = secondsTotal_;
//		else
//			secondsElapsed_ += 1.0;

//		emit progress(secondsElapsed_, secondsTotal_);
//	}
//}

//void VESPERS3DDacqScanController::onInitializationActionsSucceeded()
//{
//	setInitialized();
//}

//void VESPERS3DDacqScanController::onInitializationActionsFailed(int explanation)
//{
//	Q_UNUSED(explanation)

//	AMErrorMon::alert(this, VESPERS3DDACQSCANCONTROLLER_CANT_INTIALIZE, "3D scan failed to initialize.");
////	onInitializationActionFinished();
//	setFailed();
//}

//void VESPERS3DDacqScanController::onInitializationActionsProgress(double elapsed, double total)
//{
//	Q_UNUSED(elapsed)
//	Q_UNUSED(total)
//}

//bool VESPERS3DDacqScanController::setupSingleElementMap()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(3);
//	builder.setSingleElement(true);
//	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
//	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
//	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
//	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
//	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
//	builder.setPvNameAxis3(zAxisPVName_);	// Ditto.
//	builder.buildConfigurationFile();

//	bool loadSuccess = 	setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERS3DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS 3D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(3)){}

//	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(zAxisPVName()+":fbk", true, false, 0);

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

//	else{

//		AMErrorMon::alert(this,
//				VESPERS3DDACQSCANCONTROLLER_NO_CCD_SELECTED,
//				"Error, VESPERS 3D DACQ Scan Controller failed to start (there was no CCD detector selected). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	addSingleElementSpectraPVs(advAcq_);

//	return true;
//}

//bool VESPERS3DDacqScanController::setupFourElementMap()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(3);
//	builder.setFourElement(true);
//	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
//	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
//	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
//	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
//	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
//	builder.setPvNameAxis3(zAxisPVName_);	// Ditto.
//	builder.buildConfigurationFile();

//	bool loadSuccess = setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERS3DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS 3D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(3)){}

//	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(zAxisPVName()+":fbk", true, false, 0);

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

//	else{

//		AMErrorMon::alert(this,
//				VESPERS3DDACQSCANCONTROLLER_NO_CCD_SELECTED,
//				"Error, VESPERS 3D DACQ Scan Controller failed to start (there was no CCD detector selected). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	addFourElementSpectraPVs(advAcq_);

//	return true;
//}

//bool VESPERS3DDacqScanController::setupSingleAndFourElementMap()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(3);
//	builder.setSingleElement(true);
//	builder.setFourElement(true);
//	builder.setRoperCCD(config_->ccdDetector() == VESPERS::Roper);
//	builder.setMarCCD(config_->ccdDetector() == VESPERS::Mar);
//	builder.setPilatusCCD(config_->ccdDetector() == VESPERS::Pilatus);
//	builder.setPvNameAxis1(xAxisPVName_);	// This is fine because we have already checked what sample stage we're using in the constructor.
//	builder.setPvNameAxis2(yAxisPVName_);	// Ditto.
//	builder.setPvNameAxis3(zAxisPVName_);	// Ditto.
//	builder.buildConfigurationFile();

//	bool loadSuccess = setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERS3DDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS 3D DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(3)){}

//	advAcq_->appendRecord(xAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(yAxisPVName()+":fbk", true, false, 0);
//	advAcq_->appendRecord(zAxisPVName()+":fbk", true, false, 0);

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

//	else{

//		AMErrorMon::alert(this,
//				VESPERS3DDACQSCANCONTROLLER_NO_CCD_SELECTED,
//				"Error, VESPERS 3D DACQ Scan Controller failed to start (there was no CCD detector selected). Please report this bug to the Acquaman developers.");
//		return false;
//	}
//	addSingleElementSpectraPVs(advAcq_);
//	addFourElementSpectraPVs(advAcq_);

//	return true;
//}
