/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include <QStringBuilder>

#include "dataman/AMSamplePlate.h"
#include "dataman/AMUser.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/CLS/CLSCAEN2527HVChannel.h"
#include "beamline/CLS/CLSPGT8000HVChannel.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "SGMOptimizationSupport.h"

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
	amNames2pvNames_.set("undulatorMotor", "SMTR1411-01");
	amNames2pvNames_.set("undulatorTracking", "UND1411-01:Energy:track");
	amNames2pvNames_.set("monoTracking", "SG16114I1001:Energy:track");
	amNames2pvNames_.set("exitSlitTracking", "PSL16114I1003:Energy:track");
	amNames2pvNames_.set("tfyScaler", "BL1611-ID-1:mcs02:fbk");
	amNames2pvNames_.set("tfyHV", "PS1611401:109");
	amNames2pvNames_.set("pgtBase", "MCA1611-01");
	amNames2pvNames_.set("pgtHV", "MCA1611-01:Bias:Volt");
	amNames2pvNames_.set("amptekSDD1", "amptek:sdd1");
	amNames2pvNames_.set("amptekSDD2", "amptek:sdd2");
	amNames2pvNames_.set("loadlockCCG", "CCG1611-4-I10-09:vac:p");
	amNames2pvNames_.set("loadlockTCG", "TCGC1611-426:pressure:fbk");

	amNames2pvNames_.set("ssaManipulatorX", "SMTR16114I1022");
	amNames2pvNames_.set("ssaManipulatorY", "SMTR16114I1023");
	amNames2pvNames_.set("ssaManipulatorZ", "SMTR16114I1024");
	amNames2pvNames_.set("ssaManipulatorRot", "SMTR16114I1025");

	amNames2pvNames_.set("beamlineScanning", "BL1611-ID-1:scanning");
	amNames2pvNames_.set("beamlineReady", "BL1611-ID-1:beam:status");
	amNames2pvNames_.set("nextDwellTimeTrigger", "BL1611-ID-1:AddOns:trigger:dwellTime");
	amNames2pvNames_.set("nextDwellTimeConfirmed", "BL1611-ID-1:AddOns:confirmed:dwellTime");
	amNames2pvNames_.set("picoammeterDwellTime", "A1611I1:cont_interval");
	amNames2pvNames_.set("energyMovingStatus", "BL1611-ID-1:ready");
	amNames2pvNames_.set("fastShutterVoltage", "PSH16114I1001:V");
	amNames2pvNames_.set("scaler", "BL1611-ID-1:mcs:scan");
	amNames2pvNames_.set("scalerIntegrationTime", "BL1611-ID-1:mcs:delay");
	amNames2pvNames_.set("gratingVelocity", "SMTR16114I1002:velo");
	amNames2pvNames_.set("gratingBaseVelocity", "SMTR16114I1002:veloBase");
	amNames2pvNames_.set("gratingAcceleration", "SMTR16114I1002:accel");
	amNames2pvNames_.set("ea1CloseVacuum1", "VVR1611-4-I10-05:opr:close");
	amNames2pvNames_.set("ea1CloseVacuum2", "VVR1611-4-I10-06:opr:close");
	amNames2pvNames_.set("ea2CloseVacuum", "VVR1611-4-I10-08:opr:close");
	amNames2pvNames_.set("beamOn", "BL1611-ID-1:AddOns:beamon:trigger");
	amNames2pvNames_.set("visibleLightToggle", "BL1611-ID-1:visible");
	amNames2pvNames_.set("visibleLightStatus", "BL1611-ID-1:visible:cal");
	amNames2pvNames_.set("activeEndstation", "BL1611-ID-1:AddOns:endstation:active");
	amNames2pvNames_.set("ssaIllumination", "ILC1611-4-I10-02");
	amNames2pvNames_.set("mirrorStripeSelection", "BL1611-ID-1:stripe");
	amNames2pvNames_.set("masterDwell", "BL1611-ID-1:dwell:setTime");

	bool pvNameLookUpFail = false;

	QString sgmPVName = amNames2pvNames_.valueF("energy");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
		energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, "BL1611-ID-1:ready", sgmPVName, this, 0.25);
	energy_->setDescription("Energy");
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
	AMPVwStatusControl *mono = new AMPVwStatusControl("mono", sgmPVName+":enc:fbk", sgmPVName+":encTarget", sgmPVName+":status", "SMTR16114I1002:stop", energy_, 5, 2.0, new AMControlStatusCheckerCLSMAXv(), 1);
	mono->setDescription("Monochromator");
	sgmPVName = amNames2pvNames_.valueF("undulator");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *undulator = new AMPVwStatusControl("undulator", sgmPVName+":gap:mm:fbk", sgmPVName+":gap:mm", sgmPVName+":moveStatus", "UND1411-01:stop", energy_, 0.1);
	undulator->setDescription("Undulator");
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *exitSlit = new AMPVwStatusControl("exitSlit", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1003:status", "SMTR16114I1003:stop", energy_, 0.1, 2.0, new AMControlStatusCheckerDefault(1));
	exitSlit->setDescription("Exit Slit Position");
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);

	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	//exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1017:status", "SMTR16114I1017:stop", this, 0.5);
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", "BL1611-ID-1:AddOns:ExitSlitGap:Y:mm", "BL1611-ID-1:AddOns:ExitSlitGap:Y:status", "SMTR16114I1017:stop", this, 0.5);
	//((AMPVwStatusControl*)exitSlitGap_)->setMoveStartTolerance(0.02);
	exitSlitGap_->setDescription("Exit Slit Gap");
	sgmPVName = amNames2pvNames_.valueF("entranceSlitGap");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	entranceSlitGap_ = new AMPVwStatusControl("entranceSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1001:status", "SMTR16114I1001:stop", this, 0.1);
	entranceSlitGap_->setDescription("Entrance Slit Gap");
	sgmPVName = amNames2pvNames_.valueF("M4");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName, this);
	m4_->setDescription("M4 Mirror");
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4inboard->setDescription("M4 Inboard");
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4outboard->setDescription("M4 Outboard");
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName, "", this, 0.1);
	m4downstream->setDescription("M4 Downstream");
	m4_->addChildControl(m4inboard);
	m4_->addChildControl(m4outboard);
	m4_->addChildControl(m4downstream);

	sgmPVName = amNames2pvNames_.valueF("grating");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, "SMTR16114I1016:state", "SMTR16114I1016:emergStop", this, 0.1, 2.0, new AMControlStatusCheckerStopped(0));
	((AMPVwStatusControl*)(grating_))->setMoveStartTolerance(0.1);
	grating_->setDescription("Grating Selection");
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, "UND1411-01:moveStatus", "", this, 0.1);
	((AMPVwStatusControl*)(harmonic_))->setMoveStartTolerance(0.1);
	harmonic_->setDescription("Harmonic");

	sgmPVName = amNames2pvNames_.valueF("undulatorMotor");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	undulatorStep_ = new AMPVControl("undulatorStep", sgmPVName+":step:sp", sgmPVName+":step", QString(), this, 20, 20 );
	undulatorRelativeStepStorage_ = new AMPVControl("undulatorRelativeStepStorage", "BL1611-ID-1:AddOns:UndulatorRelativeStorage", "BL1611-ID-1:AddOns:UndulatorRelativeStorage", QString(), this, 1);
	undulatorVelocity_ = new AMPVControl("undulatorVelocity", sgmPVName+":velo:sp", sgmPVName+":velo", QString(), this, 1);
	undulatorFastTracking_ = new AMPVControl("undulatorFastTracking", "BL1611-ID-1:AddOns:UndulatorTrigger", "BL1611-ID-1:AddOns:UndulatorTrigger", QString(), this, 0.5);

	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, "", this, 0.1);
	undulatorTracking_->setDescription("Undulator Tracking");
	undulatorTracking_->setContextKnownDescription("Undulator");
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, "", this, 0.1, 10);
	monoTracking_->setDescription("Mono Tracking");
	monoTracking_->setContextKnownDescription("Mono");
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, "", this, 0.1);
	exitSlitTracking_->setDescription("Exit Slit Tracking");
	exitSlitTracking_->setContextKnownDescription("Exit Slit");

	sgmPVName = amNames2pvNames_.valueF("tfyHV");
	tfyHV_ = new AMPVControl("tfyHV", sgmPVName+":vmon", sgmPVName+":v0set", QString(), this, 5.0);
	tfyHV_->setDescription("TFY High Voltage");
	tfyHV_->setContextKnownDescription("Voltage");

	tfyHVToggle_ = new AMPVControl("tfyHVToggle", sgmPVName+":status", sgmPVName+":pwonoff", QString(), this, 0.1);
	tfyHVToggle_->setDescription("TFY High Voltage Toggle");
	tfyHVToggle_->setContextKnownDescription("Toggle");

	hvChannel106_ = new CLSCAEN2527HVChannel("Ch 6+", "PS1611401:106", AMHighVoltageChannel::positive, this);
	hvChannel109_ = new CLSCAEN2527HVChannel("Ch 9-", "PS1611401:109", AMHighVoltageChannel::negative, this);
	hvChannelPGT_ = new CLSPGT8000HVChannel("SGM PGT", "MCA1611-01", this);
	synchronizedDwellTime_ = new CLSSynchronizedDwellTime("BL1611-ID-1:dwell", this);
	synchronizedDwellTime_->addElement(0);
	synchronizedDwellTime_->addElement(1);
	synchronizedDwellTime_->addElement(2);
	synchronizedDwellTime_->addElement(3);
	synchronizedDwellTime_->addElement(4);

	sgmPVName = amNames2pvNames_.valueF("pgtHV");
	pgtHV_ = new AMPVControl("pgtHV", sgmPVName+"Actual:fbk", sgmPVName, QString(), this, 0.5);
	pgtHV_->setDescription("SDD High Voltage");
	pgtHV_->setContextKnownDescription("Voltage");

	sgmPVName = amNames2pvNames_.valueF("loadlockCCG");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	loadlockCCG_ = new AMReadOnlyPVControl("loadlockCCG", sgmPVName, this);
	loadlockCCG_->setDescription("SSA Loadlock CCG Pressure");
	loadlockCCG_->setContextKnownDescription("Loadlock CCG");
	sgmPVName = amNames2pvNames_.valueF("loadlockTCG");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	loadlockTCG_ = new AMReadOnlyPVControl("loadlockTCG", sgmPVName, this);
	loadlockTCG_->setDescription("SSA Loadlock TCG Pressure");
	loadlockTCG_->setContextKnownDescription("Loadlock TCG");

	ssaManipulatorX_ = new SGMMAXvMotor("ssaManipulatorX", amNames2pvNames_.valueF("ssaManipulatorX"), "SSA Inboard/Outboard", true, 0.2, 2.0, this);
	ssaManipulatorX_->setContextKnownDescription("X");

	ssaManipulatorY_ = new SGMMAXvMotor("ssaManipulatorY", amNames2pvNames_.valueF("ssaManipulatorY"), "SSA Upstream/Downstream", true, 0.2, 2.0, this);
	ssaManipulatorY_->setContextKnownDescription("Y");

	ssaManipulatorZ_ = new SGMMAXvMotor("ssaManipulatorZ", amNames2pvNames_.valueF("ssaManipulatorZ"), "SSA Up/Down", true, 0.2, 2.0, this);
	ssaManipulatorZ_->setContextKnownDescription("Z");

	ssaManipulatorRot_ = new SGMMAXvMotor("ssaManipulatorRot", amNames2pvNames_.valueF("ssaManipulatorRot"), "SSA Rotation", false, 0.2, 2.0, this);
	ssaManipulatorRot_->setContextKnownDescription("R");


	sgmPVName = amNames2pvNames_.valueF("beamlineScanning");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	beamlineScanning_ = new AMPVControl("beamlineScanning", sgmPVName, sgmPVName, "", this, 0.1);
	beamlineScanning_->setDescription("Beamline Scanning");
	sgmPVName = amNames2pvNames_.valueF("beamlineReady");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	beamlineReady_ = new AMReadOnlyPVControl("beamlineReady", sgmPVName, this);
	beamlineReady_->setDescription("Beamline Status");
	sgmPVName = amNames2pvNames_.valueF("nextDwellTimeTrigger");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	nextDwellTimeTrigger_ = new AMPVControl("nextDwellTimeTrigger", sgmPVName, sgmPVName, "", this, 0.1 );
	nextDwellTimeTrigger_->setDescription("Next Dwell Time Trigger");
	sgmPVName = amNames2pvNames_.valueF("nextDwellTimeConfirmed");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	nextDwellTimeConfirmed_ = new AMPVControl("nextDwellTimeConfirmed", sgmPVName, sgmPVName, "", this, 0.1 );
	nextDwellTimeConfirmed_->setDescription("Next Dwell Time Confirmed");
	sgmPVName = amNames2pvNames_.valueF("picoammeterDwellTime");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	picoammeterDwellTime_ = new AMPVControl("picoammeterDwellTime", sgmPVName, sgmPVName, "", this, 0.1 );
	picoammeterDwellTime_->setDescription("Picoammeter Dwell Time");

	sgmPVName = amNames2pvNames_.valueF("energyMovingStatus");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	energyMovingStatus_ = new AMReadOnlyPVControl("energyMovingStatus", sgmPVName, this);
	energyMovingStatus_->setDescription("Energy Status");
	sgmPVName = amNames2pvNames_.valueF("fastShutterVoltage");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	fastShutterVoltage_ = new AMPVControl("fastShutterVoltage", sgmPVName, sgmPVName, "", this, 0.1);
	fastShutterVoltage_->setDescription("Fast Shutter Voltage");
	sgmPVName = amNames2pvNames_.valueF("gratingVelocity");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	gratingVelocity_ = new AMPVControl("gratingVelocity", sgmPVName, sgmPVName, "", this, 1);
	gratingVelocity_->setDescription("Grating Motor Velocity");
	sgmPVName = amNames2pvNames_.valueF("gratingBaseVelocity");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	gratingBaseVelocity_ = new AMPVControl("gratingBaseVelocity", sgmPVName, sgmPVName, "", this, 1);
	gratingBaseVelocity_->setDescription("Grating Motor Base Velocity");
	sgmPVName = amNames2pvNames_.valueF("gratingAcceleration");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	gratingAcceleration_ = new AMPVControl("gratingAcceleration", sgmPVName, sgmPVName, "", this, 1);
	gratingAcceleration_->setDescription("Grating Motor Acceleration");
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum1");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ea1CloseVacuum1_ = new AMPVControl("ea1CloseVacuum1", sgmPVName, sgmPVName, "", this);
	ea1CloseVacuum1_->setDescription("XPS Upstream Close Vacuum Valve");
	sgmPVName = amNames2pvNames_.valueF("ea1CloseVacuum2");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ea1CloseVacuum2_ = new AMPVControl("ea1CloseVacuum2", sgmPVName, sgmPVName, "", this);
	ea1CloseVacuum2_->setDescription("XPS Downstream Close Vacuum Valve");
	sgmPVName = amNames2pvNames_.valueF("ea2CloseVacuum");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ea2CloseVacuum_ = new AMPVControl("ea2CloseVacuum", sgmPVName, sgmPVName, "", this);
	ea2CloseVacuum_->setDescription("SSA Close Vacuum Valve");
	sgmPVName = amNames2pvNames_.valueF("beamOn");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	beamOn_ = new AMPVControl("beamOn", sgmPVName, sgmPVName, "", this, 0.5);
	beamOn_->setDescription("Beam On");
	sgmPVName = amNames2pvNames_.valueF("visibleLightToggle");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	visibleLightToggle_ = new AMPVControl("visibleLightToggle", sgmPVName, sgmPVName, "", this);
	visibleLightToggle_->setDescription("Visible Light On/Off");
	sgmPVName = amNames2pvNames_.valueF("visibleLightStatus");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	visibleLightStatus_ = new AMReadOnlyPVControl("visibleLightStatus", sgmPVName, this);
	visibleLightStatus_->setDescription("Visible Light Status");
	sgmPVName = amNames2pvNames_.valueF("activeEndstation");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	activeEndstation_ = new AMPVControl("activeEndstation", sgmPVName, sgmPVName, "", this);
	activeEndstation_->setDescription("Endstation Selection");

	sgmPVName = amNames2pvNames_.valueF("scalerIntegrationTime");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	scalerIntegrationTime_ = new AMPVControl("scalerIntegrationTime", sgmPVName, sgmPVName, "", this, 0.1);
	scalerIntegrationTime_->setDescription("Scaler Integration Time");
	scalerIntegrationTime_->setContextKnownDescription("Integration Time");

	sgmPVName = amNames2pvNames_.valueF("ssaIllumination");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	ssaIllumination_ = new AMPVControl("ssaIllumination", sgmPVName, sgmPVName, "", this, 0.5);
	ssaIllumination_->setDescription("SSA Illumination");

	sgmPVName = amNames2pvNames_.valueF("mirrorStripeSelection");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	mirrorStripeSelection_ = new AMReadOnlyPVControl("mirrorStripeSelection", sgmPVName%":fbk", this);
	mirrorStripeSelection_->setDescription("Mirror Stripe");

	mirrorStripeSelectCarbon_ = new AMPVControl("mirrorStripeSelectCarbon", sgmPVName%":c_fanout.PROC", sgmPVName%":c_fanout.PROC", QString(), this, 0.5);
	mirrorStripeSelectCarbon_->setDescription("Select Carbon Stripe");
	mirrorStripeSelectSilicon_ = new AMPVControl("mirrorStripeSelectSilicon", sgmPVName%":si_fanout.PROC", sgmPVName%":si_fanout.PROC", QString(), this, 0.5);
	mirrorStripeSelectSilicon_->setDescription("Select Silicon Stripe");

	sgmPVName = amNames2pvNames_.valueF("undulator");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	undulatorOffset_ = new AMPVControl("undulatorOffset", sgmPVName%":corr_A", sgmPVName%":corr_A", QString(), this, 0.01);
	undulatorOffset_->setDescription("Undulator Offset");

	sgmPVName = amNames2pvNames_.valueF("masterDwell");
	if(sgmPVName.isEmpty())
		pvNameLookUpFail = true;
	masterDwell_ = new AMPVControl("masterDwell", sgmPVName, sgmPVName, QString(), this, 0.01);
	masterDwell_->setDescription("Master Dwell Time");

	scaler_ = new CLSSIS3820Scaler("BL1611-ID-1:mcs", this);
	scaler_->channelAt(0)->setCustomChannelName("TEY");
	scaler_->channelAt(1)->setCustomChannelName("I0");
	scaler_->channelAt(2)->setCustomChannelName("TFY PD ");
	scaler_->channelAt(3)->setCustomChannelName("PD");
	scaler_->channelAt(4)->setCustomChannelName("UP");
	scaler_->channelAt(5)->setCustomChannelName("DOWN");
	scaler_->channelAt(6)->setCustomChannelName("FPD1");
	scaler_->channelAt(7)->setCustomChannelName("FPD2");
	scaler_->channelAt(8)->setCustomChannelName("FPD3");
	scaler_->channelAt(9)->setCustomChannelName("FPD4");

	if(pvNameLookUpFail)
		AMErrorMon::alert(this, SGMBEAMLINE_PV_NAME_LOOKUPS_FAILED, "PV Name lookups in the SGM Beamline failed");
}

