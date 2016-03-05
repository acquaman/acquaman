#include "SGMBeamCoordinatorControl.h"

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMControl.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

#define CHILD_TIMEOUT 10
SGMBeamCoordinatorControl::SGMBeamCoordinatorControl(QObject* parent)
        : AMPseudoMotorControl("SGM Coordinator Beam On", QString(), parent)
{
	fastShutterVoltage_ = new AMPVControl("Fast Shutter Voltage","PSH16114I1001:V","PSH16114I1001:V", QString(), this, 0.5, CHILD_TIMEOUT);
	frontBypassValve_ = new CLSExclusiveStatesControl("Front Bypass Valve", "VVR1611-4-I10-09:state", "VVR1611-4-I10-09:opr:open", "VVR1611-4-I10-09:opr:close", this);
	backBypassValve_= new CLSExclusiveStatesControl("Back Bypass Valve", "VVR1611-4-I10-10:state", "VVR1611-4-I10-10:opr:open", "VVR1611-4-I10-10:opr:close",this);


	vvr1611_3_I10_01Valve_ = new CLSExclusiveStatesControl("VVR1611-3-I10-01", "VVR1611-3-I10-01:state","VVR1611-3-I10-01:opr:open","VVR1611-3-I10-01:opr:close",this);
	vvr1611_3_I10_02Valve_ = new CLSExclusiveStatesControl("VVR1611-3-I10-02", "VVR1611-3-I10-02:state","VVR1611-3-I10-02:opr:open","VVR1611-3-I10-02:opr:close",this);

	vvr1611_3_I10_03Valve_= new CLSExclusiveStatesControl("VVR1611-3-I10-03", "VVR1611-3-I10-03:state","VVR1611-3-I10-03:opr:open","VVR1611-3-I10-03:opr:close",this);
	vvr1611_3_I10_04Valve_= new CLSExclusiveStatesControl("VVR1611-3-I10-04", "VVR1611-3-I10-04:state","VVR1611-3-I10-04:opr:open","VVR1611-3-I10-04:opr:close",this);
	psh1611_3_I10_01Shutter_ = new CLSExclusiveStatesControl("PSH1611-3-I10-01", "PSH1611-3-I10-01:state","PSH1611-3-I10-01:opr:open","PSH1611-3-I10-01:opr:close",this);
	vvr1611_4_I10_01Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-01", "VVR1611-4-I10-01:state","VVR1611-4-I10-01:opr:open","VVR1611-4-I10-01:opr:close",this);
	vvr1611_4_I10_02Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-02", "VVR1611-4-I10-02:state","VVR1611-4-I10-02:opr:open","VVR1611-4-I10-02:opr:close",this);
	vvr1611_4_I10_03Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-03", "VVR1611-4-I10-03:state","VVR1611-4-I10-03:opr:open","VVR1611-4-I10-03:opr:close",this);
	vvr1611_4_I10_04Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-04", "VVR1611-4-I10-04:state","VVR1611-4-I10-04:opr:open","VVR1611-4-I10-04:opr:close",this);
	vvr1611_4_I10_05Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-05", "VVR1611-4-I10-05:state","VVR1611-4-I10-05:opr:open","VVR1611-4-I10-04:opr:close",this);
	vvr1611_4_I10_06Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-06", "VVR1611-4-I10-06:state","VVR1611-4-I10-06:opr:open","VVR1611-4-I10-06:opr:close",this);
	vvr1611_4_I10_07Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-07", "VVR1611-4-I10-07:state","VVR1611-4-I10-07:opr:open","VVR1611-4-I10-07:opr:close",this);
	vvr1611_4_I10_08Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-08", "VVR1611-4-I10-08:state","VVR1611-4-I10-08:opr:open","VVR1611-4-I10-08:opr:close",this);
	vvr1611_4_I10_11Valve_ = new CLSExclusiveStatesControl("VVR1611-4-I10-11", "VVR1611-4-I10-11:state","VVR1611-4-I10-11:opr:open", "VVR1611-4-I10-11:opr:close",this);
	vvr1611_3_I00_01Valve_ = new CLSExclusiveStatesControl("VVR1611-3-I00-01", "VVR1611-3-I00-01:state","VVR1611-3-I00-01:opr:open","VVR1611-3-I00-01:opr:close",this);
	vvr1611_3_I00_02Valve_ = new CLSExclusiveStatesControl("VVR1611-3-I00-02", "VVR1611-3-I00-01:state","VVR1611-3-I00-01:opr:open","VVR1611-3-I00-01:opr:close",this);

	psh1411_I00_01Shutter_ = new AMReadOnlyPVControl("PSH1411-I00-01", "PSH1411-I00-01:state", this);
	vvr1411_I00_01Valve_ = new AMReadOnlyPVControl("VVR1411-I00-01", "VVR1411-I00-01:state", this);
	vvf1411_I00_01_ = new AMReadOnlyPVControl("VVF1411-I00-01", "VVF1411-I00-01:state", this);

	safetyShutter_ = new CLSExclusiveStatesControl("Safety Shutter", "SSH1411-I00-01:state", "SSH1411-I00-01:opr:open", "SSH1411-I00-01:opr:close",this);
	psh_2_ = new CLSExclusiveStatesControl("PSH-2", "PSH1411-I00-02:state", "PSH1411-I00-02:opr:open", "PSH1411-I00-02:opr:close",this);

	es2Bypass_ = new AMReadOnlyPVControl("ES2 Bypass", "SWZ1611-4-02:fbk",this);

	addChildControl(fastShutterVoltage_);
	addChildControl(frontBypassValve_);
	addChildControl(backBypassValve_);

	addChildControl(vvr1611_3_I10_01Valve_);
	addChildControl(vvr1611_3_I10_02Valve_);
	addChildControl(vvr1611_3_I10_03Valve_);
	addChildControl(vvr1611_3_I10_04Valve_);
	addChildControl(psh1611_3_I10_01Shutter_);
	addChildControl(vvr1611_4_I10_01Valve_);
	addChildControl(vvr1611_4_I10_02Valve_);
	addChildControl(vvr1611_4_I10_03Valve_);
	addChildControl(vvr1611_4_I10_04Valve_);
	addChildControl(vvr1611_4_I10_05Valve_);
	addChildControl(vvr1611_4_I10_06Valve_);
	addChildControl(vvr1611_4_I10_07Valve_);
	addChildControl(vvr1611_4_I10_08Valve_);
	addChildControl(vvr1611_4_I10_11Valve_);
	addChildControl(psh1411_I00_01Shutter_);
	addChildControl(vvr1411_I00_01Valve_);
	addChildControl(vvf1411_I00_01_);
	addChildControl(safetyShutter_);
	addChildControl(vvr1611_3_I00_01Valve_);
	addChildControl(vvr1611_3_I00_02Valve_);
	addChildControl(es2Bypass_);

	controlsToOpen_.append(vvr1611_3_I10_01Valve_);
	controlsToOpen_.append(vvr1611_3_I10_01Valve_);
	controlsToOpen_.append(vvr1611_3_I10_02Valve_);
	controlsToOpen_.append(vvr1611_3_I10_03Valve_);
	controlsToOpen_.append(vvr1611_3_I10_04Valve_);
	controlsToOpen_.append(vvr1611_4_I10_01Valve_);
	controlsToOpen_.append(vvr1611_4_I10_02Valve_);
	controlsToOpen_.append(vvr1611_4_I10_03Valve_);
	controlsToOpen_.append(vvr1611_4_I10_04Valve_);
	controlsToOpen_.append(vvr1611_4_I10_05Valve_);
	controlsToOpen_.append(vvr1611_4_I10_06Valve_);
	controlsToOpen_.append(vvr1611_4_I10_07Valve_);
	controlsToOpen_.append(vvr1611_4_I10_08Valve_);
	controlsToOpen_.append(frontBypassValve_);
	controlsToOpen_.append(backBypassValve_);
	controlsToOpen_.append(vvr1611_4_I10_11Valve_);
	controlsToOpen_.append(vvr1611_3_I00_01Valve_);
	controlsToOpen_.append(vvr1611_3_I00_02Valve_);


	controlsToWait_.append(psh1411_I00_01Shutter_);
	controlsToWait_.append(vvr1411_I00_01Valve_);
	controlsToWait_.append(vvf1411_I00_01_);

	setMinimumValue(0);
	setMaximumValue(1);
	setTolerance(0.5);

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
	return isConnected();
}

