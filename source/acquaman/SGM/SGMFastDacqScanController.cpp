#include "SGMFastDacqScanController.h"

SGMFastDacqScanController::SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent), SGMFastScanController(cfg)
{
	emit scanCreated(scan());
}

void SGMFastDacqScanController::initialize(){
	if(SGMFastScanController::beamlineInitialize() && initializationActions_){
		initialized_ = true;
		#warning "Do we need to also clear any raw data sources here, or just the raw data itself?"
		pScan()->clearRawDataPoints();
		connect(initializationActions_, SIGNAL(listSucceeded()), this, SLOT(onInitializationActionsSucceeded()));
		connect(initializationActions_, SIGNAL(stageStarted(int)), this, SLOT(onInitializationActionsStageStarted(int)));
		connect(initializationActions_, SIGNAL(stageSucceeded(int)), this, SLOT(onInitializationActionsStageSucceeded(int)));
		connect(initializationActions_, SIGNAL(stageProgress(double,double)), this, SLOT(onInitializationActionsStageProgress(double,double)));
		initializationActions_->start();
		/*
		emit initialized();
		*/
	}
}

void SGMFastDacqScanController::reinitialize(bool removeScan){
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
	//if(!autoSavePath_.isEmpty())
	if(!pCfg()->sensibleFileSavePath().isEmpty())
		pScan()->setAutoExportFilePath(pCfg()->finalizedSavePath());
	AMDacqScanController::start();
}

bool SGMFastDacqScanController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AM::AcqEvent){
		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();
		qDebug() << "Received fast scan data event " << i.key() << aeData.count() << aeSpectra.count();
		// Fast scan should be one scalar value (the initial energy value) and one spectral value (the scaler with all the data)
		// There will be N*1000 elements of the scaler waveform, where N is the number of channels (detectors) being acquired
		// We have already set the energy axis as uniform with the proper start and increment, so we can ignore the energy value in aeData
		if(i.key() == 0 && aeData.count() == 1 && aeSpectra.count() == 1){
			qDebug() << "And doing something with it";
			while(j != aeSpectra.constEnd()){
				for(int x = 0; x < j.value().count()-1; x++){
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
		qDebug() << "But not doing anything with it";
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
	qDebug() << "Dacq stage " << advAcq_->getStatus() << " is " << completion << " complete";
}

void SGMFastDacqScanController::onInitializationActionsSucceeded(){
	qDebug() << "Initialization actions done for fast scan";
	SGMBeamline::sgm()->scalerTotalNumberOfScans()->move(1000);
	emit initialized();
}

void SGMFastDacqScanController::onInitializationActionsStageStarted(int stageIndex){
	qDebug() << "Initialization stage " << stageIndex << " started";
}

void SGMFastDacqScanController::onInitializationActionsStageSucceeded(int stageIndex){
	qDebug() << "Initialization stage " << stageIndex << " succeeded";
}

void SGMFastDacqScanController::onInitializationActionsStageProgress(double elapsed, double total){
	qDebug() << "Stage is " << (elapsed/total)*100 << " percent done";
}

void SGMFastDacqScanController::onScanFinished(){
	if(cleanUpActions_){
		connect(cleanUpActions_, SIGNAL(listSucceeded()), this, SIGNAL(finished()));
		cleanUpActions_->start();
	}
	else
		emit finished();
}
