#include "AMGCS2GetMaxCommandablePositionCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetMaxCommandablePositionCommand::AMGCS2GetMaxCommandablePositionCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

QString AMGCS2GetMaxCommandablePositionCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString returnString("Axis\t\tMax Commandable Position\n");

	foreach(AMGCS2::Axis currentAxis, maxCommandablePositions_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(maxCommandablePositions_.value(currentAxis)));
	}

	return returnString.trimmed();
}

AMPIC887AxisMap<double> AMGCS2GetMaxCommandablePositionCommand::maxCommandablePositions() const
{
	return maxCommandablePositions_;
}

bool AMGCS2GetMaxCommandablePositionCommand::validateArguments()
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

bool AMGCS2GetMaxCommandablePositionCommand::runImplementation()
{
	// Clear previous results
	maxCommandablePositions_.clear();

	AMCArrayHandler<double> maxPositionValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qTMX(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  maxPositionValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			maxCommandablePositions_.insert(axesToQuery_.at(iAxis),
								  maxPositionValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
