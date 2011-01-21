/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#warning "Do we need to also clear any raw data sources here, or just the raw data itself?"
	pScan_()->clearRawDataPoints();
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

	if(pCfg_()->usingPGT())
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/pgt.cfg"));
	else
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/defaultEnergy.cfg"));
	if(!loadSuccess){
		qDebug() << "LIBRARY FAILED TO LOAD CONFIG FILE";
		return;
	}
	foreach(const AMDetectorInfo *dtctr, pCfg_()->usingDetectors() ){
		if(dtctr->name() == SGMBeamline::sgm()->pgtDetector()->name()){
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, true, 0);
		}
		else{
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, false, 0);
		}
		for(int x = 0; x < pCfg_()->regionCount(); x++){
			if(advAcq_->getNumRegions() == x)
				advAcq_->addRegion(x, pCfg_()->regionStart(x), pCfg_()->regionDelta(x), pCfg_()->regionEnd(x), 1);
			else{
				advAcq_->setStart(x, pCfg_()->regionStart(x));
				advAcq_->setDelta(x, pCfg_()->regionDelta(x));
				advAcq_->setEnd(x, pCfg_()->regionEnd(x));
			}
		}
	}
	generalScan_ = specificScan_;
	AMDacqScanController::start();
}

AMnDIndex SGMXASDacqScanController::toScanIndex(QMap<int, double> aeData){
	// SGM XAS Scan has only one dimension (energy), simply append to the end of this
	return AMnDIndex(pScan_()->rawData()->scanSize(0));
}
