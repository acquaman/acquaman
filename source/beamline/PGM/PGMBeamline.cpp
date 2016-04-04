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
    bpm10IDX_ = new AMReadOnlyPVControl("Beam Position Monitor 10ID Downstream-X", "BPM1401-05:x:um", this);
    bpm10IDY_ = new AMReadOnlyPVControl("Beam Position Monitor 10ID Downstream-Y", "BPM1401-05:y:um", this);
    bpm11ID1X_ = new AMReadOnlyPVControl("Beam Position Monitor 11ID #1-X", "BPM1411-01:x:um", this);
    bpm11ID1Y_ = new AMReadOnlyPVControl("Beam Position Monitor 11ID #1-Y", "BPM1411-01:y:um", this);
    bpm11ID2X_ = new AMReadOnlyPVControl("Beam Position Monitor 11ID #2-X", "BPM1411-02:x:um", this);
    bpm11ID2Y_ = new AMReadOnlyPVControl("Beam Position Monitor 11ID #2-Y", "BPM1411-02:y:um", this);
}

void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{

    addExposedControl(bpm10IDX_);
    addExposedControl(bpm10IDY_);
    addExposedControl(bpm11ID1X_);
    addExposedControl(bpm11ID1Y_);
    addExposedControl(bpm11ID2X_);
    addExposedControl(bpm11ID2Y_);

}

void PGMBeamline::setupExposedDetectors()
{

}


PGMBeamline::~PGMBeamline()
{

}
