#include "AMGCS2GetLimitSwitchStatusCommand.h"

#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetLimitSwitchStatusCommand::AMGCS2GetLimitSwitchStatusCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<bool> AMGCS2GetLimitSwitchStatusCommand::limitSwitchStatuses() const
{
	return limitSwitchStatuses_;
}

bool AMGCS2GetLimitSwitchStatusCommand::validateArguments()
{
	if(axesToQuery_.isEmpty()) {
		lastError_ = "No axes to query";
		return false;
	}

	AMPIC887AxisCollection::ValidState validState = axesToQuery_.validate();

	if(validState == AMPIC887AxisCollection::ContainsUnknownAxis) {
		lastError_ = "Unknown axis";
		return false;
	}

	if(validState == AMPIC887AxisCollection::ContainsDuplicateAxes) {
		lastError_ = "Duplicate axes";
		return false;
	}

	return true;
}

bool AMGCS2GetLimitSwitchStatusCommand::runImplementation()
{
	// Clear previous results
	limitSwitchStatuses_.clear();

	AMCArrayHandler<int> SoftLimitStatusesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qLIM(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  SoftLimitStatusesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			limitSwitchStatuses_.insert(axesToQuery_.at(iAxis),
								  SoftLimitStatusesHandler.cArray()[iAxis] == 1);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
