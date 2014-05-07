#include "BioXASSideBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

BioXASSideBeamline::BioXASSideBeamline()
	: AMBeamline("BioXAS Beamline - Side Endstation")
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

void BioXASSideBeamline::setupDiagnostics()
{

}

void BioXASSideBeamline::setupSampleStage()
{

}

void BioXASSideBeamline::setupMotorGroup()
{

}

void BioXASSideBeamline::setupDetectors()
{

}

void BioXASSideBeamline::setupControlSets()
{

}

void BioXASSideBeamline::setupMono()
{

}

void BioXASSideBeamline::setupSynchronizedDwellTime()
{

}

void BioXASSideBeamline::setupComponents()
{

}

void BioXASSideBeamline::setupControlsAsDetectors()
{

}

void BioXASSideBeamline::setupExposedControls()
{

}

void BioXASSideBeamline::setupExposedDetectors()
{

}

BioXASSideBeamline::~BioXASSideBeamline()
{

}
