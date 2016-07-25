#include "PGMBranchSelectionControl.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

PGMBranchSelectionControl::PGMBranchSelectionControl(QObject *parent)
    : AMEnumeratedControl("Branch Selection", QString(), parent)
{
	branchSelectionPVControl_ = new AMPVwStatusControl("Branch Selection PV", "BL1611-ID-2:Branch:fbk", "BL1611-ID-2:Branch", "SMTR16114I2007:state", QString(), this, 0.5, 2.0, new AMControlStatusCheckerStopped(0));
	branchMotorFeedbackControl_ = new AMReadOnlyPVControl("Branch Motor Feedback", "SMTR16114I2007:step:fbk", this);

	addOption(BranchA, "Branch A");
	addOption(BranchB, "Branch B");
	addOption(Between, "Between", true);
	addOption(Unknown, "Unknown", true);

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Branch Selection Control");

	updateStates();

	addChildControl(branchSelectionPVControl_);
	addChildControl(branchMotorFeedbackControl_);
}

bool PGMBranchSelectionControl::shouldMeasure() const
{
	return true;
}

bool PGMBranchSelectionControl::shouldMove() const
{
	return true;
}

bool PGMBranchSelectionControl::shouldStop() const
{
	return false;
}

bool PGMBranchSelectionControl::canMeasure() const
{
	return shouldMeasure()
			&& branchSelectionPVControl_ && branchSelectionPVControl_->canMeasure()
			&& branchMotorFeedbackControl_ && branchMotorFeedbackControl_->canMeasure();
}

bool PGMBranchSelectionControl::canMove() const
{
	return shouldMove() && branchSelectionPVControl_ && branchSelectionPVControl_->canMove();
}

bool PGMBranchSelectionControl::canStop() const
{
	return shouldStop() && branchSelectionPVControl_ && branchSelectionPVControl_->canStop();
}

bool PGMBranchSelectionControl::validValue(double value) const
{
	int branchValue = int(value);

	return branchValue == -1 ||
		branchValue == 0 ||
		branchValue == 2;
}

bool PGMBranchSelectionControl::validSetpoint(double value) const
{
	Branch branchValue = Branch(int(value));

	return branchValue == BranchA ||
		branchValue == BranchB;
}

void PGMBranchSelectionControl::updateConnected()
{
	setConnected(branchSelectionPVControl_ &&
		     branchSelectionPVControl_->isConnected() &&
		     branchMotorFeedbackControl_ &&
		     branchMotorFeedbackControl_->isConnected());
}

void PGMBranchSelectionControl::updateMoving()
{
	setIsMoving(isConnected() &&
	            branchSelectionPVControl_->isMoving());
}

AMAction3 *PGMBranchSelectionControl::createMoveAction(double setpoint)
{
	return AMActionSupport::buildControlMoveAction(branchSelectionPVControl_, setpoint, false, false);
}

int PGMBranchSelectionControl::currentIndex() const
{
	if (!isConnected()) {

		return -1;
	}

	int intValue = int(branchSelectionPVControl_->value());

	// The PV uses 3 for unknown, the enumerated control interface uses -1
	if (intValue == 3) {
		return -1;
	}

	// This is a special case to handle the fact that the motor slips and we still want the correct index displayed.
	if (!isMoving() && intValue == Between){

		if (branchMotorFeedbackControl_->value() > 20000)
			return BranchA;

		else if (branchMotorFeedbackControl_->value() < -14500)
			return BranchB;
	}

	// Else we can just map the PV value to our enum
	return Branch(intValue);
}

