#include "SGMXASDacqScanController.h"

SGMXASDacqScanController::SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent) , SGMXASScanController(cfg)
{
	_pCfg_ = &specificCfg_;
	_pScan_ = &specificScan_;
}

void SGMXASDacqScanController::initialize(){
	if(SGMXASScanController::beamlineInitialize())
		initialized_ = true;
	emit initialized();
}
