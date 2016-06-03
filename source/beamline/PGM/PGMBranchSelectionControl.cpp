#include "PGMBranchSelectionControl.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

PGMBranchSelectionControl::PGMBranchSelectionControl(QObject *parent)
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

	addChildControl(branchSelectionPVControl_);
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
	return shouldMeasure() && branchSelectionPVControl_ && branchSelectionPVControl_->canMeasure();
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
	int branch_value = int(value);

	return branch_value == -1 ||
	        branch_value == 0 ||
	        branch_value == 2;
}

bool PGMBranchSelectionControl::validSetpoint(double value) const
{
	Branch branch_value = Branch(int(value));

	return branch_value == BranchA ||
	        branch_value == BranchB;
}

void PGMBranchSelectionControl::updateConnected()
{
	setConnected(branchSelectionPVControl_ &&
	             branchSelectionPVControl_->isConnected());
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

	int int_value = int(branchSelectionPVControl_->value());

	// The PV uses 3 for unknown, the enumerated control interface uses -1
	if (int_value == 3) {
		return -1;
	}

	// Else we can just map the PV value to our enum
	return Branch(int_value);
}

