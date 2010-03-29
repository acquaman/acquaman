#include "SGMScanConfiguration.h"

SGMScanConfiguration::SGMScanConfiguration()
{
    setExitSlitGap(SGMBeamline::sgm()->exitSlitGap()->value());
    setGrating((SGMBeamline::sgm()->grating()->value() < 1) ? SGMBeamline::lowGrating : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::mediumGrating : SGMBeamline::highGrating));
    setUndulatorTracking(SGMBeamline::sgm()->undulatorTracking()->value());
    setMonoTracking(SGMBeamline::sgm()->monoTracking()->value());
    setExitSlitTracking(SGMBeamline::sgm()->exitSlitTracking()->value());
}

bool SGMScanConfiguration::setExitSlitGap(double exitSlitGap){
    if(SGMBeamline::sgm()->exitSlitGap()->valueOutOfRange(exitSlitGap))
        return FALSE;
    exitSlitGap_ = exitSlitGap;
    return TRUE;
}
