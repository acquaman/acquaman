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

#include "REIXSBeamline.h"
#include "util/AMErrorMonitor.h"

REIXSBeamline::REIXSBeamline() :
	AMBeamline("REIXSBeamline")
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
	mcpDetector_ = new REIXSXESMCPDetector("xesImage", "CPD1610-01", this);


	// Build a control set of all the controls we want to make available to REIXSControlMoveAction, as well as record in the scan's scanInitialConditions()
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
	allControlsSet_->addControl(spectrometer()->endstationTranslation());  //DAVID ADDED

}


REIXSBeamline::~REIXSBeamline() {
}


REIXSSampleChamber::REIXSSampleChamber(QObject *parent)
	: AMCompositeControl("sampleChamber", "n/a", parent) {

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
	x_ = new CLSMDriveMotorControl("sampleX", "SMTR1610-4-I21-08", "mm", 2.116, 0, 256, "Sample Chamber X", 0.5, 1.0, this);
	x_->setSettlingTime(0.1);

	y_ = new CLSMDriveMotorControl("sampleY", "SMTR1610-4-I21-10", "mm", 2.116, 0, 256, "Sample Chamber Y", 0.5, 1.0, this);
	y_->setSettlingTime(0.1);

	z_ = new CLSMDriveMotorControl("sampleZ", "SMTR1610-4-I21-07", "mm", 0.25, 0, 256, "Sample Chamber Z", 0.5, 1.0, this);
	z_->setSettlingTime(0.1);

	r_ = new CLSMDriveMotorControl("sampleTheta", "SMTR1610-4-I21-11", "deg", 3.6, 0, 256, "Sample Chamber Theta", 0.5, 1.0, this);
	r_->setSettlingTime(0.1);

	loadLockZ_ = new CLSMDriveMotorControl("loadLockZ", "SMTR1610-4-I21-09", "mm", 0.125, 0, 256, "Load Lock Z", 0.5, 1.0, this);
	loadLockZ_->setSettlingTime(0.1);

	loadLockR_ = new CLSMDriveMotorControl("loadLockTheta", "SMTR1610-4-I21-12", "deg", 3.6, 0, 256, "Load Lock Theta", 0.5, 1.0, this);
	loadLockR_->setSettlingTime(0.1);

	addChildControl(x_);
	addChildControl(y_);
	addChildControl(z_);
	addChildControl(r_);
	addChildControl(loadLockZ_);
	addChildControl(loadLockR_);
}


