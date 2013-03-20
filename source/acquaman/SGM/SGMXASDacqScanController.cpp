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

#include "SGMXASDacqScanController.h"

#include <QDir>
#include "util/SGM/SGMDacqConfigurationFile.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "actions/AMBeamlineListAction.h"

SGMXASDacqScanController::SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent) , SGMXASScanController(cfg)
{
	scan_ = specificScan_; // MB: Moved from startImplementation(). specificScan_ is new'd in SGMXASScanController, outside of the AMScanController inheritance tree.

	useDwellTimes(SGMBeamline::sgm()->nextDwellTimeTrigger(), SGMBeamline::sgm()->nextDwellTimeConfirmed());
}

SGMXASDacqScanController::~SGMXASDacqScanController()
{
}

void SGMXASDacqScanController::onDacqStop(){
	if(dacqCancelled_)
		AMDacqScanController::onDacqStop();
	else
		onScanFinished();
}

#include "beamline/CLS/CLSSynchronizedDwellTime.h"

void SGMXASDacqScanController::onDwellTimeTriggerChanged(double newValue){
	if( fabs(newValue - 1.0) < 0.1 ){

		/*
		int curDwell = ceil(SGMBeamline::sgm()->synchronizedDwellTime()->time());
		switch(curDwell){
		case 1:
			SGMBeamline::sgm()->synchronizedDwellTime()->setTime(2);
			break;
		case 2:
			SGMBeamline::sgm()->synchronizedDwellTime()->setTime(2.5);
			break;
		case 3:
			SGMBeamline::sgm()->synchronizedDwellTime()->setTime(1);
			break;
		}
		*/

		SGMBeamline::sgm()->synchronizedDwellTime()->setTime(config_->regionTime(currentRegionIndex_));
		currentRegionIndex_++;
		dwellTimeTrigger_->move(0);
		dwellTimeConfirmed_->move(1);
	}
}

void SGMXASDacqScanController::onInitializationActionsSucceeded(){
	setInitialized();
}

void SGMXASDacqScanController::onInitializationActionsFailed(int explanation){
	Q_UNUSED(explanation)
	AMErrorMon::report(AMErrorReport(this,
			AMErrorReport::Alert,
			SGMXASDACQSCANCONTROLLER_INITIALZATION_FAILED,
			"Error, SGM XAS DACQ Scan Controller failed to initialize - one or more stages of initialization failed. Please report this bug to the Acquaman developers."));
	setFailed();
}

void SGMXASDacqScanController::onInitializationActionsProgress(double elapsed, double total){
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
}

