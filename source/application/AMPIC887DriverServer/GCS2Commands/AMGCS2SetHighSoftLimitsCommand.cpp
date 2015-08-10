#include "AMGCS2SetHighSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2SetHighSoftLimitsCommand::AMGCS2SetHighSoftLimitsCommand(const AMPIC887AxisMap<double>& axisHighLimits)
{
	axisHighLimits_ = axisHighLimits;
}

bool AMGCS2SetHighSoftLimitsCommand::validateArguments()
{
	if(axisHighLimits_.isEmpty()) {
		lastError_ = "No high soft limits to set";
		return false;
	}

	if(axisHighLimits_.containsUnknownAxis()) {
		lastError_ = "Contains unknown axis";
		return false;
	}

	return true;
}

bool AMGCS2SetHighSoftLimitsCommand::runImplementation()
{
	AMPIC887AxisCollection axes = axisHighLimits_.axes();
	int axesCount = axes.count();
	AMCArrayHandler<double> highLimitValuesHandler(axesCount);

	QString axesString = axes.toString();

	for(int iAxis = 0;
		iAxis < axesCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		highLimitValuesHandler.cArray()[iAxis] = axisHighLimits_.value(currentAxis);
	}

	bool success = PI_PLM(controller_->id(),
						  axesString.toStdString().c_str(),
						  highLimitValuesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}


