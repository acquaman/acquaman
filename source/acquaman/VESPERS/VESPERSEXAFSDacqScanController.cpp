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


//#include "VESPERSEXAFSDacqScanController.h"
//#include "beamline/VESPERS/VESPERSBeamline.h"
//#include "dataman/AMUser.h"
//#include "analysis/AM1DExpressionAB.h"
//#include "analysis/AM2DSummingAB.h"
//#include "analysis/AM2DAdditionAB.h"
//#include "analysis/AM1DNormalizationAB.h"
//#include "analysis/AM1DSummingAB.h"
//#include "analysis/AM2DDeadTimeAB.h"
////#include "actions/AMBeamlineParallelActionsList.h"
//#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
//#include "dataman/datastore/AMCDFDataStore.h"
//#include "analysis/AM1DDeadTimeAB.h"

//#include "dataman/export/AMExporterAthena.h"
//#include "application/AMAppControllerSupport.h"
//#include "dataman/database/AMDbObjectSupport.h"
//#include "dataman/export/AMExporterOptionGeneralAscii.h"

//#include <QDir>
//#include <QStringBuilder>

//VESPERSEXAFSDacqScanController::VESPERSEXAFSDacqScanController(VESPERSEXAFSScanConfiguration *cfg, QObject *parent)
//	: AMDacqScanController(cfg, parent), VESPERSScanController(cfg)
//{
//	config_ = cfg;
//	config_->setUserScanName(config_->name());

//	secondsElapsed_ = 0;
//	secondsTotal_ = config_->totalTime();
//	elapsedTime_.setInterval(1000);
//	connect(this, SIGNAL(started()), &elapsedTime_, SLOT(start()));
//	connect(this, SIGNAL(cancelled()), &elapsedTime_, SLOT(stop()));
//	connect(this, SIGNAL(paused()), &elapsedTime_, SLOT(stop()));
//	connect(this, SIGNAL(resumed()), &elapsedTime_, SLOT(start()));
//	connect(this, SIGNAL(failed()), &elapsedTime_, SLOT(stop()));
//	connect(this, SIGNAL(finished()), &elapsedTime_, SLOT(stop()));
//	connect(&elapsedTime_, SIGNAL(timeout()), this, SLOT(onScanTimerUpdate()));

//	scan_ = new AMXASScan(); 	// MB: Moved from line 363 in startImplementation.
//	scan_->setName(config_->name());
//	scan_->setScanConfiguration(config_);
//	scan_->setRunId(AMUser::user()->currentRunId());
//	scan_->setIndexType("fileSystem");
//	scan_->setFilePath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime())+".cdf");
//	scan_->setFileFormat("amCDFv1");
//	scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));
//	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

//	AMExporterOptionGeneralAscii *vespersDefault = VESPERS::buildStandardExporterOption("VESPERSDefault", config_->exportSpectraSources(), true, true, config_->exportSpectraInRows());
//	if(vespersDefault->id() > 0)
//		AMAppControllerSupport::registerClass<VESPERSEXAFSScanConfiguration, AMExporterAthena, AMExporterOptionGeneralAscii>(vespersDefault->id());

//	scan_->setNotes(buildNotes());

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	// Add all the raw datasources.
////	AMMeasurementInfo temp(*(ionChambers->detectorAt((int)config_->incomingChoice())->toInfo()));
////	temp.name = "I0";
////	scan_->rawData()->addMeasurement(temp);
////	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 0), true, false);
////	temp = AMMeasurementInfo(*(ionChambers->detectorAt((int)config_->transmissionChoice())->toInfo()));
////	temp.name = "It";
////	scan_->rawData()->addMeasurement(temp);
////	scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), 1), true, false);

////	// Adding in the extra ion chambers.
////	for (int i = 0; i < ionChambers->count(); i++){

////		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice()){

////			temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
////			temp.name = ionChambers->detectorAt(i)->detectorName();
////			scan_->rawData()->addMeasurement(temp);
////			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
////		}
////	}

//	switch((int)config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:
//		// Add all the extra raw data sources.  Only needs to happen after the ion chambers for transmission.
//		addExtraDatasources();
//		break;

//	case VESPERS::SingleElement:{

//		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();
//		QString edge = config_->edge();

//		// If this is an L edge, chop out the number for comparison purposes.
//		if (edge.contains(QRegExp("L\\d$")))
//			edge.chop(1);

//		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
//		for (int i = 0; i < detector->roiInfoList()->count(); i++){

//			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));

