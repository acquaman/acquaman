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


#include "SGMFastDacqScanController.h"
#include "util/SGM/SGMDacqConfigurationFile.h"
#include "dataman/database/AMDbObjectSupport.h"
#include <QDir>

SGMFastDacqScanController::SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent) :
	AMDacqScanController(cfg, parent), SGMFastScanController(cfg)
{
	scan_ = specificScan_;	//MB: Moved from startImplementation(). The specificScan_ is new'd in the SGMFastScanController constructor.
	lastProgress_ = 0.0;
	initializationStagesComplete_ = 0;
	timerSeconds_ = 0;
	stopMotorsAction_ = 0; //NULL
	fastScanTimer_ = 0; //NULL
	dacqRunUpStarted_ = false;
	dacqRunUpCompleted_ = false;
	dacqRunCompleted_ = false;
}

void SGMFastDacqScanController::onDacqStop(){
	if(dacqCancelled_)
		AMDacqScanController::onDacqStop();
	else
		onScanFinished();
}

void SGMFastDacqScanController::onDacqSendCompletion(int completion){
	Q_UNUSED(completion)
	//calculateProgress(completion, 100.0);
}

void SGMFastDacqScanController::onDacqState(const QString &state){
	if(state == "Runup")
		dacqRunUpStarted_ = true;
	if(state == "Run" && dacqRunUpStarted_){
		dacqRunUpCompleted_ = true;
		fastScanTimer_->start(500);
	}
	if(state == "Off"){
		dacqRunCompleted_ = true;
		if(fastScanTimer_ && fastScanTimer_->isActive())
			fastScanTimer_->stop();
	}
	calculateProgress(-1, -1);
}

void SGMFastDacqScanController::onInitializationActionsSucceeded(){
	setInitialized();
}

void SGMFastDacqScanController::onInitializationActionsStageSucceeded(int stageIndex){
	initializationStagesComplete_ = stageIndex+1;
	calculateProgress(-1, -1);
}

void SGMFastDacqScanController::onInitializationActionsFailed(int explanation){
	Q_UNUSED(explanation)
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(setCancelled()));
		cleanUpActions_->start();
	}
	else
		setCancelled();
}

void SGMFastDacqScanController::onFastScanTimerTimeout(){
	calculateProgress(SGMBeamline::sgm()->energy()->value()-config_->startEnergy(), config_->endEnergy()-config_->startEnergy());
	if( fabs(SGMBeamline::sgm()->energy()->value()-config_->endEnergy()) <  SGMBeamline::sgm()->energy()->tolerance())
		fastScanTimer_->stop();
}

void SGMFastDacqScanController::calculateProgress(double elapsed, double total){
	double localProgress = 0;
	// I'm guessing the initialization actions are 3/8, the dacq initialization is 1/4, and the dacq run is 3/8
	if(!dacqRunCompleted_){
		// Take the fraction of completed initialization stages and multiply by 3/8
		localProgress += (3.0/8.0)*((double)initializationStagesComplete_/((double)initializationActions_->stageCount()));
		// If one of the stages is running, add its fraction
		if( (initializationStagesComplete_ != initializationActions_->stageCount()) && (elapsed != -1) &&(total != -1) )
			localProgress += (elapsed/total)*(3.0/8.0)*(1.0/(double)initializationActions_->stageCount());
		// If the run up is started but not finished, add that fraction
		if(dacqRunUpStarted_ && !dacqRunUpCompleted_ && (elapsed != -1) &&(total != -1) )
			localProgress += (elapsed/total)*(1.0/4.0);
		// If the run up is completed, add the 1/4 and whatever fraction of the scan run is completed
		if(dacqRunUpCompleted_ && (elapsed != -1) && (total != -1) )
			localProgress += (elapsed/total)*(3.0/8.0) + (1.0/4.0);
	}
	else
		localProgress = 1.0;

	if(localProgress > lastProgress_)
		lastProgress_ = localProgress;

	emit progress(lastProgress_*100, 100.0);

	int stars = (lastProgress_*100);
	QString starProgress = "";
	for(int x = 0; x < 100; x++){
		if(x <= stars)
			starProgress.append("*");
		else
			starProgress.append("-");
	}
}

