#ifndef ACQMAN_SGMSCANCONFIG_H
#define ACQMAN_SGMSCANCONFIG_H

#include "beamline/SGMBeamline.h"

class SGMScanConfiguration
{
public:
    SGMScanConfiguration();

    double exitSlitGap() const { return exitSlitGap_;}
    SGMBeamline::sgmGrating grating() const { return grating_;}
    bool undulatorTracking() { return undulatorTracking_;}
    bool monoTracking() { return monoTracking_;}
    bool exitSlitTracking() { return exitSlitTracking_;}

    bool setExitSlitGap(double exitSlitGap);
    bool setGrating(SGMBeamline::sgmGrating grating) {grating_ = grating; return TRUE;}
    bool setUndulatorTracking(bool track){undulatorTracking_ = track; return TRUE;}
    bool setMonoTracking(bool track){monoTracking_ = track; return TRUE;}
    bool setExitSlitTracking(bool track){exitSlitTracking_ = track; return TRUE;}

protected:
    double exitSlitGap_;
    SGMBeamline::sgmGrating grating_;
    bool undulatorTracking_;
    bool monoTracking_;
    bool exitSlitTracking_;
};

#endif // ACQMAN_SGMSCANCONFIG_H
