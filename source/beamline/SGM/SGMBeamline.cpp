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
	// Old grating PV is in reverse order plus "automatic" field, AddOn is increasing order and 0-indexed
	//amNames2pvNames_.set("grating", "SG16114I1001:choice");
	amNames2pvNames_.set("grating", "BL1611-ID-1:AddOns:grating");
	// Old harmonic PV is DBR_DOUBLE for some reason, AddOn is enum
	//amNames2pvNames_.set("harmonic", "BL1611-ID-1:harmonic");
	amNames2pvNames_.set("harmonic", "BL1611-ID-1:AddOns:harmonic");
	amNames2pvNames_.set("undulatorTracking", "UND1411-01:Energy:track");
	amNames2pvNames_.set("monoTracking", "SG16114I1001:Energy:track");
	amNames2pvNames_.set("exitSlitTracking", "PSL16114I1003:Energy:track");
	amNames2pvNames_.set("teyPico", "A1611-4-15:A:fbk");
	amNames2pvNames_.set("teyScaler", "BL1611-ID-1:mcs00:fbk");
	amNames2pvNames_.set("tfyPico", "A1611-4-16:A:fbk");
	amNames2pvNames_.set("tfyScaler", "BL1611-ID-1:mcs02:fbk");
	amNames2pvNames_.set("tfyHV", "PS1611401:109");
	amNames2pvNames_.set("pgt", "MCA1611-01:GetChannels");
	amNames2pvNames_.set("pgtHV", "MCA1611-01:Bias:Volt");
	amNames2pvNames_.set("pgtIntegrationTime", "MCA1611-01:Preset:Live");
	amNames2pvNames_.set("pgtIntegrationMode", "MCA1611-01:Preset:Live");
	amNames2pvNames_.set("I0Pico", "A1611-4-14:A:fbk");
	amNames2pvNames_.set("I0Scaler", "BL1611-ID-1:mcs01:fbk");
	amNames2pvNames_.set("photodiodePico", "A1611-4-13:A:fbk");
	amNames2pvNames_.set("photodiodeScaler", "BL1611-ID-1:mcs03:fbk");
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
	amNames2pvNames_.set("detectorSignalSource", "BL1611-ID-1:AddOns:signalSource");

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
	grating_ = new AMPVwStatusControl("grating", sgmPVName+":fbk", sgmPVName, "SMTR16114I1016:state", "SMTR16114I1016:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
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

	sgmPVName = amNames2pvNames_.valueF("teyPico");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	teyPico_ = new AMReadOnlyPVControl("teyPico", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("teyScaler");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	teyScaler_ = new AMReadOnlyPVControl("teyScaler", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfyPico");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	tfyPico_ = new AMReadOnlyPVControl("tfyPico", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfyScaler");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	tfyScaler_ = new AMReadOnlyPVControl("tfyScaler", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("tfyHV");
	tfyHV_ = new AMPVControl("tfyHV", sgmPVName+":vmon", sgmPVName+":v0set", QString(), this, 0.5);

	sgmPVName = amNames2pvNames_.valueF("pgt");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgt_ = new AMReadOnlyPVControl("pgt", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("pgtHV");
	pgtHV_ = new AMPVControl("pgtHV", sgmPVName+"Actual:fbk", sgmPVName, QString(), this, 0.5);

	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationTime");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgtIntegrationTime_ = new AMPVControl("pgtIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationMode");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	pgtIntegrationMode_ = new AMPVControl("pgtIntegrationMode", sgmPVName, sgmPVName, "", this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("I0Pico");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	i0Pico_ = new AMReadOnlyPVControl("I0Pico", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("I0Scaler");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	i0Scaler_ = new AMReadOnlyPVControl("I0Scaler", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("eVFbk");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	eVFbk_ = new AMReadOnlyPVControl("eVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("photodiodePico");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	photodiodePico_ = new AMReadOnlyPVControl("photodiodePico", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("photodiodeScaler");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	photodiodeScaler_ = new AMReadOnlyPVControl("photodiodeScaler", sgmPVName, this);

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
	ssaManipulatorX_ = new AMPVwStatusControl("ssaManipulatorX", sgmPVName+":fbk", sgmPVName+":sp", "SMTR16114I1018:state", "SMTR16114I1018:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	/*
	ssaManipulatorX_ = new AMPVwStatusControl("ssaManipulatorX", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorXStop_ = new AMPVControl("ssaManipulatorXStop", "SMTR16114I1012:emergStop", "SMTR16114I1012:emergStop", "", this, 0.1);
	*/
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorY");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaManipulatorY_ = new AMPVwStatusControl("ssaManipulatorY", sgmPVName+":fbk", sgmPVName+":sp", "SMTR16114I1019:state", "SMTR16114I1019:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	/*
	ssaManipulatorY_ = new AMPVwStatusControl("ssaManipulatorY", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorYStop_ = new AMPVControl("ssaManipulatorYStop", "SMTR16114I1013:emergStop", "SMTR16114I1013:emergStop", "", this, 0.1);
	*/
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorZ");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaManipulatorZ_ = new AMPVwStatusControl("ssaManipulatorZ", sgmPVName+":fbk", sgmPVName+":sp", "SMTR16114I1020:state", "SMTR16114I1020:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	/*
	ssaManipulatorZ_ = new AMPVwStatusControl("ssaManipulatorZ", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorZStop_ = new AMPVControl("ssaManipulatorZStop", "SMTR16114I1014:emergStop", "SMTR16114I1014:emergStop", "", this, 0.1);
	*/
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorRot");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaManipulatorRot_ = new AMPVwStatusControl("ssaManipulatorRot", sgmPVName+":fbk", sgmPVName+":sp", "SMTR16114I1021:state", "SMTR16114I1021:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	/*
	ssaManipulatorRot_ = new AMPVwStatusControl("ssaManipulatorRot", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", "", this, 0.1);
	ssaManipulatorRotStop_ = new AMPVControl("ssaManipulatorRotStop", "SMTR16114I1015:emergStop", "SMTR16114I1015:emergStop", "", this, 0.1);
	*/

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

	sgmPVName = amNames2pvNames_.valueF("detectorSignalSource");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	detectorSignalSource_ = new AMPVControl("detectorSignalSource", sgmPVName, sgmPVName, "", this, 0.5);

	qDebug() << "\nPV Name Look Ups Failed: " << pvNameLookUpFail << "\n";
}

void SGMBeamline::usingFakeBeamline(){
	/*
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
	amNames2pvNames_.set("tfyHV", "reixsHost:tfy:hv");
	amNames2pvNames_.set("pgt", "reixsHost:sdd:spectrum");
	amNames2pvNames_.set("pgtHV", "reixsHost:sdd:hv");
	amNames2pvNames_.set("pgtIntegrationTime", "reixsHost:sdd:integration:time");
	amNames2pvNames_.set("pgtIntegrationMode", "reixsHost:sdd:integration:mode");
	amNames2pvNames_.set("I0", "reixsHost:I0");
	amNames2pvNames_.set("photodiode", "reixsHost:photodiode");
	amNames2pvNames_.set("encoderUp", "reixsHost:photodiode");
	amNames2pvNames_.set("encoderDown", "reixsHost:photodiode");
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
	addChildControl(ringCurrent_);

	QString sgmPVName = amNames2pvNames_.valueF("energy");
	energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, sgmPVName+":moving", "", this, 0.01);
	sgmPVName = amNames2pvNames_.valueF("mono");
	AMPVwStatusControl *mono = new AMPVwStatusControl("mono", sgmPVName, sgmPVName, sgmPVName+":moving", sgmPVName+":stop", energy_, 5);
	sgmPVName = amNames2pvNames_.valueF("undulator");
	AMPVwStatusControl *undulator = new AMPVwStatusControl("undulator", sgmPVName, sgmPVName, sgmPVName+":moving", sgmPVName+":stop", energy_, 0.1);
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	AMPVwStatusControl *exitSlit = new AMPVwStatusControl("exitSlit", sgmPVName, sgmPVName, sgmPVName+":moving", sgmPVName+":stop", energy_, 0.1);
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);
	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":fbk", sgmPVName, sgmPVName+":moving", "", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("entranceSlitGap");
	entranceSlitGap_ = new AMPVwStatusControl("entranceSlitGap", sgmPVName+":fbk", sgmPVName, sgmPVName+":moving", sgmPVName+":stop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4");
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName+":moving", this, new AMControlStatusCheckerDefault(0));	/// \note Is this actually 0 for moving?  You were using 0.1, which was being integerized to 0.
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
	sgmPVName = amNames2pvNames_.valueF("tfyHV");
	tfyHV_ = new AMPVControl("tfyHV", sgmPVName+":fbk", sgmPVName+":sp", QString(), this, 0.5);

	sgmPVName = amNames2pvNames_.valueF("pgt");
	pgt_ = new AMReadOnlyWaveformBinningPVControl("pgt", sgmPVName, 0, 1024, this);
	sgmPVName = amNames2pvNames_.valueF("pgtHV");
	pgtHV_ = new AMPVControl("pgtHV", sgmPVName+":fbk", sgmPVName+":sp", QString(), this, 0.5);
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

	sgmPVName = amNames2pvNames_.valueF("encoderUp");
	encoderUp_ = new AMReadOnlyPVControl("encoderUp", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("encoderDown");
	encoderDown_ = new AMReadOnlyPVControl("encoderDown", sgmPVName, this);

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
	*/
}

SGMBeamline::SGMBeamline() : AMBeamline("SGMBeamline") {
	//usingFakeBeamline();
	usingSGMBeamline();

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
	addChildControl(teyPico_);
	addChildControl(teyScaler_);
	addChildControl(tfyPico_);
	addChildControl(tfyScaler_);
	addChildControl(tfyHV_);
	addChildControl(pgt_);
	addChildControl(pgtHV_);
	addChildControl(pgtIntegrationTime_);
	addChildControl(pgtIntegrationMode_);
	addChildControl(i0Pico_);
	addChildControl(i0Scaler_);
	addChildControl(eVFbk_);
	addChildControl(encoderUp_);
	addChildControl(encoderDown_);
	addChildControl(ssaManipulatorX_);
//	addChildControl(ssaManipulatorXStop_);
	addChildControl(ssaManipulatorY_);
//	addChildControl(ssaManipulatorYStop_);
	addChildControl(ssaManipulatorZ_);
//	addChildControl(ssaManipulatorZStop_);
	addChildControl(ssaManipulatorRot_);
//	addChildControl(ssaManipulatorRotStop_);
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
	addChildControl(detectorSignalSource_);
	connect(detectorSignalSource_, SIGNAL(valueChanged(double)), this, SLOT(onDetectorSignalSourceChanged(double)));

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

	transferLoadLockOutControlSet_ = new AMControlSet(this);
	transferLoadLockOutControlSet_->setName("Transfer Load Lock Out Controls");
	transferLoadLockOutControlSet_->addControl(loadlockCCG_);
	transferLoadLockOutControlSet_->addControl(loadlockTCG_);

	transferLoadLockInControlSet_ = new AMControlSet(this);
	transferLoadLockInControlSet_->setName("Transfer Load Lock In Controls");
	transferLoadLockInControlSet_->addControl(loadlockCCG_);
	transferLoadLockInControlSet_->addControl(loadlockTCG_);

	teyPicoControlSet_ = new AMControlSet(this);
	teyPicoControlSet_->setName("TEY Pico Controls");
	teyPicoControlSet_->addControl(teyPico_);
	teyPicoDetector_ = NULL;
	unconnectedSets_.append(teyPicoControlSet_);
	connect(teyPicoControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	teyScalerControlSet_ = new AMControlSet(this);
	teyScalerControlSet_->setName("TEY Scaler Controls");
	teyScalerControlSet_->addControl(teyScaler_);
	teyScalerDetector_ = NULL;
	unconnectedSets_.append(teyScalerControlSet_);
	connect(teyScalerControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	tfyPicoControlSet_ = new AMControlSet(this);
	tfyPicoControlSet_->setName("TFY Pico Controls");
	tfyPicoControlSet_->addControl(tfyPico_);
	tfyPicoControlSet_->addControl(tfyHV_);
	tfyPicoDetector_ = NULL;
	unconnectedSets_.append(tfyPicoControlSet_);
	connect(tfyPicoControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	tfyScalerControlSet_ = new AMControlSet(this);
	tfyScalerControlSet_->setName("TFY Scaler Controls");
	tfyScalerControlSet_->addControl(tfyScaler_);
	tfyScalerControlSet_->addControl(tfyHV_);
	tfyScalerDetector_ = NULL;
	unconnectedSets_.append(tfyScalerControlSet_);
	connect(tfyScalerControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	pgtControlSet_ = new AMControlSet(this);
	pgtControlSet_->setName("SDD Controls");
	pgtControlSet_->addControl(pgt_);
	pgtControlSet_->addControl(pgtHV_);
	pgtControlSet_->addControl(pgtIntegrationTime_);
	pgtControlSet_->addControl(pgtIntegrationMode_);
	pgtDetector_ = NULL;
	unconnectedSets_.append(pgtControlSet_);
	connect(pgtControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	i0PicoControlSet_ = new AMControlSet(this);
	i0PicoControlSet_->setName("I0 Pico Controls");
	i0PicoControlSet_->addControl(i0Pico_);
	i0PicoDetector_ = NULL;
	unconnectedSets_.append(i0PicoControlSet_);
	connect(i0PicoControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	i0ScalerControlSet_ = new AMControlSet(this);
	i0ScalerControlSet_->setName("I0 Scaler Controls");
	i0ScalerControlSet_->addControl(i0Scaler_);
	i0ScalerDetector_ = NULL;
	unconnectedSets_.append(i0ScalerControlSet_);
	connect(i0ScalerControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	eVFbkControlSet_ = new AMControlSet(this);
	eVFbkControlSet_->setName("Energy Feedback Controls");
	eVFbkControlSet_->addControl(eVFbk_);
	eVFbkDetector_ = NULL;
	unconnectedSets_.append(eVFbkControlSet_);
	connect(eVFbkControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	photodiodePicoControlSet_ = new AMControlSet(this);
	photodiodePicoControlSet_->setName("Photodiode Pico Controls");
	photodiodePicoControlSet_->addControl(photodiodePico_);
	photodiodePicoDetector_ = NULL;
	unconnectedSets_.append(photodiodePicoControlSet_);
	connect(photodiodePicoControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));
	photodiodeScalerControlSet_ = new AMControlSet(this);
	photodiodeScalerControlSet_->setName("Photodiode Scaler Controls");
	photodiodeScalerControlSet_->addControl(photodiodeScaler_);
	photodiodeScalerDetector_ = NULL;
	unconnectedSets_.append(photodiodeScalerControlSet_);
	connect(photodiodeScalerControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));


	encoderUpControlSet_ = new AMControlSet(this);
	encoderUpControlSet_->setName("Encoder Up Controls");
	encoderUpControlSet_->addControl(encoderUp_);
	encoderUpDetector_ = NULL;
	unconnectedSets_.append(encoderUpControlSet_);
	connect(encoderUpControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	encoderDownControlSet_ = new AMControlSet(this);
	encoderDownControlSet_->setName("Encoder Down Controls");
	encoderDownControlSet_->addControl(encoderDown_);
	encoderDownDetector_ = NULL;
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

	allDetectors_ = new AMDetectorSet(this);
	allDetectors_->setName("All Detectors");

	feedbackDetectors_ = new AMDetectorSet(this);
	feedbackDetectors_->setName("Feedback Detectors");

	XASDetectors_ = new AMDetectorSet(this);
	XASDetectors_->setName("XAS Detectors");

	currentSamplePlate_ = new AMSamplePlate(this);

	transferLoadLockOutAction1Help_.append(QPixmap(":/LoadLockOut/action1Image1.jpg"), "1");
	transferLoadLockOutAction1Help_.append(QPixmap(":/LoadLockOut/action1Image2.jpg"), "2");
	transferLoadLockOutAction2Help_.append(QPixmap(":/LoadLockOut/action2Image1.jpg"), "1");
	transferLoadLockOutAction2Help_.append(QPixmap(":/LoadLockOut/action2Image2.jpg"), "2");
	transferLoadLockOutAction3Help_.append(QPixmap(":/LoadLockOut/action3Image1.jpg"), "1");
	transferLoadLockOutAction3Help_.append(QPixmap(":/LoadLockOut/action3Image2.jpg"), "2");
	transferLoadLockOutAction4Help_.append(QPixmap(":/LoadLockOut/action4Image1.jpg"), "1");
	transferLoadLockOutAction4Help_.append(QPixmap(":/LoadLockOut/action4Image2.jpg"), "2");
	transferLoadLockOutAction5Help_.append(QPixmap(":/LoadLockOut/action5Image1.jpg"), "1");
	transferLoadLockOutAction5Help_.append(QPixmap(":/LoadLockOut/action5Image2.jpg"), "2");

	transferLoadLockInAction2Help_.append(QPixmap(":/LoadLockIn/action2Image1.jpg"), "1");
	transferLoadLockInAction2Help_.append(QPixmap(":/LoadLockIn/action2Image2.jpg"), "2");
	transferLoadLockInAction3Help_.append(QPixmap(":/LoadLockIn/action3Image1.jpg"), "1");
	transferLoadLockInAction3Help_.append(QPixmap(":/LoadLockIn/action3Image2.jpg"), "2");
	transferLoadLockInAction4Help_.append(QPixmap(":/LoadLockIn/action4Image1.jpg"), "1");
	transferLoadLockInAction4Help_.append(QPixmap(":/LoadLockIn/action4Image2.jpg"), "2");
	transferLoadLockInAction5Help_.append(QPixmap(":/LoadLockIn/action5Image1.jpg"), "1");
	transferLoadLockInAction5Help_.append(QPixmap(":/LoadLockIn/action5Image2.jpg"), "2");
	transferLoadLockInAction6Help_.append(QPixmap(":/LoadLockIn/action6Image1.jpg"), "1");

	transferChamberOutAction1Help_.append(QPixmap(":/ChamberOut/action1Image1.jpg"), "1");
	transferChamberOutAction3Help_.append(QPixmap(":/ChamberOut/action3Image1.jpg"), "1");
	transferChamberOutAction3Help_.append(QPixmap(":/ChamberOut/action3Image2.jpg"), "2");
	transferChamberOutAction5Help_.append(QPixmap(":/ChamberOut/action5Image1.jpg"), "1");
	transferChamberOutAction5Help_.append(QPixmap(":/ChamberOut/action5Image2.jpg"), "2");
	transferChamberOutAction6Help_.append(QPixmap(":/ChamberOut/action6Image1.jpg"), "1");
	transferChamberOutAction6Help_.append(QPixmap(":/ChamberOut/action6Image2.jpg"), "2");
	transferChamberOutAction7Help_.append(QPixmap(":/ChamberOut/action7Image1.jpg"), "1");
	transferChamberOutAction7Help_.append(QPixmap(":/ChamberOut/action7Image2.jpg"), "2");
	transferChamberOutAction7Help_.append(QPixmap(":/ChamberOut/action7Image3.jpg"), "3");
	transferChamberOutAction8Help_.append(QPixmap(":/ChamberOut/action8Image1.jpg"), "1");
	transferChamberOutAction8Help_.append(QPixmap(":/ChamberOut/action8Image2.jpg"), "2");

	transferChamberInAction1Help_.append(QPixmap(":/ChamberIn/action1Image1.jpg"), "1");
	transferChamberInAction1Help_.append(QPixmap(":/ChamberIn/action1Image2.jpg"), "2");
	transferChamberInAction2Help_.append(QPixmap(":/ChamberIn/action2Image1.jpg"), "1");
	transferChamberInAction2Help_.append(QPixmap(":/ChamberIn/action2Image2.jpg"), "2");
	transferChamberInAction3Help_.append(QPixmap(":/ChamberIn/action3Image1.jpg"), "1");
	transferChamberInAction3Help_.append(QPixmap(":/ChamberIn/action3Image2.jpg"), "2");
	transferChamberInAction3Help_.append(QPixmap(":/ChamberIn/action3Image3.jpg"), "3");
	transferChamberInAction4Help_.append(QPixmap(":/ChamberIn/action4Image1.jpg"), "1");
	transferChamberInAction4Help_.append(QPixmap(":/ChamberIn/action42Image2.jpg"), "2");
}

SGMBeamline::~SGMBeamline()
{
}

QString SGMBeamline::sgmGratingName(SGMBeamline::sgmGrating grating) const {
	if(grating == SGMBeamline::lowGrating)
		return "lowGrating";
	else if(grating == SGMBeamline::mediumGrating)
		return "mediumGrating";
	else if(grating == SGMBeamline::highGrating)
		return "highGrating";
	else
		return "ERROR";
}

QString SGMBeamline::sgmGratingDescription(SGMBeamline::sgmGrating grating) const{
	if(grating == SGMBeamline::lowGrating)
		return "Low Energy";
	else if(grating == SGMBeamline::mediumGrating)
		return "Medium Energy";
	else if(grating == SGMBeamline::highGrating)
		return "High Energy";
	else
		return "ERROR";
}

QString SGMBeamline::sgmHarmonicName(SGMBeamline::sgmHarmonic harmonic) const {
	if(harmonic == SGMBeamline::firstHarmonic)
		return "firstHarmonic";
	else if(harmonic == SGMBeamline::thirdHarmonic)
		return "thirdHarmonic";
	else
		return "ERROR";
}

QString SGMBeamline::sgmHarmonicDescription(SGMBeamline::sgmHarmonic harmonic) const{
	if(harmonic == SGMBeamline::firstHarmonic)
		return "First";
	else if(harmonic == SGMBeamline::thirdHarmonic)
		return "Third";
	else
		return "ERROR";
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

bool SGMBeamline::detectorValidForCurrentSignalSource(AMDetector *detector){
	if(detectorSignalSource_->value() == 0)//ENUM 0 is Picoammeters
		if( (detector == teyPicoDetector_) || (detector == tfyPicoDetector_) || (detector == i0PicoDetector_) || (detector == photodiodePicoDetector_))
			return false;
	if(detectorSignalSource_->value() == 1)//ENUM 1 is Scaler
		if( (detector == teyScalerDetector_) || (detector == tfyScalerDetector_) || (detector == i0ScalerDetector_) || (detector == photodiodeScalerDetector_))
			return false;
	return true;
}

bool SGMBeamline::detectorValidForCurrentSignalSource(AMDetectorInfo *detectorInfo){
	if(detectorSignalSource_->value() == 0)//ENUM 0 is Picoammeters, so if the names are Scalers we're messed up
		if( (detectorInfo->name() == teyScalerDetector_->toInfo()->name()) || (detectorInfo->name() == tfyScalerDetector_->toInfo()->name()) || (detectorInfo->name() == i0ScalerDetector_->toInfo()->name()) || (detectorInfo->name() == photodiodeScalerDetector_->toInfo()->name()))
			return false;
	if(detectorSignalSource_->value() == 1)//ENUM 1 is Scalers, so if the names are Picos we're messed up
		if( (detectorInfo->name() == teyPicoDetector_->toInfo()->name()) || (detectorInfo->name() == tfyPicoDetector_->toInfo()->name()) || (detectorInfo->name() == i0PicoDetector_->toInfo()->name()) || (detectorInfo->name() == photodiodePicoDetector_->toInfo()->name()))
			return false;
	return true;
}

bool SGMBeamline::usingPicoammeterSource(){
	if(detectorSignalSource_ && detectorSignalSource_->isConnected())
		return (detectorSignalSource_->value() == 0);//ENUM 0 is Picoammeters
	return false;
}

bool SGMBeamline::usingScalerSource(){
	if(detectorSignalSource_ && detectorSignalSource_->isConnected())
		return (detectorSignalSource_->value() == 1);//ENUM 1 is Scaler
	return false;
}

AMBeamlineListAction* SGMBeamline::createBeamOnActions(){
	if(!beamOnControlSet_->isConnected())
		return 0;// NULL
	AMBeamlineParallelActionsList *beamOnActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *beamOnAction = new AMBeamlineListAction(beamOnActionsList);
	// Action to turn on beam for SGM:
	// Set beamOn to "1"
	// Set fastShutterVoltage to "0 V"
	AMBeamlineControlMoveAction *beamOnAction1 = new AMBeamlineControlMoveAction(beamOn());
	beamOnAction1->setSetpoint(1);
	AMBeamlineControlMoveAction *beamOnAction2 = new AMBeamlineControlMoveAction(fastShutterVoltage());
	beamOnAction2->setSetpoint(0);

	beamOnActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	beamOnActionsList->appendAction(beamOnActionsList->stageCount(), beamOnAction1);
	beamOnActionsList->appendAction(beamOnActionsList->stageCount(), beamOnAction2);
	return beamOnAction;
}

AMBeamlineListAction* SGMBeamline::createStopMotorsAction(){
	AMBeamlineParallelActionsList *stopMotorsActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *stopMotorsAction = new AMBeamlineListAction(stopMotorsActionsList);
	// Action to stop motors for SGM:
	// Stop mono, exit slit,
	AMBeamlineControlStopAction *stopMotorsAction1 = new AMBeamlineControlStopAction(mono());
	AMBeamlineControlStopAction *stopMotorsAction2 = new AMBeamlineControlStopAction(exitSlit());
	AMBeamlineControlStopAction *stopMotorsAction3 = new AMBeamlineControlStopAction(undulator());

	stopMotorsActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	stopMotorsActionsList->appendAction(0, stopMotorsAction1);
	stopMotorsActionsList->appendAction(0, stopMotorsAction2);
	stopMotorsActionsList->appendAction(0, stopMotorsAction3);
	return stopMotorsAction;
}

AMBeamlineListAction* SGMBeamline::createGoToTransferPositionActions(){
	AMBeamlineParallelActionsList *gotoTransferPositionActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *gotoTransferPositionAction = new AMBeamlineListAction(gotoTransferPositionActionsList);
	// Action to stop motors for SGM:
	// Stop mono, exit slit,
	AMBeamlineControlMoveAction *gotoTransferPositionAction1 = new AMBeamlineControlMoveAction(ssaManipulatorX());
	gotoTransferPositionAction1->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoTransferPositionAction2 = new AMBeamlineControlMoveAction(ssaManipulatorY());
	gotoTransferPositionAction2->setSetpoint(-5.0);
	AMBeamlineControlMoveAction *gotoTransferPositionAction3 = new AMBeamlineControlMoveAction(ssaManipulatorZ());
	gotoTransferPositionAction3->setSetpoint(-77.44);
	AMBeamlineControlMoveAction *gotoTransferPositionAction4 = new AMBeamlineControlMoveAction(ssaManipulatorRot());
	gotoTransferPositionAction4->setSetpoint(0.0);

	gotoTransferPositionActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction1);
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction2);
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction3);
	gotoTransferPositionActionsList->appendAction(0, gotoTransferPositionAction4);
	return gotoTransferPositionAction;
}

AMBeamlineListAction* SGMBeamline::createGoToMeasurementPositionActions(){
	AMBeamlineParallelActionsList *gotoMeasurementPositionActionsList = new AMBeamlineParallelActionsList();
	AMBeamlineListAction *gotoMeasurementPositionAction = new AMBeamlineListAction(gotoMeasurementPositionActionsList);
	// Action to stop motors for SGM:
	// Stop mono, exit slit,
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction1 = new AMBeamlineControlMoveAction(ssaManipulatorX());
	gotoMeasurementPositionAction1->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction2 = new AMBeamlineControlMoveAction(ssaManipulatorY());
	gotoMeasurementPositionAction2->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction3 = new AMBeamlineControlMoveAction(ssaManipulatorZ());
	gotoMeasurementPositionAction3->setSetpoint(0.0);
	AMBeamlineControlMoveAction *gotoMeasurementPositionAction4 = new AMBeamlineControlMoveAction(ssaManipulatorRot());
	gotoMeasurementPositionAction4->setSetpoint(0.0);

	gotoMeasurementPositionActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction1);
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction2);
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction3);
	gotoMeasurementPositionActionsList->appendAction(0, gotoMeasurementPositionAction4);
	return gotoMeasurementPositionAction;
}

AMBeamlineListAction* SGMBeamline::createTransferActions(SGMBeamline::sgmTransferType transferType){
	switch(transferType){
	case SGMBeamline::loadlockOut :
		return createTransferLoadLockOutActions();
	case SGMBeamline::loadlockIn :
		return createTransferLoadLockInActions();
	case SGMBeamline::ChamberOut :
		return createTransferChamberOutActions();
	case SGMBeamline::ChamberIn :
		return createTransferChamberInActions();
	default:
		return 0; //NULL
	}
}

AMBeamlineListAction* SGMBeamline::createTransferLoadLockOutActions(){
	if(!transferLoadLockOutControlSet_->isConnected())
		return 0;// NULL
	AMBeamlineParallelActionsList *transferLoadLockOutList = new AMBeamlineParallelActionsList(this);
	AMBeamlineListAction *transferLoadLockOutAction = new AMBeamlineListAction(transferLoadLockOutList, this);

	AMBeamlineUserConfirmAction *transferLoadLockOutAction1 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockOutAction1->setMessage("Close the valve between the endstation and the loadlock");
	transferLoadLockOutAction1->setHelp(transferLoadLockOutAction1Help_);
	AMBeamlineControlWaitAction *transferLoadLockOutAction2 = new AMBeamlineControlWaitAction(loadlockCCG_, AMBeamlineControlWaitAction::GreaterThanTarget, this);
	transferLoadLockOutAction2->setMessage("Turn off the CCG");
	transferLoadLockOutAction2->setWaitpoint(9e-5);
	transferLoadLockOutAction2->setHelp(transferLoadLockOutAction2Help_);
	AMBeamlineUserConfirmAction *transferLoadLockOutAction3 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockOutAction3->setMessage("Close the roughing pump valve");
	transferLoadLockOutAction3->setHelp(transferLoadLockOutAction3Help_);
	AMBeamlineUserConfirmAction *transferLoadLockOutAction4 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockOutAction4->setMessage("Turn off the turbo pump power");
	transferLoadLockOutAction4->setHelp(transferLoadLockOutAction5Help_);
	AMBeamlineControlWaitAction *transferLoadLockOutAction5 = new AMBeamlineControlWaitAction(loadlockTCG_, AMBeamlineControlWaitAction::GreaterThanTarget, this);
	transferLoadLockOutAction5->setMessage("Wait for loadlock to reach air pressure");
	transferLoadLockOutAction5->setWaitpoint(700);
	transferLoadLockOutAction5->setHelp(transferLoadLockOutAction5Help_);
	AMBeamlineUserConfirmAction *transferLoadLockOutAction6 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockOutAction6->setMessage("Open loadlock port");

	/* NTBA April 1, 2011 David Chevrier
		   Really concerned about this type of new-ing
		*/
	transferLoadLockOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockOutList->appendAction(0, transferLoadLockOutAction1);
	transferLoadLockOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockOutList->appendAction(1, transferLoadLockOutAction2);
	transferLoadLockOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockOutList->appendAction(2, transferLoadLockOutAction3);
	transferLoadLockOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockOutList->appendAction(3, transferLoadLockOutAction4);
	transferLoadLockOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockOutList->appendAction(4, transferLoadLockOutAction5);
	transferLoadLockOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockOutList->appendAction(5, transferLoadLockOutAction6);

	return transferLoadLockOutAction;
}

AMBeamlineListAction* SGMBeamline::createTransferLoadLockInActions(){
	if(!transferLoadLockInControlSet_->isConnected())
		return 0;//NULL
	AMBeamlineParallelActionsList *transferLoadLockInList = new AMBeamlineParallelActionsList(this);
	AMBeamlineListAction *transferLoadLockInAction = new AMBeamlineListAction(transferLoadLockInList, this);

	AMBeamlineUserConfirmAction *transferLoadLockInAction1 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockInAction1->setMessage("Close loadlock port");
	AMBeamlineUserConfirmAction *transferLoadLockInAction2 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockInAction2->setMessage("Open the roughing pump valve");
	transferLoadLockInAction2->setHelp(transferLoadLockInAction2Help_);
	AMBeamlineControlWaitAction *transferLoadLockInAction3 = new AMBeamlineControlWaitAction(loadlockTCG_, AMBeamlineControlWaitAction::LessThanTarget, this);
	transferLoadLockInAction3->setMessage("Wait for loadlock to reach rough vacuum");
	transferLoadLockInAction3->setWaitpoint(100);
	transferLoadLockInAction3->setHelp(transferLoadLockInAction3Help_);
	AMBeamlineUserConfirmAction *transferLoadLockInAction4 = new AMBeamlineUserConfirmAction(this);
	transferLoadLockInAction4->setMessage("Turn on the turbo pump power");
	transferLoadLockInAction4->setHelp(transferLoadLockInAction4Help_);
	AMBeamlineControlWaitAction *transferLoadLockInAction5 = new AMBeamlineControlWaitAction(loadlockCCG_, AMBeamlineControlWaitAction::LessThanTarget, this);
	transferLoadLockInAction5->setMessage("Turn on the CCG");
	transferLoadLockInAction5->setWaitpoint(9e-5);
	transferLoadLockInAction5->setHelp(transferLoadLockInAction5Help_);
	AMBeamlineControlWaitAction *transferLoadLockInAction6 = new AMBeamlineControlWaitAction(loadlockCCG_, AMBeamlineControlWaitAction::LessThanTarget, this);
	transferLoadLockInAction6->setMessage("Wait for loadlock to reach high vacuum");
	transferLoadLockInAction6->setWaitpoint(5e-6);
	transferLoadLockInAction6->setHelp(transferLoadLockInAction6Help_);

	/* NTBA April 1, 2011 David Chevrier
		   Really concerned about this type of new-ing
		*/
	transferLoadLockInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockInList->appendAction(0, transferLoadLockInAction1);
	transferLoadLockInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockInList->appendAction(1, transferLoadLockInAction2);
	transferLoadLockInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockInList->appendAction(2, transferLoadLockInAction3);
	transferLoadLockInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockInList->appendAction(3, transferLoadLockInAction4);
	transferLoadLockInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockInList->appendAction(4, transferLoadLockInAction5);
	transferLoadLockInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferLoadLockInList->appendAction(5, transferLoadLockInAction6);

	return transferLoadLockInAction;
}

AMBeamlineListAction* SGMBeamline::createTransferChamberOutActions(){
	AMBeamlineParallelActionsList *transferChamberOutList = new AMBeamlineParallelActionsList(this);
	AMBeamlineListAction *transferChamberOutAction = new AMBeamlineListAction(transferChamberOutList, this);

	AMBeamlineUserConfirmAction *transferChamberOutAction1 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction1->setMessage("Close Endstation Vacuum Valve");
	transferChamberOutAction1->setHelp(transferChamberOutAction1Help_);
	AMBeamlineUserConfirmAction *transferChamberOutAction2 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction2->setMessage("Turn off Detector High Voltage");
	AMBeamlineUserConfirmAction *transferChamberOutAction3 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction3->setMessage("Retract Detectors");
	transferChamberOutAction3->setHelp(transferChamberOutAction3Help_);
	AMBeamlineUserConfirmAction *transferChamberOutAction4 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction4->setMessage("Move to Transfer Position");
	AMBeamlineUserConfirmAction *transferChamberOutAction5 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction5->setMessage("Confirm Loadlock at High Vacuum");
	transferChamberOutAction5->setHelp(transferChamberOutAction5Help_);
	AMBeamlineUserConfirmAction *transferChamberOutAction6 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction6->setMessage("Open Transfer Valve");
	transferChamberOutAction6->setHelp(transferChamberOutAction6Help_);
	AMBeamlineUserConfirmAction *transferChamberOutAction7 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction7->setMessage("Transfer Sample Out");
	transferChamberOutAction7->setHelp(transferChamberOutAction7Help_);
	AMBeamlineUserConfirmAction *transferChamberOutAction8 = new AMBeamlineUserConfirmAction(this);
	transferChamberOutAction8->setMessage("Close Transfer Valve");
	transferChamberOutAction8->setHelp(transferChamberOutAction8Help_);

	/* NTBA April 1, 2011 David Chevrier
	   Really concerned about this type of new-ing
	*/
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(0, transferChamberOutAction1);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(1, transferChamberOutAction2);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(2, transferChamberOutAction3);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(3, transferChamberOutAction4);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(4, transferChamberOutAction5);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(5, transferChamberOutAction6);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(6, transferChamberOutAction7);
	transferChamberOutList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberOutList->appendAction(7, transferChamberOutAction8);

	return transferChamberOutAction;
}

AMBeamlineListAction* SGMBeamline::createTransferChamberInActions(){
	AMBeamlineParallelActionsList *transferChamberInList = new AMBeamlineParallelActionsList(this);
	AMBeamlineListAction *transferChamberInAction = new AMBeamlineListAction(transferChamberInList, this);

	AMBeamlineUserConfirmAction *transferChamberInAction1 = new AMBeamlineUserConfirmAction(this);
	transferChamberInAction1->setMessage("Confirm Loadlock at High Vacuum");
	transferChamberInAction1->setHelp(transferChamberInAction1Help_);
	AMBeamlineUserConfirmAction *transferChamberInAction2 = new AMBeamlineUserConfirmAction(this);
	transferChamberInAction2->setMessage("Open Transfer Valve");
	transferChamberInAction2->setHelp(transferChamberInAction2Help_);
	AMBeamlineUserConfirmAction *transferChamberInAction3 = new AMBeamlineUserConfirmAction(this);
	transferChamberInAction3->setMessage("Transfer Sample In");
	transferChamberInAction3->setHelp(transferChamberInAction3Help_);
	AMBeamlineUserConfirmAction *transferChamberInAction4 = new AMBeamlineUserConfirmAction(this);
	transferChamberInAction4->setMessage("Close Transfer Valve");
	transferChamberInAction4->setHelp(transferChamberInAction4Help_);
	AMBeamlineUserConfirmAction *transferChamberInAction5 = new AMBeamlineUserConfirmAction(this);
	transferChamberInAction5->setMessage("Turn on Detector High Voltage");

	/* NTBA April 1, 2011 David Chevrier
		   Really concerned about this type of new-ing
		*/
	transferChamberInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberInList->appendAction(0, transferChamberInAction1);
	transferChamberInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberInList->appendAction(1, transferChamberInAction2);
	transferChamberInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberInList->appendAction(2, transferChamberInAction3);
	transferChamberInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberInList->appendAction(3, transferChamberInAction4);
	transferChamberInList->appendStage(new QList<AMBeamlineActionItem*>());
	transferChamberInList->appendAction(4, transferChamberInAction5);

	return transferChamberInAction;
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
	if( (grating == 0) && (harmonic == SGMBeamline::firstHarmonic) && (energy > 240) && (energy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == SGMBeamline::firstHarmonic) && (energy > 440) && (energy < 1200) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamline::firstHarmonic) && (energy > 800) && (energy < 1150) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamline::thirdHarmonic) && (energy > 1050) && (energy < 2000) )
		return true;
	else
		return false;
}

bool SGMBeamline::energyRangeValidForSettings(sgmGrating grating, sgmHarmonic harmonic, double minEnergy, double maxEnergy){
	if( (grating == 0) && (harmonic == SGMBeamline::firstHarmonic) && (minEnergy > 240) && (maxEnergy > 240) && (minEnergy < 750) && (maxEnergy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == SGMBeamline::firstHarmonic) && (minEnergy > 440) && (maxEnergy > 440) && (minEnergy < 1200) && (maxEnergy < 1200) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamline::firstHarmonic) && (minEnergy > 800) && (maxEnergy > 800) && (minEnergy < 1150) && (maxEnergy < 1150) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamline::thirdHarmonic) && (minEnergy > 1050) && (maxEnergy > 1050) && (minEnergy < 2000) && (maxEnergy < 2000) )
		return true;
	else
		return false;
}

QList< QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> > SGMBeamline::gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy){
	QList< QPair<sgmGrating, sgmHarmonic> > rVal;
	if( (maxEnergy > 240) && (maxEnergy < 750) && (minEnergy > 240) && (minEnergy < 750) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)0, SGMBeamline::firstHarmonic ));
	if((maxEnergy > 440) && (maxEnergy < 1200) && (minEnergy > 440) && (minEnergy < 1200) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)1, SGMBeamline::firstHarmonic ));
	if( (maxEnergy > 800) && (maxEnergy < 1100) && (minEnergy > 800) && (minEnergy < 1100) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)2, SGMBeamline::firstHarmonic ));
	if( (maxEnergy > 1100) && (maxEnergy < 2000) && (minEnergy > 1100) && (minEnergy < 2000) )
		rVal.append(QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>((SGMBeamline::sgmGrating)2, SGMBeamline::thirdHarmonic ));
	return rVal;
}

