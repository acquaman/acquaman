#include "AMGCS2GetStepSizeCommand.h"

#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetStepSizeCommand::AMGCS2GetStepSizeCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<double> AMGCS2GetStepSizeCommand::axisStepSizes() const
{
	return axisStepSizes_;
}

bool AMGCS2GetStepSizeCommand::validateArguments()
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

bool AMGCS2GetStepSizeCommand::runImplementation()
{
	// Clear previous results
	axisStepSizes_.clear();

	AMCArrayHandler<double> stepSizeValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qSST(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  stepSizeValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axisStepSizes_.insert(axesToQuery_.at(iAxis),
								  stepSizeValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
