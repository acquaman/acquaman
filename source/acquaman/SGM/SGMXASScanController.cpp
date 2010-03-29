#include "SGMXASScanController.h"

SGMXASScanController::SGMXASScanController(AMXASScanConfiguration *cfg){
    xasSCfg_ = cfg;
    beamlineInitialized_ = false;
}

bool SGMXASScanController::beamlineInitialize(){
    SGMBeamline::sgm()->exitSlitGap()->move( ((SGMXASScanConfiguration*)xasSCfg_)->exitSlitGap() );
    SGMBeamline::sgm()->grating()->move( ((SGMXASScanConfiguration*)xasSCfg_)->grating() );
    SGMBeamline::sgm()->undulatorTracking()->move( ((SGMXASScanConfiguration*)xasSCfg_)->undulatorTracking() );
    SGMBeamline::sgm()->monoTracking()->move( ((SGMXASScanConfiguration*)xasSCfg_)->monoTracking() );
    SGMBeamline::sgm()->exitSlitTracking()->move( ((SGMXASScanConfiguration*)xasSCfg_)->exitSlitTracking() );
    beamlineInitialized_ = true;
    return beamlineInitialized_;
}
