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
}

void PGMBeamline::setConnected(bool newState)
{
	if (connected_ != newState) {
		connected_ = newState;
		emit connected(newState);
	}
}

void PGMBeamline::updateConnected()
{
	setConnected( getConnectedState() );
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
    energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.001, 2.0, new CLSMAXvControlStatusChecker());
    energy_->enableLimitMonitoring();
	connect( energy_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	oceanOpticsDetector_ = new PGMOceanOpticsXRFDetector("OceanOpticsDetector", "Ocean Optics XRF Detector", this);
	connect( oceanOpticsDetector_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
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
	addExposedDetector(oceanOpticsDetector_);
}

bool PGMBeamline::getConnectedState() const
{
	bool connected = (
				energy_ && energy_->isConnected() &&
				oceanOpticsDetector_ && oceanOpticsDetector_->isConnected()
				);

	return connected;
}


PGMBeamline::~PGMBeamline()
{

}
