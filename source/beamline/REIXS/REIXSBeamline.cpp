/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "beamline/CLS/CLSMDriveMotorControl.h"
#include "util/AMErrorMonitor.h"

REIXSBeamline::REIXSBeamline() :
	AMBeamline("REIXSBeamline")
{
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

}



REIXSSampleChamber::REIXSSampleChamber(QObject *parent)
	: AMControl("sampleChamber", "n/a", parent) {

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
	y_ = new CLSMDriveMotorControl("sampleY", "SMTR1610-4-I21-10", "mm", 2.116, 0, 256, "Sample Chamber Y", 0.5, 1.0, this);
	z_ = new CLSMDriveMotorControl("sampleZ", "SMTR1610-4-I21-07", "mm", 0.25, 0, 256, "Sample Chamber Z", 0.5, 1.0, this);
	r_ = new CLSMDriveMotorControl("sampleTheta", "SMTR1610-4-I21-11", "deg", 3.6, 0, 256, "Sample Chamber Theta", 0.5, 1.0, this);
	loadLockZ_ = new CLSMDriveMotorControl("loadLockZ", "SMTR1610-4-I21-09", "mm", 0.125, 0, 256, "Load Lock Z", 0.5, 1.0, this);
	loadLockR_ = new CLSMDriveMotorControl("loadLockTheta", "SMTR1610-4-I21-12", "deg", 3.6, 0, 256, "Load Lock Theta", 0.5, 1.0, this);
}


REIXSHexapod::REIXSHexapod(QObject* parent)
	: AMCompositeControl("hexapod", "n/a", parent) {

	QString baseName = "HXPD1610-401-01:";
	x_ = new AMPVwStatusControl("hexapodX", baseName+"X:sp", baseName+"X", baseName+"moving", QString(), this, 0.01);
	y_ = new AMPVwStatusControl("hexapodY", baseName+"Y:sp", baseName+"Y", baseName+"moving", QString(), this, 0.01);
	z_ = new AMPVwStatusControl("hexapodZ", baseName+"Z:sp", baseName+"Z", baseName+"moving", QString(), this, 0.01);
	u_ = new AMPVwStatusControl("hexapodU", baseName+"U:sp", baseName+"U", baseName+"moving", QString(), this, 0.05);
	v_ = new AMPVwStatusControl("hexapodV", baseName+"V:sp", baseName+"V", baseName+"moving", QString(), this, 0.05);
	w_ = new AMPVwStatusControl("hexapodW", baseName+"W:sp", baseName+"W", baseName+"moving", QString(), this, 0.05);
	r_ = new AMPVControl("hexapodR", baseName+"R:sp", baseName+"R", QString(), this, 0.01);
	s_ = new AMPVControl("hexapodS", baseName+"S:sp", baseName+"S", QString(), this, 0.01);
	t_ = new AMPVControl("hexapodT", baseName+"T:sp", baseName+"T", QString(), this, 0.01);

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

	spectrometerRotationDrive_ = new AMPVwStatusControl("spectrometerRotationDrive",
														"SMTR1610-4-I21-01:mm:sp",
														"SMTR1610-4-I21-01:mm",
														"SMTR1610-4-I21-01:status",
														"SMTR1610-4-I21-01:stop", this, 1);

	detectorTranslation_ = new AMPVwStatusControl("detectorTranslation",
												  "SMTR1610-4-I21-04:mm:sp",
												  "SMTR1610-4-I21-04:mm",
												  "SMTR1610-4-I21-04:status",
												  "SMTR1610-4-I21-04:stop", this, 1);

	detectorTiltDrive_ = new AMPVwStatusControl("detectorTiltDrive",
												"SMTR1610-4-I21-02:mm:sp",
												"SMTR1610-4-I21-02:mm",
												"SMTR1610-4-I21-02:status",
												"SMTR1610-4-I21-02:stop", this, 0.5);


	hexapod_ = new REIXSHexapod(this);

	addChildControl(spectrometerRotationDrive_);
	addChildControl(detectorTranslation_);
	addChildControl(detectorTiltDrive_);
	addChildControl(hexapod_);

	currentGrating_ = -1; specifiedGrating_ = 0;
	currentFocusOffset_ = 0; specifiedFocusOffset_ = 0;
	currentDetectorTiltOffset_ = 0; specifiedDetectorTiltOffset_ = 0;

	specifiedEV_ = 395;

	currentMoveStep_ = MoveDone;
	setTolerance(0.1);

	// valueChanged(): if the optical origin is at the rotation point and everything is perfect, then only the spectrometerRotationDrive_ motor will affect the energy value.  But in the non-perfect-aligned general math situation, the translation can also affect eV.  And of course gratings...
	// Here we make the connections to get our valueChanged() signal:
	connect(spectrometerRotationDrive_, SIGNAL(valueChanged(double)), this, SLOT(scheduleReviewValueChanged()));
	connect(detectorTranslation_, SIGNAL(valueChanged(double)), this, SLOT(scheduleReviewValueChanged()));
	connect(this, SIGNAL(gratingChanged(int)), this, SLOT(scheduleReviewValueChanged()));

	connect(&reviewValueChangedFunction_, SIGNAL(executed()), this, SLOT(reviewValueChanged()));



}








