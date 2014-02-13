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

#include "IDEASBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

#include "beamline/AMBasicControlDetectorEmulator.h"

IDEASBeamline::IDEASBeamline()
	: AMBeamline("IDEAS Beamline")
{
	setupSynchronizedDwellTime();
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
}

void IDEASBeamline::setupDiagnostics()
{

}

void IDEASBeamline::setupSampleStage()
{

}

void IDEASBeamline::setupMotorGroup()
{

}

void IDEASBeamline::setupDetectors()
{
	oldIonChamberAmmeter_ = new AMReadOnlyPVControl("OldIon", "A1608-10-01:A:fbk", this, "Old Ion Chamber");
	oxfordI0IonChamberAmmeter_ = new AMReadOnlyPVControl("I0", "A1608-10-05:A:fbk", this, "I0 Ion Chamber");
	oxfordSampleIonChamberAmmeter_ = new AMReadOnlyPVControl("Sample", "A1608-10-06:A:fbk", this, "Sample Ion Chamber");
	oxfordReferenceIonChamberAmmeter_ = new AMReadOnlyPVControl("Reference", "A1608-10-07:A:fbk", this, "Reference Ion Chamber");

	oldIonChamberDetector_ = new AMBasicControlDetectorEmulator("OldIonDetector", "Old Ion Chamber Detector", oldIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
	oxfordI0IonChamberDetector_ = new AMBasicControlDetectorEmulator("I0Detector", "I0 Detector", oxfordI0IonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
	oxfordSampleIonChamberDetector_ = new AMBasicControlDetectorEmulator("SampleDetector", "Sample Detector", oxfordSampleIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
	oxfordReferenceIonChamberDetector_ = new AMBasicControlDetectorEmulator("ReferenceDetector", "Reference Detector", oxfordReferenceIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
}

void IDEASBeamline::setupControlSets()
{

}

void IDEASBeamline::setupMono()
{
    monoEnergy_ = new IDEASMonochromatorControl(this);
}

void IDEASBeamline::setupSynchronizedDwellTime()
{
	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL08B2-1:dwell", this);
	synchronizedDwellTime_->addElement(0);

	masterDwell_ = new AMPVControl("masterDwell", "BL08B2-1:dwell:setTime", "BL08B2-1:dwell:setTime", QString(), this, 0.5);
}

void IDEASBeamline::setupComponents()
{
    safetyShutter_ = new CLSBiStateControl("Safety Shutter", "The primary safety shutter for the beamline.", "SSH1409-B20-01:state", "SSH1409-B20-01:opr:open", "SSH1409-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
    photonShutter2_ = new CLSBiStateControl("Photon Shutter 2", "The second photon shutter for the beamline.The primary safety shutter for the beamline.", "PSH1409-B20-02:state", "PSH1409-B20-02:opr:open", "PSH1409-B20-02:opr:close", new AMControlStatusCheckerDefault(2), this);

    connect(safetyShutter_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));
    connect(photonShutter2_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));
}

void IDEASBeamline::setupControlsAsDetectors()
{

}

void IDEASBeamline::setupExposedControls()
{
	addExposedControl(monoEnergy_);
	addExposedControl(masterDwell_);
}

void IDEASBeamline::setupExposedDetectors()
{
	addExposedDetector(oldIonChamberDetector_);
	addExposedDetector(oxfordI0IonChamberDetector_);
	addExposedDetector(oxfordSampleIonChamberDetector_);
	addExposedDetector(oxfordReferenceIonChamberDetector_);
}


IDEASBeamline::~IDEASBeamline()
{

}

AMAction3 *IDEASBeamline::createBeamOnAction() const
{
    // The correct order for turning the beam on is turning on the safety shutter and then the second photon shutter.
    AMSequentialListAction3 *beamOnAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam on action.", "The beam on action."));

    AMControlInfo setpoint = safetyShutter_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, safetyShutter_);
    beamOnAction->addSubAction(action);

    setpoint = photonShutter2_->toInfo();
    setpoint.setValue(1);
    actionInfo = new AMControlMoveActionInfo3(setpoint);
    action = new AMControlMoveAction3(actionInfo, photonShutter2_);
    beamOnAction->addSubAction(action);

    return beamOnAction;
}

AMAction3 *IDEASBeamline::createBeamOffAction() const
{
    // The correct order for turning the beam off is turning off the second photon shutter and then the safety shutter.
    AMSequentialListAction3 *beamOffAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam off action.", "The beam off action."));

    AMControlInfo setpoint = photonShutter2_->toInfo();
    setpoint.setValue(0);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, photonShutter2_);
    beamOffAction->addSubAction(action);

    setpoint = safetyShutter_->toInfo();
    setpoint.setValue(0);
    actionInfo = new AMControlMoveActionInfo3(setpoint);
    action = new AMControlMoveAction3(actionInfo, safetyShutter_);
    beamOffAction->addSubAction(action);

    return beamOffAction;
}

bool IDEASBeamline::shuttersOpen() const
{
    return safetyShutter_->isOpen() && photonShutter2_->isOpen();
}

void IDEASBeamline::onShutterStatusChanged()
{
    emit overallShutterStatus(safetyShutter_->isOpen() && photonShutter2_->isOpen());
}
