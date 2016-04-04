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

#include "REIXSBeamline.h"
#include "REIXSSampleMotor.h"
#include "util/AMErrorMonitor.h"
#include "dataman/AMSamplePlatePre2013.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSSR570.h"
#include "beamline/CLS/CLSMAXvMotor.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

REIXSBeamline::REIXSBeamline() :
	CLSBeamline("REIXSBeamline")
{
	// Upstream controls
	photonSource_ = new REIXSPhotonSource(this);
	addChildControl(photonSource_);

	valvesAndShutters_ = new REIXSValvesAndShutters(this);
	addChildControl(valvesAndShutters_);

	// Spectromter: controls and control set for positioners:
	spectrometer_ = new REIXSSpectrometer(this);
	addChildControl(spectrometer_);

	spectrometerPositionSet_ = new AMControlSet(this);
	spectrometerPositionSet_->addControl(spectrometer()->spectrometerRotationDrive());
	spectrometerPositionSet_->addControl(spectrometer()->detectorTranslation());
	spectrometerPositionSet_->addControl(spectrometer()->detectorTiltDrive());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->x());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->y());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->z());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->u());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->v());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->w());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->r());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->s());
	spectrometerPositionSet_->addControl(spectrometer()->hexapod()->t());
	spectrometerPositionSet_->addControl(spectrometer()->endstationTranslation());  //DAVID ADDED

	// Sample Chamber: controls and control set for positioners:
	sampleChamber_ = new REIXSSampleChamber(this);
	addChildControl(sampleChamber_);
	sampleManipulatorSet_ = new AMControlSet(this);

	sampleManipulatorSet_->addControl(sampleChamber()->x());
	sampleManipulatorSet_->addControl(sampleChamber()->y());
	sampleManipulatorSet_->addControl(sampleChamber()->z());
	sampleManipulatorSet_->addControl(sampleChamber()->r());

	// MCP detector
	mcpDetector_ = new REIXSXESMCPDetector(this);

	scaler_ = new CLSSIS3820Scaler("BL1610-ID-2:mcs", this);

	scaler_->channelAt(3)->setCustomChannelName("PFY");

	scaler_->channelAt(4)->setCustomChannelName("TFY");

	scaler_->channelAt(18)->setCustomChannelName("TEY");
	CLSSR570 *tempSR570 = new CLSSR570("TEY", "AMP1610-4-03", this);
	scaler_->channelAt(18)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(18)->setVoltagRange(AMRange(0.25, 4.75));

	scaler_->channelAt(16)->setCustomChannelName("I0");
	tempSR570 = new CLSSR570("I0", "AMP1610-4-02", this);
	scaler_->channelAt(16)->setCurrentAmplifier(tempSR570);
	scaler_->channelAt(16)->setVoltagRange(AMRange(0.25, 4.75));

	i0Detector_ = new CLSBasicScalerChannelDetector("I0", "I0", scaler_, 16, this);
	teyDetector_ = new CLSBasicScalerChannelDetector("TEY", "TEY", scaler_, 18, this);
	tfyDetector_ = new CLSBasicScalerChannelDetector("TFY", "TFY", scaler_, 4, this);
	pfyDetector_ = new CLSBasicScalerChannelDetector("PFY", "PFY", scaler_, 3, this);

	i0Current_ = new AMReadOnlyPVControl("I0Current","BL1610-ID-2:mcs16:fbk", this, "I0 Current");
	teyCurrent_ = new AMReadOnlyPVControl("TEYCurrent","BL1610-ID-2:mcs18:fbk", this, "TEY Current");

	/*
	 Build a control set of all the controls we want to make available to REIXSControlMoveAction, as well as record in the scan's scanInitialConditions()
	allControlsSet_ = new AMControlSet(this);
	allControlsSet_->addControl(photonSource()->energy());
	allControlsSet_->addControl(photonSource()->monoGratingSelector());
	allControlsSet_->addControl(photonSource()->monoMirrorSelector());
	allControlsSet_->addControl(photonSource()->monoSlit());
	allControlsSet_->addControl(spectrometer());
	allControlsSet_->addControl(spectrometer()->spectrometerRotationDrive());
	allControlsSet_->addControl(spectrometer()->detectorTranslation());
	allControlsSet_->addControl(spectrometer()->detectorTiltDrive());
	allControlsSet_->addControl(spectrometer()->hexapod()->x());
	allControlsSet_->addControl(spectrometer()->hexapod()->y());
	allControlsSet_->addControl(spectrometer()->hexapod()->z());
	allControlsSet_->addControl(spectrometer()->hexapod()->u());
	allControlsSet_->addControl(spectrometer()->hexapod()->v());
	allControlsSet_->addControl(spectrometer()->hexapod()->w());
	allControlsSet_->addControl(spectrometer()->hexapod()->r());
	allControlsSet_->addControl(spectrometer()->hexapod()->s());
	allControlsSet_->addControl(spectrometer()->hexapod()->t());
	allControlsSet_->addControl(sampleChamber()->x());
	allControlsSet_->addControl(sampleChamber()->y());
	allControlsSet_->addControl(sampleChamber()->z());
	allControlsSet_->addControl(sampleChamber()->r());
	allControlsSet_->addControl(spectrometer()->endstationTranslation()); //DAVID 001 ADDED
	allControlsSet_->addControl(photonSource()->M5Pitch());   //DAVID ADDED 003
	allControlsSet_->addControl(photonSource()->M5Yaw());   //DAVID ADDED 003
	allControlsSet_->addControl(spectrometer()->gratingMask());  //DAVID ADDED 005
	*/

	tmSet_ = new AMControlSet(this);
	tmSet_->addControl(spectrometer()->tmSOE());
	tmSet_->addControl(spectrometer()->tmMCPPreamp());

	meterSet_ = new AMControlSet(this);
	meterSet_->addControl(i0Current_);
	meterSet_->addControl(teyCurrent_);

	setupExposedControls();
	setupExposedDetectors();

	samplePlate_ = new AMSamplePlatePre2013(this);
}

