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


#include "SGMBeamline.h"

#include "beamline/SGM/SGMMAXvMotor.h"
#include "util/AMErrorMonitor.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSAdvancedScalerChannelDetector.h"

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0){
		instance_ = new SGMBeamline();
		instance_->initializeBeamlineSupport();
	}

	return static_cast<SGMBeamline*>(instance_);

}

SGMBeamline::~SGMBeamline()
{
}

AMControl * SGMBeamline::energy() const
{
	return energy_;
}

AMControl * SGMBeamline::exitSlitGap() const
{
	return exitSlitGap_;
}

AMControl * SGMBeamline::grating() const
{
	return grating_;
}

SGMMAXvMotor * SGMBeamline::ssaManipulatorX() const
{
	return ssaManipulatorX_;
}

SGMMAXvMotor * SGMBeamline::ssaManipulatorY() const
{
	return ssaManipulatorY_;
}

SGMMAXvMotor * SGMBeamline::ssaManipulatorZ() const
{
	return ssaManipulatorZ_;
}

SGMMAXvMotor * SGMBeamline::ssaManipulatorRot() const
{
	return ssaManipulatorRot_;
}

AMMotorGroup * SGMBeamline::ssaManipulatorMotorGroup() const
{
	return ssaManipulatorMotorGroup_;
}

CLSSIS3820Scaler * SGMBeamline::scaler() const
{
	return scaler_;
}

void SGMBeamline::setupBeamlineComponents()
{
	// Energy
	energy_ = new AMPVwStatusControl("energy", "BL1611-ID-1:Energy:fbk", "BL1611-ID-1:Energy", "BL1611-ID-1:ready", "SMTR16114I1002:stop", this, 0.25);
	energy_->setDescription("Energy");

	// Exit Slit Gap
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", "PSL16114I1004:Y:mm:fbk", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "SMTR16114I1017:stop", this, 0.5);
	exitSlitGap_->setDescription("Exit Slit Gap");

	// Grating
	grating_ = new AMPVwStatusControl("grating", "BL1611-ID-1:AddOns:grating", "BL1611-ID-1:AddOns:grating", "SMTR16114I1016:state", "SMTR16114I1016:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	grating_->setDescription("Grating Selection");
	grating_->setAttemptMoveWhenWithinTolerance(false);

	// SSA Manipulators
	ssaManipulatorX_ = new SGMMAXvMotor("ssaManipulatorX", "SMTR16114I1022", "SSA Inboard/Outboard", true, 0.2, 2.0, this);
	ssaManipulatorX_->setContextKnownDescription("X");

	ssaManipulatorY_ = new SGMMAXvMotor("ssaManipulatorY", "SMTR16114I1023", "SSA Upstream/Downstream", true, 0.2, 2.0, this);
	ssaManipulatorY_->setContextKnownDescription("Y");

	ssaManipulatorZ_ = new SGMMAXvMotor("ssaManipulatorZ", "SMTR16114I1024", "SSA Up/Down", true, 0.2, 2.0, this);
	ssaManipulatorZ_->setContextKnownDescription("Z");

	ssaManipulatorRot_ = new SGMMAXvMotor("ssaManipulatorRot", "SMTR16114I1025", "SSA Rotation", false, 0.2, 2.0, this);
	ssaManipulatorRot_->setContextKnownDescription("R");

	// Setup Scaler and SR570
	CLSSR570 *tempSR570;

	scaler_ = new CLSSIS3820Scaler("BL1611-ID-1:mcs", this);

	tempSR570 = new CLSSR570("TEY", "Amp1611-4-21", this);
	scaler_->channelAt(0)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(0)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(0)->setCustomChannelName("TEY");

	tempSR570 = new CLSSR570("I0", "Amp1611-4-22", this);
	scaler_->channelAt(1)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(1)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(1)->setCustomChannelName("I0");

	tempSR570 = new CLSSR570("TFY PD", "Amp1611-4-23", this);
	scaler_->channelAt(2)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(2)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(2)->setCustomChannelName("TFY PD ");

	tempSR570 = new CLSSR570("PD", "Amp1611-4-24", this);
	scaler_->channelAt(3)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(3)->setVoltagRange(AMRange(1.0, 6.5));
	scaler_->channelAt(3)->setCustomChannelName("PD");

	scaler_->channelAt(4)->setCustomChannelName("UP");
	scaler_->channelAt(5)->setCustomChannelName("DOWN");
	scaler_->channelAt(6)->setCustomChannelName("FPD1");
	scaler_->channelAt(7)->setCustomChannelName("FPD2");
	scaler_->channelAt(8)->setCustomChannelName("FPD3");
	scaler_->channelAt(9)->setCustomChannelName("FPD4");
	scaler_->channelAt(10)->setCustomChannelName("FPD5");
}

void SGMBeamline::setupMotorGroups()
{
	AMMotorGroupObject *motorObject = 0;
	ssaManipulatorMotorGroup_ = new AMMotorGroup(this);

	motorObject = new AMMotorGroupObject("Sample Stage - X, Y, Z, R",
										 QStringList() << "X" << "Y" << "Z" << "R",
										 QStringList() << "mm" << "mm" << "mm" << "deg",
										 QList<AMControl *>() << ssaManipulatorX_ << ssaManipulatorY_ << ssaManipulatorZ_ << ssaManipulatorRot_ ,
										 QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal << AMMotorGroupObject::Other,
										 QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Rotational,
										 this);

	ssaManipulatorMotorGroup_->addMotorGroupObject(motorObject->name(), motorObject);
}



void SGMBeamline::setupDetectors()
{
	teyDetector_ = new CLSAdvancedScalerChannelDetector("TEY", "TEY", scaler_, 0, this);
	tfyDetector_ = new CLSAdvancedScalerChannelDetector("TFY", "TFY", scaler_, 2, this);
	i0Detector_ = new CLSAdvancedScalerChannelDetector("I0", "I0", scaler_, 1, this);
	pdDetector_ = new CLSAdvancedScalerChannelDetector("PD", "PD", scaler_, 3, this);

	filteredPD1Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD1", "FilteredPD1", scaler_, 6, this);
	filteredPD2Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD2", "FilteredPD2", scaler_, 7, this);
	filteredPD3Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD3", "FilteredPD3", scaler_, 8, this);
	filteredPD4Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD4", "FilteredPD4", scaler_, 9, this);
	filteredPD5Detector_ = new CLSAdvancedScalerChannelDetector("FilteredPD5", "FilteredPD5", scaler_, 10, this);
}

void SGMBeamline::setupExposedControls()
{
	addExposedControl(energy_);
	addExposedControl(exitSlitGap_);
	addExposedControl(ssaManipulatorX_);
	addExposedControl(ssaManipulatorY_);
	addExposedControl(ssaManipulatorZ_);
	addExposedControl(ssaManipulatorRot_);
}

void SGMBeamline::setupExposedDetectors()
{
	addExposedDetector(teyDetector_);
	addExposedDetector(tfyDetector_);
	addExposedDetector(i0Detector_);
	addExposedDetector(pdDetector_);
	addExposedDetector(filteredPD1Detector_);
	addExposedDetector(filteredPD2Detector_);
	addExposedDetector(filteredPD3Detector_);
	addExposedDetector(filteredPD4Detector_);
	addExposedDetector(filteredPD5Detector_);

}

SGMBeamline::SGMBeamline()
	: CLSBeamline("SGMBeamline")
{
	setupBeamlineComponents();
	setupMotorGroups();
	setupDetectors();
	setupExposedControls();
	setupExposedDetectors();
}