//			if (detector->roiInfoList()->at(i).name().contains(edge))
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);

//			else
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
//		}

//		// Add all the extra raw data sources.
//		addExtraDatasources();
//		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(detector->toXRFInfo()));

//		break;
//	}

//	case VESPERS::FourElement:{

//		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF4E();
//		QString edge = config_->edge();

//		// If this is an L edge, chop out the number for comparison purposes.
//		if (edge.contains(QRegExp("L\\d$")))
//			edge.chop(1);

//		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
//		for (int i = 0; i < detector->roiInfoList()->count(); i++){

//			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" "), detector->roiInfoList()->at(i).name()));

//			if (detector->roiInfoList()->at(i).name().contains(edge))
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);

//			else
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
//		}

//		// Add all the extra raw data sources.
//		addExtraDatasources();
//		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(detector->toXRFInfo()));

//		break;
//	}

//	case VESPERS::SingleElement | VESPERS::FourElement:{

//		XRFDetector *detector = VESPERSBeamline::vespers()->vortexXRF1E();
//		QString edge = config_->edge();

//		// If this is an L edge, chop out the number for comparison purposes.
//		if (edge.contains(QRegExp("L\\d$")))
//			edge.chop(1);

//		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
//		for (int i = 0; i < detector->roiInfoList()->count(); i++){

//			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" ") % QString("-1el"), detector->roiInfoList()->at(i).name()));

//			if (detector->roiInfoList()->at(i).name().contains(edge))
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);

//			else
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
//		}

//		detector = VESPERSBeamline::vespers()->vortexXRF4E();

//		// This is safe and okay because I always have the regions of interest set taking up 0-X where X is the count-1 of the number of regions of interest.
//		for (int i = 0; i < detector->roiInfoList()->count(); i++){

//			scan_->rawData()->addMeasurement(AMMeasurementInfo(detector->roiInfoList()->at(i).name().remove(" ") % "-4el", detector->roiInfoList()->at(i).name()));

//			if (detector->roiInfoList()->at(i).name().contains(edge))
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);

//			else
//				scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), false, true);
//		}

//		// Add all the extra raw data sources.
//		addExtraDatasources();
//		addSingleElementSpectraMeasurments(scan_, AMMeasurementInfo(detector->toXRFInfo()));
//		addFourElementSpectraMeasurments(scan_, AMMeasurementInfo(detector->toXRFInfo()));

//		break;
//	}
//	}

//	// Add analysis blocks.
//	switch((int)config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:{

//		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
//		transmission->setDescription("Transmission");
//		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
//		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

//		scan_->addAnalyzedDataSource(transmission, true, false);

//		break;
//	}

//	case VESPERS::SingleElement:{

//		AM2DDeadTimeAB *correctedSpectra1El = new AM2DDeadTimeAB("correctedRawSpectra-1el");
//		correctedSpectra1El->setDescription("Corrected Spectra 1-El");
//		correctedSpectra1El->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("rawSpectra-1el")) << scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks")) << scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks")));
//		scan_->addAnalyzedDataSource(correctedSpectra1El, true, false);

//		AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
//		pfy->setDescription("PFY");
//		QList<AMDataSource*> pfySource;
//		pfySource << correctedSpectra1El;
//		pfy->setInputDataSources(pfySource);
//		pfy->setSumAxis(1);
//		pfy->setSumRangeMax(scan_->rawDataSources()->at(scan_->rawDataSourceCount()-1)->size(1)-1);
//		scan_->addAnalyzedDataSource(pfy, true, false);

//		AM1DExpressionAB *normPFY;
//		normPFY = new AM1DExpressionAB("norm_PFY");
//		normPFY->setDescription("Normalized PFY");
//		normPFY->setInputDataSources(QList<AMDataSource *>() << pfy << scan_->rawDataSources()->at(0));
//		normPFY->setExpression(QString("%1/%2").arg(scan_->analyzedDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(0)->name()));
//		scan_->addAnalyzedDataSource(normPFY, true, false);

//		QString edge = config_->edge();

//		// If this is an L edge, chop out the number for comparison purposes.
//		if (edge.contains(QRegExp("L\\d$")))
//			edge.chop(1);

//		AMDataSource *rawDataSource = 0;
//		AMDataSource *fastPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks"));
//		AMDataSource *slowPeakSource = scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks"));
//		AMAnalysisBlock *correctedROI = 0;

//		for (int i = 0; i < scan_->rawDataSourceCount(); i++){

//			if (scan_->rawDataSources()->at(i)->name().contains(edge.remove(" "))){

