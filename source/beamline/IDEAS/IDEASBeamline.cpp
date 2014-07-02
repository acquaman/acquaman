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
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"

#include "beamline/AMAdvancedControlDetectorEmulator.h"

IDEASBeamline::IDEASBeamline()
	: AMBeamline("IDEAS Beamline")
{
//	setupSynchronizedDwellTime();
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
    I0Current_ = new AMReadOnlyPVControl("I0Current","A1608-10-01:A:fbk", this, "I0 Current");
    sampleTemp_ = new AMReadOnlyPVControl("sampleTemp","TC1608-10-02:reading", this, "Sample Temperature");
}

void IDEASBeamline::setupSampleStage()
{

}

void IDEASBeamline::setupMotorGroup()
{

}

void IDEASBeamline::setupDetectors()
{
//	oldIonChamberAmmeter_ = new AMReadOnlyPVControl("OldIon", "A1608-10-01:A:fbk", this, "Old Ion Chamber");
//	oxfordI0IonChamberAmmeter_ = new AMReadOnlyPVControl("I0_6485", "A1608-10-02:A:fbk", this, "I0 Ion Chamber");
//	oxfordSampleIonChamberAmmeter_ = new AMReadOnlyPVControl("Sample_6485", "A1608-10-03:A:fbk", this, "Sample Ion Chamber");
//	oxfordReferenceIonChamberAmmeter_ = new AMReadOnlyPVControl("Reference_6485", "A1608-10-04:A:fbk", this, "Reference Ion Chamber");

//	oldIonChamberDetector_ = new AMAdvancedControlDetectorEmulator("I_vac_6485", "Old Ion Chamber Detector", oldIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::RequestRead, "A1608-10-01:start_read", this);
//	oxfordI0IonChamberDetector_ = new AMAdvancedControlDetectorEmulator("I_0_6485", "I0 Detector", oxfordI0IonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, "A1608B2:start_read NPP NMS", this);
//	oxfordSampleIonChamberDetector_ = new AMAdvancedControlDetectorEmulator("I_sample_6485", "Sample Detector", oxfordSampleIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, "A1608B2:start_read NPP NMS", this);
//	oxfordReferenceIonChamberDetector_ = new AMAdvancedControlDetectorEmulator("I_ref_6485", "Reference Detector", oxfordReferenceIonChamberAmmeter_, 0, 0, 0, AMDetectorDefinitions::WaitRead, "A1608B2:start_read NPP NMS", this);

	ketek_ = new IDEASKETEKDetector("XRF1E", "Single Element XRF Detector", this);

	ketekPeakingTime_ = new AMPVControl("XRF1E Peaking Time","dxp1608-1002:dxp1:PeakingTime_RBV","dxp1608-1002:dxp1:PeakingTime", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
	ketekTriggerLevel_ = new AMPVControl("XRF1E Trigger Level","dxp1608-1002:dxp1:TriggerThreshold_RBV","dxp1608-1002:dxp1:TriggerThreshold", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
	ketekBaselineThreshold_ = new AMPVControl("XRF1E Baseline Threshold","dxp1608-1002:dxp1:BaselineThreshold_RBV","dxp1608-1002:dxp1:BaselineThreshold", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);
	ketekPreampGain_ = new AMPVControl("XRF1E Preamp Gain","dxp1608-1002:dxp1:PreampGain_RBV","dxp1608-1002:dxp1:PreampGain", QString(), this, AMCONTROL_TOLERANCE_DONT_CARE);

	ketekRealTimeControl_ = new AMReadOnlyPVControl("XRF1E Real Time", "dxp1608-1002:mca1.ERTM", this);

	ketekRealTime_ = new AMBasicControlDetectorEmulator("dwellTime", "Dwell Time", ketekRealTimeControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	ketekRealTime_->setHiddenFromUsers(true);
	ketekRealTime_->setIsVisible(false);

	I0IonChamberScaler_ = new CLSBasicScalerChannelDetector("I_0","I_0 Ion Chamber", scaler_, 0, this);
	SampleIonChamberScaler_ = new CLSBasicScalerChannelDetector("Sample","Sample Ion Chamber", scaler_, 1, this);
	ReferenceIonChamberScaler_  = new CLSBasicScalerChannelDetector("Reference","Reference Ion Chamber", scaler_, 2, this);


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

//void IDEASBeamline::setupSynchronizedDwellTime()
//{
//	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL08B2-1:dwell", this);
//	synchronizedDwellTime_->addElement(0);
//        synchronizedDwellTime_->addElement(1);
//        synchronizedDwellTime_->addElement(2);
//        synchronizedDwellTime_->addElement(3);
//        synchronizedDwellTime_->addElement(4);
//        synchronizedDwellTime_->addElement(5);

//	masterDwell_ = new AMPVControl("MasterDwell", "BL08B2-1:dwell:setTime", "BL08B2-1:dwell:setTime", QString(), this, 0.5);
//        ammeterGroupMode_ = new AMPVControl("ammeterGroupMode", "A1608B2:configure", "A1608B2:configure", QString(),this,0.5);
//	masterDwellTime_ = new AMBasicControlDetectorEmulator("DwellTime", "Dwell Time", masterDwell_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
//	masterDwellTime_->setHiddenFromUsers(true);
//	masterDwellTime_->setIsVisible(false);

//}

void IDEASBeamline::setupComponents()
{
    safetyShutter_ = new CLSBiStateControl("Safety Shutter", "The primary safety shutter for the beamline.", "SSH1409-B20-01:state", "SSH1409-B20-01:opr:open", "SSH1409-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
    safetyShutter2_ = new CLSBiStateControl("SOE Safety Shutter", "The safety shutter for the SOE.", "SSH1608-9-B20-01:state", "SSH1608-9-B20-01:opr:open", "SSH1608-9-B20-01:opr:close", new AMControlStatusCheckerDefault(2), this);
    photonShutter2_ = new CLSBiStateControl("Photon Shutter 2", "The second photon shutter for the beamline.The primary safety shutter for the beamline.", "PSH1409-B20-02:state", "PSH1409-B20-02:opr:open", "PSH1409-B20-02:opr:close", new AMControlStatusCheckerDefault(2), this);


    connect(safetyShutter_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));
    connect(safetyShutter2_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));
    connect(photonShutter2_, SIGNAL(stateChanged(int)), this, SLOT(onShutterStatusChanged()));

    scaler_ = new CLSSIS3820Scaler("BL08B2-1:mcs", this);

    scaler_->channelAt(0)->setCustomChannelName("I_0");
    CLSSR570 *tempSR570 = new CLSSR570("I_0", "AMP1608-1001:sens_num.VAL", "AMP1608-1001:sens_unit.VAL", this);
    scaler_->channelAt(0)->setCurrentAmplifier(tempSR570);
    scaler_->channelAt(0)->setVoltagRange(AMRange(0.25, 4.75));

    scaler_->channelAt(1)->setCustomChannelName("Sample");
    tempSR570 = new CLSSR570("Sample", "AMP1608-1002:sens_num.VAL", "AMP1608-1002:sens_unit.VAL", this);
    scaler_->channelAt(1)->setCurrentAmplifier(tempSR570);
    scaler_->channelAt(1)->setVoltagRange(AMRange(0.25, 4.75));

    scaler_->channelAt(2)->setCustomChannelName("Reference");
    tempSR570 = new CLSSR570("Reference", "AMP1608-1003:sens_num.VAL", "AMP1608-1003:sens_unit.VAL", this);
    scaler_->channelAt(2)->setCurrentAmplifier(tempSR570);
    scaler_->channelAt(2)->setVoltagRange(AMRange(0.25, 4.75));

}

void IDEASBeamline::setupControlsAsDetectors()
{

}

void IDEASBeamline::setupExposedControls()
{
        addExposedControl(ringCurrent_);
	addExposedControl(I0Current_);
	addExposedControl(sampleTemp_);
        addExposedControl(monoEnergy_);
	addExposedControl(monoDirectEnergy_);
//	addExposedControl(masterDwell_);
        addExposedControl(monoCrystal_);
	addExposedControl(monoAngleOffset_);

	addExposedControl(ketekRealTimeControl_);
	addExposedControl(ketekPeakingTime_);
	addExposedControl(ketekTriggerLevel_);
	addExposedControl(ketekBaselineThreshold_);
	addExposedControl(ketekPreampGain_);



}

void IDEASBeamline::setupExposedDetectors()
{
	addExposedDetector(I0IonChamberScaler_);
	addExposedDetector(SampleIonChamberScaler_);
	addExposedDetector(ReferenceIonChamberScaler_);
//	addExposedDetector(oldIonChamberDetector_);
//	addExposedDetector(oxfordI0IonChamberDetector_);
//	addExposedDetector(oxfordSampleIonChamberDetector_);
//	addExposedDetector(oxfordReferenceIonChamberDetector_);
        addExposedDetector(ketek_);
	addExposedDetector(ketekRealTime_);
//	addExposedDetector(masterDwellTime_);
}


IDEASBeamline::~IDEASBeamline()
{

}

AMAction3 *IDEASBeamline::createBeamOnAction() const
{
    // The correct order for turning the beam on is turning on the safety shutter and then the second photon shutter.
    AMSequentialListAction3 *beamOnAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam on action.", "The beam on action."));

    AMControlInfo setpoint = safetyShutter_->toInfo();
    setpoint.setValue(1);
    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, safetyShutter_);
    beamOnAction->addSubAction(action);

    setpoint = photonShutter2_->toInfo();
    setpoint.setValue(1);
    actionInfo = new AMControlMoveActionInfo3(setpoint);
    action = new AMControlMoveAction3(actionInfo, photonShutter2_);
    beamOnAction->addSubAction(action);

    setpoint = safetyShutter2_->toInfo();
    setpoint.setValue(1);
    actionInfo = new AMControlMoveActionInfo3(setpoint);
    action = new AMControlMoveAction3(actionInfo, safetyShutter2_);
    beamOnAction->addSubAction(action);


    return beamOnAction;
}

AMAction3 *IDEASBeamline::createBeamOffAction() const
{
    // The correct order for turning the beam off is turning off the second photon shutter and then the safety shutter.
    AMSequentialListAction3 *beamOffAction = new AMSequentialListAction3(new AMSequentialListActionInfo3("The beam off action.", "The beam off action."));

	AMControlInfo setpoint = safetyShutter2_->toInfo();
	setpoint.setValue(0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, safetyShutter2_);
	beamOffAction->addSubAction(action);

//    AMControlInfo setpoint = photonShutter2_->toInfo();
//    setpoint.setValue(0);
//    AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
//    AMControlMoveAction3 *action = new AMControlMoveAction3(actionInfo, photonShutter2_);
//    beamOffAction->addSubAction(action);

//    setpoint = safetyShutter_->toInfo();
//    setpoint.setValue(0);
//    actionInfo = new AMControlMoveActionInfo3(setpoint);
//    action = new AMControlMoveAction3(actionInfo, safetyShutter_);
//    beamOffAction->addSubAction(action);

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
