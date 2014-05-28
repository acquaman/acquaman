#include "BioXASMainBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

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
