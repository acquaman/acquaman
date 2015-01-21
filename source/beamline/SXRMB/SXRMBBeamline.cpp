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


#include "SXRMBBeamline.h"

#include <QDebug>

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"

#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSSR570.h"

SXRMBBeamline::SXRMBBeamline()
	: AMBeamline("SXRMB Beamline")
{
	//energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:Energy:fbk", "BL1606-B1-1:Energy", "BL1606-B1-1:Energy:status", QString(), this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());
	energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:AddOns:Energy:fbk", "BL1606-B1-1:AddOns:Energy", "BL1606-B1-1:AddOns:Energy:status", "BL1606-B1-1:AddOns:Energy:stop", this, 0.05, 2.0, new AMControlStatusCheckerCLSMAXv());

	beamlineStatus_ = new AMReadOnlyPVControl("BeamlineStatus", "BL1606-B01:ready:status", this);

	// these motors actually have the encoder reporting positions to the "mm:sp" PV
	microprobeSampleStageX_ = new AMPVwStatusControl("MicroprobeSampleStageX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:status", "SVM1606-5-B10-07:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	microprobeSampleStageY_ = new AMPVwStatusControl("MicroprobeSampleStageY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:status", "SVM1606-5-B10-08:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	microprobeSampleStageZ_ = new AMPVwStatusControl("MicroprobeSampleStageZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:status", "SVM1606-5-B10-09:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());

	microprobeSampleStageControlSet_ = new AMControlSet(this);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageX_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageY_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageZ_);

	CLSSR570 *tempSR570;

	scaler_ = new CLSSIS3820Scaler("BL1606-B1-1:mcs", this);

	tempSR570 = new CLSSR570("TEY", "Amp1606-5-B10-03", this);
	scaler_->channelAt(18)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(18)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(18)->setCustomChannelName("TEY");

	tempSR570 = new CLSSR570("I0", "Amp1606-5-B10-02", this);
	scaler_->channelAt(17)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(17)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(17)->setCustomChannelName("I0");

	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 17, this);
	scaler_->channelAt(17)->setDetector(i0Detector_);

	teyDetector_ = new CLSBasicScalerChannelDetector("TEYDetector", "TEY Detector", scaler_, 18, this);
	scaler_->channelAt(18)->setDetector(teyDetector_);

	energyFeedbackControl_ = new AMReadOnlyPVControl("EnergyFeedback", "BL1606-B1-1:Energy:fbk", this);
	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "Energy Feedback", energyFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	PSH1406B1002Shutter_ = new CLSBiStateControl("PhotonShutter2", "Photon Shutter 2", "PSH1406-B10-02:state", "PSH1406-B10-02:opr:open", "PSH1406-B10-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	VVR16064B1003Valve_ = new CLSBiStateControl("4B1003Valve", "4-B10-03 Valve", "VVR1606-4-B10-03:state", "VVR1606-4-B10-03:opr:open", "VVR1606-4-B10-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	VVR16064B1004Valve_ = new CLSBiStateControl("4B1004Valve", "4-B10-04 Valve", "VVR1606-4-B10-04:state", "VVR1606-4-B10-04:opr:open", "VVR1606-4-B10-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	VVR16064B1006Valve_ = new CLSBiStateControl("4B1006Valve", "4-B10-06 Valve", "VVR1606-4-B10-06:state", "VVR1606-4-B10-06:opr:open", "VVR1606-4-B10-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	VVR16064B1007Valve_ = new CLSBiStateControl("4B1007Valve", "4-B10-07 Valve", "VVR1606-4-B10-07:state", "VVR1606-4-B10-07:opr:open", "VVR1606-4-B10-07:opr:close", new AMControlStatusCheckerDefault(4), this);
	VVR16065B1001Valve_ = new CLSBiStateControl("5B1001Valve", "5-B10-01 Valve", "VVR1606-5-B10-01:state", "VVR1606-5-B10-01:opr:open", "VVR1606-5-B10-01:opr:close", new AMControlStatusCheckerDefault(4), this);

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

	connect(energy_, SIGNAL(connected(bool)), this, SLOT(onEnergyPVConnected(bool)));
	connect(beamlineStatus_, SIGNAL(connected(bool)), this, SLOT(onBeamlineStatusPVConnected(bool)));
	connect(microprobeSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onMicroprobeSampleStagePVsConnected(bool)));

	wasConnected_ = false;
	connectedHelper();
}

CLSSIS3820Scaler* SXRMBBeamline::scaler() const {
	return scaler_;
}

AMPVwStatusControl* SXRMBBeamline::energy() const {
	return energy_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageX() const {
	return microprobeSampleStageX_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageY() const {
	return microprobeSampleStageY_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageZ() const {
	return microprobeSampleStageZ_;
}

AMMotorGroup *SXRMBBeamline::motorGroup() const
{
	return motorGroup_;
}

AMMotorGroupObject *SXRMBBeamline::microprobeSampleStageMotorGroupObject() const
{
	return motorGroup_->motorGroupObject("Microprobe Stage - X, Z, Y");
}

AMReadOnlyPVControl* SXRMBBeamline::beamlineStatus() const {
	return beamlineStatus_;
}

bool SXRMBBeamline::isConnected() const{
	return energy_->isConnected() && beamlineStatus_->isConnected() && microprobeSampleStageControlSet_->isConnected();
}

CLSBasicScalerChannelDetector* SXRMBBeamline::i0Detector() const{
	return i0Detector_;
}

CLSBasicScalerChannelDetector* SXRMBBeamline::teyDetector() const{
	return teyDetector_;
}

AMBasicControlDetectorEmulator* SXRMBBeamline::energyFeedbackDetector() const{
	return energyFeedbackDetector_;
}

SXRMBBrukerDetector* SXRMBBeamline::brukerDetector() const{
	return brukerDetector_;
}

AMAction3* SXRMBBeamline::createBeamOnActions() const{
	if(!isConnected())
		return 0;

	AMListAction3 *beamOnActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Beam On", "SXRMB Beam On"), AMListAction3::Parallel);

	AMListAction3 *valveOpenActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Valve Open", "SXRMB Valve Open"), AMListAction3::Sequential);

	valveOpenActionsList->addSubAction(AMActionSupport::buildControlMoveAction(VVR16064B1003Valve_, 1));
	valveOpenActionsList->addSubAction(AMActionSupport::buildControlMoveAction(VVR16064B1004Valve_, 1));
	valveOpenActionsList->addSubAction(AMActionSupport::buildControlMoveAction(VVR16064B1006Valve_, 1));
	valveOpenActionsList->addSubAction(AMActionSupport::buildControlMoveAction(VVR16064B1007Valve_, 1));
	valveOpenActionsList->addSubAction(AMActionSupport::buildControlMoveAction(VVR16065B1001Valve_, 1));

	AMListAction3 *valveWaitActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Valve Wait", "SXRMB Valve Wait"), AMListAction3::Parallel);

	AMControlInfo VVR16064B1003ValveWaitSetpoint = VVR16064B1003Valve_->toInfo();
	VVR16064B1003ValveWaitSetpoint.setValue(1);
	AMControlWaitAction *VVR16064B1003ValveWaitAction = new AMControlWaitAction(new AMControlWaitActionInfo(VVR16064B1003ValveWaitSetpoint, 10, AMControlWaitActionInfo::MatchEqual), VVR16064B1003Valve_);
	valveWaitActionsList->addSubAction(VVR16064B1003ValveWaitAction);

	AMControlInfo VVR16064B1004ValveWaitSetpoint = VVR16064B1004Valve_->toInfo();
	VVR16064B1004ValveWaitSetpoint.setValue(1);
	AMControlWaitAction *VVR16064B1004ValveWaitAction = new AMControlWaitAction(new AMControlWaitActionInfo(VVR16064B1004ValveWaitSetpoint, 10, AMControlWaitActionInfo::MatchEqual), VVR16064B1004Valve_);
	valveWaitActionsList->addSubAction(VVR16064B1004ValveWaitAction);

	AMControlInfo VVR16064B1006ValveWaitSetpoint = VVR16064B1006Valve_->toInfo();
	VVR16064B1006ValveWaitSetpoint.setValue(1);
	AMControlWaitAction *VVR16064B1006ValveWaitAction = new AMControlWaitAction(new AMControlWaitActionInfo(VVR16064B1006ValveWaitSetpoint, 10, AMControlWaitActionInfo::MatchEqual), VVR16064B1006Valve_);
	valveWaitActionsList->addSubAction(VVR16064B1006ValveWaitAction);

	AMControlInfo VVR16064B1007ValveWaitSetpoint = VVR16064B1007Valve_->toInfo();
	VVR16064B1007ValveWaitSetpoint.setValue(1);
	AMControlWaitAction *VVR16064B1007ValveWaitAction = new AMControlWaitAction(new AMControlWaitActionInfo(VVR16064B1007ValveWaitSetpoint, 10, AMControlWaitActionInfo::MatchEqual), VVR16064B1007Valve_);
	valveWaitActionsList->addSubAction(VVR16064B1007ValveWaitAction);

	AMControlInfo VVR16065B1001ValveWaitSetpoint = VVR16065B1001Valve_->toInfo();
	VVR16065B1001ValveWaitSetpoint.setValue(1);
	AMControlWaitAction *VVR16065B1001ValveWaitAction = new AMControlWaitAction(new AMControlWaitActionInfo(VVR16065B1001ValveWaitSetpoint, 10, AMControlWaitActionInfo::MatchEqual), VVR16065B1001Valve_);
	valveWaitActionsList->addSubAction(VVR16065B1001ValveWaitAction);


	beamOnActionsList->addSubAction(valveOpenActionsList);
	beamOnActionsList->addSubAction(valveWaitActionsList);


	return beamOnActionsList;
}

AMAction3* SXRMBBeamline::createBeamOffActions() const{
	if(!isConnected())
		return 0;

	AMListAction3 *beamOffActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Beam Off", "SXRMB Beam Off"), AMListAction3::Parallel);

	beamOffActionsList->addSubAction(AMActionSupport::buildControlMoveAction(PSH1406B1002Shutter_, 0));
	beamOffActionsList->addSubAction(AMActionSupport::buildControlMoveAction(PSH1406B1002Shutter_, 0));

	return beamOffActionsList;
}

void SXRMBBeamline::setupDiagnostics()
{

}

void SXRMBBeamline::setupSampleStage()
{

}

void SXRMBBeamline::setupMotorGroup()
{
	AMMotorGroupObject *motorObject = 0;
	motorGroup_ = new AMMotorGroup(this);

	motorObject = new AMMotorGroupObject("Microprobe Stage - X, Z, Y",
										 QStringList() << "X" << "Z" << "Y",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << microprobeSampleStageX_ << microprobeSampleStageZ_ << microprobeSampleStageY_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 this);

	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
}

void SXRMBBeamline::setupDetectors()
{
	brukerDetector_ = new SXRMBBrukerDetector("Bruker", "Bruker XRF detector", this);
}

void SXRMBBeamline::setupControlSets()
{

}

void SXRMBBeamline::setupMono()
{

}

void SXRMBBeamline::setupSynchronizedDwellTime()
{

}

void SXRMBBeamline::setupComponents()
{

}

void SXRMBBeamline::setupControlsAsDetectors()
{

}

void SXRMBBeamline::setupExposedControls()
{
	addExposedControl(energy_);

	addExposedControl(microprobeSampleStageX_);
	addExposedControl(microprobeSampleStageY_);
	addExposedControl(microprobeSampleStageZ_);
}

void SXRMBBeamline::setupExposedDetectors()
{
	addExposedDetector(i0Detector_);
	addExposedDetector(teyDetector_);
	addExposedDetector(energyFeedbackDetector_);
	addExposedDetector(brukerDetector_);
}

SXRMBBeamline::~SXRMBBeamline()
{

}

void SXRMBBeamline::connectedHelper(){
	if (wasConnected_ && !isConnected()) {
		emit connected(false);
		wasConnected_ = false;
	} else if (!wasConnected_ && isConnected()) {
		emit connected(true);
		wasConnected_ = true;
	}
}

void SXRMBBeamline::onEnergyPVConnected(bool) {
	connectedHelper();
}

void SXRMBBeamline::onBeamlineStatusPVConnected(bool) {
	connectedHelper();
}

void SXRMBBeamline::onMicroprobeSampleStagePVsConnected(bool) {
	connectedHelper();
}
