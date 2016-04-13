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
//    exitSlitGapBranchA_ = new AMPVwStatusControl("exitSlitGapBranchA", "PSL16114I2101:Y:mm:fbk", "PSL16114I2101:Y:mm", QString(), QString(), this, 0.5);
//    exitSlipGapBranchB_ = new AMPVwStatusControl("exitSlitGapBranchB", "PSL16114I2201:Y:mm:fbk", "PSL16114I2201:Y:mm", QString(), QString(), this, 0.5);
//    exitPositionBranchA_ = new AMPVwStatusControl("exitPositionBranchA", "PSL16114I2101:X:mm:fbk", "PSL16114I2101:X:mm", QString(), QString(), this, 0.5);
//    exitPositionBranchB_ = new AMPVwStatusControl("exitPositionBranchA", "PSL16114I2201:X:mm:fbk", "PSL16114I2201:X:mm", QString(), QString(), this, 0.5);

    exitSlitLowerBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentA", "A1611-4-02:nA:fbk", this);
    exitSlitUpperBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentA", "A1611-4-03:nA:fbk", this);
    exitSlitLowerBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentB", "A1611-4-04:nA:fbk", this);
    exitSlitUpperBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentB", "A1611-4-05:nA:fbk", this);

}


void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{
//    addExposedControl(exitSlitGapBranchA_);
//    addExposedControl(exitSlipGapBranchB_);
//    addExposedControl(exitPositionBranchA_);
//    addExposedControl(exitPositionBranchB_);
    addExposedControl(exitSlitLowerBladeCurrentA_);
    addExposedControl(exitSlitUpperBladeCurrentA_);
    addExposedControl(exitSlitLowerBladeCurrentB_);
    addExposedControl(exitSlitUpperBladeCurrentB_);
}

void PGMBeamline::setupExposedDetectors()
{

}


PGMBeamline::~PGMBeamline()
{

}