REIXSBeamline::~REIXSBeamline() {
}

void REIXSBeamline::setupExposedControls(){
	addExposedControl(photonSource()->energy());
	addExposedControl(photonSource()->userEnergyOffset());
	addExposedControl(photonSource()->monoSlit());
	addExposedControl(sampleChamber()->x());
	addExposedControl(sampleChamber()->y());
	addExposedControl(sampleChamber()->z());
	addExposedControl(sampleChamber()->r());
	addExposedControl(sampleChamber()->horizontal());
	addExposedControl(sampleChamber()->normal());
	addExposedControl(spectrometer()->gratingMask());
	addExposedControl(spectrometer());


	if(QApplication::instance()->arguments().contains("--admin"))
	{
		addExposedControl(photonSource()->monoGratingSelector());
		addExposedControl(photonSource()->monoMirrorSelector());
		addExposedControl(spectrometer()->spectrometerRotationDrive());
		addExposedControl(spectrometer()->detectorTranslation());
		addExposedControl(spectrometer()->detectorTiltDrive());
		addExposedControl(spectrometer()->hexapod()->x());
		addExposedControl(spectrometer()->hexapod()->y());
		addExposedControl(spectrometer()->hexapod()->z());
		addExposedControl(spectrometer()->hexapod()->u());
		addExposedControl(spectrometer()->hexapod()->v());
		addExposedControl(spectrometer()->hexapod()->w());
		addExposedControl(spectrometer()->hexapod()->r());
		addExposedControl(spectrometer()->hexapod()->s());
		addExposedControl(spectrometer()->hexapod()->t());
		addExposedControl(spectrometer()->endstationTranslation());
		addExposedControl(photonSource()->M5Pitch());
		addExposedControl(photonSource()->M5Yaw());
	}
}

void REIXSBeamline::setupExposedDetectors(){
	addExposedDetector(i0Detector_);
	addExposedDetector(teyDetector_);
	addExposedDetector(tfyDetector_);
	addExposedDetector(pfyDetector_);
}

AMAction3 *REIXSBeamline::buildBeamStateChangeAction(bool beamOn) const
{
	AMListAction3 *list = new AMListAction3(new AMListActionInfo3("REIXS Beam On", "REIXS Beam Off"));

	if (beamOn){

		if (REIXSBeamline::bl()->valvesAndShutters()->ssh1()->value() != 1.0)
			list->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->ssh1(), 1.0));

		if (REIXSBeamline::bl()->valvesAndShutters()->psh2()->value() != 1.0)
			list->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->psh2(), 1.0));

		if (REIXSBeamline::bl()->valvesAndShutters()->psh4()->value() != 1.0)
			list->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->psh4(), 1.0));
	}

	else if (!beamOn && REIXSBeamline::bl()->valvesAndShutters()->psh4()->value() != 0.0)
		list->addSubAction(AMActionSupport::buildControlMoveAction(REIXSBeamline::bl()->valvesAndShutters()->psh4(), 0.0));

	return list;
}

REIXSPhotonSource::~REIXSPhotonSource(){}
REIXSPhotonSource::REIXSPhotonSource(QObject *parent) :
	AMCompositeControl("photonSource", "", parent, "EPU and Monochromator")
{
	AMPVwStatusControl* directEnergy = new AMPVwStatusControl("beamlineEV", "REIXS:MONO1610-I20-01:user:energy:fbk", "REIXS:user:energy", "REIXS:status", "REIXS:energy:stop", 0, 1000);//, 2.0,new AMControlStatusCheckerDefault(1),-1)
	directEnergy->setSettlingTime(0);
	directEnergy_ = directEnergy;
	directEnergy_->setDescription("Beamline Energy");

	bypassEnergy_ = new AMPVControl("bypassBeamlineEV", "REIXS:user:energy", "REIXS:user:energy", "REIXS:energy:stop");

	userEnergyOffset_ = new AMPVControl("userEnergyOffset", "REIXS:user:energy:offset", "REIXS:user:energy:offset", QString(), this);
	userEnergyOffset_->setDescription("User Energy Offest");

	energy_ = new REIXSBrokenMonoControl(directEnergy, 1.05, 3, 0.5, 0.5, 150, 1, 0.25, this);
	energy_->setDescription("Beamline Energy");

	monoSlit_ = new AMPVwStatusAndUnitConversionControl("monoSlit", "SMTR1610-I20-10:mm:fbk", "SMTR1610-I20-10:mm", "SMTR1610-I20-10:status", "SMTR1610-I20-10:stop", new AMScaleAndOffsetUnitConverter("um", 1000), 0, this);  //DAVID changed tolerance from 0.1->0.5
	monoSlit_->setDescription("Mono Slit Width");

	M5Pitch_ = new AMPVwStatusControl("M5Pitch", "SMTR1610-4-I20-02:mm:fbk", "SMTR1610-4-I20-02:mm","SMTR1610-4-I20-02:status","SMTR1610-4-I20-02:stop",this,0.5); //DAVID ADDED
	M5Pitch_->setDescription("M5 Mirror Pitch"); //DAVID ADDED

	M5Yaw_ = new AMPVwStatusControl("M5Yaw", "SMTR1610-4-I20-04:mm:fbk", "SMTR1610-4-I20-04:mm","SMTR1610-4-I20-04:status","SMTR1610-4-I20-04:stop",this,0.5); //DAVID ADDED
	M5Yaw_->setDescription("M5 Mirror Yaw"); //DAVID ADDED

	monoGratingTranslation_ = new AMPVwStatusControl("monoGratingTranslation", "MONO1610-I20-01:grating:trans:mm:fbk", "MONO1610-I20-01:grating:trans:mm", "MONO1610-I20-01:grating:trans:status", "SMTR1610-I20-04:stop", this, 0.05);
	monoGratingTranslation_->setDescription("Mono Grating Translation");
	monoGratingSelector_ = new AMPVwStatusControl("monoGratingSelector", "MONO1610-I20-01:grating:select:fbk", "MONO1610-I20-01:grating:select", "MONO1610-I20-01:grating:trans:status", "SMTR1610-I20-04:stop", this, 1);
	monoGratingSelector_->setDescription("Mono Grating");

	monoMirrorTranslation_ = new AMPVwStatusControl("monoMirrorTranslation", "MONO1610-I20-01:mirror:trans:mm:fbk", "MONO1610-I20-01:mirror:trans:mm", "MONO1610-I20-01:mirror:trans:status", "SMTR1610-I20-02:stop", this, 0.05);
	monoMirrorTranslation_->setDescription("Mono Mirror Translation");
	monoMirrorSelector_ = new AMPVwStatusControl("monoMirrorSelector", "MONO1610-I20-01:mirror:select:fbk", "MONO1610-I20-01:mirror:select", "MONO1610-I20-01:mirror:trans:status", "SMTR1610-I20-02:stop", this, 1);
	monoMirrorSelector_->setDescription("Mono Mirror");

	monoMirrorAngleStatus_ = new AMReadOnlyPVControl("monoMirrorAngleStatus","MONO1610-I20-01:mirror:status",this,"Mono Mirror Angle Status");
	monoGratingAngleStatus_ = new AMReadOnlyPVControl("monoGratingAngleStatus","MONO1610-I20-01:grating:status",this,"Mono Grating Angle Status");

	epuPolarization_ = new AMPVwStatusControl("epuPolarization", "REIXS:UND1410-02:polarization", "REIXS:UND1410-02:polarization", "REIXS:UND1410-02:energy:status", QString(), this, 0.1);
	epuPolarization_->setDescription("EPU Polarization");
	epuPolarization_->enumNames() << "Circular Left";
	epuPolarization_->enumNames() << "Circular Right";
	epuPolarization_->enumNames() << "Linear Horizontal";
	epuPolarization_->enumNames() << "Linear Vertical -";
	epuPolarization_->enumNames() << "Linear Vertical +";
	epuPolarization_->enumNames() << "Linear Inclined";
	epuPolarizationAngle_ = new AMPVwStatusControl("epuPolarizationAngle", "REIXS:UND1410-02:polarAngle", "REIXS:UND1410-02:polarAngle", "REIXS:UND1410-02:energy:status", QString(), this, 0.5);
	epuPolarizationAngle_->setDescription("EPU Polarization Angle");

	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent","PCT1402-01:mA:fbk", this, "Storage Ring Current");
}

