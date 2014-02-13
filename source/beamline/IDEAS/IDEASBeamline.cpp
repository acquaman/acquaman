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

}

void IDEASBeamline::setupControlSets()
{

}

void IDEASBeamline::setupMono()
{

}

void IDEASBeamline::setupSynchronizedDwellTime()
{

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

}

void IDEASBeamline::setupExposedDetectors()
{

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
