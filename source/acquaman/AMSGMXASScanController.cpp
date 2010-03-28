#include "AMSGMXASScanController.h"

AMSGMXASScanController::AMSGMXASScanController(AMXASScanConfiguration *cfg){
    xasSCfg_ = cfg;
    beamlineInitialized_ = false;
}

bool AMSGMXASScanController::beamlineInitialize(){
    AMSGMBeamline::sgm()->exitSlitGap()->move( ((AMXASScanConfiguration*)xasSCfg_)->exitSlitGap() );
    AMSGMBeamline::sgm()->grating()->move( ((AMXASScanConfiguration*)xasSCfg_)->grating() );
    AMSGMBeamline::sgm()->undulatorTracking()->move( ((AMXASScanConfiguration*)xasSCfg_)->undulatorTracking() );
    AMSGMBeamline::sgm()->monoTracking()->move( ((AMXASScanConfiguration*)xasSCfg_)->monoTracking() );
    AMSGMBeamline::sgm()->exitSlitTracking()->move( ((AMXASScanConfiguration*)xasSCfg_)->exitSlitTracking() );
    beamlineInitialized_ = true;
    return beamlineInitialized_;
}
