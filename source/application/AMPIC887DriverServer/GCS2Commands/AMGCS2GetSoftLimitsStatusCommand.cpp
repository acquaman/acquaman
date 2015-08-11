#include "AMGCS2GetSoftLimitsStatusCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2GetSoftLimitsStatusCommand::AMGCS2GetSoftLimitsStatusCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<bool> AMGCS2GetSoftLimitsStatusCommand::softLimitStatuses() const
{
	return softLimitStatuses_;
}

bool AMGCS2GetSoftLimitsStatusCommand::validateArguments()
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

bool AMGCS2GetSoftLimitsStatusCommand::runImplementation()
{
	softLimitStatuses_.clear();
	int axisCount = axesToQuery_.count();

	QString axisString = axesToQuery_.toString();
	AMCArrayHandler<int> softLimitStatusHandler(axisCount);

	bool success = PI_qSSL(controller_->id(),
						   axisString.toStdString().c_str(),
						   softLimitStatusHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		for(int iAxis = 0; iAxis < axisCount; ++iAxis) {


			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);
			int rawLimitStatusValue = softLimitStatusHandler.cArray()[iAxis];

			softLimitStatuses_.insert(currentAxis,
									   rawLimitStatusValue != 0);
		}

	}

	return success;
}
