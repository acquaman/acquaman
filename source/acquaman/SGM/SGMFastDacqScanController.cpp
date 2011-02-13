#include "SGMFastDacqScanController.h"

SGMFastDacqScanController::SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent), SGMFastScanController(cfg)
{
	lastProgress_ = 0.0;
	initializationStagesComplete_ = 0;
	timerSeconds_ = 0;
	dacqRunUpStarted_ = false;
	dacqRunUpCompleted_ = false;
	dacqRunCompleted_ = false;
	emit scanCreated(scan());
}

void SGMFastDacqScanController::initialize(){
	if(SGMFastScanController::beamlineInitialize() && initializationActions_){
		initialized_ = true;
		#warning "Do we need to also clear any raw data sources here, or just the raw data itself?"
		pScan()->clearRawDataPoints();
		connect(initializationActions_, SIGNAL(listSucceeded()), this, SLOT(onInitializationActionsSucceeded()));
		//connect(initializationActions_, SIGNAL(stageStarted(int)), this, SLOT(onInitializationActionsStageStarted(int)));
		connect(initializationActions_, SIGNAL(stageSucceeded(int)), this, SLOT(onInitializationActionsStageSucceeded(int)));
		connect(initializationActions_, SIGNAL(stageProgress(double,double)), this, SLOT(calculateProgress(double,double)));
		initializationActions_->start();
		/*
		emit initialized();
		*/
	}
}

void SGMFastDacqScanController::reinitialize(bool removeScan){
	lastProgress_ = 0.0;
	initializationStagesComplete_ = 0;
	timerSeconds_ = 0;
	dacqRunUpStarted_ = false;
	dacqRunUpCompleted_ = false;
	dacqRunCompleted_ = false;
	SGMFastScanController::reinitialize();
	qDebug() << "Emitting reinitialized with removeScan " << removeScan;
	emit reinitialized(removeScan);
}

void SGMFastDacqScanController::start(){
	if(SGMBeamline::sgm()->isScanning()){
		qDebug() << "Beamline already scanning";
		return;
	}
	bool loadSuccess;
	QString homeDir = QDir::homePath();
	if( QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if( QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	/*
	if(pCfg()->usingPGT())
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/pgt.cfg"));
	else
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/defaultEnergy.cfg"));
	*/
	loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/Scalar_Fast.config"));
	if(!loadSuccess){
		qDebug() << "LIBRARY FAILED TO LOAD CONFIG FILE";
		return;
	}
	advAcq_->setStart(0, pCfg()->start());
	advAcq_->setDelta(0, pCfg()->end()-pCfg()->start());
	advAcq_->setEnd(0, pCfg()->end());
	/*
	foreach(const AMDetectorInfo *dtctr, pCfg()->usingDetectors() ){
		if(dtctr->name() == SGMBeamline::sgm()->pgtDetector()->name()){
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, true, 0);
		}
		else{
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, false, 0);
		}

		//for(int x = 0; x < pCfg()->count(); x++){
		//	if(advAcq_->getNumRegions() == x)
		//		advAcq_->addRegion(x, pCfg_()->start(x), pCfg_()->delta(x), pCfg_()->end(x), 1);
		//	else{
		//		advAcq_->setStart(x, pCfg_()->start(x));
		//		advAcq_->setDelta(x, pCfg_()->delta(x));
		//		advAcq_->setEnd(x, pCfg_()->end(x));
		//	}
		//}

	}
	*/
	generalScan_ = specificScan_;
	usingSpectraDotDatFile_ = true;
	fastScanTimer_ = new QTimer(this);
	connect(fastScanTimer_, SIGNAL(timeout()), this, SLOT(onFastScanTimerTimeout()));
	//if(!autoSavePath_.isEmpty())
	if(!pCfg()->sensibleFileSavePath().isEmpty())
		pScan()->setAutoExportFilePath(pCfg()->finalizedSavePath());
	AMDacqScanController::start();
}

void SGMFastDacqScanController::cancel(){
	if(initializationActions_ && initializationActions_->isRunning())
		qDebug() << "Need to stop the intialization actions";
	AMDacqScanController::cancel();
}

bool SGMFastDacqScanController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AM::AcqEvent){
		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();
		// Fast scan should be one scalar value (the initial energy value) and one spectral value (the scaler with all the data)
		// There will be N*1000 elements of the scaler waveform, where N is the number of channels (detectors) being acquired
		// We have already set the energy axis as uniform with the proper start and increment, so we can ignore the energy value in aeData
		if(i.key() == 0 && aeData.count() == 1 && aeSpectra.count() == 1){
			qDebug() << "And doing something with it";
			while(j != aeSpectra.constEnd()){
				int maxVal = j.value().count()-1;
				if(maxVal > 4000)
					maxVal = 4000;
				//for(int x = 0; x < j.value().count()-1; x++){
				//for(int x = 0; x < 4000; x++){
				for(int x = 0; x < maxVal; x++){
					if(x%4 == 0){
						pScan()->rawData()->beginInsertRows(0);
						pScan()->rawData()->setAxisValue(0, x/4, x/4);
					}
					pScan()->rawData()->setValue(AMnDIndex(x/4), x%4, AMnDIndex(), j.value().at(x+1));
					if(x%4 == 3 || x == j.value().count()-2)
						pScan()->rawData()->endInsertRows();
				}
				++j;
			}
		}
		e->accept();
		return true;
	}
	else
		return AMDacqScanController::event(e);
}

AMnDIndex SGMFastDacqScanController::toScanIndex(QMap<int, double> aeData){
	// SGM XAS Scan has only one dimension (energy), simply append to the end of this
	return AMnDIndex(pScan()->rawData()->scanSize(0));
}
void SGMFastDacqScanController::onStop(){
	running_ = FALSE;
	if(cancelled_)
		cancelled_ = FALSE;
	else
		onScanFinished();
}

void SGMFastDacqScanController::onSendCompletion(int completion){
	//calculateProgress(completion, 100.0);
}

void SGMFastDacqScanController::onState(const QString &state){
	if(state == "Runup")
		dacqRunUpStarted_ = true;
	if(state == "Run" && dacqRunUpStarted_){
		dacqRunUpCompleted_ = true;
		fastScanTimer_->start(500);
	}
	if(state == "Off"){
		dacqRunCompleted_ = true;
		if(fastScanTimer_->isActive())
			fastScanTimer_->stop();
	}
	calculateProgress(-1, -1);
}

void SGMFastDacqScanController::onInitializationActionsSucceeded(){
	//qDebug() << "Initialization actions done for fast scan";
	emit initialized();
}

void SGMFastDacqScanController::onInitializationActionsStageSucceeded(int stageIndex){
	qDebug() << "Initialization stage " << stageIndex << " succeeded";
	initializationStagesComplete_ = stageIndex+1;
	calculateProgress(-1, -1);
}

void SGMFastDacqScanController::onFastScanTimerTimeout(){
	//timerSeconds_++;
	//calculateProgress(timerSeconds_, pCfg()->runTime());
	calculateProgress(SGMBeamline::sgm()->energy()->value()-pCfg()->start(), pCfg()->end()-pCfg()->start());
	if( fabs(SGMBeamline::sgm()->energy()->value()-pCfg()->end()) <  SGMBeamline::sgm()->energy()->tolerance())
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
	qDebug() << QString("%1 (%2)").arg(starProgress).arg(lastProgress_);
}

void SGMFastDacqScanController::onScanFinished(){
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SIGNAL(finished()));
		cleanUpActions_->start();
	}
	else
		emit finished();
}
