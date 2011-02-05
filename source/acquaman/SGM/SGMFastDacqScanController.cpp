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
	qDebug() << "Trying to load FAST SCAN";
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
	AMDacqScanController::start();
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
//		if(i.key() == 0 && (aeData.count()+aeSpectra.count()) > 1){
		if(i.key() == 0 && aeData.count() == 1 && aeSpectra.count() == 1){
			/*
			AMnDIndex insertIndex = toScanIndex(aeData);
			pScan()->rawData()->beginInsertRowsAsNecessaryForScanPoint(insertIndex);
			/// \bug CRITICAL: This is ASSUMING ONE AXIS, need to fix that somewhere
			pScan()->rawData()->setAxisValue(0, insertIndex.row(), i.value());
			++i;
			while(i != aeData.constEnd()){
				pScan()->rawData()->setValue(insertIndex, i.key()-1, AMnDIndex(), i.value());
				++i;
			}
			*/
			while(j != aeSpectra.constEnd()){
				int numPoints = j.value().at(0);
				for(int x = 0; x < j.value().count()-1; x++){
					if(x%4 == 0){
						pScan()->rawData()->beginInsertRows(0);
						pScan()->rawData()->setAxisValue(0, x/4, x/4);
						qDebug() << "NEW ROW";
					}
					qDebug() << "Column " << x%4 << " row " << x/4 << " value " << j.value().at(x+1);
					pScan()->rawData()->setValue(AMnDIndex(x/4), x%4, AMnDIndex(), j.value().at(x+1));
					if(x%4 == 3 || x == j.value().count()-2){
						pScan()->rawData()->endInsertRows();
						qDebug() << "ROW DONE";
					}
				}
				++j;
			}
			/*
			pScan()->rawData()->endInsertRows();
			*/
			qDebug() << "From data store";
			for(int x = 0; x < pScan()->rawData()->scanSize(0); x++){
				qDebug() << (double)(pScan()->rawData()->axisValue(0, x))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 0, AMnDIndex()))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 1, AMnDIndex()))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 2, AMnDIndex()))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 3, AMnDIndex()));
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

void SGMFastDacqScanController::onInitializationActionsSucceeded(){
	qDebug() << "Fast Scan: Initialization Actions Succeeded and emiting initialized";
	emit initialized();
}
