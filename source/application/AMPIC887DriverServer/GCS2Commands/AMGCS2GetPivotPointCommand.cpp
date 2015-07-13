#include "AMGCS2GetPivotPointCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetPivotPointCommand::AMGCS2GetPivotPointCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

QString AMGCS2GetPivotPointCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}


	QString outputString("Axis\t\tPivot Point\n");

	foreach(AMGCS2::Axis currentAxis, axisPivotPoints_.keys()) {

		double pivotPointValue = axisPivotPoints_.value(currentAxis);
		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(pivotPointValue));
	}

	return outputString.trimmed();
}

AMPIC887AxisMap<double> AMGCS2GetPivotPointCommand::axisPivotPoints() const
{
	return axisPivotPoints_;
}

bool AMGCS2GetPivotPointCommand::validateArguments()
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

bool AMGCS2GetPivotPointCommand::runImplementation()
{
	// Clear previous results
	axisPivotPoints_.clear();

	AMCArrayHandler<double> pivotPointValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qSPI(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  pivotPointValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axisPivotPoints_.insert(axesToQuery_.at(iAxis),
								  pivotPointValuesHandler.cArray()[iAxis]);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
