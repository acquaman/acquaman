#include "AMGCS2ReferenceMoveCommand.h"

#include "../AMGCS2Support.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2ReferenceMoveCommand::AMGCS2ReferenceMoveCommand(const AMPIC887AxisCollection& axesToReference)
{
	axesToReference_ = axesToReference;
}

bool AMGCS2ReferenceMoveCommand::validateArguments()
{
	if(axesToReference_.isEmpty()) {
		lastError_ = "No axes to reference";
		return false;
	}

	AMPIC887AxisCollection::ValidState validState = axesToReference_.validate();

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

bool AMGCS2ReferenceMoveCommand::runImplementation()
{
	QString axesString = axesToReference_.toString();

	bool success = PI_FRF(controller_->id(), axesString.toStdString().c_str());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}



