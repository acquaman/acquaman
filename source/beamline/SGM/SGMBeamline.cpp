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

#include "util/AMErrorMonitor.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/CLS/CLSAdvancedScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"

/// acquaman data server
#include "source/appController/AMDSClientAppController.h"
#include "source/Connection/AMDSServer.h"
#include "source/DataElement/AMDSConfigurationDef.h"

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0){
		instance_ = new SGMBeamline();
		instance_->initializeBeamlineSupport();
	}

	return static_cast<SGMBeamline*>(instance_);

}

SGMBeamline::~SGMBeamline()
{
	AMDSClientAppController::releaseAppController();
}

bool SGMBeamline::isConnected() const
{
	return 	exitSlitGap_->isConnected() &&
			energyControlSet_->isConnected() &&
			ssaManipulatorX_->isConnected() &&
			ssaManipulatorY_->isConnected() &&
			ssaManipulatorZ_->isConnected() &&
			ssaManipulatorRot_->isConnected() &&
			scaler_->isConnected();
}

AMControl * SGMBeamline::endStationTranslationSetpoint() const
{
	return endStationTranslationSetpont_;
}

AMControl * SGMBeamline::endStationTranslationFeedback() const
{
	return endStationTranslationFeedback_;
}

SGMEnergyControlSet * SGMBeamline::energyControlSet() const
{
	return energyControlSet_;
}

AMControl * SGMBeamline::exitSlitGap() const
{
	return exitSlitGap_;
}

SGMHexapod* SGMBeamline::hexapod() const
{
	return hexapod_;
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

AMMotorGroup * SGMBeamline::sampleManipulatorsMotorGroup() const
{
	return sampleManipulatorsMotorGroup_;
}


CLSAmptekSDD123DetectorNew * SGMBeamline::amptekSDD1() const
{
	return amptekSDD1_;
}

CLSAmptekSDD123DetectorNew * SGMBeamline::amptekSDD2() const
{
	return amptekSDD2_;
}

CLSAmptekSDD123DetectorNew * SGMBeamline::amptekSDD3() const
{
	return amptekSDD3_;
}

CLSAmptekSDD123DetectorNew * SGMBeamline::amptekSDD4() const
{
	return amptekSDD4_;
}

CLSSIS3820Scaler * SGMBeamline::scaler() const
{
	return scaler_;
}

void SGMBeamline::onConnectionStateChanged(bool)
{
	bool actualConnectedState = isConnected();

	if(actualConnectedState != cachedConnectedState_) {
		cachedConnectedState_ = actualConnectedState;
		emit connected(actualConnectedState);
	}
}

void SGMBeamline::connectAMDSServers()
{
	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	foreach (AMDSServerConfiguration serverConfiguration, AMDSServerDefs_.values())
		clientAppController->connectToServer(serverConfiguration);
}

void SGMBeamline::onAMDSServerConnected(const QString &hostIdentifier)
{
	if(hostIdentifier == AMDSServerDefs_.value("AmptekServer").serverIdentifier() && amptekSDD1_ && amptekSDD2_ && amptekSDD3_ && amptekSDD4_){
		amptekSDD1_->configAMDSServer(AMDSServerDefs_.value("AmptekServer").serverIdentifier());
		amptekSDD2_->configAMDSServer(AMDSServerDefs_.value("AmptekServer").serverIdentifier());
		amptekSDD3_->configAMDSServer(AMDSServerDefs_.value("AmptekServer").serverIdentifier());
		amptekSDD4_->configAMDSServer(AMDSServerDefs_.value("AmptekServer").serverIdentifier());
	}
	if(hostIdentifier == AMDSServerDefs_.value("AmptekServer").serverIdentifier() && scaler_){
		scaler_->configAMDSServer(AMDSServerDefs_.value("AmptekServer").serverIdentifier());
	}
}

void SGMBeamline::setupAMDSClientAppController()
{
	AMDSServerDefs_.insert(QString("AmptekServer"), AMDSServerConfiguration(QString("AmptekServer"), QString("10.52.48.40"), 28044));

	// NOTE: it will be better to move this to CLSBeamline, when
	AMDSClientAppController *AMDSClientController = AMDSClientAppController::clientAppController();
	connect(AMDSClientController, SIGNAL(networkSessionOpened()), this, SLOT(connectAMDSServers()));
	connect(AMDSClientController, SIGNAL(newServerConnected(QString)), this, SLOT(onAMDSServerConnected(QString)));
	if (AMDSClientController->isSessionOpen()) {
		connectAMDSServers();
	}
}

void SGMBeamline::setupBeamlineComponents()
{
	energyControlSet_ = new SGMEnergyControlSet(this);

	// Exit Slit Gap
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", "PSL16114I1004:Y:mm:fbk", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "SMTR16114I1017:stop", this, 0.5);
	exitSlitGap_->setDescription("Exit Slit Gap");

	// End Station Translation
	endStationTranslationSetpont_ = new AMSinglePVControl("endStationStep", "SMTR16114I1038:step", this, 1.0, 2.0);
	endStationTranslationSetpont_->setDescription("Step");
	endStationTranslationFeedback_ = new AMReadOnlyPVControl("endStationFeedback", "ENC16114I1029:raw:cnt:fbk", this);
	endStationTranslationFeedback_->setDescription("Position");
	endStationTranslationFeedback_->setTolerance(1e15);

	// Hexapod
	hexapod_ = new SGMHexapod(this);
	// Rotate the hexapod system to the plane of the sample plate.
	hexapod_->rotateSystem(45, 0, 105);

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

	scaler_ = new CLSSIS3820Scaler("BL1611-ID-1:mcs",  "Scaler (BL1611-ID-1)", this);

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

	connect(energyControlSet_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(exitSlitGap_ ,SIGNAL(connected(bool)),this, SLOT(onConnectionStateChanged(bool)));
	connect(endStationTranslationSetpont_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(endStationTranslationFeedback_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(ssaManipulatorX_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(ssaManipulatorY_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(ssaManipulatorZ_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(ssaManipulatorRot_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged(bool)));
	connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onConnectionStateChanged(bool)));

	// Ensure that the inital cached connected state is valid, and emit an initial
	// connected signal:
	cachedConnectedState_ = isConnected();
	emit connected(cachedConnectedState_);
}

void SGMBeamline::setupMotorGroups()
{
	sampleManipulatorsMotorGroup_ = new AMMotorGroup(this);

	// Scienta manipulator
	AMMotorGroupObject* groupObject =
			new AMMotorGroupObject("Scienta Manipulator", this);

	groupObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
										  "X", ssaManipulatorX_,
										  "", 0);

	groupObject->setDirectionAxis(AMMotorGroupObject::NormalMotion,
										  "Y", ssaManipulatorY_,
										  "", 0);

	groupObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
										  "Z", ssaManipulatorZ_,
										  "rZ", ssaManipulatorRot_);

	groupObject->axis(AMMotorGroupObject::VerticalMotion)->setRotationPositionUnits("deg");

	sampleManipulatorsMotorGroup_->addMotorGroupObject(groupObject);

	// Hexapod manipulator
	groupObject = new AMMotorGroupObject("Hexapod Manipulator", this);

	groupObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion,
								  "X", hexapod_->xAxisPrimeControl(),
								  "", 0);

	groupObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion,
								  "Y", hexapod_->yAxisPrimeControl(),
								  "", 0);

	groupObject->setDirectionAxis(AMMotorGroupObject::NormalMotion,
								  "Z", hexapod_->zAxisPrimeControl(),
								  "", 0);

	sampleManipulatorsMotorGroup_->addMotorGroupObject(groupObject);
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

	// Amptek
//	amptekSDD1_ = new CLSAmptekSDD123DetectorNew("AmptekSDD1", "Amptek SDD 1", "amptek:sdd1", this);
	amptekSDD1_ = new CLSAmptekSDD123DetectorNew("AmptekSDD1", "Amptek SDD 1", "amptek:sdd2", "Amptek SDD 240", this);
	amptekSDD2_ = new CLSAmptekSDD123DetectorNew("AmptekSDD2", "Amptek SDD 2", "amptek:sdd3", "Amptek SDD 241", this);
	amptekSDD3_ = new CLSAmptekSDD123DetectorNew("AmptekSDD3", "Amptek SDD 3", "amptek:sdd4", "Amptek SDD 242", this);
	amptekSDD4_ = new CLSAmptekSDD123DetectorNew("AmptekSDD4", "Amptek SDD 4", "amptek:sdd5", "Amptek SDD 243", this);
	amptekSDD1_->setEVPerBin(2.25);
	amptekSDD2_->setEVPerBin(2.25);
	amptekSDD3_->setEVPerBin(2.25);
	amptekSDD4_->setEVPerBin(2.25);
//	amptekSDD1_->configAMDSServer(AMDSServerDefs_.value("AmptekServer").serverIdentifier());
}