REIXSHexapod::REIXSHexapod(QObject* parent)
	: AMCompositeControl("hexapod", "n/a", parent) {

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

REIXSSpectrometer::REIXSSpectrometer(QObject *parent)
	: AMCompositeControl("spectrometer", "eV", parent) {

	setDescription("XES Detector Energy");

	spectrometerRotationDrive_ = new AMPVwStatusControl("spectrometerRotationDrive",
														"SMTR1610-4-I21-01:mm:fbk",
														"SMTR1610-4-I21-01:mm",
														"SMTR1610-4-I21-01:status",
														"SMTR1610-4-I21-01:stop", this, 0.05);
	spectrometerRotationDrive_->setDescription("XES Spectrometer Lift");
	spectrometerRotationDrive_->setSettlingTime(0.1);



	detectorTranslation_ = new AMPVwStatusControl("detectorTranslation",
												  "SMTR1610-4-I21-04:mm:fbk",
												  "SMTR1610-4-I21-04:mm",
												  "SMTR1610-4-I21-04:status",
												  "SMTR1610-4-I21-04:stop", this, 0.05);

	detectorTranslation_->setDescription("XES Detector Translation");
	detectorTranslation_->setSettlingTime(0.1);

	detectorTiltDrive_ = new AMPVwStatusControl("detectorTiltDrive",
												"SMTR1610-4-I21-02:mm:sp",
												"SMTR1610-4-I21-02:mm",
												"SMTR1610-4-I21-02:status",
												"SMTR1610-4-I21-02:stop", this, 0.05);
	detectorTiltDrive_->setDescription("XES Detector Tilt Stage");
	detectorTiltDrive_->setSettlingTime(0.1);

	endstationTranslation_ = new AMPVwStatusControl("endstationTranslation",
														"SMTR1610-4-I21-05:mm:fbk",
														"SMTR1610-4-I21-05:mm",
														"SMTR1610-4-I21-05:status",
														"SMTR1610-4-I21-05:stop", this, 0.05);  //DAVID ADDED
	endstationTranslation_->setDescription("Endstation Translation");
	endstationTranslation_->setSettlingTime(0.1);

	hexapod_ = new REIXSHexapod(this);

	addChildControl(spectrometerRotationDrive_);
	addChildControl(detectorTranslation_);
	addChildControl(detectorTiltDrive_);
	addChildControl(endstationTranslation_);  //DAVID ADDED
	addChildControl(hexapod_);

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



}







#include "actions2/AMListAction.h"
#include "actions2/actions/AMInternalControlMoveAction.h"

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
	moveAction_ = new AMListAction(new AMActionInfo("spectrometer eV move"), AMListAction::ParallelMode, this);
	// if we need to move the grating into position, add actions for that:
	if(!gratingInPosition()) {
		// make a sequential action for the grating moves
		AMListAction* gratingAction = new AMListAction(new AMActionInfo("grating move"));	// sequential by default.
		// first, move Z to 0
		gratingAction->addSubAction(new AMInternalControlMoveAction(hexapod_->z(), 0));
		// then move U,V,W to 0
		AMListAction* uvw0Action = new AMListAction(new AMActionInfo("grating UVW move to 0"), AMListAction::ParallelMode);
		uvw0Action->addSubAction(new AMInternalControlMoveAction(hexapod_->u(), 0));
		uvw0Action->addSubAction(new AMInternalControlMoveAction(hexapod_->v(), 0));
		uvw0Action->addSubAction(new AMInternalControlMoveAction(hexapod_->w(), 0));
		gratingAction->addSubAction(uvw0Action);
		// then move R,S,T into position (Can do simultaneously with parallel action)
		AMListAction* rstAction = new AMListAction(new AMActionInfo("grating RST move"), AMListAction::ParallelMode);
		rstAction->addSubAction(new AMInternalControlMoveAction(hexapod_->r(), moveSetpoint_.controlNamed("hexapodR").value()));
		rstAction->addSubAction(new AMInternalControlMoveAction(hexapod_->s(), moveSetpoint_.controlNamed("hexapodS").value()));
		rstAction->addSubAction(new AMInternalControlMoveAction(hexapod_->t(), moveSetpoint_.controlNamed("hexapodT").value()));
		gratingAction->addSubAction(rstAction);
		// move U,V,W to actual position
		AMListAction* uvwAction = new AMListAction(new AMActionInfo("grating UVW move"), AMListAction::ParallelMode);
		uvwAction->addSubAction(new AMInternalControlMoveAction(hexapod_->u(), moveSetpoint_.controlNamed("hexapodU").value()));
		uvwAction->addSubAction(new AMInternalControlMoveAction(hexapod_->v(), moveSetpoint_.controlNamed("hexapodV").value()));
		uvwAction->addSubAction(new AMInternalControlMoveAction(hexapod_->w(), moveSetpoint_.controlNamed("hexapodW").value()));
		gratingAction->addSubAction(uvwAction);
		// then move X,Y,Z into position (can do simultaneously with parallel action)
		AMListAction* xyzAction = new AMListAction(new AMActionInfo("grating XYZ move"), AMListAction::ParallelMode);
		xyzAction->addSubAction(new AMInternalControlMoveAction(hexapod_->x(), moveSetpoint_.controlNamed("hexapodX").value()));
		xyzAction->addSubAction(new AMInternalControlMoveAction(hexapod_->y(), moveSetpoint_.controlNamed("hexapodY").value()));
		xyzAction->addSubAction(new AMInternalControlMoveAction(hexapod_->z(), moveSetpoint_.controlNamed("hexapodZ").value()));
		gratingAction->addSubAction(xyzAction);

		moveAction_->addSubAction(gratingAction);
	}
	// add Lift, Tilt, and Translation
	moveAction_->addSubAction(new AMInternalControlMoveAction(spectrometerRotationDrive_, moveSetpoint_.controlNamed("spectrometerRotationDrive").value()));
	moveAction_->addSubAction(new AMInternalControlMoveAction(detectorTiltDrive_, moveSetpoint_.controlNamed("detectorTiltDrive").value()));
	moveAction_->addSubAction(new AMInternalControlMoveAction(detectorTranslation_, moveSetpoint_.controlNamed("detectorTranslation").value()));
	// Disabled for now: moveAction_->addSubAction(new AMInternalControlMoveAction(endstationTranslation_, moveSetpoint_.controlNamed("endstationTranslation").value()));

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
		delete moveAction_;
		moveAction_ = 0;
		emit moveFailed(AMControl::WasStoppedFailure);
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

void REIXSSpectrometer::specifyDetectorTiltOffset(double tiltOffsetDeg)
{
	specifiedDetectorTiltOffset_ = tiltOffsetDeg;
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

	if(state == AMAction::Succeeded || state == AMAction::Failed || state == AMAction::Cancelled) {
		moveAction_->deleteLater();	// cannot delete right away, because we might still be executing inside the moveAction_'s code.
		moveAction_ = 0;
		if(state == AMAction::Succeeded)
			emit moveSucceeded();
		else if(state == AMAction::Failed)
			emit moveFailed(AMControl::OtherFailure);
		// cancelled: handled previously in stop().
	}
}

REIXSPhotonSource::REIXSPhotonSource(QObject *parent) :
	AMCompositeControl("photonSource", "n/a", parent, "EPU and Monochromator")
{
	energy_ = new AMPVwStatusControl("beamlineEV", "REIXS:MONO1610-I20-01:energy:fbk", "REIXS:energy", "REIXS:status", "REIXS:energy:stop", this, 0.01);
	energy_->setDescription("Beamline Energy");

	monoSlit_ = new AMPVwStatusAndUnitConversionControl("monoSlit", "SMTR1610-I20-10:mm:fbk", "SMTR1610-I20-10:mm", "SMTR1610-I20-10:status", "SMTR1610-I20-10:stop", new AMScaleAndOffsetUnitConverter("um", 1000), 0, this, 0.1);
	monoSlit_->setDescription("Mono Slit Width");

	monoGratingTranslation_ = new AMPVwStatusControl("monoGratingTranslation", "MONO1610-I20-01:grating:trans:mm:fbk", "MONO1610-I20-01:grating:trans:mm", "MONO1610-I20-01:grating:trans:status", "SMTR1610-I20-04:stop", this, 0.05);
	monoGratingTranslation_->setDescription("Mono Grating Translation");
	monoGratingSelector_ = new AMPVwStatusControl("monoGratingSelector", "MONO1610-I20-01:grating:select:fbk", "MONO1610-I20-01:grating:select", "MONO1610-I20-01:grating:trans:status", "SMTR1610-I20-04:stop", this, 1);
	monoGratingSelector_->setDescription("Mono Grating");

	monoMirrorTranslation_ = new AMPVwStatusControl("monoMirrorTranslation", "MONO1610-I20-01:mirror:trans:mm:fbk", "MONO1610-I20-01:mirror:trans:mm", "MONO1610-I20-01:mirror:trans:status", "SMTR1610-I20-02:stop", this, 0.05);
	monoMirrorTranslation_->setDescription("Mono Mirror Translation");
	monoMirrorSelector_ = new AMPVwStatusControl("monoMirrorSelector", "MONO1610-I20-01:mirror:select:fbk", "MONO1610-I20-01:mirror:select", "MONO1610-I20-01:mirror:trans:status", "SMTR1610-I20-02:stop", this, 1);
	monoMirrorSelector_->setDescription("Mono Mirror");
}

REIXSValvesAndShutters::REIXSValvesAndShutters(QObject *parent) : AMCompositeControl("valvesAndShutters", "n/a", parent)
{
	beamIsOn_ = false;

	ssh1_ = new CLSBiStateControl("safetyShutter1", "Safety Shutter 1", "SSH1410-I00-01:state", "SSH1410-I00-01:opr:open", "SSH1410-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	psh2_ = new CLSBiStateControl("photonShutter2", "Photon Shutter 2", "PSH1410-I00-02:state", "PSH1410-I00-02:opr:open", "PSH1410-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);

	psh4_ = new CLSBiStateControl("photonShutter4", "Photon Shutter 4", "PSH1610-I20-01:state", "PSH1610-I20-01:opr:open", "PSH1610-I20-01:opr:close", new AMControlStatusCheckerDefault(2), this);

	endstationValve_ = new CLSBiStateControl("XESendstationValve", "XES Endstation Valve", "VVR1610-4-I21-01:state", "VVR1610-4-I21-01:opr:open", "VVR1610-4-I21-01:opr:close", new AMControlStatusCheckerDefault(2), this);

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
			ssh1_->state() == 1 &&
			psh2_->state() == 1 &&
			psh4_->state() == 1;

	if(beamIsOn_ != beamWasOn)
		emit beamOnChanged(beamIsOn_);
}





