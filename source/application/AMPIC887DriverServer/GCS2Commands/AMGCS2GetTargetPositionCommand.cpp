#include "AMGCS2GetTargetPositionCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetTargetPositionCommand::AMGCS2GetTargetPositionCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

QString AMGCS2GetTargetPositionCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString returnString("Axis\t\tTarget Position\n");

	foreach(AMGCS2::Axis currentAxis, axisTargetPositions_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axisTargetPositions_.value(currentAxis)));
	}

	return returnString.trimmed();
}

AMPIC887AxisMap<double> AMGCS2GetTargetPositionCommand::axisTargetPositions() const
{
	return axisTargetPositions_;
}

bool AMGCS2GetTargetPositionCommand::validateArguments()
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

bool AMGCS2GetTargetPositionCommand::runImplementation()
{
	// Clear previous results
	axisTargetPositions_.clear();

	AMCArrayHandler<double> targetPositionValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qMOV(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  targetPositionValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axisTargetPositions_.insert(axesToQuery_.at(iAxis),
								  targetPositionValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