QPair<double, double> SGMBeamline::energyRangeForGratingHarmonic(SGMBeamline::sgmGrating grating, SGMBeamline::sgmHarmonic harmonic){
	QPair<double, double> rVal;
	if( (grating == 0) && (harmonic == SGMBeamline::firstHarmonic) )
		rVal = QPair<double, double>(240, 750);
	else if( (grating == 1) && (harmonic == SGMBeamline::firstHarmonic) )
		rVal = QPair<double, double>(440, 1200);
	else if( (grating == 2) && (harmonic == SGMBeamline::firstHarmonic) )
		rVal = QPair<double, double>(800, 1100);
	else if( (grating == 2) && (harmonic == SGMBeamline::thirdHarmonic) )
		rVal = QPair<double, double>(1100, 2000);
	return rVal;
}

QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> SGMBeamline::forBestFlux(double minEnergy, double maxEnergy) const{
	bool straddlesTransition = false;
	double testEnergy = 0;
	QList<double> transitionPoints;
	transitionPoints << 625 << 1200;
	for(int x = 0; x < transitionPoints.count(); x++){
		if(minEnergy < transitionPoints.at(x) && maxEnergy > transitionPoints.at(x)){
			straddlesTransition = true;
			if( (transitionPoints.at(x)-minEnergy) >= (maxEnergy-transitionPoints.at(x)) )
				testEnergy = minEnergy;
			else
				testEnergy = maxEnergy;
		}
	}

	if(!straddlesTransition)
		testEnergy = minEnergy;

	if(testEnergy < 625)
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::lowGrating, SGMBeamline::firstHarmonic);
	else if(testEnergy < 1200)
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::mediumGrating, SGMBeamline::firstHarmonic);
	else
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::highGrating, SGMBeamline::thirdHarmonic);
}

QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic> SGMBeamline::forBestResolution(double minEnergy, double maxEnergy) const{
	bool straddlesTransition = false;
	double testEnergy = 0;
	QList<double> transitionPoints;
	transitionPoints << 400 << 800 << 1100;
	for(int x = 0; x < transitionPoints.count(); x++){
		if(minEnergy < transitionPoints.at(x) && maxEnergy > transitionPoints.at(x)){
			straddlesTransition = true;
			if( (transitionPoints.at(x)-minEnergy) >= (maxEnergy-transitionPoints.at(x)) )
				testEnergy = minEnergy;
			else
				testEnergy = maxEnergy;
		}
	}

	if(!straddlesTransition)
		testEnergy = minEnergy;

	if(testEnergy < 400)
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::lowGrating, SGMBeamline::firstHarmonic);
	else if(testEnergy < 800)
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::mediumGrating, SGMBeamline::firstHarmonic);
	else if(testEnergy < 1100)
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::highGrating, SGMBeamline::firstHarmonic);
	else
		return QPair<SGMBeamline::sgmGrating, SGMBeamline::sgmHarmonic>(SGMBeamline::highGrating, SGMBeamline::thirdHarmonic);
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
		unconnectedSets_.removeAll(ctrlSet);
		if(!teyPicoDetector_ && ctrlSet->name() == "TEY Pico Controls"){
			teyPicoDetector_ = new AMSingleControlDetector(teyPico_->name(), teyPico_, AMDetector::WaitRead, this);
			teyPicoDetector_->setDescription("TEY");
			allDetectors_->addDetector(teyPicoDetector_);
			XASDetectors_->addDetector(teyPicoDetector_, true);
		}
		else if(!teyScalerDetector_ && ctrlSet->name() == "TEY Scaler Controls"){
			teyScalerDetector_ = new AMSingleControlDetector(teyScaler_->name(), teyScaler_, AMDetector::WaitRead, this);
			teyScalerDetector_->setDescription("TEY");
			allDetectors_->addDetector(teyScalerDetector_);
			XASDetectors_->addDetector(teyScalerDetector_, true);
		}
		else if(!tfyPicoDetector_ && ctrlSet->name() == "TFY Pico Controls"){
			tfyPicoDetector_ = new MCPDetector(tfyPico_->name(), tfyPico_, tfyHV_, AMDetector::WaitRead, this);
			tfyPicoDetector_->setDescription("TFY");
			allDetectors_->addDetector(tfyPicoDetector_);
			XASDetectors_->addDetector(tfyPicoDetector_, true);
		}
		else if(!tfyScalerDetector_ && ctrlSet->name() == "TFY Scaler Controls"){
			tfyScalerDetector_ = new MCPDetector(tfyScaler_->name(), tfyScaler_, tfyHV_, AMDetector::WaitRead, this);
			tfyScalerDetector_->setDescription("TFY");
			allDetectors_->addDetector(tfyScalerDetector_);
			XASDetectors_->addDetector(tfyScalerDetector_, true);
		}
		else if(!pgtDetector_ && ctrlSet->name() == "SDD Controls"){
			pgtDetector_ = new PGTDetector(pgt_->name(), pgt_, pgtHV_, pgtIntegrationTime_, pgtIntegrationMode_, AMDetector::WaitRead, this);
			allDetectors_->addDetector(pgtDetector_);
			XASDetectors_->addDetector(pgtDetector_);
		}
		else if(!i0PicoDetector_ && ctrlSet->name() == "I0 Pico Controls"){
			i0PicoDetector_ = new AMSingleControlDetector(i0Pico_->name(), i0Pico_, AMDetector::WaitRead, this);
			i0PicoDetector_->setDescription("I0");
			allDetectors_->addDetector(i0PicoDetector_);
			feedbackDetectors_->addDetector(i0PicoDetector_);
		}
		else if(!i0ScalerDetector_ && ctrlSet->name() == "I0 Scaler Controls"){
			i0ScalerDetector_ = new AMSingleControlDetector(i0Scaler_->name(), i0Scaler_, AMDetector::WaitRead, this);
			i0ScalerDetector_->setDescription("I0");
			allDetectors_->addDetector(i0ScalerDetector_);
			feedbackDetectors_->addDetector(i0ScalerDetector_);
		}
		else if(!eVFbkDetector_ && ctrlSet->name() == "Energy Feedback Controls"){
			eVFbkDetector_ = new AMSingleControlDetector(eVFbk_->name(), eVFbk_, AMDetector::ImmediateRead, this);
			allDetectors_->addDetector(eVFbkDetector_);
			feedbackDetectors_->addDetector(eVFbkDetector_);
		}
		else if(!photodiodePicoDetector_ && ctrlSet->name() == "Photodiode Pico Controls"){
			photodiodePicoDetector_ = new AMSingleControlDetector(photodiodePico_->name(), photodiodePico_, AMDetector::WaitRead, this);
			photodiodePicoDetector_->setDescription("Photodiode");
			allDetectors_->addDetector(photodiodePicoDetector_);
		}
		else if(!photodiodeScalerDetector_ && ctrlSet->name() == "Photodiode Scaler Controls"){
			photodiodeScalerDetector_ = new AMSingleControlDetector(photodiodeScaler_->name(), photodiodeScaler_, AMDetector::WaitRead, this);
			photodiodeScalerDetector_->setDescription("Photodiode");
			allDetectors_->addDetector(photodiodeScalerDetector_);
		}
		else if(!encoderUpDetector_ && ctrlSet->name() == "Encoder Up Controls"){
			encoderUpDetector_ = new AMSingleControlDetector(encoderUp_->name(), encoderUp_, AMDetector::WaitRead, this);
			allDetectors_->addDetector(encoderUpDetector_);
		}
		else if(!encoderDownDetector_ && ctrlSet->name() == "Encoder Down Controls"){
			encoderDownDetector_ = new AMSingleControlDetector(encoderDown_->name(), encoderDown_, AMDetector::WaitRead, this);
			allDetectors_->addDetector(encoderDownDetector_);
		}
		if(detectorSignalSource_->isConnected())
			onDetectorSignalSourceChanged(detectorSignalSource_->value());
		emit controlSetConnectionsChanged();
	}
	else{
		if( !unconnectedSets_.contains(ctrlSet) ){
			unconnectedSets_.append(ctrlSet);
			emit controlSetConnectionsChanged();
		}
	}
}

