/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ACQMAN_SGMSCANCONFIG_H
#define ACQMAN_SGMSCANCONFIG_H

#include "beamline/SGM/SGMBeamline.h"

class SGMScanConfiguration
{
public:
	SGMScanConfiguration();

	double exitSlitGap() const { return exitSlitGap_;}
	SGMBeamline::sgmGrating grating() const { return grating_;}
	SGMBeamline::sgmHarmonic harmonic() const { return harmonic_;}
	//bool undulatorTracking() { return undulatorTracking_;}
	//bool monoTracking() { return monoTracking_;}
	//bool exitSlitTracking() { return exitSlitTracking_;}
	AMControlInfoList trackingGroup() { return trackingGroup_;}

	bool setExitSlitGap(double exitSlitGap);
	bool setGrating(SGMBeamline::sgmGrating grating) {grating_ = grating; return true;}
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic) { harmonic_ = harmonic; return true;}

	//bool setUndulatorTracking(bool track){undulatorTracking_ = track; return true;}
	//bool setMonoTracking(bool track){monoTracking_ = track; return true;}
	//bool setExitSlitTracking(bool track){exitSlitTracking_ = track; return true;}
	bool setTrackingGroup(AMControlInfoList trackingGroup);

	bool setUsingTEY(bool active) { usingTEY_ = active; return true;}
	bool setUsingTFY(bool active) { usingTFY_ = active; return true;}
	bool setUsingPGT(bool active) { usingPGT_ = active; return true;}

	bool usingPGT() const { return usingPGT_; }
	bool usingTEY() const { return usingTEY_; }
	bool usingTFY() const { return usingTFY_; }

protected:
	double exitSlitGap_;
	SGMBeamline::sgmGrating grating_;
	SGMBeamline::sgmHarmonic harmonic_;
	AMControlInfoList trackingGroup_;
	bool undulatorTracking_;
	bool monoTracking_;
	bool exitSlitTracking_;
	bool usingTEY_;
	bool usingTFY_;
	bool usingPGT_;
};

#endif // ACQMAN_SGMSCANCONFIG_H
