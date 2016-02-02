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

#include "IDEASBeamline.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

#include "beamline/AMAdvancedControlDetectorEmulator.h"


IDEASBeamline::IDEASBeamline()
	: CLSBeamline("IDEAS Beamline")
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

void IDEASBeamline::setupDiagnostics()
{
	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent","PCT1402-01:mA:fbk", this, "Storage Ring Current");
	i0Current_ = new AMReadOnlyPVControl("I0Current","A1608-10-01:A:fbk", this, "I0 Current");
	sampleTemp_ = new AMReadOnlyPVControl("sampleTemp","TC1608-10-02:reading", this, "Sample Temperature");
}

void IDEASBeamline::setupSampleStage()
{
	samplePlatformVertical_ = new AMPVwStatusControl("Sample Platform Vertical","SMTR1608-10-B20-05:mm:sp","SMTR1608-10-B20-05:mm","SMTR1608-10-B20-05:status","SMTR1608-10-B20-05:stop",this,0.1);
	samplePlatformHorizontal_ = new AMPVwStatusControl("Sample Platform Horizontal","SMTR1608-10-B20-06:mm:sp","SMTR1608-10-B20-06:mm","SMTR1608-10-B20-06:status","SMTR1608-10-B20-06:stop",this,0.1);
	vacuumSampleStage_ = new AMPVwStatusControl("Vacuum Stage Position","SMTR1608-10-B20-08:mm:sp","SMTR1608-10-B20-08:mm","SMTR1608-10-B20-08:status","SMTR1608-10-B20-08:stop",this,0.1);

}

void IDEASBeamline::setupMotorGroup()
{
	motorGroup_ = new AMMotorGroup(this);

	// Set up sample platform motor object:
	AMMotorGroupObject* samplePlatformObject = new AMMotorGroupObject("Sample Platform", this);

	samplePlatformObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
								  "X", samplePlatformHorizontal_,
								  "", 0);

	samplePlatformObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
								  "Z", samplePlatformVertical_,
								  "", 0);

	motorGroup_->addMotorGroupObject(samplePlatformObject);

	// Set up vacuum stage motor object:
	AMMotorGroupObject* vacuumStageObject = new AMMotorGroupObject("Vacuum Stage", this);

	vacuumStageObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
							   "Z", vacuumSampleStage_,
							   "", 0);

	motorGroup_->addMotorGroupObject(vacuumStageObject);
}