void SGMBeamline::setupExposedControls()
{
	addExposedControl(endStationTranslationSetpont_);
	addExposedControl(exitSlitGap_);
	addExposedControl(ssaManipulatorX_);
	addExposedControl(ssaManipulatorY_);
	addExposedControl(ssaManipulatorZ_);
	addExposedControl(ssaManipulatorRot_);
	addExposedControl(hexapod_->xAxisPrimeControl());
	addExposedControl(hexapod_->yAxisPrimeControl());
	addExposedControl(hexapod_->zAxisPrimeControl());
	addExposedControl(energyControlSet_->energy());
}

void SGMBeamline::setupExposedDetectors()
{
	AMBasicControlDetectorEmulator* endStationPositionDetector =
			new AMBasicControlDetectorEmulator("EA2Pos", "Position of the End Station 2", endStationTranslationFeedback_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	endStationPositionDetector->setIsVisible(true);
	endStationPositionDetector->setHiddenFromUsers(false);
	addExposedDetector(endStationPositionDetector);
	addExposedDetector(teyDetector_);
	addExposedDetector(tfyDetector_);
	addExposedDetector(i0Detector_);
	addExposedDetector(pdDetector_);
	addExposedDetector(filteredPD1Detector_);
	addExposedDetector(filteredPD2Detector_);
	addExposedDetector(filteredPD3Detector_);
	addExposedDetector(filteredPD4Detector_);
	addExposedDetector(filteredPD5Detector_);
	addExposedDetector(amptekSDD1_);
	addExposedDetector(amptekSDD2_);
	addExposedDetector(amptekSDD3_);
	addExposedDetector(amptekSDD4_);

}

SGMBeamline::SGMBeamline()
	: CLSBeamline("SGMBeamline")
{
	setupAMDSClientAppController();
	setupBeamlineComponents();
	setupMotorGroups();
	setupDetectors();
	setupExposedControls();
	setupExposedDetectors();
}





