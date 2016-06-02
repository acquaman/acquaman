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
	energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.001, 2.0, new CLSMAXvControlStatusChecker());
	energy_->enableLimitMonitoring();
}

void PGMBeamline::setupComponents()
{
	exitSlitLowerBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentA", "A1611-4-02:nA:fbk", this);
	exitSlitUpperBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentA", "A1611-4-03:nA:fbk", this);
	exitSlitLowerBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentB", "A1611-4-04:nA:fbk", this);
	exitSlitUpperBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentB", "A1611-4-05:nA:fbk", this);

	entranceSlitLowerBladeCurrent_ = new AMReadOnlyPVControl("entranceSlitLowerBladeCurrent", "A1611-3-03:nA:fbk", this);
	entranceSlitUpperBladeCurrent_ = new AMReadOnlyPVControl("entranceSlitUpperBladeCurrent", "A1611-3-04:nA:fbk", this);

	teyBladeCurrentControl_ = new AMReadOnlyPVControl("teyBladeCurrentControl", "A1611-4-09:nA:fbk", this);
	flyBladeCurrentControl_ = new AMReadOnlyPVControl("flyBladeCurrentControl", "A1611-4-08:nA:fbk", this);
	i0EndstationBladeCurrentControl_ = new AMReadOnlyPVControl("i0EndstationBladeCurrentControl", "A1611-4-11:nA:fbk", this);
	i0BeamlineBladeCurrentControl_ = new AMReadOnlyPVControl("i0BeamlineBladeCurrentControl", "A1611-4-12:nA:fbk", this);
	photodiodeBladeCurrentControl_ = new AMReadOnlyPVControl("photodiodeBladeCurrentControl", "A1611-4-10:nA:fbk", this);
}

void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{
    addExposedControl(exitSlitLowerBladeCurrentA_);
    addExposedControl(exitSlitUpperBladeCurrentA_);
    addExposedControl(exitSlitLowerBladeCurrentB_);
    addExposedControl(exitSlitUpperBladeCurrentB_);

    addExposedControl(entranceSlitLowerBladeCurrent_);
    addExposedControl(entranceSlitUpperBladeCurrent_);

    addExposedControl(teyBladeCurrentControl_);
    addExposedControl(flyBladeCurrentControl_);
    addExposedControl(i0EndstationBladeCurrentControl_);
    addExposedControl(i0BeamlineBladeCurrentControl_);
    addExposedControl(photodiodeBladeCurrentControl_);

    addExposedControl(energy_);
}

void PGMBeamline::setupExposedDetectors()
{



}


PGMBeamline::~PGMBeamline()
{

}
