/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
			"Error, SGM XAS DACQ Scan Controller failed to initialize. Please report this bug to the Acquaman developers."));
	return false;
}

bool SGMXASDacqScanController::startImplementation(){
	if(SGMBeamline::sgm()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASDACQSCANCONTROLLER_CANT_INTIALIZE,
				"Error, SGM XAS DACQ Scan Controller failed to start (SGM is already scanning). Please report this bug to the Acquaman developers."));
		return false;
	}
	bool loadSuccess = false;

	for(int x = 0; x < config_->allDetectorConfigurations().count(); x++){
		if(config_->allDetectorConfigurations().isActiveAt(x) && !SGMBeamline::sgm()->detectorValidForCurrentSignalSource(config_->allDetectorConfigurations().detectorInfoAt(x))){
			AMErrorMon::report(AMErrorReport(this,
					AMErrorReport::Alert,
					SGMXASDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH,
					"Error, SGM XAS DACQ Scan Controller failed to start. The SGM Beamline thinks you're configured to use the wrong detectors (picoammeters versus scalers). Please report this bug to the Acquaman developers."));
			return false;
		}
	}

	SGMDacqConfigurationFile *configFile = new SGMDacqConfigurationFile();
	QList<int> matchIDs;
	if( SGMBeamline::sgm()->pgtDetector() && SGMBeamline::sgm()->oos65000Detector() && config_->allDetectorConfigurations().isActiveNamed(SGMBeamline::sgm()->pgtDetector()->detectorName())
		&& config_->allDetectorConfigurations().isActiveNamed(SGMBeamline::sgm()->oos65000Detector()->detectorName())){
		qDebug() << "Using SDD and OOS";
		if(SGMBeamline::sgm()->usingPicoammeterSource())
			matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTXEOLAmmeter");
		else if(SGMBeamline::sgm()->usingScalerSource())
			matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTXEOLScaler");
		usingSpectraDotDatFile_ = true;
	}
	else if(SGMBeamline::sgm()->pgtDetector() && config_->allDetectorConfigurations().isActiveNamed(SGMBeamline::sgm()->pgtDetector()->detectorName())){
		qDebug() << "Using SDD";
		if(SGMBeamline::sgm()->usingPicoammeterSource())
			matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTAmmeter");
		else if(SGMBeamline::sgm()->usingScalerSource())
			matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "PGTScaler");
		usingSpectraDotDatFile_ = true;
	}
	else if(SGMBeamline::sgm()->oos65000Detector() && config_->allDetectorConfigurations().isActiveNamed(SGMBeamline::sgm()->oos65000Detector()->detectorName())){
		qDebug() << "Using OOS";
		if(SGMBeamline::sgm()->usingPicoammeterSource())
			matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "XEOLAmmeter");
		else if(SGMBeamline::sgm()->usingScalerSource())
			matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "XEOLScaler");
		usingSpectraDotDatFile_ = true;
	}
	else if(SGMBeamline::sgm()->usingPicoammeterSource())
		matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "DefaultAmmeter");
	else if(SGMBeamline::sgm()->usingScalerSource())
		matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "DefaultScaler");

	if(matchIDs.count() > 0){
		configFile->loadFromDb(AMDatabase::database("SGMBeamline"), matchIDs.at(0));
		loadSuccess = advAcq_->setConfigFile(configFile->configurationFileFullPath());
	}
	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
				"Error, SGM XAS DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	for(int i = 0; i < config_->allDetectorConfigurations().count(); i++){
		if(config_->allDetectorConfigurations().isActiveAt(i)){
			AMDetector *dtctr = config_->allDetectors()->detectorNamed(config_->allDetectorConfigurations().detectorInfoAt(i)->name());
			//if(dtctr->detectorName() == SGMBeamline::sgm()->pgtDetector()->detectorName())
			if(dtctr->toInfo()->rank() > 0)
				advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->detectorName()), true, true, detectorReadMethodToDacqReadMethod(dtctr->readMethod()));
			else
				advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->detectorName()), true, false, detectorReadMethodToDacqReadMethod(dtctr->readMethod()));
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
	advAcq_->saveConfigFile("/Users/fawkes/dev/acquaman/devConfigurationFiles/davidTest.cfg");

	qDebug() << "Calling startImplementation for dacq";
	return AMDacqScanController::startImplementation();
}

void SGMXASDacqScanController::cancelImplementation(){
	dacqCancelled_ = true;
	if(initializationActions_ && initializationActions_->isRunning()){
		qDebug() << "Need to stop the intialization actions";
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

void SGMXASDacqScanController::onDacqStop(){
	if(dacqCancelled_)
		AMDacqScanController::onDacqStop();
	else
		onScanFinished();
}

#include "beamline/CLS/CLSSynchronizedDwellTime.h"

void SGMXASDacqScanController::onDwellTimeTriggerChanged(double newValue){
	qDebug() << "Looks like dwell time trigger has changed to " << newValue << " in SGM version";
	if( fabs(newValue - 1.0) < 0.1 ){
		qDebug() << "Confirm dwell time has been set and reset trigger";
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

		dwellTimeTrigger_->move(0);
		dwellTimeConfirmed_->move(1);
	}
}

void SGMXASDacqScanController::onInitializationActionsSucceeded(){
	qDebug() << "The actions list succeeded";
	setInitialized();
}

void SGMXASDacqScanController::onInitializationActionsFailed(int explanation){
	Q_UNUSED(explanation)
	qDebug() << "The actions list failed";
	setFailed();
}

void SGMXASDacqScanController::onInitializationActionsProgress(double elapsed, double total){
	Q_UNUSED(elapsed)
	Q_UNUSED(total)
	//qDebug() << "Initialization is " << elapsed/total << "% completed";
}

void SGMXASDacqScanController::onScanFinished(){
	qDebug() << "HEARD XAS SCAN FINISHED";
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(setFinished()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

void SGMXASDacqScanController::onScanCancelledBeforeInitialized(){
	qDebug() << "HEARD XAS SCAN CANCELLED BEFORE INITIALIZED";
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(onDacqStop()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

void SGMXASDacqScanController::onScanCancelledWhileRunning(){

}
