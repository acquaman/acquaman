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

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSBeamline.h"

#include "beamline/PGM/PGMBPMControl.h"
#include "beamline/PGM/PGMOceanOpticsXRFDetector.h"
#include "beamline/PGM/PGMVAM.h"

class AMBasicControlDetectorEmulator;

/// This class is the master class that holds EVERY control inside the PGM beamline.

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
    PGMBPMControl *bpm10IDxControl() const { return bpm10IDxControl_; }
    PGMBPMControl *bpm10IDyControl() const { return bpm10IDyControl_; }
    PGMBPMControl *bpm11ID1xControl() const { return bpm11ID1xControl_; }
    PGMBPMControl *bpm11ID1yControl() const { return bpm11ID1yControl_; }
    PGMBPMControl *bpm11ID2xControl() const { return bpm11ID2xControl_; }
    PGMBPMControl *bpm11ID2yControl() const { return bpm11ID2yControl_; }

	/// returns the current beamline connected state
	virtual bool isConnected() const;

	/// The control for the branch A exit slit position
	AMPVwStatusControl *exitSlitBranchAPosition() const;

	/// The control for the branch A exit slit gap
	AMPVwStatusControl *exitSlitBranchAGap() const;

	/// The control for the branch B exit slit position
	AMPVwStatusControl *exitSlitBranchBPosition() const;

	/// The control for the branch B exit slit gap
	AMPVwStatusControl *exitSlitBranchBGap() const;

	/// The control for the entrance slit gap
	AMPVwStatusControl *entranceSlitGap() const;

    /// Returns energy control for PGM
    AMPVwStatusControl* energy() const { return energy_; }

	/// Returns the read only control for Exit slit lower blade current - branch A
	AMReadOnlyPVControl *exitSlitLowerBladeCurrentA() const { return exitSlitLowerBladeCurrentA_; }
	/// Returns the read only control for Exit slit upper blade current - branch A
	AMReadOnlyPVControl *exitSlitUpperBladeCurrentA() const { return exitSlitUpperBladeCurrentA_; }
	/// Returns the read only control for Exit slit lower blade current - branch B
	AMReadOnlyPVControl *exitSlitLowerBladeCurrentB() const { return exitSlitLowerBladeCurrentB_; }
	/// Returns the read only control for Exit slit upper blade current - branch B
	AMReadOnlyPVControl *exitSlitUpperBladeCurrentB() const { return exitSlitUpperBladeCurrentB_; }

	/// Returns the read only control for Entrance slit lower blade current
	AMReadOnlyPVControl *entranceSlitLowerBladeCurrent() const { return entranceSlitLowerBladeCurrent_; }
	/// Returns the read only control for Entrance slit upper blade current
	AMReadOnlyPVControl *entranceSlitUpperBladeCurrent() const { return entranceSlitUpperBladeCurrent_; }

	/// Returns the read only control for TEY
	AMReadOnlyPVControl *teyBladeCurrentControl() const { return teyBladeCurrentControl_; }
	/// Returns the read only control for FLY
	AMReadOnlyPVControl *flyBladeCurrentControl() const { return flyBladeCurrentControl_; }
	/// Returns the read only control for endstation Ni I0 current
	AMReadOnlyPVControl *i0EndstationBladeCurrentControl() const { return i0EndstationBladeCurrentControl_; }
	/// Returns the read only control for beamline Ni I0 current
	AMReadOnlyPVControl *i0BeamlineBladeCurrentControl() const { return i0BeamlineBladeCurrentControl_; }
	/// Returns the read only control for photodiode current
	AMReadOnlyPVControl *photodiodeBladeCurrentControl() const { return photodiodeBladeCurrentControl_; }

	/// Returns the variable aperture mask.
	PGMVAM* vam() const { return vam_; }

	/// Returns the Ocean Optics XRF detector.
	PGMOceanOpticsXRFDetector* oceanOpticsDetector() const { return oceanOpticsDetector_; }

signals:

public slots:

