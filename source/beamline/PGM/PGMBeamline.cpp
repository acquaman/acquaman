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

PGMBeamline::PGMBeamline()
	: CLSBeamline("PGM Beamline")
{
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

void PGMBeamline::setupDiagnostics()
{

    // Photon Shutters
    PSH1411I0001Shutter1_ = new CLSExclusiveStatesControl("PhotonShutter1", "PSH1411-I00-01:state", "PSH1411-I00-01:opr:open", "PSH1411-I00-01:opr:close", this);
    PSH1411I0001Shutter1_->setDescription("Photon Shutter 1");

    PSH1411I0002Shutter2_ = new CLSExclusiveStatesControl("PhotonShutter2", "PSH1411-I00-02:state", "PSH1411-I00-02:opr:open", "PSH1411-I00-02:opr:close", this);
    PSH1411I0002Shutter2_->setDescription("Photon Shutter 2");

    PSH16113I2001Shutter3_ = new CLSExclusiveStatesControl("PhotonShutter3", "PSH1611-3-I20-01:state", "PSH1611-3-I20-01:opr:open", "PSH1611-3-I20-01:opr:close", this);
    PSH16113I2001Shutter3_->setDescription("Photon Shutter 3");

    // Safety Shutter
    SSH1411I0001Shutter1_ = new CLSExclusiveStatesControl("SafetyShutter1", "SSH1411-I00-01:state",  "SSH1411-I00-01:opr:open", "SSH1411-I00-01:opr:close", this);
    SSH1411I0001Shutter1_->setDescription("Safety Shutter 1");

    // Valves
    VVR16113I0003Valve_ = new CLSExclusiveStatesControl("VVR16113I0003", "VVR1611-3-I00-03:state", "VVR1611-3-I00-03:opr:open", "VVR1611-3-I00-03:opr:close", this);
    VVR16113I0003Valve_->setDescription("VVR1611-3-I00-03 Valve");

    VVR16113I2001Valve_ = new CLSExclusiveStatesControl("VVR16113I2001", "VVR1611-3-I20-01:state", "VVR1611-3-I20-01:opr:open",  "VVR1611-3-I20-01:opr:close", this);
    VVR16113I2001Valve_->setDescription("VVR1611-3-I20-01 Valve");

    VVR16113I2002Valve_ = new CLSExclusiveStatesControl("VVR16113I2002", "VVR1611-3-I20-02:state", "VVR1611-3-I20-02:opr:open", "VVR1611-3-I20-02:opr:close", this);
    VVR16113I2002Valve_->setDescription("VVR1611-3-I20-02 Valve");

    VVR16114I2001Valve_ = new CLSExclusiveStatesControl("VVR16114I2001", "VVR1611-4-I20-01:state", "VVR1611-4-I20-01:opr:open", "VVR1611-4-I20-01:opr:close", this);
    VVR16114I2001Valve_->setDescription("VVR1611-4-I20-01 Valve");

    VVR16114I2002Valve_ = new CLSExclusiveStatesControl("VVR16114I2002", "VVR1611-4-I20-02:state", "VVR1611-4-I20-02:opr:open", "VVR1611-4-I20-02:opr:close", this);
    VVR16114I2002Valve_->setDescription("VVR1611-4-I20-02 Valve");

    VVR16114I2003Valve_ = new CLSExclusiveStatesControl("VVR16114I2003", "VVR1611-4-I20-03:state", "VVR1611-4-I20-03:opr:open", "VVR1611-4-I20-03:opr:close", this);
    VVR16114I2003Valve_->setDescription("VVR16114-I20-03 Valve");

    // Branch A Valves
    VVR16114I2201Valve_ = new CLSExclusiveStatesControl("VVR16114I2201", "VVR1611-4-I22-01:state",  "VVR1611-4-I22-01:opr:open",  "VVR1611-4-I22-01:opr:close", this);
    VVR16114I2201Valve_->setDescription("VVR1611-4-I22-01 Valve");

    VVR16114I2202Valve_ = new CLSExclusiveStatesControl("VVR16114I2202", "VVR1611-4-I22-02:state",  "VVR1611-4-I22-02:opr:open",  "VVR1611-4-I22-02:opr:close", this);
    VVR16114I2202Valve_->setDescription("VVR1611-4-I22-02 Valve");

    VVR16114I2203Valve_ = new CLSExclusiveStatesControl("VVR16114I2203", "VVR1611-4-I22-03:state",  "VVR1611-4-I22-03:opr:open",  "VVR1611-4-I22-03:opr:close", this);
    VVR16114I2203Valve_->setDescription("VVR1611-4-I22-03 Valve");

    VVR16114I2204Valve_ = new CLSExclusiveStatesControl("VVR16114I2204", "VVR1611-4-I22-04:state",  "VVR1611-4-I22-04:opr:open",  "VVR1611-4-I22-04:opr:close", this);
    VVR16114I2204Valve_->setDescription("VVR1611-4-I22-04 Valve");

    // Branch B Valves
    VVR16114I2101Valve_ = new CLSExclusiveStatesControl("VVR16114I2101", "VVR1611-4-I21-01:state",  "VVR1611-4-I21-01:opr:open",  "VVR1611-4-I21-01:opr:close", this);
    VVR16114I2101Valve_->setDescription("VVR1611-4-I21-01 Valve");

    VVR16114I2102Valve_ = new CLSExclusiveStatesControl("VVR16114I2102", "VVR1611-4-I21-02:state",  "VVR1611-4-I21-02:opr:open",  "VVR1611-4-I21-02:opr:close", this);
    VVR16114I2102Valve_->setDescription("VVR1611-4-I21-02 Valve");

    VVR16114I2103Valve_ = new CLSExclusiveStatesControl("VVR16114I2103", "VVR1611-4-I21-03:state",  "VVR1611-4-I21-03:opr:open",  "VVR1611-4-I21-03:opr:close", this);
    VVR16114I2103Valve_->setDescription("VVR1611-4-I21-03 Valve");

    VVR16114I2104Valve_ = new CLSExclusiveStatesControl("VVR16114I2104", "VVR1611-4-I21-04:state",  "VVR1611-4-I21-04:opr:open",  "VVR1611-4-I21-04:opr:close", this);
    VVR16114I2104Valve_->setDescription("VVR1611-4-I21-04 Valve");

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

}

void PGMBeamline::setupMono()
{

}

void PGMBeamline::setupComponents()
{

}

void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{

}

void PGMBeamline::setupExposedDetectors()
{

}


PGMBeamline::~PGMBeamline()
{

}
