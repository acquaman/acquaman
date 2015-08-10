#include "AMGCS2GetOnTargetStateCommand.h"

#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2GetOnTargetStateCommand::AMGCS2GetOnTargetStateCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<bool> AMGCS2GetOnTargetStateCommand::onTargetStates() const
{
	return onTargetStates_;
}

bool AMGCS2GetOnTargetStateCommand::validateArguments()
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

bool AMGCS2GetOnTargetStateCommand::runImplementation()
{
	onTargetStates_.clear();

	QString axisString = axesToQuery_.toString();
	int axisCount = axesToQuery_.count();

	AMCArrayHandler<int> onTargetValuesHandler(axisCount);

	bool success = PI_qONT(controller_->id(),
						   axisString.toStdString().c_str(),
						   onTargetValuesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		for (int iAxis = 0; iAxis < axisCount; ++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

			onTargetStates_.insert(currentAxis,
								   (onTargetValuesHandler.cArray()[iAxis] == 1));
		}
	}

	return success;
}
