#include "AMXASDacqScanController.h"

AMXASDacqScanController::AMXASDacqScanController(AMXASScanConfiguration *xasSCfg, QObject *parent) :
    AMDacqScanController(parent)
{
    initialized_ = FALSE;
    cfg_ = xasSCfg;
}

void AMXASDacqScanController::initialize(){
    AMSGMBeamline::sgm()->exitSlitGap()->move( ((AMXASScanConfiguration*)cfg_)->exitSlitGap() );
    AMSGMBeamline::sgm()->grating()->move( ((AMXASScanConfiguration*)cfg_)->grating() );
    AMSGMBeamline::sgm()->undulatorTracking()->move( ((AMXASScanConfiguration*)cfg_)->undulatorTracking() );
    AMSGMBeamline::sgm()->monoTracking()->move( ((AMXASScanConfiguration*)cfg_)->monoTracking() );
    AMSGMBeamline::sgm()->exitSlitTracking()->move( ((AMXASScanConfiguration*)cfg_)->exitSlitTracking() );
}
