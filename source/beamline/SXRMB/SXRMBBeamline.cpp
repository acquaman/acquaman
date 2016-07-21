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


#include "beamline/CLS/CLSBeamStatusShutters.h"
#include "beamline/CLS/CLSBeamStatusValves.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

#include "util/AMErrorMonitor.h"

SXRMBBeamline::SXRMBBeamline()
	: CLSBeamline("SXRMB", "SXRMB Beamline")
{
	beamlineEnergyLowEnd_ = 1300;   //   --- Al 1486 ev
	beamlineEnergyHighEnd_ = 10000; // ---
	currentEndstation_ = SXRMB::UnkownEndstation;

	setupSynchronizedDwellTime();
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupHVControls();
	setupExposedControls();
	setupExposedDetectors();
	setupConnections();

	initializeBeamline();
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
			removeExposedControl(jjSlits_->horizontalGap());

			removeExposedDetector(beamlineI0Detector_);
			removeExposedDetector(teyDetector_);

			break;

		case SXRMB::AmbiantWithGasChamber:

			removeExposedControl(ambiantSampleHolderZ_);
			removeExposedControl(ambiantSampleHolderR_);
			removeExposedControl(jjSlits_->horizontalGap());

			removeExposedDetector(beamlineI0Detector_);
			removeExposedDetector(i0Detector_);
			removeExposedDetector(transmissionDetector_);

			break;

		case SXRMB::AmbiantWithoutGasChamber:

			removeExposedControl(ambiantSampleStageX_);
			removeExposedControl(ambiantSampleStageZ_);
			removeExposedControl(jjSlits_->horizontalGap());

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
			addExposedControl(jjSlits_->horizontalGap());

			addExposedDetector(beamlineI0Detector_);
			addExposedDetector(teyDetector_);

			break;
		case SXRMB::AmbiantWithGasChamber:

			addExposedControl(ambiantSampleStageX_);
			addExposedControl(ambiantSampleHolderZ_);
			addExposedControl(ambiantSampleHolderR_);
			addExposedControl(jjSlits_->horizontalGap());

			addExposedDetector(beamlineI0Detector_);
			addExposedDetector(i0Detector_);
			addExposedDetector(transmissionDetector_);

			break;

		case SXRMB::AmbiantWithoutGasChamber:

			addExposedControl(ambiantSampleStageX_);
			addExposedControl(ambiantSampleStageZ_);
			addExposedControl(jjSlits_->horizontalGap());

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

		endstationControl_->move(currentEndstation_);
	}
}