REIXSValvesAndShutters::~REIXSValvesAndShutters(){}
REIXSValvesAndShutters::REIXSValvesAndShutters(QObject *parent) : AMCompositeControl("valvesAndShutters", "", parent)
{
	beamIsOn_ = false;

	ssh1_ = new CLSExclusiveStatesControl("safetyShutter1", "SSH1410-I00-01:state", "SSH1410-I00-01:opr:open", "SSH1410-I00-01:opr:close", this);
	ssh1_->setDescription("Safety Shutter 1");

	psh2_ = new CLSExclusiveStatesControl("photonShutter2", "PSH1410-I00-02:state", "PSH1410-I00-02:opr:open", "PSH1410-I00-02:opr:close", this);
	psh2_->setDescription("Safety Shutter 2");

	psh4_ = new CLSExclusiveStatesControl("photonShutter4", "PSH1610-I20-01:state", "PSH1610-I20-01:opr:open", "PSH1610-I20-01:opr:close", this);
	psh4_->setDescription("Photon Shutter 4");

	endstationValve_ = new CLSExclusiveStatesControl("XESendstationValve", "VVR1610-4-I21-01:state", "VVR1610-4-I21-01:opr:open", "VVR1610-4-I21-01:opr:close", this);
	endstationValve_->setDescription("XES Endstation Valve");

	addChildControl(ssh1_);
	addChildControl(psh2_);
	addChildControl(psh4_);
	addChildControl(endstationValve_);


	// connect to monitor full beam status:
	/////////////////////
	connect(ssh1_, SIGNAL(connected(bool)), this, SLOT(reviewIsBeamOn()));
	connect(psh2_, SIGNAL(connected(bool)), this, SLOT(reviewIsBeamOn()));
	connect(psh4_, SIGNAL(connected(bool)), this, SLOT(reviewIsBeamOn()));
	connect(ssh1_, SIGNAL(stateChanged(int)), this, SLOT(reviewIsBeamOn()));
	connect(psh2_, SIGNAL(stateChanged(int)), this, SLOT(reviewIsBeamOn()));
	connect(psh4_, SIGNAL(stateChanged(int)), this, SLOT(reviewIsBeamOn()));

	reviewIsBeamOn();
}


void REIXSValvesAndShutters::reviewIsBeamOn()
{
	bool beamWasOn = beamIsOn_;

	beamIsOn_ = ssh1_->isConnected() &&
			psh2_->isConnected() &&
			psh4_->isConnected() &&
			ssh1_->isOpen() &&
			psh2_->isOpen() &&
			psh4_->isOpen();

	if(beamIsOn_ != beamWasOn)
		emit beamOnChanged(beamIsOn_);
}


