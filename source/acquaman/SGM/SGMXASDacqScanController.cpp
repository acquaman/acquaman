#include "SGMXASDacqScanController.h"

SGMXASDacqScanController::SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent) , SGMXASScanController(cfg)
{
	_pCfg_ = &specificCfg_;
	_pScan_ = &specificScan_;
	emit scanCreated(scan());
}

void SGMXASDacqScanController::initialize(){
	if(SGMXASScanController::beamlineInitialize())
		initialized_ = true;
	pScan_()->clear();
	emit initialized();
}

void SGMXASDacqScanController::reinitialize(bool removeScan){
	SGMXASScanController::reinitialize();
	qDebug() << "Emitting reinitialized with removeScan " << removeScan;
	emit reinitialized(removeScan);
}

void SGMXASDacqScanController::start(){
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
	if(pScan_()->detectorNames().contains("pgt"))
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/pgt.cfg"));
	else
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/defaultEnergy.cfg"));
	if(!loadSuccess){
		qDebug() << "LIBRARY FAILED TO LOAD CONFIG FILE";
		return;
	}

	foreach(const AMDetectorInfo *dtctr, pScan_()->detectors()){
		if(dtctr->name() == SGMBeamline::sgm()->pgtDetector()->name()){
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, true, 0);
		}
		else{
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, false, 0);
		}
		for(int x = 0; x < pCfg_()->count(); x++){
			if(advAcq_->getNumRegions() == x)
				advAcq_->addRegion(x, pCfg_()->start(x), pCfg_()->delta(x), pCfg_()->end(x), 1);
			else{
				advAcq_->setStart(x, pCfg_()->start(x));
				advAcq_->setDelta(x, pCfg_()->delta(x));
				advAcq_->setEnd(x, pCfg_()->end(x));
			}
		}
	}
	generalScan_ = specificScan_;
	AMDacqScanController::start();
}

/*
bool SGMXASDacqScanController::event(QEvent *e){
	if(e->type() == QEvent::MaxUser){
		//qDebug() << "Data package in scan controller is " << ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		qDebug() << "Data package in scan controller is " << aeData;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		qDebug() << i.key() << aeData.count();
		if(i.key() == 0 && aeData.count() > 1){
			pScan_()->d_->append(i.value());
			++i;
			while(i != aeData.constEnd()){
				qDebug() << "Trying to set " << i.key()-1 << " as " << i.value();
				pScan_()->d_->setLastValue(i.key()-1, i.value());
				++i;
			}
			qDebug() << "After " << pScan_()->d_->x(pScan_()->d_->count()-1);
			for(int x = 1; x < aeData.count(); x++)
				qDebug() << "After val " << pScan_()->d_->value(x-1, pScan_()->d_->count()-1);
		}
		e->accept();
	}
	else
		e->ignore();
}
*/
