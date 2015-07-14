#include "AMGCS2SetSoftLimitsStatusCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2SetSoftLimitsStatusCommand::AMGCS2SetSoftLimitsStatusCommand(const AMPIC887AxisMap<bool>& softLimitStatuses)
{
	softLimitStatuses_ = softLimitStatuses;
}

bool AMGCS2SetSoftLimitsStatusCommand::validateArguments()
{
	if(softLimitStatuses_.isEmpty()) {
		lastError_ = "No soft limit states to set";
		return false;
	}

	if(softLimitStatuses_.containsUnknownAxis()) {
		lastError_ = "Contains unknown axis";
		return false;
	}

	return true;
}

bool AMGCS2SetSoftLimitsStatusCommand::runImplementation()
{
	AMPIC887AxisCollection axes = softLimitStatuses_.axes();
	int axisCount = axes.count();


	AMCArrayHandler<int> softLimitStatusHandler(axes.count());
	QString axesString = axes.toString();


	for(int iAxis = 0;
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);

		bool activateLimit = softLimitStatuses_.value(currentAxis);

		if(activateLimit) {
			softLimitStatusHandler.cArray()[iAxis] = 1;
		} else {
			softLimitStatusHandler.cArray()[iAxis] = 0;
		}
	}

	bool success = PI_SSL(controller_->id(),
						  axesString.toStdString().c_str(),
						  softLimitStatusHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
