#ifndef ACQMAN_SGMSCANCONFIG_H
#define ACQMAN_SGMSCANCONFIG_H

#include "beamline/SGMBeamline.h"

class SGMScanConfiguration
{
public:
	SGMScanConfiguration();

	double exitSlitGap() const { return exitSlitGap_;}
	SGMBeamline::sgmGrating grating() const { return grating_;}
	SGMBeamline::sgmHarmonic harmonic() const { return harmonic_;}
	bool undulatorTracking() { return undulatorTracking_;}
	bool monoTracking() { return monoTracking_;}
	bool exitSlitTracking() { return exitSlitTracking_;}

	bool setExitSlitGap(double exitSlitGap);
	bool setGrating(SGMBeamline::sgmGrating grating) {grating_ = grating; return true;}
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic) { qDebug() << "Harmonic value is " << harmonic; harmonic_ = harmonic; return true;}
	bool setUndulatorTracking(bool track){undulatorTracking_ = track; return true;}
	bool setMonoTracking(bool track){monoTracking_ = track; return true;}
	bool setExitSlitTracking(bool track){exitSlitTracking_ = track; return true;}
	bool setUsingTEY(bool active) { usingTEY_ = active; return true;}
	bool setUsingTFY(bool active) { usingTFY_ = active; return true;}
	bool setUsingPGT(bool active) { usingPGT_ = active; return true;}

protected:
	double exitSlitGap_;
	SGMBeamline::sgmGrating grating_;
	SGMBeamline::sgmHarmonic harmonic_;
	bool undulatorTracking_;
	bool monoTracking_;
	bool exitSlitTracking_;
	bool usingTEY_;
	bool usingTFY_;
	bool usingPGT_;
};

#endif // ACQMAN_SGMSCANCONFIG_H
