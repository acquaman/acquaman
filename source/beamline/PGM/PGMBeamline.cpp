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
	bool connected =
			 photonShutter1_ && photonShutter1_->isConnected()
			&& photonShutter2_ && photonShutter2_->isConnected()
			&& photonShutter3_ && photonShutter3_->isConnected()
			&& safetyShutter1_ && safetyShutter1_->isConnected()
			&& valveSet_ && valveSet_->isConnected();

	return connected;
}

void PGMBeamline::togglePhotonShutter2()
{
	if ( !photonShutter2_ )
		return;

	if (photonShutter2_->isOpen()) {

		photonShutter2_->close();
	} else if (photonShutter2_->isClosed() && (!branchAOpen() && !branchBOpen())) {

		photonShutter2_->open();
	}
}

void PGMBeamline::togglePhotonShutter3()
{
	if ( !photonShutter3_ )
		return;

	if (photonShutter3_->isOpen()) {

		photonShutter3_->close();
	} else if (photonShutter3_->isClosed() && (photonShutter1_->value() == 4 && photonShutter2_->isOpen() && safetyShutter1_->isOpen()) && !(branchAOpen() || branchBOpen())) {

		photonShutter3_->open();
	}
}

void PGMBeamline::toggleSafetyShutter1()
{
	if ( !safetyShutter1_ )
		return;

	if (safetyShutter1_->isOpen())
		safetyShutter1_->close();
	else if (safetyShutter1_->isClosed())
		safetyShutter1_->open();
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

}

void PGMBeamline::setupMono()
{

}

void PGMBeamline::setupComponents()
{
	// ==== set up the shutters
	// Photon Shutters
	photonShutter1_ = new AMReadOnlyPVControl("PhotonShutter1", "PSH1411-I00-01:state", this, "Photon Shutter 1");
	connect(photonShutter1_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));

    photonShutter2_ = new CLSExclusiveStatesControl("PhotonShutter2", "PSH1411-I00-02:state", "PSH1411-I00-02:opr:open", "PSH1411-I00-02:opr:close", this);
    photonShutter2_->setDescription("Photon Shutter 2");
	connect(photonShutter2_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));

    photonShutter3_ = new CLSExclusiveStatesControl("PhotonShutter3", "PSH1611-3-I20-01:state", "PSH1611-3-I20-01:opr:open", "PSH1611-3-I20-01:opr:close", this);
    photonShutter3_->setDescription("Photon Shutter 3");
	connect(photonShutter3_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));

    // Safety Shutter
    safetyShutter1_ = new CLSExclusiveStatesControl("SafetyShutter1", "SSH1411-I00-01:state",  "SSH1411-I00-01:opr:open", "SSH1411-I00-01:opr:close", this);
    safetyShutter1_->setDescription("Safety Shutter 1");
	connect(safetyShutter1_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));

	// ==== set up the valves
	// Branch A Valves
    valveBranchA_ = new CLSExclusiveStatesControl("VVR16114I2204", "VVR1611-4-I22-04:state",  "VVR1611-4-I22-04:opr:open",  "VVR1611-4-I22-04:opr:close", this);
    valveBranchA_->setDescription("Branch A Valve");

    // Branch B Valves
    valveBranchB_ = new CLSExclusiveStatesControl("VVR16114I2104", "VVR1611-4-I21-04:state",  "VVR1611-4-I21-04:opr:open",  "VVR1611-4-I21-04:opr:close", this);
    valveBranchB_->setDescription("Branch B Valve");

	valveSet_ = new AMControlSet(this);
	valveSet_->addControl(valveBranchA_);
	valveSet_->addControl(valveBranchB_);
	connect(valveSet_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));
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

bool PGMBeamline::branchAOpen() const
{
	return valveBranchA_ && valveBranchA_->isOpen();
}

bool PGMBeamline::branchBOpen() const
{
	return valveBranchB_ && valveBranchB_->isOpen();
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







