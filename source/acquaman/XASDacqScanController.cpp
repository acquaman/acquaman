#include "XASDacqScanController.h"

XASDacqScanController::XASDacqScanController(XASScanConfiguration *xasSCfg, QObject *parent) :
    DacqScanController(parent)
{
    initialized_ = FALSE;
    cfg_ = xasSCfg;
}

void XASDacqScanController::initialize(){
    SGMBeamline::sgm()->exitSlitGap()->move( ((XASScanConfiguration*)cfg_)->exitSlitGap() );
    SGMBeamline::sgm()->grating()->move( ((XASScanConfiguration*)cfg_)->grating() );
    SGMBeamline::sgm()->undulatorTracking()->move( ((XASScanConfiguration*)cfg_)->undulatorTracking() );
    SGMBeamline::sgm()->monoTracking()->move( ((XASScanConfiguration*)cfg_)->monoTracking() );
    SGMBeamline::sgm()->exitSlitTracking()->move( ((XASScanConfiguration*)cfg_)->exitSlitTracking() );
}
