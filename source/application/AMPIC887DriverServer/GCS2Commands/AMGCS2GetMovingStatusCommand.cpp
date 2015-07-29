#include "AMGCS2GetMovingStatusCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetMovingStatusCommand::AMGCS2GetMovingStatusCommand()
{	
}

AMGCS2::AxisMovementStatuses AMGCS2GetMovingStatusCommand::movementStatuses()
{
	return movementStatuses_;
}

bool AMGCS2GetMovingStatusCommand::runImplementation()
{
	// Clear any previous results.
	movementStatuses_ = 0;

	AMPIC887AxisCollection axesToQuery = AMPIC887AxisCollection(AMPIC887AxisCollection::AllAxes);
	if(axesToQuery.count() == 0) {
		return false;
	}

	QString axesString = axesToQuery.toString();
	AMCArrayHandler<int> valuesHandler(axesToQuery.count());

	bool success = PI_IsMoving(controller_->id(),
							   axesString.toStdString().c_str(),
							   valuesHandler.cArray());


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
		movementStatuses_ = AMGCS2::UnknownAxisMovementStatus;
		lastError_ = controllerErrorMessage();
	}

	return success;
}
