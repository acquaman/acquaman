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

QString AMGCS2AsyncMoveCommand::outputString() const
{
	if(runningState_ == Succeeded) {
		return "Move succeeded";
	} else {
		return "";
	}
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

#include <QDebug>
void AMGCS2AsyncMoveCommand::isFinishedImplementation()
{
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
	currentPositionCommand.setController(controller_);
	currentPositionCommand.run();

	if(currentPositionCommand.runningState() != Succeeded) {
		lastError_ = "Could not obtain final position";
		runningState_ = Failed;
		return;
	}

	QHash<AMGCS2::Axis, double> finalPositions = currentPositionCommand.axisPositions();
	foreach (AMGCS2::Axis currentAxis, axesToMove_.keys()) {
		double destination = axesToMove_.value(currentAxis);
		double finalPosition = finalPositions.value(currentAxis);

		double epsilon = 0.0001;

		qDebug() << "Destination: " << destination << " Final position: " << finalPosition << " Difference: " << qAbs(destination - finalPosition) << " Check epsilon: " << epsilon;

		qDebug() << "Is " << qAbs(destination - finalPosition) << " > " << epsilon;
		qDebug() << ((qAbs(destination - finalPosition)) > epsilon);
		if(qAbs(destination - finalPosition) > epsilon) {
			lastError_ = QString("Axis %1 did not reach its destination of %2 (stopped at %3)")
					.arg(AMGCS2Support::axisToCharacter(currentAxis))
					.arg(destination)
					.arg(finalPosition);

			runningState_ = Failed;
			return;
		}
	}

	runningState_ = Succeeded;
}


