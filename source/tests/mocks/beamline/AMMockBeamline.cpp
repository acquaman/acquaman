#include "AMMockBeamline.h"

#include "tests/mocks/beamline/AMMockControl.h"
#include "tests/mocks/beamline/AMMockDetector.h"
#include "beamline/AMMotorGroup.h"

AMMockBeamline::~AMMockBeamline()
{

}

bool AMMockBeamline::isConnected() const
{
	return true;
}

AMMotorGroup * AMMockBeamline::sampleStageMotorGroup() const
{
	return sampleStageMotorGroup_;
}

AMMockBeamline::AMMockBeamline() :
	AMBeamline("Mock Beamline")
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

	mockSampleXTranslation_ = new AMMockControl("Mock X Translation",
	                                            "mm",
	                                            this);
	mockSampleXRotation_ = new AMMockControl("Mock X Rotation",
	                                         "deg",
	                                         this);

	mockSampleYTranslation_ = new AMMockControl("Mock Y Translation",
	                                            "mm",
	                                            this);
	mockSampleYRotation_ = new AMMockControl("Mock Y Rotation",
	                                         "deg",
	                                         this);

	mockSampleZTranslation_ = new AMMockControl("Mock Z Translation",
	                                            "mm",
	                                            this);
	mockSampleZRotation_ = new AMMockControl("Mock Z Rotation",
	                                         "deg",
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
	sampleStageMotorGroup_ = new AMMotorGroup(this);

	AMMotorGroupObject* mockSampleStageGroup = new AMMotorGroupObject("Mock Sample Stage");
	mockSampleStageGroup->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
	                                       "X", mockSampleXTranslation_,
	                                       "X Rot", mockSampleXRotation_);

	mockSampleStageGroup->setDirectionAxis(AMMotorGroupObject::NormalMotion,
	                                       "Y", mockSampleYTranslation_,
	                                       "Y Rot", mockSampleYRotation_);

	mockSampleStageGroup->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
	                                       "Z", mockSampleZTranslation_,
	                                       "Z Rot", mockSampleZRotation_);

	sampleStageMotorGroup_->addMotorGroupObject(mockSampleStageGroup);
}

void AMMockBeamline::setupExposedControls()
{
	addExposedControl(mockControl1_);
	addExposedControl(mockControl2_);
	addExposedControl(mockSampleXTranslation_);
	addExposedControl(mockSampleXRotation_);
	addExposedControl(mockSampleYTranslation_);
	addExposedControl(mockSampleYRotation_);
	addExposedControl(mockSampleZTranslation_);
	addExposedControl(mockSampleZRotation_);
}

void AMMockBeamline::setupExposedDetectors()
{
	addExposedDetector(mockImmediateDetector1_);
	addExposedDetector(mockDwellDetector1_);
}

void AMMockBeamline::setupConnections()
{
}
