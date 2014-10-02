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


#include "BioXASMainBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASMainBeamline::BioXASMainBeamline()
	: AMBeamline("BioXAS Beamline - Main Endstation")
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

void BioXASMainBeamline::setupDiagnostics()
{

}

void BioXASMainBeamline::setupSampleStage()
{

}

void BioXASMainBeamline::setupMotorGroup()
{

}

void BioXASMainBeamline::setupDetectors()
{

}

void BioXASMainBeamline::setupControlSets()
{

}

void BioXASMainBeamline::setupMono()
{

}

void BioXASMainBeamline::setupSynchronizedDwellTime()
{

}

void BioXASMainBeamline::setupComponents()
{

}

void BioXASMainBeamline::setupControlsAsDetectors()
{

}

void BioXASMainBeamline::setupExposedControls()
{

}

void BioXASMainBeamline::setupExposedDetectors()
{

}

BioXASMainBeamline::~BioXASMainBeamline()
{

}
