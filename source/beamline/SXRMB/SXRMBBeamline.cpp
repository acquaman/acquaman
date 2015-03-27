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

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlWaitAction.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

SXRMBBeamline::SXRMBBeamline()
	: AMBeamline("SXRMB Beamline")
{
	currentEndstation_ = SXRMB::Invalid;

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
	setupConnections();

	wasConnected_ = false;
	sampleStageConnectHelper();
	connectedHelper();
}

SXRMBBeamline::~SXRMBBeamline()
{
}

void SXRMBBeamline::switchEndstation(SXRMB::Endstation endstation)
{
	if (currentEndstation_ != endstation) {

		switch (currentEndstation_){

		case SXRMB::SolidState:

			removeExposedControl(solidStateSampleStageX_);
			removeExposedControl(solidStateSampleStageY_);
			removeExposedControl(solidStateSampleStageZ_);
			removeExposedControl(solidStateSampleStageR_);

			removeExposedDetector(beamlineI0Detector_);
			removeExposedDetector(teyDetector_);

			break;

		case SXRMB::AmbiantWithGasChamber:

			removeExposedControl(ambiantSampleHolderZ_);
			removeExposedControl(ambiantSampleHolderR_);

			removeExposedDetector(beamlineI0Detector_);
			removeExposedDetector(i0Detector_);
			removeExposedDetector(transmissionDetector_);

			break;

		case SXRMB::AmbiantWithoutGasChamber:

			removeExposedControl(ambiantSampleStageX_);
			removeExposedControl(ambiantSampleStageZ_);

			removeExposedDetector(beamlineI0Detector_);
			removeExposedDetector(i0Detector_);
			removeExposedDetector(transmissionDetector_);

			break;

		case SXRMB::Microprobe:

			removeExposedControl(microprobeSampleStageX_);
			removeExposedControl(microprobeSampleStageY_);
			removeExposedControl(microprobeSampleStageZ_);

			removeExposedDetector(i0Detector_);
			removeExposedDetector(teyDetector_);

			break;

		default:
			break;
		}

		switch (endstation){

		case SXRMB::SolidState:

			addExposedControl(solidStateSampleStageX_);
			addExposedControl(solidStateSampleStageY_);
			addExposedControl(solidStateSampleStageZ_);
			addExposedControl(solidStateSampleStageR_);

			addExposedDetector(beamlineI0Detector_);
			addExposedDetector(teyDetector_);

			break;

		case SXRMB::AmbiantWithGasChamber:

			addExposedControl(ambiantSampleHolderZ_);
			addExposedControl(ambiantSampleHolderR_);

			addExposedDetector(beamlineI0Detector_);
			addExposedDetector(i0Detector_);
			addExposedDetector(transmissionDetector_);

			break;

		case SXRMB::AmbiantWithoutGasChamber:

			addExposedControl(ambiantSampleStageX_);
			addExposedControl(ambiantSampleStageZ_);

			addExposedDetector(beamlineI0Detector_);
			addExposedDetector(i0Detector_);
			addExposedDetector(transmissionDetector_);

			break;

		case SXRMB::Microprobe:

			addExposedControl(microprobeSampleStageX_);
			addExposedControl(microprobeSampleStageY_);
			addExposedControl(microprobeSampleStageZ_);

			addExposedDetector(i0Detector_);
			addExposedDetector(teyDetector_);

			break;

		default:
			break;
		}

		SXRMB::Endstation fromEndstation = currentEndstation_;
		currentEndstation_ = endstation;
		emit endstationChanged(fromEndstation, currentEndstation_);
	}
}

CLSSIS3820Scaler* SXRMBBeamline::scaler() const
{
	return scaler_;
}

AMPVwStatusControl* SXRMBBeamline::energy() const
{
	return energy_;
}

SXRMB::Endstation SXRMBBeamline::currentEndstation() const
{
	return currentEndstation_;
}

