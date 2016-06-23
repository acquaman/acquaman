#include "AMGCS2SetPivotPointCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2SetPivotPointCommand::AMGCS2SetPivotPointCommand(const AMPIC887AxisMap<double>& pivotPoints)
{
	pivotPoints_ = pivotPoints;
}

bool AMGCS2SetPivotPointCommand::validateArguments()
{
	if(pivotPoints_.isEmpty()) {
		lastError_ = "No high soft limits to set";
		return false;
	}

	if(pivotPoints_.containsUnknownAxis()) {
		lastError_ = "Contains unknown axis";
		return false;
	}

	if(pivotPoints_.axes().containsRotationalAxes()) {
		lastError_ = "Contains rotational axes";
		return false;
	}

	return true;
}

bool AMGCS2SetPivotPointCommand::runImplementation()
{
	AMPIC887AxisCollection axes = pivotPoints_.axes();
	int axisCount = axes.count();

	AMCArrayHandler<double> pivotPointValuesHandler(axisCount);
	QString axesString = axes.toString();

	for(int iAxis = 0;
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axes.at(iAxis);

		pivotPointValuesHandler.cArray()[iAxis]
				= pivotPoints_.value(currentAxis);
	}

	bool success = PI_SPI(controller_->id(),
						  axesString.toStdString().c_str(),
						  pivotPointValuesHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