void SGMXASDacqScanController::onScanFinished(){
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(setFinished()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

void SGMXASDacqScanController::onScanCancelledBeforeInitialized(){
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(onDacqStop()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

void SGMXASDacqScanController::onScanCancelledWhileRunning(){

}

bool SGMXASDacqScanController::initializeImplementation(){
	if(SGMXASScanController::beamlineInitialize() && initializationActions_){
		/* NTBA - August 25th, 2011 (David Chevrier)
			Do we need to also clear any raw data sources here, or just the raw data itself?"
		*/
		scan_->clearRawDataPoints();

		AMBeamlineListAction *listAction = new AMBeamlineListAction(initializationActions_);
		connect(listAction, SIGNAL(succeeded()), this, SLOT(onInitializationActionsSucceeded()));
		connect(listAction, SIGNAL(failed(int)), this, SLOT(onInitializationActionsFailed(int)));
		connect(listAction, SIGNAL(progress(double,double)), this, SLOT(onInitializationActionsProgress(double,double)));
		listAction->start();
		return true;
	}
	AMErrorMon::report(AMErrorReport(this,
			AMErrorReport::Alert,
			SGMXASDACQSCANCONTROLLER_CANT_INTIALIZE,
			"Error, SGM XAS DACQ Scan Controller failed to initialize - the initialization actions could not be created. Please report this bug to the Acquaman developers."));
	return false;
}

bool SGMXASDacqScanController::startImplementation(){
	if(SGMBeamline::sgm()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASDACQSCANCONTROLLER_CANT_INTIALIZE,
				"Error, SGM XAS DACQ Scan Controller failed to start (SGM is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning."));
		return false;
	}

	currentRegionIndex_ = 0;

	bool loadSuccess = false;

	QStringList sgmBeginStage, sgmMoveStage, sgmFinishStage;
	sgmBeginStage << QString("%1||=||%2||=||%3").arg("SetPV").arg("BL1611-ID-1:scanning").arg("1");
	sgmBeginStage << QString("%1||=||%2||=||%3").arg("SetPV").arg("BL1611-ID-1:Dwell:setMode").arg("1");
	sgmMoveStage << QString("%1||=||%2||=||%3").arg("WaitPV").arg("BL1611-ID-1:ready").arg("STOPPED");
	sgmFinishStage << QString("%1||=||%2||=||%3").arg("SetPV").arg("BL1611-ID-1:scanning").arg("0");

	//QStringList scalerDwellStage;
	//scalerDwellStage << QString("%1||=||%2||=||%3").arg("SetPV").arg("BL1611-ID-1:mcs:startScan").arg("1");
	QStringList synchronizedDwellStage;
	synchronizedDwellStage << QString("%1||=||%2||=||%3").arg("SetPV").arg("BL1611-ID-1:dwell:startScan").arg("1");

	QStringList allBeginStage, allMoveStage, allDwellStage, allFinishStage;
	allBeginStage.append(sgmBeginStage);
	allMoveStage.append(sgmMoveStage);
	//allDwellStage.append(scalerDwellStage);
	allDwellStage.append(synchronizedDwellStage);
	allFinishStage.append(sgmFinishStage);

	for(int i = 0; i < config_->allDetectorConfigurations().count(); i++){
		if(config_->allDetectorConfigurations().isActiveAt(i)){
			AMOldDetector *dtctr = config_->allDetectors()->detectorNamed(config_->allDetectorConfigurations().detectorInfoAt(i)->name());
			if(!dtctr->dacqBegin().isEmpty())
				allBeginStage.append(dtctr->dacqBegin());
			if(!dtctr->dacqMove().isEmpty())
				allMoveStage.append(dtctr->dacqMove());
			if(!dtctr->dacqDwell().isEmpty())
				allDwellStage.append(dtctr->dacqDwell());
			if(!dtctr->dacqFinish().isEmpty())
				allFinishStage.append(dtctr->dacqFinish());

			if(!usingSpectraDotDatFile_ && (dtctr->toInfo()->rank() != 0) )
				usingSpectraDotDatFile_ = true;
		}
	}

	QString dacqConfigFile;
	dacqConfigFile.append("# Version 20600\n");
	dacqConfigFile.append("# Scan \"newScan\" 2 onStart:1\n");
	dacqConfigFile.append("# Control \"BL1611-ID-1:Energy\" start:1500 delta:1 final:1600 active:7\n");

	dacqConfigFile.append("# Action Begin CallEvent \"header\" 1\n");
	for(int x = 0; x < allBeginStage.count(); x++){
		dacqConfigFile.append(QString("# Action Begin %1 ").arg(allBeginStage.at(x).section("||=||", 0, 0)));
		dacqConfigFile.append(QString("\"%1\" ").arg(allBeginStage.at(x).section("||=||", 1, 1)));
		dacqConfigFile.append(QString("\"%1\"\n").arg(allBeginStage.at(x).section("||=||", 2, 2)));
	}
	dacqConfigFile.append("# Action Begin Delay 0.1\n");
	dacqConfigFile.append("# Action Move Delay 0.1\n");
	for(int x = 0; x < allMoveStage.count(); x++){
		if(allMoveStage.at(x).section("||=||", 0, 0) == "SetPV"){
			dacqConfigFile.append(QString("# Action Move %1 ").arg(allMoveStage.at(x).section("||=||", 0, 0)));
			dacqConfigFile.append(QString("\"%1\" ").arg(allMoveStage.at(x).section("||=||", 1, 1)));
			dacqConfigFile.append(QString("\"%1\"\n").arg(allMoveStage.at(x).section("||=||", 2, 2)));
		}
	}
	for(int x = 0; x < allMoveStage.count(); x++){
		if(allMoveStage.at(x).section("||=||", 0, 0) != "SetPV"){
			dacqConfigFile.append(QString("# Action Move %1 ").arg(allMoveStage.at(x).section("||=||", 0, 0)));
			dacqConfigFile.append(QString("\"%1\" ").arg(allMoveStage.at(x).section("||=||", 1, 1)));
			dacqConfigFile.append(QString("\"%1\"\n").arg(allMoveStage.at(x).section("||=||", 2, 2)));
		}
	}
	for(int x = 0; x < allDwellStage.count(); x++){
		dacqConfigFile.append(QString("# Action Dwell %1 ").arg(allDwellStage.at(x).section("||=||", 0, 0)));
		dacqConfigFile.append(QString("\"%1\" ").arg(allDwellStage.at(x).section("||=||", 1, 1)));
		dacqConfigFile.append(QString("\"%1\"\n").arg(allDwellStage.at(x).section("||=||", 2, 2)));
	}
	dacqConfigFile.append("# Action Dwell CallEvent \"read\" 1\n");
	for(int x = 0; x < allFinishStage.count(); x++){
		dacqConfigFile.append(QString("# Action Finish %1 ").arg(allFinishStage.at(x).section("||=||", 0, 0)));
		dacqConfigFile.append(QString("\"%1\" ").arg(allFinishStage.at(x).section("||=||", 1, 1)));
		dacqConfigFile.append(QString("\"%1\"\n").arg(allFinishStage.at(x).section("||=||", 2, 2)));
	}
	dacqConfigFile.append("# Action StartPass SetPV \"BL1611-ID-1:AddOns:confirmed:dwellTime\" \"0\"\n");
	dacqConfigFile.append("# Action StartPass SetPV \"BL1611-ID-1:AddOns:trigger:dwellTime\" \"1\"\n");
	dacqConfigFile.append("# Action StartPass Delay 0.5\n");
	dacqConfigFile.append("# Action StartPass WaitPV \"BL1611-ID-1:AddOns:confirmed:dwellTime\" \"1\"\n");
	dacqConfigFile.append("# Event \"read\" 1\n");
	dacqConfigFile.append("# datastream columns: eventID, absolute/relative time stamps\n");
	dacqConfigFile.append("# eventID:1 AbsTime:0 Rel0Time:0 relPTime:0\n");
	dacqConfigFile.append("# commentPrefix: 0\n");
	dacqConfigFile.append("# PV 0: \"BL1611-ID-1:Energy\" disable:0 format:\"%3.6e\" spectrum:0 ready:0\n");
	dacqConfigFile.append("#\n");
	dacqConfigFile.append("# Event \"header\" 2\n");
	dacqConfigFile.append("# datastream columns: eventID, absolute/relative time stamps\n");
	dacqConfigFile.append("# eventID:1 AbsTime:0 Rel0Time:0 relPTime:0\n");
	dacqConfigFile.append("# commentPrefix: 1\n");
	dacqConfigFile.append("# PV 0: \"BL1611-ID-1:Energy\" disable:0 format:\"Crystal=%g\" spectrum:0 ready:0\n");
	dacqConfigFile.append("#\n");
	dacqConfigFile.append("# File: \"/home/sgm\" sequence:0 time:0 datedir:0\n");
	dacqConfigFile.append("# Template: \"Test_%d.dat\"\n");
	dacqConfigFile.append("# Sequence: 1\n");
	dacqConfigFile.append("# Header: 0\n");
	dacqConfigFile.append("# SpectrumFormat: 2\n");

	QList<int> matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "FastScaler");
	if(matchIDs.count() == 0){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASDACQSCANCONTROLLER_CANT_START_NO_TEMPLATE_SAVE_PATH,
				"Error, SGM XAS DACQ Scan Controller failed to start (couldn't find a save path for the template'). Please report this bug to the Acquaman developers."));
		return false;
	}

	QString pathToUserDb = AMDatabase::database("user")->dbAccessString().replace("//", "/").section("/", 0, -2);
	QDir configTemplateDir;
	configTemplateDir.setPath(pathToUserDb);
	if(!configTemplateDir.cd(".TEMPLATE")){
		configTemplateDir.mkdir(".TEMPLATE");
		if(!configTemplateDir.cd(".TEMPLATE")){
			AMErrorMon::report(AMErrorReport(this,
					AMErrorReport::Alert,
					SGMXASDACQSCANCONTROLLER_CANT_START_CANT_FIND_TEMPLATE_DIRECTORY,
					"Error, SGM XAS DACQ Scan Controller failed to start (couldn't find the template directory). Please report this bug to the Acquaman developers."));
			return false;
		}
	}
	QString templateFullFilePath = configTemplateDir.absolutePath()+"/template.cfg";

	QFile file(templateFullFilePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASDACQSCANCONTROLLER_CANT_START_CANT_WRITE_TEMPLATE,
				"Error, SGM XAS DACQ Scan Controller failed to start (couldn't write the template). Please report this bug to the Acquaman developers."));
		return false;
	}

	QTextStream out(&file);
	out << dacqConfigFile;
	file.close();

	loadSuccess = advAcq_->setConfigFile(templateFullFilePath);
	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, SGM XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	for(int i = 0; i < config_->allDetectorConfigurations().count(); i++){
		if(config_->allDetectorConfigurations().isActiveAt(i)){
			AMOldDetector *dtctr = config_->allDetectors()->detectorNamed(config_->allDetectorConfigurations().detectorInfoAt(i)->name());
//			qdebug() << "Dacq append record as " << dtctr->detectorName() << dtctr->toInfo()->rank() << dtctr->dacqName() << dtctr->readMethod();
			if(dtctr->toInfo()->rank() > 0)
				advAcq_->appendRecord(dtctr->dacqName(), true, true, detectorReadMethodToDacqReadMethod(dtctr->readMethod()));
			else
				advAcq_->appendRecord(dtctr->dacqName(), true, false, detectorReadMethodToDacqReadMethod(dtctr->readMethod()));
		}
	}
	for(int x = 0; x < config_->regionCount(); x++){
		if(advAcq_->getNumRegions() == x && x == 0)
			advAcq_->addRegion(x, config_->regionStart(x), config_->regionDelta(x), config_->regionEnd(x), 1);
		else if (advAcq_->getNumRegions() == x)
			advAcq_->addRegion(x, config_->regionStart(x)+config_->regionDelta(x), config_->regionDelta(x), config_->regionEnd(x), 1);
		else{

			if (x == 0)
				advAcq_->setStart(x, config_->regionStart(x));
			else
				advAcq_->setStart(x, config_->regionStart(x)+config_->regionDelta(x));

			advAcq_->setDelta(x, config_->regionDelta(x));
			advAcq_->setEnd(x, config_->regionEnd(x));
		}
	}
	advAcq_->saveConfigFile(configTemplateDir.absolutePath()+"/fullyFormedTemplate.cfg");
	return AMDacqScanController::startImplementation();
}

void SGMXASDacqScanController::cancelImplementation(){
	dacqCancelled_ = true;
	if(initializationActions_ && initializationActions_->isRunning()){
		disconnect(initializationActions_, 0);
		connect(initializationActions_, SIGNAL(listSucceeded()), this, SLOT(onScanCancelledBeforeInitialized()));
		connect(initializationActions_, SIGNAL(listFailed(int)), this, SLOT(onScanCancelledBeforeInitialized()));
	}
	else
		AMDacqScanController::cancelImplementation();
}

AMnDIndex SGMXASDacqScanController::toScanIndex(QMap<int, double> aeData){
	Q_UNUSED(aeData)
	// SGM XAS Scan has only one dimension (energy), simply append to the end of this
	return AMnDIndex(scan_->rawData()->scanSize(0));
}