bool SGMBeamCoordinatorControl::canStop() const
{
	return false;
}

void SGMBeamCoordinatorControl::updateValue()
{
	if(moveInProgress()) {
		return;
	}

	if(fastShutterVoltage_->withinTolerance(5)) {

		setValue(0);
	} else {

		bool isOpen = true;

		if(es2Bypass_->withinTolerance(1)) {
			isOpen &= frontBypassValve_->withinTolerance(1);
			isOpen &= backBypassValve_->withinTolerance(1);
		}

		isOpen &= vvr1611_3_I10_01Valve_->withinTolerance(1);
		isOpen &= vvr1611_3_I10_02Valve_->withinTolerance(1);
		isOpen &= vvr1611_3_I10_03Valve_->withinTolerance(1);
		isOpen &= vvr1611_3_I10_04Valve_->withinTolerance(1);
		isOpen &= psh1611_3_I10_01Shutter_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_01Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_02Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_03Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_04Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_05Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_06Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_07Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_08Valve_->withinTolerance(1);
		isOpen &= vvr1611_4_I10_11Valve_->withinTolerance(1);
		isOpen &= vvr1611_3_I00_01Valve_->withinTolerance(1);
		isOpen &= vvr1611_3_I00_02Valve_->withinTolerance(1);

		isOpen &= psh1411_I00_01Shutter_->withinTolerance(1);
		isOpen &= vvr1411_I00_01Valve_->withinTolerance(1);
		isOpen &= vvf1411_I00_01_->withinTolerance(1);
		isOpen &= safetyShutter_->withinTolerance(1);
		isOpen &= psh_2_->withinTolerance(1);

		if(isOpen) {
			setValue(1);
		} else {
			setValue(0);
		}
	}
}