REIXSSampleChamber::~REIXSSampleChamber(){}
REIXSSampleChamber::REIXSSampleChamber(QObject *parent)
	: AMCompositeControl("sampleChamber", "", parent) {

	setDescription("XES Sample Chamber Controls");

	// Motor information here was updated Nov. 2011 for the MDrive motors on the sample chamber, which still don't have unit conversion built into the driver.
	// All motors are currently running at a microstep setting of 256. Therefore one revolution of the motor is: 200*256 = 51200 steps.
	// Units per rev:
	// The X and Y motors are equipped with the IMS "D" lead screw: 0.0833"/rev (2.116mm/rev).
	// The Z stage has a 2.5mm/rev lead screw. However, it has a 10:1 gear reducer before the lead screw, so it's actually 0.25mm/rev.
	// The sample theta stage has 100 teeth on the circumference gear. One motor revolution advances by one tooth, so it's 360deg/100revs, or 3.6deg/rev.
	// The load lock theta stage also has 100 teeth on its circumference gear, for 3.6deg/rev.
	// [Guessing] The load lock Z stage looks like it has the same 2.5mm/lead screw rev. However, it also has a 90-degree gear from the motor to the lead screw with 20 teeth, or 1 lead screw rev/20 motor revs.   ie: (2.5mm/screwRev)*(1screwRev/20rev) = 0.125mm/rev.

	//								name	  PV base name        units unitsPerRev offset microsteps descript. tolerance startTimeoutSecs, parent
	x_ = new CLSMDriveMotorControl("sampleX", "SMTR1610-4-I21-08", "mm", 2.116, 0, 256, "Sample Chamber X", 0.5, 2.0, this);
	x_->setSettlingTime(0.5);
	x_->setMoveStartTolerance(x_->writeUnitConverter()->convertFromRaw(5));
	x_->setContextKnownDescription("X");

	y_ = new CLSMDriveMotorControl("sampleY", "SMTR1610-4-I21-10", "mm", 2.116, 0, 256, "Sample Chamber Y", 0.5, 2.0, this);
	y_->setSettlingTime(0.5);
	y_->setMoveStartTolerance(y_->writeUnitConverter()->convertFromRaw(5));
	y_->setContextKnownDescription("Y");

	z_ = new CLSMDriveMotorControl("sampleZ", "SMTR1610-4-I21-07", "mm", 0.25, 0, 256, "Sample Chamber Z", 0.5, 2.0, this);
	z_->setSettlingTime(0.5);
	z_->setMoveStartTolerance(z_->writeUnitConverter()->convertFromRaw(5));
	z_->setContextKnownDescription("Z");

	r_ = new CLSMDriveMotorControl("sampleTheta", "SMTR1610-4-I21-11", "deg", 3.6, 0, 256, "Sample Chamber Theta", 0.5, 2.0, this);
	r_->setSettlingTime(0.5);
	r_->setMoveStartTolerance(r_->writeUnitConverter()->convertFromRaw(5));
	r_->setContextKnownDescription("Theta");

	horizontal_ = new REIXSSampleMotor(AMMotorGroupObject::HorizontalMotion, "sampleHorizontal", "mm", x_, y_, r_, this, "Sample Plate Horizontal");
	normal_ = new REIXSSampleMotor(AMMotorGroupObject::NormalMotion, "sampleNormal", "mm", x_, y_, r_, this, "Sample Plate Normal");

	loadLockZ_ = new CLSMDriveMotorControl("loadLockZ", "SMTR1610-4-I21-09", "mm", 0.125, 0, 256, "Load Lock Z", 0.5, 2.0, this);
	loadLockZ_->setSettlingTime(0.2);
	loadLockZ_->setMoveStartTolerance(loadLockZ_->writeUnitConverter()->convertFromRaw(5));

	loadLockR_ = new CLSMDriveMotorControl("loadLockTheta", "SMTR1610-4-I21-12", "deg", 3.6, 0, 256, "Load Lock Theta", 0.5, 2.0, this);
	loadLockR_->setSettlingTime(0.2);
	loadLockR_->setMoveStartTolerance(loadLockR_->writeUnitConverter()->convertFromRaw(5));

	addChildControl(x_);
	addChildControl(y_);
	addChildControl(z_);
	addChildControl(r_);
	addChildControl(loadLockZ_);
	addChildControl(loadLockR_);
}


REIXSHexapod::~REIXSHexapod(){}
REIXSHexapod::REIXSHexapod(QObject* parent)
	: AMCompositeControl("hexapod", "", parent) {

	setDescription("XES Hexapod");

	QString baseName = "HXPD1610-4-I21-01:";

	x_ = new AMPVwStatusControl("hexapodX", baseName+"X:sp", baseName+"X", baseName+"moving", QString(), this, 0.01);
	x_->setDescription("Hexapod X");
	x_->setAllowsMovesWhileMoving(true);
	x_->setSettlingTime(0.1);

	y_ = new AMPVwStatusControl("hexapodY", baseName+"Y:sp", baseName+"Y", baseName+"moving", QString(), this, 0.01);
	y_->setDescription("Hexapod Y");
	y_->setAllowsMovesWhileMoving(true);
	y_->setSettlingTime(0.1);

	z_ = new AMPVwStatusControl("hexapodZ", baseName+"Z:sp", baseName+"Z", baseName+"moving", QString(), this, 0.01);
	z_->setDescription("Hexapod Z");
	z_->setAllowsMovesWhileMoving(true);
	z_->setSettlingTime(0.1);

	u_ = new AMPVwStatusControl("hexapodU", baseName+"U:sp", baseName+"U", baseName+"moving", QString(), this, 0.05);
	u_->setDescription("Hexapod U");
	u_->setAllowsMovesWhileMoving(true);
	u_->setSettlingTime(0.1);

	v_ = new AMPVwStatusControl("hexapodV", baseName+"V:sp", baseName+"V", baseName+"moving", QString(), this, 0.05);
	v_->setDescription("Hexapod V");
	v_->setAllowsMovesWhileMoving(true);
	v_->setSettlingTime(0.1);

	w_ = new AMPVwStatusControl("hexapodW", baseName+"W:sp", baseName+"W", baseName+"moving", QString(), this, 0.05);
	w_->setDescription("Hexapod W");
	w_->setAllowsMovesWhileMoving(true);
	w_->setSettlingTime(0.1);

	r_ = new AMPVControl("hexapodR", baseName+"R:sp", baseName+"R", QString(), this, 0.001);
	r_->setDescription("Hexapod R");
	r_->setAllowsMovesWhileMoving(true);

	s_ = new AMPVControl("hexapodS", baseName+"S:sp", baseName+"S", QString(), this, 0.001);
	s_->setDescription("Hexapod S");
	s_->setAllowsMovesWhileMoving(true);

	t_ = new AMPVControl("hexapodT", baseName+"T:sp", baseName+"T", QString(), this, 0.001);
	t_->setDescription("Hexapod T");
	t_->setAllowsMovesWhileMoving(true);

	addChildControl(x_);
	addChildControl(y_);
	addChildControl(z_);
	addChildControl(u_);
	addChildControl(v_);
	addChildControl(w_);
	addChildControl(r_);
	addChildControl(s_);
	addChildControl(t_);

}

