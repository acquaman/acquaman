#include "AMGCS2AsyncMoveCommand.h"
#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetCurrentPositionCommand.h"
#include "../AMGCS2Support.h"
AMGCS2AsyncMoveCommand::AMGCS2AsyncMoveCommand(const QHash<AMGCS2::Axis, double>& axisPositions)
{
	command_ = new AMGCS2MoveCommand(axisPositions);
	axesToMove_ = axisPositions;
}

AMGCS2AsyncMoveCommand::~AMGCS2AsyncMoveCommand() {
	delete command_;
}

bool AMGCS2AsyncMoveCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToMove_.keys()) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot move unknown axis";
			return false;
		}
	}
	return true;
}

bool AMGCS2AsyncMoveCommand::runImplementation()
{
	command_->setControllerId(controllerId_);
	command_->run();

	if(command_->runningState() != Succeeded) {
		lastError_ = command_->lastError();
		return false;
	}

	return true;
}

#include <QDebug>
void AMGCS2AsyncMoveCommand::isFinishedImplementation()
{
	AMGCS2GetMovingStatusCommand movingStatusCommand;

	movingStatusCommand.setControllerId(controllerId_);
	movingStatusCommand.run();

	if(movingStatusCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain moving status of axes";
		runningState_ = Failed;
		return;
	}

	QFlags<AMGCS2::AxisMovementStatus> movementStatuses =
			movingStatusCommand.movementStatuses();

	bool allStillMoving = true;
	foreach(AMGCS2::Axis currentAxis, axesToMove_.keys()) {
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

	// We've stopped. Did we get where we wanted?

	AMGCS2GetCurrentPositionCommand currentPositionCommand(axesToMove_.keys());
	currentPositionCommand.setControllerId(controllerId_);
	currentPositionCommand.run();

	if(currentPositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain final position";
		runningState_ = Failed;
		return;
	}

	QHash<AMGCS2::Axis, double> finalPositions = currentPositionCommand.axisPositions();
	foreach (AMGCS2::Axis currentAxis, axesToMove_.keys()) {
		double currentDesination = axesToMove_.value(currentAxis);
		double finalPosition = finalPositions.value(currentAxis);
		qDebug() << "Destination = " << currentDesination << " Final = " << finalPosition;
		if(!qFuzzyIsNull(currentDesination - finalPosition)) {
			lastError_ = QString("Axis %1 did not reach its destination")
					.arg(AMGCS2Support::axisToCharacter(currentAxis));
			runningState_ = Failed;
			return;
		}
	}

	runningState_ = Succeeded;
}
