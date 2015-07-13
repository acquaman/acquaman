#include "AMGCS2GetCurrentPositionCommand.h"

#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
#include "../AMGCS2Support.h"
AMGCS2GetCurrentPositionCommand::AMGCS2GetCurrentPositionCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<double> AMGCS2GetCurrentPositionCommand::axisPositions()
{
	return axisPositions_;
}

QString AMGCS2GetCurrentPositionCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return QString();
	}

	QString returnString;

	foreach (AMGCS2::Axis axis, axisPositions_.keys()) {
		returnString.append(QString("%1 : %2")
							.arg(AMGCS2Support::axisToCharacter(axis))
							.arg(axisPositions_.value(axis)));
		returnString.append("\n");
	}

	return returnString.trimmed();
}

bool AMGCS2GetCurrentPositionCommand::validateArguments()
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

bool AMGCS2GetCurrentPositionCommand::runImplementation()
{
	// Clear previous results
	axisPositions_.clear();

	AMCArrayHandler<double> positionValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qPOS(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  positionValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axisPositions_.insert(axesToQuery_.at(iAxis),
								  positionValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}


