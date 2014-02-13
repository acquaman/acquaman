/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "IDEASBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

#include "beamline/AMBasicControlDetectorEmulator.h"

IDEASBeamline::IDEASBeamline()
	: AMBeamline("IDEAS Beamline")
{
	setupSynchronizedDwellTime();
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

void IDEASBeamline::setupDiagnostics()
{

}

void IDEASBeamline::setupSampleStage()
{

}

void IDEASBeamline::setupMotorGroup()
{

}

void IDEASBeamline::setupDetectors()
{
	oldIonChamberAmmeter_ = new AMReadOnlyPVControl("OldIon", "A1608-10-01:A:fbk", this, "Old Ion Chamber");
	oxfordI0IonChamberAmmeter_ = new AMReadOnlyPVControl("I0", "A1608-10-05:A:fbk", this, "I0 Ion Chamber");
	oxfordSampleIonChamberAmmeter_ = new AMReadOnlyPVControl("Sample", "A1608-10-06:A:fbk", this, "Sample Ion Chamber");
	oxfordReferenceIonChamberAmmeter_ = new AMReadOnlyPVControl("Reference", "A1608-10-07:A:fbk", this, "Reference Ion Chamber");

	oldIonChamberDetector_ = new AMBasicControlDetectorEmulator("OldIonDetector", "Old Ion Chamber Detector", oldIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
	oxfordI0IonChamberDetector_ = new AMBasicControlDetectorEmulator("I0Detector", "I0 Detector", oxfordI0IonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
	oxfordSampleIonChamberDetector_ = new AMBasicControlDetectorEmulator("SampleDetector", "Sample Detector", oxfordSampleIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
	oxfordReferenceIonChamberDetector_ = new AMBasicControlDetectorEmulator("ReferenceDetector", "Reference Detector", oxfordReferenceIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, this);
}

void IDEASBeamline::setupControlSets()
{

}

void IDEASBeamline::setupMono()
{
	monoEnergy_ = new AMPVwStatusControl("Energy", "BL08B2-1:Energy:EV:fbk", "BL08B2-1:Energy:EV", "BL08B2-1:Energy:status", "BL08B2-1:Energy:stop", this, 0.5, 2.0, new AMControlStatusCheckerDefault(1), 1, "Mono Energy");
}

void IDEASBeamline::setupSynchronizedDwellTime()
{
	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL08B2-1:dwell", this);
	synchronizedDwellTime_->addElement(0);
}

void IDEASBeamline::setupComponents()
{

}

void IDEASBeamline::setupControlsAsDetectors()
{

}

void IDEASBeamline::setupExposedControls()
{

}

void IDEASBeamline::setupExposedDetectors()
{
	addExposedDetector(oldIonChamberDetector_);
	addExposedDetector(oxfordI0IonChamberDetector_);
	addExposedDetector(oxfordSampleIonChamberDetector_);
	addExposedDetector(oxfordReferenceIonChamberDetector_);
}


IDEASBeamline::~IDEASBeamline()
{

}
