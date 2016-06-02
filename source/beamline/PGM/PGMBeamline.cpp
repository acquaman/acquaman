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
	exitSlitLowerBladeCurrentA_->setDescription("Exit Slit Lower A");
	exitSlitUpperBladeCurrentA_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentA", "A1611-4-03:nA:fbk", this);
	exitSlitUpperBladeCurrentA_->setDescription("Exit Slit Upper A");
	exitSlitLowerBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitLowerBladeCurrentB", "A1611-4-04:nA:fbk", this);
	exitSlitLowerBladeCurrentB_->setDescription("Exit Slit Lower B");
	exitSlitUpperBladeCurrentB_ = new AMReadOnlyPVControl("exitSlitUpperBladeCurrentB", "A1611-4-05:nA:fbk", this);
	exitSlitUpperBladeCurrentB_->setDescription("Exit Slit Upper B");

	entranceSlitLowerBladeCurrent_ = new AMReadOnlyPVControl("entranceSlitLowerBladeCurrent", "A1611-3-03:nA:fbk", this);
	entranceSlitLowerBladeCurrent_->setDescription("Entrance Slit Lower");
	entranceSlitUpperBladeCurrent_ = new AMReadOnlyPVControl("entranceSlitUpperBladeCurrent", "A1611-3-04:nA:fbk", this);
	entranceSlitUpperBladeCurrent_->setDescription("Entrance Slit Lower");

	teyBladeCurrentControl_ = new AMReadOnlyPVControl("teyBladeCurrentControl", "A1611-4-09:nA:fbk", this);
	teyBladeCurrentControl_->setDescription("TEY");
	flyBladeCurrentControl_ = new AMReadOnlyPVControl("flyBladeCurrentControl", "A1611-4-08:nA:fbk", this);
	flyBladeCurrentControl_->setDescription("FLY");
	i0EndstationBladeCurrentControl_ = new AMReadOnlyPVControl("i0EndstationBladeCurrentControl", "A1611-4-11:nA:fbk", this);
	i0EndstationBladeCurrentControl_->setDescription("Endstation I0");
	i0BeamlineBladeCurrentControl_ = new AMReadOnlyPVControl("i0BeamlineBladeCurrentControl", "A1611-4-12:nA:fbk", this);
	i0BeamlineBladeCurrentControl_->setDescription("Beamline I0");
	photodiodeBladeCurrentControl_ = new AMReadOnlyPVControl("photodiodeBladeCurrentControl", "A1611-4-10:nA:fbk", this);
	photodiodeBladeCurrentControl_->setDescription("Photodiode");
}

void PGMBeamline::setupControlsAsDetectors()
{
	exitSlitLowerBladeCurrentADetector_ = new AMBasicControlDetectorEmulator("exitSlitLowerBladeCurrentADetector", "exitSlitLowerBladeCurrentADetector", exitSlitLowerBladeCurrentA_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	exitSlitUpperBladeCurrentADetector_ = new AMBasicControlDetectorEmulator("exitSlitUpperBladeCurrentADetector", "exitSlitUpperBladeCurrentADetector", exitSlitUpperBladeCurrentA_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	exitSlitLowerBladeCurrentBDetector_ = new AMBasicControlDetectorEmulator("exitSlitLowerBladeCurrentBDetector", "exitSlitLowerBladeCurrentBDetector", exitSlitLowerBladeCurrentB_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	exitSlitUpperBladeCurrentBDetector_ = new AMBasicControlDetectorEmulator("exitSlitUpperBladeCurrentBDetector", "exitSlitUpperBladeCurrentBDetector", exitSlitUpperBladeCurrentB_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	entranceSlitLowerBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("entranceSlitLowerBladeCurrentDetector", "entranceSlitLowerBladeCurrentDetector", entranceSlitLowerBladeCurrent_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	entranceSlitUpperBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("entranceSlitUpperBladeCurrentDetector", "entranceSlitUpperBladeCurrentDetector", entranceSlitUpperBladeCurrent_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	teyBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("teyBladeCurrentDetector", "teyBladeCurrentDetector", teyBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	flyBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("flyBladeCurrentDetector", "flyBladeCurrentDetector", flyBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	i0EndstationBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("i0EndstationBladeCurrentDetector", "i0EndstationBladeCurrentDetector", i0EndstationBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	i0BeamlineBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("i0BeamlineBladeCurrentDetector", "i0BeamlineBladeCurrentDetector", i0BeamlineBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	photodiodeBladeCurrentDetector_ = new AMBasicControlDetectorEmulator("photodiodeBladeCurrentDetector", "photodiodeBladeCurrentDetector", photodiodeBladeCurrentControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
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
	addExposedDetector(exitSlitLowerBladeCurrentADetector_);
	addExposedDetector(exitSlitUpperBladeCurrentADetector_);
	addExposedDetector(exitSlitLowerBladeCurrentBDetector_);
	addExposedDetector(exitSlitUpperBladeCurrentBDetector_);

	addExposedDetector(entranceSlitLowerBladeCurrentDetector_);
	addExposedDetector(entranceSlitUpperBladeCurrentDetector_);

	addExposedDetector(teyBladeCurrentDetector_);
	addExposedDetector(flyBladeCurrentDetector_);
	addExposedDetector(i0EndstationBladeCurrentDetector_);
	addExposedDetector(i0BeamlineBladeCurrentDetector_);
	addExposedDetector(photodiodeBladeCurrentDetector_);
}


PGMBeamline::~PGMBeamline()
{

}
