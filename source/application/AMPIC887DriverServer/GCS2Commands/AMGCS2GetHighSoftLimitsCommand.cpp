#include "AMGCS2GetHighSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetHighSoftLimitsCommand::AMGCS2GetHighSoftLimitsCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<double> AMGCS2GetHighSoftLimitsCommand::axesHighSoftLimits() const
{
	return axesHighSoftLimits_;
}

bool AMGCS2GetHighSoftLimitsCommand::validateArguments()
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

bool AMGCS2GetHighSoftLimitsCommand::runImplementation()
{
	// Clear previous results
	axesHighSoftLimits_.clear();

	AMCArrayHandler<double> highSoftLimitValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qPLM(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  highSoftLimitValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axesHighSoftLimits_.insert(axesToQuery_.at(iAxis),
								  highSoftLimitValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
