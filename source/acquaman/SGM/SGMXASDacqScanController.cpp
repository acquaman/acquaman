#include "SGMXASDacqScanController.h"

SGMXASDacqScanController::SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent) , SGMXASScanController(cfg)
{
	pCfg_ = cfg;
}

void SGMXASDacqScanController::initialize(){
	if(SGMXASScanController::beamlineInitialize())
		initialized_ = true;
	emit initialized();
}