SGMBeamline::SGMBeamline() : AMBeamline("SGMBeamline") {
	//infoObject_ = new SGMBeamlineInfo(this);
	infoObject_ = SGMBeamlineInfo::sgmInfo();

	beamlineIsInitialized_ = false;
	usingSGMBeamline();

	beamlineWarnings_ = "";
	connect(this, SIGNAL(criticalConnectionsChanged()), this, SLOT(recomputeWarnings()));
	connect(energy(), SIGNAL(valueChanged(double)), this, SLOT(onEnergyValueChanged()));

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
	addChildControl(tfyHV_);
	addChildControl(pgtHV_);
	addChildControl(ssaManipulatorX_);
	addChildControl(ssaManipulatorY_);
	addChildControl(ssaManipulatorZ_);
	addChildControl(ssaManipulatorRot_);
	addChildControl(beamlineScanning_);
	connect(beamlineScanning_, SIGNAL(valueChanged(double)), this, SLOT(onBeamlineScanningValueChanged(double)));

	addChildControl(beamlineReady_);
	addChildControl(nextDwellTimeTrigger_);
	addChildControl(nextDwellTimeConfirmed_);
	addChildControl(picoammeterDwellTime_);
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
	connect(activeEndstation_, SIGNAL(valueChanged(double)), this, SLOT(onActiveEndstationChanged(double)));
	addChildControl(ssaIllumination_);
	connect(mirrorStripeSelection_, SIGNAL(valueChanged(double)), this, SLOT(onMirrorStripeChanged(double)));

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

	connect(criticalControlsSet_, SIGNAL(connected(bool)), this, SLOT(onCriticalsConnectedChanged()));

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

	detectorMap_ = new QMultiMap<AMOldDetector*, QPair<AMOldDetectorSet*, bool> >();

	criticalDetectorsSet_ = new AMOldDetectorSet(this);
	criticalDetectorsSet_->setName("Critical Beamline Detectors");
	rawDetectorsSet_ = new AMOldDetectorSet(this);
	rawDetectorsSet_->setName("All possible detectors for SGM Beamline");
	connect(criticalDetectorsSet_, SIGNAL(connected(bool)), this, SLOT(onCriticalsConnectedChanged()));
	connect(criticalDetectorsSet_, SIGNAL(detectorAdded(int)), this, SLOT(recomputeWarnings()));
	connect(criticalDetectorsSet_, SIGNAL(detectorRemoved(int)), this, SLOT(recomputeWarnings()));

	allDetectors_ = new AMOldDetectorSet(this);
	allDetectors_->setName("All Detectors");

	feedbackDetectors_ = new AMOldDetectorSet(this);
	feedbackDetectors_->setName("Feedback Detectors");

	XASDetectors_ = new AMOldDetectorSet(this);
	XASDetectors_->setName("XAS Detectors");

	FastDetectors_ = new AMOldDetectorSet(this);
	FastDetectors_->setName("Fast Scan Detectors");

	teyScalerDetector_ = new AMSingleReadOnlyControlDetector("teyScaler", "BL1611-ID-1:mcs00:fbk", AMOldDetector::WaitRead, this);
	teyScalerDetector_->setDescription("TEY");
	detectorRegistry_.append(teyScalerDetector_);
	detectorMap_->insert(teyScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(teyScalerDetector_, qMakePair(XASDetectors(), true));
	detectorMap_->insert(teyScalerDetector_, qMakePair(FastDetectors(), true));
	criticalDetectorsSet_->addDetector(teyScalerDetector_);
	rawDetectorsSet_->addDetector(teyScalerDetector_);
	connect(teyScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));


	tfyScalerDetector_ = new AMSingleReadOnlyControlDetector("tfyScaler", "BL1611-ID-1:mcs02:fbk", AMOldDetector::WaitRead, this);
	tfyScalerDetector_->setDescription("TFY Diode");
	detectorRegistry_.append(tfyScalerDetector_);
	detectorMap_->insert(tfyScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(tfyScalerDetector_, qMakePair(XASDetectors(), true));
	detectorMap_->insert(tfyScalerDetector_, qMakePair(FastDetectors(), true));
	criticalDetectorsSet_->addDetector(tfyScalerDetector_);
	rawDetectorsSet_->addDetector(tfyScalerDetector_);
	connect(tfyScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));
	//connect(tfyHVToggle_, SIGNAL(valueChanged(double)), this, SIGNAL(detectorHVChanged()));

	pgtDetector_ = new CLSPGTDetector("pgt", "MCA1611-01", createHVPGTOnActions(), createHVPGTOffActions(), AMOldDetector::WaitRead, this);
	pgtDetector_->setDescription("SDD");
	detectorRegistry_.append(pgtDetector_);
	connect(pgtDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));
	detectorMap_->insert(pgtDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(pgtDetector_, qMakePair(XASDetectors(), false));
	criticalDetectorsSet_->addDetector(pgtDetector_);
	rawDetectorsSet_->addDetector(pgtDetector_);

	oos65000Detector_ = new CLSOceanOptics65000Detector("oos65000", "SA0000-03", AMOldDetector::WaitRead, this);
	oos65000Detector_->setDescription("OceanOptics 65000");
	detectorRegistry_.append(oos65000Detector_);
	connect(oos65000Detector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));
	detectorMap_->insert(oos65000Detector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(oos65000Detector_, qMakePair(XASDetectors(), false));
	rawDetectorsSet_->addDetector(oos65000Detector_);

	i0ScalerDetector_ = new AMSingleReadOnlyControlDetector("I0Scaler", "BL1611-ID-1:mcs01:fbk", AMOldDetector::WaitRead, this);
	i0ScalerDetector_->setDescription("I0");
	detectorRegistry_.append(i0ScalerDetector_);
	detectorMap_->insert(i0ScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(i0ScalerDetector_, qMakePair(feedbackDetectors(), false));
	criticalDetectorsSet_->addDetector(i0ScalerDetector_);
	rawDetectorsSet_->addDetector(i0ScalerDetector_);
	connect(i0ScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	eVFbkDetector_ = new AMSingleReadOnlyControlDetector("eVFbk", "BL1611-ID-1:Energy:fbk", AMOldDetector::ImmediateRead, this);
	eVFbkDetector_->setDescription("Energy Feedback");
	detectorRegistry_.append(eVFbkDetector_);
	detectorMap_->insert(eVFbkDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(eVFbkDetector_, qMakePair(feedbackDetectors(), false));
	criticalDetectorsSet_->addDetector(eVFbkDetector_);
	rawDetectorsSet_->addDetector(eVFbkDetector_);
	connect(eVFbkDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	photodiodeScalerDetector_ = new AMSingleReadOnlyControlDetector("photodiodeScaler", "BL1611-ID-1:mcs03:fbk", AMOldDetector::WaitRead, this);
	photodiodeScalerDetector_->setDescription("Photodiode");
	detectorRegistry_.append(photodiodeScalerDetector_);
	detectorMap_->insert(photodiodeScalerDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(photodiodeScalerDetector_, qMakePair(feedbackDetectors(), false));
	criticalDetectorsSet_->addDetector(photodiodeScalerDetector_);
	rawDetectorsSet_->addDetector(photodiodeScalerDetector_);
	connect(photodiodeScalerDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	encoderUpDetector_ = new AMSingleReadOnlyControlDetector("encoderUp", "BL1611-ID-1:mcs04:fbk", AMOldDetector::WaitRead, this);
	encoderUpDetector_->setDescription("Encoder Up Counts");
	detectorRegistry_.append(encoderUpDetector_);
	detectorMap_->insert(encoderUpDetector_, qMakePair(allDetectors(), false));
	rawDetectorsSet_->addDetector(encoderUpDetector_);
	connect(encoderUpDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	encoderDownDetector_ = new AMSingleReadOnlyControlDetector("encoderDown", "BL1611-ID-1:mcs04:fbk", AMOldDetector::WaitRead, this);
	encoderDownDetector_->setDescription("Encoder Down Counts");
	detectorRegistry_.append(encoderDownDetector_);
	rawDetectorsSet_->addDetector(encoderDownDetector_);
	detectorMap_->insert(encoderDownDetector_, qMakePair(allDetectors(), false));
	connect(encoderDownDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	ringCurrentDetector_ = new AMSingleReadOnlyControlDetector("ringCurrent", "PCT1402-01:mA:fbk", AMOldDetector::ImmediateRead, this);
	ringCurrentDetector_->setDescription("Ring Current");
	detectorRegistry_.append(ringCurrentDetector_);
	rawDetectorsSet_->addDetector(ringCurrentDetector_);
	detectorMap_->insert(ringCurrentDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(ringCurrentDetector_, qMakePair(feedbackDetectors(), false));
	connect(ringCurrentDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD1ScalarDetector_ = new AMSingleReadOnlyControlDetector("filterPD1Current", "BL1611-ID-1:mcs06:fbk", AMOldDetector::ImmediateRead, this);
	filterPD1ScalarDetector_->setDescription("Aux 1 Diode");
	detectorRegistry_.append(filterPD1ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD1ScalarDetector_);
	detectorMap_->insert(filterPD1ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD1ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD1ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD2ScalarDetector_ = new AMSingleReadOnlyControlDetector("filterPD2Current", "BL1611-ID-1:mcs07:fbk", AMOldDetector::ImmediateRead, this);
	filterPD2ScalarDetector_->setDescription("Aux 2 Diode");
	detectorRegistry_.append(filterPD2ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD2ScalarDetector_);
	detectorMap_->insert(filterPD2ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD2ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD2ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD3ScalarDetector_ = new AMSingleReadOnlyControlDetector("filterPD3Current", "BL1611-ID-1:mcs08:fbk", AMOldDetector::ImmediateRead, this);
	filterPD3ScalarDetector_->setDescription("Aux 3 Diode");
	detectorRegistry_.append(filterPD3ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD3ScalarDetector_);
	detectorMap_->insert(filterPD3ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD3ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD3ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	filterPD4ScalarDetector_ = new AMSingleReadOnlyControlDetector("filterPD4Current", "BL1611-ID-1:mcs09:fbk", AMOldDetector::ImmediateRead, this);
	filterPD4ScalarDetector_->setDescription("Aux 4 Diode");
	detectorRegistry_.append(filterPD4ScalarDetector_);
	rawDetectorsSet_->addDetector(filterPD4ScalarDetector_);
	detectorMap_->insert(filterPD4ScalarDetector_, qMakePair(allDetectors(), false));
	detectorMap_->insert(filterPD4ScalarDetector_, qMakePair(XASDetectors(), false));
	connect(filterPD4ScalarDetector_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	amptekSDD1_ = new CLSAmptekSDD123Detector("AmptekSDD1", "amptek:sdd1", AMOldDetector::WaitRead, this);
	detectorRegistry_.append(amptekSDD1_);
	rawDetectorsSet_->addDetector(amptekSDD1_);
	detectorMap_->insert(amptekSDD1_, qMakePair(allDetectors(), false));
	detectorMap_->insert(amptekSDD1_, qMakePair(XASDetectors(), false));
	connect(amptekSDD1_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	amptekSDD2_ = new CLSAmptekSDD123Detector("AmptekSDD2", "amptek:sdd2", AMOldDetector::WaitRead, this);
	detectorRegistry_.append(amptekSDD2_);
	rawDetectorsSet_->addDetector(amptekSDD2_);
	detectorMap_->insert(amptekSDD2_, qMakePair(allDetectors(), false));
	detectorMap_->insert(amptekSDD2_, qMakePair(XASDetectors(), false));
	connect(amptekSDD2_->signalSource(), SIGNAL(availabilityChagned(AMOldDetector*,bool)), this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)));

	newAmptekSDD1_ = new CLSAmptekSDD123DetectorNew("NEWAmptekSDD1", "Amptek SDD 1", "amptek:sdd1", this);
	newAmptekSDD2_ = new CLSAmptekSDD123DetectorNew("NEWAmptekSDD2", "Amptek SDD 2", "amptek:sdd2", this);
	newTEYDetector_ = new CLSBasicScalerChannelDetector("NEWTEY", "TEY", scaler_, 0, this);
	newTFYDetector_ = new CLSBasicScalerChannelDetector("NEWTFY", "TFY", scaler_, 2, this);
	newI0Detector_ = new CLSBasicScalerChannelDetector("NEWI0", "I0", scaler_, 1, this);
	newPDDetector_ = new CLSBasicScalerChannelDetector("NEWPD", "PD", scaler_, 3, this);
	newDetectorSet_ = new AMDetectorGroup("New Detectors", this);
	newDetectorSet_->addDetector(newAmptekSDD1_);
	newDetectorSet_->addDetector(newAmptekSDD2_);
	newDetectorSet_->addDetector(newTEYDetector_);
	newDetectorSet_->addDetector(newTFYDetector_);
	newDetectorSet_->addDetector(newI0Detector_);
	newDetectorSet_->addDetector(newPDDetector_);

	unrespondedDetectors_ = detectorRegistry_;
	QTimer::singleShot(10000, this, SLOT(ensureDetectorTimeout()));

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
	ssaManipulatorSampleTolerances_ << 1.0 << 1.0 << 1.0 << 15.0;
	unconnectedSets_.append(ssaManipulatorSet_);
	connect(ssaManipulatorSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	sampleManipulator_ = new AMControlSetSampleManipulator(ssaManipulatorSet_);

	connect(this, SIGNAL(detectorAvailabilityChanged(AMOldDetector*,bool)), this, SLOT(onDetectorAvailabilityChanged(AMOldDetector*,bool)));

	currentSamplePlate_ = 0;//NULL

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

	setupExposedControls();
	setupExposedDetectors();
}

SGMBeamline::~SGMBeamline()
{
}

bool SGMBeamline::isConnected() const{
	return criticalControlsSet_->isConnected() && criticalDetectorsSet_->isConnected();
}

bool SGMBeamline::isReady() const{
	if(isConnected())
		if( (energy()->value() > 150) && (energy()->value() < 2100) )
			return true;
	return false;
}

SGMBeamlineInfo::sgmGrating SGMBeamline::currentGrating() const{
	return (SGMBeamlineInfo::sgmGrating)grating()->value();
}

QStringList SGMBeamline::unconnectedCriticals() const{
	QStringList allUnconnected;
	allUnconnected.append(criticalControlsSet_->unconnected());
	allUnconnected.append(criticalDetectorsSet_->unconnected());
	return allUnconnected;
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

QString SGMBeamline::currentEndstation() const{
	if(activeEndstation_->value() == 0)
		return infoObject_->sgmEndstationName(SGMBeamlineInfo::scienta);
	else if(activeEndstation_->value() == 1)
		return infoObject_->sgmEndstationName(SGMBeamlineInfo::ssa);
	else
		return infoObject_->sgmEndstationName((SGMBeamlineInfo::sgmEndstation)272727);
}

bool SGMBeamline::isSDD1Enabled() const{
	if(amptekSDD1_->isConnected()){
		CLSAmptekSDD123Detector *sddAsAmptek = (CLSAmptekSDD123Detector*)(amptekSDD1_);
		return sddAsAmptek->isEnabled();
	}
	return false;
}

AMBeamlineActionItem* SGMBeamline::createSDD1EnableAction(bool setEnabled){
	if(amptekSDD1_->isConnected()){
		CLSAmptekSDD123Detector *sddAsAmptek = (CLSAmptekSDD123Detector*)(amptekSDD1_);
		return sddAsAmptek->createEnableAction(setEnabled);
	}
	return 0;
}

bool SGMBeamline::isSDD2Enabled() const{
	if(amptekSDD2_->isConnected()){
		CLSAmptekSDD123Detector *sddAsAmptek = (CLSAmptekSDD123Detector*)(amptekSDD2_);
		return sddAsAmptek->isEnabled();
	}
	return false;
}

AMBeamlineActionItem* SGMBeamline::createSDD2EnableAction(bool setEnabled){
	if(amptekSDD2_->isConnected()){
		CLSAmptekSDD123Detector *sddAsAmptek = (CLSAmptekSDD123Detector*)(amptekSDD2_);
		return sddAsAmptek->createEnableAction(setEnabled);
	}
	return 0;
}

int SGMBeamline::synchronizedDwellTimeDetectorIndex(AMOldDetector *detector) const{
	if(detector == pgtDetector_)
		return 2;
	else if(detector == oos65000Detector_)
		return 3;
	else if((detector == amptekSDD1_) || (detector == amptekSDD2_) )
		return 4;
	else
		return -1;
}

int SGMBeamline::currentSamplePlateId() const{
	if(currentSamplePlate_)
		return currentSamplePlate()->id();
	return -1;
}

int SGMBeamline::currentSampleId(){
	if(currentSamplePlate_)
		return currentSamplePlate_->sampleIdAtPosition(currentSamplePositioner()->toInfoList(), ssaManipulatorSampleTolerances_);
	return -1;
}

QString SGMBeamline::currentSampleDescription(){
	int currentId = currentSampleId();
	if(currentId == -1)
		return "<Unknown Sample>";
	else
		return AMSample(currentId, AMUser::user()->database()).name();
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
	beamOnActionsList->appendAction(0, beamOnAction1);
	beamOnActionsList->appendAction(0, beamOnAction2);
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
	gotoTransferPositionAction2->setSetpoint(-6.58);
	AMBeamlineControlMoveAction *gotoTransferPositionAction3 = new AMBeamlineControlMoveAction(ssaManipulatorZ());
	gotoTransferPositionAction3->setSetpoint(-77.0);
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

AMBeamlineListAction* SGMBeamline::createTransferActions(SGMBeamlineInfo::sgmTransferType transferType){
	switch(transferType){
	case SGMBeamlineInfo::loadlockOut :
		return createTransferLoadLockOutActions();
	case SGMBeamlineInfo::loadlockIn :
		return createTransferLoadLockInActions();
	case SGMBeamlineInfo::ChamberOut :
		return createTransferChamberOutActions();
	case SGMBeamlineInfo::ChamberIn :
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

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV106OnActions(){
	AMBeamlineHighVoltageChannelToggleAction *onAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel106());
	onAction->setSetpoint(AMHighVoltageChannel::isPowerOn);
	return onAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV106OffActions(){
	AMBeamlineHighVoltageChannelToggleAction *offAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel106());
	offAction->setSetpoint(AMHighVoltageChannel::isPowerOff);
	return offAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV109OnActions(){
	AMBeamlineHighVoltageChannelToggleAction *onAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel109());
	onAction->setSetpoint(AMHighVoltageChannel::isPowerOn);
	return onAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHV109OffActions(){
	AMBeamlineHighVoltageChannelToggleAction *offAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannel109());
	offAction->setSetpoint(AMHighVoltageChannel::isPowerOff);
	return offAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHVPGTOnActions(){
	AMBeamlineHighVoltageChannelToggleAction *onAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannelPGT());
	onAction->setSetpoint(AMHighVoltageChannel::isPowerOn);
	return onAction;
}

AMBeamlineHighVoltageChannelToggleAction* SGMBeamline::createHVPGTOffActions(){
	AMBeamlineHighVoltageChannelToggleAction *offAction = new AMBeamlineHighVoltageChannelToggleAction(hvChannelPGT());
	offAction->setSetpoint(AMHighVoltageChannel::isPowerOff);
	return offAction;
}

CLSSIS3820Scaler* SGMBeamline::scaler(){
	if(scaler_->isConnected())
		return scaler_;
	return 0; //NULL
}

CLSSIS3820Scaler* SGMBeamline::rawScaler(){
	return scaler_;
}

bool SGMBeamline::isBeamlineScanning(){
	if( fabs(beamlineScanning_->value() -1.0) < beamlineScanning_->tolerance() )
		return true;
	return false;
}

bool SGMBeamline::isVisibleLightOn() const{
	if(visibleLightToggle_->value() == 1)
		return true;
	return false;
}

bool SGMBeamline::energyValidForSettings(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic, double energy){
	if( (grating == 0) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (energy > 240) && (energy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (energy > 440) && (energy < 1200) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (energy > 800) && (energy < 1150) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::thirdHarmonic) && (energy > 1050) && (energy < 2000) )
		return true;
	else
		return false;
}

bool SGMBeamline::energyRangeValidForSettings(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic, double minEnergy, double maxEnergy){
	if( (grating == 0) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (minEnergy > 240) && (maxEnergy > 240) && (minEnergy < 750) && (maxEnergy < 750) )
		return true;
	else if( (grating == 1) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (minEnergy > 440) && (maxEnergy > 440) && (minEnergy < 1200) && (maxEnergy < 1200) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::firstHarmonic) && (minEnergy > 800) && (maxEnergy > 800) && (minEnergy < 1150) && (maxEnergy < 1150) )
		return true;
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::thirdHarmonic) && (minEnergy > 1050) && (maxEnergy > 1050) && (minEnergy < 2000) && (maxEnergy < 2000) )
		return true;
	else
		return false;
}

QList< QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> > SGMBeamline::gratingHarmonicForEnergyRange(double minEnergy, double maxEnergy){
	QList< QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> > rVal;
	if( (maxEnergy > 240) && (maxEnergy < 750) && (minEnergy > 240) && (minEnergy < 750) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)0, SGMBeamlineInfo::firstHarmonic ));
	if((maxEnergy > 440) && (maxEnergy < 1200) && (minEnergy > 440) && (minEnergy < 1200) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)1, SGMBeamlineInfo::firstHarmonic ));
	if( (maxEnergy > 800) && (maxEnergy < 1100) && (minEnergy > 800) && (minEnergy < 1100) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)2, SGMBeamlineInfo::firstHarmonic ));
	if( (maxEnergy > 1100) && (maxEnergy < 2000) && (minEnergy > 1100) && (minEnergy < 2000) )
		rVal.append(QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>((SGMBeamlineInfo::sgmGrating)2, SGMBeamlineInfo::thirdHarmonic ));
	return rVal;
}

QPair<double, double> SGMBeamline::energyRangeForGratingHarmonic(SGMBeamlineInfo::sgmGrating grating, SGMBeamlineInfo::sgmHarmonic harmonic){
	QPair<double, double> rVal;
	if( (grating == 0) && (harmonic == SGMBeamlineInfo::firstHarmonic) )
		rVal = QPair<double, double>(240, 750);
	else if( (grating == 1) && (harmonic == SGMBeamlineInfo::firstHarmonic) )
		rVal = QPair<double, double>(440, 1200);
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::firstHarmonic) )
		rVal = QPair<double, double>(800, 1100);
	else if( (grating == 2) && (harmonic == SGMBeamlineInfo::thirdHarmonic) )
		rVal = QPair<double, double>(1100, 2000);
	return rVal;
}

QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> SGMBeamline::forBestFlux(double minEnergy, double maxEnergy) const{
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
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::lowGrating, SGMBeamlineInfo::firstHarmonic);
	else if(testEnergy < 1200)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::mediumGrating, SGMBeamlineInfo::firstHarmonic);
	else
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::highGrating, SGMBeamlineInfo::thirdHarmonic);
}

QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic> SGMBeamline::forBestResolution(double minEnergy, double maxEnergy) const{
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
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::lowGrating, SGMBeamlineInfo::firstHarmonic);
	else if(testEnergy < 800)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::mediumGrating, SGMBeamlineInfo::firstHarmonic);
	else if(testEnergy < 1100)
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::highGrating, SGMBeamlineInfo::firstHarmonic);
	else
		return QPair<SGMBeamlineInfo::sgmGrating, SGMBeamlineInfo::sgmHarmonic>(SGMBeamlineInfo::highGrating, SGMBeamlineInfo::thirdHarmonic);
}