REIXSSpectrometer::~REIXSSpectrometer(){}
REIXSSpectrometer::REIXSSpectrometer(QObject *parent)
	: AMCompositeControl("spectrometer", "eV", parent) {

	setDescription("XES Detector Energy");

	spectrometerRotationDrive_ = new AMPVwStatusControl("spectrometerRotationDrive",
							    "SMTR1610-4-I21-01:mm:fbk",
							    "SMTR1610-4-I21-01:mm",
							    "SMTR1610-4-I21-01:status",
							    "SMTR1610-4-I21-01:stop", this, 1.0);
	spectrometerRotationDrive_->setDescription("XES Spectrometer Lift");
	spectrometerRotationDrive_->setSettlingTime(1.0);


	detectorTranslation_ = new AMPVwStatusControl("detectorTranslation",
						      "SMTR1610-4-I21-04:mm:fbk",
						      "SMTR1610-4-I21-04:mm",
						      "SMTR1610-4-I21-04:status",
						      "SMTR1610-4-I21-04:stop", this, 2.0);

	detectorTranslation_->setDescription("XES Detector Translation");
	detectorTranslation_->setSettlingTime(1.0);

	detectorTiltDrive_ = new AMPVwStatusControl("detectorTiltDrive",
						    "SMTR1610-4-I21-02:mm:sp",
						    "SMTR1610-4-I21-02:mm",
						    "SMTR1610-4-I21-02:status",
						    "SMTR1610-4-I21-02:stop", this, 0.05);
	detectorTiltDrive_->setDescription("XES Detector Tilt Stage");
	detectorTiltDrive_->setSettlingTime(0.5);

	endstationTranslation_ = new AMPVwStatusControl("endstationTranslation",
							"SMTR1610-4-I21-05:mm:fbk",
							"SMTR1610-4-I21-05:mm",
							"SMTR1610-4-I21-05:status",
							"SMTR1610-4-I21-05:stop", this, 0.05);  //DAVID ADDED
	endstationTranslation_->setDescription("Endstation Translation");
	endstationTranslation_->setSettlingTime(0.2);

	gratingMask_ = new AMPVwStatusControl("gratingMask",
					      "SMTR1610-4-I21-03:mm:sp",
					      "SMTR1610-4-I21-03:mm",
					      "SMTR1610-4-I21-03:status",
					      "SMTR1610-4-I21-03:stop",this,0.01); //DAVID ADDED 005
	gratingMask_->setDescription("Grating Mask Position");
	gratingMask_->setSettlingTime(0.2);


	hexapod_ = new REIXSHexapod(this);

	tmMCPPreamp_ = new AMReadOnlyPVControl("MCPPreampTemp", "TM1610-4-I21-03", this, "MCP Preamp Temperature");
	tmSOE_ = new AMReadOnlyPVControl("SOETemp", "TM1609-01", this, "SOE Temperature");


	addChildControl(spectrometerRotationDrive_);
	addChildControl(detectorTranslation_);
	addChildControl(detectorTiltDrive_);
	addChildControl(endstationTranslation_);  //DAVID ADDED
	addChildControl(hexapod_);
	addChildControl(gratingMask_);  //DAVID ADDED 005
	addChildControl(tmMCPPreamp_);
	addChildControl(tmSOE_);

	currentGrating_ = -1; specifiedGrating_ = 0;
	currentFocusOffset_ = 0; specifiedFocusOffset_ = 0;
	currentDetectorTiltOffset_ = 0; specifiedDetectorTiltOffset_ = 0;

	specifiedEV_ = 395;

	moveAction_ = 0;
	setTolerance(0.1);

	// valueChanged(): if the optical origin is at the rotation point and everything is perfect, then only the spectrometerRotationDrive_ motor will affect the energy value.  But in the non-perfect-aligned general math situation, the translation can also affect eV.  And of course gratings...
	// Here we make the connections to get our valueChanged() signal:
	connect(spectrometerRotationDrive_, SIGNAL(valueChanged(double)), this, SLOT(scheduleReviewValueChanged()));
	connect(detectorTranslation_, SIGNAL(valueChanged(double)), this, SLOT(scheduleReviewValueChanged()));
	connect(this, SIGNAL(gratingChanged(int)), this, SLOT(scheduleReviewValueChanged()));
	connect(endstationTranslation_, SIGNAL(valueChanged(double)), this, SLOT(scheduleReviewValueChanged()));  //DAVID ADDED

	connect(&reviewValueChangedFunction_, SIGNAL(executed()), this, SLOT(reviewValueChanged()));

	connect(this, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));

}