AMPVwStatusControl* SXRMBBeamline::endstationSampleStageX(SXRMB::Endstation endstation) const
{
	AMPVwStatusControl* statusControl;
	switch (endstation) {
	case SXRMB::SolidState:
		statusControl = solidStateSampleStageX();
		break;
	case SXRMB::AmbiantWithoutGasChamber:
		statusControl = ambiantSampleStageX();
		break;
	case SXRMB::Microprobe:
		statusControl = microprobeSampleStageX();
		break;
	case SXRMB::AmbiantWithGasChamber:
	default:
		statusControl = 0;
	}

	return statusControl;
}

AMPVwStatusControl* SXRMBBeamline::endstationSampleStageY(SXRMB::Endstation endstation) const
{
	AMPVwStatusControl* statusControl;
	switch (endstation) {
	case SXRMB::SolidState:
		statusControl = solidStateSampleStageY();
		break;
	case SXRMB::Microprobe:
		statusControl = microprobeSampleStageY();
		break;
	case SXRMB::AmbiantWithGasChamber:
	case SXRMB::AmbiantWithoutGasChamber:
	default:
		statusControl = 0;
	}

	return statusControl;
}

AMPVwStatusControl* SXRMBBeamline::endstationSampleStageZ(SXRMB::Endstation endstation) const
{
	AMPVwStatusControl* statusControl;
	switch (endstation) {
	case SXRMB::SolidState:
		statusControl = solidStateSampleStageZ();
		break;
	case SXRMB::AmbiantWithGasChamber:
		statusControl = ambiantSampleHolderZ();
		break;
	case SXRMB::AmbiantWithoutGasChamber:
		statusControl = ambiantSampleStageZ();
		break;
	case SXRMB::Microprobe:
		statusControl = microprobeSampleStageZ();
		break;
	default:
		statusControl = 0;
	}

	return statusControl;
}

AMPVwStatusControl* SXRMBBeamline::endstationSampleStageR(SXRMB::Endstation endstation) const
{
	AMPVwStatusControl* statusControl;

	switch (endstation) {
	case SXRMB::SolidState:
		statusControl = solidStateSampleStageR();
		break;
	case SXRMB::AmbiantWithGasChamber:
		statusControl = ambiantSampleHolderR();
		break;
	case SXRMB::AmbiantWithoutGasChamber:
	case SXRMB::Microprobe:
	default:
		statusControl = 0;
	}

	return statusControl;
}



AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageX() const
{
	return microprobeSampleStageX_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageY() const
{
	return microprobeSampleStageY_;
}

AMPVwStatusControl* SXRMBBeamline::microprobeSampleStageZ() const
{
	return microprobeSampleStageZ_;
}

AMPVwStatusControl* SXRMBBeamline::solidStateSampleStageX() const
{
	return solidStateSampleStageX_;
}

AMPVwStatusControl* SXRMBBeamline::solidStateSampleStageY() const
{
	return solidStateSampleStageY_;
}

AMPVwStatusControl* SXRMBBeamline::solidStateSampleStageZ() const
{
	return solidStateSampleStageZ_;
}

AMPVwStatusControl* SXRMBBeamline::solidStateSampleStageR() const
{
	return solidStateSampleStageR_;
}

AMMotorGroup *SXRMBBeamline::motorGroup() const
{
	return motorGroup_;
}

AMMotorGroupObject *SXRMBBeamline::microprobeSampleStageMotorGroupObject() const
{
	return motorGroup_->motorGroupObject("Microprobe Stage - X, Z, Y");
}

AMMotorGroupObject *SXRMBBeamline::solidStateSampleStageMotorGroupObject() const
{
	return motorGroup_->motorGroupObject("Solid State - X, Z, Y, R");
}

AMMotorGroupObject *SXRMBBeamline::ambiantWithGasChamberSampleStageMotorGroupObject() const
{
	return motorGroup_->motorGroupObject("Ambiant With Gas Chamber - Z, R");
}

AMMotorGroupObject *SXRMBBeamline::ambiantWithoutGasChamberSampleStageMotorGroupObject() const
{
	return motorGroup_->motorGroupObject("Ambiant Without Gas Chamber - X, Z");
}

QString SXRMBBeamline::currentMotorGroupName() const
{
	QString motorGroupName;

	switch (currentEndstation_) {

	case SXRMB::SolidState:
		motorGroupName = solidStateSampleStageMotorGroupObject()->name();
		break;

	case SXRMB::AmbiantWithGasChamber:
		motorGroupName = ambiantWithGasChamberSampleStageMotorGroupObject()->name();
		break;

	case SXRMB::AmbiantWithoutGasChamber:
		motorGroupName = ambiantWithoutGasChamberSampleStageMotorGroupObject()->name();
		break;

	case SXRMB::Microprobe:
		motorGroupName = microprobeSampleStageMotorGroupObject()->name();
		break;

	default:
		motorGroupName = "Unknown";
		break;
	}

	return motorGroupName;
}


AMReadOnlyPVControl* SXRMBBeamline::beamlineStatus() const
{
	return beamlineStatus_;
}

bool SXRMBBeamline::isConnected() const
{
	// check whether the sample stage is connected or not
	bool sampleStageConnected = false;
	switch (currentEndstation_) {
	case SXRMB::SolidState:
		sampleStageConnected = solidStateSampleStageControlSet_->isConnected();
		break;

	case SXRMB::AmbiantWithGasChamber:
		sampleStageConnected = ambiantWithGasChamberSampleStageControlSet_->isConnected();
		break;

	case SXRMB::AmbiantWithoutGasChamber:
		sampleStageConnected = ambiantWithoutGasChamberSampleStageControlSet_->isConnected();
		break;

	case SXRMB::Microprobe:
		sampleStageConnected = microprobeSampleStageControlSet_->isConnected();
		break;

	default:
		sampleStageConnected = false;
		break;
	}

	// return whether the expected PVs are connected or not
	return energy_->isConnected() && beamlineStatus_->isConnected()
			&& beamlineControlShutterSet_->isConnected() && sampleStageConnected;
}


CLSBasicScalerChannelDetector* SXRMBBeamline::beamlineI0Detector() const
{
	return beamlineI0Detector_;
}

CLSBasicScalerChannelDetector* SXRMBBeamline::i0Detector() const
{
	return i0Detector_;
}

CLSBasicScalerChannelDetector* SXRMBBeamline::teyDetector() const
{
	return teyDetector_;
}


CLSBasicScalerChannelDetector* SXRMBBeamline::transmissionDetector() const
{
	return transmissionDetector_;
}

AMBasicControlDetectorEmulator* SXRMBBeamline::energyFeedbackDetector() const
{
	return energyFeedbackDetector_;
}

SXRMBBrukerDetector* SXRMBBeamline::brukerDetector() const
{
	return brukerDetector_;
}

SXRMBFourElementVortexDetector *SXRMBBeamline::fourElementVortexDetector() const
{
	return fourElementVortexDetector_;
}

AMAction3* SXRMBBeamline::createBeamOnActions() const
{
	if(!isConnected())
		return 0;

	// if all the valves are already open, we don't need to do that again
	if (VVR16064B1003Valve_->isOpen() && VVR16064B1004Valve_->isOpen() && VVR16064B1006Valve_->isOpen() && VVR16064B1007Valve_->isOpen() && VVR16065B1001Valve_->isOpen() && PSH1406B1002Shutter_->isOpen())
		return 0;

	// stage 1: open / wait the valves action list
	AMListAction3 *valveOpenActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Valve Open action list", "SXRMB Valve Open"), AMListAction3::Sequential);
	AMListAction3 *valveWaitActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Valve Wait action list", "SXRMB Valve Wait"), AMListAction3::Parallel);

	if (VVR16064B1003Valve_->isClosed()) {
		AMAction3 *VVR16064B1003ValveSetpointOpenAction = AMActionSupport::buildControlMoveAction(VVR16064B1003Valve_, 1);
		valveOpenActionsList->addSubAction(VVR16064B1003ValveSetpointOpenAction);

		AMAction3 *VVR16064B1003ValveWaitAction = AMActionSupport::buildControlWaitAction(VVR16064B1003Valve_, 1);
		valveWaitActionsList->addSubAction(VVR16064B1003ValveWaitAction);
	}

	if (VVR16064B1004Valve_->isClosed()) {
		AMAction3 *VVR16064B1004ValveSetpointOpenAction = AMActionSupport::buildControlMoveAction(VVR16064B1004Valve_, 1);
		valveOpenActionsList->addSubAction(VVR16064B1004ValveSetpointOpenAction);

		AMAction3 *VVR16064B1004ValveWaitAction = AMActionSupport::buildControlWaitAction(VVR16064B1004Valve_, 1);
		valveWaitActionsList->addSubAction(VVR16064B1004ValveWaitAction);
	}

	if (VVR16064B1006Valve_->isClosed()) {
		AMAction3 *VVR16064B1006ValveSetpointOpenAction = AMActionSupport::buildControlMoveAction(VVR16064B1006Valve_, 1);
		valveOpenActionsList->addSubAction(VVR16064B1006ValveSetpointOpenAction);

		AMAction3 *VVR16064B1006ValveWaitAction = AMActionSupport::buildControlWaitAction(VVR16064B1006Valve_, 1);
		valveWaitActionsList->addSubAction(VVR16064B1006ValveWaitAction);
	}

	if (VVR16064B1007Valve_->isClosed()) {
		AMAction3 *VVR16064B1007ValveSetpointOpenAction = AMActionSupport::buildControlMoveAction(VVR16064B1007Valve_, 1);
		valveOpenActionsList->addSubAction(VVR16064B1007ValveSetpointOpenAction);

		AMAction3 *VVR16064B1007ValveWaitAction = AMActionSupport::buildControlWaitAction(VVR16064B1007Valve_, 1);
		valveWaitActionsList->addSubAction(VVR16064B1007ValveWaitAction);
	}

	if (VVR16065B1001Valve_->isClosed()) {
		AMAction3 *VVR16065B1001ValveSetpointOpenAction = AMActionSupport::buildControlMoveAction(VVR16065B1001Valve_, 1);
		valveOpenActionsList->addSubAction(VVR16065B1001ValveSetpointOpenAction);

		AMAction3 *VVR16065B1001ValveWaitAction = AMActionSupport::buildControlWaitAction(VVR16065B1001Valve_, 1);
		valveWaitActionsList->addSubAction(VVR16065B1001ValveWaitAction);
	}

	AMListAction3 *openValvesActionsList = 0;
	if (valveOpenActionsList->subActionCount() > 0) {
		AMListAction3 *openValvesActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Beam On", "SXRMB Beam On: stage 1"), AMListAction3::Parallel);
		openValvesActionsList->addSubAction(valveOpenActionsList);
		openValvesActionsList->addSubAction(valveWaitActionsList);
	}

	// stage 2: open/wait photon shutter action list, which MUST run after the valves open actions
	AMListAction3 *openPhotonShutterActionsList = 0;
	if (PSH1406B1002Shutter_->isClosed()) {
		AMAction3 *PSH1406B1002ShutterOpenAction = AMActionSupport::buildControlMoveAction(PSH1406B1002Shutter_, 1);
		AMAction3 *PSH1406B1002ShutterWaitAction = AMActionSupport::buildControlWaitAction(PSH1406B1002Shutter_, 1);

		openPhotonShutterActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Beam On", "SXRMB Beam On: stage 2"), AMListAction3::Parallel);
		openPhotonShutterActionsList->addSubAction(PSH1406B1002ShutterOpenAction);
		openPhotonShutterActionsList->addSubAction(PSH1406B1002ShutterWaitAction);
	}

	// create the beam on action list. The openValveActionsList and openPhotonShutterActionsList MUST run sequentially
	AMListAction3 *beamOnActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Beam On", "SXRMB Beam On"), AMListAction3::Sequential);
	if (openValvesActionsList) {
		beamOnActionsList->addSubAction(openValvesActionsList);
	}
	if (openPhotonShutterActionsList) {
		beamOnActionsList->addSubAction(openPhotonShutterActionsList);
	}

	return beamOnActionsList;
}

AMAction3* SXRMBBeamline::createBeamOffActions() const
{
	if(!isConnected() || PSH1406B1002Shutter_->isClosed())
		return 0;

	AMControlInfo PSH1406B1002ShutterSetpoint = PSH1406B1002Shutter_->toInfo();
	PSH1406B1002ShutterSetpoint.setValue(0);
	AMControlMoveAction3 *PSH1406B1002ShutterCloseAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(PSH1406B1002ShutterSetpoint), PSH1406B1002Shutter_);

	AMControlInfo PSH1406B1002ShutterWaitSetpoint = PSH1406B1002Shutter_->toInfo();
	PSH1406B1002ShutterWaitSetpoint.setValue(0);
	AMControlWaitAction *PSH1406B1002ShutterWaitAction = new AMControlWaitAction(new AMControlWaitActionInfo(PSH1406B1002ShutterWaitSetpoint, 10, AMControlWaitActionInfo::MatchEqual), PSH1406B1002Shutter_);

	AMListAction3 *beamOffActionsList = new AMListAction3(new AMListActionInfo3("SXRMB Beam Off", "SXRMB Beam Off"), AMListAction3::Parallel);
	beamOffActionsList->addSubAction(PSH1406B1002ShutterCloseAction);
	beamOffActionsList->addSubAction(PSH1406B1002ShutterWaitAction);

	return beamOffActionsList;
}

