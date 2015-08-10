#include "AMGCS2SetLowSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
#include "../AMPIC887AxisCollection.h"
AMGCS2SetLowSoftLimitsCommand::AMGCS2SetLowSoftLimitsCommand(const AMPIC887AxisMap<double>& axisLowLimits)
{
	axisLowLimits_ = axisLowLimits;
}

bool AMGCS2SetLowSoftLimitsCommand::validateArguments()
{
	if(axisLowLimits_.isEmpty()) {
		lastError_ = "No low soft limits to set";
		return false;
	}

	if(axisLowLimits_.containsUnknownAxis()) {
		lastError_ = "Contains unknown axis";
		return false;
	}

	return true;
}

bool AMGCS2SetLowSoftLimitsCommand::runImplementation()
{
	AMPIC887AxisCollection axes = axisLowLimits_.axes();
	int axesCount = axes.count();

	AMCArrayHandler<double> lowLimitValues(axesCount);
	QString axesString = axes.toString();

	for(int iAxis = 0;
		iAxis < axesCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);
		lowLimitValues.cArray()[iAxis] = axisLowLimits_.value(currentAxis);
	}

	bool success = PI_NLM(controller_->id(),
						  axesString.toStdString().c_str(),
						  lowLimitValues.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
