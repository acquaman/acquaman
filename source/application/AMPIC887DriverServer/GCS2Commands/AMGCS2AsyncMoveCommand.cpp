#include "AMGCS2AsyncMoveCommand.h"
#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "AMGCS2GetTargetPositionCommand.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887Controller.h"
AMGCS2AsyncMoveCommand::AMGCS2AsyncMoveCommand(const AMPIC887AxisMap<double>& targetPositions)
	: AMGCS2AsyncCommand()
{
	command_ = new AMGCS2MoveCommand(targetPositions);
	targetPositions_ = targetPositions;
}

AMGCS2AsyncMoveCommand::~AMGCS2AsyncMoveCommand() {
	delete command_;
}

AMPIC887AxisMap<double> AMGCS2AsyncMoveCommand::targetPositions() const
{
	return targetPositions_;
}

bool AMGCS2AsyncMoveCommand::validateArguments()
{
	if(targetPositions_.isEmpty()) {
		lastError_ = "No axis positions provided";
		return false;
	}

	if(targetPositions_.containsUnknownAxis()) {
		lastError_ = "Contains unknown axis";
		return false;
	}

	return true;
}

bool AMGCS2AsyncMoveCommand::runImplementation()
{
	command_->setController(controller_);
	command_->run();

	if(command_->runningState() != Succeeded) {
		lastError_ = command_->lastError();
		return false;
	}

	return true;
}

void AMGCS2AsyncMoveCommand::checkRunningState()
{
	AMPIC887AxisCollection axesMoved = targetPositions_.axes();

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
		double destination = targetPositions_.value(currentAxis);
		double finalPosition = finalPositions.value(currentAxis);
		double currentTargetPosition = currentTargetPositions.value(currentAxis);

		if(qAbs(destination - finalPosition) > AXIS_POSITION_TOLERANCE) {
			// We've stopped and aren't yet at out final position, this might
			// be because our target position has been altered with another move
			// or a stop (in which case we failed) or because we're just doing
			// our fine grain motions.
			if(qAbs(destination - currentTargetPosition) > AXIS_POSITION_TOLERANCE) {
				// Our target position has been altered.
				lastError_ = "Move failed to reach target";
				runningState_ = Failed;
				return;
			}
			// We're still doing the fine grain segment of the move.
			return;
		}
	}

	runningState_ = Succeeded;
}