void SGMBeamCoordinatorControl::onMoveFailed(QObject *action)
{
	moveActionCleanup(action);
	updateValue();
	emit moveFailed(AMControl::OtherFailure);
}

void SGMBeamCoordinatorControl::onMoveSucceeded(QObject *action)
{
	moveActionCleanup(action);
	updateValue();
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

		// 1. Set fast shutter voltage to 0
		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(fastShutterVoltage_, 0));
		returnAction->addSubAction(AMActionSupport::buildControlWaitAction(fastShutterVoltage_, 0, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));

		// 2. Open all the valves in 1611, in order - except for the bypass valves, which we only open if ES2 is
		// in position
		foreach(AMControl* control, controlsToOpen_) {

			if(control == frontBypassValve_ || control == backBypassValve_) {
				if(es2Bypass_->withinTolerance(1)) {
					// ES2 is in position
					returnAction->addSubAction(AMActionSupport::buildControlMoveAction(control, 1));
					returnAction->addSubAction(AMActionSupport::buildControlWaitAction(control, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));
				}

			} else {

				returnAction->addSubAction(AMActionSupport::buildControlMoveAction(control, 1));
				returnAction->addSubAction(AMActionSupport::buildControlWaitAction(control, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));
			}
		}

		// 3. Ensure that all the required controls which we can't move are also open
		foreach(AMControl* control, controlsToWait_) {
			returnAction->addSubAction(AMActionSupport::buildControlWaitAction(control, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));
		}

		// 4. Open the safety shutters and PSH2 IN THAT ORDER
		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutter_, 1));
		returnAction->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutter_, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));

		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(psh_2_, 1));
		returnAction->addSubAction(AMActionSupport::buildControlWaitAction(psh_2_, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));

		// 5. Finally, open the beam shutter
		returnAction->addSubAction(AMActionSupport::buildControlMoveAction(psh1611_3_I10_01Shutter_, 1));
		returnAction->addSubAction(AMActionSupport::buildControlWaitAction(psh1611_3_I10_01Shutter_, 1, CHILD_TIMEOUT, AMControlWaitActionInfo::MatchWithinTolerance));


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