void SGMFastDacqScanController::onScanFinished(){
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(setFinished()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

void SGMFastDacqScanController::onScanCancelledBeforeInitialized(){
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(onDacqStop()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

void SGMFastDacqScanController::onScanCancelledWhileRunning(){
	delete stopMotorsAction_;
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SLOT(onDacqStop()));
		cleanUpActions_->start();
	}
	else
		AMDacqScanController::onDacqStop();
}

bool SGMFastDacqScanController::initializeImplementation(){
	if(SGMFastScanController::beamlineInitialize() && initializationActions_){
		/* NTBA - August 25th, 2011 (David Chevrier)
   Do we need to also clear any raw data sources here, or just the raw data itself?"
  */
		scan_->clearRawDataPoints();
		connect(initializationActions_, SIGNAL(listSucceeded()), this, SLOT(onInitializationActionsSucceeded()));
		connect(initializationActions_, SIGNAL(stageSucceeded(int)), this, SLOT(onInitializationActionsStageSucceeded(int)));
		connect(initializationActions_, SIGNAL(listFailed(int)), this, SLOT(onInitializationActionsFailed(int)));
		connect(initializationActions_, SIGNAL(stageProgress(double,double)), this, SLOT(calculateProgress(double,double)));
		initializationActions_->start();
		return true;
	}
	AMErrorMon::report(AMErrorReport(this,
					 AMErrorReport::Alert,
					 SGMFASTDACQSCANCONTROLLER_CANT_INTIALIZE,
					 "Error, SGM Fast DACQ Scan Controller failed to initialize. Please report this bug to the Acquaman developers."));
	return false;
}

bool SGMFastDacqScanController::startImplementation(){
	if(SGMBeamline::sgm()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
						 AMErrorReport::Alert,
						 SGMFASTDACQSCANCONTROLLER_CANT_INTIALIZE,
						 "Error, SGM Fast DACQ Scan Controller failed to start (SGM is already scanning). Please report this bug to the Acquaman developers."));
		return false;
	}

	bool loadSuccess = false;

	SGMDacqConfigurationFile *configFile = new SGMDacqConfigurationFile();
	QList<int> matchIDs = AMDatabase::database("SGMBeamline")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMDacqConfigurationFile>(), "name", "FastScaler");
	if(matchIDs.count() > 0){
		configFile->loadFromDb(AMDatabase::database("SGMBeamline"), matchIDs.at(0));
		loadSuccess = advAcq_->setConfigFile(configFile->configurationFileFullPath());
	}

	if(!loadSuccess){
		AMErrorMon::report(AMErrorReport(this,
						 AMErrorReport::Alert,
						 SGMFASTDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE,
						 "Error, SGM Fast DACQ Scan Controller failed to start (the config file failed to load). Please report this bug to the Acquaman developers."));
		return false;
	}

	advAcq_->setStart(0, config_->startEnergy());
	advAcq_->setDelta(0, config_->endEnergy()-config_->startEnergy());
	advAcq_->setEnd(0, config_->endEnergy());

	usingSpectraDotDatFile_ = true;
	fastScanTimer_ = new QTimer(this);
	connect(fastScanTimer_, SIGNAL(timeout()), this, SLOT(onFastScanTimerTimeout()));
	advAcq_->saveConfigFile("/Users/fawkes/dev/acquaman/devConfigurationFiles/davidTest.cfg");

	return AMDacqScanController::startImplementation();
}

void SGMFastDacqScanController::cancelImplementation(){
	dacqCancelled_ = true;
	if(initializationActions_ && initializationActions_->isRunning()){
		disconnect(initializationActions_, 0);
		connect(initializationActions_, SIGNAL(listSucceeded()), this, SLOT(onScanCancelledBeforeInitialized()));
		connect(initializationActions_, SIGNAL(listFailed(int)), this, SLOT(onScanCancelledBeforeInitialized()));
	}
	else{
		AMDacqScanController::cancelImplementation();
		stopMotorsAction_ = SGMBeamline::sgm()->createStopMotorsAction();
		connect(stopMotorsAction_, SIGNAL(finished()), this, SLOT(onScanCancelledWhileRunning()));
		stopMotorsAction_->start();
	}
}

