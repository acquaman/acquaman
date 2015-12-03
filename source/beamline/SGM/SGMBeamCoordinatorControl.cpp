#include "SGMBeamCoordinatorControl.h"

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#define CHILD_TIMEOUT 10
SGMBeamCoordinatorControl::SGMBeamCoordinatorControl(QObject* parent)
        : AMPseudoMotorControl("SGM Coordinator Beam On", QString(), parent)
{
	fastShutterVoltage_ = new AMPVControl("Fast Shutter Voltage","PSH16114I1001:V","PSH16114I1001:V", QString(), this, 0.5, CHILD_TIMEOUT);
	frontBypassValve_ = new CLSBiStateControl("Front Bypass Valve", "Before Bypass Valve", "VVR1611-4-I10-09:state", "VVR1611-4-I10-09:opr:open", "VVR1611-4-I10-09:opr:close", new AMControlStatusCheckerDefault(2), this);
	backBypassValve_= new CLSBiStateControl("Back Bypass Valve", "Behind Bypass Valve", "VVR1611-4-I10-10:state", "VVR1611-4-I10-10:opr:open", "VVR1611-4-I10-10:opr:close", new AMControlStatusCheckerDefault(2), this);


	vvr1611_3_I10_01Shutter_ = new CLSBiStateControl("VVR1611-3-I10-01","VVR1611-3-I10-01", "VVR1611-3-I10-01:state","VVR1611-3-I10-01:opr:open","VVR1611-3-I10-01:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_3_I10_02Shutter_ = new CLSBiStateControl("VVR1611-3-I10-02", "VVR1611-3-I10-02", "VVR1611-3-I10-02:state","VVR1611-3-I10-02:opr:open","VVR1611-3-I10-02:opr:close", new AMControlStatusCheckerDefault(2),this);

	vvr1611_3_I10_03Shutter_= new CLSBiStateControl("VVR1611-3-I10-03", "VVR1611-3-I10-03", "VVR1611-3-I10-03:state","VVR1611-3-I10-03:opr:open","VVR1611-3-I10-03:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_3_I10_04Shutter_= new CLSBiStateControl("VVR1611-3-I10-04", "VVR1611-3-I10-04", "VVR1611-3-I10-04:state","VVR1611-3-I10-04:opr:open","VVR1611-3-I10-04:opr:close", new AMControlStatusCheckerDefault(2),this);
	psh1611_3_I10_01Shutter_ = new CLSBiStateControl("PSH1611-3-I10-01", "PSH1611-3-I10-01", "PSH1611-3-I10-01:state","PSH1611-3-I10-01:opr:open","PSH1611-3-I10-01:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_01Shutter_ = new CLSBiStateControl("VVR1611-4-I10-01", "VVR1611-4-I10-01", "VVR1611-4-I10-01:state","VVR1611-4-I10-01:opr:open","VVR1611-4-I10-01:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_02Shutter_ = new CLSBiStateControl("VVR1611-4-I10-02", "VVR1611-4-I10-02", "VVR1611-4-I10-02:state","VVR1611-4-I10-02:opr:open","VVR1611-4-I10-02:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_03Shutter_ = new CLSBiStateControl("VVR1611-4-I10-03", "VVR1611-4-I10-03", "VVR1611-4-I10-03:state","VVR1611-4-I10-03:opr:open","VVR1611-4-I10-03:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_04Shutter_ = new CLSBiStateControl("VVR1611-4-I10-04", "VVR1611-4-I10-04", "VVR1611-4-I10-04:state","VVR1611-4-I10-04:opr:open","VVR1611-4-I10-04:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_05Shutter_ = new CLSBiStateControl("VVR1611-4-I10-05", "VVR1611-4-I10-05", "VVR1611-4-I10-05:state","VVR1611-4-I10-05:opr:open","VVR1611-4-I10-04:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_06Shutter_ = new CLSBiStateControl("VVR1611-4-I10-06", "VVR1611-4-I10-06", "VVR1611-4-I10-06:state","VVR1611-4-I10-06:opr:open","VVR1611-4-I10-06:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_07Shutter_ = new CLSBiStateControl("VVR1611-4-I10-07", "VVR1611-4-I10-07", "VVR1611-4-I10-07:state","VVR1611-4-I10-07:opr:open","VVR1611-4-I10-07:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_08Shutter_ = new CLSBiStateControl("VVR1611-4-I10-08", "VVR1611-4-I10-08", "VVR1611-4-I10-08:state","VVR1611-4-I10-08:opr:open","VVR1611-4-I10-08:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_4_I10_11Shutter_ = new CLSBiStateControl("VVR1611-4-I10-11", "VVR1611-4-I10-11", "VVR1611-4-I10-11:state","VVR1611-4-I10-11:opr:open", "VVR1611-4-I10-11:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_3_I00_01Shutter_ = new CLSBiStateControl("VVR1611-3-I00-01", "VVR1611-3-I00-01", "VVR1611-3-I00-01:state","VVR1611-3-I00-01:opr:open","VVR1611-3-I00-01:opr:close", new AMControlStatusCheckerDefault(2),this);
	vvr1611_3_I00_02Shutter_ = new CLSBiStateControl("VVR1611-3-I00-02", "VVR1611-3-I00-02", "VVR1611-3-I00-01:state","VVR1611-3-I00-01:opr:open","VVR1611-3-I00-01:opr:close", new AMControlStatusCheckerDefault(2),this);

	psh1411_I00_01Shutter_ = new AMReadOnlyPVControl("PSH1411-I00-01", "PSH1411-I00-01:state", this);
	vvr1411_I00_01Shutter_ = new AMReadOnlyPVControl("VVR1411-I00-01", "VVR1411-I00-01:state", this);
	vvf1411_I00_01Shutter_ = new AMReadOnlyPVControl("VVF1411-I00-01", "VVF1411-I00-01:state", this);
	psh1411_I00_02Shutter_ = new AMReadOnlyPVControl("PSH1411-I00-01", "PSH1411-I00-02:state", this);
	ssh1411_I00_01Shutter_ = new AMReadOnlyPVControl("SSH1411-I00-01", "SSH1411-I00-01:state", this);

	addChildControl(fastShutterVoltage_);
	addChildControl(frontBypassValve_);
	addChildControl(backBypassValve_);

	addChildControl(vvr1611_3_I10_01Shutter_);
	addChildControl(vvr1611_3_I10_02Shutter_);
	addChildControl(vvr1611_3_I10_03Shutter_);
	addChildControl(vvr1611_3_I10_04Shutter_);
	addChildControl(psh1611_3_I10_01Shutter_);
	addChildControl(vvr1611_4_I10_01Shutter_);
	addChildControl(vvr1611_4_I10_02Shutter_);
	addChildControl(vvr1611_4_I10_03Shutter_);
	addChildControl(vvr1611_4_I10_04Shutter_);
	addChildControl(vvr1611_4_I10_05Shutter_);
	addChildControl(vvr1611_4_I10_06Shutter_);
	addChildControl(vvr1611_4_I10_07Shutter_);
	addChildControl(vvr1611_4_I10_08Shutter_);
	addChildControl(vvr1611_4_I10_11Shutter_);
	addChildControl(psh1411_I00_01Shutter_);
	addChildControl(vvr1411_I00_01Shutter_);
	addChildControl(vvf1411_I00_01Shutter_);
	addChildControl(psh1411_I00_02Shutter_);
	addChildControl(ssh1411_I00_01Shutter_);
	addChildControl(vvr1611_3_I00_01Shutter_);
	addChildControl(vvr1611_3_I00_02Shutter_);

	sgmValveControls_ = new AMControlSet(this);
	sgmValveControls_->addControl(frontBypassValve_);
	sgmValveControls_->addControl(backBypassValve_);

	sgmValveControls_->addControl(vvr1611_3_I10_01Shutter_);
	sgmValveControls_->addControl(vvr1611_3_I10_02Shutter_);
	sgmValveControls_->addControl(vvr1611_3_I10_03Shutter_);
	sgmValveControls_->addControl(vvr1611_3_I10_04Shutter_);
	sgmValveControls_->addControl(psh1611_3_I10_01Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_01Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_02Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_03Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_04Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_05Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_06Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_07Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_08Shutter_);
	sgmValveControls_->addControl(vvr1611_4_I10_11Shutter_);
	sgmValveControls_->addControl(vvr1611_3_I00_01Shutter_);
	sgmValveControls_->addControl(vvr1611_3_I00_02Shutter_);

	ringValveControls_ = new AMControlSet(this);
	ringValveControls_->addControl(psh1411_I00_01Shutter_);
	ringValveControls_->addControl(vvr1411_I00_01Shutter_);
	ringValveControls_->addControl(vvf1411_I00_01Shutter_);
	ringValveControls_->addControl(psh1411_I00_02Shutter_);
	ringValveControls_->addControl(ssh1411_I00_01Shutter_);


	setMinimumValue(0);
	setMaximumValue(1);

	setAllowsMovesWhileMoving(false);
	setAttemptMoveWhenWithinTolerance(false);
	updateStates();
}

bool SGMBeamCoordinatorControl::canMeasure() const
{
	bool canMeasureResult = true;
	foreach(AMControl* childControl, children_) {
		canMeasureResult &= childControl->canMeasure();
	}

	return canMeasureResult;
}

bool SGMBeamCoordinatorControl::canMove() const
{
	bool canMoveResult = true;
	foreach(AMControl* childControl, sgmValveControls_->toList()) {
		canMoveResult &= childControl->canMove();
	}

	return canMoveResult;
}

bool SGMBeamCoordinatorControl::canStop() const
{
	bool canStopResult = true;
	foreach(AMControl* childControl, sgmValveControls_->toList()) {
		canStopResult &= childControl->canStop();
	}

	return canStopResult;
}

void SGMBeamCoordinatorControl::updateConnected()
{
	bool connectedResult = true;
	foreach(AMControl* childControl, children_) {
		connectedResult &= childControl->isConnected();
	}

	setConnected(connectedResult);
	updateValue();
}

void SGMBeamCoordinatorControl::updateValue()
{
	// If all child controls except the fast shutter voltage are at 1, and the fast shutter voltage is at 0, the beam is on

	// If the fast shutter voltage is at 5 the beam is off

	// any other state combination and we are Unknown

	if(fastShutterVoltage_->withinTolerance(0)) {

		foreach(AMControl* valveControl, sgmValveControls_->toList()) {

			if(!valveControl->withinTolerance(1)) {
				// The voltage is right, but one of the sgm valves isn't at 1
				setValue(2);
				return;
			}
		}

		foreach(AMControl* valveControl, ringValveControls_->toList()) {

			if(!valveControl->withinTolerance(1)) {
				// The voltage is right, but one of the inner valves isn't at 1
				setValue(2);
				return;
			}
		}

		// The voltage is right, and all valves are at 1
		setValue(1);
		return;

	} else if(fastShutterVoltage_->withinTolerance(5)) {

		// The volate is set to 5, the beam is off
		setValue(0);

	}

	// The controls are in some other state. We don't know
	setValue(2);
}

void SGMBeamCoordinatorControl::onMoveStarted(QObject* /*action*/)
{
	setMoveInProgress(true);
	setIsMoving(true);
	emit moveStarted();
}

void SGMBeamCoordinatorControl::onMoveCancelled(QObject* action)
{
	setIsMoving(false);
	moveActionCleanup(action);
	emit moveFailed(AMControl::WasStoppedFailure);
}

void SGMBeamCoordinatorControl::onMoveFailed(QObject* action)
{
	setIsMoving(false);
	moveActionCleanup(action);
	emit moveFailed(AMControl::OtherFailure);
}

void SGMBeamCoordinatorControl::onMoveSucceeded(QObject* action)
{
	setIsMoving(false);
	moveActionCleanup(action);
	emit moveSucceeded();
}

AMAction3*SGMBeamCoordinatorControl::createMoveAction(double setpoint)
{
	AMListAction3* returnAction = 0;
	if(qAbs(setpoint - 1) < tolerance()) {

		// Setpoint indicates a move to on
		returnAction = new AMListAction3(new AMListActionInfo3("Moving beam to ON",
		                                                       "Moving beam to ON"),
		                                 AMListAction3::Sequential);

		// Move the voltage to zero
		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(fastShutterVoltage_, 0));
		returnAction->addSubAction(AMActionSupport::buildControlWaitAction(fastShutterVoltage_, 0, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));

		// Move all the sgm valves to 1, and wait for them to open one at a time.
		foreach(AMControl* valveControl, sgmValveControls_->toList()) {

			returnAction->addSubAction(AMActionSupport::buildControlMoveAction(valveControl, 1));
			returnAction->addSubAction(AMActionSupport::buildControlWaitAction(valveControl, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));
		}

		// Wait for all the upstream valves to also be one.
		foreach(AMControl* valveControl, ringValveControls_->toList()) {

			returnAction->addSubAction(AMActionSupport::buildControlWaitAction(valveControl, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));

		}

	} else if(qAbs(setpoint) < tolerance()) {

		// Setpoint indicates a move to off

		// Move the voltage to 5
		returnAction = new AMListAction3(new AMListActionInfo3("Moving beam to OFF",
		                                                       "Moving beam to OFF"),
		                                 AMListAction3::Sequential);

		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(fastShutterVoltage_, 5));
		returnAction->addSubAction(AMActionSupport::buildControlWaitAction(fastShutterVoltage_, 5, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));

	}

	return returnAction;
}
