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


#include "BioXASImagingBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASImagingBeamline::BioXASImagingBeamline()
	: AMBeamline("BioXAS Beamline - Imaging Endstation")
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

void BioXASImagingBeamline::setupDiagnostics()
{

}

void BioXASImagingBeamline::setupSampleStage()
{

}

void BioXASImagingBeamline::setupMotorGroup()
{

}

void BioXASImagingBeamline::setupDetectors()
{

}

void BioXASImagingBeamline::setupControlSets()
{

}

void BioXASImagingBeamline::setupMono()
{

}

void BioXASImagingBeamline::setupSynchronizedDwellTime()
{

}

void BioXASImagingBeamline::setupComponents()
{

}

void BioXASImagingBeamline::setupControlsAsDetectors()
{

}

void BioXASImagingBeamline::setupExposedControls()
{

}

void BioXASImagingBeamline::setupExposedDetectors()
{

}

BioXASImagingBeamline::~BioXASImagingBeamline()
{

}