bool SGMFastDacqScanController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AM::AcqEvent){
		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
		QMap<int, double> aeExtras = ((AMAcqEvent*)e)->extraPackage_;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();
		QMap<int, double>::const_iterator k = aeExtras.constBegin();
		// Fast scan should be one scalar value (the initial energy value) and one spectral value (the scaler with all the data)
		// There will be N*1000 elements of the scaler waveform, where N is the number of channels (detectors) being acquire
		int encoderEndpoint = 0;
		int encoderStartPoint = 0;
		int encoderReading = 0;
		double energyFbk = 0.0;
		double spacingParam = SGMBeamline::sgm()->energySpacingParam()->value();
		double c1Param = SGMBeamline::sgm()->energyC1Param()->value();
		double c2Param = SGMBeamline::sgm()->energyC2Param()->value();
		double sParam = SGMBeamline::sgm()->energySParam()->value();
		double thetaParam = SGMBeamline::sgm()->energyThetaParam()->value();
		QList<double> readings;
		if(i.key() == 0 && aeData.count() == 2 && aeSpectra.count() == 1){
//			qdebug() << "And doing something with it";
			++i;

			encoderEndpoint = i.value();
//			qdebug() << "Encoder endpoint was " << encoderEndpoint;
			QString readingStr;
			if(k.key() == 2 && aeExtras.count() == 1){
//				qdebug() << "Using encoder start from event";
				encoderStartPoint = k.value();
			}
			else{
//				qdebug() << "Have to back calculate encoder start";
				while(j != aeSpectra.constEnd()){
					encoderStartPoint = encoderEndpoint;
					int maxVal = j.value().count()-1;
					if(maxVal > 6000)
						maxVal = 6000;
					for(int x = 0; x < maxVal; x++){
						if( x%6 == 4 )
							encoderStartPoint += j.value().at(x+1);
						if( x%6 == 5 )
							encoderStartPoint -= j.value().at(x+1);
						readingStr.append(QString("%1 ").arg(j.value().at(x+1)));
						if( x%6 == 5 ){
							readingStr.clear();
						}
					}
					++j;
				}
			}
			encoderReading = encoderStartPoint;
//			qdebug() << "\n\nEnoder start was " << encoderStartPoint;// << " avg up is " << avgUp << " down is " << avgDown << " thresholds " << upMax << downMax;
			j = aeSpectra.constBegin();
			while(j != aeSpectra.constEnd()){
				int maxVal = j.value().count()-1;
				//changed from 6000 to 10000, with 4 new diode channels
				if(maxVal > 10000)
					maxVal = 10000;
				for(int x = 0; x < maxVal; x++){
					if(x%10 == 0)
						readings.clear();
					if( x%10 == 0 || x%10 == 1 || x%10 == 2 || x%10 == 3 || x%10 == 6 || x%10 == 7 || x%10 == 8 || x%10 == 9){
						//if( x%6 == 0 || x%6 == 1 || x%6 == 4 || x%6 == 5 ){
						if(j.value().at(x+1) == 0)
							readings.append(1);
						else
							readings.append(j.value().at(x+1));
					}
					if( x%10 == 4 ){
						encoderReading -= j.value().at(x+1);
						//encoderReading += j.value().at(x+1);
					}
					if( x%10 == 5 ){
						encoderReading += j.value().at(x+1);
						//encoderReading -= j.value().at(x+1);
					}
					if( x%10 == 9 ){
						energyFbk = (1.0e-9*1239.842*sParam)/(2*spacingParam*c1Param*c2Param*(double)encoderReading*cos(thetaParam/2));
						//if( ( (readings.at(0) > config_->baseLine()) && (scan_->rawData()->scanSize(0) == 0) ) || ( (scan_->rawData()->scanSize(0) > 0) && (fabs(energyFbk - (double)scan_->rawData()->axisValue(0, scan_->rawData()->scanSize(0)-1)) > 0.001) ) ){
						scan_->rawData()->beginInsertRows(1, -1);
						scan_->rawData()->setAxisValue(0, scan_->rawData()->scanSize(0)-1, energyFbk);
						//scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 0, AMnDIndex(), readings.at(0));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 0, AMnDIndex(), max(readings.at(0), 1.0));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 1, AMnDIndex(), readings.at(1));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 2, AMnDIndex(), readings.at(2));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 3, AMnDIndex(), readings.at(3));

						//adding new diodes
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 4, AMnDIndex(), readings.at(4));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 5, AMnDIndex(), readings.at(5));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 6, AMnDIndex(), readings.at(6));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 7, AMnDIndex(), readings.at(7));

						scan_->rawData()->endInsertRows();
						//}
					}
				}
				++j;
			}
		}
		e->accept();
		return true;
	}
	else
		return AMDacqScanController::event(e);

	/*
	if(e->type() == (QEvent::Type)AM::AcqEvent){
		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
		QMap<int, double> aeExtras = ((AMAcqEvent*)e)->extraPackage_;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();
		QMap<int, double>::const_iterator k = aeExtras.constBegin();
		// Fast scan should be one scalar value (the initial energy value) and one spectral value (the scaler with all the data)
		// There will be N*1000 elements of the scaler waveform, where N is the number of channels (detectors) being acquire
		int encoderEndpoint = 0;
		int encoderStartPoint = 0;
		int encoderReading = 0;
		double energyFbk = 0.0;
		double spacingParam = SGMBeamline::sgm()->energySpacingParam()->value();
		double c1Param = SGMBeamline::sgm()->energyC1Param()->value();
		double c2Param = SGMBeamline::sgm()->energyC2Param()->value();
		double sParam = SGMBeamline::sgm()->energySParam()->value();
		double thetaParam = SGMBeamline::sgm()->energyThetaParam()->value();
		QList<double> readings;
		if(i.key() == 0 && aeData.count() == 2 && aeSpectra.count() == 1){
//			qdebug() << "And doing something with it";
			++i;

			encoderEndpoint = i.value();
//			qdebug() << "Encoder endpoint was " << encoderEndpoint;
			QString readingStr;
			if(k.key() == 2 && aeExtras.count() == 1){
//				qdebug() << "Using encoder start from event";
				encoderStartPoint = k.value();
			}
			else{
//				qdebug() << "Have to back calculate encoder start";
				while(j != aeSpectra.constEnd()){
					encoderStartPoint = encoderEndpoint;
					int maxVal = j.value().count()-1;
					if(maxVal > 6000)
						maxVal = 6000;
					for(int x = 0; x < maxVal; x++){
						if( x%6 == 4 )
							encoderStartPoint += j.value().at(x+1);
						if( x%6 == 5 )
							encoderStartPoint -= j.value().at(x+1);
						readingStr.append(QString("%1 ").arg(j.value().at(x+1)));
						if( x%6 == 5 ){
							readingStr.clear();
						}
					}
					++j;
				}
			}
			encoderReading = encoderStartPoint;
//			qdebug() << "\n\nEnoder start was " << encoderStartPoint;// << " avg up is " << avgUp << " down is " << avgDown << " thresholds " << upMax << downMax;
			j = aeSpectra.constBegin();
			while(j != aeSpectra.constEnd()){
				int maxVal = j.value().count()-1;
				if(maxVal > 6000)
					maxVal = 6000;
				for(int x = 0; x < maxVal; x++){
					if(x%6 == 0)
						readings.clear();
					if( x%6 == 0 || x%6 == 1 || x%6 == 2 || x%6 == 3 ){
						//if( x%6 == 0 || x%6 == 1 || x%6 == 4 || x%6 == 5 ){
						if(j.value().at(x+1) == 0)
							readings.append(1);
						else
							readings.append(j.value().at(x+1));
					}
					if( x%6 == 4 ){
						encoderReading -= j.value().at(x+1);
						//encoderReading += j.value().at(x+1);
					}
					if( x%6 == 5 ){
						encoderReading += j.value().at(x+1);
						//encoderReading -= j.value().at(x+1);
					}
					if( x%6 == 5 ){
						energyFbk = (1.0e-9*1239.842*sParam)/(2*spacingParam*c1Param*c2Param*(double)encoderReading*cos(thetaParam/2));
						//if( ( (readings.at(0) > config_->baseLine()) && (scan_->rawData()->scanSize(0) == 0) ) || ( (scan_->rawData()->scanSize(0) > 0) && (fabs(energyFbk - (double)scan_->rawData()->axisValue(0, scan_->rawData()->scanSize(0)-1)) > 0.001) ) ){
						scan_->rawData()->beginInsertRows(1, -1);
						scan_->rawData()->setAxisValue(0, scan_->rawData()->scanSize(0)-1, energyFbk);
						//scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 0, AMnDIndex(), readings.at(0));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 0, AMnDIndex(), max(readings.at(0), 1.0));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 1, AMnDIndex(), readings.at(1));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 2, AMnDIndex(), readings.at(2));
						scan_->rawData()->setValue(AMnDIndex(scan_->rawData()->scanSize(0)-1), 3, AMnDIndex(), readings.at(3));
						scan_->rawData()->endInsertRows();
						//}
					}
				}
				++j;
			}
		}
		e->accept();
		return true;
	}
	else
		return AMDacqScanController::event(e);
	*/
}

AMnDIndex SGMFastDacqScanController::toScanIndex(QMap<int, double> aeData){
	Q_UNUSED(aeData)
	// SGM XAS Scan has only one dimension (energy), simply append to the end of this
	return AMnDIndex(scan_->rawData()->scanSize(0));
}