void REIXSSpectrometer::move(double setpoint)
{
	if(!isConnected()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 2, "Can't move the spectrometer: some motor controls are not connected. Check that the IOCs are running and the network connections are good."));
		emit moveFailed(AMControl::NotConnectedFailure);
		return;
	}

	// can't start a move while moving
	if(moveInProgress() || isMoving()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, 2, "Can't move the spectrometer, because it's already moving.  Stop the spectrometer first before attempting another move."));
		emit moveFailed(AMControl::OtherFailure);
		return;
	}

	specifiedEV_ = setpoint;
	currentFocusOffset_ = specifiedFocusOffset_;
	currentDetectorTiltOffset_ = specifiedDetectorTiltOffset_;

	if(currentGrating_ != specifiedGrating_) {
		emit gratingChanged(currentGrating_ = specifiedGrating_);
	}

	currentMoveStep_ = Starting;
	/// \todo limits? out of range?
	moveSetpoint_ = calibration_.computeSpectrometerPosition(specifiedGrating_, specifiedEV_, currentFocusOffset_, currentDetectorTiltOffset_);
	emit moveStarted();

	if(!gratingInPosition()) {
		smStartMoveZto0();
	}
	else
		smStartMoveLiftTiltTranslation();

}


void REIXSSpectrometer::smStartMoveZto0()
{
	qDebug() << "Moving Z to 0";
	currentMoveStep_ = MovingZto0;
	connect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smMoveZto0Succeeded()));
	connect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smMoveZto0Failed(int)));

	hexapod_->z()->move(0);
}

void REIXSSpectrometer::smMoveZto0Succeeded()
{
	qDebug() << "Moving Z to 0: success!";
	disconnect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smMoveZto0Succeeded()));
	disconnect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smMoveZto0Failed(int)));

	smStartMoveUto0();
}

void REIXSSpectrometer::smMoveZto0Failed(int reason)
{
	disconnect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smMoveZto0Succeeded()));
	disconnect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smMoveZto0Failed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod Z to 0"));
}

void REIXSSpectrometer::smStartMoveUto0()
{
	qDebug() << "Moving U to 0";
	currentMoveStep_ = MovingUto0;

	connect(hexapod_->u(), SIGNAL(moveSucceeded()), this, SLOT(smMoveUto0Succeeded()));
	connect(hexapod_->u(), SIGNAL(moveFailed(int)), this, SLOT(smMoveUto0Failed(int)));

	hexapod_->u()->move(0);
}

void REIXSSpectrometer::smMoveUto0Succeeded()
{
	qDebug() << "Moving Z to 0: success!";

	disconnect(hexapod_->u(), SIGNAL(moveSucceeded()), this, SLOT(smMoveUto0Succeeded()));
	disconnect(hexapod_->u(), SIGNAL(moveFailed(int)), this, SLOT(smMoveUto0Failed(int)));

	smStartMoveRST();
}

