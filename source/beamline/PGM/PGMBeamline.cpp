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
    valveSet_ = new AMControlSet(this);
    valveSet_->addControl(valveBranchA_);
    valveSet_->addControl(valveBranchB_);

}

void PGMBeamline::setupMono()
{

}

void PGMBeamline::setupComponents()
{
    // Photon Shutters
    photonShutter1_ = new AMReadOnlyPVControl("PhotonShutter1", "PSH1411-I00-01:state", this);
    photonShutter1_->setDescription("Photon Shutter 1");

    photonShutter2_ = new CLSExclusiveStatesControl("PhotonShutter2", "PSH1411-I00-02:state", "PSH1411-I00-02:opr:open", "PSH1411-I00-02:opr:close", this);
    photonShutter2_->setDescription("Photon Shutter 2");

    photonShutter3_ = new CLSExclusiveStatesControl("PhotonShutter3", "PSH1611-3-I20-01:state", "PSH1611-3-I20-01:opr:open", "PSH1611-3-I20-01:opr:close", this);
    photonShutter3_->setDescription("Photon Shutter 3");

    // Safety Shutter
    safetyShutter1_ = new CLSExclusiveStatesControl("SafetyShutter1", "SSH1411-I00-01:state",  "SSH1411-I00-01:opr:open", "SSH1411-I00-01:opr:close", this);
    safetyShutter1_->setDescription("Safety Shutter 1");

    // Branch A Valves
    valveBranchA_ = new CLSExclusiveStatesControl("VVR16114I2204", "VVR1611-4-I22-04:state",  "VVR1611-4-I22-04:opr:open",  "VVR1611-4-I22-04:opr:close", this);
    valveBranchA_->setDescription("Branch A Valve");

    // Branch B Valves
    valveBranchB_ = new CLSExclusiveStatesControl("VVR16114I2104", "VVR1611-4-I21-04:state",  "VVR1611-4-I21-04:opr:open",  "VVR1611-4-I21-04:opr:close", this);
    valveBranchB_->setDescription("Branch B Valve");

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

bool PGMBeamline::openPhotonShutter3(){

    // Ensure PSH1, PSH2, SSH1 are open and both valves are closed.
    if(int(photonShutter1_->value() == 4) && photonShutter2_->isOpen() && safetyShutter1_->isOpen()
            && !(branchAOpen() || branchBOpen())){
        photonShutter3_->open();
        return true;
    }

    return false;
}

bool PGMBeamline::closePhotonShutter3(){

    if(photonShutter3_->isOpen()){
        photonShutter3_->close();
        return true;
    }
    return false;
}


bool PGMBeamline::openSafetyShutter1(){

    // Ensure that PSH1 and PHS2 are open and both valves are closed.
    if( int(photonShutter1_->value()) == 4 && photonShutter2_->isOpen()
            && (!branchAOpen() && !branchBOpen())){
        photonShutter2_->open();
        return true;
    }
    return false;
}

bool PGMBeamline::closeSafetyShutter1(){

    if(safetyShutter1_->isOpen()){
        safetyShutter1_->close();
        return true;
    }
    return false;
}

bool PGMBeamline::openPhotonShutter2(){

    if( int(photonShutter1_->value()) == 4 && (!branchAOpen() && !branchBOpen())){
        photonShutter2_->open();
        return true;
    }
    return false;
}

bool PGMBeamline::closePhotonShutter2(){

    if(photonShutter2_->isOpen()){
        photonShutter2_->close();
        return true;
    }
    return false;
}


bool PGMBeamline::branchAOpen() const{

    CLSExclusiveStatesControl *control = qobject_cast<CLSExclusiveStatesControl *>(valveSet_->at(0));

    if (control && control->isOpen())
        return true;

    return false;
}

bool PGMBeamline::branchBOpen() const{
    CLSExclusiveStatesControl *control = qobject_cast<CLSExclusiveStatesControl *>(valveSet_->at(1));

    if (control && control->isOpen())
        return true;

    return false;
}

void PGMBeamline::openValve(int index)
{
    if (index > 0 && index < valveSet_->count()){

        CLSExclusiveStatesControl *control = qobject_cast<CLSExclusiveStatesControl *>(valveSet_->at(index));

        if (control && control->isClosed())
            control->open();
    }
}

void PGMBeamline::closeValve(int index)
{
    if (index > 0 && index < valveSet_->count()){

        CLSExclusiveStatesControl *control = qobject_cast<CLSExclusiveStatesControl *>(valveSet_->at(index));

        if (control && control->isOpen())
            control->close();
    }
}







PGMBeamline::~PGMBeamline()
{

}
