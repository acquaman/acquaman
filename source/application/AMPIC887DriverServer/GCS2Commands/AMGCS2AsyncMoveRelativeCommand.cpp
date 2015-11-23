#include "AMGCS2AsyncMoveRelativeCommand.h"
#include "AMGCS2GetTargetPositionCommand.h"
#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887Controller.h"
AMGCS2AsyncMoveRelativeCommand::AMGCS2AsyncMoveRelativeCommand(const AMPIC887AxisMap<double>& relativeAxisPositions)
	: AMGCS2AsyncCommand()
{
	relativeAxisPositions_ = relativeAxisPositions;
	moveRelativeCommand_ = new AMGCS2MoveRelativeCommand(relativeAxisPositions);
}

AMGCS2AsyncMoveRelativeCommand::~AMGCS2AsyncMoveRelativeCommand()
{
	delete moveRelativeCommand_;
}

AMPIC887AxisMap<double> AMGCS2AsyncMoveRelativeCommand::relativeTargetPositions() const
{
	return relativeAxisPositions_;
}

bool AMGCS2AsyncMoveRelativeCommand::validateArguments()
{
	if(relativeAxisPositions_.isEmpty()) {
		lastError_ = "No relative positions provided";
		return false;
	}

	if(relativeAxisPositions_.containsUnknownAxis()) {
		lastError_ = "Unknown axis provided";
		return false;
	}

	return true;
}

bool AMGCS2AsyncMoveRelativeCommand::runImplementation()
{
	// Need to store the original target positions, so we know where we're
	// heading later.
	AMPIC887AxisCollection axesMoved = relativeAxisPositions_.axes();
	AMGCS2GetTargetPositionCommand targetPositionCommand(axesMoved);
	targetPositionCommand.setController(controller_);
	targetPositionCommand.run();

	AMPIC887AxisMap<double> currentTargetPositions =
			targetPositionCommand.axisTargetPositions();

	foreach(AMGCS2::Axis currentAxis, axesMoved) {
		double currentTargetPosition = currentTargetPositions.value(currentAxis);
		double relativeAddition = relativeAxisPositions_.value(currentAxis);
		originalTargetPositions_.insert(currentAxis, (currentTargetPosition + relativeAddition));
	}

	if(targetPositionCommand.runningState() != Succeeded) {
		lastError_ = targetPositionCommand.lastError();
		return false;
	}

	moveRelativeCommand_->setController(controller_);
	moveRelativeCommand_->run();

	if(moveRelativeCommand_->runningState() != Succeeded) {
		lastError_ = moveRelativeCommand_->lastError();
		return false;
	}

	return true;
}

void AMGCS2AsyncMoveRelativeCommand::checkRunningState()
{
	AMPIC887AxisCollection axesMoved = relativeAxisPositions_.axes();

	// Are we still moving?

	AMGCS2GetMovingStatusCommand movingStatusCommand;

	movingStatusCommand.setController(controller_);
	movingStatusCommand.run();

	if(movingStatusCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain moving status of axes";
		runningState_ = Failed;
		return;
	}

	AMGCS2::AxisMovementStatuses movementStatuses =
			movingStatusCommand.movementStatuses();

	bool anyStillMoving = false;
	foreach(AMGCS2::Axis currentAxis, axesMoved) {
		switch(currentAxis) {
		case AMGCS2::XAxis:
			anyStillMoving |= movementStatuses.testFlag(AMGCS2::XAxisIsMoving);
			break;
		case AMGCS2::YAxis:
			anyStillMoving |= movementStatuses.testFlag(AMGCS2::YAxisIsMoving);
			break;
		case AMGCS2::ZAxis:
			anyStillMoving |= movementStatuses.testFlag(AMGCS2::ZAxisIsMoving);
			break;
		case AMGCS2::UAxis:
			anyStillMoving |= movementStatuses.testFlag(AMGCS2::UAxisIsMoving);
			break;
		case AMGCS2::VAxis:
			anyStillMoving |= movementStatuses.testFlag(AMGCS2::VAxisIsMoving);
			break;
		case AMGCS2::WAxis:
			anyStillMoving |= movementStatuses.testFlag(AMGCS2::WAxisIsMoving);
			break;
		default:
			break;
		}
	}

	if(anyStillMoving) {
		// We're still moving. Do nothing and wait for the next check.
		return;
	}

	// We've stopped. We need to check whether we're at our position, also because
	// the motion status of the axes fluctuates between moving and not moving during
	// the fine grain segment of the motion, we must also check if the target position
	// has been altered.

	AMGCS2GetCurrentPositionCommand currentPositionCommand(axesMoved);
	currentPositionCommand.setController(controller_);
	currentPositionCommand.run();

	if(currentPositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain final position";
		runningState_ = Failed;
		return;
	}

	AMGCS2GetTargetPositionCommand targetPositionCommand(axesMoved);
	targetPositionCommand.setController(controller_);
	targetPositionCommand.run();

	if(targetPositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain target position of axes";
		runningState_ = Failed;
		return;
	}

	AMPIC887AxisMap<double> currentTargetPositions =
			targetPositionCommand.axisTargetPositions();

	AMPIC887AxisMap<double> finalPositions = currentPositionCommand.axisPositions();

	foreach (AMGCS2::Axis currentAxis, axesMoved) {
		double destination = originalTargetPositions_.value(currentAxis);
		double finalPosition = finalPositions.value(currentAxis);
		double currentTargetPosition = currentTargetPositions.value(currentAxis);

		if(qAbs(destination - finalPosition) > AXIS_POSITION_TOLERANCE) {
			// We've stopped and aren't yet at out final position, this might
			// be because our target position has been altered with another move
			// or a stop (in which case we failed) or because we're just doing
			// our fine grain motions.
			if(qAbs(destination - currentTargetPosition) > AXIS_POSITION_TOLERANCE) {
				// Our target position has been altered.
				lastError_ = "Relative move failed to reach target";
				runningState_ = Failed;
				return;
			}
			// We're still doing the fine grain segment of the move.
			return;
		}
	}

	runningState_ = Succeeded;
}