protected slots:
	/// slot to handle connection changed signals of the control
	void onControlConnectionChanged();

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

protected:
	/// flag to identify whether the beamline controls were connected or not
	bool connected_;

	/// Storage ring current
	AMReadOnlyPVControl *ringCurrent_;
	/// Beam lifetime value
	AMReadOnlyPVControl *beamLifetime_;

	/// Beam-position monitors
	/// BPM Downstream from 10ID
	PGMBPMControl *bpm10IDxControl_;
	PGMBPMControl *bpm10IDyControl_;
	/// BPM from 11ID #1
	PGMBPMControl *bpm11ID1xControl_;
	PGMBPMControl *bpm11ID1yControl_;
	/// BPM from 11ID #2
	PGMBPMControl *bpm11ID2xControl_;
	PGMBPMControl *bpm11ID2yControl_;

	// Exit slit gap/position for both branches

	AMPVwStatusControl* exitSlitBranchAPosition_;
	AMPVwStatusControl* exitSlitBranchAGap_;
	AMPVwStatusControl* exitSlitBranchBPosition_;
	AMPVwStatusControl* exitSlitBranchBGap_;

    AMPVwStatusControl *entranceSlitGap_;

	AMControlSet* allControls_;

    /// Energy control for PGM
    AMPVwStatusControl *energy_;

	/// Read only control for Exit slit lower blade current - branch A
	AMReadOnlyPVControl *exitSlitLowerBladeCurrentA_;
	/// Read only control for Exit slit upper blade current - branch A
	AMReadOnlyPVControl *exitSlitUpperBladeCurrentA_;
	/// Read only control for Exit slit lower blade current - branch B
	AMReadOnlyPVControl *exitSlitLowerBladeCurrentB_;
	/// Read only control for Exit slit upper blade current - branch B
	AMReadOnlyPVControl *exitSlitUpperBladeCurrentB_;

	/// Read only control for Entrance slit lower blade current
	AMReadOnlyPVControl *entranceSlitLowerBladeCurrent_;
	/// Read only control for Entrance slit upper blade current
	AMReadOnlyPVControl *entranceSlitUpperBladeCurrent_;

	/// Read only control for TEY
	AMReadOnlyPVControl *teyBladeCurrentControl_;
	/// Read only control for FLY
	AMReadOnlyPVControl *flyBladeCurrentControl_;
	/// Read only control for endstation Ni I0 current
	AMReadOnlyPVControl *i0EndstationBladeCurrentControl_;
	/// Read only control for beamline Ni I0 current
	AMReadOnlyPVControl *i0BeamlineBladeCurrentControl_;
	/// Read only control for photodiode current
	AMReadOnlyPVControl *photodiodeBladeCurrentControl_;

	/// The variable aperture mask.
	PGMVAM* vam_;

	// Detectors
	AMBasicControlDetectorEmulator *exitSlitLowerBladeCurrentADetector_;
	AMBasicControlDetectorEmulator *exitSlitUpperBladeCurrentADetector_;
	AMBasicControlDetectorEmulator *exitSlitLowerBladeCurrentBDetector_;
	AMBasicControlDetectorEmulator *exitSlitUpperBladeCurrentBDetector_;

	AMBasicControlDetectorEmulator *entranceSlitLowerBladeCurrentDetector_;
	AMBasicControlDetectorEmulator *entranceSlitUpperBladeCurrentDetector_;

	AMBasicControlDetectorEmulator *teyBladeCurrentDetector_;
	AMBasicControlDetectorEmulator *flyBladeCurrentDetector_;
	AMBasicControlDetectorEmulator *i0EndstationBladeCurrentDetector_;
	AMBasicControlDetectorEmulator *i0BeamlineBladeCurrentDetector_;
	AMBasicControlDetectorEmulator *photodiodeBladeCurrentDetector_;

	/// The Ocean Optics detector.
	PGMOceanOpticsXRFDetector *oceanOpticsDetector_;
};

#endif // PGMBEAMLINE_H
