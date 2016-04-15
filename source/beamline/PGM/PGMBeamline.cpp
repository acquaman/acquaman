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

}

void PGMBeamline::setupMono()
{

}

void PGMBeamline::setupComponents()
{
    // BPM for 10ID
    bpm10IDxControl_ = new PGMBpmControl("BPM 10ID Downstream-X", "BPM1410-05:x:um", 665, 50, this);
    bpm10IDyControl_ = new PGMBpmControl("BPM 10ID Downstream-Y", "BPM1410-05:y:um",-245, 50, this);
    // BPM from 11ID #1
    bpm11ID1xControl_ = new PGMBpmControl("BPM 11ID #1-X", "BPM1411-01:x:um", -400, 50, this);
    bpm11ID1yControl_ = new PGMBpmControl("BPM 11ID #1-Y", "BPM1411-01:y:um", -970, 50, this);
    // BPM from IID #2
    bpm11ID2xControl_ = new PGMBpmControl("BPM 11ID #2-X", "BPM1411-02:x:um", -505, 50, this);
    bpm11ID2yControl_ = new PGMBpmControl("BPM 11ID #2-Y", "BPM1411-02:y:um", -245, 50, this);

    // Storage ring
    ringCurrent_ = new AMReadOnlyPVControl("Ring Current", "PCT1402-01:mA:fbk", this);
    // Beam lifetime
    beamLifetime_ = new AMReadOnlyPVControl("Beam Lifetime", "PCT1402-01:halfLife", this);

}

void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{
    // Storage ring current
    addExposedControl(ringCurrent_);
    // Beam lifetime
    addExposedControl(beamLifetime_);
}

void PGMBeamline::setupExposedDetectors()
{

}


PGMBeamline::~PGMBeamline()
{

}