void SXRMBBeamline::setupSynchronizedDwellTime()
{
}

void SXRMBBeamline::setupComponents()
{
	beamlineStatus_ = new AMReadOnlyPVControl("BeamlineStatus", "BL1606-B01:ready:status", this);

	//energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:Energy:fbk", "BL1606-B1-1:Energy", "BL1606-B1-1:Energy:status", QString(), this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());
	energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:AddOns:Energy:fbk", "BL1606-B1-1:AddOns:Energy", "BL1606-B1-1:AddOns:Energy:status", "BL1606-B1-1:AddOns:Energy:stop", this, 0.05, 2.0, new AMControlStatusCheckerCLSMAXv());

	CLSSR570 *tempSR570;
	scaler_ = new CLSSIS3820Scaler("BL1606-B1-1:mcs", this);

	tempSR570 = new CLSSR570("BeamlineI0", "Amp1606-5-B10-01", this);
	scaler_->channelAt(16)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(16)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(16)->setCustomChannelName("Beamline I0");

	tempSR570 = new CLSSR570("I0", "Amp1606-5-B10-02", this);
	scaler_->channelAt(17)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(17)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(17)->setCustomChannelName("I0");

	tempSR570 = new CLSSR570("TEY", "Amp1606-5-B10-03", this);
	scaler_->channelAt(18)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(18)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(18)->setCustomChannelName("TEY");

	tempSR570 = new CLSSR570("Transmission", "Amp1606-5-B10-04", this);
	scaler_->channelAt(19)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(19)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(19)->setCustomChannelName("Transmission");
}

