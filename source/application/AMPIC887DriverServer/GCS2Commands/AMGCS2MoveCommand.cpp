#include "AMGCS2MoveCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "../AMPIC887Controller.h"
#include "PI_GCS2_DLL.h"

AMGCS2MoveCommand::AMGCS2MoveCommand(const AMPIC887AxisMap<double>& targetPositions)
{
	targetPositions_ = targetPositions;
}

bool AMGCS2MoveCommand::validateArguments()
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

bool AMGCS2MoveCommand::runImplementation()
{
	AMPIC887AxisCollection axes = targetPositions_.axes();
	int axisCount = axes.count();

	AMCArrayHandler<double> positionValuesHandler(axisCount);
	QString axesString = axes.toString();

	for(int iAxis = 0; iAxis < axisCount; ++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		positionValuesHandler.cArray()[iAxis] = targetPositions_.value(currentAxis);
	}

	// Ensure move is possible using VMO
	int movePossible = 0;
	bool success = PI_qVMO(controller_->id(),
						   axesString.toStdString().c_str(),
						   positionValuesHandler.cArray(),
						   &movePossible);

	if(!success) {
		lastError_ = "Could not determine if attempted move is possible";
		return false;
	}

	if(movePossible == 0) {
		lastError_ = "Not safe to perform specified move";
		return false;
	}

	success = PI_MOV(controller_->id(),
					 axesString.toStdString().c_str(),
					 positionValuesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