void REIXSSpectrometer::smMoveUto0Failed(int reason)
{
	disconnect(hexapod_->u(), SIGNAL(moveSucceeded()), this, SLOT(smMoveUto0Succeeded()));
	disconnect(hexapod_->u(), SIGNAL(moveFailed(int)), this, SLOT(smMoveUto0Failed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, QString("Spectrometer move: failed while trying to move Hexapod U into position. Target: %1.  Reached: %2").arg(hexapod_->u()->setpoint()).arg(hexapod_->u()->value())));
}

void REIXSSpectrometer::smStartMoveRST()
{
	qDebug() << "Moving RST to pos";

	currentMoveStep_ = MovingRST;

	smRMoveDone_ = smSMoveDone_ = smTMoveDone_ = false;
	connect(hexapod_->r(), SIGNAL(moveSucceeded()), this, SLOT(smRMoveSucceeded()));
	connect(hexapod_->r(), SIGNAL(moveFailed(int)), this, SLOT(smRMoveFailed(int)));
	connect(hexapod_->s(), SIGNAL(moveSucceeded()), this, SLOT(smSMoveSucceeded()));
	connect(hexapod_->s(), SIGNAL(moveFailed(int)), this, SLOT(smSMoveFailed(int)));
	connect(hexapod_->t(), SIGNAL(moveSucceeded()), this, SLOT(smTMoveSucceeded()));
	connect(hexapod_->t(), SIGNAL(moveFailed(int)), this, SLOT(smTMoveFailed(int)));

	hexapod_->r()->move(moveSetpoint_.controlNamed("hexapodR").value());
	hexapod_->s()->move(moveSetpoint_.controlNamed("hexapodS").value());
	hexapod_->t()->move(moveSetpoint_.controlNamed("hexapodT").value());
}



void REIXSSpectrometer::smStartMoveU()
{
	qDebug() << "Moving U to pos";

	currentMoveStep_ = MovingU;

	connect(hexapod_->u(), SIGNAL(moveSucceeded()), this, SLOT(smMoveUSucceeded()));
	connect(hexapod_->u(), SIGNAL(moveFailed(int)), this, SLOT(smMoveUFailed(int)));

	hexapod_->u()->move(moveSetpoint_.controlNamed("hexapodU").value());
}

void REIXSSpectrometer::smMoveUSucceeded()
{
	qDebug() << "Moving U to pos: Success!";

	disconnect(hexapod_->u(), SIGNAL(moveSucceeded()), this, SLOT(smMoveUSucceeded()));
	disconnect(hexapod_->u(), SIGNAL(moveFailed(int)), this, SLOT(smMoveUFailed(int)));

	smStartMoveXYZ();
}

void REIXSSpectrometer::smMoveUFailed(int reason)
{
	disconnect(hexapod_->u(), SIGNAL(moveSucceeded()), this, SLOT(smMoveUSucceeded()));
	disconnect(hexapod_->u(), SIGNAL(moveFailed(int)), this, SLOT(smMoveUFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, QString("Spectrometer move: failed while trying to move Hexapod U into position. Target: %1.  Reached: %2").arg(hexapod_->u()->setpoint()).arg(hexapod_->u()->value())));
}

void REIXSSpectrometer::smStartMoveXYZ()
{
	qDebug() << "Moving XYZ to pos";

	currentMoveStep_ = MovingXYZ;
	smXMoveDone_ = smYMoveDone_ = smZMoveDone_ = false;

	connect(hexapod_->x(), SIGNAL(moveSucceeded()), this, SLOT(smXMoveSucceeded()));
	connect(hexapod_->x(), SIGNAL(moveFailed(int)), this, SLOT(smXMoveFailed(int)));
	connect(hexapod_->y(), SIGNAL(moveSucceeded()), this, SLOT(smYMoveSucceeded()));
	connect(hexapod_->y(), SIGNAL(moveFailed(int)), this, SLOT(smYMoveFailed(int)));
	connect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smZMoveSucceeded()));
	connect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smZMoveFailed(int)));

	hexapod_->x()->move(moveSetpoint_.controlNamed("hexapodX").value());
	hexapod_->y()->move(moveSetpoint_.controlNamed("hexapodY").value());
	hexapod_->z()->move(moveSetpoint_.controlNamed("hexapodZ").value());
}



