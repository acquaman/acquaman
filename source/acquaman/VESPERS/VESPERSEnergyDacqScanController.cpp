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


#include "VESPERSEnergyDacqScanController.h"

//#include "beamline/VESPERS/VESPERSBeamline.h"
//#include "dataman/AMUser.h"
////#include "actions/AMBeamlineParallelActionsList.h"
//#include "util/VESPERS/VESPERSConfigurationFileBuilder.h"
//#include "dataman/datastore/AMCDFDataStore.h"

//#include <QDir>
//#include <QStringBuilder>

//VESPERSEnergyDacqScanController::VESPERSEnergyDacqScanController(VESPERSEnergyScanConfiguration *cfg, QObject *parent)
//	: AMDacqScanController(cfg, parent), VESPERSScanController(cfg)
//{
//	config_ = cfg;

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

//	scan_->setNotes(buildNotes());

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();
//	AMMeasurementInfo temp = AMMeasurementInfo("", "");

////	for (int i = 0; i < ionChambers->count(); i++){

////		temp = AMMeasurementInfo(*(ionChambers->detectorAt(i)->toInfo()));
////		temp.name = ionChambers->detectorAt(i)->detectorName();
////		scan_->rawData()->addMeasurement(temp);
////		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount() - 1), true, false);
////	}

//	addExtraDatasources();
//	useDwellTimes(VESPERSBeamline::vespers()->dwellTimeTrigger(), VESPERSBeamline::vespers()->dwellTimeConfirmed());
//}

//void VESPERSEnergyDacqScanController::addExtraDatasources()
//{
//	addStandardMeasurements(scan_, true, false);

//	if (config_->ccdDetector() != VESPERS::NoCCD){

//		scan_->rawData()->addMeasurement(AMMeasurementInfo("CCDFileNumber", "CCD file number"));
//		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), false, true);
//	}
//}

//bool VESPERSEnergyDacqScanController::initializeImplementation()
//{
////	buildBaseInitializationAction(config_->regionTime(0));

////	QString ccdName = buildCCDInitializationAction(config_->ccdDetector(), config_->ccdFileName());

////	if (config_->ccdFileName() != ccdName)
////		config_->setCCDFileName(ccdName);

////	AMBeamlineParallelActionsList *setupActionsList = initializationAction_->list();

////	// Fourth stage.
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

////	connect(initializationAction_, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
////	connect(initializationAction_, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
////	connect(initializationAction_, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
////	initializationAction_->start();

//	return true;
//}

//bool VESPERSEnergyDacqScanController::startImplementation()
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
//	switch(config_->ccdDetector()){

//	case VESPERS::Roper:
//		if (!setupRoperScan()){

//			AMErrorMon::alert(this,
//					VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS Energy DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}
//		break;

//	case VESPERS::Mar:
//		if (!setupMarScan()){

//			AMErrorMon::alert(this,
//					VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS Energy DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}
//		break;

//	case VESPERS::Pilatus:
//		if (!setupPilatusScan()){

//			AMErrorMon::alert(this,
//					VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//					"Error, VESPERS Energy DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//			return false;
//		}

//		break;

//	default:
//		AMErrorMon::alert(this,
//				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS EXAFS DACQ Scan Controller failed to start (Invalid Fluorescence Detector chosen). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	for (int i = 0; i < config_->regionCount(); i++){

//		if (advAcq_->getNumRegions() == i){

//			AMPVwStatusControl *control = 0;

//			control = qobject_cast<AMPVwStatusControl *>(config_->regions()->defaultControl());

//			if (i == 0)
//				advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i), config_->regionDelta(i), config_->regionEnd(i), 1);
//			else
//				advAcq_->addRegion(i, control->writePVName(), config_->regionStart(i)+config_->regionDelta(i), config_->regionDelta(i), config_->regionEnd(i), 1);
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
//	advAcq_->saveConfigFile("/home/hunterd/Desktop/writeTest.cfg");

//	return AMDacqScanController::startImplementation();
//}

//void VESPERSEnergyDacqScanController::cleanup()
//{
//	buildCleanupAction(true);
////	connect(cleanupAction_, SIGNAL(succeeded()), this, SLOT(onCleanupFinished()));
////	connect(cleanupAction_, SIGNAL(failed(int)), this, SLOT(onCleanupFinished()));
////	cleanupAction_->start();
//}

