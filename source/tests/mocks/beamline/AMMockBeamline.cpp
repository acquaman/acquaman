#include "AMMockBeamline.h"

#include "tests/mocks/beamline/AMMockControl.h"
AMMockBeamline::~AMMockBeamline()
{

}

bool AMMockBeamline::isConnected() const
{
	return true;
}

AMMockBeamline::AMMockBeamline() :
    CLSBeamline("Mock Beamline")
{
	setupComponents();
	setupDetectors();
	setupControlSets();
	setupExposedControls();
	setupExposedDetectors();
	setupConnections();
}

void AMMockBeamline::setupComponents()
{
	mockControl1_ = new AMMockControl("Mock Control 1",
	                                  "Units",
	                                  this);
}

void AMMockBeamline::setupDetectors()
{
}

void AMMockBeamline::setupControlSets()
{
}

void AMMockBeamline::setupExposedControls()
{
	addExposedControl(mockControl1_);
}

void AMMockBeamline::setupExposedDetectors()
{
}

void AMMockBeamline::setupConnections()
{
}
