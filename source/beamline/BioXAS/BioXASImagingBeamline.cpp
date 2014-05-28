#include "BioXASImagingBeamline.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

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
