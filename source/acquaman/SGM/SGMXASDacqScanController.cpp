#include "SGMXASDacqScanController.h"

SGMXASDacqScanController::SGMXASDacqScanController(AMXASScanConfiguration *xasSCfg, QObject *parent) :
        AMDacqScanController(parent) , SGMXASScanController(xasSCfg)
{
}

void SGMXASDacqScanController::initialize(){
    if(SGMXASScanController::beamlineInitialize())
        initialized_ = true;
    emit initialized();
}