//				rawDataSource = scan_->rawDataSources()->at(i);
//				correctedROI = new AM1DDeadTimeAB("corrected_" % rawDataSource->name());
//				correctedROI->setDescription("Corrected " % rawDataSource->description());
//				correctedROI->setInputDataSources(QList<AMDataSource *>() << rawDataSource << fastPeakSource << slowPeakSource);
//				scan_->addAnalyzedDataSource(correctedROI, false, true);
//				normPFY = new AM1DExpressionAB("norm_"+scan_->rawDataSources()->at(i)->name());
//				normPFY->setDescription("Normalized "+scan_->rawDataSources()->at(i)->description());
//				normPFY->setInputDataSources(QList<AMDataSource *>() << correctedROI << scan_->rawDataSources()->at(i));
//				normPFY->setExpression(QString("%1/%2").arg(correctedROI->name()).arg(scan_->rawDataSources()->at(0)->name()));
//				scan_->addAnalyzedDataSource(normPFY, true, false);
//			}
//		}

//		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
//		transmission->setDescription("Transmission");
//		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
//		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

//		scan_->addAnalyzedDataSource(transmission, true, false);

//		break;
//	}

//	case VESPERS::FourElement:{

//		AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
//		pfy->setDescription("PFY");
//		QList<AMDataSource*> pfySource;
//		pfySource << scan_->rawDataSources()->at(scan_->rawDataSourceCount()-5);
//		pfy->setInputDataSources(pfySource);
//		pfy->setSumAxis(1);
//		pfy->setSumRangeMax(scan_->rawDataSources()->at(scan_->rawDataSourceCount()-5)->size(1)-1);
//		scan_->addAnalyzedDataSource(pfy, true, false);

//		AM1DExpressionAB *normPFY;
//		normPFY = new AM1DExpressionAB("norm_PFY");
//		normPFY->setDescription("Normalized PFY");
//		normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->analyzedDataSources()->at(0));
//		normPFY->setExpression(QString("%1/%2").arg(scan_->analyzedDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(0)->name()));
//		scan_->addAnalyzedDataSource(normPFY, true, false);

//		QString edge = config_->edge();

//		// If this is an L edge, chop out the number for comparison purposes.
//		if (edge.contains(QRegExp("L\\d$")))
//			edge.chop(1);

//		for (int i = 0; i < scan_->rawDataSourceCount(); i++){

//			if (scan_->rawDataSources()->at(i)->name().contains(edge.remove(" "))){

//				normPFY = new AM1DExpressionAB("norm_"+scan_->rawDataSources()->at(i)->name());
//				normPFY->setDescription("Normalized "+scan_->rawDataSources()->at(i)->description());
//				normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(i));
//				normPFY->setExpression(QString("%1/%2").arg(scan_->rawDataSources()->at(i)->name()).arg(scan_->rawDataSources()->at(0)->name()));
//				scan_->addAnalyzedDataSource(normPFY, true, false);
//			}
//		}

//		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
//		transmission->setDescription("Transmission");
//		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
//		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

//		scan_->addAnalyzedDataSource(transmission, true, false);

//		break;
//	}

//	case VESPERS::SingleElement | VESPERS::FourElement:{

//		AM2DDeadTimeAB *correctedSpectra1El = new AM2DDeadTimeAB("correctedRawSpectra-1el");
//		correctedSpectra1El->setDescription("Corrected Spectra 1-El");
//		correctedSpectra1El->setInputDataSources(QList<AMDataSource *>() << scan_->dataSourceAt(scan_->indexOfDataSource("rawSpectra-1el")) << scan_->dataSourceAt(scan_->indexOfDataSource("FastPeaks")) << scan_->dataSourceAt(scan_->indexOfDataSource("SlowPeaks")));
//		scan_->addAnalyzedDataSource(correctedSpectra1El, false, true);

//		AM2DAdditionAB *spectraSumAB = new AM2DAdditionAB("sumSpectra-1eland4el");
//		spectraSumAB->setDescription("Sum of Single and Four Element detectors");
//		spectraSumAB->setInputDataSources(QList<AMDataSource *>() << correctedSpectra1El << scan_->dataSourceAt(scan_->indexOfDataSource("correctedSum-4el")));
//		scan_->addAnalyzedDataSource(spectraSumAB, true, false);

//		AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
//		pfy->setDescription("PFY");
//		pfy->setInputDataSources(QList<AMDataSource *>() << spectraSumAB);
//		pfy->setSumAxis(1);
//		pfy->setSumRangeMax(spectraSumAB->size(1)-1);
//		scan_->addAnalyzedDataSource(pfy, true, false);

