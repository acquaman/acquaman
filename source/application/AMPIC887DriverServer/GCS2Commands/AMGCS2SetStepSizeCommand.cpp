#include "AMGCS2SetStepSizeCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2SetStepSizeCommand::AMGCS2SetStepSizeCommand(const AMPIC887AxisMap<double>& axisStepSizes)
{
	axisStepSizes_ = axisStepSizes;
}

bool AMGCS2SetStepSizeCommand::validateArguments()
{
	if(axisStepSizes_.isEmpty()) {
		lastError_ = "No step sizes to set";
		return false;
	}

	if(axisStepSizes_.containsUnknownAxis()) {
		lastError_ = "Contains unknown axis";
		return false;
	}

	return true;
}

bool AMGCS2SetStepSizeCommand::runImplementation()
{
	AMPIC887AxisCollection axes = axisStepSizes_.axes();
	int axisCount = axes.count();

	AMCArrayHandler<double> stepSizeValueHandler(axisCount);
	QString axesString = axes.toString();

	for(int iAxis = 0;
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);

		stepSizeValueHandler.cArray()[iAxis] = axisStepSizes_.value(currentAxis);
	}

	bool success = PI_SST(controller_->id(),
						  axesString.toStdString().c_str(),
						  stepSizeValueHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