void REIXSSpectrometer::smStartMoveLiftTiltTranslation()
{
	qDebug() << "Moving Lift, Tilt, Translation to pos";

	currentMoveStep_ = MovingLiftTiltTranslation;
	smLiftMoveDone_ = smTiltMoveDone_ = smTranslationMoveDone_ = false;

	connect(spectrometerRotationDrive_, SIGNAL(moveSucceeded()), this, SLOT(smLiftMoveSucceeded()));
	connect(spectrometerRotationDrive_, SIGNAL(moveFailed(int)), this, SLOT(smLiftMoveFailed(int)));
	connect(detectorTiltDrive_, SIGNAL(moveSucceeded()), this, SLOT(smTiltMoveSucceeded()));
	connect(detectorTiltDrive_, SIGNAL(moveFailed(int)), this, SLOT(smTiltMoveFailed(int)));
	connect(detectorTranslation_, SIGNAL(moveSucceeded()), this, SLOT(smTranslationMoveSucceeded()));
	connect(detectorTranslation_, SIGNAL(moveFailed(int)), this, SLOT(smTranslationMoveFailed(int)));


	spectrometerRotationDrive_->move(moveSetpoint_.controlNamed("spectrometerRotationDrive").value());
	detectorTiltDrive_->move(moveSetpoint_.controlNamed("detectorTiltDrive").value());
	detectorTranslation_->move(moveSetpoint_.controlNamed("detectorTranslation").value());

}



void REIXSSpectrometer::smRMoveSucceeded()
{
	qDebug() << "Moving R to pos: success!";

	disconnect(hexapod_->r(), SIGNAL(moveSucceeded()), this, SLOT(smRMoveSucceeded()));
	disconnect(hexapod_->r(), SIGNAL(moveFailed(int)), this, SLOT(smRMoveFailed(int)));

	smRMoveDone_ = true;
	if(smRMoveDone_ && smSMoveDone_ && smTMoveDone_)
		smStartMoveU();
}

void REIXSSpectrometer::smSMoveSucceeded()
{
	qDebug() << "Moving S to pos: success!";

	disconnect(hexapod_->s(), SIGNAL(moveSucceeded()), this, SLOT(smSMoveSucceeded()));
	disconnect(hexapod_->s(), SIGNAL(moveFailed(int)), this, SLOT(smSMoveFailed(int)));

	smSMoveDone_ = true;
	if(smRMoveDone_ && smSMoveDone_ && smTMoveDone_)
		smStartMoveU();

}

void REIXSSpectrometer::smTMoveSucceeded()
{
	qDebug() << "Moving T to pos: success!";

	disconnect(hexapod_->t(), SIGNAL(moveSucceeded()), this, SLOT(smTMoveSucceeded()));
	disconnect(hexapod_->t(), SIGNAL(moveFailed(int)), this, SLOT(smTMoveFailed(int)));

	smTMoveDone_ = true;
	if(smRMoveDone_ && smSMoveDone_ && smTMoveDone_)
		smStartMoveU();
}

