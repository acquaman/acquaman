#include "AMGCS2GetMovingStatusCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetMovingStatusCommand::AMGCS2GetMovingStatusCommand()
{
	movementStatuses_ =QFlags<AMGCS2::AxisMovementStatus>(AMGCS2::UnknownAxisMovementStatus);
}

QFlags<AMGCS2::AxisMovementStatus> AMGCS2GetMovingStatusCommand::movementStatuses()
{
	return movementStatuses_;
}

bool AMGCS2GetMovingStatusCommand::runImplementation()
{
	// Clear any previous results.
	movementStatuses_ = QFlags<AMGCS2::AxisMovementStatus>();

	QString axesString;

	QList<AMGCS2::Axis> axesToQuery;
	axesToQuery << AMGCS2::XAxis << AMGCS2::YAxis << AMGCS2::ZAxis << AMGCS2::UAxis
				   << AMGCS2::VAxis << AMGCS2::WAxis;

	AMCArrayHandler<int> valuesHandler(axesToQuery.count());

	foreach (AMGCS2::Axis axis, axesToQuery) {
		axesString.append(QString(" %1").arg(AMGCS2Support::axisToCharacter(axis)));
	}

	bool success = PI_IsMoving(controller_->id(), axesString.toStdString().c_str(), valuesHandler.cArray());


	if(success) {
		for(int iAxis = 0, axisCount = axesToQuery.count();
			iAxis < axisCount;
			++iAxis) {

			bool currentAxisIsMoving = (valuesHandler.cArray()[iAxis] == 1);

			if(currentAxisIsMoving) {
				switch (axesToQuery.at(iAxis)) {
				case AMGCS2::XAxis:
					movementStatuses_ |= AMGCS2::XAxisIsMoving;
					break;
				case AMGCS2::YAxis:
					movementStatuses_ |= AMGCS2::YAxisIsMoving;
					break;
				case AMGCS2::ZAxis:
					movementStatuses_ |= AMGCS2::ZAxisIsMoving;
					break;
				case AMGCS2::UAxis:
					movementStatuses_ |= AMGCS2::UAxisIsMoving;
					break;
				case AMGCS2::VAxis:
					movementStatuses_ |= AMGCS2::VAxisIsMoving;
					break;
				case AMGCS2::WAxis:
					movementStatuses_ |= AMGCS2::WAxisIsMoving;
					break;
				case AMGCS2::UnknownAxis:
					break;
				}
			}
		}

	} else {
		movementStatuses_ = QFlags<AMGCS2::AxisMovementStatus>(AMGCS2::UnknownAxisMovementStatus);
		lastError_ = controllerErrorMessage();
	}

	return success;
}

QString AMGCS2GetMovingStatusCommand::outputString() const
{
	if(movementStatuses_.testFlag(AMGCS2::UnknownAxisMovementStatus)) {
		return "Axis movement status not known.";
	}

	QString movementStatusesText;
	bool anyMovement = false;

	if(movementStatuses_.testFlag(AMGCS2::XAxisIsMoving)) {
		anyMovement = true;
		movementStatusesText.append("X is moving\n");
	} else {
		movementStatusesText.append("X is not moving\n");
	}

	if(movementStatuses_.testFlag(AMGCS2::YAxisIsMoving)) {
		anyMovement = true;
		movementStatusesText.append("Y is moving\n");
	} else {
		movementStatusesText.append("Y is not moving\n");
	}

	if(movementStatuses_.testFlag(AMGCS2::ZAxisIsMoving)) {
		anyMovement = true;
		movementStatusesText.append("Z is moving\n");
	} else {
		movementStatusesText.append("Z is not moving\n");
	}

	if(movementStatuses_.testFlag(AMGCS2::UAxisIsMoving)) {
		anyMovement = true;
		movementStatusesText.append("U is moving\n");
	} else {
		movementStatusesText.append("U is not moving\n");
	}

	if(movementStatuses_.testFlag(AMGCS2::VAxisIsMoving)) {
		anyMovement = true;
		movementStatusesText.append("V is moving\n");
	} else {
		movementStatusesText.append("V is not moving\n");
	}

	if(movementStatuses_.testFlag(AMGCS2::WAxisIsMoving)) {
		anyMovement = true;
		movementStatusesText.append("W is moving\n");
	} else {
		movementStatusesText.append("W is not moving\n");
	}

	if(anyMovement)	{
		return movementStatusesText;
	} else {
		return "No axes movement";
	}
}