AMControl::FailureExplanation REIXSSpectrometer::move(double setpoint)
{
	if(!isConnected()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 2, "Can't move the spectrometer: some motor controls are not connected. Check that the IOCs are running and the network connections are good."));
		return NotConnectedFailure;
	}

	// can't start a move while moving
	if(moveInProgress() || isMoving()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 2, "Can't move the spectrometer, because it's already moving.  Stop the spectrometer first before attempting another move."));
		return AlreadyMovingFailure;
	}

	specifiedEV_ = setpoint;
	currentFocusOffset_ = specifiedFocusOffset_;
	currentDetectorTiltOffset_ = specifiedDetectorTiltOffset_;

	if(currentGrating_ != specifiedGrating_) {
		emit gratingChanged(currentGrating_ = specifiedGrating_);
	}

	/// \todo limits? out of range?
	moveSetpoint_ = calibration_.computeSpectrometerPosition(specifiedGrating_, specifiedEV_, currentFocusOffset_, currentDetectorTiltOffset_);

	// build the move action (With sub-actions to run in parallel)
	moveAction_ = new AMListAction3(new AMListActionInfo3("spectrometer eV move", "spectrometer eV move"), AMListAction3::Parallel, this);
	// if we need to move the grating into position, add actions for that:
	if(!gratingInPosition()) {
		// make a sequential action for the grating moves
		AMListAction3* gratingAction = new AMListAction3(new AMListActionInfo3("grating move", "grating move"));	// sequential by default.
		// first, move Z to 0
		gratingAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->z(), 0));
		// then move U,V,W to 0
		AMListAction3* uvw0Action = new AMListAction3(new AMListActionInfo3("grating UVW move to 0"), AMListAction3::Parallel);
		uvw0Action->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->u(), 0));
		uvw0Action->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->v(), 0));
		uvw0Action->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->w(), 0));
		gratingAction->addSubAction(uvw0Action);
		// then move R,S,T into position (Can do simultaneously with parallel action)
		AMListAction3* rstAction = new AMListAction3(new AMListActionInfo3("grating RST move"), AMListAction3::Parallel);
		rstAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->r(), moveSetpoint_.controlNamed("hexapodR").value()));
		rstAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->s(), moveSetpoint_.controlNamed("hexapodS").value()));
		rstAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->t(), moveSetpoint_.controlNamed("hexapodT").value()));
		gratingAction->addSubAction(rstAction);
		// move U,V,W to actual position
		AMListAction3* uvwAction = new AMListAction3(new AMListActionInfo3("grating UVW move"), AMListAction3::Parallel);
		uvwAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->u(), moveSetpoint_.controlNamed("hexapodU").value()));
		uvwAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->v(), moveSetpoint_.controlNamed("hexapodV").value()));
		uvwAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->w(), moveSetpoint_.controlNamed("hexapodW").value()));
		gratingAction->addSubAction(uvwAction);
		// then move X,Y,Z into position (can do simultaneously with parallel action)
		AMListAction3* xyzAction = new AMListAction3(new AMListActionInfo3("grating XYZ move"), AMListAction3::Parallel);
		xyzAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->x(), moveSetpoint_.controlNamed("hexapodX").value()));
		xyzAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->y(), moveSetpoint_.controlNamed("hexapodY").value()));
		xyzAction->addSubAction(AMActionSupport::buildControlMoveAction(hexapod_->z(), moveSetpoint_.controlNamed("hexapodZ").value()));
		gratingAction->addSubAction(xyzAction);

		moveAction_->addSubAction(gratingAction);
	}
	// add Lift, Tilt, and Translation
	moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(spectrometerRotationDrive_, moveSetpoint_.controlNamed("spectrometerRotationDrive").value()));
	moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(detectorTiltDrive_, moveSetpoint_.controlNamed("detectorTiltDrive").value()));
	moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(detectorTranslation_, moveSetpoint_.controlNamed("detectorTranslation").value()));
	// Disabled for now: moveAction_->addSubAction(new AMInternalControlMoveAction(endstationTranslation_, moveSetpoint_.controlNamed("endstationTranslation").value()))

	// Watch the move action: succeeded or failed (or cancelled)
	connect(moveAction_, SIGNAL(stateChanged(int,int)), this, SLOT(onMoveActionStateChanged(int,int)));
	emit moveStarted();
	moveAction_->start();
	return NoFailure;
}


bool REIXSSpectrometer::stop()
{
	if(!canStop())
		return false;

	if(moveInProgress()) {
		moveAction_->cancel();
		/// \todo Actually, have to flag that a stop has started, and also catch when the stop is finished... Motors will take a while to actually receive and decelerate.
		moveAction_->deleteLater();
		moveAction_ = 0;
		emit moveFailed(AMControl::WasStoppedFailure);
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMControl::WasStoppedFailure, "Spectrometer Move Stopped."));
	}

	// just in case anything was moving from outside our instructions (ie: commanded from somewhere else in the building)
	spectrometerRotationDrive_->stop();
	detectorTranslation_->stop();
	detectorTiltDrive_->stop();
	endstationTranslation_->stop();
	// hexapod: cannot stop without wrecking init. Don't worry for now... just let it stop over time. Not necessary for it to be not-moving before we re-send it somewhere new.

	/// \todo Actually, have to flag that a stop has started, and also catch when the stop is finished... Motors will take a while to actually receive and decelerate.

	return true;
}

bool REIXSSpectrometer::loadSpectrometerCalibration(AMDatabase *db, int databaseId)
{
	if(databaseId >= 1) {
		if(!calibration_.loadFromDb(db, databaseId))
			return false;
	}
	else {
		calibration_ = REIXSXESCalibration2();
	}

	// need to check grating indexes... Are they too big?
	if(currentGrating_ >= calibration_.gratingCount()) {
		currentGrating_ = -1;
	}
	if(specifiedGrating_ >= calibration_.gratingCount()) {
		specifiedGrating_ = 0;
	}

	emit calibrationChanged();
	return true;

}

void REIXSSpectrometer::specifyFocusOffset(double focusOffsetMm)
{
	specifiedFocusOffset_ = focusOffsetMm;
}

bool REIXSSpectrometer::specifyGrating(int gratingIndex)
{
	if(gratingIndex < 0 || gratingIndex >= gratingCount())
		return false;

	specifiedGrating_ = gratingIndex;
	return true;
}