//		AM1DNormalizationAB *normPFY;
//		normPFY = new AM1DNormalizationAB("norm_PFY");
//		normPFY->setDescription("Normalized PFY");
//		normPFY->setDataName(pfy->name());
//		normPFY->setNormalizationName(scan_->rawDataSources()->at(0)->name());
//		normPFY->setInputDataSources(QList<AMDataSource *>() << pfy << scan_->rawDataSources()->at(0));
//		scan_->addAnalyzedDataSource(normPFY, true, false);

//		AMDataSource *roi1 = 0;
//		AMDataSource *roi4 = 0;
//		AM1DSummingAB *sumAB = 0;
//		QList<QPair<int, int> > sameRois = VESPERS::findRoiPairs(VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList(), VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
//		QStringList roiNames;
//		int singleElRoiCount = VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count();

//		QString edge = config_->edge();

//		// If this is an L edge, chop out the number for comparison purposes.
//		if (edge.contains(QRegExp("L\\d$")))
//			edge.chop(1);
//
//		for (int i = 0, count = sameRois.size(); i < count; i++){

//			roi1 = scan_->rawDataSources()->at(sameRois.at(i).first+4);
//			roi4 = scan_->rawDataSources()->at(sameRois.at(i).second+4+singleElRoiCount);
//			QString name = roi1->name().left(roi1->name().size()-4);
//			roiNames << name;

//			if (roi1->name().contains(edge.remove(" "))){

//				sumAB = new AM1DSummingAB("sum_" % name);
//				sumAB->setDescription("Summed " % roi1->description());
//				sumAB->setInputDataSources(QList<AMDataSource *>() << roi1 << roi4);
//				scan_->addAnalyzedDataSource(sumAB, false, true);
//			}
//		}

//		AMDataSource *source = 0;

//		for (int i = 0, count = scan_->analyzedDataSourceCount(); i < count; i++){

//			if (scan_->analyzedDataSources()->at(i)->name().contains("sum_")){

//				source = scan_->analyzedDataSources()->at(i);
//				normPFY = new AM1DNormalizationAB("norm_"+source->name());
//				normPFY->setDescription("Normalized "+source->description());
//				normPFY->setDataName(source->name());
//				normPFY->setNormalizationName(scan_->rawDataSources()->at(0)->name());
//				normPFY->setInputDataSources(QList<AMDataSource *>() << scan_->analyzedDataSources()->at(i) << scan_->rawDataSources()->at(0));
//				scan_->addAnalyzedDataSource(normPFY, true, false);
//			}
//		}

//		AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
//		transmission->setDescription("Transmission");
//		transmission->setInputDataSources(QList<AMDataSource *>() << scan_->rawDataSources()->at(0) << scan_->rawDataSources()->at(1));
//		transmission->setExpression(QString("ln(%1/%2)").arg(scan_->rawDataSources()->at(0)->name()).arg(scan_->rawDataSources()->at(1)->name()));

//		scan_->addAnalyzedDataSource(transmission, true, false);

//		break;
//	}
//	}

//	useDwellTimes(VESPERSBeamline::vespers()->dwellTimeTrigger(), VESPERSBeamline::vespers()->dwellTimeConfirmed());
//}

//void VESPERSEXAFSDacqScanController::addExtraDatasources()
//{
//	addStandardMeasurements(scan_, true, true);

//	switch((int)config_->fluorescenceDetector()){

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
//}

//bool VESPERSEXAFSDacqScanController::initializeImplementation()
//{
////	buildBaseInitializationAction(config_->regionTime(0));
////	AMBeamlineParallelActionsList *setupActionsList = initializationAction_->list();

////	// Add the rest of the stuff needed for XAS.
////	if (config_->exafsRegions()->hasKSpace() && !config_->useFixedTime()){

////		int regionCount = config_->regionCount();
////		double time = (regionCount > 1) ? config_->regionTime(regionCount - 2) : 1; // Grab the time from the region before the EXAFS region or default it to 1 second.
////		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->variableIntegrationTime()->createSetupAction(CLSVariableIntegrationTime::EnabledwThreshold,
////																													  time,
////																													  config_->regionStart(regionCount - 1),
////																													  VESPERSBeamline::vespers()->variableIntegrationTime()->function(),
////																													  config_->regionStart(regionCount - 1),
////																													  config_->regionEnd(regionCount - 1),
////																													  config_->regionTime(regionCount - 1)));
////	}