void REIXSSpectrometer::smRMoveFailed(int reason)
{
	disconnect(hexapod_->r(), SIGNAL(moveSucceeded()), this, SLOT(smRMoveSucceeded()));
	disconnect(hexapod_->r(), SIGNAL(moveFailed(int)), this, SLOT(smRMoveFailed(int)));
	disconnect(hexapod_->s(), SIGNAL(moveSucceeded()), this, SLOT(smSMoveSucceeded()));
	disconnect(hexapod_->s(), SIGNAL(moveFailed(int)), this, SLOT(smSMoveFailed(int)));
	disconnect(hexapod_->t(), SIGNAL(moveSucceeded()), this, SLOT(smTMoveSucceeded()));
	disconnect(hexapod_->t(), SIGNAL(moveFailed(int)), this, SLOT(smTMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod R into position."));
}

void REIXSSpectrometer::smSMoveFailed(int reason)
{
	disconnect(hexapod_->r(), SIGNAL(moveSucceeded()), this, SLOT(smRMoveSucceeded()));
	disconnect(hexapod_->r(), SIGNAL(moveFailed(int)), this, SLOT(smRMoveFailed(int)));
	disconnect(hexapod_->s(), SIGNAL(moveSucceeded()), this, SLOT(smSMoveSucceeded()));
	disconnect(hexapod_->s(), SIGNAL(moveFailed(int)), this, SLOT(smSMoveFailed(int)));
	disconnect(hexapod_->t(), SIGNAL(moveSucceeded()), this, SLOT(smTMoveSucceeded()));
	disconnect(hexapod_->t(), SIGNAL(moveFailed(int)), this, SLOT(smTMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod S into position."));
}

void REIXSSpectrometer::smTMoveFailed(int reason)
{
	disconnect(hexapod_->r(), SIGNAL(moveSucceeded()), this, SLOT(smRMoveSucceeded()));
	disconnect(hexapod_->r(), SIGNAL(moveFailed(int)), this, SLOT(smRMoveFailed(int)));
	disconnect(hexapod_->s(), SIGNAL(moveSucceeded()), this, SLOT(smSMoveSucceeded()));
	disconnect(hexapod_->s(), SIGNAL(moveFailed(int)), this, SLOT(smSMoveFailed(int)));
	disconnect(hexapod_->t(), SIGNAL(moveSucceeded()), this, SLOT(smTMoveSucceeded()));
	disconnect(hexapod_->t(), SIGNAL(moveFailed(int)), this, SLOT(smTMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod S into position."));
}

void REIXSSpectrometer::smXMoveSucceeded()
{
	qDebug() << "Moving X to pos: success!";

	disconnect(hexapod_->x(), SIGNAL(moveSucceeded()), this, SLOT(smXMoveSucceeded()));
	disconnect(hexapod_->x(), SIGNAL(moveFailed(int)), this, SLOT(smXMoveFailed(int)));

	smXMoveDone_ = true;
	if(smXMoveDone_ && smYMoveDone_ && smZMoveDone_)
		smStartMoveLiftTiltTranslation();
}

void REIXSSpectrometer::smYMoveSucceeded()
{
	qDebug() << "Moving Y to pos: success!";

	disconnect(hexapod_->y(), SIGNAL(moveSucceeded()), this, SLOT(smYMoveSucceeded()));
	disconnect(hexapod_->y(), SIGNAL(moveFailed(int)), this, SLOT(smYMoveFailed(int)));

	smYMoveDone_ = true;
	if(smXMoveDone_ && smYMoveDone_ && smZMoveDone_)
		smStartMoveLiftTiltTranslation();

}

void REIXSSpectrometer::smZMoveSucceeded()
{
	qDebug() << "Moving Z to pos: success!";

	disconnect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smZMoveSucceeded()));
	disconnect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smZMoveFailed(int)));

	smZMoveDone_ = true;
	if(smXMoveDone_ && smYMoveDone_ && smZMoveDone_)
		smStartMoveLiftTiltTranslation();
}

void REIXSSpectrometer::smXMoveFailed(int reason)
{
	disconnect(hexapod_->x(), SIGNAL(moveSucceeded()), this, SLOT(smXMoveSucceeded()));
	disconnect(hexapod_->x(), SIGNAL(moveFailed(int)), this, SLOT(smXMoveFailed(int)));
	disconnect(hexapod_->y(), SIGNAL(moveSucceeded()), this, SLOT(smYMoveSucceeded()));
	disconnect(hexapod_->y(), SIGNAL(moveFailed(int)), this, SLOT(smYMoveFailed(int)));
	disconnect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smZMoveSucceeded()));
	disconnect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smZMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod X into position."));
}

