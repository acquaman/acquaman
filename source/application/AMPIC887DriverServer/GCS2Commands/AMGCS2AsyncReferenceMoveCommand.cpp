#include "AMGCS2AsyncReferenceMoveCommand.h"
#include "AMGCS2ReferenceMoveCommand.h"

#include "AMGCS2GetMovingStatusCommand.h"
#include "AMGCS2GetReferenceResultCommand.h"

AMGCS2AsyncReferenceMoveCommand::AMGCS2AsyncReferenceMoveCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToReference_ = axes;
}

bool AMGCS2AsyncReferenceMoveCommand::runImplementation()
{
	// Better to implement this in terms of a synchronous reference move command
	// so we only have to change the reference move logic in one place.
	AMGCS2ReferenceMoveCommand referenceMoveCommand(axesToReference_);
	referenceMoveCommand.setControllerId(controllerId_);
	referenceMoveCommand.run();

	bool success = referenceMoveCommand.wasSuccessful();
	if(!success) {
		lastError_ = referenceMoveCommand.lastError();
	}

	return success;
}

AMGCS2AsyncCommand::RunningState AMGCS2AsyncReferenceMoveCommand::isFinishedImplementation()
{
	AMGCS2GetReferenceResultCommand referenceResultsCommand(axesToReference_);
	referenceResultsCommand.setControllerId(controllerId_);
	referenceResultsCommand.run();

	if(!referenceResultsCommand.wasSuccessful()) {
		return Failed;
	}

	QHash<AMGCS2::Axis, bool> axisReferenceResults =
			referenceResultsCommand.axesReferenceResults();

	bool allReferenced = true;
	foreach(AMGCS2::Axis currentAxis, axesToReference_) {
		allReferenced &= axisReferenceResults.value(currentAxis);
	}

	// If all are referenced we can trust that the reference completed successfully.
	if(allReferenced) {
		return Completed;
	}

	// Now we need to ensure that if it hasn't completed yet, the axes are at least
	// still moving.

	AMGCS2GetMovingStatusCommand movingStatusCommand();
	movingStatusCommand().setControllerId(controllerId_);
	movingStatusCommand().run();

	if(!movingStatusCommand().wasSuccessful()) {
		return Failed;
	}

	QFlags<AMGCS2::AxisMovementStatus> movementStatuses;

	bool allStillMoving = true;
	foreach(AMGCS2::Axis currentAxis, axesToReference_) {
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
		}
	}

	if(allStillMoving) {
		return NotFinished;
	} else {
		lastError_ = "Reference move motions have stopped with reference not complete";
		return Failed;
	}
}