//void VESPERSEnergyDacqScanController::onCleanupFinished()
//{
//	AMDacqScanController::onDacqStop();
//}

//void VESPERSEnergyDacqScanController::onDwellTimeTriggerChanged(double newValue)
//{
//	if( fabs(newValue - 1.0) < 0.1){

//		VESPERSBeamline::vespers()->synchronizedDwellTime()->setTime(config_->regionTime(currentRegionIndex_));
//		currentRegionIndex_++;
//		dwellTimeTrigger_->move(0);
//		dwellTimeConfirmed_->move(1);
//	}
//}

//AMnDIndex VESPERSEnergyDacqScanController::toScanIndex(QMap<int, double> aeData)
//{
//	Q_UNUSED(aeData)
//	return AMnDIndex(scan_->rawData()->scanSize(0));
//}

//void VESPERSEnergyDacqScanController::onScanTimerUpdate()
//{
//	if (elapsedTime_.isActive()){

//		if (secondsElapsed_ >= secondsTotal_)
//			secondsElapsed_ = secondsTotal_;
//		else
//			secondsElapsed_ += 1.0;

//		emit progress(secondsElapsed_, secondsTotal_);
//	}
//}

//void VESPERSEnergyDacqScanController::onInitializationActionsSucceeded()
//{
//	setInitialized();
//}

//void VESPERSEnergyDacqScanController::onInitializationActionsFailed(int explanation)
//{
//	Q_UNUSED(explanation)

//	AMErrorMon::alert(this, 0, "Energy scan failed to initialize.");
////	onInitializationActionFinished();
//	setFailed();
//}

//void VESPERSEnergyDacqScanController::onInitializationActionsProgress(double elapsed, double total)
//{
//	Q_UNUSED(elapsed)
//	Q_UNUSED(total)
//}

//bool VESPERSEnergyDacqScanController::setupRoperScan()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setRoperCCD(true);
//	builder.setPvNameAxis1("07B2_Mono_SineB_Ea");
//	builder.buildConfigurationFile();

//	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++)
////		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addStandardExtraPVs(advAcq_, true, false);

//	advAcq_->appendRecord("BL1607-B2-1:AddOns:Roper:FileNumber", true, false, 0);

//	return loadSuccess;
//}

//bool VESPERSEnergyDacqScanController::setupMarScan()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setMarCCD(true);
//	builder.setPvNameAxis1("07B2_Mono_SineB_Ea");
//	builder.buildConfigurationFile();

//	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++)
////		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addStandardExtraPVs(advAcq_, true, false);

//	advAcq_->appendRecord("BL1607-B2-1:AddOns:Mar:FileNumber", true, false, 0);

//	return loadSuccess;
//}

//bool VESPERSEnergyDacqScanController::setupPilatusScan()
//{
//	VESPERSConfigurationFileBuilder builder;
//	builder.setDimensions(1);
//	builder.setPilatusCCD(true);
//	builder.setPvNameAxis1("07B2_Mono_SineB_Ea");
//	builder.buildConfigurationFile();

//	bool loadSuccess = advAcq_->setConfigFile(VESPERS::getHomeDirectory().append("/acquaman/devConfigurationFiles/VESPERS/template.cfg"));

//	if(!loadSuccess){
//		AMErrorMon::alert(this,
//				VESPERSENERGYDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
//				"Error, VESPERS XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers.");
//		return false;
//	}

//	// Remove all the "goober" records that were added to create enough space for the Dacq.  (Hack the Dacq solution).
//	while (advAcq_->deleteRecord(1)){}

////	AMOldDetectorSet *ionChambers = VESPERSBeamline::vespers()->ionChambers();

////	for (int i = 0; i < ionChambers->count(); i++)
////		advAcq_->appendRecord(VESPERSBeamline::vespers()->pvName(ionChambers->detectorAt(i)->detectorName()), true, false, detectorReadMethodToDacqReadMethod(ionChambers->detectorAt(i)->readMethod()));

//	addStandardExtraPVs(advAcq_, true, false);

//	advAcq_->appendRecord("BL1607-B2-1:AddOns:Pilatus:FileNumber", true, false, 0);

//	return loadSuccess;
//}
