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
	exitSlitLowerBladeCurrentADetector_ = new PGMPicoAmmeter("exitSlitLowerBladeCurrentA", "Exit Slit Lower A", "A1611-4-02:nA:fbk", this);
	exitSlitUpperBladeCurrentADetector_ = new PGMPicoAmmeter("exitSlitUpperBladeCurrentA", "Exit Slit Upper A", "A1611-4-03:nA:fbk", this);
	exitSlitLowerBladeCurrentBDetector_ = new PGMPicoAmmeter("exitSlitLowerBladeCurrentB", "Exit Slit Lower B", "A1611-4-04:nA:fbk", this);
	exitSlitUpperBladeCurrentBDetector_ = new PGMPicoAmmeter("exitSlitUpperBladeCurrentB", "Exit Slit Upper B", "A1611-4-05:nA:fbk", this);

	entranceSlitLowerBladeCurrentDetector_ = new PGMPicoAmmeter("entranceSlitLowerBladeCurrent", "Entrance Slit Lower", "A1611-3-03:nA:fbk", this);
	entranceSlitUpperBladeCurrentDetector_ = new PGMPicoAmmeter("entranceSlitUpperBladeCurrent", "Entrance Slit Lower", "A1611-3-04:nA:fbk", this);

	teyBladeCurrentDetector_ = new PGMPicoAmmeter("teyBladeCurrentDetector", "TEY", "A1611-4-09:nA:fbk", this);
	flyBladeCurrentDetector_ = new PGMPicoAmmeter("flyBladeCurrentDetector", "FLY", "A1611-4-08:nA:fbk", this);
	i0EndstationBladeCurrentDetector_ = new PGMPicoAmmeter("i0EndstationBladeCurrentDetector", "Endstation I0", "A1611-4-11:nA:fbk", this);
	i0BeamlineBladeCurrentDetector_ = new PGMPicoAmmeter("i0BeamlineBladeCurrentDetector", "Beamline I0", "A1611-4-12:nA:fbk", this);
	photodiodeBladeCurrentDetector_ = new PGMPicoAmmeter("photodiodeBladeCurrentDetector", "Photodiode" , "A1611-4-10:nA:fbk", this);
}

void PGMBeamline::setupControlSets()
{

}

void PGMBeamline::setupMono()
{
	energy_ = new AMPVwStatusControl("Energy", "BL1611-ID-2:Energy:fbk", "BL1611-ID-2:Energy", "BL1611-ID-2:status", "PGM_mono:emergStop", this, 0.01);
	energy_->enableLimitMonitoring();
}

void PGMBeamline::setupComponents()
{
	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1611-ID-2:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);
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
