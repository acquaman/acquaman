#include "AMGCS2GetMinCommandablePositionCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2GetMinCommandablePositionCommand::AMGCS2GetMinCommandablePositionCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<double> AMGCS2GetMinCommandablePositionCommand::minCommandablePositions() const
{
	return minCommandablePositions_;
}

bool AMGCS2GetMinCommandablePositionCommand::validateArguments()
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

bool AMGCS2GetMinCommandablePositionCommand::runImplementation()
{
	// Clear previous results
	minCommandablePositions_.clear();

	AMCArrayHandler<double> minPositionValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qTMN(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  minPositionValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			minCommandablePositions_.insert(axesToQuery_.at(iAxis),
								  minPositionValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
