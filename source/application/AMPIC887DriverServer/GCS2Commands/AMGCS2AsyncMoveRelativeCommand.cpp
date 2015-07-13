#include "AMGCS2AsyncMoveRelativeCommand.h"
#include "AMGCS2GetTargetPositionCommand.h"
#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "../AMGCS2Support.h"
AMGCS2AsyncMoveRelativeCommand::AMGCS2AsyncMoveRelativeCommand(const QHash<AMGCS2::Axis, double>& relativeAxisPositions)
{
	relativeAxisPositions_ = relativeAxisPositions;
	moveRelativeCommand_ = new AMGCS2MoveRelativeCommand(relativeAxisPositions);
}

AMGCS2AsyncMoveRelativeCommand::~AMGCS2AsyncMoveRelativeCommand()
{
	delete moveRelativeCommand_;
}

QString AMGCS2AsyncMoveRelativeCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	return "Move Succeeded";
}

QHash<AMGCS2::Axis, double> AMGCS2AsyncMoveRelativeCommand::relativeTargetPositions() const
{
	return relativeAxisPositions_;
}

bool AMGCS2AsyncMoveRelativeCommand::validateArguments()
{
	if(relativeAxisPositions_.isEmpty()) {
		lastError_ = "No axis positions provided";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, relativeAxisPositions_.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot move unknown axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2AsyncMoveRelativeCommand::runImplementation()
{
	// Need to store the original target positions, so we know where we're
	// heading later.
	QList<AMGCS2::Axis> axesMoved = relativeAxisPositions_.keys();
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

void AMGCS2AsyncMoveRelativeCommand::isFinishedImplementation()
{
	QList<AMGCS2::Axis> axesMoved = relativeAxisPositions_.keys();

	// Are we still moving?

	AMGCS2GetMovingStatusCommand movingStatusCommand;

	movingStatusCommand.setController(controller_);
	movingStatusCommand.run();

	if(movingStatusCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain moving status of axes";
		runningState_ = Failed;
		return;
	}

	QFlags<AMGCS2::AxisMovementStatus> movementStatuses =
			movingStatusCommand.movementStatuses();

	bool allStillMoving = true;
	foreach(AMGCS2::Axis currentAxis, axesMoved) {
		switch(currentAxis) {
		case AMGCS2::XAxis:
			allStillMoving &= movementStatuses.testFlag(AMGCS2::XAxisIsMoving);
			break;
		case AMGCS2::YAxis:
			allStillMoving &= movementStatuses.testFlag(AMGCS2::YAxisIsMoving);
			break;
		case AMGCS2::ZAxis:
			allStillMoving &= movementStatuses.testFlag(AMGCS2::ZAxisIsMoving);
			break;
		case AMGCS2::UAxis:
			allStillMoving &= movementStatuses.testFlag(AMGCS2::UAxisIsMoving);
			break;
		case AMGCS2::VAxis:
			allStillMoving &= movementStatuses.testFlag(AMGCS2::VAxisIsMoving);
			break;
		case AMGCS2::WAxis:
			allStillMoving &= movementStatuses.testFlag(AMGCS2::WAxisIsMoving);
			break;
		default:
			break;
		}
	}

	if(allStillMoving) {
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

		double epsilon = 0.0001;

		if(qAbs(destination - finalPosition) > epsilon) {
			// We've stopped and aren't yet at out final position, this might
			// be because our target position has been altered with another move
			// or a stop (in which case we failed) or because we're just doing
			// our fine grain motions.
			if(qAbs(destination - currentTargetPosition) > epsilon) {
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