void SXRMBBeamline::setupDiagnostics()
{
	// the shutters used for Beam on/off control
	PSH1406B1002Shutter_ = new CLSBiStateControl("PhotonShutter2", "Photon Shutter 2", "PSH1406-B10-02:state", "PSH1406-B10-02:opr:open", "PSH1406-B10-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	VVR16064B1003Valve_ = new CLSBiStateControl("VVR16064B1003", "VVR1606-4-B10-03 Valve", "VVR1606-4-B10-03:state", "VVR1606-4-B10-03:opr:open", "VVR1606-4-B10-03:opr:close", new AMControlStatusCheckerDefault(2), this);
	VVR16064B1004Valve_ = new CLSBiStateControl("VVR16064B1004", "VVR1606-4-B10-04 Valve", "VVR1606-4-B10-04:state", "VVR1606-4-B10-04:opr:open", "VVR1606-4-B10-04:opr:close", new AMControlStatusCheckerDefault(2), this);
	VVR16064B1006Valve_ = new CLSBiStateControl("VVR16064B1006", "VVR1606-4-B10-06 Valve", "VVR1606-4-B10-06:state", "VVR1606-4-B10-06:opr:open", "VVR1606-4-B10-06:opr:close", new AMControlStatusCheckerDefault(2), this);
	VVR16064B1007Valve_ = new CLSBiStateControl("VVR16064B1007", "VVR1606-4-B10-07 Valve", "VVR1606-4-B10-07:state", "VVR1606-4-B10-07:opr:open", "VVR1606-4-B10-07:opr:close", new AMControlStatusCheckerDefault(2), this);
	VVR16065B1001Valve_ = new CLSBiStateControl("VVR16065B1001", "VVR1606-5-B10-01 Valve", "VVR1606-5-B10-01:state", "VVR1606-5-B10-01:opr:open", "VVR1606-5-B10-01:opr:close", new AMControlStatusCheckerDefault(2), this);

	beamlineControlShutterSet_ = new AMControlSet(this);
	beamlineControlShutterSet_->addControl(PSH1406B1002Shutter_);
	beamlineControlShutterSet_->addControl(VVR16064B1003Valve_);
	beamlineControlShutterSet_->addControl(VVR16064B1004Valve_);
	beamlineControlShutterSet_->addControl(VVR16064B1006Valve_);
	beamlineControlShutterSet_->addControl(VVR16064B1007Valve_);
	beamlineControlShutterSet_->addControl(VVR16065B1001Valve_);
}

void SXRMBBeamline::setupSampleStage()
{
	// these motors actually have the encoder reporting positions to the "mm:sp" PV
	microprobeSampleStageX_ = new AMPVwStatusControl("MicroprobeSampleStageX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:status", "SVM1606-5-B10-07:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	microprobeSampleStageY_ = new AMPVwStatusControl("MicroprobeSampleStageY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:status", "SVM1606-5-B10-08:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	microprobeSampleStageZ_ = new AMPVwStatusControl("MicroprobeSampleStageZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:status", "SVM1606-5-B10-09:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());

	microprobeSampleStageControlSet_ = new AMControlSet(this);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageX_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageY_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageZ_);

	// Solid State Endstation sample stage
	solidStateSampleStageX_ = new AMPVwStatusControl("SolidStateSampleStageX", "SMTR0000-E01-01:mm:sp", "SMTR0000-E01-01:mm", "SMTR0000-E01-01:status", "MSD1606-5-01:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	solidStateSampleStageY_ = new AMPVwStatusControl("SolidStateSampleStageY", "SMTR0000-E01-02:mm:sp", "SMTR0000-E01-02:mm", "SMTR0000-E01-02:status", "MSD1606-5-02:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	solidStateSampleStageZ_ = new AMPVwStatusControl("SolidStateSampleStageZ", "SMTR0000-E01-03:mm:sp", "SMTR0000-E01-03:mm", "SMTR0000-E01-03:status", "MSD1606-5-03:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	solidStateSampleStageR_ = new AMPVwStatusControl("SolidStateSampleStageR", "SMTR0000-E01-04:dgr:sp", "SMTR0000-E01-04:dgr", "SMTR0000-E01-04:status", "MSD1606-5-04:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());

	solidStateSampleStageControlSet_ = new AMControlSet(this);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageX_);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageY_);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageZ_);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageR_);

	// Ambiant Endstation sample state
	ambiantSampleStageX_ = new AMPVwStatusControl("ambiantSampleStageX", "SMTR0000-E07-15:mm:sp", "SMTR0000-E07-15:mm", "SMTR0000-E07-15:status", "SMTR0000-E07-15:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	ambiantSampleStageZ_ = new AMPVwStatusControl("ambiantSampleStageZ", "SMTR0000-E07-16:mm:sp", "SMTR0000-E07-16:mm", "SMTR0000-E07-16:status", "SMTR0000-E07-16:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	ambiantSampleHolderZ_ = new AMPVwStatusControl("ambiantSampleHolderZ", "SMTR0000-E07-04:mm:sp", "SMTR0000-E07-04:mm", "SMTR0000-E07-04:status", "SMTR0000-E07-04:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());
	ambiantSampleHolderR_ = new AMPVwStatusControl("ambiantSampleHolderR", "SMTR0000-E07-05:dgr:sp", "SMTR0000-E07-05:dgr", "SMTR0000-E07-05:status", "SMTR0000-E07-05:stop", this, 0.005, 2.0, new AMControlStatusCheckerCLSMAXv());

	ambiantWithGasChamberSampleStageControlSet_ = new AMControlSet(this);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleStageX_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleStageZ_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleHolderZ_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleHolderR_);

	ambiantWithoutGasChamberSampleStageControlSet_ = new AMControlSet(this);
	ambiantWithoutGasChamberSampleStageControlSet_->addControl(ambiantSampleStageX_);
	ambiantWithoutGasChamberSampleStageControlSet_->addControl(ambiantSampleStageZ_);
}

