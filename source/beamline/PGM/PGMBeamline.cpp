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
#include "beamline/AMBasicControlDetectorEmulator.h"

PGMBeamline::PGMBeamline()
	: CLSBeamline("PGM Beamline")
{
	connected_ = false;

	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
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

AMSinglePVControl *PGMBeamline::undulatorTracking() const
{
	return undulatorTracking_;
}

AMSinglePVControl *PGMBeamline::gratingTracking() const
{
	return gratingTracking_;
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

}

void PGMBeamline::setupControlSets()
{
	requiredControls_ = new AMControlSet(this);

	requiredControls_->addControl(exitSlitBranchAPosition_);
	requiredControls_->addControl(exitSlitBranchAGap_);
	requiredControls_->addControl(exitSlitBranchBPosition_);
	requiredControls_->addControl(exitSlitBranchBGap_);
	requiredControls_->addControl(entranceSlitGap_);
	requiredControls_->addControl(energy_);

	connect(requiredControls_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));
}

void PGMBeamline::setupMono()
{
	energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.001, 2.0, new CLSMAXvControlStatusChecker());
	energy_->enableLimitMonitoring();
}

void PGMBeamline::setupComponents()
{
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

    energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.001, 2.0, new CLSMAXvControlStatusChecker());
    energy_->enableLimitMonitoring();

	oceanOpticsDetector_ = new PGMOceanOpticsXRFDetector("OceanOpticsDetector", "Ocean Optics XRF Detector", this);
	connect( oceanOpticsDetector_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()) );

	exitSlitBranchAPosition_ = new AMPVwStatusControl("Exit Slit (A) Position", "PSL16114I2101:X:mm:fbk", "PSL16114I2101:X:mm", "SMTR16114I2104:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchAPositionTracking_ = new AMSinglePVControl("Exit Slit (A) Tracking", "PSL16114I2101:X:Energy:track", this);
	exitSlitBranchAGap_ = new AMPVwStatusControl("Exit Slit (A) Gap", "PSL16114I2101:Y:mm:fbk", "PSL16114I2101:Y:mm", "SMTR16114I2105:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchBPosition_ = new AMPVwStatusControl("Exit Slit (B) Position", "PSL16114I2201:X:mm:fbk", "PSL16114I2201:X:mm", "SMTR16114I2204:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchBPositionTracking_ = new AMSinglePVControl("Exit Slit (B) Tracking", "PSL16114I2201:X:Energy:track", this);
	exitSlitBranchBGap_ = new AMPVwStatusControl("Exit Slit (B) Gap", "PSL16114I2201:Y:mm:fbk", "PSL16114I2201:Y:mm", "SMTR16114I2205:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

	entranceSlitGap_ = new AMPVwStatusControl("Entrance Slit","PSL16113I2001:Y:mm:fbk", "PSL16113I2001:Y:mm", "SMTR16113I2010:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

    energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.001, 2.0, new CLSMAXvControlStatusChecker());
    energy_->enableLimitMonitoring();

	undulatorTracking_ = new AMSinglePVControl("Undulator Tracking", "UND1411-02:Energy:track", this);

	gratingTracking_ = new AMSinglePVControl("Grating Tracking", "PGM_mono:Energy:track", this);

	exitSlitLowerBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentA", "A1611-4-02:nA:fbk", this);
	exitSlitLowerBladeCurrentA_->setDescription("Exit Slit Lower A");
	exitSlitUpperBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentA", "A1611-4-03:nA:fbk", this);
	exitSlitUpperBladeCurrentA_->setDescription("Exit Slit Upper A");
	exitSlitLowerBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentB", "A1611-4-04:nA:fbk", this);
	exitSlitLowerBladeCurrentB_->setDescription("Exit Slit Lower B");
	exitSlitUpperBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentB", "A1611-4-05:nA:fbk", this);
	exitSlitUpperBladeCurrentB_->setDescription("Exit Slit Upper B");

	entranceSlitLowerBladeCurrent_ = new AMReadOnlyPVControl("entranceSlitLowerBladeCurrent", "A1611-3-03:nA:fbk", this);
	entranceSlitLowerBladeCurrent_->setDescription("Entrance Slit Lower");
	entranceSlitUpperBladeCurrent_ = new AMReadOnlyPVControl("entranceSlitUpperBladeCurrent", "A1611-3-04:nA:fbk", this);
	entranceSlitUpperBladeCurrent_->setDescription("Entrance Slit Lower");

	teyBladeCurrentControl_ = new AMReadOnlyPVControl("teyBladeCurrentControl", "A1611-4-09:nA:fbk", this);
	teyBladeCurrentControl_->setDescription("TEY");
	flyBladeCurrentControl_ = new AMReadOnlyPVControl("flyBladeCurrentControl", "A1611-4-08:nA:fbk", this);
	flyBladeCurrentControl_->setDescription("FLY");
	i0EndstationBladeCurrentControl_ = new AMReadOnlyPVControl("i0EndstationBladeCurrentControl", "A1611-4-11:nA:fbk", this);
	i0EndstationBladeCurrentControl_->setDescription("Endstation I0");
	i0BeamlineBladeCurrentControl_ = new AMReadOnlyPVControl("i0BeamlineBladeCurrentControl", "A1611-4-12:nA:fbk", this);
	i0BeamlineBladeCurrentControl_->setDescription("Beamline I0");
	photodiodeBladeCurrentControl_ = new AMReadOnlyPVControl("photodiodeBladeCurrentControl", "A1611-4-10:nA:fbk", this);
	photodiodeBladeCurrentControl_->setDescription("Photodiode");
}

void PGMBeamline::setupControlsAsDetectors()
{
	exitSlitLowerBladeCurrentADetector_ = new AMBasicControlDetectorEmulator("exitSlitLowerBladeCurrentADetector", "exitSlitLowerBladeCurrentADetector", exitSlitLowerBladeCurrentA_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	exitSlitUpperBladeCurrentADetector_ = new AMBasicControlDetectorEmulator("exitSlitUpperBladeCurrentADetector", "exitSlitUpperBladeCurrentADetector", exitSlitUpperBladeCurrentA_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	exitSlitLowerBladeCurrentBDetector_ = new AMBasicControlDetectorEmulator("exitSlitLowerBladeCurrentBDetector", "exitSlitLowerBladeCurrentBDetector", exitSlitLowerBladeCurrentB_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	exitSlitUpperBladeCurrentBDetector_ = new AMBasicControlDetectorEmulator("exitSlitUpperBladeCurrentBDetector", "exitSlitUpperBladeCurrentBDetector", exitSlitUpperBladeCurrentB_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	entranceSlitLowerBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("entranceSlitLowerBladeCurrentDetector", "entranceSlitLowerBladeCurrentDetector", entranceSlitLowerBladeCurrent_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	entranceSlitUpperBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("entranceSlitUpperBladeCurrentDetector", "entranceSlitUpperBladeCurrentDetector", entranceSlitUpperBladeCurrent_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	teyBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("teyBladeCurrentDetector", "teyBladeCurrentDetector", teyBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	flyBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("flyBladeCurrentDetector", "flyBladeCurrentDetector", flyBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	i0EndstationBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("i0EndstationBladeCurrentDetector", "i0EndstationBladeCurrentDetector", i0EndstationBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	i0BeamlineBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("i0BeamlineBladeCurrentDetector", "i0BeamlineBladeCurrentDetector", i0BeamlineBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	photodiodeBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("photodiodeBladeCurrentDetector", "photodiodeBladeCurrentDetector", photodiodeBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
}

void PGMBeamline::setupExposedControls()
{
	addExposedControl(exitSlitLowerBladeCurrentA_);
	addExposedControl(exitSlitUpperBladeCurrentA_);
	addExposedControl(exitSlitLowerBladeCurrentB_);
	addExposedControl(exitSlitUpperBladeCurrentB_);

	addExposedControl(entranceSlitLowerBladeCurrent_);
	addExposedControl(entranceSlitUpperBladeCurrent_);

	addExposedControl(teyBladeCurrentControl_);
	addExposedControl(flyBladeCurrentControl_);
	addExposedControl(i0EndstationBladeCurrentControl_);
	addExposedControl(i0BeamlineBladeCurrentControl_);
	addExposedControl(photodiodeBladeCurrentControl_);

	addExposedControl(energy_);
}

void PGMBeamline::setupExposedDetectors()
{
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
}