void REIXSSpectrometer::updateGrating()
{
	qDebug() << "Updating Grating";
	QVector3D xyz = QVector3D(hexapod_->x()->value(), hexapod_->y()->value(), hexapod_->z()->value());
	QVector3D uvw = QVector3D(hexapod_->u()->value(), hexapod_->v()->value(), hexapod_->w()->value());
	QVector3D rst = QVector3D(hexapod_->r()->value(), hexapod_->s()->value(), hexapod_->t()->value());

	for (int i = calibration_.gratingCount() - 1; i >= 0; i--)
	{

		if (qFuzzyCompare(xyz, calibration_.hexapodXYZ(i)) &&
				qFuzzyCompare(uvw, calibration_.hexapodUVW(i)) &&
				qFuzzyCompare(rst, calibration_.hexapodRST(i)))
		{
			currentGrating_ = i;
			specifiedGrating_ = i;
		}
	}
}

void REIXSSpectrometer::specifyDetectorTiltOffset(double tiltOffsetDeg)
{
	specifiedDetectorTiltOffset_ = tiltOffsetDeg;
}

void REIXSSpectrometer::onConnected(bool isConnected){
	Q_UNUSED(isConnected)
	//figure out those values
	//onConnected not required
	updateGrating();
}

double REIXSSpectrometer::value() const
{
	if(currentGrating_ < 0 || currentGrating_ >= gratingCount())
		return -1.;

	return calibration_.computeEVFromSpectrometerPosition(currentGrating_, spectrometerRotationDrive_->value(), detectorTranslation_->value());
}

bool REIXSSpectrometer::gratingInPosition() const
{
	if(currentGrating_ < 0 || currentGrating_ >= gratingCount())
		return false;

	QVector3D xyz = calibration_.hexapodXYZ(currentGrating_);
	QVector3D rst = calibration_.hexapodRST(currentGrating_);
	QVector3D uvw = calibration_.hexapodUVW(currentGrating_);

	return hexapod_->x()->withinTolerance(xyz.x()) &&
			hexapod_->y()->withinTolerance(xyz.y()) &&
			hexapod_->z()->withinTolerance(xyz.z()) &&
			hexapod_->r()->withinTolerance(rst.x()) &&
			hexapod_->s()->withinTolerance(rst.y()) &&
			hexapod_->t()->withinTolerance(rst.z()) &&
			hexapod_->u()->withinTolerance(uvw.x()) &&
			hexapod_->v()->withinTolerance(uvw.y()) &&
			hexapod_->w()->withinTolerance(uvw.z());
}

void REIXSSpectrometer::onMoveActionStateChanged(int state, int previousState)
{
	Q_UNUSED(previousState)

	if(state == AMAction3::Succeeded || state == AMAction3::Failed || state == AMAction3::Cancelled) {
		moveAction_->deleteLater();	// cannot delete right away, because we might still be executing inside the moveAction_'s code.
		moveAction_ = 0;
		if(state == AMAction3::Succeeded) {
			emit moveSucceeded();
		}
		else if(state == AMAction3::Failed) {
			emit moveFailed(AMControl::OtherFailure);
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMControl::OtherFailure, "Spectrometer Move Failed."));
		}
		// cancelled: handled previously in stop().
	}
}



REIXSBrokenMonoControl::REIXSBrokenMonoControl(AMPVwStatusControl *underlyingControl, double repeatMoveThreshold, int repeatMoveAttempts, double repeatMoveSettlingTime, double singleMoveSettlingTime, double lowEnergyThreshold, double lowEnergyStepSize, double actualTolerance, QObject *parent) : AMControl("beamlineEV", "eV", parent, "Beamline Energy")
{
	control_ = underlyingControl;
	repeatMoveThreshold_ = repeatMoveThreshold;
	repeatMoveAttempts_ = repeatMoveAttempts;
	repeatMoveSettlingTime_ = repeatMoveSettlingTime;
	singleMoveSettlingTime_ = singleMoveSettlingTime;
	lowEnergyThreshold_ = lowEnergyThreshold;
	lowEnergyStepSize_ = lowEnergyStepSize;

	stopInProgress_ = false;

	// Set a (very) wide tolerance on the control_ so that all of its moves succeed, even if it doesn't make it to where it's supposed to go. That's what we're here for.
	control_->setTolerance(1000);
	// But we can have a tighter tolerance
	setTolerance(actualTolerance);

	setDisplayPrecision(3);

	moveAction_ = 0;

	// connect signals from control_ so we can emit correctly. Direct forwards:
	connect(control_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(control_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(control_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)));

	// require some parsing on our part:
	connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onControlMovingChanged(bool)));


}

void REIXSBrokenMonoControl::onControlMovingChanged(bool)
{
	bool nowMoving = isMoving();
	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);
}