void SXRMBBeamline::setupDetectors()
{
	brukerDetector_ = new SXRMBBrukerDetector("Bruker", "Bruker XRF detector", this);
	fourElementVortexDetector_ = new SXRMBFourElementVortexDetector("FourElementVortex", "Four element Vortex detector", this);

	beamlineI0Detector_ = new CLSBasicScalerChannelDetector("BeamlineI0Detector", "Beamline I0 Detector", scaler_, 16, this);
	scaler_->channelAt(16)->setDetector(beamlineI0Detector_);

	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 17, this);
	scaler_->channelAt(17)->setDetector(i0Detector_);

	teyDetector_ = new CLSBasicScalerChannelDetector("TEYDetector", "TEY Detector", scaler_, 18, this);
	scaler_->channelAt(18)->setDetector(teyDetector_);

	transmissionDetector_ = new CLSBasicScalerChannelDetector("TransmissionDetector", "Transmission Detector", scaler_, 18, this);
	scaler_->channelAt(18)->setDetector(i0Detector_);
}

void SXRMBBeamline::setupControlSets()
{

}

void SXRMBBeamline::setupMono()
{

}

void SXRMBBeamline::setupMotorGroup()
{
	motorGroup_ = new AMMotorGroup(this);

	AMMotorGroupObject *motorObject;

	// Microprobe motor group
	motorObject = new AMMotorGroupObject("Microprobe Stage - X, Z, Y",
										 QStringList() << "X" << "Z" << "Y",
										 QStringList() << "mm" << "mm" << "mm",
										 QList<AMControl *>() << microprobeSampleStageX_ << microprobeSampleStageZ_ << microprobeSampleStageY_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);

	// Solidstate motor group
	motorObject = new AM4DMotorGroupObject("Solid State - X, Z, Y, R",
										 QStringList() << "X" << "Z" << "Y" << "R",
										 QStringList() << "mm" << "mm" << "mm" << "deg",
										 QList<AMControl *>() << solidStateSampleStageX_ << solidStateSampleStageZ_ << solidStateSampleStageY_ << solidStateSampleStageR_,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal << AMMotorGroupObject::Other,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Rotational,
										 this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);

	// Ambiant with gas chamber motor group
	motorObject = new AMMotorGroupObject("Ambiant With Gas Chamber - Z, R",
										   QStringList() << "Z" << "R",
										   QStringList() << "mm" << "deg",
										   QList<AMControl *>() << ambiantSampleHolderZ_ << ambiantSampleHolderR_ ,
										   QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Vertical << AMMotorGroupObject::Horizontal,
										   QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Rotational,
										   this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);

	// Ambiant without gas chamber motor group
	motorObject = new AMMotorGroupObject("Ambiant Without Gas Chamber - X, Z",
										   QStringList() << "X" << "Z",
										   QStringList() << "mm" << "mm",
										   QList<AMControl *>() << ambiantSampleStageX_ << ambiantSampleStageZ_ ,
										   QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical,
										   QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
										   this);
	motorGroup_->addMotorGroupObject(motorObject->name(), motorObject);

}