////	// Move the mono, and if necessary, move the sample stage.
////	setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
////	setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->mono()->createDelEAction(0));

////	if (config_->goToPosition() && config_->motor() == (VESPERS::H | VESPERS::V)){

////		setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
////		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createHorizontalMoveAction(config_->x()));
////		setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
////		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->createVerticalMoveAction(config_->y()));
////	}

////	else if (config_->goToPosition() && config_->motor() == (VESPERS::X | VESPERS::Z)){

////		setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
////		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createHorizontalMoveAction(config_->x()));
////		setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
////		setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->realSampleStageMotorGroupObject()->createVerticalMoveAction(config_->y()));
////	}

////	// Change the edge energy.
////	setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
////	setupActionsList->appendAction(setupActionsList->stageCount()-1, VESPERSBeamline::vespers()->mono()->createEoAction(config_->energy()));

////	connect(initializationAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
////	connect(initializationAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
////	connect(initializationAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
////	initializationAction_->start();

//	return true;
//}

//bool VESPERSEXAFSDacqScanController::startImplementation()
//{
//	currentRegionIndex_ = 0;

//	if (config_->motor() == (VESPERS::H | VESPERS::V)){

//		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageHorizontal()->toInfo());
//		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageVertical()->toInfo());
//	}
//	else if (config_->motor() == (VESPERS::X | VESPERS::Z)){

//		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageX()->toInfo());
//		scan_->scanInitialConditions()->append(VESPERSBeamline::vespers()->sampleStageZ()->toInfo());
//	}

//	// Setup the real config.
//	switch(config_->fluorescenceDetector()){

//	case VESPERS::NoXRF:
//		if (!setupTransmissionXAS()){

//			AMErrorMon::alert(this,
//					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}
//		break;

//	case VESPERS::SingleElement:
//		if (!setupSingleElementXAS()){

//			AMErrorMon::alert(this,
//					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}
//		break;

//	case VESPERS::FourElement:
//		if (!setupFourElementXAS()){

//			AMErrorMon::alert(this,
//					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}
//		break;

//	case VESPERS::SingleElement | VESPERS::FourElement:
//		if (!setupSingleAndFourElementXAS()){

//			AMErrorMon::alert(this,
//					VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS EXAFS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}
//		break;

//	default:
//		AMErrorMon::alert(this,
//				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS EXAFS DACQ Scan Controller failed to start (Invalid Fluorescence Detector chosen). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	for (int i = 0; i < config_->regionCount(); i++){

//		if (advAcq_->getNumRegions() == i){

//			AMPVwStatusControl *control = 0;

//			if (config_->regionType(i) == AMEXAFSRegion::Energy){

//				control = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());

//				if (i == 0)
//					advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i), config_->regionDelta(i), config_->regionEnd(i), 1);
//				else
//					advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i)+config_->regionDelta(i), config_->regionDelta(i), config_->regionEnd(i), 1);
//			}
//			else if (config_->regionType(i) == AMEXAFSRegion::kSpace){

//				control = qobject_cast<AMPVwStatusControl *>(config_->exafsRegions()->defaultKControl());

//				if (i == 0)
//					advAcq_->addRegion(i, control->writePVName(), config_->regionStartByType(i, AMEXAFSRegion::kSpace), config_->regionDelta(i), config_->regionEnd(i), 1);
//				else
//					advAcq_->addRegion(i, control->writePVName(), config_->regionStartByType(i, AMEXAFSRegion::kSpace)+config_->regionDelta(i), config_->regionDelta(i), config_->regionEnd(i), 1);
//			}
//		}

//		else {

//			if (i == 0)
//				advAcq_->setStart(i, config_->regionStart(i));
//			else
//				advAcq_->setStart(i, config_->regionStart(i)+config_->regionDelta(i));

//			advAcq_->setDelta(i, config_->regionDelta(i));
//			advAcq_->setEnd(i, config_->regionEnd(i));
//		}
//	}
////	advAcq_->saveConfigFile("/home/hunterd/Desktop/writeTest.cfg");

//	return AMDacqScanController::startImplementation();
//}

//void VESPERSEXAFSDacqScanController::cleanup()
//{
//	buildCleanupAction(true);
////	connect(cleanupAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
////	connect(cleanupAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
////	cleanupAction_->start();
//}

//void VESPERSEXAFSDacqScanController::onCleanupFinished()
//{
//	AMDacqScanController::onDacqStop();
//}

