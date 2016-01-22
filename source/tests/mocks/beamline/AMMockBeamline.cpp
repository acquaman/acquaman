#include "AMMockBeamline.h"

#include "tests/mocks/beamline/AMMockControl.h"
#include "tests/mocks/beamline/AMMockDetector.h"
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
	mockControl2_ = new AMMockControl("Mock Control 2",
	                                  "Units",
	                                  this);
}

void AMMockBeamline::setupDetectors()
{
	mockImmediateDetector1_ = new AMMockDetector("Mock Immediate Detector 1",
	                                             "Mock Immediate Detector 1",
	                                             AMDetectorDefinitions::ImmediateRead,
	                                             this);

	mockDwellDetector1_ = new AMMockDetector("Mock Dwell Detector 1",
	                                         "Mock Dwell Detector 1",
	                                         AMDetectorDefinitions::RequestRead,
	                                         this);
}

void AMMockBeamline::setupControlSets()
{
}

void AMMockBeamline::setupExposedControls()
{
	addExposedControl(mockControl1_);
	addExposedControl(mockControl2_);
}

void AMMockBeamline::setupExposedDetectors()
{
	addExposedDetector(mockImmediateDetector1_);
	addExposedDetector(mockDwellDetector1_);
}

void AMMockBeamline::setupConnections()
{
}