void SXRMBBeamline::setupControlsAsDetectors()
{
	energyFeedbackControl_ = new AMReadOnlyPVControl("EnergyFeedback", "BL1606-B1-1:Energy:fbk", this);
	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "Energy Feedback", energyFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
}

void SXRMBBeamline::setupExposedControls()
{
	addExposedControl(energy_);
}

void SXRMBBeamline::setupExposedDetectors()
{
	addExposedDetector(energyFeedbackDetector_);
	addExposedDetector(brukerDetector_);
	addExposedDetector(fourElementVortexDetector_);
}

void SXRMBBeamline::setupConnections()
{
	connect(CLSStorageRing::sr1(), SIGNAL(beamAvaliability(bool)), this, SLOT(onStorageRingBeamAvailabilityChanged(bool)));
	connect(beamlineStatus_, SIGNAL(valueChanged(double)), this, SLOT(onBeamlineStatusPVValueChanged(double)));
	connect(beamlineStatus_, SIGNAL(connected(bool)), this, SLOT(onBeamlineStatusPVConnected(bool)));

	connect(energy_, SIGNAL(connected(bool)), this, SLOT(onEnergyPVConnected(bool)));
	connect(microprobeSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(solidStateSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(ambiantWithGasChamberSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(ambiantWithoutGasChamberSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(beamlineControlShutterSet_, SIGNAL(connected(bool)), this, SLOT(onBeamlineControlShuttersConnected(bool)));
	connect(beamlineControlShutterSet_, SIGNAL(controlSetTimedOut()), this, SIGNAL(beamlineControlShuttersTimeout()));

	if (beamlineStatus_->isConnected()) {
		onBeamlineStatusPVConnected(true);
	}
}

void SXRMBBeamline::beamAvailabilityHelper()
{
	bool beamOn = (CLSStorageRing::sr1()->beamAvailable()) && ( beamlineStatus_->value() == 1);

	emit beamAvaliability(beamOn);
}

void SXRMBBeamline::sampleStageConnectHelper()
{
	// check the available endstation if it is NOT assigned yet and whether sample stage is connected or not
	if (currentEndstation_ == SXRMB::Invalid) {
		if (microprobeSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::Microprobe );

		else if (solidStateSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::SolidState );

		else if (ambiantWithGasChamberSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::AmbiantWithGasChamber );

		else if (ambiantWithoutGasChamberSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::AmbiantWithoutGasChamber );
	}
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

void SXRMBBeamline::onStorageRingBeamAvailabilityChanged(bool)
{
	beamAvailabilityHelper();
}

void SXRMBBeamline::onBeamlineStatusPVValueChanged(double)
{
	beamAvailabilityHelper();
}

void SXRMBBeamline::onBeamlineStatusPVConnected(bool value) {
	connectedHelper();

	if (value) {
		beamAvailabilityHelper();
	}
}

void SXRMBBeamline::onEnergyPVConnected(bool) {
	connectedHelper();
}

void SXRMBBeamline::onSampleStagePVsConnected(bool) {
	sampleStageConnectHelper();
	connectedHelper();
}

void SXRMBBeamline::onBeamlineControlShuttersConnected(bool) {
	connectedHelper();
}

AMPVwStatusControl * SXRMBBeamline::ambiantSampleStageX() const
{
	return ambiantSampleStageX_;
}

AMPVwStatusControl * SXRMBBeamline::ambiantSampleStageZ() const
{
	return ambiantSampleStageZ_;
}

AMPVwStatusControl * SXRMBBeamline::ambiantSampleHolderZ() const
{
	return ambiantSampleHolderZ_;
}

AMPVwStatusControl * SXRMBBeamline::ambiantSampleHolderR() const
{
	return ambiantSampleHolderR_;
}
