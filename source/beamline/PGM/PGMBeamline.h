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

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/AMPVControl.h"

#include "beamline/PGM/PGMBpmControl.h"

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

    /// Returns storage ring current
    AMReadOnlyPVControl *ringCurrent() const { return ringCurrent_; }
    /// Returns beam lifetime
    AMReadOnlyPVControl *beamLifetime() const { return beamLifetime_; }

    /// Returns controls for beam position monitors
    PGMBpmControl *bpm10IDxControl() const { return bpm10IDxControl_; }
    PGMBpmControl *bpm10IDyControl() const { return bpm10IDyControl_; }
    PGMBpmControl *bpm11ID1xControl() const { return bpm11ID1xControl_; }
    PGMBpmControl *bpm11ID1yControl() const { return bpm11ID1yControl_; }
    PGMBpmControl *bpm11ID2xControl() const { return bpm11ID2xControl_; }
    PGMBpmControl *bpm11ID2yControl() const { return bpm11ID2yControl_; }
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

    /// Storage ring current
    AMReadOnlyPVControl *ringCurrent_;
    /// Beam lifetime value
    AMReadOnlyPVControl *beamLifetime_;

    /// Beam-position monitors
    /// BPM Downstream from 10ID
    PGMBpmControl *bpm10IDxControl_;
    PGMBpmControl *bpm10IDyControl_;
    /// BPM from 11ID #1
    PGMBpmControl *bpm11ID1xControl_;
    PGMBpmControl *bpm11ID1yControl_;
    /// BPM from IID #2
    PGMBpmControl *bpm11ID2xControl_;
    PGMBpmControl *bpm11ID2yControl_;
};

#endif // PGMSBEAMLINE_H
