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
#include "beamline/CLS/CLSSynchronizedDwellTime.h"

#include "beamline/SXRMB/SXRMBHVControl.h"
#include "beamline/PGM/PGMPicoAmmeter.h"
#include "beamline/PGM/PGMBPMControl.h"
#include "beamline/PGM/PGMOceanOpticsXRFDetector.h"

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

	/// The control for determining whether the branch A exit slit position tracks the energy
	AMSinglePVControl *exitSlitBranchAPositionTracking() const;

	/// The control for the branch A exit slit gap
	AMPVwStatusControl *exitSlitBranchAGap() const;

	/// The control for the branch B exit slit position
	AMPVwStatusControl *exitSlitBranchBPosition() const;

	/// The control for determining whether the branch B exit slit position tracks the energy
	AMSinglePVControl *exitSlitBranchBPositionTracking() const;

	/// The control for the branch B exit slit gap
	AMPVwStatusControl *exitSlitBranchBGap() const;

	/// The control for the entrance slit gap
	AMPVwStatusControl *entranceSlitGap() const;

	/// Returns energy control for PGM
	AMPVwStatusControl* energy() const { return energy_; }

	/// The control which determines whether the undulator tracks the energy
	AMSinglePVControl* undulatorTracking() const;

	/// The control which determines whether the mono grating tracks the energy
	AMSinglePVControl* gratingTracking() const;

	/// Returns the read only control for Exit slit lower blade current - branch A
	PGMPicoAmmeter *exitSlitLowerBladeCurrentA() const { return exitSlitLowerBladeCurrentADetector_; }
	/// Returns the read only control for Exit slit upper blade current - branch A
	PGMPicoAmmeter *exitSlitUpperBladeCurrentA() const { return exitSlitUpperBladeCurrentADetector_; }
	/// Returns the read only control for Exit slit lower blade current - branch B
	PGMPicoAmmeter *exitSlitLowerBladeCurrentB() const { return exitSlitLowerBladeCurrentBDetector_; }
	/// Returns the read only control for Exit slit upper blade current - branch B
	PGMPicoAmmeter *exitSlitUpperBladeCurrentB() const { return exitSlitUpperBladeCurrentBDetector_; }

	/// Returns the read only control for Entrance slit lower blade current
	PGMPicoAmmeter *entranceSlitLowerBladeCurrent() const { return entranceSlitLowerBladeCurrentDetector_; }
	/// Returns the read only control for Entrance slit upper blade current
	PGMPicoAmmeter *entranceSlitUpperBladeCurrent() const { return entranceSlitUpperBladeCurrentDetector_; }

	/// Returns the read only control for TEY
	PGMPicoAmmeter *teyBladeCurrentControl() const { return teyBladeCurrentDetector_; }
	/// Returns the read only control for FLY
	PGMPicoAmmeter *flyBladeCurrentControl() const { return flyBladeCurrentDetector_; }
	/// Returns the read only control for endstation Ni I0 current
	PGMPicoAmmeter *i0EndstationBladeCurrentControl() const { return i0EndstationBladeCurrentDetector_; }
	/// Returns the read only control for beamline Ni I0 current
	PGMPicoAmmeter *i0BeamlineBladeCurrentControl() const { return i0BeamlineBladeCurrentDetector_; }
	/// Returns the read only control for photodiode current
	PGMPicoAmmeter *photodiodeBladeCurrentControl() const { return photodiodeBladeCurrentDetector_; }

	/// Returns the CLS Synchronized dwell time.
	AMSynchronizedDwellTime *synchronizedDwellTime() const { return synchronizedDwellTime_; }

	/// Returns the Ocean Optics XRF detector.
	PGMOceanOpticsXRFDetector* oceanOpticsDetector() const { return oceanOpticsDetector_; }

	/// Returns the set of HV controls
	AMControlSet *beamlineHVControlSet() const { return beamlineHVControlSet_; }

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
	/// Sets up the HV controls
	void setupHVControls();
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

	/// Energy control for PGM
	AMPVwStatusControl *energy_;

	// Detectors
	PGMPicoAmmeter *exitSlitLowerBladeCurrentADetector_;
	PGMPicoAmmeter *exitSlitUpperBladeCurrentADetector_;
	PGMPicoAmmeter *exitSlitLowerBladeCurrentBDetector_;
	PGMPicoAmmeter *exitSlitUpperBladeCurrentBDetector_;

	PGMPicoAmmeter *entranceSlitLowerBladeCurrentDetector_;
	PGMPicoAmmeter *entranceSlitUpperBladeCurrentDetector_;

	PGMPicoAmmeter *teyBladeCurrentDetector_;
	PGMPicoAmmeter *flyBladeCurrentDetector_;
	PGMPicoAmmeter *i0EndstationBladeCurrentDetector_;
	PGMPicoAmmeter *i0BeamlineBladeCurrentDetector_;
	PGMPicoAmmeter *photodiodeBladeCurrentDetector_;

	// Synchronized dwell time.
	CLSSynchronizedDwellTime *synchronizedDwellTime_;

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

	/// The exit slit position control for A branch
	AMPVwStatusControl* exitSlitBranchAPosition_;
	/// The control which determines whether the A branch exit slit position tracks the energy
	AMSinglePVControl * exitSlitBranchAPositionTracking_;
	/// The exit slit gap control for A branch
	AMPVwStatusControl* exitSlitBranchAGap_;
	/// The exit slit position control for B branch
	AMPVwStatusControl* exitSlitBranchBPosition_;
	/// The control which determines whether the B branch exit slit position tracks the energy
	AMSinglePVControl* exitSlitBranchBPositionTracking_;
	/// The exit slit gap control for B branch
	AMPVwStatusControl* exitSlitBranchBGap_;

	/// The entrance slit control
    AMPVwStatusControl *entranceSlitGap_;

    /// Energy control for PGM
    AMPVwStatusControl *energy_;

	/// The control which determines whether the undulator tracks the energy
	AMSinglePVControl *undulatorTracking_;

	/// The control which determines whether the mono grating tracks the energy
	AMSinglePVControl *gratingTracking_;

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

	/// High Votage controls
	SXRMBHVControl *branchAI0BLHVControl_;
	SXRMBHVControl *branchATeyHVControl_;
	SXRMBHVControl *branchAI0EHVControl_;
	SXRMBHVControl *branchAFLHVControl_ ;
	SXRMBHVControl *branchA104HVControl_;
	SXRMBHVControl *branchA105HVControl_;

	SXRMBHVControl *branchBI0BLHVControl_;
	SXRMBHVControl *branchBTeyHVControl_;
	SXRMBHVControl *branchBI0EHVControl_;
	SXRMBHVControl *branchBFLHVControl_ ;
	SXRMBHVControl *branchB101HVControl_;
	SXRMBHVControl *branchB102HVControl_;

	/// The controls which are required to be connected for the beamline to return connected
	AMControlSet* requiredControls_;
	/// The control set of the HV controls
	AMControlSet* beamlineHVControlSet_;

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
