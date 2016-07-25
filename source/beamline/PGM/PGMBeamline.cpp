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

#include "PGMBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/PGM/PGMBranchSelectionControl.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

PGMBeamline::PGMBeamline()
	: CLSBeamline("PGM Beamline")
{
	connected_ = false;

	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupMono();
	setupMotorGroup();
	setupControlSets();
	setupControlsAsDetectors();
	setupHVControls();
	setupExposedControls();
	setupExposedDetectors();

	// update the connection changed flag and emit the signal to indicate whether conneciton is changed
	onControlConnectionChanged();
}

PGMBeamline::~PGMBeamline()
{

}

bool PGMBeamline::isConnected() const
{
	return requiredControls_->isConnected();
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchAPosition() const
{
	return exitSlitBranchAPosition_;
}

AMSinglePVControl *PGMBeamline::exitSlitBranchAPositionTracking() const
{
	return exitSlitBranchAPositionTracking_;
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchAGap() const
{
	return exitSlitBranchAGap_;
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchBPosition() const
{
	return exitSlitBranchBPosition_;
}

AMSinglePVControl *PGMBeamline::exitSlitBranchBPositionTracking() const
{
	return exitSlitBranchBPositionTracking_;
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchBGap() const
{
	return exitSlitBranchBGap_;
}

AMPVwStatusControl *PGMBeamline::entranceSlitGap() const
{
	return entranceSlitGap_;
}

AMPVwStatusControl *PGMBeamline::undulatorGap() const
{
	return undulatorGap_;
}

AMSinglePVControl *PGMBeamline::undulatorTracking() const
{
	return undulatorTracking_;
}

AMSinglePVControl *PGMBeamline::gratingTracking() const
{
	return gratingTracking_;
}

AMControl *PGMBeamline::branchSelectionControl() const
{
	return branchSelectionControl_;
}

AMControl *PGMBeamline::gratingSelectionControl() const
{
	return gratingSelectionControl_;
}

void PGMBeamline::onControlConnectionChanged()
{
	bool isConnectedNow = isConnected();
	if (connected_ != isConnectedNow) {
		connected_ = isConnectedNow;

		emit connected(connected_);
	}
}

void PGMBeamline::setupDiagnostics()
{

}

void PGMBeamline::setupSampleStage()
{

}

void PGMBeamline::setupMotorGroup()
{

}

void PGMBeamline::setupDetectors()
{
	exitSlitLowerBladeCurrentADetector_ = new PGMPicoAmmeter("exitSlitLowerBladeCurrentA", "Exit Slit Lower A", "A1611-4-02:nA:fbk", this);
	exitSlitUpperBladeCurrentADetector_ = new PGMPicoAmmeter("exitSlitUpperBladeCurrentA", "Exit Slit Upper A", "A1611-4-03:nA:fbk", this);
	exitSlitLowerBladeCurrentBDetector_ = new PGMPicoAmmeter("exitSlitLowerBladeCurrentB", "Exit Slit Lower B", "A1611-4-04:nA:fbk", this);
	exitSlitUpperBladeCurrentBDetector_ = new PGMPicoAmmeter("exitSlitUpperBladeCurrentB", "Exit Slit Upper B", "A1611-4-05:nA:fbk", this);

	entranceSlitLowerBladeCurrentDetector_ = new PGMPicoAmmeter("entranceSlitLowerBladeCurrent", "Entrance Slit Lower", "A1611-3-03:nA:fbk", this);
	entranceSlitUpperBladeCurrentDetector_ = new PGMPicoAmmeter("entranceSlitUpperBladeCurrent", "Entrance Slit Upper", "A1611-3-04:nA:fbk", this);

	teyBladeCurrentDetector_ = new PGMPicoAmmeter("teyBladeCurrentDetector", "TEY", "A1611-4-09:nA:fbk", this);
	flyBladeCurrentDetector_ = new PGMPicoAmmeter("flyBladeCurrentDetector", "FLY", "A1611-4-08:nA:fbk", this);
	i0EndstationBladeCurrentDetector_ = new PGMPicoAmmeter("i0EndstationBladeCurrentDetector", "Endstation I0", "A1611-4-11:nA:fbk", this);
	i0BeamlineBladeCurrentDetector_ = new PGMPicoAmmeter("i0BeamlineBladeCurrentDetector", "Beamline I0", "A1611-4-12:nA:fbk", this);
	photodiodeBladeCurrentDetector_ = new PGMPicoAmmeter("photodiodeBladeCurrentDetector", "Photodiode" , "A1611-4-10:nA:fbk", this);
}

void PGMBeamline::setupControlSets()
{	
	requiredControls_ = new AMControlSet(this);

	requiredControls_->addControl(beamlineStatus_);
	requiredControls_->addControl(exitSlitBranchAPosition_);
	requiredControls_->addControl(exitSlitBranchAGap_);
	requiredControls_->addControl(exitSlitBranchBPosition_);
	requiredControls_->addControl(exitSlitBranchBGap_);
	requiredControls_->addControl(entranceSlitGap_);
	requiredControls_->addControl(energy_);
	requiredControls_->addControl(vam_);
	requiredControls_->addControl(undulatorGap_);
	requiredControls_->addControl(branchSelectionControl_);

	connect(requiredControls_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));
}

void PGMBeamline::setupMono()
{
	energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.01);
	energy_->enableLimitMonitoring();
}

void PGMBeamline::setupComponents()
{
	// ==== set up the beamline status
	//       Photon Shutters
	AMControl *photonShutter1 = new AMReadOnlyPVControl("PhotonShutter1", "PSH1411-I00-01:state", this, "Photon Shutter 1");
	AMControl *photonShutter2 = new CLSExclusiveStatesControl("PhotonShutter2", "PSH1411-I00-02:state", "PSH1411-I00-02:opr:open", "PSH1411-I00-02:opr:close", this, "Photon Shutter 2");
	AMControl *photonShutter3 = new CLSExclusiveStatesControl("PhotonShutter3", "PSH1611-3-I20-01:state", "PSH1611-3-I20-01:opr:open", "PSH1611-3-I20-01:opr:close", this, "Photon Shutter 3");
	AMControl *safetyShutter1 = new CLSExclusiveStatesControl("SafetyShutter1", "SSH1411-I00-01:state",  "SSH1411-I00-01:opr:open", "SSH1411-I00-01:opr:close", this, "Safety Shutter 1");

	beamlineShutters_ = new CLSShutters(QString("PGM Shutters"), this);
	beamlineShutters_->addShutter(photonShutter1, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineShutters_->addShutter(photonShutter2, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineShutters_->addShutter(photonShutter3, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineShutters_->addShutter(safetyShutter1, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);

	//      set up the valves
	AMControl *branchAValve = new CLSExclusiveStatesControl("BranchAValves", "VVR1611-4-I22-04:state",  "VVR1611-4-I22-04:opr:open",  "VVR1611-4-I22-04:opr:close", this, "Branch A Valve");
	AMControl *branchBValve = new CLSExclusiveStatesControl("BranchBValves", "VVR1611-4-I21-04:state",  "VVR1611-4-I21-04:opr:open",  "VVR1611-4-I21-04:opr:close", this, "Branch B Valve");

	beamlineValves_ = new CLSValves(QString("PGM Valves"), this);
	beamlineValves_->addValve(branchAValve, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineValves_->addValve(branchBValve, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);

	//       set up the Beamline status.
	beamlineStatus_ = new CLSBeamlineStatus("PGM BeamlineStatus", this);

	beamlineStatus_->addShutterControl(beamlineShutters_, CLSShutters::Open);
	beamlineStatus_->addValveControl(beamlineValves_, CLSValves::Open);

	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1611-ID-2:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);

	// Storage ring
	ringCurrent_ = new AMReadOnlyPVControl("Ring Current", "PCT1402-01:mA:fbk", this);
	// Beam lifetime
	beamLifetime_ = new AMReadOnlyPVControl("Beam Lifetime", "PCT1402-01:halfLife", this);

	// BPM for 10ID
	bpm10IDxControl_ = new PGMBPMControl("BPM 10ID Downstream-X", "BPM1410-05:x:um", 665, 50, this);
	bpm10IDyControl_ = new PGMBPMControl("BPM 10ID Downstream-Y", "BPM1410-05:y:um",-245, 50, this);
	// BPM from 11ID #1
	bpm11ID1xControl_ = new PGMBPMControl("BPM 11ID #1-X", "BPM1411-01:x:um", -400, 50, this);
	bpm11ID1yControl_ = new PGMBPMControl("BPM 11ID #1-Y", "BPM1411-01:y:um", -970, 50, this);
	// BPM from 11ID #2
	bpm11ID2xControl_ = new PGMBPMControl("BPM 11ID #2-X", "BPM1411-02:x:um", -505, 50, this);
	bpm11ID2yControl_ = new PGMBPMControl("BPM 11ID #2-Y", "BPM1411-02:y:um", -245, 50, this);

	oceanOpticsDetector_ = new PGMOceanOpticsXRFDetector("OceanOpticsDetector", "Ocean Optics XRF Detector", this);
	connect( oceanOpticsDetector_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()) );

	// ==== set up the slits
	exitSlitBranchAPosition_ = new AMPVwStatusControl("Exit Slit (A) Position", "PSL16114I2101:X:mm:fbk", "PSL16114I2101:X:mm", "SMTR16114I2104:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchAPositionTracking_ = new AMSinglePVControl("Exit Slit (A) Tracking", "PSL16114I2101:X:Energy:track", this);
	exitSlitBranchAGap_ = new AMPVwStatusControl("Exit Slit (A) Gap", "PSL16114I2101:Y:mm:fbk", "PSL16114I2101:Y:mm", "SMTR16114I2105:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchBPosition_ = new AMPVwStatusControl("Exit Slit (B) Position", "PSL16114I2201:X:mm:fbk", "PSL16114I2201:X:mm", "SMTR16114I2204:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchBPositionTracking_ = new AMSinglePVControl("Exit Slit (B) Tracking", "PSL16114I2201:X:Energy:track", this);
	exitSlitBranchBGap_ = new AMPVwStatusControl("Exit Slit (B) Gap", "PSL16114I2201:Y:mm:fbk", "PSL16114I2201:Y:mm", "SMTR16114I2205:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

	entranceSlitGap_ = new AMPVwStatusControl("Entrance Slit","PSL16113I2001:Y:mm:fbk", "PSL16113I2001:Y:mm", "SMTR16113I2010:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

	undulatorGap_ = new AMPVwStatusControl("Undulator Gap", "UND1411-02:gap:mm:fbk", "UND1411-02:gap:mm", "SMTR1411-02:moving", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	undulatorTracking_ = new AMSinglePVControl("Undulator Tracking", "UND1411-02:Energy:track", this);
	gratingTracking_ = new AMSinglePVControl("Grating Tracking", "PGM_mono:Energy:track", this);

	branchSelectionControl_ = new PGMBranchSelectionControl(this);
	gratingSelectionControl_ = new PGMMonoGratingSelectionControl(this);
	vam_ = new PGMVariableApertureMask("VAM", this);
}

void PGMBeamline::setupControlsAsDetectors()
{
	// The undulator gap.

	undulatorGapDetector_ = new AMBasicControlDetectorEmulator("PGMUndulatorGapDetector", "Undulator gap", undulatorGap_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	// The ring current.

	ringCurrentDetector_ = new AMBasicControlDetectorEmulator("PGMRingCurrentDetector", "Ring current", ringCurrent_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
}

void PGMBeamline::setupHVControls()
{
	branchAI0BLHVControl_ = new CLSHVControl("I0_BL Bias", "PS1611401:203", ":vmon", ":v0set", ":pwonoff", ":status"); //:imon
	branchATeyHVControl_ = new CLSHVControl("TEY Bias", "PS1611401:204", ":vmon", ":v0set", ":pwonoff", ":status");
	branchAI0EHVControl_ = new CLSHVControl("I0_E Bias", "PS1611401:205", ":vmon", ":v0set", ":pwonoff", ":status");
	branchAFLHVControl_ = new CLSHVControl("FL Detect", "PS1611401:103", ":vmon", ":v0set", ":pwonoff", ":status"); //negative HV
	branchA104HVControl_ = new CLSHVControl("", "PS1611401:104", ":vmon", ":v0set", ":pwonoff", ":status"); //negative HV
	branchA105HVControl_ = new CLSHVControl(" ", "PS1611401:105", ":vmon", ":v0set", ":pwonoff", ":status"); //negative HV

	branchBI0BLHVControl_ = new CLSHVControl("I0_BL Bias", "PS1611401:200", ":vmon", ":v0set", ":pwonoff", ":status");
	branchBTeyHVControl_ = new CLSHVControl("TEY Bias", "PS1611401:201", ":vmon", ":v0set", ":pwonoff", ":status");
	branchBI0EHVControl_ = new CLSHVControl("I0_E Bias", "PS1611401:202", ":vmon", ":v0set", ":pwonoff", ":status");
	branchBFLHVControl_ = new CLSHVControl("FL Detect", "PS1611401:100", ":vmon", ":v0set", ":pwonoff", ":status"); //negative HV
	branchB101HVControl_ = new CLSHVControl("", "PS1611401:101", ":vmon", ":v0set", ":pwonoff", ":status"); //negative HV
	branchB102HVControl_ = new CLSHVControl(" ", "PS1611401:102", ":vmon", ":v0set", ":pwonoff", ":status"); //negative HV

	branchAHVControlSet_ = new AMControlSet(this);
	branchAHVControlSet_->setName("High Voltage: Branch A");
	branchAHVControlSet_->addControl(branchAI0BLHVControl_);
	branchAHVControlSet_->addControl(branchATeyHVControl_);
	branchAHVControlSet_->addControl(branchAI0EHVControl_);
	branchAHVControlSet_->addControl(branchAFLHVControl_);
	branchAHVControlSet_->addControl(branchA104HVControl_);
	branchAHVControlSet_->addControl(branchA105HVControl_);

	branchBHVControlSet_ = new AMControlSet(this);
	branchBHVControlSet_->setName("High Voltage: Branch B");
	branchBHVControlSet_->addControl(branchBI0BLHVControl_);
	branchBHVControlSet_->addControl(branchBTeyHVControl_);
	branchBHVControlSet_->addControl(branchBI0EHVControl_);
	branchBHVControlSet_->addControl(branchBFLHVControl_);
	branchBHVControlSet_->addControl(branchB101HVControl_);
	branchBHVControlSet_->addControl(branchB102HVControl_);
}

void PGMBeamline::setupExposedControls()
{
	addExposedControl(energy_);

	addExposedControl(vam_->upperBlade());
	addExposedControl(vam_->lowerBlade());
	addExposedControl(vam_->outboardBlade());
	addExposedControl(vam_->inboardBlade());
}

void PGMBeamline::setupExposedDetectors()
{
	// Setup all exposed detectors.
	addExposedDetector(exitSlitLowerBladeCurrentADetector_);
	addExposedDetector(exitSlitUpperBladeCurrentADetector_);
	addExposedDetector(exitSlitLowerBladeCurrentBDetector_);
	addExposedDetector(exitSlitUpperBladeCurrentBDetector_);

	addExposedDetector(entranceSlitLowerBladeCurrentDetector_);
	addExposedDetector(entranceSlitUpperBladeCurrentDetector_);

	addExposedDetector(teyBladeCurrentDetector_);
	addExposedDetector(flyBladeCurrentDetector_);
	addExposedDetector(i0EndstationBladeCurrentDetector_);
	addExposedDetector(i0BeamlineBladeCurrentDetector_);
	addExposedDetector(photodiodeBladeCurrentDetector_);

	addExposedDetector(oceanOpticsDetector_);

	addExposedDetector(undulatorGapDetector_);
	addExposedDetector(ringCurrentDetector_);

	// Setup scientific exposed detectors. I picked random ones for testing.
	addExposedScientificDetector(oceanOpticsDetector_);
	addExposedScientificDetector(teyBladeCurrentDetector_);
	addExposedScientificDetector(flyBladeCurrentDetector_);

	addExposedScientificDetector(undulatorGapDetector_);
	addExposedScientificDetector(ringCurrentDetector_);
}
