/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

#include "dataman/AMSamplePlate.h"


void SGMBeamline::usingSGMBeamline(){
	amNames2pvNames_.set("energy", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("energySpacingParam", "SG16114I1001:selI");
	amNames2pvNames_.set("energyC1Param", "SG16114I1001:selG");
	amNames2pvNames_.set("energyC2Param", "SG16114I1001:selH");
	amNames2pvNames_.set("energySParam", "SG16114I1001:selE");
	amNames2pvNames_.set("energyThetaParam", "SG16114I1001:selF");
	amNames2pvNames_.set("eV_Fbk", "BL1611-ID-1:Energy:fbk");
	amNames2pvNames_.set("eVFbk", "BL1611-ID-1:Energy:fbk");
	amNames2pvNames_.set("mono", "SMTR16114I1002");
	amNames2pvNames_.set("undulator", "UND1411-01");
	amNames2pvNames_.set("exitSlit", "PSL16114I1003");
	amNames2pvNames_.set("exitSlitGap", "PSL16114I1004");
	amNames2pvNames_.set("entranceSlitGap", "PSL16114I1001");
	amNames2pvNames_.set("M4", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Inboard", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Outboard", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Downstream", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("grating", "SG16114I1001:choice");
	amNames2pvNames_.set("harmonic", "BL1611-ID-1:harmonic");
	amNames2pvNames_.set("undulatorTracking", "UND1411-01:Energy:track");
	amNames2pvNames_.set("monoTracking", "SG16114I1001:Energy:track");
	amNames2pvNames_.set("exitSlitTracking", "PSL16114I1003:Energy:track");
	//amNames2pvNames_.set("tey", "A1611-4-15:A:fbk");
	amNames2pvNames_.set("tey", "BL1611-ID-1:mcs00:fbk");
	//amNames2pvNames_.set("tfy", "A1611-4-16:A:fbk");
	amNames2pvNames_.set("tfy", "BL1611-ID-1:mcs02:fbk");
	amNames2pvNames_.set("tfyHVSetpoint", "PS1611401:109:v0set");
	amNames2pvNames_.set("tfyHVFbk", "PS1611401:109:vmon");
	amNames2pvNames_.set("pgt", "MCA1611-01:GetChannels");
	amNames2pvNames_.set("pgtHVSetpoint", "MCA1611-01:Bias:Volt");
	amNames2pvNames_.set("pgtHVFbk", "MCA1611-01:Bias:VoltActual:fbk");
	amNames2pvNames_.set("pgtHVRamping", "MCA1611-01:Bias:Volt:fbk");
	amNames2pvNames_.set("pgtIntegrationTime", "MCA1611-01:Preset:Live");
	amNames2pvNames_.set("pgtIntegrationMode", "MCA1611-01:Preset:Live");
	//amNames2pvNames_.set("I0", "A1611-4-14:A:fbk");
	amNames2pvNames_.set("I0", "BL1611-ID-1:mcs01:fbk");
	//amNames2pvNames_.set("photodiode", "A1611-4-13:A:fbk");
	amNames2pvNames_.set("photodiode", "BL1611-ID-1:mcs03:fbk");
	amNames2pvNames_.set("encoderUp", "BL1611-ID-1:mcs04:fbk");
	amNames2pvNames_.set("encoderDown", "BL1611-ID-1:mcs05:fbk");
	amNames2pvNames_.set("loadlockCCG", "CCG1611-4-I10-09:vac:p");
	amNames2pvNames_.set("loadlockTCG", "TCGC1611-426:pressure:fbk");
	amNames2pvNames_.set("ssaManipulatorX", "BL1611-ID-1:EA2:x");
	amNames2pvNames_.set("ssaManipulatorY", "BL1611-ID-1:EA2:z");
	amNames2pvNames_.set("ssaManipulatorZ", "BL1611-ID-1:EA2:y");
	amNames2pvNames_.set("ssaManipulatorRot", "BL1611-ID-1:EA2:r");
	amNames2pvNames_.set("beamlineScanning", "BL1611-ID-1:scanning");
	amNames2pvNames_.set("beamlineReady", "BL1611-ID-1:beam:status");
	amNames2pvNames_.set("energyMovingStatus", "BL1611-ID-1:ready");
	amNames2pvNames_.set("fastShutterVoltage", "PSH16114I1001:V");
	amNames2pvNames_.set("scalerMode", "BL1611-ID-1:mcs:continuous");
	amNames2pvNames_.set("scalerStart", "BL1611-ID-1:mcs:startScan");
	amNames2pvNames_.set("scaler", "BL1611-ID-1:mcs:scan");
	amNames2pvNames_.set("scalerIntegrationTime", "BL1611-ID-1:mcs:delay");
	amNames2pvNames_.set("scalerScansPerBuffer", "BL1611-ID-1:mcs:nscan");
	amNames2pvNames_.set("scalerTotalNumberOfScans", "BL1611-ID-1:mcs:scanCount");
	amNames2pvNames_.set("gratingVelocity", "SMTR16114I1002:velo");
	amNames2pvNames_.set("gratingBaseVelocity", "SMTR16114I1002:veloBase");
	amNames2pvNames_.set("gratingAcceleration", "SMTR16114I1002:accel");
	amNames2pvNames_.set("ea1CloseVacuum1", "VVR1611-4-I10-05:opr:close");
	amNames2pvNames_.set("ea1CloseVacuum2", "VVR1611-4-I10-06:opr:close");
	amNames2pvNames_.set("ea2CloseVacuum", "VVR1611-4-I10-08:opr:close");
	//amNames2pvNames_.set("beamOn", "BL1611-ID-1:beam:turnon");
	amNames2pvNames_.set("beamOn", "david:beamon:trigger");
	amNames2pvNames_.set("visibleLightToggle", "BL1611-ID-1:visible");
	amNames2pvNames_.set("visibleLightStatus", "BL1611-ID-1:visible:cal");
	amNames2pvNames_.set("activeEndstation", "david:endstation:active");

	//ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::toPV("ringCurrent"), this);
	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", "PCT1402-01:mA:fbk", this);
	addChildControl(ringCurrent_);

	bool pvNameLookUpFail = false;

	QString sgmPVName = amNames2pvNames_.valueF("energy");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, "BL1611-ID-1:ready", sgmPVName, this, 0.05);
	sgmPVName = amNames2pvNames_.valueF("energySpacingParam");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energySpacingParam_ = new AMReadOnlyPVControl("energySpacingParam", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyC1Param");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energyC1Param_ = new AMReadOnlyPVControl("energyC1Param", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyC2Param");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energyC2Param_ = new AMReadOnlyPVControl("energyC2Param", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energySParam");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energySParam_ = new AMReadOnlyPVControl("energySParam", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyThetaParam");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energyThetaParam_ = new AMReadOnlyPVControl("energyThetaParam", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("mono");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *mono = new AMPVwStatusControl("mono", sgmPVName+":enc:fbk", sgmPVName+":encTarget", sgmPVName+":moving", "SMTR16114I1002:stop", energy_, 5);
	sgmPVName = amNames2pvNames_.valueF("undulator");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *undulator = new AMPVwStatusControl("undulator", sgmPVName+":gap:mm:fbk", sgmPVName+":gap:mm", sgmPVName+":moveStatus", "UND1411-01:stop", energy_, 0.1);
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *exitSlit = new AMPVwStatusControl("exitSlit", sgmPVName+":Y:mm:encsp", "SMTR16114I1003:mm", "SMTR16114I1003:moving", "SMTR16114I1003:stop", energy_, 0.1);
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);

	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1017:status", "SMTR16114I1017:stop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("entranceSlitGap");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	entranceSlitGap_ = new AMPVwStatusControl("entranceSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1001:status", "SMTR16114I1001:stop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;

	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4_->addChildControl(m4inboard);
	m4_->addChildControl(m4outboard);
	m4_->addChildControl(m4downstream);

	sgmPVName = amNames2pvNames_.valueF("grating");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, "SMTR16114I1016:state", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, "UND1411-01:moveStatus", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, "", this, 0.1, 10);
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("tey");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	tey_ = new AMReadOnlyPVControl("tey", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfy");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	tfy_ = new AMReadOnlyPVControl("tfy", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfyHVSetpoint");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	tfyHVSetpoint_ = new AMPVControl("tfyHVSetpoint", amNames2pvNames_.valueF("tfyHVFbk"), amNames2pvNames_.valueF("tfyHVSetpoint"), "", this, 10.0);
	sgmPVName = amNames2pvNames_.valueF("tfyHVFbk");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	tfyHVFbk_ = new AMPVControl("tfyHVFbk", sgmPVName, sgmPVName, "", this, 10.0);

	sgmPVName = amNames2pvNames_.valueF("pgt");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgt_ = new AMReadOnlyPVControl("pgt", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtHVSetpoint");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	//    pgtHVSetpoint_ = new AMPVControl("pgtHVSetpoint", amNames2pvNames_.valueF("pgtHVFbk"), amNames2pvNames_.valueF("pgtHVSetpoint"), this, 0.5);
	pgtHVSetpoint_ = new AMPVControl("pgtHVSetpoint", amNames2pvNames_.valueF("pgtHVSetpoint"), amNames2pvNames_.valueF("pgtHVSetpoint"), "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("pgtHVFbk");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgtHVFbk_ = new AMReadOnlyPVControl("pgtHVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationTime");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgtIntegrationTime_ = new AMPVControl("pgtIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationMode");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgtIntegrationMode_ = new AMPVControl("pgtIntegrationMode", sgmPVName, sgmPVName, "", this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("I0");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	i0_ = new AMReadOnlyPVControl("I0", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("eVFbk");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	eVFbk_ = new AMReadOnlyPVControl("eVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("photodiode");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	photodiode_ = new AMReadOnlyPVControl("photodiode", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("encoderUp");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	encoderUp_ = new AMReadOnlyPVControl("encoderUp", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("encoderDown");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	encoderDown_ = new AMReadOnlyPVControl("encoderDown", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("loadlockCCG");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	loadlockCCG_ = new AMReadOnlyPVControl("loadlockCCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockTCG");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	loadlockTCG_ = new AMReadOnlyPVControl("loadlockTCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorX");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	/// \todo Now that we have stop integrated into the AMControls... Remove separate stop PVs.
	ssaManipulatorX_ = new AMPVwStatusControl("ssaManipulatorX", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorXStop_ = new AMPVControl("ssaManipulatorXStop", "SMTR16114I1012:emergStop", "SMTR16114I1012:emergStop", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorY");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaManipulatorY_ = new AMPVwStatusControl("ssaManipulatorY", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorYStop_ = new AMPVControl("ssaManipulatorYStop", "SMTR16114I1013:emergStop", "SMTR16114I1013:emergStop", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorZ");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaManipulatorZ_ = new AMPVwStatusControl("ssaManipulatorZ", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorZStop_ = new AMPVControl("ssaManipulatorZStop", "SMTR16114I1014:emergStop", "SMTR16114I1014:emergStop", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorRot");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaManipulatorRot_ = new AMPVwStatusControl("ssaManipulatorRot", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorRotStop_ = new AMPVControl("ssaManipulatorRotStop", "SMTR16114I1015:emergStop", "SMTR16114I1015:emergStop", "", this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("beamlineScanning");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	beamlineScanning_ = new AMPVControl("beamlineScanning", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineReady");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	beamlineReady_ = new AMReadOnlyPVControl("beamlineReady", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyMovingStatus");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energyMovingStatus_ = new AMReadOnlyPVControl("energyMovingStatus", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("fastShutterVoltage");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	fastShutterVoltage_ = new AMPVControl("fastShutterVoltage", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("gratingVelocity");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	gratingVelocity_ = new AMPVControl("gratingVelocity", sgmPVName, sgmPVName, "", this, 1);
	sgmPVName = amNames2pvNames_.valueF("gratingBaseVelocity");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	gratingBaseVelocity_ = new AMPVControl("gratingBaseVelocity", sgmPVName, sgmPVName, "", this, 1);
	sgmPVName = amNames2pvNames_.valueF("gratingAcceleration");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	gratingAcceleration_ = new AMPVControl("gratingAcceleration", sgmPVName, sgmPVName, "", this, 1);
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum1");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ea1CloseVacuum1_ = new AMPVControl("ea1CloseVacuum1", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum2");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ea1CloseVacuum2_ = new AMPVControl("ea1CloseVacuum2", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("ea2CloseVacuum");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ea2CloseVacuum_ = new AMPVControl("ea2CloseVacuum", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("beamOn");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	beamOn_ = new AMPVControl("beamOn", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("visibleLightToggle");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	visibleLightToggle_ = new AMPVControl("visibleLightToggle", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("visibleLightStatus");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	visibleLightStatus_ = new AMReadOnlyPVControl("visibleLightStatus", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("activeEndstation");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	activeEndstation_ = new AMPVControl("activeEndstation", sgmPVName, sgmPVName, "", this);

	sgmPVName = amNames2pvNames_.valueF("scalerIntegrationTime");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	scalerIntegrationTime_ = new AMPVControl("scalerIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("scalerMode");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	scalerMode_ = new AMPVControl("scalerMode", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("scalerTotalNumberOfScans");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	scalerTotalNumberOfScans_ = new AMPVControl("scalerTotalNumberOfScans", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("scalerScansPerBuffer");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	scalerScansPerBuffer_ = new AMPVControl("scalerScansPerBuffer", sgmPVName, sgmPVName, "", this, 0.5);

	qDebug() << "\nPV Name Look Ups Succeeded: " << pvNameLookUpFail << "\n";
}

void SGMBeamline::usingFakeBeamline(){
	amNames2pvNames_.set("energy", "reixsHost:Energy");
	amNames2pvNames_.set("eV_Fbk", "reixsHost:Energy:fbk");
	amNames2pvNames_.set("eVFbk", "reixsHost:eVfbk");
	amNames2pvNames_.set("mono", "reixsHost:Energy:mono");
	amNames2pvNames_.set("undulator", "reixsHost:Energy:undulator");
	amNames2pvNames_.set("exitSlit", "reixsHost:Energy:exitSlit");
	amNames2pvNames_.set("exitSlitGap", "reixsHost:Slit");
	amNames2pvNames_.set("entranceSlitGap", "reixsHost:entranceSlit");
	amNames2pvNames_.set("M4", "reixsHost:M4");
	amNames2pvNames_.set("M4Inboard", "reixsHost:M4:inboard");
	amNames2pvNames_.set("M4Outboard", "reixsHost:M4:outboard");
	amNames2pvNames_.set("M4Downstream", "reixsHost:M4:downstream");
	amNames2pvNames_.set("grating", "reixsHost:Energy:mono:grating");
	amNames2pvNames_.set("harmonic", "reixsHost:Energy:undulator:harmonic");
	amNames2pvNames_.set("undulatorTracking", "reixsHost:Energy:undulator:tracking");
	amNames2pvNames_.set("monoTracking", "reixsHost:Energy:mono:tracking");
	amNames2pvNames_.set("exitSlitTracking", "reixsHost:Energy:exitSlit:tracking");
	amNames2pvNames_.set("tey", "reixsHost:tey");
	amNames2pvNames_.set("tfy", "reixsHost:tfy");
	amNames2pvNames_.set("tfyHVSetpoint", "reixsHost:tfy:hv:sp");
	amNames2pvNames_.set("tfyHVFbk", "reixsHost:tfy:hv:fbk");
	amNames2pvNames_.set("pgt", "reixsHost:sdd:spectrum");
	amNames2pvNames_.set("pgtHVSetpoint", "reixsHost:sdd:hv:sp");
	amNames2pvNames_.set("pgtHVFbk", "reixsHost:sdd:hv:fbk");
	amNames2pvNames_.set("pgtHVRamping", "reixsHost:sdd:hv:ramping");
	amNames2pvNames_.set("pgtIntegrationTime", "reixsHost:sdd:integration:time");
	amNames2pvNames_.set("pgtIntegrationMode", "reixsHost:sdd:integration:mode");
	amNames2pvNames_.set("I0", "reixsHost:I0");
	amNames2pvNames_.set("photodiode", "reixsHost:photodiode");
	amNames2pvNames_.set("loadlockCCG", "reixsHost:Endstation:loadlock:ccg");
	amNames2pvNames_.set("loadlockTCG", "reixsHost:Endstation:loadlock:tcg");
	amNames2pvNames_.set("ssaManipulatorX", "reixsHost:ssa:x");
	amNames2pvNames_.set("ssaManipulatorY", "reixsHost:ssa:y");
	amNames2pvNames_.set("ssaManipulatorZ", "reixsHost:ssa:z");
	amNames2pvNames_.set("ssaManipulatorRot", "reixsHost:ssa:r");
	amNames2pvNames_.set("beamlineScanning", "reixsHost:scanning");
	amNames2pvNames_.set("beamlineReady", "reixsHost:ready");
	amNames2pvNames_.set("energyMovingStatus", "reixsHost:Energy:moving:status");
	amNames2pvNames_.set("fastShutterVoltage", "reixsHost:fastShutter:V");
	amNames2pvNames_.set("scalerMode", "reixsHost:scaler:continuous");
	amNames2pvNames_.set("scalerStart", "reixsHost:scaler:start");
	amNames2pvNames_.set("scaler", "reixsHost:scaler:spectrum");
	amNames2pvNames_.set("scalerIntegrationTime", "reixsHost:scaler:integrationTime");
	amNames2pvNames_.set("scalerScansPerBuffer", "reixsHost:scaler:nscan");
	amNames2pvNames_.set("scalerTotalNumberOfScans", "reixsHost:scaler:scanCount");
	amNames2pvNames_.set("gratingVelocity", "reixsHost:mono:velo");
	amNames2pvNames_.set("gratingBaseVelocity", "reixsHost:mono:veloBase");
	amNames2pvNames_.set("gratingAcceleration", "reixsHost:mono:accel");
	amNames2pvNames_.set("ea1CloseVacuum1", "reixsHost:close:ea1:1");
	amNames2pvNames_.set("ea1CloseVacuum2", "reixsHost:close:ea1:2");
	amNames2pvNames_.set("ea2CloseVacuum", "reixsHost:close:ea2");
	amNames2pvNames_.set("beamOn", "reixsHost:open:beamline");
	amNames2pvNames_.set("visibleLightToggle", "reixsHost:visible");
	amNames2pvNames_.set("visibleLightStatus", "reixsHost:visible:cal");
	amNames2pvNames_.set("activeEndstation", "reixsHost:endstation:active");

	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", "PCT1402-01:mA:fbk", this);
	//ringCurrent_ = new AMPVControl("ringCurrent", "PCT1402-01:mA:fbk", "PCT1402-01:mA:fbk", QString(), this);
	//ringCurrent_ = new AMPVwStatusControl("ringCurrent", "PCT1402-01:mA:fbk", "PCT1402-01:mA:fbk", "PCT1402-01:mA:fbk", QString(), this);
	addChildControl(ringCurrent_);

	QString sgmPVName = amNames2pvNames_.valueF("energy");
	energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, sgmPVName+":moving", "", this, 0.01);
	sgmPVName = amNames2pvNames_.valueF("mono");
	//AMReadOnlyPVwStatusControl *mono = new AMReadOnlyPVwStatusControl("mono", sgmPVName, sgmPVName+":moving", energy_);
	AMPVwStatusControl *mono = new AMPVwStatusControl("mono", sgmPVName, sgmPVName, sgmPVName+":moving", sgmPVName+":stop", energy_, 5);
	sgmPVName = amNames2pvNames_.valueF("undulator");
	//AMReadOnlyPVwStatusControl *undulator = new AMReadOnlyPVwStatusControl("undulator", sgmPVName, sgmPVName+":moving", energy_);
	AMPVwStatusControl *undulator = new AMPVwStatusControl("undulator", sgmPVName, sgmPVName, sgmPVName+":moving", sgmPVName+":stop", energy_, 0.1);
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	//AMReadOnlyPVwStatusControl *exitSlit = new AMReadOnlyPVwStatusControl("exitSlit", sgmPVName, sgmPVName+":moving", energy_);
	AMPVwStatusControl *exitSlit = new AMPVwStatusControl("exitSlit", sgmPVName, sgmPVName, sgmPVName+":moving", sgmPVName+":stop", energy_, 0.1);
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);
	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":fbk", sgmPVName, sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("entranceSlitGap");
	entranceSlitGap_ = new AMPVwStatusControl("entranceSlitGap", sgmPVName+":fbk", sgmPVName, sgmPVName+":moving", sgmPVName+":stop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4");
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");

	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName+":moving", "", this, 0.1);
	m4_->addChildControl(m4inboard);
	m4_->addChildControl(m4outboard);
	m4_->addChildControl(m4downstream);

	sgmPVName = amNames2pvNames_.valueF("grating");
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("tey");
	tey_ = new AMReadOnlyPVControl("tey", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("tfy");
	tfy_ = new AMReadOnlyPVControl("tfy", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfyHVSetpoint");
	tfyHVSetpoint_ = new AMPVControl("tfyHVSetpoint", amNames2pvNames_.valueF("tfyHVFbk"), amNames2pvNames_.valueF("tfyHVSetpoint"), "", this, 10.0);
	sgmPVName = amNames2pvNames_.valueF("tfyHVFbk");
	tfyHVFbk_ = new AMPVControl("tfyHVFbk", sgmPVName, sgmPVName, "", this, 10.0);

	sgmPVName = amNames2pvNames_.valueF("pgt");
	pgt_ = new AMReadOnlyPVControl("pgt", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtHVSetpoint");
	pgtHVSetpoint_ = new AMPVControl("pgtHVSetpoint", amNames2pvNames_.valueF("pgtHVSetpoint"), amNames2pvNames_.valueF("pgtHVSetpoint"), "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("pgtHVFbk");
	pgtHVFbk_ = new AMReadOnlyPVControl("pgtHVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationTime");
	pgtIntegrationTime_ = new AMPVControl("pgtIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationMode");
	pgtIntegrationMode_ = new AMPVControl("pgtIntegrationMode", sgmPVName, sgmPVName, "", this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("I0");
	i0_ = new AMReadOnlyPVControl("I0", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("eVFbk");
	eVFbk_ = new AMReadOnlyPVControl("eVFbk", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("photodiode");
	photodiode_ = new AMReadOnlyPVControl("photodiode", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("loadlockCCG");
	loadlockCCG_ = new AMReadOnlyPVControl("loadlockCCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockTCG");
	loadlockTCG_ = new AMReadOnlyPVControl("loadlockTCG", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorX");
	/// \todo Now that AMControl has stop built in, get rid of separate stop PVs.
	ssaManipulatorX_ = new AMPVwStatusControl("ssaManipulatorX", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", "", this, 0.1);
	ssaManipulatorXStop_ = new AMPVControl("ssaManipulatorXStop", sgmPVName+":moving", sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorY");
	ssaManipulatorY_ = new AMPVwStatusControl("ssaManipulatorY", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", "", this, 0.1);
	ssaManipulatorYStop_ = new AMPVControl("ssaManipulatorYStop", sgmPVName+":moving", sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorZ");
	ssaManipulatorZ_ = new AMPVwStatusControl("ssaManipulatorZ", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", "", this, 0.1);
	ssaManipulatorZStop_ = new AMPVControl("ssaManipulatorZStop", sgmPVName+":moving", sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorRot");
	ssaManipulatorRot_ = new AMPVwStatusControl("ssaManipulatorRot", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", "", this, 0.1);
	ssaManipulatorRotStop_ = new AMPVControl("ssaManipulatorRotStop", sgmPVName+":moving", sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineScanning");
	beamlineScanning_ = new AMPVControl("beamlineScanning", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineReady");
	beamlineReady_ = new AMReadOnlyPVControl("beamlineReady", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyMovingStatus");
	energyMovingStatus_ = new AMReadOnlyPVControl("energyMovingStatus", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("fastShutterVoltage");
	fastShutterVoltage_ = new AMPVControl("fastShutterVoltage", sgmPVName, sgmPVName, "", this);
	sgmPVName = amNames2pvNames_.valueF("gratingVelocity");
	gratingVelocity_ = new AMPVControl("gratingVelocity", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("gratingBaseVelocity");
	gratingBaseVelocity_ = new AMPVControl("gratingBaseVelocity", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("gratingAcceleration");
	gratingAcceleration_ = new AMPVControl("gratingAcceleration", sgmPVName, sgmPVName, "", this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum1");
	ea1CloseVacuum1_ = new AMPVControl("ea1CloseVacuum1", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum2");
	ea1CloseVacuum2_ = new AMPVControl("ea1CloseVacuum2", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("ea2CloseVacuum");
	ea2CloseVacuum_ = new AMPVControl("ea2CloseVacuum", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("beamOn");
	beamOn_ = new AMPVControl("beamOn", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("visibleLightToggle");
	visibleLightToggle_ = new AMPVControl("visibleLightToggle", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("visibleLightStatus");
	visibleLightStatus_ = new AMReadOnlyPVControl("visibleLightStatus", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("activeEndstation");
	activeEndstation_ = new AMPVControl("activeEndstation", sgmPVName, sgmPVName, "", this, 0.5);

	sgmPVName = amNames2pvNames_.valueF("scalerIntegrationTime");
	scalerIntegrationTime_ = new AMPVControl("scalerIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("scalerScansPerBuffer");
	scalerScansPerBuffer_ = new AMPVControl("scalerScansPerBuffer", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("scalerTotalNumberOfScans");
	scalerTotalNumberOfScans_ = new AMPVControl("scalerTotalNumberOfScans", sgmPVName, sgmPVName, "", this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("scalerMode");
	scalerMode_ = new AMPVControl("scalerMode", sgmPVName, sgmPVName, "", this, 0.5);
}

SGMBeamline::SGMBeamline() : AMBeamline("SGMBeamline") {
	usingFakeBeamline();
	//usingSGMBeamline();

	beamlineWarnings_ = "";
	connect(this, SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(recomputeWarnings()));

	addChildControl(energy_);
	addChildControl(energySpacingParam_);
	addChildControl(energyC1Param_);
	addChildControl(energyC2Param_);
	addChildControl(energySParam_);
	addChildControl(energyThetaParam_);
	addChildControl(exitSlitGap_);
	addChildControl(entranceSlitGap_);
	addChildControl(m4_);
	addChildControl(grating_);
	addChildControl(harmonic_);
	addChildControl(undulatorTracking_);
	addChildControl(monoTracking_);
	addChildControl(exitSlitTracking_);
	addChildControl(tey_);
	addChildControl(tfy_);
	addChildControl(tfyHVSetpoint_);
	addChildControl(tfyHVFbk_);
	addChildControl(pgt_);
	addChildControl(pgtHVSetpoint_);
	addChildControl(pgtHVFbk_);
	addChildControl(pgtIntegrationTime_);
	addChildControl(pgtIntegrationMode_);
	addChildControl(i0_);
	addChildControl(eVFbk_);
	addChildControl(encoderUp_);
	addChildControl(encoderDown_);
	addChildControl(ssaManipulatorX_);
	addChildControl(ssaManipulatorXStop_);
	addChildControl(ssaManipulatorY_);
	addChildControl(ssaManipulatorYStop_);
	addChildControl(ssaManipulatorZStop_);
	addChildControl(ssaManipulatorRot_);
	addChildControl(ssaManipulatorRotStop_);
	addChildControl(beamlineScanning_);
	connect(beamlineScanning_, SIGNAL(valueChanged(double)), this, SLOT(onBeamlineScanningValueChanged(double)));

	addChildControl(beamlineReady_);
	addChildControl(energyMovingStatus_);
	addChildControl(fastShutterVoltage_);
	addChildControl(gratingVelocity_);
	addChildControl(gratingBaseVelocity_);
	addChildControl(gratingAcceleration_);
	addChildControl(ea1CloseVacuum1_);
	addChildControl(ea1CloseVacuum2_);
	addChildControl(ea2CloseVacuum_);
	addChildControl(beamOn_);
	addChildControl(visibleLightToggle_);
	connect(visibleLightToggle_, SIGNAL(valueChanged(double)), this, SLOT(onVisibleLightChanged(double)));
	addChildControl(visibleLightStatus_);
	connect(visibleLightStatus_, SIGNAL(valueChanged(double)), this, SLOT(onVisibleLightChanged(double)));
	addChildControl(scalerIntegrationTime_);
	addChildControl(scalerScansPerBuffer_);
	addChildControl(scalerTotalNumberOfScans_);
	addChildControl(scalerMode_);

	criticalControlsSet_ = new AMControlSet(this);
	criticalControlsSet_->setName("Critical Beamline Controls");
	criticalControlsSet_->addControl(energy_);
	criticalControlsSet_->addControl(exitSlitGap_);
	criticalControlsSet_->addControl(m4_);
	criticalControlsSet_->addControl(grating_);
	criticalControlsSet_->addControl(harmonic_);
	criticalControlsSet_->addControl(undulatorTracking_);
	criticalControlsSet_->addControl(monoTracking_);
	criticalControlsSet_->addControl(exitSlitTracking_);
	criticalControlsSet_->addControl(beamlineScanning_);

	criticalControlsSet_->addControl(beamlineReady_);
	criticalControlsSet_->addControl(energyMovingStatus_);

	unconnectedSets_.append(criticalControlsSet_);
	connect(criticalControlsSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	connect(criticalControlsSet_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(onCriticalControlsConnectedChanged(bool,AMControl*)));

	beamOnControlSet_ = new AMControlSet(this);
	beamOnControlSet_->setName("Beam On Controls");
	beamOnControlSet_->addControl(beamOn_);
	beamOnControlSet_->addControl(fastShutterVoltage_);
	unconnectedSets_.append(beamOnControlSet_);
	connect(beamOnControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	teyControlSet_ = new AMControlSet(this);
	teyControlSet_->setName("TEY Controls");
	teyControlSet_->addControl(tey_);
	teyDetector_ = NULL;
	teyDetectorNew_ = NULL;
	unconnectedSets_.append(teyControlSet_);
	connect(teyControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	tfyControlSet_ = new AMControlSet(this);
	tfyControlSet_->setName("TFY Controls");
	tfyControlSet_->addControl(tfy_);
	tfyControlSet_->addControl(tfyHVSetpoint_);
	tfyControlSet_->addControl(tfyHVFbk_);
	tfyDetector_ = NULL;
	tfyDetectorNew_ = NULL;
	unconnectedSets_.append(tfyControlSet_);
	connect(tfyControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	pgtControlSet_ = new AMControlSet(this);
	pgtControlSet_->setName("SDD Controls");
	pgtControlSet_->addControl(pgt_);
	pgtControlSet_->addControl(pgtHVSetpoint_);
	pgtControlSet_->addControl(pgtHVFbk_);
	pgtControlSet_->addControl(pgtIntegrationTime_);
	pgtControlSet_->addControl(pgtIntegrationMode_);
	pgtDetector_ = NULL;
	pgtDetectorNew_ = NULL;
	unconnectedSets_.append(pgtControlSet_);
	connect(pgtControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	i0ControlSet_ = new AMControlSet(this);
	i0ControlSet_->setName("I0 Controls");
	i0ControlSet_->addControl(i0_);
	i0Detector_ = NULL;
	i0DetectorNew_ = NULL;
	unconnectedSets_.append(i0ControlSet_);
	connect(i0ControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	eVFbkControlSet_ = new AMControlSet(this);
	eVFbkControlSet_->setName("Energy Feedback Controls");
	eVFbkControlSet_->addControl(photodiode_);
	eVFbkDetector_ = NULL;
	eVFbkDetectorNew_ = NULL;
	unconnectedSets_.append(eVFbkControlSet_);
	connect(eVFbkControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	photodiodeControlSet_ = new AMControlSet(this);
	photodiodeControlSet_->setName("Photodiode Controls");
	photodiodeControlSet_->addControl(photodiode_);
	photodiodeDetector_ = NULL;
	photodiodeDetectorNew_ = NULL;
	unconnectedSets_.append(photodiodeControlSet_);
	connect(photodiodeControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	encoderUpControlSet_ = new AMControlSet(this);
	encoderUpControlSet_->setName("Encoder Up Controls");
	encoderUpControlSet_->addControl(encoderUp_);
	encoderUpDetector_ = NULL;
	encoderUpDetectorNew_ = NULL;
	unconnectedSets_.append(encoderUpControlSet_);
	connect(encoderUpControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	encoderDownControlSet_ = new AMControlSet(this);
	encoderDownControlSet_->setName("Encoder Down Controls");
	encoderDownControlSet_->addControl(encoderDown_);
	encoderDownDetector_ = NULL;
	encoderDownDetectorNew_ = NULL;
	unconnectedSets_.append(encoderDownControlSet_);
	connect(encoderDownControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	fluxOptimization_ = new SGMFluxOptimization(this);
	fluxOptimization_->setDescription("Flux");
	resolutionOptimization_ = new SGMResolutionOptimization(this);
	resolutionOptimization_->setDescription("Resolution");
	fluxResolutionSet_ = new AMControlOptimizationSet(this);
	fluxResolutionSet_->setName("Flux/Resolution");
	fluxResolutionSet_->addControl(grating_);
	fluxResolutionSet_->addControl(harmonic_);
	fluxResolutionSet_->addControl(exitSlitGap_);
	unconnectedSets_.append(fluxResolutionSet_);
	connect(fluxResolutionSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(fluxOptimization_);
	((AMControlOptimizationSet*)fluxResolutionSet_)->addOptimization(resolutionOptimization_);

	trackingSet_ = new AMControlSet(this);
	trackingSet_->setName("Tracking");
	trackingSet_->addControl(undulatorTracking_);
	trackingSet_->addControl(monoTracking_);
	trackingSet_->addControl(exitSlitTracking_);
	unconnectedSets_.append(trackingSet_);
	connect(trackingSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	ssaManipulatorSet_ = new AMControlSet(this);
	ssaManipulatorSet_->setName("SSA Manipulator");
	ssaManipulatorSet_->addControl(ssaManipulatorX_);
	ssaManipulatorSet_->addControl(ssaManipulatorY_);
	ssaManipulatorSet_->addControl(ssaManipulatorZ_);
	ssaManipulatorSet_->addControl(ssaManipulatorRot_);
	unconnectedSets_.append(ssaManipulatorSet_);
	connect(ssaManipulatorSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	allDetectors_ = new AMOldDetectorInfoSet(this);
	allDetectors_->setName("All Detectors");
	allDetectorsNew_ = new AMDetectorSet(this);
	allDetectorsNew_->setName("All Detectors");


	feedbackDetectors_ = new AMOldDetectorInfoSet(this);
	feedbackDetectors_->setName("Feedback Detectors");
	feedbackDetectorsNew_ = new AMDetectorSet(this);
	feedbackDetectorsNew_->setName("Feedback Detectors");

	XASDetectors_ = new AMOldDetectorInfoSet(this);
	XASDetectors_->setName("XAS Detectors");
	XASDetectorsNew_ = new AMDetectorSet(this);
	XASDetectorsNew_->setName("XAS Detectors");

	currentSamplePlate_ = new AMSamplePlate(this);

	transferAction1_ = new SGMTransferAction1(this);
	transferAction2_ = new SGMTransferAction2(this);
	connect(loadlockCCG_, SIGNAL(valueChanged(double)), transferAction2_, SLOT(checkValue(double)));
	transferAction3_ = new SGMTransferAction3(this);
	transferAction4_ = new SGMTransferAction4(this);
	transferAction5_ = new SGMTransferAction5(this);
	connect(loadlockTCG_, SIGNAL(valueChanged(double)), transferAction5_, SLOT(checkValue(double)));
	transferAction6_ = new SGMTransferAction6(this);

	transferAction7_ = new SGMTransferAction7(this);
	transferAction8_ = new SGMTransferAction8(this);
	transferAction9_ = new SGMTransferAction9(this);
	connect(loadlockTCG_, SIGNAL(valueChanged(double)), transferAction9_, SLOT(checkValue(double)));
	transferAction10_ = new SGMTransferAction10(this);
	transferAction11_ = new SGMTransferAction11(this);
	connect(loadlockCCG_, SIGNAL(valueChanged(double)), transferAction11_, SLOT(checkValue(double)));
	transferAction12_ = new SGMTransferAction12(this);
	connect(loadlockCCG_, SIGNAL(valueChanged(double)), transferAction12_, SLOT(checkValue(double)));

	transferAction13_ = new SGMTransferAction13(this);
	transferAction14_ = new SGMTransferAction14(this);
	transferAction15_ = new SGMTransferAction15(this);
	transferAction16_ = new SGMTransferAction16(this);
	transferAction17_ = new SGMTransferAction17(this);
	transferAction18_ = new SGMTransferAction18(this);
	transferAction19_ = new SGMTransferAction19(this);
	transferAction20_ = new SGMTransferAction20(this);
	transferAction21_ = new SGMTransferAction21(this);
	transferAction22_ = new SGMTransferAction22(this);
	transferAction23_ = new SGMTransferAction23(this);
	transferAction24_ = new SGMTransferAction24(this);
	transferAction25_ = new SGMTransferAction25(this);

	transferAction1_->setNext(transferAction2_);
	transferAction2_->setPrevious(transferAction1_);
	transferAction2_->setNext(transferAction3_);
	transferAction3_->setPrevious(transferAction2_);
	transferAction3_->setNext(transferAction4_);
	transferAction4_->setPrevious(transferAction3_);
	transferAction4_->setNext(transferAction5_);
	transferAction5_->setPrevious(transferAction4_);
	transferAction5_->setNext(transferAction6_);
	transferAction6_->setPrevious(transferAction5_);

	transferAction7_->setNext(transferAction8_);
	transferAction8_->setPrevious(transferAction7_);
	transferAction8_->setNext(transferAction9_);
	transferAction9_->setPrevious(transferAction8_);
	transferAction9_->setNext(transferAction10_);
	transferAction10_->setPrevious(transferAction9_);
	transferAction10_->setNext(transferAction11_);
	transferAction11_->setPrevious(transferAction10_);
	transferAction11_->setNext(transferAction12_);
	transferAction12_->setPrevious(transferAction11_);

	transferAction13_->setNext(transferAction14_);
	transferAction14_->setPrevious(transferAction13_);
	transferAction14_->setNext(transferAction15_);
	transferAction15_->setPrevious(transferAction14_);
	transferAction15_->setNext(transferAction16_);
	transferAction16_->setPrevious(transferAction15_);
	transferAction16_->setNext(transferAction17_);
	transferAction17_->setPrevious(transferAction16_);
	transferAction17_->setNext(transferAction18_);
	transferAction18_->setPrevious(transferAction17_);
	transferAction18_->setNext(transferAction19_);
	transferAction19_->setPrevious(transferAction18_);
	transferAction19_->setNext(transferAction20_);
	transferAction20_->setPrevious(transferAction19_);

	transferAction21_->setNext(transferAction22_);
	transferAction22_->setPrevious(transferAction21_);
	transferAction22_->setNext(transferAction23_);
	transferAction23_->setPrevious(transferAction22_);
	transferAction23_->setNext(transferAction24_);
	transferAction24_->setPrevious(transferAction23_);
	transferAction24_->setNext(transferAction25_);
	transferAction25_->setPrevious(transferAction24_);

	beamOnAction1_ = 0; //NULL
	beamOnAction2_ = 0; //NULL
	beamOnActionsList_ = 0; //NULL

	stopMotorsActions1_ = 0; //NULL
	stopMotorsActions2_ = 0; //NULL
	stopMotorsActions3_ = 0; //NULL
	stopMotorsActions4_ = 0; //NULL
	stopMotorsActionsList_ = 0; //NULL
}

SGMBeamline::~SGMBeamline()
{
}

QString SGMBeamline::sgmGratingName(SGMBeamline::sgmGrating grating) const {
	if(grating == 0)
		return "lowGrating";
	else if(grating == 1)
		return "mediumGrating";
	else if(grating == 2)
		return "highGrating";
}

QString SGMBeamline::sgmHarmonicName(SGMBeamline::sgmHarmonic harmonic) const {
	if(harmonic == 1)
		return "firstHarmonic";
	else if(harmonic == 3)
		return "thirdHarmonic";
}

QStringList SGMBeamline::unconnectedCriticals() const{
	return criticalControlsSet_->unconnected();
}

bool SGMBeamline::detectorConnectedByName(QString name){
	for(int x = 0; x < unconnectedSets_.count(); x++){
		if( QString::compare(unconnectedSets_.at(x)->name().section(' ', 0, 0), name, Qt::CaseInsensitive) == 0)
			return false;
	}
	return true;
}

QString SGMBeamline::beamlineWarnings(){
	return beamlineWarnings_;
}

AMBeamlineControlMoveAction* SGMBeamline::beamOnAction(){
	if(!beamOnAction1_ && !beamOnAction2_)
		createBeamOnActions();
	return beamOnAction1_;
}

AMBeamlineParallelActionsList* SGMBeamline::beamOnActionsList(){
	if(!beamOnActionsList_)
		createBeamOnActions();
	return beamOnActionsList_;
}

AMBeamlineParallelActionsList* SGMBeamline::stopMotorsActionsList(){
	if(!stopMotorsActionsList_)
		createStopMotorsActions();
	return stopMotorsActionsList_;
}

QList<AM1BeamlineActionItem*> SGMBeamline::transferLoadlockOutActions() const {
	QList<AM1BeamlineActionItem*> rVal;
	rVal.append(transferAction1_);
	rVal.append(transferAction2_);
	rVal.append(transferAction3_);
	rVal.append(transferAction4_);
	rVal.append(transferAction5_);
	rVal.append(transferAction6_);
	return rVal;
}

QList<AM1BeamlineActionItem*> SGMBeamline::transferLoadlockInActions() const {
	QList<AM1BeamlineActionItem*> rVal;
	rVal.append(transferAction7_);
	rVal.append(transferAction8_);
	rVal.append(transferAction9_);
	rVal.append(transferAction10_);
	rVal.append(transferAction11_);
	rVal.append(transferAction12_);
	return rVal;
}

QList<AM1BeamlineActionItem*> SGMBeamline::transferChamberOutActions() const {
	QList<AM1BeamlineActionItem*> rVal;
	rVal.append(transferAction13_);
	rVal.append(transferAction14_);
	rVal.append(transferAction15_);
	rVal.append(transferAction16_);
	rVal.append(transferAction17_);
	rVal.append(transferAction18_);
	rVal.append(transferAction19_);
	rVal.append(transferAction20_);
	return rVal;
}

QList<AM1BeamlineActionItem*> SGMBeamline::transferChamberInActions() const {
	QList<AM1BeamlineActionItem*> rVal;
	rVal.append(transferAction21_);
	rVal.append(transferAction22_);
	rVal.append(transferAction23_);
	rVal.append(transferAction24_);
	rVal.append(transferAction25_);
	return rVal;
}

bool SGMBeamline::isBeamlineScanning(){
	if( fabs(beamlineScanning_->value() -1.0) < beamlineScanning_->tolerance() )
		return true;
	return false;
}

bool SGMBeamline::isVisibleLightOn(){
	if(visibleLightToggle_->value() == 1)
		return true;
	return false;
}

bool SGMBeamline::energyValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double energy){
	if( (grating == 0) && (harmonic == 1) && (energy > 240) && (energy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == 1) && (energy > 440) && (energy < 1200) )
		return true;
	//else if( (grating == 2) && (harmonic == 1) && (energy > 800) && (energy < 1100) )
	else if( (grating == 2) && (harmonic == 1) && (energy > 800) && (energy < 1150) )
		return true;
	//else if( (grating == 2) && (harmonic == 3) && (energy > 1100) && (energy < 2000) )
	else if( (grating == 2) && (harmonic == 3) && (energy > 1050) && (energy < 2000) )
		return true;
	else
		return false;
}

bool SGMBeamline::energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy){
	if( (grating == 0) && (harmonic == 1) && (maxEnergy > 240) && (minEnergy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == 1) && (maxEnergy > 440) && (minEnergy < 1200) )
		return true;
	//else if( (grating == 2) && (harmonic == 1) && (maxEnergy > 800) && (minEnergy < 1100) )
	else if( (grating == 2) && (harmonic == 1) && (maxEnergy > 800) && (minEnergy < 1150) )
		return true;
	//else if( (grating == 2) && (harmonic == 3) && (maxEnergy > 1100) && (minEnergy < 2000) )
	else if( (grating == 2) && (harmonic == 3) && (maxEnergy > 1050) && (minEnergy < 2000) )
		return true;
	else
		return false;
}

QList< QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> > SGMBeamline::gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy){
	QList< QPair<sgmGrating, sgmHarmonic> > rVal;
	if( (maxEnergy > 240) && (maxEnergy < 750) && (minEnergy > 240) && (minEnergy < 750) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)0, (SGMBeamline::sgmHarmonic)1 ));
	if((maxEnergy > 440) && (maxEnergy < 1200) && (minEnergy > 440) && (minEnergy < 1200) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)1, (SGMBeamline::sgmHarmonic)1 ));
	if( (maxEnergy > 800) && (maxEnergy < 1100) && (minEnergy > 800) && (minEnergy < 1100) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)2, (SGMBeamline::sgmHarmonic)1 ));
	if( (maxEnergy > 1100) && (maxEnergy < 2000) && (minEnergy > 1100) && (minEnergy < 2000) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)2, (SGMBeamline::sgmHarmonic)3 ));
	return rVal;
}

QPair<double, double> SGMBeamline::energyRangeForGratingHarmonic(SGMBeamline::sgmGrating grating, SGMBeamline::sgmHarmonic harmonic){
	QPair<double, double> rVal;
	if( (grating == 0) && (harmonic == 1) )
		rVal = QPair<double, double>(240, 750);
	else if( (grating == 1) && (harmonic == 1) )
		rVal = QPair<double, double>(440, 1200);
	else if( (grating == 2) && (harmonic == 1) )
		rVal = QPair<double, double>(800, 1100);
	else if( (grating == 2) && (harmonic == 3) )
		rVal = QPair<double, double>(1100, 2000);
	return rVal;
}

void SGMBeamline::visibleLightOn(){
	visibleLightToggle_->move(1);
}

void SGMBeamline::visibleLightOff(){
	visibleLightToggle_->move(0);
}

void SGMBeamline::closeVacuum(){
	if(activeEndstation_->value() == 0){
		ea1CloseVacuum1_->move(1);
		ea1CloseVacuum2_->move(1);
	}
	else if(activeEndstation_->value() == 1)
		ea2CloseVacuum_->move(1);
}

void SGMBeamline::onBeamlineScanningValueChanged(double value){
	bool isScanning;
	if( fabs(value - 1.0) < beamlineScanning_->tolerance() )
		isScanning = true;
	else
		isScanning = false;
	emit beamlineScanningChanged(isScanning);
}

void SGMBeamline::onControlSetConnected(bool csConnected){
	AMControlSet *ctrlSet = (AMControlSet*)QObject::sender();

	if(csConnected){
		//qDebug() << ctrlSet->name() << " is connected";
		unconnectedSets_.removeAll(ctrlSet);
		if(!teyDetector_ && ctrlSet->name() == "TEY Controls"){
			teyDetector_ = new AMSingleControlDetector(tey_->name(), tey_, this);
			teyDetector_->setDescription("TEY");
			teyDetectorNew_ = (AMSingleControlDetector*)teyDetector_;
			allDetectors_->addDetector(teyDetector_, true);
			XASDetectors_->addDetector(teyDetector_, true);
			allDetectorsNew_->addDetector(teyDetectorNew_);
			XASDetectorsNew_->addDetector(teyDetectorNew_);
		}
		else if(!tfyDetector_ && ctrlSet->name() == "TFY Controls"){
			tfyDetector_ = new MCPDetector(tfy_->name(), tfy_, tfyHVSetpoint_, tfyHVFbk_, this);
			tfyDetector_->setDescription("TFY");
			tfyDetectorNew_ = (MCPDetector*)tfyDetector_;
			allDetectors_->addDetector(tfyDetector_, true);
			XASDetectors_->addDetector(tfyDetector_, true);
			allDetectorsNew_->addDetector(tfyDetectorNew_);
			XASDetectorsNew_->addDetector(tfyDetectorNew_);
		}
		else if(!pgtDetector_ && ctrlSet->name() == "SDD Controls"){
			pgtDetector_ = new PGTDetector(pgt_->name(), pgt_, pgtHVSetpoint_, pgtHVFbk_, pgtIntegrationTime_, pgtIntegrationMode_, this);
			pgtDetector_->setDescription("SDD");
			pgtDetectorNew_ = (PGTDetector*)pgtDetector_;
			allDetectors_->addDetector(pgtDetector_, false);
			XASDetectors_->addDetector(pgtDetector_, false);
			allDetectorsNew_->addDetector(pgtDetectorNew_);
			XASDetectorsNew_->addDetector(pgtDetectorNew_);
		}
		else if(!i0Detector_ && ctrlSet->name() == "I0 Controls"){
			i0Detector_ = new AMSingleControlDetector(i0_->name(), i0_, this);
			i0Detector_->setDescription("I0");
			i0DetectorNew_ = (AMSingleControlDetector*)i0Detector_;
			allDetectors_->addDetector(i0Detector_, true);
			feedbackDetectors_->addDetector(i0Detector_, true);
			allDetectorsNew_->addDetector(i0DetectorNew_);
			feedbackDetectorsNew_->addDetector(i0DetectorNew_);
		}
		else if(!eVFbkDetector_ && ctrlSet->name() == "Energy Feedback Controls"){
			eVFbkDetector_ = new AMSingleControlDetector(eVFbk_->name(), eVFbk_, this);
			eVFbkDetector_->setDescription("Energy Feedback");
			eVFbkDetectorNew_ = (AMSingleControlDetector*)eVFbkDetector_;
			allDetectors_->addDetector(eVFbkDetector_, true);
			feedbackDetectors_->addDetector(eVFbkDetector_, true);
			allDetectorsNew_->addDetector(eVFbkDetectorNew_);
			feedbackDetectorsNew_->addDetector(eVFbkDetectorNew_);
		}
		else if(!photodiodeDetector_ && ctrlSet->name() == "Photodiode Controls"){
			photodiodeDetector_ = new AMSingleControlDetector(photodiode_->name(), photodiode_, this);
			photodiodeDetector_->setDescription("Photodiode");
			photodiodeDetectorNew_ = (AMSingleControlDetector*)photodiodeDetector_;
			allDetectors_->addDetector(photodiodeDetector_, true);
			allDetectorsNew_->addDetector(photodiodeDetectorNew_);
		}
		else if(!encoderUpDetector_ && ctrlSet->name() == "Encoder Up Controls"){
			encoderUpDetector_ = new AMSingleControlDetector(encoderUp_->name(), encoderUp_, this);
			encoderUpDetector_->setDescription("Encoder Up");
			encoderUpDetectorNew_ = (AMSingleControlDetector*)encoderUpDetector_;
			allDetectors_->addDetector(encoderUpDetector_, true);
			allDetectorsNew_->addDetector(encoderUpDetectorNew_);
		}
		else if(!encoderDownDetector_ && ctrlSet->name() == "Encoder Down Controls"){
			encoderDownDetector_ = new AMSingleControlDetector(encoderDown_->name(), encoderDown_, this);
			encoderDownDetector_->setDescription("Encoder Down");
			encoderDownDetectorNew_ = (AMSingleControlDetector*)encoderDownDetector_;
			allDetectors_->addDetector(encoderDownDetector_, true);
			allDetectorsNew_->addDetector(encoderDownDetectorNew_);
		}
		emit controlSetConnectionsChanged();
	}
	else{
		if( !unconnectedSets_.contains(ctrlSet) ){
			unconnectedSets_.append(ctrlSet);
			emit controlSetConnectionsChanged();
		}
		//qDebug() << ctrlSet->name() << " is NOT connected";
	}
	/*
	QString tmpStr = "Unconnected at end";
	for(int x = 0; x < unconnectedSets_.count(); x++)
		tmpStr.append(" "+unconnectedSets_.at(x)->name());
	qDebug() << tmpStr;
	*/
}

void SGMBeamline::onCriticalControlsConnectedChanged(bool isConnected, AMControl *control){
	//qDebug() << "Detected a critical contorl change " << control->name();
	emit criticalControlsConnectionsChanged();
}

void SGMBeamline::recomputeWarnings(){
	if(!criticalControlsSet_->isConnected()){
		beamlineWarnings_ = "Warning some critical beamline\ncontrols are not connected:\n";
		foreach(QString ctrlName, unconnectedCriticals())
			beamlineWarnings_.append("  "+ctrlName+"\n");
	}
	else
		beamlineWarnings_ = "";

	emit beamlineWarningsChanged(beamlineWarnings_);
}

void SGMBeamline::createBeamOnActions(){
	if(!beamOnControlSet_->isConnected())
		return;
	if(!beamOnActionsList_){
		beamOnActionsList_ = new AMBeamlineParallelActionsList(this);
	}
	if(!beamOnAction1_ && !beamOnAction2_){
		// Action to turn on beam for SGM:
		// Set beamOn to "1"
		// Set fastShutterVoltage to "0 V"
		beamOnAction1_ = new AMBeamlineControlMoveAction(beamOn(), this);
		beamOnAction1_->setSetpoint(1);
		beamOnAction2_ = new AMBeamlineControlMoveAction(fastShutterVoltage(), this);
		beamOnAction2_->setSetpoint(0);
//		beamOnActionsList_->addAction(0, beamOnAction1_);
//		beamOnActionsList_->appendAction(beamOnAction2_);
//		connect(beamOnAction2_, SIGNAL(finished()), this, SLOT(onBeamOnActionsFinsihed()));
		beamOnActionsList_->appendStage(new QList<AMBeamlineActionItem*>());
		beamOnActionsList_->appendAction(beamOnActionsList_->stageCount(), beamOnAction1_);
		beamOnActionsList_->appendAction(beamOnActionsList_->stageCount(), beamOnAction2_);
		connect(beamOnActionsList_, SIGNAL(listSucceeded()), this, SLOT(onBeamOnActionsFinsihed()));
	}
}

void SGMBeamline::onBeamOnActionsFinsihed(){
	if(beamOnAction1_ && beamOnAction2_ && beamOnAction1_->hasFinished() && beamOnAction2_->hasFinished()){
		disconnect(beamOnActionsList_, SIGNAL(listSucceeded()), this, SLOT(onBeamOnActionsFinsihed()));
//		beamOnActionsList_->deleteAction(1);
//		beamOnActionsList_->deleteAction(0);
		beamOnActionsList_->deleteStage(beamOnActionsList_->stageCount());
		delete beamOnAction1_;
		delete beamOnAction2_;
		beamOnAction1_ = 0; //NULL
		beamOnAction2_ = 0; //NULL
		createBeamOnActions();
	}
}

void SGMBeamline::createStopMotorsActions(){
	/*
	//if(!beamOnControlSet_->isConnected())
	//	return;
	if(!stopMotorsActionsList_){
		stopMotorsActionsList_ = new AMBeamlineParallelActionsList(this);
	}
	if(!stopMotorsActions1_ && !stopMotorsActions2_ && !stopMotorsActions3_ && !stopMotorsActions4_){
		// Action to stop motors for SGM:
		// Stop mono, exit slit,
		beamOnAction1_ = new AMBeamlineControlMoveAction(beamOn(), this);
		beamOnAction1_->setSetpoint(1);
		beamOnAction2_ = new AMBeamlineControlMoveAction(fastShutterVoltage(), this);
		beamOnAction2_->setSetpoint(0);
		stopMotorsActionsList_->appendStage(new QList<AMBeamlineActionItem*>());
		stopMotorsActionsList_->appendAction(beamOnActionsList_->stageCount(), beamOnAction1_);
		stopMotorsActionsList_->appendAction(beamOnActionsList_->stageCount(), beamOnAction2_);
		connect(stopMotorsActionsList_, SIGNAL(listSucceeded()), this, SLOT(onBeamOnActionsFinsihed()));
	}
	*/
}

void SGMBeamline::onStopMotorsActionsFinished(){
	/*
	if(beamOnAction1_ && beamOnAction2_ && beamOnAction1_->hasFinished() && beamOnAction2_->hasFinished()){
		disconnect(beamOnActionsList_, SIGNAL(listSucceeded()), this, SLOT(onBeamOnActionsFinsihed()));
//		beamOnActionsList_->deleteAction(1);
//		beamOnActionsList_->deleteAction(0);
		beamOnActionsList_->deleteStage(beamOnActionsList_->stageCount());
		delete beamOnAction1_;
		delete beamOnAction2_;
		beamOnAction1_ = 0; //NULL
		beamOnAction2_ = 0; //NULL
		createStopMotorsActions();
	}
	*/
}


void SGMBeamline::onVisibleLightChanged(double value){
	Q_UNUSED(value);
	AMPVwStatusControl *lMono = qobject_cast<AMPVwStatusControl*>(mono());
	if(visibleLightToggle_->value() == 1 && visibleLightStatus_->value() == 8)
		emit visibleLightStatusChanged("Visible Light\n is ON");
	else if( visibleLightToggle_->value() == 1 && visibleLightStatus_->value() == 0 && lMono->movingPV()->getString() == "MOVING - CCW" )
		emit visibleLightStatusChanged("Visible Light\n is moving to ON");
	else if( visibleLightToggle_->value() == 1 && visibleLightStatus_->value() == 0 && lMono->movingPV()->getString() == "MOVING +  CW" )
		visibleLightToggle_->move(0);
	else if( visibleLightStatus_->value() == 8)
		emit visibleLightStatusChanged("Visible Light\n is moving to OFF");
	else if( visibleLightStatus_->value() == 0)
		emit visibleLightStatusChanged("Visible Light\n is OFF");
}

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0)
		instance_ = new SGMBeamline();

	return static_cast<SGMBeamline*>(instance_);

}





SGMFluxOptimization::SGMFluxOptimization(QObject *parent) : AMControlOptimization(parent) {
	name_ = "SGMFlux";
}

QMap<double, double> SGMFluxOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters){
	double _maxenergy = maximumEnergy(contextParameters);
	double _minenergy = minimumEnergy(contextParameters);
	double _maxflux = 0;
	double _minflux = 0;
	double _maximum = 0;
	double _minimum = 1;
	double _slit = stateParameters.at(0).toDouble();
	SGMBeamline::sgmGrating _grating = (SGMBeamline::sgmGrating)stateParameters.at(1).toInt();
	SGMBeamline::sgmHarmonic _harmonic = (SGMBeamline::sgmHarmonic)stateParameters.at(2).toInt();
	if(!SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
	{
	}
	else if((_harmonic == 3) && (_grating == 2)){

		_maxflux = 1.2;
		_minflux = 1.05;
		_maximum = 1600;
		_minimum = 1100;
	}
	else{
		if( (_grating == 0) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 5.75;
			_minflux = 0.5;
			_maximum = 475;
			_minimum = 200;
		}
		else if( (_grating == 1) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 3.25;
			_minflux = 0.5;
			_maximum = 815;
			_minimum = 450;
		}
		else if( (_grating == 2) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_maxflux = 1.1;
			_minflux = 0.5;
			_maximum = 1075;
			_minimum = 800;
		}
	}
	QMap<double, double> rCurve;
	double tmpStart, tmpEnd, tmpDelta;
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);
		if( tmpStart >= 250 && tmpStart <= 2000 && tmpEnd >= 250 && tmpEnd <= 2000 ){
			for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
				rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 : (_slit/62500)*(500-_slit)* ( ((_minflux-_maxflux)/((_minimum-_maximum)*(_minimum-_maximum)))*(y-_maximum)*(y-_maximum)+_maxflux );
			}
		}
		else
			rCurve[250] = 0;
	}
	return rCurve;
}

QMap< QString, QMap<double, double> > SGMFluxOptimization::collapse(AMRegionsList *contextParameters){
	QList<QVariant> l1, m1, h1, h3;
	l1 << 250.0 << 0 << 1;
	m1 << 250.0 << 1 << 1;
	h1 << 250.0 << 2 << 1;
	h3 << 250.0 << 2 << 3;
	int numPoints = 50;
	//int numPoints = 100;
	double stepSize = 250/(numPoints-1);
	QMap<double, double> fluxL1, fluxM1, fluxH1, fluxH3;
	for(double x = stepSize; x < 250; x+=stepSize){
		l1.replace(0, x);
		m1.replace(0, x);
		h1.replace(0, x);
		h3.replace(0, x);

		fluxL1.insert(x, collapser(curve(l1, contextParameters)));
		fluxM1.insert(x, collapser(curve(m1, contextParameters)));
		fluxH1.insert(x, collapser(curve(h1, contextParameters)));
		fluxH3.insert(x, collapser(curve(h3, contextParameters)));
	}

	QMap< QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", fluxL1);
	rVal.insert("MEG1", fluxM1);
	rVal.insert("HEG1", fluxH1);
	rVal.insert("HEG3", fluxH3);
	return rVal;
}

double SGMFluxOptimization::collapser(QMap<double, double> optCurve){
	QMap<double, double>::const_iterator i;
	i = optCurve.constBegin();
	double tmpMin = i.value();
	++i;
	while(i != optCurve.constEnd()){
		if(i.value() < tmpMin)
			tmpMin = i.value();
		++i;
	}
	return tmpMin;
}

double SGMFluxOptimization::maximumEnergy(AMRegionsList* regions){
	double curMax = 240;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMax = (regions->start(x) > curMax ? regions->start(x) : curMax);
		curMax = (regions->end(x) > curMax ? regions->end(x) : curMax);
	}
	return curMax;
}

double SGMFluxOptimization::minimumEnergy(AMRegionsList* regions){
	double curMin = 2000;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMin = (regions->start(x) < curMin ? regions->start(x) : curMin);
		curMin = (regions->end(x) < curMin ? regions->end(x) : curMin);
	}
	return curMin;
}


SGMResolutionOptimization::SGMResolutionOptimization(QObject *parent) : AMControlOptimization(parent) {
	name_ = "SGMResolution";
}

QMap<double, double> SGMResolutionOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters){
	double _maxenergy = maximumEnergy(contextParameters);
	double _minenergy = minimumEnergy(contextParameters);
	double _slit = stateParameters.at(0).toDouble();
	double _y1, _y2, _y3, _x1, _x2, _x3;
	double _maxRes = 0;
	SGMBeamline::sgmGrating _grating = (SGMBeamline::sgmGrating)stateParameters.at(1).toInt();
	SGMBeamline::sgmHarmonic _harmonic = (SGMBeamline::sgmHarmonic)stateParameters.at(2).toInt();
	if(!SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy))
	{
	}
	else if((_harmonic == 3) && (_grating == 2)){
		_y1 = (0.95)*(0.5229*log(_slit)+1.4221);
		_y2 = (0.95)*(0.4391*log(_slit)+1.2617);
		_y3 = (0.95)*(0.421*log(_slit)+0.9037);
		_x1 = 2000;
		_x2 = 1200;
		_x3 = 800;
		_maxRes = 12500;
	}
	else{
		if( (_grating == 0) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.4568*log(_slit)+1.0199;
			_y2 = 0.4739*log(_slit)+0.605;
			_y3 = 0.4257*log(_slit)+0.4438;
			_x1 = 600;
			_x2 = 400;
			_x3 = 250;
			_maxRes = 17500;
		}
		else if( (_grating == 1) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.425*log(_slit)+1.4936;
			_y2 = 0.4484*log(_slit)+1.0287;
			_y3 = 0.4029*log(_slit)+0.7914;
			_x1 = 1200;
			_x2 = 800;
			_x3 = 500;
			_maxRes = 15000;
		}
		else if( (_grating == 2) && SGMBeamline::sgm()->energyRangeValidForSettings(_grating, _harmonic, _minenergy, _maxenergy) ){
			_y1 = 0.5229*log(_slit)+1.4221;
			_y2 = 0.4391*log(_slit)+1.2617;
			_y3 = 0.421*log(_slit)+0.9037;
			_x1 = 2000;
			_x2 = 1200;
			_x3 = 800;
			_maxRes = 13750;
		}
	}
	int i, n;
	double xi, yi, ei, chisq;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;

	n = 3;

	X = gsl_matrix_alloc (n, 3);
	y = gsl_vector_alloc (n);
	w = gsl_vector_alloc (n);

	c = gsl_vector_alloc (3);
	cov = gsl_matrix_alloc (3, 3);

	double ix[3];
	double iy[3];
	double ie[3];
	ix[0] = _x1;
	ix[1] = _x2;
	ix[2] = _x3;
	iy[0] = _y1;
	iy[1] = _y2;
	iy[2] = _y3;
	ie[0] = 0.1*iy[0];
	ie[1] = 0.1*iy[1];
	ie[2] = 0.1*iy[2];
	for (i = 0; i < n; i++)
	{
		xi = ix[i];
		yi = iy[i];
		ei = ie[i];

		gsl_matrix_set (X, i, 0, 1.0);
		gsl_matrix_set (X, i, 1, xi);
		gsl_matrix_set (X, i, 2, xi*xi);

		gsl_vector_set (y, i, yi);
		gsl_vector_set (w, i, 1.0/(ei*ei));
	}

	gsl_multifit_linear_workspace * work
			= gsl_multifit_linear_alloc (n, 3);
	gsl_multifit_wlinear (X, w, y, c, cov,
						  &chisq, work);
	gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

	QMap<double, double> rCurve;
	double tmpStart, tmpEnd, tmpDelta;//, tmpVal;
	for( int x = 0; x < contextParameters->count(); x++){
		tmpStart = contextParameters->start(x);
		tmpDelta = contextParameters->delta(x);
		tmpEnd = contextParameters->end(x);

		if( tmpStart >= 250 && tmpStart <= 2000 && tmpEnd >= 250 && tmpEnd <= 2000 ){
			for( double y = tmpStart; ((tmpDelta > 0) ? (y <= tmpEnd) : (y >= tmpEnd)); y += tmpDelta ){
				//rCurve[y] = !SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y) ? 0.0 :y/(pow(10, C(2)*y*y + C(1)*y + C(0))*1e-3);
				if(!SGMBeamline::sgm()->energyValidForSettings(_grating, _harmonic, y))
					rCurve[y] = 0.0;
				else{
					rCurve[y] = y/(pow(10, C(2)*y*y + C(1)*y + C(0))*1e-3);
					/*
				tmpVal = y/(pow(10, C(2)*y*y + C(1)*y + C(0))*1e-3);
				rCurve[y] = tmpVal - (1-exp(-tmpVal))*(tmpVal-_maxRes);
				*/
				}
			}
		}
		else
			rCurve[250] = 0;
	}


	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);
	return rCurve;
}

QMap< QString, QMap<double, double> > SGMResolutionOptimization::collapse(AMRegionsList *contextParameters){
	QList<QVariant> l1, m1, h1, h3;
	l1 << 250.0 << 0 << 1;
	m1 << 250.0 << 1 << 1;
	h1 << 250.0 << 2 << 1;
	h3 << 250.0 << 2 << 3;
	int numPoints = 50;
	//int numPoints = 100;
	double stepSize = 250/(numPoints-1);
	QMap<double, double> resL1, resM1, resH1, resH3;
	for(double x = stepSize; x < 250; x+=stepSize){
		l1.replace(0, x);
		m1.replace(0, x);
		h1.replace(0, x);
		h3.replace(0, x);

		resL1.insert(x, collapser(curve(l1, contextParameters)));
		resM1.insert(x, collapser(curve(m1, contextParameters)));
		resH1.insert(x, collapser(curve(h1, contextParameters)));
		resH3.insert(x, collapser(curve(h3, contextParameters)));
	}
	QMap< QString, QMap<double, double> > rVal;
	rVal.insert("LEG1", resL1);
	rVal.insert("MEG1", resM1);
	rVal.insert("HEG1", resH1);
	rVal.insert("HEG3", resH3);
	return rVal;
}

double SGMResolutionOptimization::collapser(QMap<double, double> optCurve){
	QMap<double, double>::const_iterator i;
	i = optCurve.constBegin();
	double tmpMin = i.value();
	++i;
	while(i != optCurve.constEnd()){
		if(i.value() < tmpMin)
			tmpMin = i.value();
		++i;
	}
	return tmpMin;
}

double SGMResolutionOptimization::maximumEnergy(AMRegionsList* regions){
	double curMax = 240;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMax = (regions->start(x) > curMax ? regions->start(x) : curMax);
		curMax = (regions->end(x) > curMax ? regions->end(x) : curMax);
	}
	return curMax;
}

double SGMResolutionOptimization::minimumEnergy(AMRegionsList* regions){
	double curMin = 2000;
	if(regions->count() == 0)
		return -1;
	for(int x = 0; x < regions->count(); x++){
		curMin = (regions->start(x) < curMin ? regions->start(x) : curMin);
		curMin = (regions->end(x) < curMin ? regions->end(x) : curMin);
	}
	return curMin;
}
