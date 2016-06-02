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
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSMAXvMotor.h"

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
	return allControls_->isConnected();
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchBPosition() const
{
	return exitSlitBranchBPosition_;
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchBGap() const
{
	return exitSlitBranchBGap_;
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchAPosition() const
{
	return exitSlitBranchAPosition_;
}

AMPVwStatusControl *PGMBeamline::exitSlitBranchAGap() const
{
	return exitSlitBranchAGap_;
}

AMPVwStatusControl *PGMBeamline::entranceSlitGap() const
{
	return entranceSlitGap_;
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
	allControls_ = new AMControlSet(this);

	allControls_->addControl(exitSlitBranchAPosition_);
	allControls_->addControl(exitSlitBranchAGap_);
	allControls_->addControl(exitSlitBranchBPosition_);
	allControls_->addControl(exitSlitBranchBGap_);
	allControls_->addControl(entranceSlitGap_);
	allControls_->addControl(energy_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));
}

void PGMBeamline::setupMono()
{

}

void PGMBeamline::setupComponents()
{
	exitSlitBranchAPosition_ = new AMPVwStatusControl("Exit Slit (A) Position", "PSL16114I2101:X:mm:fbk", "PSL16114I2101:X:mm", "SMTR16114I2104:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchAGap_ = new AMPVwStatusControl("Exit Slit (A) Gap", "PSL16114I2101:Y:mm:fbk", "PSL16114I2101:Y:mm", "SMTR16114I2105:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

	exitSlitBranchBPosition_ = new AMPVwStatusControl("Exit Slit (B) Position", "PSL16114I2201:X:mm:fbk", "PSL16114I2201:X:mm", "SMTR16114I2204:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	exitSlitBranchBGap_ = new AMPVwStatusControl("Exit Slit (B) Gap", "PSL16114I2201:Y:mm:fbk", "PSL16114I2201:Y:mm", "SMTR16114I2205:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

	entranceSlitGap_ = new AMPVwStatusControl("entranceSlit","PSL16113I2001:Y:mm:fbk", "PSL16113I2001:Y:mm", "SMTR16113I2010:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));

    energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.001, 2.0, new CLSMAXvControlStatusChecker());
    energy_->enableLimitMonitoring();
}

void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{
    addExposedControl(energy_);
}

void PGMBeamline::setupExposedDetectors()
{

}


