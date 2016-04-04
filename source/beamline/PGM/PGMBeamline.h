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

#ifndef PGMBEAMLINE_H
#define PGMBEAMLINE_H

#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"

#include "util/AMErrorMonitor.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/CLS/CLSBeamline.h"

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class PGMBeamline : public CLSBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static PGMBeamline *pgm()
	{
		if(!instance_){
			instance_ = new PGMBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<PGMBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~PGMBeamline();

signals:

public slots:

protected:
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up mono settings.
	void setupMono();
	/// Sets up various beamline components.
	void setupComponents();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up the motor group for the various sample stages.
	void setupMotorGroup();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();

	/// Constructor. This is a singleton class, access it through IDEASBeamline::ideas().
	PGMBeamline();

    /// Beamline valves NOTE: Unsure which are needed or not, so adding them all until further notice.
    /// Valves are ordered starting from storage ring outward to endstation.

    /// Valves PGM Main
    CLSExclusiveStatesControl *VVR16113I0003Valve_;
    CLSExclusiveStatesControl *VVR16113I2001Valve_;
    CLSExclusiveStatesControl *VVR16113I2002Valve_;
    CLSExclusiveStatesControl *VVR16114I2001Valve_;
    CLSExclusiveStatesControl *VVR16114I2002Valve_;
    CLSExclusiveStatesControl *VVR16114I2003Valve_;
    /// Valves Branch A
    CLSExclusiveStatesControl *VVR16114I2201Valve_;
    CLSExclusiveStatesControl *VVR16114I2202Valve_;
    CLSExclusiveStatesControl *VVR16114I2203Valve_;
    CLSExclusiveStatesControl *VVR16114I2204Valve_;
    /// Valves Branch B
    CLSExclusiveStatesControl *VVR16114I2101Valve_;
    CLSExclusiveStatesControl *VVR16114I2102Valve_;
    CLSExclusiveStatesControl *VVR16114I2103Valve_;
    CLSExclusiveStatesControl *VVR16114I2104Valve_;
    /// Beamline Shutters
    /// Photon Shutter 3
    CLSExclusiveStatesControl *PSH16113I2001Shutter3_;
    /// Photon Shutter 2
    CLSExclusiveStatesControl *PSH1411I0002Shutter2_;
    /// Photon Shutter 1.  This shutter is shared by PGM/SGM so may only want read access to this.
    CLSExclusiveStatesControl *PSH1411I0001Shutter1_;
    /// Safety Shutter 1
    CLSExclusiveStatesControl *SSH1411I0001Shutter1_;

};

#endif // PGMSBEAMLINE_H