void REIXSSpectrometer::smYMoveFailed(int reason)
{
	disconnect(hexapod_->x(), SIGNAL(moveSucceeded()), this, SLOT(smXMoveSucceeded()));
	disconnect(hexapod_->x(), SIGNAL(moveFailed(int)), this, SLOT(smXMoveFailed(int)));
	disconnect(hexapod_->y(), SIGNAL(moveSucceeded()), this, SLOT(smYMoveSucceeded()));
	disconnect(hexapod_->y(), SIGNAL(moveFailed(int)), this, SLOT(smYMoveFailed(int)));
	disconnect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smZMoveSucceeded()));
	disconnect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smZMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod Y into position."));
}

void REIXSSpectrometer::smZMoveFailed(int reason)
{
	disconnect(hexapod_->x(), SIGNAL(moveSucceeded()), this, SLOT(smXMoveSucceeded()));
	disconnect(hexapod_->x(), SIGNAL(moveFailed(int)), this, SLOT(smXMoveFailed(int)));
	disconnect(hexapod_->y(), SIGNAL(moveSucceeded()), this, SLOT(smYMoveSucceeded()));
	disconnect(hexapod_->y(), SIGNAL(moveFailed(int)), this, SLOT(smYMoveFailed(int)));
	disconnect(hexapod_->z(), SIGNAL(moveSucceeded()), this, SLOT(smZMoveSucceeded()));
	disconnect(hexapod_->z(), SIGNAL(moveFailed(int)), this, SLOT(smZMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move Hexapod Z into position."));
}

void REIXSSpectrometer::smLiftMoveSucceeded()
{
	qDebug() << "Moving Lift to pos: success!";

	disconnect(spectrometerRotationDrive_, SIGNAL(moveSucceeded()), this, SLOT(smLiftMoveSucceeded()));
	disconnect(spectrometerRotationDrive_, SIGNAL(moveFailed(int)), this, SLOT(smLiftMoveFailed(int)));

	smLiftMoveDone_ = true;
	if(smLiftMoveDone_ && smTiltMoveDone_ && smTranslationMoveDone_) {
		currentMoveStep_ = MoveDone;
		emit moveSucceeded();
	}
}

void REIXSSpectrometer::smTiltMoveSucceeded()
{
	qDebug() << "Moving Tilt to pos: success!";

	disconnect(detectorTiltDrive_, SIGNAL(moveSucceeded()), this, SLOT(smTiltMoveSucceeded()));
	disconnect(detectorTiltDrive_, SIGNAL(moveFailed(int)), this, SLOT(smTiltMoveFailed(int)));

	smTiltMoveDone_ = true;
	if(smLiftMoveDone_ && smTiltMoveDone_ && smTranslationMoveDone_) {
		currentMoveStep_ = MoveDone;
		emit moveSucceeded();
	}
}

void REIXSSpectrometer::smTranslationMoveSucceeded()
{
	qDebug() << "Moving Translation to pos: success!";

	disconnect(detectorTranslation_, SIGNAL(moveSucceeded()), this, SLOT(smTranslationMoveSucceeded()));
	disconnect(detectorTranslation_, SIGNAL(moveFailed(int)), this, SLOT(smTranslationMoveFailed(int)));

	smTranslationMoveDone_ = true;
	if(smLiftMoveDone_ && smTiltMoveDone_ && smTranslationMoveDone_) {
		currentMoveStep_ = MoveDone;
		emit moveSucceeded();
	}
}

void REIXSSpectrometer::smLiftMoveFailed(int reason)
{
	disconnect(spectrometerRotationDrive_, SIGNAL(moveSucceeded()), this, SLOT(smLiftMoveSucceeded()));
	disconnect(spectrometerRotationDrive_, SIGNAL(moveFailed(int)), this, SLOT(smLiftMoveFailed(int)));
	disconnect(detectorTiltDrive_, SIGNAL(moveSucceeded()), this, SLOT(smTiltMoveSucceeded()));
	disconnect(detectorTiltDrive_, SIGNAL(moveFailed(int)), this, SLOT(smTiltMoveFailed(int)));
	disconnect(detectorTranslation_, SIGNAL(moveSucceeded()), this, SLOT(smTranslationMoveSucceeded()));
	disconnect(detectorTranslation_, SIGNAL(moveFailed(int)), this, SLOT(smTranslationMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move the Spectrometer Rotation motor into position."));
}

void REIXSSpectrometer::smTiltMoveFailed(int reason)
{
	disconnect(spectrometerRotationDrive_, SIGNAL(moveSucceeded()), this, SLOT(smLiftMoveSucceeded()));
	disconnect(spectrometerRotationDrive_, SIGNAL(moveFailed(int)), this, SLOT(smLiftMoveFailed(int)));
	disconnect(detectorTiltDrive_, SIGNAL(moveSucceeded()), this, SLOT(smTiltMoveSucceeded()));
	disconnect(detectorTiltDrive_, SIGNAL(moveFailed(int)), this, SLOT(smTiltMoveFailed(int)));
	disconnect(detectorTranslation_, SIGNAL(moveSucceeded()), this, SLOT(smTranslationMoveSucceeded()));
	disconnect(detectorTranslation_, SIGNAL(moveFailed(int)), this, SLOT(smTranslationMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, QString("Spectrometer move: failed while trying to move the detector tilt motor into position. Target: %1.  Reached: %2").arg(detectorTiltDrive_->setpoint()).arg(detectorTiltDrive_->value())));
}

void REIXSSpectrometer::smTranslationMoveFailed(int reason)
{
	disconnect(spectrometerRotationDrive_, SIGNAL(moveSucceeded()), this, SLOT(smLiftMoveSucceeded()));
	disconnect(spectrometerRotationDrive_, SIGNAL(moveFailed(int)), this, SLOT(smLiftMoveFailed(int)));
	disconnect(detectorTiltDrive_, SIGNAL(moveSucceeded()), this, SLOT(smTiltMoveSucceeded()));
	disconnect(detectorTiltDrive_, SIGNAL(moveFailed(int)), this, SLOT(smTiltMoveFailed(int)));
	disconnect(detectorTranslation_, SIGNAL(moveSucceeded()), this, SLOT(smTranslationMoveSucceeded()));
	disconnect(detectorTranslation_, SIGNAL(moveFailed(int)), this, SLOT(smTranslationMoveFailed(int)));

	currentMoveStep_ = MoveDone;
	emit moveFailed(reason);

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, reason, "Spectrometer move: failed while trying to move the Detector Translation motor into position."));
}

bool REIXSSpectrometer::stop()
{
	if(!canStop())
		return false;

	if(currentMoveStep_ != MoveDone) {
		emit moveFailed(AMControl::WasStoppedFailure);
	}

	spectrometerRotationDrive_->stop();
	detectorTranslation_->stop();
	detectorTiltDrive_->stop();
	// hexapod: cannot stop without wrecking init. Don't worry for now... just let it stop over time. Not necessary for it to be not-moving before we re-send it somewhere new.

	/// \todo Actually, have to flag that a stop has started, and also catch when the stop is finished... Motors will take a while to actually receive and decelerate.
	currentMoveStep_ = MoveDone;

	return true;
}

bool REIXSSpectrometer::loadSpectrometerCalibration(AMDatabase *db, int databaseId)
{
	if(databaseId >= 1) {
		if(!calibration_.loadFromDb(db, databaseId))
			return false;
	}
	else {
		calibration_ = REIXSXESCalibration();
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
	double u = calibration_.hexapodU(currentGrating_);

	return hexapod_->x()->withinTolerance(xyz.x()) &&
			hexapod_->y()->withinTolerance(xyz.y()) &&
			hexapod_->z()->withinTolerance(xyz.z()) &&
			hexapod_->r()->withinTolerance(rst.x()) &&
			hexapod_->s()->withinTolerance(rst.y()) &&
			hexapod_->t()->withinTolerance(rst.z()) &&
			hexapod_->u()->withinTolerance(u) &&
			hexapod_->v()->withinTolerance(0) &&
			hexapod_->w()->withinTolerance(0);
}