AMControl::FailureExplanation REIXSBrokenMonoControl::move(double setpoint)
{
	if(moveAction_) {
		return AMControl::AlreadyMovingFailure;
	}

	if(!isConnected())
		return AMControl::NotConnectedFailure;

	setpoint_ = setpoint;
	stopInProgress_ = false;
	moveAction_ = new AMListAction3(new AMListActionInfo3("REIXS mono move", "REIXS mono move"));

	//NO MOVE REQUIRED
	if (fabs(value() - setpoint_) < 0.0025)  //naughty arbitrary magic number for now
	{
		onMoveActionSucceeded();
		return AMControl::NoFailure;
	}

	//REPEAT MOVE REQUIRED
	else if(fabs(value() - setpoint_) > repeatMoveThreshold_)
	{
			control_->setSettlingTime(repeatMoveSettlingTime_);	// ensures we wait for this long before finishing each sub-move.
			double movePoint_ = value();

			if(setpoint_ > lowEnergyThreshold_ && value() < lowEnergyThreshold_) { //below lowEnergySetpoint moving up

				while(movePoint_ < lowEnergyThreshold_) {
					movePoint_ = movePoint_ + lowEnergyStepSize_;
					moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, movePoint_));
				}


			}

			else if(setpoint_ <= lowEnergyThreshold_ && value() > lowEnergyThreshold_) {   //above lowEnergySetpoint moving into

				movePoint_ = lowEnergyThreshold_;

				for(int i=0; i<repeatMoveAttempts_; ++i) {
					moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, movePoint_));
				}

				while(movePoint_ - setpoint_ > lowEnergyStepSize_) {
					movePoint_ = movePoint_ - lowEnergyStepSize_;
					moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, movePoint_));
				}


			}

			else if(setpoint_ <= lowEnergyThreshold_ && value() <= lowEnergyThreshold_ && setpoint_ > value() ) {   //below lowEnergySetpoint moving up within


				while(setpoint_ - movePoint_ > lowEnergyStepSize_) {
					movePoint_ = movePoint_ + lowEnergyStepSize_;
					moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, movePoint_));
				}

			}

			else if(setpoint_ <= lowEnergyThreshold_ && value() <= lowEnergyThreshold_ && setpoint_ < value() ) {   //below lowEnergySetpoint moving down within


				while(movePoint_ - setpoint_ > lowEnergyStepSize_) {
					movePoint_ = movePoint_ - lowEnergyStepSize_;
					moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, movePoint_));
				}

			}

			//Fall though or finalize move:

			for(int i=0; i<repeatMoveAttempts_; ++i) {
				moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, setpoint_));
			}



	}
	//SMALL MOVE, JUST DO IT
	else {
		control_->setSettlingTime(0);
		setTolerance(AMCONTROL_TOLERANCE_DONT_CARE);
		moveAction_->addSubAction(AMActionSupport::buildControlMoveAction(control_, setpoint_));
	}

	/// \todo Low-energy moves
	/// \todo Single-move settling time?

	connect(moveAction_, SIGNAL(failed()), this, SLOT(onMoveActionFailed()));
	connect(moveAction_, SIGNAL(cancelled()), this, SLOT(onMoveActionFailed()));
	connect(moveAction_, SIGNAL(succeeded()), this, SLOT(onMoveActionSucceeded()));

	/// monitor mono angles for error state, clear and restart move if detected
	connect(REIXSBeamline::bl()->photonSource()->monoMirrorAngleStatus(), SIGNAL(valueChanged(double)), this, SLOT(onMonoAngleError(double)));
	connect(REIXSBeamline::bl()->photonSource()->monoGratingAngleStatus(), SIGNAL(valueChanged(double)), this, SLOT(onMonoAngleError(double)));


	moveAction_->start();

	return AMControl::NoFailure;
}

bool REIXSBrokenMonoControl::stop()
{
	if(moveAction_) {
		stopInProgress_ = true;
		moveAction_->cancel();
	}
	bool success = control_->stop();
	QTimer *repeat = new QTimer(this);
	connect(repeat, SIGNAL(timeout()), control_, SLOT(stop()));
	connect(control_,SIGNAL(movingChanged(bool)),repeat,SLOT(stop()));

	if(control_->movingPVValue()==1)
		repeat->start(250);


	return success;
}

void REIXSBrokenMonoControl::onMoveActionFailed()
{
	disconnect(moveAction_, 0, this, 0);
	disconnect(REIXSBeamline::bl()->photonSource()->monoMirrorAngleStatus(),0, this, 0);
	disconnect(REIXSBeamline::bl()->photonSource()->monoGratingAngleStatus(),0, this, 0);

	moveAction_->deleteLater();
	moveAction_ = 0;

	bool nowMoving = isMoving();
	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);

	// was cancelled or failed.  If we were supposed to be stopping:
	if(stopInProgress_) {
		emit moveFailed(AMControl::WasStoppedFailure);
	}
	else {
		emit moveFailed(AMControl::OtherFailure);
	}

	stopInProgress_ = false;
}

void REIXSBrokenMonoControl::onMoveActionSucceeded()
{
	disconnect(moveAction_, 0, this, 0);
	disconnect(REIXSBeamline::bl()->photonSource()->monoMirrorAngleStatus(),0, this, 0);
	disconnect(REIXSBeamline::bl()->photonSource()->monoGratingAngleStatus(),0, this, 0);

	moveAction_->deleteLater();
	moveAction_ = 0;

	bool nowMoving = isMoving();
	if(nowMoving != wasMoving_)
		emit movingChanged(wasMoving_ = nowMoving);

	// check tolerance:
	if(inPosition()) {
		emit moveSucceeded();
	}
	else {
		emit moveFailed(AMControl::ToleranceFailure);
	}

	stopInProgress_ = false;
}

void REIXSBrokenMonoControl::onMonoAngleError(double error)
{
	if(qFuzzyCompare(error,4)){
		AMErrorMon::information(this,0,"The mono mirror or grating angle move stalled, Acquaman has caught and corrected the problem. No user action required.");
		REIXSBeamline::bl()->photonSource()->bypassEnergy()->stop();
		REIXSBeamline::bl()->photonSource()->bypassEnergy()->move(REIXSBeamline::bl()->photonSource()->directEnergy()->setpoint());
	}
}



REIXSBrokenMonoControl::~REIXSBrokenMonoControl() {
	control_->deleteLater();
	control_ = 0;
}

// To have a current sample in position, there must be a marked sample on the beamline's current plate, which has four positions set, and the sample manipulator is within tolerance of the X,Y,Z position. (We ignore theta, to allow different incident angles to all count as the same sample. Only works if the sample is at the center of rotation of the plate, unfortunately.)
int REIXSBeamline::currentSampleId()
{
	if(samplePlate_->id() < 1)
		return -1;

	// loop through all samples on the current plate.
	for(int i=0; i<samplePlate_->count(); ++i) {
		const AMSamplePositionPre2013& samplePos = samplePlate_->at(i);

		if(samplePos.position().count() != 4)
			continue;

		if(!sampleChamber()->x()->withinTolerance(samplePos.position().at(0).value()))
			continue;
		if(!sampleChamber()->y()->withinTolerance(samplePos.position().at(1).value()))
			continue;
		if(!sampleChamber()->z()->withinTolerance(samplePos.position().at(2).value()))
			continue;

		// it's good!
		return samplePos.sampleId();
	}

	return -1;
}

int REIXSBeamline::currentSamplePlateId() const
{
	return samplePlate_->id();
}