AMSlits* SXRMBBeamline::jjSlits() const
{
	return jjSlits_;
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
	case SXRMB::AmbiantWithGasChamber:
	case SXRMB::AmbiantWithoutGasChamber:
		statusControl = ambiantSampleStageX();
		break;
	case SXRMB::Microprobe:
		statusControl = microprobeSampleStageX();
		break;
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

AMPVwStatusControl *SXRMBBeamline::ambiantTableHeight() const
{
	return ambiantTableHeight_;
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

CLSCrossHairGeneratorControl * SXRMBBeamline::crossHairGenerator() const
{
	return crossHairGenerator_;
}

SXRMBCrystalChangeModel * SXRMBBeamline::crystalSelection() const
{
	return crystalSelection_;
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
	return motorGroup_->motorGroupObject("Ambiant With Gas Chamber - X, Z, R");
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
	return CLSBeamline::isConnected() && sampleStageConnected;
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

AMXRFDetector *SXRMBBeamline::xrfDetector(SXRMB::FluorescenceDetectors detectorType) const
{
	AMXRFDetector * XRFDetector = 0;

	if (detectorType.testFlag(SXRMB::BrukerDetector)){
		XRFDetector = brukerDetector();
	} else if (detectorType.testFlag(SXRMB::FourElementDetector)) {
		XRFDetector = fourElementVortexDetector();
	}

	return XRFDetector;
}

AMControlSet *SXRMBBeamline::beamlineHVControlSet() const
{
	return beamlineHVControlSet_;
}

AMControlSet *SXRMBBeamline::beamlinePersistentHVControlSet() const
{
	return beamlinePersistentHVControlSet_;
}

CLSHVControl *SXRMBBeamline::i0HVControl() const
{
	return i0HVControl_;
}

CLSHVControl *SXRMBBeamline::teyHVControl() const
{
	return teyHVControl_;
}

CLSHVControl *SXRMBBeamline::microprobeTEYHVControl() const
{
	return microprobeTEYHVControl_;
}

CLSHVControl *SXRMBBeamline::ambiantIC0HVControl() const
{
	return ambiantIC0HVControl_;
}

CLSHVControl *SXRMBBeamline::ambiantIC1HVControl() const
{
	return ambiantIC1HVControl_;
}

void SXRMBBeamline::setupSynchronizedDwellTime()
{
}

void SXRMBBeamline::setupComponents()
{
	beamStatusShutters_ = new CLSBeamStatusShutters(QString("%1 Beam Status Shutters").arg(beamlineName_), this);
	beamStatusValves_ = new CLSBeamStatusValves(QString("%1 Beam Status Valves").arg(beamlineName_), this);

	createBeamlineStatus(beamStatusShutters_, beamStatusValves_);

	crossHairGenerator_ = new CLSCrossHairGeneratorControl("MUX1606-601", "VLG1606-601", this);
	crystalSelection_ = new SXRMBCrystalChangeModel(this);
	endstationControl_ = new AMPVControl("SXRMB Endstation", "BL1606-B1-1:AddOns:Endstation:fbk", "BL1606-B1-1:AddOns:Endstation");

	jjSlits_ = new AMSlits("JJSlits", this);
	jjSlits_->setUpperBlade(new CLSMAXvMotor("SMTR1606-4-B10-02", "SMTR1606-4-B10-02", "SMTR1606-4-B10-02", false, 0.05, 2.0, this));
	jjSlits_->setLowerBlade(new CLSMAXvMotor("SMTR1606-4-B10-01", "SMTR1606-4-B10-01", "SMTR1606-4-B10-01", false, 0.05, 2.0, this));
	jjSlits_->setInboardBlade(new CLSMAXvMotor("SMTR1606-4-B10-03", "SMTR1606-4-B10-03", "SMTR1606-4-B10-03", false, 0.05, 2.0, this), AMSlit::OpensNegatively);
	jjSlits_->setOutboardBlade(new CLSMAXvMotor("SMTR1606-4-B10-04", "SMTR1606-4-B10-04", "SMTR1606-4-B10-04", false, 0.05, 2.0, this), AMSlit::OpensPositively);

	jjSlits_->setVerticalSlitOpenGapValue(16);
	jjSlits_->setVerticalSlitClosedGapValue(9);
	jjSlits_->setHorizontalSlitOpenGapValue(16);
	jjSlits_->setHorizontalSlitClosedGapValue(0);

	//energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:Energy:fbk", "BL1606-B1-1:Energy", "BL1606-B1-1:Energy:status", QString(), this, 0.1, 2.0, new AMControlStatusCheckerCLSMAXv());
	energy_ = new AMPVwStatusControl("Energy", "BL1606-B1-1:AddOns:Energy:fbk", "BL1606-B1-1:AddOns:Energy", "BL1606-B1-1:AddOns:Energy:status", "BL1606-B1-1:AddOns:Energy:stop", this, 0.05, 2.0, new CLSMAXvControlStatusChecker());

	CLSSR570 *tempSR570;
	scaler_ = new CLSSIS3820Scaler("BL1606-B1-1:mcs", this);

	tempSR570 = new CLSSR570("BeamlineI0", "Amp1606-5-B10-01", this);
	scaler_->channelAt(16)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(16)->setVoltagRange(AMRange(0.1, 2.0));
	scaler_->channelAt(16)->setCustomChannelName("Beamline I0");

	tempSR570 = new CLSSR570("I0", "Amp1606-5-B10-02", this);
	scaler_->channelAt(17)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(17)->setVoltagRange(AMRange(0.1, 2.0));
	scaler_->channelAt(17)->setCustomChannelName("I0");

	tempSR570 = new CLSSR570("TEY", "Amp1606-5-B10-03", this);
	scaler_->channelAt(18)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(18)->setVoltagRange(AMRange(0.1, 2.0));
	scaler_->channelAt(18)->setCustomChannelName("TEY");

	tempSR570 = new CLSSR570("Transmission", "Amp1606-5-B10-04", this);
	scaler_->channelAt(19)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(19)->setVoltagRange(AMRange(0.1, 2.0));
	scaler_->channelAt(19)->setCustomChannelName("Transmission");
}

void SXRMBBeamline::setupDiagnostics()
{
	// the shutters used for Beam on/off control
	AMReadOnlyPVControl * SSH1406B1001Shutter = new AMReadOnlyPVControl("FE Safety Shutter", "SSH1406-B10-01:state", this);
	PSH1406B1002Shutter_ = new CLSExclusiveStatesControl("Photon Shutter 2", "PSH1406-B10-02:state", "PSH1406-B10-02:opr:open", "PSH1406-B10-02:opr:close", this, "Photon Shutter 2");

	beamStatusShutters_->setSafetyShutter(SSH1406B1001Shutter, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	beamStatusShutters_->addShutter(PSH1406B1002Shutter_, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed, 1);

	// the valves used for Beam on/off control
	CLSExclusiveStatesControl* VVR16064B1003Valve = new CLSExclusiveStatesControl("VVR16064B1003", "VVR1606-4-B10-03:state", "VVR1606-4-B10-03:opr:open", "VVR1606-4-B10-03:opr:close", this, "VVR1606-4-B10-03 Valve");
	CLSExclusiveStatesControl* VVR16064B1004Valve = new CLSExclusiveStatesControl("VVR16064B1004", "VVR1606-4-B10-04:state", "VVR1606-4-B10-04:opr:open", "VVR1606-4-B10-04:opr:close", this, "VVR1606-4-B10-04 Valve");
	// NOT THIS ONE! It's connected to the pump on the mono
	//CLSBiStateControl *VVR16064B1005Valve;
	CLSExclusiveStatesControl* VVR16064B1006Valve = new CLSExclusiveStatesControl("VVR16064B1006", "VVR1606-4-B10-06:state", "VVR1606-4-B10-06:opr:open", "VVR1606-4-B10-06:opr:close", this, "VVR1606-4-B10-06 Valve");
	CLSExclusiveStatesControl* VVR16064B1007Valve = new CLSExclusiveStatesControl("VVR16064B1007", "VVR1606-4-B10-07:state", "VVR1606-4-B10-07:opr:open", "VVR1606-4-B10-07:opr:close", this, "VVR1606-4-B10-07 Valve");
	CLSExclusiveStatesControl* VVR16065B1001Valve = new CLSExclusiveStatesControl("VVR16065B1001", "VVR1606-5-B10-01:state", "VVR1606-5-B10-01:opr:open", "VVR1606-5-B10-01:opr:close", this, "VVR1606-5-B10-01 Valve");

	beamStatusValves_->addValve(VVR16064B1003Valve, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed, 1);
	beamStatusValves_->addValve(VVR16064B1004Valve, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed, 2);
	beamStatusValves_->addValve(VVR16064B1006Valve, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed, 3);
	beamStatusValves_->addValve(VVR16064B1007Valve, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed, 4);
	beamStatusValves_->addValve(VVR16065B1001Valve, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed, 5);
}

void SXRMBBeamline::setupSampleStage()
{
	// these motors actually have the encoder reporting positions to the "mm:sp" PV
	microprobeSampleStageX_ = new AMPVwStatusControl("MicroprobeSampleStageX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:status", "SVM1606-5-B10-07:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	microprobeSampleStageY_ = new AMPVwStatusControl("MicroprobeSampleStageY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:status", "SVM1606-5-B10-08:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	microprobeSampleStageZ_ = new AMPVwStatusControl("MicroprobeSampleStageZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm:fbk", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:status", "SVM1606-5-B10-09:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());

	microprobeSampleStageControlSet_ = new AMControlSet(this);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageX_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageY_);
	microprobeSampleStageControlSet_->addControl(microprobeSampleStageZ_);

	// Solid State Endstation sample stage
	solidStateSampleStageX_ = new AMPVwStatusControl("SolidStateSampleStageX", "SMTR0000-E01-01:mm:sp", "SMTR0000-E01-01:mm", "SMTR0000-E01-01:status", "SMTR0000-E01-01:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	solidStateSampleStageY_ = new AMPVwStatusControl("SolidStateSampleStageY", "SMTR0000-E01-02:mm:sp", "SMTR0000-E01-02:mm", "SMTR0000-E01-02:status", "SMTR0000-E01-02:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	solidStateSampleStageZ_ = new AMPVwStatusControl("SolidStateSampleStageZ", "SMTR0000-E01-03:mm:sp", "SMTR0000-E01-03:mm", "SMTR0000-E01-03:status", "SMTR0000-E01-03:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	solidStateSampleStageR_ = new AMPVwStatusControl("SolidStateSampleStageR", "SMTR0000-E01-04:dgr:sp", "SMTR0000-E01-04:dgr", "SMTR0000-E01-04:status", "SMTR0000-E01-04:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());

	solidStateSampleStageControlSet_ = new AMControlSet(this);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageX_);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageY_);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageZ_);
	solidStateSampleStageControlSet_->addControl(solidStateSampleStageR_);

	// Ambiant Endstation sample stage
	ambiantTableHeight_ = new AMPVwStatusControl("Ambiant Table Height", "TBL0000-E07-01:Y:mm:fbk", "TBL0000-E07-01:Y:mm", "TBL0000-E07-01:Y:mm:status", "BL1606-B1-1:AddOns:Ambiant:TableHeight:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	ambiantSampleStageX_ = new AMPVwStatusControl("ambiantSampleStageX", "SMTR0000-E07-15:mm:sp", "SMTR0000-E07-15:mm", "SMTR0000-E07-15:status", "SMTR0000-E07-15:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	ambiantSampleStageZ_ = new AMPVwStatusControl("ambiantSampleStageZ", "SMTR0000-E07-16:mm:sp", "SMTR0000-E07-16:mm", "SMTR0000-E07-16:status", "SMTR0000-E07-16:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	ambiantSampleHolderZ_ = new AMPVwStatusControl("ambiantSampleHolderZ", "SMTR0000-E07-04:mm:sp", "SMTR0000-E07-04:mm", "SMTR0000-E07-04:status", "SMTR0000-E07-04:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());
	ambiantSampleHolderR_ = new AMPVwStatusControl("ambiantSampleHolderR", "SMTR0000-E07-05:dgr:sp", "SMTR0000-E07-05:dgr", "SMTR0000-E07-05:status", "SMTR0000-E07-05:stop", this, 0.005, 2.0, new CLSMAXvControlStatusChecker());

	ambiantWithGasChamberSampleStageControlSet_ = new AMControlSet(this);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantTableHeight_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleStageX_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleStageZ_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleHolderZ_);
	ambiantWithGasChamberSampleStageControlSet_->addControl(ambiantSampleHolderR_);

	ambiantWithoutGasChamberSampleStageControlSet_ = new AMControlSet(this);
	ambiantWithoutGasChamberSampleStageControlSet_->addControl(ambiantTableHeight_);
	ambiantWithoutGasChamberSampleStageControlSet_->addControl(ambiantSampleStageX_);
	ambiantWithoutGasChamberSampleStageControlSet_->addControl(ambiantSampleStageZ_);
}

void SXRMBBeamline::setupDetectors()
{
	brukerDetector_ = new SXRMBBrukerDetector("Bruker", "Bruker XRF detector", this);
	fourElementVortexDetector_ = new SXRMBFourElementVortexDetector("FourElementVortex", "4 elements Vortex detector", this);

	addSynchronizedXRFDetector(brukerDetector_);
	addSynchronizedXRFDetector(fourElementVortexDetector_);

	// initialize the scaler detectors
	beamlineI0Detector_ = new CLSBasicScalerChannelDetector("BeamlineI0Detector", "Beamline I0 Detector", scaler_, 16, this);
	scaler_->channelAt(16)->setDetector(beamlineI0Detector_);

	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 17, this);
	scaler_->channelAt(17)->setDetector(i0Detector_);

	teyDetector_ = new CLSBasicScalerChannelDetector("TEYDetector", "TEY Detector", scaler_, 18, this);
	scaler_->channelAt(18)->setDetector(teyDetector_);

	transmissionDetector_ = new CLSBasicScalerChannelDetector("TransmissionDetector", "Transmission Detector", scaler_, 19, this);
	scaler_->channelAt(19)->setDetector(i0Detector_);
}

void SXRMBBeamline::setupControlSets()
{
	requiredControls_->addControl(endstationControl_);
	requiredControls_->addControl(energy_);
	requiredControls_->addControl(jjSlits_);
}

void SXRMBBeamline::setupMono()
{

}

void SXRMBBeamline::setupMotorGroup()
{
	motorGroup_ = new AMMotorGroup(this);

	AMMotorGroupObject *motorObject;

	// Microprobe motor group
	motorObject = new AMMotorGroupObject("Microprobe Stage - X, Z, Y", this);

	motorObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
						 "X", microprobeSampleStageX_,
						 "", 0);

	motorObject->setDirectionAxis(AMMotorGroupObject::NormalMotion,
						 "Y", microprobeSampleStageY_,
						 "", 0);

	motorObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
						 "Z", microprobeSampleStageZ_,
						 "", 0);

	motorGroup_->addMotorGroupObject(motorObject);

	// Solidstate motor group
	motorObject = new AMMotorGroupObject("Solid State - X, Z, Y, R", this);
	motorObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
						 "X", solidStateSampleStageX_,
						 "", 0);

	motorObject->setDirectionAxis(AMMotorGroupObject::NormalMotion,
						 "Y", solidStateSampleStageY_,
						 "", 0);

	motorObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
						 "Z", solidStateSampleStageZ_,
						 "R", solidStateSampleStageR_);

	motorGroup_->addMotorGroupObject(motorObject);

	// Ambiant with gas chamber motor group
	motorObject = new AMMotorGroupObject("Ambiant With Gas Chamber - X, Z, R", this);
	motorObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
						 "X", ambiantSampleStageX_,
						 "", 0);

	motorObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
						 "Z", ambiantSampleHolderZ_,
						 "R", ambiantSampleHolderR_);

	motorGroup_->addMotorGroupObject(motorObject);

	// Ambiant without gas chamber motor group
	motorObject = new AMMotorGroupObject("Ambiant Without Gas Chamber - X, Z", this);
	motorObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
						 "X", ambiantSampleStageX_,
						 "", 0);
	motorObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
						 "Z", ambiantSampleStageZ_,
						 "", 0);

	motorGroup_->addMotorGroupObject(motorObject);

}

