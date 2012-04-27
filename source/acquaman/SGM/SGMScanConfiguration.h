/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

	AMControlInfoList trackingGroup() const;
	AMControlInfoList fluxResolutionGroup() const;

	bool undulatorTracking() const;
	bool monoTracking() const;
	bool exitSlitTracking() const;

	double exitSlitGap() const;
	SGMBeamline::sgmGrating grating() const;
	SGMBeamline::sgmHarmonic harmonic() const;

	bool setTrackingGroup(AMControlInfoList trackingGroup);
	bool setFluxResolutionGroup(AMControlInfoList fluxResolutionGroup);

	bool setUndulatorTracking(bool undulatorTracking);
	bool setMonoTracking(bool monoTracking);
	bool setExitSlitTracking(bool exitSlitTracking);

	bool setExitSlitGap(double exitSlitGap);
	bool setGrating(SGMBeamline::sgmGrating grating);
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic);

	/* NTBA March 14, 2011 David Chevrier
	   Need something like setTrackingGroup for the detectorSet
	*/

protected:
	AMControlInfoList trackingGroup_;
	AMControlInfoList fluxResolutionGroup_;

	bool undulatorTracking_;
	bool monoTracking_;
	bool exitSlitTracking_;

	double exitSlitGap_;
	SGMBeamline::sgmGrating grating_;
	SGMBeamline::sgmHarmonic harmonic_;
};

#endif // ACQMAN_SGMSCANCONFIG_H