//void VESPERSEXAFSDacqScanController::onDwellTimeTriggerChanged(double newValue)
//{
//	if( fabs(newValue - 1.0) < 0.1){

//		// Only set the time for the region if in energy space or if told explicitly to do so.  The variable integration time app handles this in k-space.
//		if ((config_->regionType(currentRegionIndex_) == AMEXAFSRegion::Energy) || (config_->regionType(currentRegionIndex_) == AMEXAFSRegion::kSpace && config_->useFixedTime()))
//			VESPERSBeamline::vespers()->synchronizedDwellTime()->setTime(config_->regionTime(currentRegionIndex_));

//		currentRegionIndex_++;
//		dwellTimeTrigger_->move(0);
//		dwellTimeConfirmed_->move(1);
//	}
//}

//AMnDIndex VESPERSEXAFSDacqScanController::toScanIndex(QMap<int, double> aeData)
//{
//	Q_UNUSED(aeData)
//	return AMnDIndex(scan_->rawData()->scanSize(0));
//}

//void VESPERSEXAFSDacqScanController::onScanTimerUpdate()
//{
//	if (elapsedTime_.isActive()){

//		if (secondsElapsed_ >= secondsTotal_)
//			secondsElapsed_ = secondsTotal_;
//		else
//			secondsElapsed_ += 1.0;

//		emit progress(secondsElapsed_, secondsTotal_);
//	}
//}

//void VESPERSEXAFSDacqScanController::onInitializationActionsSucceeded()
//{
//	setInitialized();
//}

//void VESPERSEXAFSDacqScanController::onInitializationActionsFailed(int explanation)
//{
//	Q_UNUSED(explanation)

//	AMErrorMon::alert(this, 0, "XAS scan failed to initialize.");
////	onInitializationActionFinished();
//	setFailed();
//}

//void VESPERSEXAFSDacqScanController::onInitializationActionsProgress(double elapsed, double total)
//{
//	Q_UNUSED(elapsed)
//	Q_UNUSED(total)
//}

//bool VESPERSEXAFSDacqScanController::setupTransmissionXAS()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setPvNameAxis1("07B2_Mono_SineB_delE");
//	builder.buildConfigurationFile();

//	bool loadSuccess = false;

//	loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addStandardExtraPVs(advAcq_, true, true);

//	return loadSuccess;
//}

//bool VESPERSEXAFSDacqScanController::setupSingleElementXAS()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setPvNameAxis1("07B2_Mono_SineB_delE");
//	builder.setSingleElement(true);
//	builder.buildConfigurationFile();

//	bool loadSuccess = false;

//	loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addSingleElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count());
//	addStandardExtraPVs(advAcq_, true, true);
//	addSingleElementDeadTimePVs(advAcq_);
//	addSingleElementSpectraPVs(advAcq_);

//	return loadSuccess;
//}

//bool VESPERSEXAFSDacqScanController::setupFourElementXAS()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setPvNameAxis1("07B2_Mono_SineB_delE");
//	builder.setFourElement(true);
//	builder.buildConfigurationFile();

//	bool loadSuccess = false;

//	loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addFourElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count());
//	addStandardExtraPVs(advAcq_, true, true);
//	addFourElementDeadTimePVs(advAcq_);
//	addFourElementSpectraPVs(advAcq_);

//	return loadSuccess;
//}

//bool VESPERSEXAFSDacqScanController::setupSingleAndFourElementXAS()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setPvNameAxis1("07B2_Mono_SineB_delE");
//	builder.setSingleElement(true);
//	builder.setFourElement(true);
//	builder.buildConfigurationFile();

//	bool loadSuccess = false;

//	loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSEXAFSDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	usingSpectraDotDatFile_ = true;

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->incomingChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->incomingChoice())->readMethod()));
////	advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt((int)config_->transmissionChoice())->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt((int)config_->transmissionChoice())->readMethod()));

////	for (int i = 0; i < ionChambers->count(); i++)
////		if (i != (int)config_->incomingChoice() && i != (int)config_->transmissionChoice())
////			advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addSingleElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList()->count());
//	addFourElementRegionsOfInterestPVs(advAcq_, VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList()->count());

//	addStandardExtraPVs(advAcq_, true, true);
//	addSingleElementDeadTimePVs(advAcq_);
//	addFourElementDeadTimePVs(advAcq_);
//	addSingleElementSpectraPVs(advAcq_);
//	addFourElementSpectraPVs(advAcq_);

//	return loadSuccess;
//}
