/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM_SGMSCANCONFIG_H
#define AM_SGMSCANCONFIG_H

#include "beamline/SGM/SGMBeamline.h"

class SGMScanConfiguration
{
public:
	/// Standard constructor, sets up nominal beamline values
 	virtual ~SGMScanConfiguration();
	SGMScanConfiguration();

	/// Returns a control info list regarding the tracking group (mono tracking, exit slit tracking, and undulator tracking)
	AMControlInfoList trackingGroup() const;
	/// Returns a control info list regarding the flux/resolution group (exit slit gap, grating, and harmonic)
	AMControlInfoList fluxResolutionGroup() const;

	/// Returns whether the undulator is tracking for this configuration
	bool undulatorTracking() const;
	/// Returns whether the mono is tracking for this configuration
	bool monoTracking() const;
	/// Returns whether the exit slit is tracking for this configuration
	bool exitSlitTracking() const;

	/// Returns the exit slit gap setting for this configuration
	double exitSlitGap() const;
	/// Returns the grating chosen for this configuration
	SGMBeamlineInfo::sgmGrating grating() const;
	/// Returns the harmonic chosen for this configuration
	SGMBeamlineInfo::sgmHarmonic harmonic() const;

	/// Sets the tracking group from an existing control info list
	bool setTrackingGroup(AMControlInfoList trackingGroup);
	/// Sets the flux/resolution group from an existing control info list
	bool setFluxResolutionGroup(AMControlInfoList fluxResolutionGroup);

	/// Sets the undulator tracking for this configuration
	bool setUndulatorTracking(bool undulatorTracking);
	/// Sets the mono tracking for this configuration
	bool setMonoTracking(bool monoTracking);
	/// Sets the exit slit tracking for this configuration
	bool setExitSlitTracking(bool exitSlitTracking);

	/// Sets the exit slit gap setting for this configuration
	bool setExitSlitGap(double exitSlitGap);
	/// Sets which grating to use for this configuration
	bool setGrating(SGMBeamlineInfo::sgmGrating grating);
	/// Sets which harmonic to use for this configuration
	bool setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonic);

	/* NTBA March 14, 2011 David Chevrier
	   Need something like setTrackingGroup for the detectorSet
	*/

protected:
	/// Info list for the tracking group (track mono, track undulator, track exit slit)
	AMControlInfoList trackingGroup_;
	/// Info list for the flux/resolution group (exit slit gap, grating, harmonic)
	AMControlInfoList fluxResolutionGroup_;

	/// Whether or not the undulator tracking is on
	bool undulatorTracking_;
	/// Whether or not the mono tracking is on
	bool monoTracking_;
	/// Whether or not the exit slit tracking is on
	bool exitSlitTracking_;

	/// Value for the exit slit for this configuration
	double exitSlitGap_;
	/// Which grating will be used for this configuration
	SGMBeamlineInfo::sgmGrating grating_;
	/// Which harmonic will be used for this configuration
	SGMBeamlineInfo::sgmHarmonic harmonic_;
};

#endif // AM_SGMSCANCONFIG_H
