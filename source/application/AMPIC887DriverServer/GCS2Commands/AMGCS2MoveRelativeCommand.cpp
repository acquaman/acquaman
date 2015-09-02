#include "AMGCS2MoveRelativeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2MoveRelativeCommand::AMGCS2MoveRelativeCommand(const AMPIC887AxisMap<double>& relativeAxisPositions)
{
	relativeAxisPositions_ = relativeAxisPositions;
}

bool AMGCS2MoveRelativeCommand::validateArguments()
{
	if(relativeAxisPositions_.isEmpty()) {
		lastError_ = "No relative positions provided";
		return false;
	}

	if(relativeAxisPositions_.containsUnknownAxis()) {
		lastError_ = "Unknown axis provided";
		return false;
	}

	return true;
}

bool AMGCS2MoveRelativeCommand::runImplementation()
{
	AMPIC887AxisCollection axes = relativeAxisPositions_.axes();
	int axisCount = axes.count();

	QString axesString = axes.toString();
	AMCArrayHandler<double> positionValuesHandler(axisCount);

	for(int iAxis = 0; iAxis < axisCount; ++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		positionValuesHandler.cArray()[iAxis] = relativeAxisPositions_.value(currentAxis);
	}

	bool success = PI_MVR(controller_->id(),
						  axesString.toStdString().c_str(),
						  positionValuesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
