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
			beamlineStatus_ && beamlineStatus_->isConnected();

	return connected;
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
	AMControl *photonShutter1 = new AMReadOnlyPVControl("PhotonShutter1", "PSH1411-I00-01:state", this, "Photon Shutter 1");
	AMControl *photonShutter2 = new CLSExclusiveStatesControl("PhotonShutter2", "PSH1411-I00-02:state", "PSH1411-I00-02:opr:open", "PSH1411-I00-02:opr:close", this, "Photon Shutter 2");
	AMControl *photonShutter3 = new CLSExclusiveStatesControl("PhotonShutter3", "PSH1611-3-I20-01:state", "PSH1611-3-I20-01:opr:open", "PSH1611-3-I20-01:opr:close", this, "Photon Shutter 3");
	AMControl *safetyShutter1 = new CLSExclusiveStatesControl("SafetyShutter1", "SSH1411-I00-01:state",  "SSH1411-I00-01:opr:open", "SSH1411-I00-01:opr:close", this, "Safety Shutter 1");

	beamlineShutters_ = new CLSShutters(QString("PGM Shutters"), this);
	beamlineShutters_->addShutter(photonShutter1, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineShutters_->addShutter(photonShutter2, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineShutters_->addShutter(photonShutter3, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineShutters_->addShutter(safetyShutter1, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);

	// ==== set up the valves
	AMControl *branchAValve = new CLSExclusiveStatesControl("BranchAValves", "VVR1611-4-I22-04:state",  "VVR1611-4-I22-04:opr:open",  "VVR1611-4-I22-04:opr:close", this, "Branch A Valve");
	AMControl *branchBValve = new CLSExclusiveStatesControl("BranchBValves", "VVR1611-4-I21-04:state",  "VVR1611-4-I21-04:opr:open",  "VVR1611-4-I21-04:opr:close", this, "Branch B Valve");

	beamlineValves_ = new CLSValves(QString("PGM Valves"), this);
	beamlineValves_->addValve(branchAValve, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);
	beamlineValves_->addValve(branchBValve, CLSBEAMLINE_VALVE_OPEN, CLSBEAMLINE_VALVE_CLOSED);

	// ==== set up the Beamline status.
	beamlineStatus_ = new CLSBeamlineStatus("PGM BeamlineStatus", this);
	connect( beamlineStatus_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()) );

	beamlineStatus_->addShutterControl(beamlineShutters_, CLSShutters::Open);
	beamlineStatus_->addValveControl(beamlineValves_, CLSValves::Open);
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
