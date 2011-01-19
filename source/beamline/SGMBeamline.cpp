#include "SGMBeamline.h"

#include "dataman/AMSamplePlate.h"

SGMBeamline* SGMBeamline::instance_ = 0;

void SGMBeamline::usingSGMBeamline(){
	amNames2pvNames_.set("energy", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("eV_Fbk", "BL1611-ID-1:Energy:fbk");
	amNames2pvNames_.set("eVFbk", "BL1611-ID-1:Energy:fbk");
	amNames2pvNames_.set("mono", "SMTR16114I1002");
	amNames2pvNames_.set("undulator", "UND1411-01");
	amNames2pvNames_.set("exitSlit", "PSL16114I1003");
	amNames2pvNames_.set("exitSlitGap", "PSL16114I1004");
	amNames2pvNames_.set("M4", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Inboard", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Outboard", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("M4Downstream", "BL1611-ID-1:Energy");
	amNames2pvNames_.set("grating", "SG16114I1001:choice");
	amNames2pvNames_.set("harmonic", "BL1611-ID-1:harmonic");
	amNames2pvNames_.set("undulatorTracking", "UND1411-01:Energy:track");
	amNames2pvNames_.set("monoTracking", "SG16114I1001:Energy:track");
	amNames2pvNames_.set("exitSlitTracking", "PSL16114I1003:Energy:track");
	amNames2pvNames_.set("tey", "A1611-4-15:A:fbk");
	amNames2pvNames_.set("tfy", "A1611-4-16:A:fbk");
	amNames2pvNames_.set("tfyHVSetpoint", "PS1611401:109:v0set");
	amNames2pvNames_.set("tfyHVFbk", "PS1611401:109:vmon");
#warning "Hey Dave, I think you need a different PV here for the pgt's spectrum"
	amNames2pvNames_.set("pgt", "MCA1611-01:ROI:0");
	amNames2pvNames_.set("pgtHVSetpoint", "MCA1611-01:Bias:Volt");
	amNames2pvNames_.set("pgtHVFbk", "MCA1611-01:Bias:VoltActual:fbk");
	amNames2pvNames_.set("pgtHVRamping", "MCA1611-01:Bias:Volt:fbk");
	amNames2pvNames_.set("pgtIntegrationTime", "MCA1611-01:Preset:Live");
	amNames2pvNames_.set("pgtIntegrationMode", "MCA1611-01:Preset:Live");
	amNames2pvNames_.set("I0", "A1611-4-14:A:fbk");
	amNames2pvNames_.set("loadlockCCG", "CCG1611-4-I10-09:vac:p");
	amNames2pvNames_.set("loadlockTCG", "TCGC1611-426:pressure:fbk");
	amNames2pvNames_.set("ssaManipulatorX", "BL1611-ID-1:EA1:x");
	amNames2pvNames_.set("ssaManipulatorY", "BL1611-ID-1:EA1:z");
	amNames2pvNames_.set("ssaManipulatorZ", "BL1611-ID-1:EA1:y");
	amNames2pvNames_.set("ssaManipulatorRot", "BL1611-ID-1:EA1:r");
	amNames2pvNames_.set("beamlineScanning", "BL1611-ID-1:scanning");
	amNames2pvNames_.set("beamlineReady", "BL1611-ID-1:beam:status");
	amNames2pvNames_.set("energyMovingStatus", "BL1611-ID-1:ready");
	amNames2pvNames_.set("fastShutterVoltage", "PSH161141001:V");
	amNames2pvNames_.set("scalarMode", "BL1611-ID-1:mcs:continuous");
	amNames2pvNames_.set("scalarStart", "BL1611-ID-1:mcs:startScan");
	amNames2pvNames_.set("gratingVelocity", "SMTR16114I1002:velo");
	amNames2pvNames_.set("gratingBaseVelocity", "SMTR16114I1002:veloBase");
	amNames2pvNames_.set("gratingAcceleration", "SMTR16114I1002:accel");

	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::toPV("ringCurrent"), this);
	addChildControl(ringCurrent_);
	QString sgmPVName = amNames2pvNames_.valueF("energy");
	energy_ = new AMPVwStatusControl("energy", sgmPVName+":fbk", sgmPVName, amNames2pvNames_.valueF("energyMovingStatus"), this, 0.01);
	sgmPVName = amNames2pvNames_.valueF("mono");
	AMReadOnlyPVwStatusControl *mono = new AMReadOnlyPVwStatusControl("mono", sgmPVName+":enc:fbk", sgmPVName+":moving", energy_);
	sgmPVName = amNames2pvNames_.valueF("undulator");
	AMReadOnlyPVwStatusControl *undulator = new AMReadOnlyPVwStatusControl("undulator", sgmPVName+":gap:mm:fbk", sgmPVName+":moveStatus", energy_);
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	AMReadOnlyPVwStatusControl *exitSlit = new AMReadOnlyPVwStatusControl("exitSlit", sgmPVName+":Y:mm:encsp", "SMTR16114I1003:moving", energy_);
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);

	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName+":Y:mm:fbk", sgmPVName+":Y:mm:encsp", "SMTR16114I1017:status", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4");
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");
	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName, this, 0.1);
	m4_->addChildControl(m4inboard);
	m4_->addChildControl(m4outboard);
	m4_->addChildControl(m4downstream);
	sgmPVName = amNames2pvNames_.valueF("grating");
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, "SMTR16114I1016:state", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, "UND1411-01:moveStatus", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, this, 0.1, 10);
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("tey");
	tey_ = new AMReadOnlyPVControl("tey", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfy");
	tfy_ = new AMReadOnlyPVControl("tfy", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfyHVSetpoint");
	tfyHVSetpoint_ = new AMPVControl("tfyHVSetpoint", amNames2pvNames_.valueF("tfyHVFbk"), amNames2pvNames_.valueF("tfyHVSetpoint"), this, 10.0);
	sgmPVName = amNames2pvNames_.valueF("tfyHVFbk");
	tfyHVFbk_ = new AMPVControl("tfyHVFbk", sgmPVName, sgmPVName, this, 10.0);

	sgmPVName = amNames2pvNames_.valueF("pgt");
	pgt_ = new AMReadOnlyPVControl("pgt", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtHVSetpoint");
	//    pgtHVSetpoint_ = new AMPVControl("pgtHVSetpoint", amNames2pvNames_.valueF("pgtHVFbk"), amNames2pvNames_.valueF("pgtHVSetpoint"), this, 0.5);
	pgtHVSetpoint_ = new AMPVControl("pgtHVSetpoint", amNames2pvNames_.valueF("pgtHVSetpoint"), amNames2pvNames_.valueF("pgtHVSetpoint"), this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("pgtHVFbk");
	pgtHVFbk_ = new AMReadOnlyPVControl("pgtHVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationTime");
	pgtIntegrationTime_ = new AMPVControl("pgtIntegrationTime", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationMode");
	pgtIntegrationMode_ = new AMPVControl("pgtIntegrationMode", sgmPVName, sgmPVName, this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("I0");
	i0_ = new AMReadOnlyPVControl("I0", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("eVFbk");
	eVFbk_ = new AMReadOnlyPVControl("eVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockCCG");
	loadlockCCG_ = new AMReadOnlyPVControl("loadlockCCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockTCG");
	loadlockTCG_ = new AMReadOnlyPVControl("loadlockTCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorX");
	ssaManipulatorX_ = new AMPVwStatusControl("ssaManipulatorX", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", this, 0.1);
	ssaManipulatorXStop_ = new AMPVControl("ssaManipulatorXStop", "SMTR16114I1012:emergStop", "SMTR16114I1012:emergStop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorY");
	ssaManipulatorY_ = new AMPVwStatusControl("ssaManipulatorY", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", this, 0.1);
	ssaManipulatorYStop_ = new AMPVControl("ssaManipulatorYStop", "SMTR16114I1013:emergStop", "SMTR16114I1013:emergStop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorZ");
	ssaManipulatorZ_ = new AMPVwStatusControl("ssaManipulatorZ", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", this, 0.1);
	ssaManipulatorZStop_ = new AMPVControl("ssaManipulatorZStop", "SMTR16114I1014:emergStop", "SMTR16114I1014:emergStop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorRot");
	ssaManipulatorRot_ = new AMPVwStatusControl("ssaManipulatorRot", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":sp", this, 0.1);
	ssaManipulatorRotStop_ = new AMPVControl("ssaManipulatorRotStop", "SMTR16114I1015:emergStop", "SMTR16114I1015:emergStop", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineScanning");
	beamlineScanning_ = new AMPVControl("beamlineScanning", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineReady");
	beamlineReady_ = new AMReadOnlyPVControl("beamlineReady", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyMovingStatus");
	energyMovingStatus_ = new AMReadOnlyPVControl("energyMovingStatus", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("fastShutterVoltage");
	energyMovingStatus_ = new AMPVControl("fastShutterVoltage", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("gratingVelocity");
	energyMovingStatus_ = new AMPVControl("gratingVelocity", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("gratingVelocityBase");
	energyMovingStatus_ = new AMPVControl("gratingVelocityBase", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("gratingAcceleration");
	energyMovingStatus_ = new AMPVControl("gratingAcceleration", sgmPVName, sgmPVName, this);
}

void SGMBeamline::usingFakeBeamline(){
	amNames2pvNames_.set("energy", "reixsHost:Energy");
	amNames2pvNames_.set("eV_Fbk", "dave:Energy:fbk");
	amNames2pvNames_.set("eVFbk", "reixsHost:eVfbk");
	amNames2pvNames_.set("mono", "dave:Energy:mono");
	amNames2pvNames_.set("undulator", "dave:Energy:undulator");
	amNames2pvNames_.set("exitSlit", "dave:Energy:exitSlit");
	amNames2pvNames_.set("exitSlitGap", "dave:Slit");
	amNames2pvNames_.set("M4", "dave:M4");
	amNames2pvNames_.set("M4Inboard", "dave:M4:inboard");
	amNames2pvNames_.set("M4Outboard", "dave:M4:outboard");
	amNames2pvNames_.set("M4Downstream", "dave:M4:downstream");
	amNames2pvNames_.set("grating", "dave:Energy:mono:grating");
	amNames2pvNames_.set("harmonic", "dave:Energy:undulator:harmonic");
	amNames2pvNames_.set("undulatorTracking", "dave:Energy:undulator:tracking");
	amNames2pvNames_.set("monoTracking", "dave:Energy:mono:tracking");
	amNames2pvNames_.set("exitSlitTracking", "dave:Energy:exitSlit:tracking");
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
	amNames2pvNames_.set("loadlockCCG", "dave:Endstation:loadlock:ccg");
	amNames2pvNames_.set("loadlockTCG", "dave:Endstation:loadlock:tcg");
	amNames2pvNames_.set("ssaManipulatorX", "reixsHost:ssa:x");
	amNames2pvNames_.set("ssaManipulatorY", "reixsHost:ssa:y");
	amNames2pvNames_.set("ssaManipulatorZ", "reixsHost:ssa:z");
	amNames2pvNames_.set("ssaManipulatorRot", "reixsHost:ssa:r");
	amNames2pvNames_.set("beamlineScanning", "reixsHost:scanning");
	amNames2pvNames_.set("beamlineReady", "dave:ready");
	amNames2pvNames_.set("energyMovingStatus", "dave:Energy:moving:status");
	amNames2pvNames_.set("fastShutterVoltage", "dave:fastShutter:V");
	amNames2pvNames_.set("scalarMode", "reixsHost:fast:trigger");
	amNames2pvNames_.set("scalarStart", "dave:scalar:start");
	amNames2pvNames_.set("gratingVelocity", "dave:mono:velo");
	amNames2pvNames_.set("gratingBaseVelocity", "dave:mono:veloBase");
	amNames2pvNames_.set("gratingAcceleration", "dave:mono:accel");

	QString sgmPVName = amNames2pvNames_.valueF("energy");
	energy_ = new AMPVwStatusControl("energy", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.01);
	sgmPVName = amNames2pvNames_.valueF("mono");
	AMReadOnlyPVwStatusControl *mono = new AMReadOnlyPVwStatusControl("mono", sgmPVName, sgmPVName+":moving", energy_);
	sgmPVName = amNames2pvNames_.valueF("undulator");
	AMReadOnlyPVwStatusControl *undulator = new AMReadOnlyPVwStatusControl("undulator", sgmPVName, sgmPVName+":moving", energy_);
	sgmPVName = amNames2pvNames_.valueF("exitSlit");
	AMReadOnlyPVwStatusControl *exitSlit = new AMReadOnlyPVwStatusControl("exitSlit", sgmPVName, sgmPVName+":moving", energy_);
	energy_->addChildControl(mono);
	energy_->addChildControl(undulator);
	energy_->addChildControl(exitSlit);
	sgmPVName = amNames2pvNames_.valueF("exitSlitGap");
	exitSlitGap_ = new AMPVwStatusControl("exitSlitGap", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4");
	m4_ = new AMReadOnlyPVwStatusControl("M4", sgmPVName, sgmPVName+":moving", this);
	sgmPVName = amNames2pvNames_.valueF("M4Inboard");
	AMPVwStatusControl *m4inboard = new AMPVwStatusControl("M4Inboard", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Outboard");
	AMPVwStatusControl *m4outboard = new AMPVwStatusControl("M4Outboard", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("M4Downstream");
	AMPVwStatusControl *m4downstream = new AMPVwStatusControl("M4Downstream", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	m4_->addChildControl(m4inboard);
	m4_->addChildControl(m4outboard);
	m4_->addChildControl(m4downstream);
	sgmPVName = amNames2pvNames_.valueF("grating");
	grating_ = new AMPVwStatusControl("grating", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("harmonic");
	harmonic_ = new AMPVwStatusControl("harmonic", sgmPVName, sgmPVName, sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("undulatorTracking");
	undulatorTracking_ = new AMPVControl("undulatorTracking", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("monoTracking");
	monoTracking_ = new AMPVControl("monoTracking", sgmPVName, sgmPVName, this, 0.1, 10);
	sgmPVName = amNames2pvNames_.valueF("exitSlitTracking");
	exitSlitTracking_ = new AMPVControl("exitSlitTracking", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("tey");
	tey_ = new AMReadOnlyPVControl("tey", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("tfy");
	tfy_ = new AMReadOnlyPVControl("tfy", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("tfyHVSetpoint");
	tfyHVSetpoint_ = new AMPVControl("tfyHVSetpoint", amNames2pvNames_.valueF("tfyHVFbk"), amNames2pvNames_.valueF("tfyHVSetpoint"), this, 10.0);
	sgmPVName = amNames2pvNames_.valueF("tfyHVFbk");
	tfyHVFbk_ = new AMPVControl("tfyHVFbk", sgmPVName, sgmPVName, this, 10.0);

	sgmPVName = amNames2pvNames_.valueF("pgt");
	pgt_ = new AMReadOnlyPVControl("pgt", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtHVSetpoint");
	pgtHVSetpoint_ = new AMPVControl("pgtHVSetpoint", amNames2pvNames_.valueF("pgtHVSetpoint"), amNames2pvNames_.valueF("pgtHVSetpoint"), this, 0.5);
	sgmPVName = amNames2pvNames_.valueF("pgtHVFbk");
	pgtHVFbk_ = new AMReadOnlyPVControl("pgtHVFbk", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationTime");
	pgtIntegrationTime_ = new AMPVControl("pgtIntegrationTime", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("pgtIntegrationMode");
	pgtIntegrationMode_ = new AMPVControl("pgtIntegrationMode", sgmPVName, sgmPVName, this, 0.1);

	sgmPVName = amNames2pvNames_.valueF("I0");
	i0_ = new AMReadOnlyPVControl("I0", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("eVFbk");
	eVFbk_ = new AMReadOnlyPVControl("eVFbk", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("loadlockCCG");
	loadlockCCG_ = new AMReadOnlyPVControl("loadlockCCG", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("loadlockTCG");
	loadlockTCG_ = new AMReadOnlyPVControl("loadlockTCG", sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorX");
	ssaManipulatorX_ = new AMPVwStatusControl("ssaManipulatorX", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", this, 0.1);
	ssaManipulatorXStop_ = new AMPVControl("ssaManipulatorXStop", sgmPVName+":moving", sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorY");
	ssaManipulatorY_ = new AMPVwStatusControl("ssaManipulatorY", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", this, 0.1);
	ssaManipulatorYStop_ = new AMPVControl("ssaManipulatorYStop", sgmPVName+":moving", sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorZ");
	ssaManipulatorZ_ = new AMPVwStatusControl("ssaManipulatorZ", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", this, 0.1);
	ssaManipulatorZStop_ = new AMPVControl("ssaManipulatorZStop", sgmPVName+":moving", sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("ssaManipulatorRot");
	ssaManipulatorRot_ = new AMPVwStatusControl("ssaManipulatorRot", sgmPVName+":fbk", sgmPVName+":sp", sgmPVName+":moving", this, 0.1);
	ssaManipulatorRotStop_ = new AMPVControl("ssaManipulatorRotStop", sgmPVName+":moving", sgmPVName+":moving", this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineScanning");
	beamlineScanning_ = new AMPVControl("beamlineScanning", sgmPVName, sgmPVName, this, 0.1);
	sgmPVName = amNames2pvNames_.valueF("beamlineReady");
	beamlineReady_ = new AMReadOnlyPVControl("beamlineReady", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("energyMovingStatus");
	energyMovingStatus_ = new AMReadOnlyPVControl("energyMovingStatus", sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("fastShutterVoltage");
	energyMovingStatus_ = new AMPVControl("fastShutterVoltage", sgmPVName, sgmPVName, this);

	sgmPVName = amNames2pvNames_.valueF("gratingVelocity");
	energyMovingStatus_ = new AMPVControl("gratingVelocity", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("gratingVelocityBase");
	energyMovingStatus_ = new AMPVControl("gratingVelocityBase", sgmPVName, sgmPVName, this);
	sgmPVName = amNames2pvNames_.valueF("gratingAcceleration");
	energyMovingStatus_ = new AMPVControl("gratingAcceleration", sgmPVName, sgmPVName, this);
}

SGMBeamline::SGMBeamline() : AMControl("SGMBeamline", "n/a") {
	usingFakeBeamline();
	//	usingSGMBeamline();


	addChildControl(energy_);
	addChildControl(exitSlitGap_);
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

	teyControlSet_ = new AMControlSet(this);
	teyControlSet_->setName("TEY Controls");
	teyControlSet_->addControl(tey_);
	teyDetector_ = NULL;
	unconnectedSets_.append(teyControlSet_);
	connect(teyControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	tfyControlSet_ = new AMControlSet(this);
	tfyControlSet_->setName("TFY Controls");
	tfyControlSet_->addControl(tfy_);
	tfyControlSet_->addControl(tfyHVSetpoint_);
	tfyControlSet_->addControl(tfyHVFbk_);
	tfyDetector_ = NULL;
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
	unconnectedSets_.append(pgtControlSet_);
	connect(pgtControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	i0ControlSet_ = new AMControlSet(this);
	i0ControlSet_->setName("I0 Controls");
	i0ControlSet_->addControl(i0_);
	i0Detector_ = NULL;
	unconnectedSets_.append(i0ControlSet_);
	connect(i0ControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

	eVFbkControlSet_ = new AMControlSet(this);
	eVFbkControlSet_->setName("Energy Feedback Controls");
	eVFbkControlSet_->addControl(eVFbk_);
	eVFbkDetector_ = NULL;
	unconnectedSets_.append(eVFbkControlSet_);
	connect(eVFbkControlSet_, SIGNAL(connected(bool)), this, SLOT(onControlSetConnected(bool)));

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

	allDetectors_ = new AMDetectorInfoSet(this);
	allDetectors_->setName("All Detectors");
	/*
	allDetectors_->addDetector(i0Detector_, true);
	allDetectors_->addDetector(eVFbkDetector_, true);
	allDetectors_->addDetector(teyDetector_, true);
	allDetectors_->addDetector(tfyDetector_, true);
	allDetectors_->addDetector(pgtDetector_, false);
	*/

	feedbackDetectors_ = new AMDetectorInfoSet(this);
	feedbackDetectors_->setName("Feedback Detectors");

	XASDetectors_ = new AMDetectorInfoSet(this);
	XASDetectors_->setName("XAS Detectors");
	/*
	XASDetectors_->addDetector(teyDetector_, true);
	XASDetectors_->addDetector(tfyDetector_, true);
	XASDetectors_->addDetector(pgtDetector_, false);
	*/

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
}

SGMBeamline::~SGMBeamline()
{
}

bool SGMBeamline::detectorConnectedByName(QString name){
	for(int x = 0; x < unconnectedSets_.count(); x++){
		if( QString::compare(unconnectedSets_.at(x)->name().section(' ', 0, 0), name, Qt::CaseInsensitive) == 0)
			return false;
	}
	return true;
}

bool SGMBeamline::isScanning(){
	if( fabs(beamlineScanning_->value() -1.0) < beamlineScanning_->tolerance() )
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
		qDebug() << ctrlSet->name() << " is connected";
		unconnectedSets_.removeAll(ctrlSet);
		if(!teyDetector_ && ctrlSet->name() == "TEY Controls"){
			teyDetector_ = new AMSingleControlDetector(tey_->name(), tey_, this);
			teyDetector_->setDescription("TEY");
			allDetectors_->addDetector(teyDetector_, true);
			XASDetectors_->addDetector(teyDetector_, true);
		}
		else if(!tfyDetector_ && ctrlSet->name() == "TFY Controls"){
			tfyDetector_ = new MCPDetector(tfy_->name(), tfy_, tfyHVSetpoint_, tfyHVFbk_, this);
			tfyDetector_->setDescription("TFY");
			allDetectors_->addDetector(tfyDetector_, true);
			XASDetectors_->addDetector(tfyDetector_, true);
		}
		else if(!pgtDetector_ && ctrlSet->name() == "SDD Controls"){
			pgtDetector_ = new PGTDetector(pgt_->name(), pgt_, pgtHVSetpoint_, pgtHVFbk_, pgtIntegrationTime_, pgtIntegrationMode_, this);
			pgtDetector_->setDescription("SDD");
			allDetectors_->addDetector(pgtDetector_, false);
			XASDetectors_->addDetector(pgtDetector_, false);
		}
		else if(!i0Detector_ && ctrlSet->name() == "I0 Controls"){
			i0Detector_ = new AMSingleControlDetector(i0_->name(), i0_, this);
			i0Detector_->setDescription("I0");
			allDetectors_->addDetector(i0Detector_, true);
			feedbackDetectors_->addDetector(i0Detector_, true);
		}
		else if(!eVFbkDetector_ && ctrlSet->name() == "Energy Feedback Controls"){
			eVFbkDetector_ = new AMSingleControlDetector(eVFbk_->name(), eVFbk_, this);
			eVFbkDetector_->setDescription("Energy Feedback");
			allDetectors_->addDetector(eVFbkDetector_, true);
			feedbackDetectors_->addDetector(eVFbkDetector_, true);
		}
		emit controlSetConnectionschanged();
	}
	else{
		if( !unconnectedSets_.contains(ctrlSet) ){
			unconnectedSets_.append(ctrlSet);
			emit controlSetConnectionschanged();
		}
		qDebug() << ctrlSet->name() << " is NOT connected";
	}
	QString tmpStr = "Unconnected at end";
	for(int x = 0; x < unconnectedSets_.count(); x++)
		tmpStr.append(" "+unconnectedSets_.at(x)->name());
	qDebug() << tmpStr;
}

SGMBeamline* SGMBeamline::sgm() {

	if(instance_ == 0)
		instance_ = new SGMBeamline();

	return instance_;

}

void SGMBeamline::releaseSGM() {

	if(instance_) {
		delete instance_;
		instance_ = 0;
	}

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
