#include "SGMXASScanController.h"

SGMXASScanController::SGMXASScanController(AMXASScanConfiguration *cfg){
    xasSCfg_ = cfg;
    beamlineInitialized_ = false;
}

bool SGMXASScanController::beamlineInitialize(){
    SGMBeamline::sgm()->exitSlitGap()->move( ((AMXASScanConfiguration*)xasSCfg_)->exitSlitGap() );
    SGMBeamline::sgm()->grating()->move( ((AMXASScanConfiguration*)xasSCfg_)->grating() );
    SGMBeamline::sgm()->undulatorTracking()->move( ((AMXASScanConfiguration*)xasSCfg_)->undulatorTracking() );
    SGMBeamline::sgm()->monoTracking()->move( ((AMXASScanConfiguration*)xasSCfg_)->monoTracking() );
    SGMBeamline::sgm()->exitSlitTracking()->move( ((AMXASScanConfiguration*)xasSCfg_)->exitSlitTracking() );
    beamlineInitialized_ = true;
    return beamlineInitialized_;
}