QList<AMOldDetector*> SGMBeamline::possibleDetectorsForSet(AMOldDetectorSet *set){
	QPair<AMOldDetectorSet*, bool> tempDetectorSet;
	QList<AMOldDetector*> retVal;
	QMultiMap<AMOldDetector*, QPair<AMOldDetectorSet*, bool> >::iterator i = detectorMap_->begin();
	while(i != detectorMap_->end()){
		tempDetectorSet = i.value();
		if(tempDetectorSet.first == set)
			retVal.append(i.key());
		++i;
	}
	return retVal;
}

void SGMBeamline::setCurrentSamplePlate(AMSamplePlate *newSamplePlate){
	if(currentSamplePlate_ != newSamplePlate){
		currentSamplePlate_ = newSamplePlate;
		emit currentSamplePlateChanged(currentSamplePlate_);
	}
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

void SGMBeamline::setCurrentEndstation(SGMBeamlineInfo::sgmEndstation endstation){
	if(endstation == SGMBeamlineInfo::scienta)
		activeEndstation_->move(0);
	else if(endstation == SGMBeamlineInfo::ssa)
		activeEndstation_->move(1);
	return;
}

void SGMBeamline::setCurrentMirrorStripe(SGMBeamlineInfo::sgmMirrorStripe mirrorStripe){
	if(mirrorStripe == SGMBeamlineInfo::carbonStripe)
		mirrorStripeSelectCarbon_->move(1);
	else if(mirrorStripe == SGMBeamlineInfo::siliconStripe)
		mirrorStripeSelectSilicon_->move(1);
	return;
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
		if(ctrlSet->name() == "SSA Manipulator"){
			AMControlInfoList ssaInfoList = ssaManipulatorSet_->toInfoList();
			ssaInfoList[0].setValue(-1.0);
			ssaInfoList[1].setValue( 0.0);
			ssaInfoList[2].setValue(-1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 0.0);
			ssaInfoList[2].setValue(-1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 1.0);
			ssaInfoList[2].setValue(-1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue(-1.0);
			ssaInfoList[2].setValue( 0.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 0.0);
			ssaInfoList[2].setValue( 0.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 1.0);
			ssaInfoList[2].setValue( 0.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue(-1.0);
			ssaInfoList[2].setValue( 1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 0.0);
			ssaInfoList[2].setValue( 1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
			ssaInfoList[0].setValue( 1.0);
			ssaInfoList[2].setValue( 1.0);
			ssaFiducializations_.append(AMControlInfoList(ssaInfoList));
		}
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
	Q_UNUSED(isConnected)
	Q_UNUSED(control)
	emit criticalControlsConnectionsChanged();
}

void SGMBeamline::onCriticalsConnectedChanged(){
	//qdebug() << "Critical controls are connected: " << criticalControlsSet_->isConnected();
	//qdebug() << "Critical detectors are connected: " << criticalDetectorsSet_->isConnected();
	emit criticalConnectionsChanged();
}

void SGMBeamline::onEnergyValueChanged(){
	if(isConnected()){
		bool wasOutsideGoodRange = false;
		bool isOutsideGoodRange = false;
		if( (lastEnergyValue_ < 150) || (lastEnergyValue_ > 2100) )
			wasOutsideGoodRange = true;
		if( (energy()->value() < 150) || (energy()->value() > 2100) )
			isOutsideGoodRange = true;

		if(wasOutsideGoodRange != isOutsideGoodRange)
			emit beamlineReadyChanged();

		lastEnergyValue_ = energy()->value();
	}
}

void SGMBeamline::onActiveEndstationChanged(double value){
	emit currentEndstationChanged((SGMBeamlineInfo::sgmEndstation)value);
}

void SGMBeamline::onMirrorStripeChanged(double value){
	emit currentMirrorStripeChanged((SGMBeamlineInfo::sgmMirrorStripe)value);
}

void SGMBeamline::recomputeWarnings(){
	if(!isConnected()){
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

void SGMBeamline::onDetectorAvailabilityChanged(AMOldDetector *detector, bool isAvailable){
	QList<QPair<AMOldDetectorSet*, bool> > detectorSets = detectorMap_->values(detector);
	if(isAvailable){

		bool hasDetector;
		for(int x = 0; x < detectorSets.count(); x++){
			hasDetector = false;
			for(int y = 0; y < detectorSets.at(x).first->count(); y++)
				if(detectorSets.at(x).first->detectorAt(y)->detectorName() == detector->detectorName())
					hasDetector = true;
			if(!hasDetector){
				//qdebug() << "Adding detector " << detector->detectorName() << " as default " << detectorSets.at(x).second;
				detectorSets.at(x).first->addDetector(detector, detectorSets.at(x).second);
			}
		}
	}
	else{
		for(int x = 0; x < detectorSets.count(); x++){
			for(int y = 0; y < detectorSets.at(x).first->count(); y++)
				if(detectorSets.at(x).first->detectorAt(y)->detectorName() == detector->detectorName())
					detectorSets.at(x).first->removeDetector(detector);
		}
	}

	if(unrespondedDetectors_.contains(detector))
		unrespondedDetectors_.removeAll(detector);

	if(unrespondedDetectors_.count() == 0 && !beamlineIsInitialized_)
		computeBeamlineInitialized();
}

void SGMBeamline::ensureDetectorTimeout(){
	if(unrespondedDetectors_.count() == 0)
		return;

	while(unrespondedDetectors_.count() > 0)
		onDetectorAvailabilityChanged(unrespondedDetectors_.last(), false);
}

void SGMBeamline::computeBeamlineInitialized(){
	if(!beamlineIsInitialized_){
		beamlineIsInitialized_ = true;
		emit beamlineInitialized();
	}
}

void SGMBeamline::setupExposedControls(){
	addExposedControl(ssaManipulatorX_);
	addExposedControl(ssaManipulatorY_);
	addExposedControl(ssaManipulatorZ_);
	addExposedControl(energy_);
	addExposedControl(masterDwell_);
}

void SGMBeamline::setupExposedDetectors(){
	addExposedDetector(newAmptekSDD1_);
	addExposedDetector(newAmptekSDD2_);
}

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0)
		instance_ = new SGMBeamline();

	return static_cast<SGMBeamline*>(instance_);

}
