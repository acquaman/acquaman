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

    /// Returns the photon and safety shutters.
    /// Photon Shutter 3
    CLSExclusiveStatesControl* photonShutter3() const { return photonShutter3_; }
    /// Photon Shutter 2
    CLSExclusiveStatesControl* photonShutter2() const { return photonShutter2_; }
    /// Photon Shutter 1.  This shutter is shared by PGM/SGM so may only want read access to this.
    AMReadOnlyPVControl* photonShutter1() const { return photonShutter1_; }
    /// Safety Shutter 1
    CLSExclusiveStatesControl* safetyShutter1() const { return safetyShutter1_; }

    /// ToDo: Waiting to hear back from Lucia regarding shutter open/close logic
    /// Open and close the photon or safety shutters. Used for the logic needed when opening and closing properly.
    /// Open/Close Photon Shutter 3
    bool openPhotonShutter3();
    bool closePhotonShutter3();
    /// Open/Close Photon Shutter 2
    bool openPhotonShutter2();
    bool closePhotonShutter2();
    /// Open/Close Safety Shutter 1
    bool openSafetyShutter1();
    bool closeSafetyShutter1();

    /// Returns endstation gate valves for Branch A and B
    /// Branch A Valve
    CLSExclusiveStatesControl* valveBranchA() const { return valveBranchA_; }
    /// Branch B Valve
    CLSExclusiveStatesControl* valveBranchB() const { return valveBranchB_; }

    /// Valve status methods
    bool branchAOpen() const;
    bool branchBOpen() const;
    /// Open or close valves. Currently it's a bit overkill as there are only two main valves that
    /// I'm aware of but this is incase more get added later.
    void openValve(int index);
    void closeValve(int index);

    /// Control sets for logical grouping of controls
    AMControlSet *valveSet() const { return valveSet_; }


signals:


public slots:

protected slots:

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

    /// Beamline valves

    /// Branch A Valve
    CLSExclusiveStatesControl *valveBranchA_;
    /// Branch B Valve
    CLSExclusiveStatesControl *valveBranchB_;

    /// Beamline Shutters

    /// Photon Shutter 3
    CLSExclusiveStatesControl *photonShutter3_;
    /// Photon Shutter 2
    CLSExclusiveStatesControl *photonShutter2_;
    /// Photon Shutter 1.  This shutter is shared by PGM/SGM so may only want read access to this.
    AMReadOnlyPVControl *photonShutter1_;
    /// Safety Shutter 1
    CLSExclusiveStatesControl *safetyShutter1_;

    /// Control sets
    AMControlSet *valveSet_;

};

#endif // PGMSBEAMLINE_H
