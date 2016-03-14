#include "AMGCS2VirtualMoveCommand.h"
#include "util/AMCArrayHandler.h"
#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2VirtualMoveCommand::AMGCS2VirtualMoveCommand(const AMPIC887AxisMap<double>& axisPositions)
{
	axisPositions_ = axisPositions;
	isMoveSafe_ = false;
}

bool AMGCS2VirtualMoveCommand::isMoveSafe() const
{
	return isMoveSafe_;
}

bool AMGCS2VirtualMoveCommand::validateArguments()
{
	if(axisPositions_.isEmpty()) {
		lastError_ = "No axis positions provided";
		return false;
	}

	if(axisPositions_.containsUnknownAxis()) {
		lastError_ = "Unknown axis provided";
		return false;
	}

	return true;
}

bool AMGCS2VirtualMoveCommand::runImplementation()
{
	AMPIC887AxisCollection axes = axisPositions_.axes();
	int axisCount = axes.count();
	QString axisString = axes.toString();
	AMCArrayHandler<double> positionValuesHandler(axisCount);

	for(int iAxis = 0; iAxis < axisCount; ++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		positionValuesHandler.cArray()[iAxis] = axisPositions_.value(currentAxis);
	}

	int moveSafe = 0;

	bool success = PI_qVMO(controller_->id(),
						   axisString.toStdString().c_str(),
						   positionValuesHandler.cArray(),
						   &moveSafe);

	if(success) {
		isMoveSafe_ = (moveSafe == 1);
	} else {
		isMoveSafe_ = false;
		lastError_ = controllerErrorMessage();
	}

	return success;
}
