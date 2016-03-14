#include "AMGCS2AsyncReferenceMoveCommand.h"
#include "AMGCS2ReferenceMoveCommand.h"

#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetReferenceResultCommand.h"

AMGCS2AsyncReferenceMoveCommand::AMGCS2AsyncReferenceMoveCommand(const AMPIC887AxisCollection& axesToReference)
	: AMGCS2AsyncCommand()
{
	axesToReference_ = axesToReference;
}

bool AMGCS2AsyncReferenceMoveCommand::runImplementation()
{
	// Better to implement this in terms of a synchronous reference move command
	// so we only have to change the reference move logic in one place.
	AMGCS2ReferenceMoveCommand referenceMoveCommand(axesToReference_);
	referenceMoveCommand.setController(controller_);
	referenceMoveCommand.run();

	bool success = (referenceMoveCommand.runningState() == Succeeded);

	if(!success) {
		lastError_ = referenceMoveCommand.lastError();
	}

	return success;
}

void AMGCS2AsyncReferenceMoveCommand::checkRunningState()
{
	AMGCS2GetReferenceResultCommand referenceResultsCommand(axesToReference_);
	referenceResultsCommand.setController(controller_);
	referenceResultsCommand.run();

	if(referenceResultsCommand.runningState() != Succeeded) {
		lastError_ = QString("Could not obtain reference status with message: %1")
				.arg(referenceResultsCommand.lastError());
		runningState_ = Failed;
		return;
	}

	AMPIC887AxisMap<bool> axisReferenceResults =
			referenceResultsCommand.axesReferenceResults();

	bool allReferenced = true;
	foreach(AMGCS2::Axis currentAxis, axesToReference_) {
		allReferenced &= axisReferenceResults.value(currentAxis);
	}

	// If all are referenced we can trust that the reference completed successfully.
	if(allReferenced) {
		runningState_ = Succeeded;
		return;
	}

	// Now we need to ensure that if it hasn't completed yet, the axes are at least
	// still moving.

	AMGCS2GetMovingStatusCommand movingStatusCommand;
	movingStatusCommand.setController(controller_);
	movingStatusCommand.run();

	if(movingStatusCommand.runningState() != Succeeded) {
		lastError_ = QString("Could not obtain movement status with message: %1")
				.arg(movingStatusCommand.lastError());

		runningState_ = Failed;;
		return;
	}

	AMGCS2::AxisMovementStatuses movementStatuses = movingStatusCommand.movementStatuses();

	bool anyStillMoving = false;
	foreach(AMGCS2::Axis currentAxis, axesToReference_) {
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
		return;
	} else {
		lastError_ = "Reference move motions have stopped with reference not complete";
		runningState_ = Failed;
		return;
	}
}