void IDEASBeamline::setupDetectors()
{
	ge13Element_ = 0;
	ge13ElementRealTimeControl_ = 0;
	ge13ElementRealTime_ = 0;

	ketek_ = new IDEASKETEKDetector("KETEK", "Single Element XRF Detector", this);
	addSynchronizedXRFDetector(ketek_);

	ge13Element_ = new IDEAS13ElementGeDetector("13-el Ge", "The thirteen element Germanium Detector", this);
	addSynchronizedXRFDetector(ge13Element_);
	ge13ElementRealTimeControl_ = new AMReadOnlyPVControl("13-el Ge Real Time", "dxp1608-B21-13:ElapsedReal", this);
	ge13ElementRealTime_ = new AMBasicControlDetectorEmulator("13E_dwellTime", "13-element Ge dwell time", ge13ElementRealTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	ketekPeakingTime_ = new AMPVControl("XRF1E Peaking Time","dxp1608-1002:dxp1:PeakingTime_RBV","dxp1608-1002:dxp1:PeakingTime", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
	ketekTriggerLevel_ = new AMPVControl("XRF1E Trigger Level","dxp1608-1002:dxp1:TriggerThreshold_RBV","dxp1608-1002:dxp1:TriggerThreshold", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
	ketekBaselineThreshold_ = new AMPVControl("XRF1E Baseline Threshold","dxp1608-1002:dxp1:BaselineThreshold_RBV","dxp1608-1002:dxp1:BaselineThreshold", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
	ketekPreampGain_ = new AMPVControl("XRF1E Preamp Gain","dxp1608-1002:dxp1:PreampGain_RBV","dxp1608-1002:dxp1:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);

	ketekRealTimeControl_ = new AMReadOnlyPVControl("XRF1E Real Time", "dxp1608-1002:mca1.ERTM", this);

	ketekRealTime_ = new AMBasicControlDetectorEmulator("dwellTime", "Dwell Time", ketekRealTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);



	i0IonChamberScaler_ = new CLSBasicScalerChannelDetector("I_0","I_0 Ion Chamber", scaler_, 0, this);
	sampleIonChamberScaler_ = new CLSBasicScalerChannelDetector("Sample","Sample Ion Chamber", scaler_, 1, this);
	referenceIonChamberScaler_  = new CLSBasicScalerChannelDetector("Reference","Reference Ion Chamber", scaler_, 2, this);
}

void IDEASBeamline::setupControlSets()
{

}

void IDEASBeamline::setupMono()
{
	monoEnergy_ = new IDEASMonochromatorControl(this);
	monoEnergy_->setSettlingTime(0.25); //HACK

	monoDirectEnergy_ = new IDEASDirectMonochromatorControl(this);

	monoDirectEnergy_->setSettlingTime(3);

	monoCrystal_ = new AMReadOnlyPVControl("monoCrystal","BL08B2-1:Crystal:Type",this,"Mono Crystals");
	monoHighEV_ = new AMReadOnlyPVControl("monoHighEV", "BL08B2-1:Crystal:EvHigh", this, "Maximum Energy for current Crystal");
	monoLowEV_ = new AMReadOnlyPVControl("monoLowEV", "BL08B2-1:Crystal:EvLow", this, "Minimum Energy for current Crystal");
	monoBraggAngle_ = new AMReadOnlyPVControl("monoBraggAngle", "BL08B2-1:Energy:EV:fbk:tr.C", this, "Current Mono Bragg Angle");
	mono2d_ = new AMReadOnlyPVControl("mono2d","BL08B2-1:Crystal:2d", this, "2d spacing of current mono crystals");
	monoAngleOffset_ = new AMPVControl("monoAngleOffset","BL08B2-1:Energy:angleOffset","BL08B2-1:Energy:angleOffset",QString(), this);
	monoAngleOffset_->setDescription("Mono offset angle");


}

void IDEASBeamline::setupComponents()
{
	safetyShutter_ = new CLSBiStateControl("Safety Shutter", "The primary safety shutter for the beamline.", "SSH1409-B20-01:state", "SSH1409-B20-01:opr:open", "SSH1409-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	safetyShutter2_ = new CLSBiStateControl("SOE Safety Shutter", "The safety shutter for the SOE.", "SSH1608-9-B20-01:state", "SSH1608-9-B20-01:opr:open", "SSH1608-9-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	photonShutter2_ = new CLSBiStateControl("Photon Shutter 2", "The second photon shutter for the beamline.The primary safety shutter for the beamline.", "PSH1409-B20-02:state", "PSH1409-B20-02:opr:open", "PSH1409-B20-02:opr:close", new AMControlStatusCheckerDefault(2), this);

	jjSlitHGap_ = new AMPVwStatusControl("Sample Slit Width","SMTR1608-10-B20-01:mm:sp","SMTR1608-10-B20-01:mm","SMTR1608-10-B20-01:status","SMTR1608-10-B20-01:stop",this,0.1);
	jjSlitHCenter_ = new AMPVwStatusControl("Horizontal Center","SMTR1608-10-B20-02:mm:sp","SMTR1608-10-B20-02:mm","SMTR1608-10-B20-02:status","SMTR1608-10-B20-02:stop",this,0.1);
	jjSlitVGap_ = new AMPVwStatusControl("Sample Slit Height","SMTR1608-10-B20-03:mm:sp","SMTR1608-10-B20-03:mm","SMTR1608-10-B20-03:status","SMTR1608-10-B20-03:stop",this,0.1);
	jjSlitVCenter_ = new AMPVwStatusControl("Vertical Center","SMTR1608-10-B20-04:mm:sp","SMTR1608-10-B20-04:mm","SMTR1608-10-B20-04:status","SMTR1608-10-B20-04:stop",this,0.1);

	connect(safetyShutter_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));
	connect(safetyShutter2_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));
	connect(photonShutter2_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));

	scaler_ = new CLSSIS3820Scaler("BL08B2-1:mcs", this);

	scaler_->channelAt(0)->setCustomChannelName("I_0");
	CLSSR570 *tempSensitivitySR570 = new CLSSR570("I_0", "AMP1608-1001", this);
	scaler_->channelAt(0)->setCurrentAmplifier(tempSensitivitySR570);
	scaler_->channelAt(0)->setVoltagRange(AMRange(0.25, 4.75));

	scaler_->channelAt(1)->setCustomChannelName("Sample");
	tempSensitivitySR570 = new CLSSR570("Sample", "AMP1608-1002", this);
	scaler_->channelAt(1)->setCurrentAmplifier(tempSensitivitySR570);
	scaler_->channelAt(1)->setVoltagRange(AMRange(0.25, 4.75));

	scaler_->channelAt(2)->setCustomChannelName("Reference");
	tempSensitivitySR570 = new CLSSR570("Reference", "AMP1608-1003", this);
	scaler_->channelAt(2)->setCurrentAmplifier(tempSensitivitySR570);
	scaler_->channelAt(2)->setVoltagRange(AMRange(0.25, 4.75));
}

void IDEASBeamline::setupControlsAsDetectors()
{

}

void IDEASBeamline::setupExposedControls()
{
	addExposedControl(ringCurrent_);
	addExposedControl(i0Current_);
	addExposedControl(sampleTemp_);
	addExposedControl(monoEnergy_);
	addExposedControl(monoDirectEnergy_);
	addExposedControl(monoCrystal_);
	addExposedControl(monoAngleOffset_);

	if (ge13Element_)
		addExposedControl(ge13ElementRealTimeControl_);

	addExposedControl(ketekRealTimeControl_);
	addExposedControl(ketekPeakingTime_);
	addExposedControl(ketekTriggerLevel_);
	addExposedControl(ketekBaselineThreshold_);
	addExposedControl(ketekPreampGain_);

	addExposedControl(samplePlatformHorizontal_);
	addExposedControl(samplePlatformVertical_);
	addExposedControl(vacuumSampleStage_);

	addExposedControl(jjSlitHGap_);
	addExposedControl(jjSlitHCenter_);
	addExposedControl(jjSlitVGap_);
	addExposedControl(jjSlitVCenter_);
}

void IDEASBeamline::setupExposedDetectors()
{
	addExposedDetector(i0IonChamberScaler_);
	addExposedDetector(sampleIonChamberScaler_);
	addExposedDetector(referenceIonChamberScaler_);

	addExposedDetector(ketek_);
	addExposedDetector(ketekRealTime_);

	if (ge13Element_)
	{
	addExposedDetector(ge13Element_);
	addExposedDetector(ge13ElementRealTime_);
	}
}


IDEASBeamline::~IDEASBeamline()
{

}

AMAction3 *IDEASBeamline::createBeamOnAction() const
{
	// The correct order for turning the beam on is turning on the safety shutter and then the second photon shutter.
	AMSequentialListAction3 *beamOnAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam on action.", "The beam on action."));

	AMAction3 *safetyShutterAction = AMActionSupport::buildControlMoveAction(safetyShutter_, 1);
	beamOnAction->addSubAction(safetyShutterAction);

	AMAction3 *photonShutter2Action = AMActionSupport::buildControlMoveAction(photonShutter2_, 1);
	beamOnAction->addSubAction(photonShutter2Action);

	AMAction3 *safetyShutter2Action = AMActionSupport::buildControlMoveAction(safetyShutter2_, 1);
	beamOnAction->addSubAction(safetyShutter2Action);

	return beamOnAction;
}

AMAction3 *IDEASBeamline::createBeamOffAction() const
{
	// The correct order for turning the beam off is turning off the second photon shutter and then the safety shutter.
	AMSequentialListAction3 *beamOffAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam off action.", "The beam off action."));

	AMAction3 *safetyShutter2Action = AMActionSupport::buildControlMoveAction(safetyShutter2_, 0);
	beamOffAction->addSubAction(safetyShutter2Action);

	return beamOffAction;
}

bool IDEASBeamline::shuttersOpen() const
{
	return safetyShutter_->isOpen() && photonShutter2_->isOpen() && safetyShutter2_->isOpen();
}

void IDEASBeamline::onShutterStatusChanged()
{
	emit overallShutterStatus(safetyShutter_->isOpen() && photonShutter2_->isOpen() && safetyShutter2_->isOpen());
}

AMXRFDetector *IDEASBeamline::xrfDetector(IDEAS::FluorescenceDetectors detectorType)
{
	AMXRFDetector * XRFDetector = 0;

	if (detectorType.testFlag(IDEAS::Ketek))
		XRFDetector = IDEASBeamline::ideas()->ketek();

	else if (detectorType.testFlag(IDEAS::Ge13Element) && IDEASBeamline::ideas()->ge13Element()->isConnected())
		XRFDetector = IDEASBeamline::ideas()->ge13Element();

	return XRFDetector;
}

AMAction3 *IDEASBeamline::createSamplePlatformMoveVertical(double verticalPosition)
{
	if(!samplePlatformVertical_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(samplePlatformVertical_, verticalPosition);
}

AMAction3 *IDEASBeamline::createSamplePlatformMoveHorizontal(double horiztonalPosition)
{
	if(!samplePlatformHorizontal_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(samplePlatformHorizontal_, horiztonalPosition);
}

AMAction3 *IDEASBeamline::createVaccumSampleStageMove(double vaccumStagePosition)
{
	if(!vacuumSampleStage_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(vacuumSampleStage_, vaccumStagePosition);
}

AMAction3 *IDEASBeamline::createJJSlitHGapMove(double jjSlitHGapPosition)
{
	if(!jjSlitHGap_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(jjSlitHGap_, jjSlitHGapPosition);
}

AMAction3 *IDEASBeamline::createJJSlitHCenterMove(double jjSlitHCenterPosition)
{
	if(!jjSlitHCenter_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(jjSlitHCenter_, jjSlitHCenterPosition);
}

AMAction3 *IDEASBeamline::createJJSlitVGapMove(double jjSlitVGapPosition)
{
	if(!jjSlitVGap_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(jjSlitVGap_, jjSlitVGapPosition);
}

AMAction3 *IDEASBeamline::createJJSlitVCenterMove(double jjSlitVCenterPosition)
{
	if(!jjSlitVCenter_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(jjSlitVCenter_, jjSlitVCenterPosition);
}
