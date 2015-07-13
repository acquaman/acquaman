#include "AMGCS2GetLowSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetLowSoftLimitsCommand::AMGCS2GetLowSoftLimitsCommand(const AMPIC887AxisCollection& axesToQuery )
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<double> AMGCS2GetLowSoftLimitsCommand::axesLowSoftLimits() const
{
	return axesLowSoftLimits_;
}

QString AMGCS2GetLowSoftLimitsCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString outputString("Axis\t\tLow Limit\n");
	QList<AMGCS2::Axis> axesResults = axesLowSoftLimits_.keys();

	for(int iAxis = 0, axisCount = axesResults.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axesResults.at(iAxis);
		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axesLowSoftLimits_.value(currentAxis)));
	}

	return outputString.trimmed();
}

bool AMGCS2GetLowSoftLimitsCommand::validateArguments()
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

bool AMGCS2GetLowSoftLimitsCommand::runImplementation()
{
	// Clear previous results
	axesLowSoftLimits_.clear();

	AMCArrayHandler<double> lowSoftLimitValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qNLM(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  lowSoftLimitValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axesLowSoftLimits_.insert(axesToQuery_.at(iAxis),
								  lowSoftLimitValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
