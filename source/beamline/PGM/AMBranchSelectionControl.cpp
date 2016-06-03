#include "AMBranchSelectionControl.h"

#include "beamline/AMPVControl.h"

AMBranchSelectionControl::AMBranchSelectionControl(QObject *parent)
    : AMEnumeratedControl("Branch Selection", QString(), parent)
{
	branchSelectionPVControl_ = new AMPVwStatusControl("Branch Selection PV", "BL1611-ID-2:Branch:fbk", "BL1611-ID-2:Branch", "SMTR16114I2007:state",QString(),this,0.5, 2.0, new AMControlStatusCheckerStopped(0));

	addOption(BranchA, "Branch A");
	addOption(BranchB, "Branch B");
	addOption(Between, "Between", true);
	addOption(Unknown, "Unknown", true);

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Branch Selection Control");

	updateStates();
}

bool AMBranchSelectionControl::shouldMeasure() const
{
	return true;
}

bool AMBranchSelectionControl::shouldMove() const
{
	return true;
}

bool AMBranchSelectionControl::shouldStop() const
{
	return false;
}

bool AMBranchSelectionControl::canMeasure() const
{
	return shouldMeasure() && branchSelectionPVControl_ && branchSelectionPVControl_->canMeasure();
}

bool AMBranchSelectionControl::canMove() const
{
	return shouldMove() && branchSelectionPVControl_ && branchSelectionPVControl_->canMove();
}

bool AMBranchSelectionControl::canStop() const
{
	return shouldStop() && branchSelectionPVControl_ && branchSelectionPVControl_->canStop();
}

bool AMBranchSelectionControl::validValue(double value) const
{
	int branch_value = int(value);

	return branch_value == -1 ||
	        branch_value == 0 ||
	        branch_value == 2;
}

bool AMBranchSelectionControl::validSetpoint(double value) const
{
	Branch branch_value = Branch(int(value));

	return branch_value == BranchA ||
	        branch_value == BranchB;
}

void AMBranchSelectionControl::updateConnected()
{
	setConnected(branchSelectionPVControl_ &&
	             branchSelectionPVControl_->isConnected());
}

void AMBranchSelectionControl::updateMoving()
{
	setIsMoving(isConnected() &&
	            branchSelectionPVControl_->isMoving());
}

int AMBranchSelectionControl::currentIndex() const
{
	if (!isConnected()) {

		return -1;
	}

	int int_value = int(branchSelectionPVControl_->value());

	// The PV uses 3 for unknown, the enumerated control interface uses -1
	if (int_value == 3) {
		return -1;
	}

	// Else we can just map the PV value to our enum
	return Branch(int_value);
}