void SGMBeamline::onCriticalControlsConnectedChanged(bool isConnected, AMControl *control){
	emit criticalControlsConnectionsChanged();
}

void SGMBeamline::onDetectorSignalSourceChanged(double value){
	if(value == 0){// ENUM 0 is "Picoammeters"
		if(allDetectors_->indexOf(teyScalerDetector_) >= 0)
			allDetectors_->removeDetector(teyScalerDetector_);
		if(allDetectors_->indexOf(teyPicoDetector_) < 0)
			allDetectors_->addDetector(teyPicoDetector_);
		if(XASDetectors_->indexOf(teyScalerDetector_) >= 0)
			XASDetectors_->removeDetector(teyScalerDetector_);
		if(XASDetectors_->indexOf(teyPicoDetector_) < 0)
			XASDetectors_->addDetector(teyPicoDetector_);
		if(allDetectors_->indexOf(tfyScalerDetector_) >= 0)
			allDetectors_->removeDetector(tfyScalerDetector_);
		if(allDetectors_->indexOf(tfyPicoDetector_) < 0)
			allDetectors_->addDetector(tfyPicoDetector_);
		if(XASDetectors_->indexOf(tfyScalerDetector_) >= 0)
			XASDetectors_->removeDetector(tfyScalerDetector_);
		if(XASDetectors_->indexOf(tfyPicoDetector_) < 0)
			XASDetectors_->addDetector(tfyPicoDetector_);
		if(allDetectors_->indexOf(i0ScalerDetector_) >= 0)
			allDetectors_->removeDetector(i0ScalerDetector_);
		if(allDetectors_->indexOf(i0PicoDetector_) < 0)
			allDetectors_->addDetector(i0PicoDetector_);
		if(feedbackDetectors_->indexOf(i0ScalerDetector_) >= 0)
			feedbackDetectors_->removeDetector(i0ScalerDetector_);
		if(feedbackDetectors_->indexOf(i0PicoDetector_) < 0)
			feedbackDetectors_->addDetector(i0PicoDetector_);
		if(allDetectors_->indexOf(photodiodeScalerDetector_) >= 0)
			allDetectors_->removeDetector(photodiodeScalerDetector_);
		if(allDetectors_->indexOf(photodiodePicoDetector_) < 0)
			allDetectors_->addDetector(photodiodePicoDetector_);
		if(feedbackDetectors_->indexOf(photodiodeScalerDetector_) >= 0)
			feedbackDetectors_->removeDetector(photodiodeScalerDetector_);
		if(feedbackDetectors_->indexOf(photodiodePicoDetector_) < 0)
			feedbackDetectors_->addDetector(photodiodePicoDetector_);
	}
	else if(value == 1){// ENUM 1 is "Scaler"
		if(allDetectors_->indexOf(teyScalerDetector_) < 0)
			allDetectors_->addDetector(teyScalerDetector_);
		if(allDetectors_->indexOf(teyPicoDetector_) >= 0)
			allDetectors_->removeDetector(teyPicoDetector_);
		if(XASDetectors_->indexOf(teyScalerDetector_) < 0)
			XASDetectors_->addDetector(teyScalerDetector_);
		if(XASDetectors_->indexOf(teyPicoDetector_) >= 0)
			XASDetectors_->removeDetector(teyPicoDetector_);
		if(allDetectors_->indexOf(tfyScalerDetector_) < 0)
			allDetectors_->addDetector(tfyScalerDetector_);
		if(allDetectors_->indexOf(tfyPicoDetector_) >= 0)
			allDetectors_->removeDetector(tfyPicoDetector_);
		if(XASDetectors_->indexOf(tfyScalerDetector_) < 0)
			XASDetectors_->addDetector(tfyScalerDetector_);
		if(XASDetectors_->indexOf(tfyPicoDetector_) >= 0)
			XASDetectors_->removeDetector(tfyPicoDetector_);
		if(allDetectors_->indexOf(i0ScalerDetector_) < 0)
			allDetectors_->addDetector(i0ScalerDetector_);
		if(allDetectors_->indexOf(i0PicoDetector_) >= 0)
			allDetectors_->removeDetector(i0PicoDetector_);
		if(feedbackDetectors_->indexOf(i0ScalerDetector_) < 0)
			feedbackDetectors_->addDetector(i0ScalerDetector_);
		if(feedbackDetectors_->indexOf(i0PicoDetector_) >= 0)
			feedbackDetectors_->removeDetector(i0PicoDetector_);
		if(allDetectors_->indexOf(photodiodeScalerDetector_) < 0)
			allDetectors_->addDetector(photodiodeScalerDetector_);
		if(allDetectors_->indexOf(photodiodePicoDetector_) >= 0)
			allDetectors_->removeDetector(photodiodePicoDetector_);
		if(feedbackDetectors_->indexOf(photodiodeScalerDetector_) < 0)
			feedbackDetectors_->addDetector(photodiodeScalerDetector_);
		if(feedbackDetectors_->indexOf(photodiodePicoDetector_) >= 0)
			feedbackDetectors_->removeDetector(photodiodePicoDetector_);
	}
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