void SXRMBBeamline::setupControlsAsDetectors()
{
	energyFeedbackControl_ = new AMReadOnlyPVControl("EnergyFeedback", "BL1606-B1-1:Energy:fbk", this);
	energyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EnergyFeedback", "Energy Feedback", energyFeedbackControl_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
}

void SXRMBBeamline::setupHVControls()
{
	i0HVControl_ = new CLSHVControl("I0", "PS1606506:100", ":vmon", ":v0set", ":pwonoff", ":status", ":imon");
	teyHVControl_ = new CLSHVControl("TEY", "PS1606506:101", ":vmon", ":v0set", ":pwonoff", ":status", ":imon");
	microprobeTEYHVControl_ = new CLSHVControl("Microprobe TEY", "PS1606506:102", ":vmon", ":v0set", ":pwonoff", ":status", ":imon");
	ambiantIC0HVControl_ = new CLSHVControl("IC0", "PS1606506:103", ":vmon", ":v0set", ":pwonoff", ":status", ":imon");
	ambiantIC1HVControl_ = new CLSHVControl("IC1", "PS1606506:104", ":vmon", ":v0set", ":pwonoff", ":status", ":imon");

	beamlineHVControlSet_ = new AMControlSet(this);
	beamlineHVControlSet_->setName("SXRMB HV Controls");
	beamlineHVControlSet_->addControl(i0HVControl_);
	beamlineHVControlSet_->addControl(teyHVControl_);
	beamlineHVControlSet_->addControl(microprobeTEYHVControl_);
	beamlineHVControlSet_->addControl(ambiantIC0HVControl_);
	beamlineHVControlSet_->addControl(ambiantIC1HVControl_);

	beamlinePersistentHVControlSet_ = new AMControlSet(this);
	beamlinePersistentHVControlSet_->setName("HV Controls");
	beamlinePersistentHVControlSet_->addControl(i0HVControl_);
	beamlinePersistentHVControlSet_->addControl(teyHVControl_);
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
	connect(jjSlits_, SIGNAL(connected(bool)), this, SLOT(onBeamlineComponentConnected()) );

	connect(PSH1406B1002Shutter_, SIGNAL(statusChanged(AMControl *)), this, SLOT(onPhotonShutterStateChanged()));

	connect(endstationControl_, SIGNAL(connected(bool)), this, SLOT(onEndstationPVConnected(bool)));
	connect(endstationControl_, SIGNAL(valueChanged(double)), this, SLOT(onEndstationPVValueChanged(double)));

	connect(microprobeSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(solidStateSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(ambiantWithGasChamberSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));
	connect(ambiantWithoutGasChamberSampleStageControlSet_, SIGNAL(connected(bool)), this, SLOT(onSampleStagePVsConnected(bool)));

	if (endstationControl_->isConnected()) {
		onEndstationPVValueChanged(endstationControl_->value());
	}
}

/// ==================== protected slots =======================

void SXRMBBeamline::onPhotonShutterStateChanged()
{
	if (PSH1406B1002Shutter_->isClosed()) {
		teyHVControl_->onPowerOff();
		microprobeTEYHVControl_->onPowerOff();
	}
}

void SXRMBBeamline::onEndstationPVConnected(bool value)
{
	if (value) {
		switchEndstation(SXRMB::Endstation(endstationControl_->value()));
	}
}

void SXRMBBeamline::onEndstationPVValueChanged(double value)
{
	Q_UNUSED(value)

	switchEndstation(SXRMB::Endstation(endstationControl_->value()));
}

void SXRMBBeamline::onSampleStagePVsConnected(bool) {
	// check the available endstation if it is NOT assigned yet and whether sample stage is connected or not
	if (currentEndstation_ == SXRMB::UnkownEndstation) {
		if (microprobeSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::Microprobe );

		else if (solidStateSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::SolidState );

		else if (ambiantWithGasChamberSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::AmbiantWithGasChamber );

		else if (ambiantWithoutGasChamberSampleStageControlSet_->isConnected())
			switchEndstation( SXRMB::AmbiantWithoutGasChamber );
	}

	onBeamlineComponentConnected();
}

void SXRMBBeamline::createBeamlineStatus(CLSShutters *shutters, CLSValves *valves)
{
	CLSBeamline::createBeamlineStatus(shutters, valves);

	AMReadOnlyPVControl *beamlineStatusPV = new AMReadOnlyPVControl("SXRMB Beamline Status PV", "BL1606-B01:ready:status", this);
	beamlineStatus_->setBeamlineStatusPVControl(beamlineStatusPV, SXRMBBeamline::BeamReady);
}
